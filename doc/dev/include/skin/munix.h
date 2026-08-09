#ifndef H_LAND_MUNIX_H
#define H_LAND_MUNIX_H

/**
 * @file munix.h
 * @brief mmap + AF_UNIX SOCK_DGRAM IPC (payload in shared maps, signaling on unix)
 *
 * Client: munix_connect → munix_slot_alloc / munix_post / munix_take
 * Server: munix_listen  → sunix_talk / sunix_slot_alloc / sunix_post
 * key is a NUL-terminated C string on the wire (soft max inside implementation).
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct munix_st       *munix_t;
typedef struct munix_slot_st  *munix_slot_t;
typedef struct munix_reply_st *munix_reply_t;

#define MUNIX_POST_KEEP  0x1  /* peer must not pool-free; sender keeps handle */

/* endpoint */
munix_t      munix_listen( const char *name,
	int in_slots, size_t in_heap,
	int out_slots, size_t out_heap );
munix_t      munix_connect( const char *name );
void         munix_close( munix_t mx );
int          munix_fd( munix_t mx );

/* slot */
void        *munix_slot_data( munix_slot_t slot );
size_t       munix_slot_len( munix_slot_t slot );
void         munix_slot_free( munix_slot_t slot );

/**
 * @brief alloc from outbound map (client: in, server: out)
 * @param timeout_ms 0=nonblock, >0=wait ms, <0=forever
 */
munix_slot_t munix_slot_alloc( munix_t mx, size_t len, int timeout_ms );

/** @brief server reply alloc; full → NULL/EAGAIN (no wait) */
munix_slot_t sunix_slot_alloc( munix_t mx, size_t len );

/* client */
/**
 * @brief client post
 * @param key NUL-terminated, may be NULL
 * @param flags 0 or MUNIX_POST_KEEP (KEEP: handle remains valid)
 */
int          munix_post( munix_t mx, const char *key, munix_slot_t slot, int flags );

/**
 * @brief client take one reply
 * @return key (internal buf until next take; "" if none); NULL on fail
 * @param timeout_ms 0=nonblock, >0=wait ms, <0=forever
 * @param slot out: payload handle or NULL if key-only
 */
const char  *munix_take( munix_t mx, munix_slot_t *slot, int timeout_ms );

/* server */
/**
 * @brief server take one request (non-blocking)
 * @return key (internal buf until next sunix_talk; "" if none); NULL on fail
 * @param slot out: request payload or NULL if key-only
 * @param reply out: address for sunix_post (required)
 */
const char  *sunix_talk( munix_t mx, munix_slot_t *slot, munix_reply_t *reply );

/**
 * @brief server post reply
 * @param key NUL-terminated, may be NULL
 * @param to from sunix_talk (required)
 */
int          sunix_post( munix_t mx, const char *key, munix_slot_t slot, munix_reply_t to );

#ifdef __cplusplus
}
#endif

#endif /* H_LAND_MUNIX_H */
