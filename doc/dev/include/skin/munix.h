#ifndef H_LAND_MUNIX_H
#define H_LAND_MUNIX_H

/**
 * @file munix.h
 * @brief mmap + AF_UNIX SOCK_DGRAM IPC (payload in shared maps, signaling on unix)
 *
 * Handle is opaque munix_t (listen/connect return it; close with munix_close).
 * Use munix_fd(mx) for poll/select/libevent. One endpoint = one serialized
 * request/reply channel.
 *
 * Naming (skinos style): munix_t / munix_slot_t / munix_client_t are pointers (*_t).
 * munix_st / munix_slot_st are opaque. munix_client_st is public caller storage
 * (recvfrom):
 *   munix_client_st peer;
 *   sunix_take(mx, &slot, &peer);   // fills addr/addrlen/corr; preserves data
 *   sunix_post(mx, key, slot, &peer, flags);  // echoes peer.corr on the reply
 *
 * Client: munix_connect → munix_slot_alloc / munix_post / munix_take
 * Server: munix_listen  → sunix_take / sunix_slot_alloc / sunix_post
 *
 * Wire corr (uint64): client post assigns a monotonic id; while a reply is
 * pending, take delivers only that corr (mismatched POSTs discarded, out slots
 * returned). Idle take (no pending post) delivers any valid POST.
 * GRANT_REQ/GRANT use the same corr field for alloc wait matching.
 * One connect endpoint = one in-flight RPC (alloc→post→take). Alloc abandons
 * any prior unread reply (clears reply_pending; mmap-hit does not drain the fd).
 * post without take → EBUSY; parallel RPCs → more connects.
 * Alloc tries the mmap freelist first; mmap-hit clears any pending GRANT wait.
 * Same-len pending GRANT skips mmap and only drains/claims. On miss may
 * GRANT_REQ + drain. Non-block: EAGAIN until GRANT (caller times out / reconnects).
 * Event loop: POLLIN while waiting for GRANT must retry munix_slot_alloc, not take
 * (take returns unused GRANTs to the pool; matching corr clears grant_pending_req).
 *
 * key: required NUL-terminated C string (may be ""; soft max in impl).
 * post slot: may be NULL for key-only (KEEP + NULL → EINVAL).
 * take slot: out-pointer required; *slot may be NULL (key-only).
 * After server restart: take/alloc/post fail with errno=ESTALE; munix_close and reconnect.
 * Event loops: poll/select/libevent on munix_fd(mx); use timeout_ms=0.
 *
 * Lifetime: always munix_close(mx, 0) — never close(munix_fd)/dup() into these APIs.
 * Prefer munix_close all endpoints before fork (free slots first). After fork
 * without prior close: munix_close(mx, 1) skips unix path unlink (parent listen
 * may still own the path); plain-close fds only if you must avoid free/mmap too.
 * Cookie: munix_set_data / get_data (mx), munix_slot_set_data / get_data,
 * munix_client_set_data / get_data. Munix never frees cookies.
 * Free slots before munix_close (slot free-after-close is UAF).
 *
 * KEEP + death / GRANT age: see skin.md §10.9.
 * Client: one in-flight RPC per endpoint (post while reply pending → EBUSY).
 * Server: sunix_take never blocks — poll first, then take until EAGAIN
 * (same as non-blocking recvfrom). GRANT waiters wake on that EAGAIN.
 * One endpoint is not thread-safe. Multi-endpoint × one thread each is OK
 * (map lock word carries unix fd so siblings wait without a global plock). Parallel RPCs → multiple connects.
 * listen pool: slots 1..2048 (larger requests clamped to 2048); heap >= 64
 * and map size must fit uint32_t.
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct munix_st *munix_t; /* opaque; munix_close */
typedef struct munix_slot_st *munix_slot_t; /* opaque; munix_slot_free */

/** peer storage for sunix_take / sunix_post (caller-owned, recvfrom style) */
typedef struct munix_client_struct
{
	struct sockaddr_un addr;
	socklen_t addrlen;
	void *data; /* caller cookie; sunix_take does not clear; munix never frees */
	uint64_t corr; /* request corr from last sunix_take; sunix_post echoes it */
} munix_client_st;
typedef munix_client_st *munix_client_t;

#define MUNIX_POST_KEEP  0x1  /* post: peer must not pool-free; sender keeps handle */
#define MUNIX_MMAP_ONLY  0x2  /* alloc: mmap only; never GRANT_REQ / drain fd */

/* endpoint — returns munix_t or NULL */
munix_t      munix_listen( const char *name,
	int in_slots, size_t in_heap,
	int out_slots, size_t out_heap );
munix_t      munix_connect( const char *name );
/** tear down endpoint; NULL-safe; never use close(fd)/dup(fd) instead.
 * dont_unlink: 0 = unlink unix path (normal); 1 = keep path (e.g. fork child). */
void         munix_close( munix_t mx, int dont_unlink );
/** unix socket fd for poll/select/libevent; NULL → -1 */
int          munix_fd( munix_t mx );

/**
 * @brief attach / fetch caller cookie on endpoint
 * munix never frees data; clear or replace before munix_close as needed.
 */
void         munix_set_data( munix_t mx, void *data );
void        *munix_get_data( munix_t mx );

/**
 * @brief attach / fetch caller cookie on munix_client_st
 * sunix_take does not clear; munix never frees.
 */
void         munix_client_set_data( munix_client_t client, void *data );
void        *munix_client_get_data( munix_client_t client );

/* slot */
/** @brief mmap payload pointer (not the set_data/get_data cookie) */
void        *munix_slot_data( munix_slot_t slot );
size_t       munix_slot_cap( munix_slot_t slot );
void         munix_slot_free( munix_slot_t slot );
/**
 * @brief caller cookie on a slot handle (not the mmap payload from munix_slot_data)
 * munix never frees data; NULL when the handle is created.
 */
void         munix_slot_set_data( munix_slot_t slot, void *data );
void        *munix_slot_get_data( munix_slot_t slot );

/**
 * @brief client alloc from inbound (request) map
 * @param len must be > 0 (len==0 or overflow-prone size → NULL/EINVAL); no-payload traffic uses slot=NULL on post
 * @param timeout_ms 0=nonblock, >0=wait ms, <0=forever (ignored if MUNIX_MMAP_ONLY)
 * @param flags 0=may GRANT via unix when mmap empty; MUNIX_MMAP_ONLY=mmap only
 *        (never GRANT_REQ/drain/wait; mmap-hit still clears a pending GRANT wait)
 */
munix_slot_t munix_slot_alloc( munix_t mx, size_t len, int timeout_ms, int flags );

/**
 * @brief server reply alloc from outbound map; full → NULL/EAGAIN (no wait)
 * @param len must be > 0 (len==0 → NULL/EINVAL); no-payload traffic uses slot=NULL on post
 */
munix_slot_t sunix_slot_alloc( munix_t mx, size_t len );

/* client */
/**
 * @brief client post request (non-blocking sendto-style)
 * @param key required (may be "")
 * @param slot may be NULL for key-only; KEEP + NULL → EINVAL; non-KEEP success consumes handle
 * @param flags 0 or MUNIX_POST_KEEP
 * @return 0 on success; -1 on fail — errno=EAGAIN if would block (retry when writable);
 *         EBUSY if a reply is still pending; ESTALE if session expired (close + reconnect)
 */
int          munix_post( munix_t mx, const char *key, munix_slot_t slot, int flags );

/**
 * @brief client take one reply
 * @return key (internal until next successful take); NULL on fail (EAGAIN/ETIMEDOUT/ESTALE/…)
 * @param slot out-pointer required (NULL → EINVAL); *slot may be NULL if key-only
 * @param timeout_ms 0=nonblock, >0=wait ms, <0=forever
 * Delivers the last post's matching corr while a reply is pending; idle take
 * (reply_pending clear) returns any valid POST. GRANT datagrams are returned to the
 * pool (not claimed). Matching corr clears grant_pending_req so a later alloc
 * may send a fresh GRANT_REQ; retry munix_slot_alloc to claim.
 * Corrupt datagrams and mismatched reply corr (while pending) are skipped
 * (out slots returned); EPROTO is for slot/state breaks on a valid matching POST.
 */
const char  *munix_take( munix_t mx, munix_slot_t *slot, int timeout_ms );

/* server */
/**
 * @brief server take one request (always non-blocking)
 * @return key (internal until next successful sunix_take); NULL on fail (EAGAIN if none)
 * @param slot out-pointer required (NULL → EINVAL); *slot may be NULL if key-only
 * @param client required (pointer to caller munix_client_st); fills addr/addrlen/corr (preserves data)
 * Corrupt datagrams are skipped (same as client take).
 * After POLLIN, loop until EAGAIN (same as recvfrom). GRANT waiters wake on that EAGAIN.
 */
const char  *sunix_take( munix_t mx, munix_slot_t *slot, munix_client_t client );

/**
 * @brief server post reply (non-blocking sendto-style)
 * @param key required (may be "")
 * @param slot may be NULL for key-only; KEEP + NULL → EINVAL; non-KEEP success consumes handle
 * @param client from sunix_take (required; pointer to caller storage / copy); echoes client->corr
 * @param flags 0 or MUNIX_POST_KEEP
 * @return 0 on success; -1 on fail — errno=EAGAIN if would block (retry when writable)
 */
int          sunix_post( munix_t mx, const char *key, munix_slot_t slot,
	munix_client_t client, int flags );

#ifdef __cplusplus
}
#endif

#endif /* H_LAND_MUNIX_H */
