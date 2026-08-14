#ifndef H_LAND_MXTALK_H
#define H_LAND_MXTALK_H

/**
 * @file mxtalk.h
 * @brief mmap structured talk (m1/m2 create; shared mxtalk_* ops)
 *
 * Max depth is set at create: m1talk_create → depth 1 (flat L1 only);
 * m2talk_create → depth 2 (L1 + L2 OBJECT). Handles are nullable pointers
 * (root = map base; children = L1/L2 slots). Offsets are relative to map base
 * except MX_VOID (writer-local absolute pointers).
 *
 * On depth-1 maps, mxtalk_json_create / nested talk_to_mxtalk fail (EPERM).
 *
 * Single-writer: *_create_file / mxtalk_attach(path,1) take an exclusive
 * flock for the handle lifetime; another writable open fails with EBUSY.
 * mxtalk_attach(path,0) is shared read and does not take that lock.
 *
 * Lifetime: child handles and string/int/pointer interiors into the map are like
 * pointers into a malloc block — invalid after mxtalk_detach, or after the
 * writer recycles that slot/heap (remove/delete/type-change/STRING grow).
 * No handle generation; caller must not use dangling interiors (re-lookup or
 * copy out). Seqlock protects structure/lookup snapshots, not pointer lifetime.
 *
 * Getters / introspect / mxtalk_next wait out an in-progress writer (odd seq)
 * instead of failing with EAGAIN: string/stringp/json/int/exist/string_len/string_cap/
 * name/type/next/intp/pointer/to_talk. The writer process may call those same
 * getters while inside begin/end (reentrant; does not self-deadlock). NULL getters
 * still set errno: EINVAL (bad arg / wrong parent / wrong type), ENOENT (missing key),
 * EROFS (writable-only stringp/intp/pointer on RO). Success leaves errno unchanged.
 * mxtalk_int() still returns def on miss/wrong type without requiring errno.
 * talk_to_mxtalk publishes the whole import under one begin/end.
 *
 * MX_VOID cells use mxtalk_set_pointer/mxtalk_pointer. mxtalk_type(root) is
 * always MX_OBJECT. File create rebuilds the map; use attach to open existing.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MX_MAGIC              0x4d32544bu  /* 'M2TK' */
#define MX_VERSION            2u
#define MX_NIL                0xFFFFFFFFu
#define MX_DEFAULT_NAME_MAX   32
#define MX_DEFAULT_HEAP       (64u * 1024u)

#define MX_FREE               0
#define MX_STRING             1
#define MX_INT                2
#define MX_OBJECT             3
#define MX_VOID               4

typedef struct mxtalk_st *mxtalk_t;

/* Opaque; callers only pass/compare pointers. Root points at map hdr. */
struct mxtalk_st;

#define mxtalk_each(var, parent) \
	for ( (var) = mxtalk_next( (parent), NULL ); \
	      (var) != NULL; \
	      (var) = mxtalk_next( (parent), (var) ) )

/* —— create (depth 1: STRING|INT|VOID only, no L2) —— */
mxtalk_t m1talk_create( int max_l1, int name_max, int max_heap );
mxtalk_t m1talk_create_file( const char *path, int max_l1, int name_max, int max_heap );
uint32_t m1talk_map_size( int max_l1, int name_max, int max_heap );
mxtalk_t m1talk_create_mem( void *mem, uint32_t size, int max_l1, int name_max, int max_heap );

/* —— create (depth 2) —— */
mxtalk_t m2talk_create( int max_l1, int max_l2, int max_l2_pool,
	int name_max, int max_heap );
mxtalk_t m2talk_create_file( const char *path, int max_l1, int max_l2,
	int max_l2_pool, int name_max, int max_heap );
uint32_t m2talk_map_size( int max_l1, int max_l2, int max_l2_pool,
	int name_max, int max_heap );
mxtalk_t m2talk_create_mem( void *mem, uint32_t size, int max_l1, int max_l2,
	int max_l2_pool, int name_max, int max_heap );

/* —— lifecycle —— */
mxtalk_t mxtalk_attach( const char *path, int writable );
mxtalk_t mxtalk_wrap( void *mem, uint32_t size, int writable );
void     mxtalk_detach( mxtalk_t t );
void    *mxtalk_data( mxtalk_t t );
uint32_t mxtalk_size( mxtalk_t t );

/* —— writer coherency (raw cell writes must sit inside begin/end) —— */
void mxtalk_begin( mxtalk_t t );
void mxtalk_end( mxtalk_t t );

/* —— set / get —— */
mxtalk_t     mxtalk_json_create( mxtalk_t t, const char *name );
mxtalk_t     mxtalk_json( mxtalk_t t, const char *name );

char        *mxtalk_set_string( mxtalk_t t, const char *name, const char *val );
char        *mxtalk_set_string_cap( mxtalk_t t, const char *name, const char *val, int cap );
const char  *mxtalk_string( mxtalk_t t, const char *name );
char        *mxtalk_stringp( mxtalk_t t, const char *name );

int64_t     *mxtalk_set_int( mxtalk_t t, const char *name, int64_t v );
int64_t      mxtalk_int( mxtalk_t t, const char *name, int64_t def );
int64_t     *mxtalk_intp( mxtalk_t t, const char *name );

boole        mxtalk_set_pointer( mxtalk_t t, const char *name, void *ptr );
void        *mxtalk_pointer( mxtalk_t t, const char *name );

/* —— walk / introspect / delete —— */
const char  *mxtalk_name( mxtalk_t t );
int          mxtalk_type( mxtalk_t t );
boole        mxtalk_exist( mxtalk_t t, const char *name );
int          mxtalk_string_len( mxtalk_t t, const char *name );
int          mxtalk_string_cap( mxtalk_t t, const char *name );

mxtalk_t     mxtalk_next( mxtalk_t parent, mxtalk_t cur );
boole        mxtalk_delete( mxtalk_t node );
boole        mxtalk_remove( mxtalk_t parent, const char *name );

/* —— optional talk_t bridge —— */
talk_t       mxtalk_to_talk( mxtalk_t t );
boole        talk_to_mxtalk( mxtalk_t t, const char *name, talk_t src );

#ifdef __cplusplus
}
#endif

#endif
