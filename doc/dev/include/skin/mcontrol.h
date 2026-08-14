#ifndef H_LAND_MCONTROL_H
#define H_LAND_MCONTROL_H

/**
 * @file mcontrol.h
 * @brief thin RPC/control layer over munix + libevent + optional mxtalk
 *
 * Server: mcontrol_listen → control callback
 *   sync key only: return "ttrue" (leave *out NULL)
 *   sync body: *out = p from salloc* / mxtalk; return key
 *              *out MUST be a registered salloc* pointer (else body dropped)
 *   async: mcontrol_bind(p, client); return NULL (do not set *out);
 *          later mcontrol_reply(key, p)
 *   drop: return NULL without bind (often no salloc at all)
 *   return NULL alone does not mean async — only bind does
 *   on_read drains sunix_take until EAGAIN (same as recvfrom)
 *
 * Client: mcontrol_connect → alloc* → mcontrol_call → mcontrol_close
 *   one in-flight RPC per connect fd (post+take). Take timeout does not
 *   clear reply_pending → next call EBUSY unless alloc (abandon) or close.
 *   Recommended recovery (business wrapper): on mcontrol_call failure with
 *   errno EBUSY or ETIMEDOUT (also EAGAIN on timeout_ms==0): free any req,
 *   then either (1) mcontrol_alloc(fd, …) and mcontrol_free it to abandon
 *   the pending round, or (2) mcontrol_close + mcontrol_connect; then retry
 *   the RPC with backoff. Do not spin post/call without abandon/close.
 *   After successful post, req must not be dereferenced (consumed).
 *   mcontrol_free(req) is always safe (no-op if already consumed).
 *   in_len is munix slot cap, not bytes written — frame inside the buffer.
 *   corr: bind copies munix_client_st for async reply. See skin.md §10.11.
 *
 * salloc/alloc return payload base (or mxtalk root). bind/reply/free/getbind
 * take that same base pointer. Inbound in is payload (+ in_len); no body →
 * in==NULL, in_len==0. m1/m2 inbound: (mxtalk_t)in or mxtalk_wrap(in, in_len, 0).
 *
 * alloc(fd, len, timeout) vs alloc_m1/m2talk(fd, timeout, geometry...) —
 * mxtalk has no len; timeout is 2nd. salloc* (server) has no timeout.
 */

#include "munix.h"
#include "mxtalk.h"

struct event_base;

#ifdef __cplusplus
extern "C" {
#endif

typedef const char *(*mcontrol_fn)( int fd,
	const char *key, void *in, size_t in_len,
	munix_client_t client, void **out );

int mcontrol_listen( struct event_base *base, const char *object, mcontrol_fn control,
	int in_slots, size_t in_heap, int out_slots, size_t out_heap );

void        *mcontrol_salloc( int fd, size_t len );
mxtalk_t     mcontrol_salloc_m1talk( int fd, int max_l1, int name_max, int max_heap );
mxtalk_t     mcontrol_salloc_m2talk( int fd,
	int max_l1, int max_l2, int max_l2_pool, int name_max, int max_heap );
/* Deprecated for sync *out: use *out = p. Rare escape to munix_slot_t. */
munix_slot_t mcontrol_slot( void *p );

munix_client_t mcontrol_bind( void *p, munix_client_t client );
munix_client_t mcontrol_getbind( void *p ); /* bound peer copy, or NULL */
int            mcontrol_reply( const char *key, void *p );
void           mcontrol_free( void *p ); /* always safe; no-op if unknown/consumed */
void           mcontrol_close( int fd );

int          mcontrol_connect( const char *object );
void        *mcontrol_alloc( int fd, size_t len, int timeout_ms );
mxtalk_t     mcontrol_alloc_m1talk( int fd, int timeout_ms,
	int max_l1, int name_max, int max_heap );
mxtalk_t     mcontrol_alloc_m2talk( int fd, int timeout_ms,
	int max_l1, int max_l2, int max_l2_pool, int name_max, int max_heap );
const char  *mcontrol_call( int fd, const char *key, void *req, void **rep, int timeout_ms );

#ifdef __cplusplus
}
#endif

#endif /* H_LAND_MCONTROL_H */
