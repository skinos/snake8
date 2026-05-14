#ifndef H_LAND_REGISTER_H
#define H_LAND_REGISTER_H

/**
 * @file register.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief global register variable implementation
 * @details Functions similar to environment variables are different from environment variables as follows:
 			1. register variable have only one COPY of information in the system, as opposed to environment variables having a COPY per process
 			2. Can store binary information
 			3. Can be used for inter-process communication, cross-process small data interaction
 */



/// register file handler structure: this structure is a handler of register file
typedef struct register_file_st
{
	// link node for add to linker
	link_struct link;
	// reference count
	int ref;
	// file handler
	int fd;
	int flags;
	// mmap to the memory
	char *mem;
	// mmap size
	int size;
	// register object
	char object[NAME_MAX];
	// register file pathname
	char path[PATH_MAX];
} register_file_struct;
typedef register_file_struct* register_file_t;
/* default number register at one register file */
#define REGISTER_VAR_NUM          (50)
/* register value default size */
#define REGISTER_VAR_SIZE         (NAME_MAX)
/* register default object */
#define REGISTER_DEFAULT_OBJECT   MACHINE_COM
/// register index structure: the structure store at the head of register file to describe file layout
typedef struct register_index_st
{
	int magic;				   // magic head
	int var_numb;			   // variable number
	int var_offset; 		   // variable offset at the file
	int var_inuse;			   // how many variable in use
	int value_size; 		   // variable value size
	int value_offset;		   // variable value offset at the file
	int value_inuse;		   // how many variable value in use
} register_index_struct;
typedef register_index_struct* register_index_t;
/// register variable structure: this structure is a handler of register variable
typedef struct register_var_st
{
	char identify[NAME_MAX];	 // variable name
	int value_start;			 // variable value offset at file
	int value_size; 			 // register value size
} register_var_struct;
typedef register_var_struct* register_var_t;

/**
 * @brief open a register file and map it into memory
 * @param[in] object register object name (e.g., MACHINE_COM)
 * @param[in] flags open flags (O_RDONLY, O_RDWR, O_CREAT, etc.)
 * @param[in] mode file permissions mode when creating (e.g., 0644)
 * @param[in] value_number maximum number of register variables in the file
 * @param[in] total_value_size total bytes reserved for all variable values in the file (value heap)
 * @return register file handler
 * 		@retval register_file_t for succeed
 *  	@retval NULL for failed, errno will be set
 * @note Must call register_close() to release the handler when done
 * @see register_close
 */
register_file_t register_open( const char *object, int flags, int mode, int value_number, int total_value_size );
/**
 * @brief search for a register variable by name or by mapped value address
 * @param[in] h register file handler returned by register_open()
 * @param[in] name variable name to match (used when value_start is NULL); must be a valid string for name lookup
 * @param[in] value_start if non-NULL, find the slot whose value storage starts at this pointer inside h->mem; name is ignored in that mode
 * @return pointer to the in-map register_var_struct for the match
 * 		@retval non-NULL for found
 *  	@retval NULL for not found or invalid arguments, errno will be set (e.g. EINVAL)
 * @note Exactly one of the two search modes applies: by value_start, or by name when value_start is NULL
 */
register_var_t  register_search( register_file_t h, const char *name, void *value_start );
/**
 * @brief close a register file handler and unmap memory
 * @param[in] h register file handler to close
 * @return none
 * @note Directly closes the file descriptor, unmaps memory, and frees the handler
 * @see register_open
 */
void            register_close( register_file_t h );

/**
 * @brief get the value storage size of a register variable
 * @param[in] h register file handler (must be non-NULL)
 * @param[in] name register variable name (must not be NULL) (must be non-NULL)
 * @return size in bytes
 * 		@retval positive for succeed
 *  	@retval negative for not found or error, errno will be set
 */
int 			register_value_size( register_file_t h, const char *name );
/**
 * @brief get a read-write pointer to a register variable's value storage
 * @param[in] h register file handler (must be non-NULL)
 * @param[in] name register variable name (must not be NULL) (must be non-NULL)
 * @return pointer to value storage area
 * 		@retval pointer for succeed (directly mapped memory)
 *  	@retval NULL for not found, errno will be set
 * @note The returned pointer points to mmap'd memory, writes are immediately visible
 * @warning Do not write beyond register_value_size() bytes
 */
void           *register_value_pointer( register_file_t h, const char *name );
/**
 * @brief set a register variable's value in the register file
 * @param[in] h register file handler
 * @param[in] name register variable name (must not be NULL) (must be non-NULL; created if not exists)
 * @param[in] v pointer to value data to store
 * @param[in] size size of value data in bytes
 * @param[in] capacity minimum storage capacity to allocate (if larger than size, extra space is reserved for future growth)
 * @return pointer to the stored value in register file
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, errno will be set
 * @note If capacity > size, the variable gets a larger storage area for future in-place updates
 */
void           *register_value_set( register_file_t h, const char *name, const void *v, int size, int capacity );

/**
 * @brief set a register variable value using object pointer
 * @param[in] this object pointer (must not be NULL)
 * @param[in] name register variable name (must not be NULL)
 * @param[in] v pointer to value data to store
 * @param[in] size size of value data in bytes
 * @param[in] capacity minimum storage capacity (if larger than size, reserves extra space)
 * @return pointer to the stored value in register
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, errno will be set
 * @see register_sset for string-based object specification
 */
void           *register_set( obj_t this, const char *name, const void *v, int size, int capacity );
/**
 * @brief set a register variable value using string object name
 * @param[in] object object name string (e.g., "land@machine"; must not be NULL, not REGISTER_DEFAULT_OBJECT)
 * @param[in] name register variable name (must not be NULL)
 * @param[in] v pointer to value data to store
 * @param[in] size size of value data in bytes
 * @param[in] capacity minimum storage capacity (if larger than size, reserves extra space)
 * @return pointer to the stored value in register
 * 		@retval pointer for succeed
 *  	@retval NULL for failed, errno will be set
 * @see register_set for obj_t-based object specification
 */
void           *register_sset( const char *object, const char *name, const void *v, int size, int capacity );

/**
 * @brief get a read-write pointer to a register variable's value using object pointer
 * @param[in] this object pointer (must not be NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return read-write pointer to value storage
 * 		@retval pointer for succeed (directly mapped memory, writable)
 *  	@retval NULL for not found, errno will be set
 * @warning Writes through this pointer are immediately effective; do not exceed register_size() bytes
 * @see register_spointer for string-based object specification
 */
void           *register_pointer( obj_t this, const char *name );
/**
 * @brief get a read-write pointer to a register variable's value using string object name
 * @param[in] object object name string (e.g., "land@machine")
 * @param[in] name register variable name (must not be NULL)
 * @return read-write pointer to value storage
 * 		@retval pointer for succeed (directly mapped memory, writable)
 *  	@retval NULL for not found, errno will be set
 * @see register_pointer for obj_t-based object specification
 */
void           *register_spointer( const char *object, const char *name );

/**
 * @brief get a read-only pointer to a register variable's value using object pointer
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return read-only pointer to value data
 * 		@retval pointer for succeed
 *  	@retval NULL for not found, errno will be set
 * @note The returned pointer is valid as long as the register file is open
 * @see register_svalue for string-based object specification
 */
const void	   *register_value(     obj_t this, const char *name );
/**
 * @brief get a read-only pointer to a register variable's value using string object name
 * @param[in] object object name string (e.g., "land@machine"; must not be NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return read-only pointer to value data
 * 		@retval pointer for succeed
 *  	@retval NULL for not found, errno will be set
 * @see register_value for obj_t-based object specification
 */
const void	   *register_svalue( const char *object, const char *name );

/**
 * @brief get the value storage size of a register variable using object pointer
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return size in bytes
 * 		@retval positive for succeed
 *  	@retval negative for not found or error, errno will be set
 * @see register_ssize for string-based object specification
 */
int 	        register_size(     obj_t this, const char *name );
/**
 * @brief get the value storage size of a register variable using string object name
 * @param[in] object object name string (e.g., "land@machine"; must not be NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return size in bytes
 * 		@retval positive for succeed
 *  	@retval negative for not found or error, errno will be set
 * @see register_size for obj_t-based object specification
 */
int 	        register_ssize( const char *object, const char *name );

/**
 * @brief synchronize register file changes to disk using object pointer
 * @param[in] this object pointer (use default object when NULL)
 * @return none
 * @note Calls msync() on the mmap'd region to flush changes to disk
 * @see register_ssync for string-based object specification
 */
void            register_sync(      obj_t this );
/**
 * @brief synchronize register file changes to disk using string object name
 * @param[in] object object name string (e.g., "land@machine"; must not be NULL)
 * @return none
 * @note Calls msync() on the mmap'd region to flush changes to disk
 * @see register_sync for obj_t-based object specification
 */
void            register_ssync( const char *object );

/**
 * @brief acquire advisory fcntl record lock on one byte at address point (non-blocking)
 * @param[in] this object pointer (must not be NULL)
 * @param[in] point address inside this->wreg or this->rreg mmap (e.g. from register_pointer() or register_value()); must not be NULL
 * @return operation result
 * 		@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Uses fcntl(F_SETLK). If point lies in wreg the lock type is F_WRLCK; if in rreg, F_RDLCK. Does not call register_open().
 * @see register_lockw register_unlock
 */
boole           register_lock( obj_t this, const void *point );
/**
 * @brief acquire advisory fcntl record lock on one byte at address point (blocking)
 * @param[in] this object pointer (must not be NULL)
 * @param[in] point address inside this->wreg or this->rreg mmap (e.g. from register_pointer() or register_value()); must not be NULL
 * @return operation result
 * 		@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Uses fcntl(F_SETLKW). If point lies in wreg the lock type is F_WRLCK; if in rreg, F_RDLCK. Does not call register_open().
 * @see register_lock register_unlock
 */
boole           register_lockw( obj_t this, const void *point );
/**
 * @brief release advisory fcntl record lock on one byte at address point
 * @param[in] this object pointer (must not be NULL)
 * @param[in] point same address passed to register_lock() or register_lockw(); must not be NULL
 * @return operation result
 * 		@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Uses fcntl(F_SETLK) with F_UNLCK. wreg vs rreg resolution must match the lock call.
 * @see register_lock register_lockw
 */
boole			register_unlock( obj_t this, const void *point );



/**
 * @brief list all register variables using object pointer
 * @param[in] this object pointer (use default object when NULL)
 * @return talk_t json list of register variable names and values
 * 		@retval talk_t json for succeed - caller must free with talk_free()
 * 		@retval NULL for failed and errno will be set
 * @see reg_slist for string-based object specification
 */
talk_t          reg_list( obj_t this );
/**
 * @brief list all register variables using string object name
 * @param[in] object object name string (e.g., "land@machine"; must not be NULL)
 * @return talk_t json list of register variable names and values
 * 		@retval talk_t json for succeed - caller must free with talk_free()
 * 		@retval NULL for failed and errno will be set
 * @see reg_list for obj_t-based object specification
 */
talk_t          reg_slist( const char *object );



/**
 * @brief get integer value from register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return integer
 * 		@retval integer value for succeed (including 0)
 * 		@retval 0 for failed, errno will be set (check errno to distinguish)
 * @note IMPORTANT: Cannot distinguish between value 0 and error by return value alone!
 * @note Always check errno to determine if an error occurred:
 * @code
 * errno = 0;
 * int value = reg_int(obj, "counter");
 * if (errno != 0) {
 *     // Error occurred
 *     perror("reg_int failed");
 * } else {
 *     // Success - value is valid (may be 0)
 *     printf("Counter: %d\n", value);
 * }
 * @endcode
 * @note For reliable error detection, use register_value() and check return pointer
 * @see register_value for pointer-based access with better error detection
 * @see reg_set_int for setting integer values
 */
int             reg_int( obj_t this, const char *name );
int             reg_sint( const char *object, const char *name );
/**
 * @brief set a integer value to register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @param[in] v register value
 * @return a pointer for integer or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_set_int( this, name, v )                register_set( this, name, &v, sizeof(int), sizeof(int) )
#define         reg_sset_int( this, name, v )               register_sset( this, name, &v, sizeof(int), sizeof(int) )
/**
 * @brief get a readonly pointer of integer data in register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return a pointer for integer or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_intv( this, name )          (const int*)register_value( this, name )
#define         reg_sintv( this, name )         (const int*)register_svalue( this, name )
/**
 * @brief get a read and write pointer of integer data in register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return a pointer for integer or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_intp( this, name )                (int*)register_pointer( this, name )
#define         reg_sintp( this, name )               (int*)register_spointer( this, name )



/**
 * @brief get value of boole data in register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return boole
 * 		@retval true for succeed
 * 		@retval false for succeed
 * 		@retval false for failed and errno will be set
 */
boole           reg_boole( obj_t this, const char *name );
boole           reg_sboole( const char *object, const char *name );
/**
 * @brief set a boole value to register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @param[in] v register value
 * @return a pointer for boole or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_set_boole( this, name, v )                register_set( this, name, &v, sizeof(boole), sizeof(boole) )
#define         reg_sset_boole( this, name, v )               register_sset( this, name, &v, sizeof(boole), sizeof(boole) )
/**
 * @brief get a readonly pointer of boole data in register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return a pointer for boole or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_boolev( this, name )        (const boole*)register_value( this, name )
#define         reg_sboolev( this, name )       (const boole*)register_svalue( this, name )
/**
 * @brief get a read and write pointer of boole data in register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return a pointer for boole or NULL
 * 		@retval a pointer for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_boolep( this, name )              (boole*)register_pointer( this, name )
#define         reg_sboolep( this, name )             (boole*)register_spointer( this, name )



/**
 * @brief get string from register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return string or NULL
 * 		@retval string for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
const char     *reg_string( obj_t this, const char *name );
const char     *reg_sstring( const char *object, const char *name );
/**
 * @brief set a string value to register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @param[in] v register value
 * @return string or NULL
 * 		@retval string for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
char	         *reg_set_string( obj_t this, const char *name, const char *v );
char	         *reg_sset_string( const char *object, const char *name, const char *v );
/**
 * @brief get a readonly string register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return string or NULL
 * 		@retval string for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_stringv( this, name )        (const char*)register_value( this, name )
#define         reg_sstringv( this, name )       (const char*)register_svalue( this, name )
/**
 * @brief get a read and write string register
 * @param[in] this object pointer (use default object when NULL)
 * @param[in] name register variable name (must not be NULL)
 * @return string or NULL
 * 		@retval string for succeed, 
 * 		@retval NULL for failed and errno will be set
 */
#define         reg_stringp( this, name )              (char*)register_pointer( this, name )
#define         reg_sstringp( this, name )             (char*)register_spointer( this, name )



#endif   /* ----- #ifndef H_LAND_REGISTER_H  ----- */

