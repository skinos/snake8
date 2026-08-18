#ifndef H_LAND_MXTALK_H
#define H_LAND_MXTALK_H

/**
 * @file mxtalk.h
 * @brief mmap structured talk (m1/m2 create; shared mxjson_* ops)
 *
 * mxtalk_t — process-local map owner (fd, lifetime flock, mmap/wrap).
 * mxjson_t — pointer into the map (root = base, or L1/L2 slot).
 *
 * Max depth is set at create: m1talk_create → depth 1 (flat L1 only);
 * m2talk_create → depth 2 (L1 + L2 OBJECT). Slot offsets are relative to
 * map base except MX_VOID (writer-local absolute pointers).
 *
 * On depth-1 maps, mxtalk_json_create / nested talk_to_mxtalk fail (EPERM).
 *
 * Single-writer: *_create_file / mxtalk_attach(path,1) take exclusive
 * flock for the owner lifetime (blocks another writable open / create
 * rebuild). hdr.writer_va = this mapping's base on claim; node writes
 * require mx_base == writer_va (no pid, no process-global owner table).
 * mxtalk_attach(path,0) is a new mmap (different VA) and does not claim.
 * mxtalk_attach(path,1) also validates hdr layout (depth/caps/offsets);
 * corrupt map → NULL + EINVAL before claim. Claim heals a stuck even seq.
 * After fork the child keeps the same VA — call mxtalk_detach on the
 * inherited handle (caller owns the handle).
 *
 * Lifetime: mxjson_t and string/int interiors are invalid after
 * mxtalk_detach, or after the writer recycles that slot/heap.
 * Seqlock (hdr.seq): even = write/unpublished, odd = quiet. memset leaves
 * seq==0 so readers block until create publishes. Writer reentry when this
 * mapping's base is writer_va and seq is even.
 *
 * MX_VOID cells use mxtalk_set_pointer/mxtalk_pointer. mxtalk_type(root)
 * is always MX_OBJECT. File create rebuilds the map; use attach to open
 * existing.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MX_MAGIC              0x4d32544bu  /* 'M2TK' */
#define MX_VERSION            6u
#define MX_NIL                0xFFFFFFFFu
#define MX_DEFAULT_NAME_MAX   32
#define MX_DEFAULT_HEAP       (64u * 1024u)

#define MX_FREE               0
#define MX_STRING             1
#define MX_INT                2
#define MX_OBJECT             3
#define MX_VOID               4

typedef struct mxtalk_st *mxtalk_t;
typedef struct mxjson_st *mxjson_t;

struct mxtalk_st;
struct mxjson_st;

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
void     mxtalk_detach( mxtalk_t map );
mxjson_t mxtalk_root( mxtalk_t map );
void    *mxtalk_data( mxtalk_t map );
uint32_t mxtalk_size( mxtalk_t map );

/* —— writer coherency —— */
void mxtalk_begin( mxjson_t node );
void mxtalk_end( mxjson_t node );

/* —— set / get (mxjson_t only) —— */
mxjson_t     mxtalk_json_create( mxjson_t parent, const char *name );
mxjson_t     mxtalk_json( mxjson_t parent, const char *name );

char        *mxtalk_set_string( mxjson_t obj, const char *name, const char *val );
char        *mxtalk_set_string_cap( mxjson_t obj, const char *name, const char *val, int cap );
const char  *mxtalk_string( mxjson_t obj, const char *name );
char        *mxtalk_stringp( mxjson_t obj, const char *name );

int64_t     *mxtalk_set_int( mxjson_t obj, const char *name, int64_t v );
int64_t      mxtalk_int( mxjson_t obj, const char *name, int64_t def );
int64_t     *mxtalk_intp( mxjson_t obj, const char *name );

boole        mxtalk_set_pointer( mxjson_t obj, const char *name, void *ptr );
void        *mxtalk_pointer( mxjson_t obj, const char *name );

const char  *mxtalk_name( mxjson_t node );
int          mxtalk_type( mxjson_t node );
boole        mxtalk_exist( mxjson_t obj, const char *name );
int          mxtalk_string_len( mxjson_t obj, const char *name );
int          mxtalk_string_cap( mxjson_t obj, const char *name );

mxjson_t     mxtalk_next( mxjson_t parent, mxjson_t cur );
boole        mxtalk_delete( mxjson_t node );
boole        mxtalk_remove( mxjson_t parent, const char *name );

talk_t       mxtalk_to_talk( mxjson_t node );
boole        talk_to_mxtalk( mxjson_t parent, const char *name, talk_t src );

#ifdef __cplusplus
}
#endif

#endif
