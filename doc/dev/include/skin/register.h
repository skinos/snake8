#ifndef H_LAND_REGISTER_H
#define H_LAND_REGISTER_H

/**
 * @file register.h
 * @brief Shared register variables (mmap hash store)
 */

/*
 * Internal mmap layout (implementation details; not for external callers).
 */
#define REG_MAGIC            0x53475602u
#define REG_VERSION          3u
#define REG_NIL              0xFFFFFFFFu
#define REG_SLOT_USED        1u
#define REG_SLOT_FREE        2u
#define REG_SLOT_LOCKED      4u   /* or'd with USED; cooperative reg_lock */
#define REG_CHUNK_USED       1u
#define REG_CHUNK_FREE       2u
#define REG_NAME_MAX         32
#define REG_DEFAULT_BUCKETS  32u
#define REG_DEFAULT_SYS_SLOTS 1024u   /* no '@' (machine/com/…) */
#define REG_DEFAULT_OBJ_SLOTS 128u    /* name contains '@' */
#define REG_DEFAULT_HEAP     (16u * 1024u)  /* initial heap at create */
#define REG_DEFAULT_HEAP_PER_SLOT 512u      /* max_heap≤0 → slots * this */
#define REG_DEFAULT_VAR_SIZE 64       /* put capacity≤0 default slack; path buffers */
#define REG_NS_CACHE_BUCKETS 64
#define REG_ALIGN8(n)        (((n) + 7u) & ~7u)
typedef struct reg_hdr_st
{
	uint32_t magic;
	uint32_t version;
	uint32_t file_size;   /* committed bytes (heap_off + heap_size) */
	uint32_t bucket_num;
	uint32_t bucket_off;
	uint32_t slot_num;    /* usable slots (= slot cap in v2) */
	uint32_t slot_off;
	uint32_t slot_free;
	uint32_t slot_bump;
	uint32_t heap_off;
	uint32_t heap_size;   /* current committed heap */
	uint32_t heap_free;
	uint32_t heap_bump;
	uint32_t gen;
	uint32_t map_size;    /* mmap window (slot table + heap_cap) */
	uint32_t heap_cap;    /* max heap (grow ceiling) */
	uint32_t seq;         /* seqlock: odd = write in progress */
} reg_hdr_t;
typedef struct reg_slot_st
{
	uint32_t hash;
	uint32_t next;
	uint32_t flags;       /* USED/FREE; USED may or REG_SLOT_LOCKED */
	uint32_t val_off;
	uint32_t val_size;
	uint32_t val_cap;
	uint32_t lock_owner;  /* pid while LOCKED; else 0 */
	char     name[REG_NAME_MAX];
} reg_slot_t;
typedef struct reg_chunk_st
{
	uint32_t size;
	uint32_t flags;
	uint32_t next_free;
	uint32_t reserved;
} reg_chunk_t;
#define REG_CHUNK_HDR_SIZE   ((uint32_t)REG_ALIGN8(sizeof(reg_chunk_t)))
#define REG_MIN_CHUNK        REG_CHUNK_HDR_SIZE
typedef struct reg_ns_st
{
	struct reg_ns_st *cache_next;
	int ref;
	int fd;
	int cached;    /* 1: in process cache (permanent); detach is no-op */
	int writable;  /* 1: PROT_WRITE map; put/del/lock allowed */
	char *mem;
	uint32_t size; /* mmap length (= hdr->map_size) */
	char object[NAME_MAX];
	char path[PATH_MAX];
} *reg_t;
#define reg_hdr(mem)              ((reg_hdr_t *)(mem))
#define reg_buckets(mem, h)       ((uint32_t *)((char *)(mem) + (h)->bucket_off))
#define reg_slots(mem, h)         ((reg_slot_t *)((char *)(mem) + (h)->slot_off))
#define reg_chunk_at(mem, h, off) ((reg_chunk_t *)((char *)(mem) + (h)->heap_off + (off)))


/* Cache attach: stays in process cache forever (reg_detach is no-op).
 * Name with '@' → RDWR map; without '@' → read-only map (put/del/ptr/lock → EROFS).
 * NULL/empty object → MACHINE_REGFILE.
 * max_slots ≤0 → REG_DEFAULT_OBJ_SLOTS if name has '@', else REG_DEFAULT_SYS_SLOTS.
 * max_heap ≤0 → max_slots * REG_DEFAULT_HEAP_PER_SLOT (create only; ignored if file exists).
 * Create: slot table sized to max_slots; heap starts at REG_DEFAULT_HEAP and may grow to max_heap. */
reg_t       reg_attach( const char *object, int max_slots, int max_heap );
void        reg_detach( reg_t r );
/* Write attach: any namespace, always RDWR, never cached. NULL/empty →
 * MACHINE_REGFILE. Handle works with reg_put/reg_ptr/...; must wreg_detach. */
reg_t       wreg_attach( const char *object, int max_slots, int max_heap );
void        wreg_detach( reg_t r );



/* capacity<=0: default slack (REG_DEFAULT_VAR_SIZE or size↑32).
 * Success → value pointer in r's map (invalid after detach); fail → NULL.
 * Wait (default) if another process holds reg_lock on this name;
 * noblock fails with NULL/EBUSY instead. RO handle → EROFS.
 * Key length must be < REG_NAME_MAX. */
void       *reg_put( reg_t r, const char *name, const void *data, int size, int capacity );
void       *reg_put_noblock( reg_t r, const char *name, const void *data, int size, int capacity );
/* Mutable pointer into mmap (RO handle → EROFS). Caller must not use after detach. */
void       *reg_ptr( reg_t r, const char *name, int *size_out );
/* Read-only pointer into mmap (RO or RW handle). Zero-copy; caller copies if needed. */
const void *reg_val( reg_t r, const char *name, int *size_out );
int         reg_len( reg_t r, const char *name );
int         reg_cap( reg_t r, const char *name );
boole       reg_del( reg_t r, const char *name );
boole       reg_del_noblock( reg_t r, const char *name );
/* Cooperative per-variable write lock: slot LOCKED flag + lock_owner pid
 * (under brief flock EX). Stable across value relocate. Kill/exit does not
 * auto-clear; waiters reclaim if owner pid is dead (ESRCH).
 * Success → mmap value pointer (like reg_ptr; valid while lock held / map live);
 * fail → NULL + errno. reg_lock waits; reg_lock_noblock → EBUSY if held.
 * RO handles → EROFS. */
void       *reg_lock( reg_t r, const char *name );
void       *reg_lock_noblock( reg_t r, const char *name );
boole       reg_unlock( reg_t r, const char *name );
/* Typed put/get helpers.
 * put_int/put_boole capacity = sizeof(type); put_str uses reg_put default slack.
 * put_* return void* (caller casts); get_str is zero-copy const char*. */
void       *reg_put_int( reg_t r, const char *name, int v );
void       *reg_put_str( reg_t r, const char *name, const char *s );
#define     reg_put_string                   reg_put_str
void       *reg_put_boole( reg_t r, const char *name, boole v );
int         reg_get_int( reg_t r, const char *name, int def );
const char *reg_get_str( reg_t r, const char *name );
boole       reg_get_boole( reg_t r, const char *name, boole def );
/* List used keys as JSON name→size map; caller must talk_free(). */
talk_t      reg_keys( reg_t r );



/* object-string convenience.
 * sput uses reg_attach+put: sys (no '@') attaches RO → put EROFS; use wreg+put to write sys.
 * capacity<=0 → same default slack as reg_put; sput_int/sput_boole use sizeof(type). */
void       *reg_sput( const char *object, const char *name, const void *data, int size, int capacity );
void       *reg_sput_int( const char *object, const char *name, int v );
void       *reg_sput_str( const char *object, const char *name, const char *s );
#define     reg_sput_string                  reg_sput_str
void       *reg_sput_boole( const char *object, const char *name, boole v );
const void *reg_sget( const char *object, const char *name, int *size_out );
int         reg_sget_int( const char *object, const char *name, int def );
const char *reg_sget_str( const char *object, const char *name );
boole       reg_sget_boole( const char *object, const char *name, boole def );
/* obj_t convenience (NULL obj → default object) */
void       *reg_oput( obj_t this, const char *name, const void *data, int size, int capacity );
void       *reg_oput_int( obj_t this, const char *name, int v );
void       *reg_oput_str( obj_t this, const char *name, const char *s );
#define     reg_oput_string                  reg_oput_str
void       *reg_oput_boole( obj_t this, const char *name, boole v );
const void *reg_oget( obj_t this, const char *name, int *size_out );
int         reg_oget_int( obj_t this, const char *name, int def );
const char *reg_oget_str( obj_t this, const char *name );
boole       reg_oget_boole( obj_t this, const char *name, boole def );
/* s/o lock convenience: leave an attach ref so fd (and fcntl lock) stays alive;
 * pair with matching unlock. Success → value pointer; fail → NULL. */
void       *reg_slock( const char *object, const char *name );
void       *reg_slock_noblock( const char *object, const char *name );
boole       reg_sunlock( const char *object, const char *name );
void       *reg_olock( obj_t this, const char *name );
void       *reg_olock_noblock( obj_t this, const char *name );
boole       reg_ounlock( obj_t this, const char *name );
/* s/o keys convenience (attach → reg_keys → detach; talk_free result). */
talk_t      reg_skeys( const char *object );
talk_t      reg_okeys( obj_t this );






/* Legacy wrappers (prefer reg_oput/reg_sput/reg_oget/… above).
 * register_set/sset → reg_put (capacity honored); sys (no '@') → EROFS. */
void           *register_set( obj_t this, const char *name, const void *v, int size, int capacity );
void           *register_sset( const char *object, const char *name, const void *v, int size, int capacity );
/* Writable mmap ptr (→ reg_ptr); RO handle → EROFS. */
void           *register_pointer( obj_t this, const char *name );
void           *register_spointer( const char *object, const char *name );
/* Read-only mmap ptr (→ reg_val / reg_oget / reg_sget). */
const void     *register_value( obj_t this, const char *name );
const void     *register_svalue( const char *object, const char *name );



/* int: get → reg_oget_int/reg_sget_int(…, 0); set macros → register_set/sset;
 * v = const value ptr; p = mutable ptr. */
int             reg_int( obj_t this, const char *name );
int             reg_sint( const char *object, const char *name );
#define         reg_set_int( this, name, v )     register_set( this, name, &v, sizeof(int), sizeof(int) )
#define         reg_sset_int( this, name, v )    register_sset( this, name, &v, sizeof(int), sizeof(int) )
#define         reg_intv( this, name )           (const int*)register_value( this, name )
#define         reg_sintv( this, name )          (const int*)register_svalue( this, name )
#define         reg_intp( this, name )           (int*)register_pointer( this, name )
#define         reg_sintp( this, name )          (int*)register_spointer( this, name )
/* boole: get → reg_oget_boole/reg_sget_boole(…, false); set/v/p as int family. */
boole           reg_boole( obj_t this, const char *name );
boole           reg_sboole( const char *object, const char *name );
#define         reg_set_boole( this, name, v )   register_set( this, name, &v, sizeof(boole), sizeof(boole) )
#define         reg_sset_boole( this, name, v )  register_sset( this, name, &v, sizeof(boole), sizeof(boole) )
#define         reg_boolev( this, name )         (const boole*)register_value( this, name )
#define         reg_sboolev( this, name )        (const boole*)register_svalue( this, name )
#define         reg_boolep( this, name )         (boole*)register_pointer( this, name )
#define         reg_sboolep( this, name )        (boole*)register_spointer( this, name )
/* string: get → reg_oget_str/reg_sget_str; set → reg_oput_str/reg_sput_str
 * (same path/capacity as *_str; NULL v → ""). v/p as above. */
const char     *reg_string( obj_t this, const char *name );
const char     *reg_sstring( const char *object, const char *name );
char           *reg_set_string( obj_t this, const char *name, const char *v );
char           *reg_sset_string( const char *object, const char *name, const char *v );
#define         reg_stringv( this, name )        (const char*)register_value( this, name )
#define         reg_sstringv( this, name )       (const char*)register_svalue( this, name )
#define         reg_stringp( this, name )        (char*)register_pointer( this, name )
#define         reg_sstringp( this, name )       (char*)register_spointer( this, name )



#endif
