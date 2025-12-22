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
#define REGISTER_VAR_NUM          (100)
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

/// register underlying functions
register_file_t register_open( const char *object, int flags, int mode, int value_number, int total_size );
register_var_t  register_search( register_file_t h, void *point, const char *name );
void            register_close( register_file_t h );
/// register help functions
int 			register_value_size( register_file_t h, const char *name );
void           *register_value_pointer( register_file_t h, const char *name );
void           *register_value_set( register_file_t h, const char *name, const void *v, int size, int capacity );

/// register general functions
void           *register_set( obj_t this, const char *name, const void *v, int size, int capacity );
void           *register_sset( const char *object, const char *name, const void *v, int size, int capacity );

void           *register_pointer( obj_t this, const char *name );
void           *register_spointer( const char *object, const char *name );

const void	   *register_value(     obj_t this, const char *name );
const void	   *register_svalue( const char *object, const char *name );

int 	        register_size(     obj_t this, const char *name );
int 	        register_ssize( const char *object, const char *name );

void            register_sync(      obj_t this );
void            register_ssync( const char *object );

// register lock functions F_WRLCK, F_RDLCK
boole           register_lock( obj_t this, void *point, const char *name, int flag );
boole           register_lockw( obj_t this, void *point, const char *name, int flag );
boole			register_unlock( obj_t this, void *point, const char *name );



/**
 * @brief list all register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @return talk
 		$retval talk for succeed
 		$retval NULL for failed and errno will be set
 */
talk_t          reg_list( obj_t this );
talk_t          reg_slist( const char *object );



/**
 * @brief get value of integer date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return integer
 		$retval !=0 for succeed
 		$retval =0 for succeed
 		$retval =0 for failed and errno will be set
 */
int             reg_int( obj_t this, const char *name );
int             reg_sint( const char *object, const char *name );
/**
 * @brief set a integer date to register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @param[in] v, register name value
 * @return a pointer for integer or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_set_int( this, name, v )                register_set( this, name, &v, sizeof(int), sizeof(int) )
#define         reg_sset_int( this, name, v )               register_sset( this, name, &v, sizeof(int), sizeof(int) )
/**
 * @brief get a readonly pointer of integer date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return a pointer for integer or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_intv( this, name )          (const int*)register_value( this, name )
#define         reg_sintv( this, name )         (const int*)register_svalue( this, name )
/**
 * @brief get a read and write pointer of integer date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return a pointer for integer or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_intp( this, name )                (int*)register_pointer( this, name )
#define         reg_sintp( this, name )               (int*)register_spointer( this, name )



/**
 * @brief get value of boole date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return boole
 		$retval true for succeed
 		$retval false for succeed
 		$retval false for failed and errno will be set
 */
boole           reg_boole( obj_t this, const char *name );
boole           reg_sboole( const char *object, const char *name );
/**
 * @brief set a boole date to register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @param[in] v, register name value
 * @return a pointer for boole or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_set_boole( this, name, v )                register_set( this, name, &v, sizeof(boole), sizeof(boole) )
#define         reg_sset_boole( this, name, v )               register_sset( this, name, &v, sizeof(boole), sizeof(boole) )
/**
 * @brief get a readonly pointer of boole date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return a pointer for boole or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_boolev( this, name )        (const boole*)register_value( this, name )
#define         reg_sboolev( this, name )       (const boole*)register_svalue( this, name )
/**
 * @brief get a read and write pointer of boole date register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return a pointer for boole or NULL
 		$retval a pointer for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_boolep( this, name )              (boole*)register_pointer( this, name )
#define         reg_sboolep( this, name )             (boole*)register_spointer( this, name )



/**
 * @brief get string from register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return string or NULL
 		$retval string for succeed, 
 		@retval NULL for failed and errno will be set
 */
const char     *reg_string( obj_t this, const char *name );
const char     *reg_sstring( const char *object, const char *name );
/**
 * @brief set a string date to register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @param[in] v, register name value
 * @return string or NULL
 		$retval string for succeed, 
 		@retval NULL for failed and errno will be set
 */
char	         *reg_set_string( obj_t this, const char *name, const char *v );
char	         *reg_sset_string( const char *object, const char *name, const char *v );
/**
 * @brief get a readonly string register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return string or NULL
 		$retval string for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_stringv( this, name )        (const boole*)register_value( this, name )
#define         reg_sstringv( this, name )       (const boole*)register_svalue( this, name )
/**
 * @brief get a read and write string register
 * @param[in] this, a object pointer( use the land@machine when this be NULL )
 * @param[in] name, register name
 * @return string or NULL
 		$retval string for succeed, 
 		@retval NULL for failed and errno will be set
 */
#define         reg_stringp( this, name )              (boole*)register_pointer( this, name )
#define         reg_sstringp( this, name )             (boole*)register_spointer( this, name )



#endif   /* ----- #ifndef H_LAND_REGISTER_H  ----- */

