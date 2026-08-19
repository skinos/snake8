#ifndef H_LAND_COM_H
#define H_LAND_COM_H

/**
 * @file com.h
 * @author dimmalex@gmail.com
 * @version 8.0
 * @date 20240903
 * @brief component call implementation
 * @details implementation communication to other component function use talk structure or parameter structure
 */



/**
 * @brief function type of get the runtime configure of component
 * @param obj_t object pointer identifying the component
 * @param attr_t attribute path within the component configuration
 * @return talk_t configuration value, caller must free with talk_free(); NULL for not found
 */
typedef talk_t (*comget_t)( obj_t, attr_t );
/**
 * @brief function type of set the runtime configure of component
 * @param obj_t object pointer identifying the component
 * @param talk_t value to set (json/x type)
 * @param attr_t attribute path within the component configuration
 * @return true for succeed, false for failed
 */
typedef boole  (*comset_t)( obj_t, talk_t, attr_t );
/**
 * @brief function type of fetch the persistent database of component
 * @param obj_t object pointer identifying the component
 * @param attr_t file attribute (namespace/file path)
 * @param attr_t attribute path within the database file
 * @return talk_t database value, caller must free with talk_free(); NULL for not found
 */
typedef talk_t (*comfetch_t)( obj_t, attr_t, attr_t );
/**
 * @brief function type of save to the persistent database of component
 * @param obj_t object pointer identifying the component
 * @param attr_t file attribute (namespace/file path)
 * @param talk_t value to save (json/x type)
 * @param attr_t attribute path within the database file
 * @return true for succeed, false for failed
 */
typedef boole  (*comsave_t)( obj_t, attr_t, talk_t, attr_t );
/**
 * @brief universal api function type of component
 * @param obj_t object pointer identifying the component
 * @param param_t parameter structure containing call arguments
 * @return void* result pointer, actual type depends on the API implementation
 */
typedef void*  (*comapi_t)( obj_t, param_t );

/**
 * @brief One row in the executable (COM_FILE_EXECUTE) API dispatch table: method name → handler.
 * @see MAIN2API
 */
typedef struct eapi_table_st
{
	const char *name;
	comapi_t    fn;
} eapi_table_t;

/**
 * @brief Define main() for a standalone executable with custom get/set handlers.
 * @param table Array of eapi_table_t (method name → comapi_t). Must be an array identifier.
 * @param getfn comget_t (obj, attr) → talk_t; used when API is "get" (from cget/sget).
 * @param setfn comset_t (obj, talk, attr) → boole; used when API is "set" (from cset/sset).
 * @note "get"/"set"/"exist" are not looked up in table; methods stay in table. setfn's boole is
 *       mapped to ttrue/tfalse before talk2fd. Prefer MAIN2API(table) when config_* is enough.
 * @note "exist" is a meta probe (PARAM1=api name → ttrue/tfalse) used by com_exist/com_have.
 */
#define MAIN2COM( table, getfn, setfn ) \
int main( int argc, const char **argv ) \
{ \
	he_t h; \
	size_t i; \
	obj_t this; \
	talk_t ret; \
	int pipe_fd; \
	int exit_code; \
	param_t param; \
	const char *api; \
					\
	h = NULL;	\
	this = execute_object(); \
	api = execute_api(); \
	pipe_fd = execute_pipe(); \
	/* Shell spawn sets API/cpipe even when OBJECT is omitted (e.g. com_exist). */ \
	if ( api != NULL || pipe_fd >= 0 ) \
	{ \
		param = execute_param(); \
	} \
	else \
	{ \
		h = argv2he( argc, argv );  \
		if ( h == NULL ) \
		{ \
			perror( "parameter" ); \
			return -1; \
		} \
		this = h->o; \
		param = h->p; \
		api = h->m; \
		pipe_fd = STDOUT_FILENO; \
	} \
								\
	/* Default tpanic: API not exported (same as ccall dlsym miss). terror only from peer fn. */ \
	ret = tpanic; \
	if ( api != NULL && *api != '\0' ) \
	{ \
		if ( 0 == strcmp( api, "exist" ) ) \
		{ \
			/* Meta probe: PARAM1 = API name. Object unused for now. */ \
			const char *name; \
			boole found; \
			name = param_string( param, 1 ); \
			found = false; \
			if ( name != NULL && *name != '\0' ) \
			{ \
				if ( 0 == strcmp( name, "get" ) || 0 == strcmp( name, "set" ) \
					|| 0 == strcmp( name, "exist" ) ) \
				{ \
					found = true; \
				} \
				else \
				{ \
					for ( i = 0; i < sizeof( table ) / sizeof( (table)[0] ); i++ ) \
					{ \
						if ( 0 == strcmp( name, (table)[i].name ) ) \
						{ \
							found = true; \
							break; \
						} \
					} \
				} \
			} \
			ret = ( found == true ) ? ttrue : tfalse; \
			if ( found == false ) \
			{ \
				errno = ENOSYS; \
			} \
		} \
		else if ( 0 == strcmp( api, "get" ) ) \
		{ \
			if ( this == NULL ) \
			{ \
				errno = EINVAL; \
				ret = tpanic; \
			} \
			else \
			{ \
				attr_t a; \
				const char *ap; \
				a = NULL; \
				ap = param_string( param, 1 ); \
				if ( ap != NULL && *ap != '\0' ) \
				{ \
					a = attr_create( ap ); \
				} \
				ret = (talk_t)( (getfn)( this, a ) ); \
				attr_free( a ); \
			} \
		} \
		else if ( 0 == strcmp( api, "set" ) ) \
		{ \
			if ( this == NULL ) \
			{ \
				errno = EINVAL; \
				ret = tpanic; \
			} \
			else \
			{ \
				boole ok; \
				attr_t a; \
				talk_t v; \
				const char *ap; \
				a = NULL; \
				ap = param_string( param, 1 ); \
				v = param_talk( param, 2 ); \
				if ( ap != NULL && *ap != '\0' ) \
				{ \
					a = attr_create( ap ); \
				} \
				ok = (setfn)( this, v, a ); \
				ret = ( ok == true ) ? ttrue : tfalse; \
				attr_free( a ); \
			} \
		} \
		else \
		{ \
			boole found; \
			found = false; \
			if ( this == NULL ) \
			{ \
				errno = EINVAL; \
				ret = tpanic; \
			} \
			else \
			{ \
				for ( i = 0; i < sizeof( table ) / sizeof( (table)[0] ); i++ ) \
				{ \
					if ( 0 == strcmp( api, (table)[i].name ) ) \
					{ \
						ret = (talk_t)( (table)[i].fn( this, param ) ); \
						found = true; \
						break; \
					} \
				} \
				if ( found == false ) \
				{ \
					errno = ENOSYS; \
				} \
			} \
		} \
	} \
	else if ( this == NULL ) \
	{ \
		errno = EINVAL; \
		ret = tpanic; \
	} \
	else \
	{ \
		ret = (talk_t)( (table)[0].fn( this, param ) ); \
	} \
	if ( h == NULL ) \
	{ \
		exit_code = EXIT_EFUNC; \
		if ( ret > tpanic ) \
		{ \
			exit_code = 0; \
		} \
		if ( ret == ttrue ) \
		{ \
			exit_code = EXIT_ttrue; \
		} \
		else if ( ret == tfalse ) \
		{ \
			exit_code = EXIT_tfalse; \
		} \
		else if ( ret == tnull ) \
		{ \
			exit_code = EXIT_tnull; \
		} \
		else if ( ret == terror ) \
		{ \
			exit_code = EXIT_terror; \
		} \
		if ( pipe_fd >= 0 ) \
		{ \
			talk2fd( pipe_fd, ret, errno ); \
			close( pipe_fd ); \
		} \
		param_free( param ); \
		obj_free( this ); \
		talk_free( ret ); \
	} \
	else \
	{ \
		exit_code = 0; \
		if ( ret > tpanic )      \
		{                         \
			talk_print( ret );   \
			talk_free( ret );    \
		}                         \
		else if ( ret == ttrue )  \
		{                           \
			printf( ttrue_string"\n" );  \
		}                          \
		else if ( ret == tfalse )   \
		{	\
			if ( errno == 0 )	\
			{	\
				printf( tfalse_string"\n" );	\
			}	\
			else	\
			{	\
				printf( tfalse_string", %s\n", strerror( errno ) );	\
			}	\
			exit_code = -1; \
		}	\
		else if ( ret == terror )	\
		{	\
			if ( errno == 0 )	\
			{	\
				printf( terror_string"\n" );	\
			}	\
			else	\
			{	\
				printf( terror_string", %s\n", strerror( errno ) );	\
			}	\
			exit_code = -2; \
		}	\
		else if ( ret == tpanic )	\
		{	\
			if ( errno == 0 )	\
			{	\
				printf( tpanic_string"\n" );   \
			}   \
			else   \
			{   \
				printf( tpanic_string", %s\n", strerror( errno ) );   \
			}      \
			exit_code = -3; \
		}                 \
		he_free( h );     \
	}                    \
	return exit_code; \
}

/**
 * @brief Define main() for a standalone executable: methods via table; get/set via config_*.
 * @param table Array of eapi_table_t. Must be an array identifier (e.g. static const eapi_table_t foo[]),
 *              not a pointer, so sizeof(table)/sizeof((table)[0]) yields the entry count.
 * @note Prefer execute_object / execute_param / execute_api / execute_pipe when spawned by shell;
 *       otherwise falls back to argv2he(argc, argv). Maps return like ccall peers.
 *       Equivalent to MAIN2COM(table, config_get, config_set). Use MAIN2COM for custom _get/_set.
 */
#define MAIN2API( table ) MAIN2COM( table, config_get, config_set )

/// Prefix for component API function symbol names (e.g., function "status" is exported as "_status")
#define COM_API_PREFIX "_"
/// Macro to declare a component API function returning talk_t
/// @param func API function name (e.g., status)
/// @param obj object parameter name
/// @param param parameter parameter name
#define COM_TALK_API( func, obj, param )   talk_t _##func( obj, param )
/// Macro to declare a component API function returning boole_t
/// @param func API function name
/// @param obj object parameter name
/// @param param parameter parameter name
#define COM_BOOLE_API( func, obj, param )  boole_t _##func( obj, param )
/// Macro to declare the standard getter API (_get) for a component
/// @param obj object parameter name
/// @param attr attribute parameter name
#define COM_GET_API( obj, attr )           talk_t _get( obj, attr )
/// Macro to declare the standard setter API (_set) for a component
/// @param obj object parameter name
/// @param v value parameter name
/// @param attr attribute parameter name
#define COM_SET_API( obj, v, attr )        boole _set( obj, v, attr )

/**
 * @brief get the object pointer of the current execute calling context
 * @return obj_t of the current execute context
 * 	@retval obj_t for succeed
 *  @retval NULL when not in a execute context
 * @note Only valid inside a component API invoked via shell (he command)
 */
obj_t       execute_object( void );
/**
 * @brief get the parameter structure of the current execute calling context
 * @return param_t of the current execute context
 * 	@retval non-NULL parameter object on success (possibly empty: no options when PARAM_SIZE missing or 0)
 *  @retval NULL only on allocation failure (errno from param layer)
 * @note Built from env PARAM_SIZE and PARAM1..PARAMn; missing vars add NULL string options via param_adds
 * @note Only valid inside a component API invoked via shell (he command)
 */
param_t     execute_param( void );
/**
 * @brief get the API method name of the current execute calling context
 * @return API method name string
 * 	@retval string for succeed
 *  @retval NULL when not in a execute context
 * @note Only valid inside a component API invoked via shell (he command)
 */
const char *execute_api( void );
/**
 * @brief get the pipe file descriptor of the current execute calling context
 * @return pipe file descriptor
 * 	@retval >=0 for succeed (valid file descriptor)
 *  @retval <0 when not in a execute context or no pipe
 * @note Only valid inside a component API invoked via shell (he command)
 */
int         execute_pipe( void );



/// com structure: this structure is a handler of components
typedef struct com_st
{
	/* link kept for ABI; process cache uses cache_next hash chain */
    link_struct link;
	struct com_st *cache_next;
	/* 1: permanent process cache (com_close is no-op), like reg_attach */
	int cached;
	// reference count
	int ref;
	// component file type
#define COM_FILE_LIB     1
#define COM_FILE_EXECUTE 2
/* obj alias → one real component name (not another alias); unresolved = not listed / not usable */
#define COM_FILE_ALIAS   3
/** Fixed fd for JSON/talk reply pipe in shell-spawned COM_FILE_EXECUTE children (dup2 from pipe write end) */
#define SHELL_COM_PIPE   7
	signed char type;
	// component file
    char path[PATH_MAX];
	// handler of component
    void *handle;
	// function of set the configure of component 
    comset_t setter;
	// function of get the configure of component 
    comget_t getter;
} com_struct;
typedef com_struct *com_t;

/// Max component number
#define COMPONENT_MAX 512
/** Process-local com_open hash buckets (power of two) */
#define COM_CACHE_BUCKETS 64

/**
 * @brief build component object name for com_register (project + GAPC + name)
 * @param[in] project project or instance prefix (e.g., "routes", "land")
 * @param[in] name instance or component id (e.g., "123", "machine")
 * @param[out] buffer buffer to store object name; NULL to use internal static buffer
 * @param[in] buflen size of buffer; <=0 with NULL buffer selects the static buffer
 * @return object name string (e.g., "routes@123")
 * 	@retval string for succeed
 *  @retval NULL for error, errno will be set
 * @note Static buffer is not reentrant; copy the result before the next call if needed
 */
const char *com_name2com( const char *project, const char *name, char *buffer, int buflen );
/**
 * @brief get the name part after PROJECT_OBJECT_GAPC in a component object
 * @param[in] com component object name (e.g., "routes@123", "land@machine")
 * @return pointer to the name substring within com (e.g., "123", "machine")
 * 	@retval string for succeed
 *  @retval NULL for error, errno will be set
 * @note Returned pointer aliases into com; do not free it
 */
const char *com_com2name( const char *com );
/**
 * @brief register a component (maps object name to origin component or path)
 * @param[in] object object to register (e.g., "myproject@alias")
 * @param[in] origin component or path being aliased (e.g., "myproject@realcom", "/usr/share/skinos/myproject/mycom")
 * @param[in] type file type of path, must be 0 when origin be other compoent
 * @return operation result
 * 	@retval true for succeed
 *  @retval false for failed, errno will be set
 */
boole  com_register( const char *object, const char *origin, char type );
/**
 * @brief unregister a component
 * @param[in] object object to unregister (e.g., "myproject@alias")
 * @return operation result
 * 	@retval true for succeed
 *  @retval false for failed, errno will be set
 */
boole  com_unregister( const char *object );
/**
 * @brief get component file pathname for the given object
 * @param[in] object component object name (e.g., "land@machine")
 * @param[out] buffer buffer to store the component file path
 * @param[in] buflen size of buffer
 * @return component file type
 * 	@retval COM_FILE_LIB (1) for shared library
 * 	@retval COM_FILE_EXECUTE (2) for executable
 * 	@retval negative for not found or error, errno will be set
 * 	        (EINVAL when path probe finds no file / bad args / corrupt nested
 * 	         alias; ENOENT when alias target is missing; ENAMETOOLONG on overflow)
 * @note The actual path is stored in buffer, not returned as string pointer
 * @note There is no dedicated 0 return; callers use type<=0 or not LIB/EXECUTE
 */
char   com_path( const char *object, char *buffer, int buflen );
/**
 * @brief get all component list for project
 * @param[in] prefix prefix name (e.g., "usbdrv", "sdiodrv"), NULL for all
 * @param[in] project project name (e.g., "land", "network"), NULL for all projects
 * @return json list of components
 * 	@retval talk_t json for succeed - caller must free with talk_free()
 *  @retval NULL for error or no components, errno will be set
 */
talk_t com_list( const char *prefix, const char *project );
/**
 * @brief list all obj alias placeholders and whether the target com resolves
 * @return json map: alias → { "com":"<target>", "exist":"true"|"false" }
 * 	@retval talk_t for succeed - caller must free with talk_free()
 *  @retval NULL for error, errno will be set
 * @note Unresolved aliases are included with exist=false; com_list still hides them
 */
talk_t com_alias( void );



/**
 * @brief open a component by string path and load its handler
 * @param[in] object component path string (e.g., "land@machine")
 * @return component handler
 * 	@retval com_t for succeed
 *  @retval NULL for failed (component not found or load error), errno will be set
 * @note Stays in a process-local path hash cache (like reg_attach). Repeat opens
 *       of the same path return the same com_t. LIB is dlopen'd once; com_close
 *       does not unload cached handlers.
 * @see com_close
 */
com_t  com_open( const char *object );
/**
 * @brief search for a symbol (function) in the component's shared library
 * @param[in] com component handler returned by com_open()
 * @param[in] name symbol name to search for (e.g., "_status", "_setup")
 * @return pointer to the symbol
 * 	@retval function pointer for succeed
 *  @retval NULL for symbol not found or invalid args, errno will be set
 * @note Symbol names are prefixed with COM_API_PREFIX ("_") by convention
 * @note com==NULL or name==NULL/empty → NULL with errno=EINVAL (does not crash)
 * @note Non-LIB component → NULL with errno=ENOSYS
 */
void  *com_symbol( com_t com, const char *name );
/**
 * @brief release a component handler reference
 * @param[in] com component handler to close
 * @return none
 * @note For cached handlers from com_open this is a no-op (permanent process cache).
 * @see com_open
 */
void   com_close( com_t com );
/**
 * @brief determine whether a component exists and optionally check for a specific API
 * @param[in] com component handler returned by com_open()
 * @param[in] api API method name to check (e.g., "status"), NULL to only check component existence
 * @return existence result
 * 	@retval true for component (and API if specified) exists
 *  @retval false for not found, errno will be set
 * @note LIB: dlsym("_"+api). EXE: probe via MAIN2COM "exist" —
 *       com_exist → execute_ccall(com, NULL, "exist"); com_have → execute_scall(com, object, "exist").
 *       Peer exist only checks the API table (object unused). Only ttrue → true; tfalse/tpanic → false.
 * @see com_have to check by object string without keeping a handle open
 * @see ccall, scall to invoke an API on an object
 */
boole  com_exist( com_t com, const char *api );
boole  com_have( const char *object, const char *api );



/**
 * @brief call a component API using parameter structure
 * @param[in] objp object pointer (obj_t), must not be NULL
 * @param[in] api name of API to call on the component (e.g., "status", "setup"), must not be NULL
 * @param[in] parameter parameter structure containing call arguments
 * @return call result as talk_t or special values
 *	@retval talk_t (json) for peer succeed - must be freed with talk_free() after use
 *	@retval ttrue for peer return true (no free needed)
 *	@retval NULL for peer return NULL, errno may be set
 *	@retval tfalse for peer return false, errno may be set
 *	@retval terror for peer error, errno will be set
 *	@retval tpanic for calling error (invalid component/api), errno will be set
 * @note The return value comparison: tpanic < terror < tfalse < NULL < ttrue < json
 * @note For json return values, caller MUST call talk_free() to avoid memory leak
 * @note Example:
 * @code
 * // Using obj_t
 * obj_t obj = obj_create("land@machine");
 * talk_t result = ccall(obj, "status", NULL);
 * if (result > tpanic) {
 *     // Process result
 *     talk_free(result);
 * }
 * obj_free(obj);
 * 
 * // Using string (scall)
 * talk_t result = scall("land@machine", "status", NULL);
 * if (result > tpanic) talk_free(result);
 * @endcode
 * @see scall for string-based component specification
 * @see ccalls for printf-style parameter passing
 * @see cget for getting configuration values
 * @see cset for setting configuration values
 */
void         *ccall( obj_t objp, const char *api,        param_t parameter );
void         *scall( const char *object, const char *api, param_t parameter );

/**
 * @brief call a component API with talk_t parameter(s)
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] json talk_t for first parameter (ccallt/scallt)
 * @param[in] json2 talk_t for second parameter (ccall2t/scall2t)
 * @param[in] json string for first parameter (ccallst/scallst)
 * @return call result or tpanic
 *	@retval same as ccall/scall (see ccall documentation)
 * @note These are convenience wrappers for common parameter patterns
 * @note The talk_t parameters are NOT transferred - caller retains ownership
 * @note Variants:
 *   - ccallt/scallt: single talk_t parameter
 *   - ccall2t/scall2t: two talk_t parameters
 *   - ccallst/scallst: string + talk_t parameters
 * @see ccall for detailed return value semantics
 * @see ccalls for string-based parameter passing
 */
void         *ccallt( obj_t objp, const char *api,          talk_t json );
void         *scallt( const char *object, const char *api,    talk_t json );
void         *ccallst( obj_t objp, const char *api,         const char *json, talk_t json2 );
void         *scallst( const char *object, const char *api,  const char *json, talk_t json2 );
void         *ccall2t( obj_t objp, const char *api,         talk_t json, talk_t json2 );
void         *scall2t( const char *object, const char *api,  talk_t json, talk_t json2  );

/**
 * @brief call a component API with pointer parameters
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] pointer first pointer parameter
 * @param[in] pointer2 second pointer parameter
 * @param[in] pointer3 third pointer parameter
 * @param[in] pointer4 fourth pointer parameter
 * @return call result or tpanic
 *	@retval same as ccall/scall (see ccall documentation)
 * @note Passes up to 4 pointer parameters to the component API
 * @note Pointers are passed as-is - component API interprets their meaning
 * @warning Ensure pointers remain valid during the call
 * @see ccall for detailed return value semantics
 */
void         *ccall4p( obj_t objp, const char *api,         void *pointer, void *pointer2, void *pointer3, void *pointer4 );
void         *scall4p( const char *object, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4 );

/**
 * @brief call a component API using printf-style string parameters (variadic)
 * @param[in] com object pointer or string description for object (e.g., "land@machine")
 * @param[in] api name of API for component (e.g., "status", "setup")
 * @param[in] paramformat printf-style format string for parameters
 * @param[in] ... variadic arguments for format string
 * @return call result or tpanic
 *	@retval talk_t (json) for peer succeed - must be freed with talk_free() after use
 *	@retval ttrue for peer return true
 *	@retval NULL for peer return NULL, errno may be set
 *	@retval tfalse for peer return false, errno may be set
 *	@retval terror for peer error, errno will be set
 *	@retval tpanic for calling error, errno will be set
 * @note This function builds a param_t internally from the format string and arguments
 * @note Format string uses comma-separated values: "eth0,192.168.1.1,24"
 * @note Example:
 * @code
 * // Simple string parameter
 * talk_t result = ccalls(obj, "configure", "eth0");
 * if (result > tpanic) talk_free(result);
 * 
 * // Multiple parameters with format
 * const char *ip = "192.168.1.1";
 * const char *mask = "255.255.255.0";
 * talk_t result = ccalls(obj, "configure", "eth0,%s,%s", ip, mask);
 * if (result > tpanic) talk_free(result);
 * 
 * // Integer parameter
 * int mtu = 1500;
 * talk_t result = ccalls(obj, "set_mtu", "eth0,%d", mtu);
 * if (result > tpanic) talk_free(result);
 * @endcode
 * @see ccall2s/scall2s for two fixed string parameters (more efficient)
 * @see ccall/scall for param_t parameter
 * @see ccall for detailed return value semantics
 */
void         *ccalls( obj_t objp, const char *api,          const char *paramformat, ... );
void         *scalls( const char *object, const char *api,   const char *paramformat, ... );

/**
 * @brief call a component API with two fixed string parameters
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] option first string parameter
 * @param[in] option2 second string parameter
 * @return call result or tpanic
 *	@retval same as ccall/scall (see ccall documentation)
 * @note More efficient than ccalls when exactly 2 string parameters are needed
 * @note No format string parsing overhead
 * @see ccalls for variadic parameter passing
 * @see ccall for detailed return value semantics
 */
void         *ccall2s( obj_t objp, const char *api,         const char *option, const char *option2 );
void         *scall2s( const char *object, const char *api, const char *option, const char *option2 );

/**
 * @brief call a component API and get string return value (param_t version)
 * @param[out] buffer buffer to store the returned string
 * @param[in] buflen size of buffer
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] parameter parameter structure
 * @return string result
 *	@retval string (pointer to buffer) for component API return succeed
 *	@retval NULL for component no return or call failed, errno will be set
 * @note This is a convenience wrapper that converts talk_t return to string
 * @note The string is stored in the provided buffer, not dynamically allocated
 * @note Safe for APIs that return string values
 * @see scall_string for string-based component specification
 * @see ccalls_string for printf-style parameter passing
 */
const char   *ccall_string( char *buffer, int buflen, obj_t objp, const char *api,       param_t parameter );
const char   *scall_string( char *buffer, int buflen, const char *object, const char *api, param_t parameter );

/**
 * @brief call a component API and get string return value (talk_t parameter version)
 * @param[out] buffer buffer to store the returned string
 * @param[in] buflen size of buffer
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] json talk_t parameter
 * @return string result
 *	@retval string (pointer to buffer) for component API return succeed
 *	@retval NULL for component no return or call failed, errno will be set
 * @note Convenience wrapper for single talk_t parameter calls
 * @see ccall_string for param_t version
 * @see scallt_string for string-based component specification
 */
const char   *ccallt_string( char *buffer, int buflen, obj_t objp, const char *api,       talk_t json );
const char   *scallt_string( char *buffer, int buflen, const char *object, const char *api, talk_t json );

/**
 * @brief call a component API and get string return value (printf-style version)
 * @param[out] buffer buffer to store the returned string
 * @param[in] buflen size of buffer
 * @param[in] com object pointer or string description for object
 * @param[in] api name of API for component
 * @param[in] paramformat printf-style format string for parameters
 * @param[in] ... variadic arguments
 * @return string result
 *	@retval string (pointer to buffer) for component API return succeed
 *	@retval NULL for component no return or call failed, errno will be set
 * @note Combines printf-style parameter building with string result return
 * @note Example:
 * @code
 * char buf[256];
 * const char *result = ccalls_string(buf, sizeof(buf), obj, "get_ip", "eth0");
 * if (result) printf("IP: %s\n", result);
 * @endcode
 * @see ccall_string for param_t version
 * @see scall_string for string-based component specification
 */
const char   *ccalls_string( char *buffer, int buflen, obj_t objp, const char *api,       const char *paramformat, ... );
const char   *scalls_string( char *buffer, int buflen, const char *object, const char *api, const char *paramformat, ... );



/**
 * @brief set component configuration with json value (runtime configuration and persistent configuration)
 * @param[in] com object pointer or string description for object
 * @param[in] value json value to set
 * @param[in] attr attribute path (e.g., attr_create("wan/ip"))
 * @return operation result
 *	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note This sets runtime configuration and the runtime API must save to persistent configuration
 * @note This sets persistent configuration when runtime API not exist
 * @note The value is copied internally, caller retains ownership
 * @note Creates intermediate objects if they don't exist
 * @note Difference from dbs_save: cset can affect runtime; dbs_save persists to database
 * @note Example:
 * @code
 * // Set runtime IP address
 * obj_t obj = obj_create("network@eth0");
 * talk_t v = string2x("192.168.1.1");
 * attr_t a = attr_create("ip");
 * cset(obj, v, a);
 * obj_free(obj);
 * talk_free(v);
 * attr_free(a);
 * 
 * // Using string component path
 * talk_t v = string2x("192.168.1.1");
 * attr_t a = attr_create("ip");
 * sset("network@eth0", v, a);
 * talk_free(v);
 * attr_free(a);
 * 
 * // Variadic attribute path
 * talk_t v = string2x("192.168.1.1");
 * csets(obj, v, "%s/%s", "wan", "ip");
 * talk_free(v);
 * @endcode
 * @see cget for getting configuration values
 * @see dbs_save for persistent database storage
 * @see sset, csets, ssets for alternative interfaces
 */
boole cset( obj_t objp,           talk_t value, attr_t attr );
boole csets( obj_t objp,          talk_t value, const char *attr, ... );
boole sset( const char *object,  talk_t value, attr_t attr );
boole ssets( const char *object, talk_t value, const char *attr, ... );

/**
 * @brief set component configuration with string value (runtime configuration and persistent configuration)
 * @param[in] com object pointer or string description for object
 * @param[in] value string value to set
 * @param[in] attr attribute path
 * @return operation result
 *	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Convenience wrapper that converts string to json internally
 * @note The string is copied internally
 * @note Same as cset but accepts string instead of talk_t
 * @see cset for talk_t version
 * @see cget_string for getting string values
 */
boole cset_string( obj_t objp,             const char *value, attr_t attr );
boole csets_string( obj_t objp,            const char *value, const char *attr, ... );
boole sset_string( const char *object,  const char *value, attr_t attr );
boole ssets_string( const char *object, const char *value, const char *attr, ... );



/**
 * @brief get component configuration value as talk_t (runtime configuration or persistent configuration)
 * @param[in] com object pointer or string description for object
 * @param[in] attr attribute path (e.g., attr_create("wan/ip"))
 * @return configuration value
 *	@retval talk_t for component configure - caller must free with talk_free()
 *	@retval NULL for no configuration value
 *  	@retval tpanic for invalid options, errno will be set
 * @note This gets runtime configuration
 * @note This gets persistent configuration when runtime API not exist 
 * @note The returned talk_t is dynamically allocated, caller must free it
 * @note Difference from dbs_fetch: cget can read runtime config; dbs_fetch reads from database
 * @note Example:
 * @code
 * // Get runtime IP address
 * obj_t obj = obj_create("network@eth0");
 * attr_t a = attr_create("ip");
 * talk_t ip = cget(obj, a);
 * if (ip && ip > tpanic) {
 *     printf("IP: %s\n", x2string(ip));
 *     talk_free(ip);
 * }
 * obj_free(obj);
 * attr_free(a);
 * 
 * // Using string component path with variadic attribute
 * talk_t ip = sgets("network@eth0", "%s/%s", "wan", "ip");
 * if (ip && ip > tpanic) talk_free(ip);
 * @endcode
 * @see cset for setting configuration values
 * @see dbs_fetch for persistent database retrieval
 * @see cget_string for string return version
 */
void          *cget( obj_t objp, attr_t attr );
void          *cgets( obj_t objp, const char *attr, ... );
void          *sget( const char *object, attr_t attr );
void          *sgets( const char *object, const char *attr, ... );

/**
 * @brief get component configuration value as string (runtime configuration or persistent configuration)
 * @param[out] buffer buffer to store the string value
 * @param[in] buflen size of buffer
 * @param[in] com object pointer or string description for object
 * @param[in] attr attribute path
 * @return string result
 *	@retval string (pointer to buffer) for succeed
 *	@retval NULL for no value or error, errno will be set
 * @note Convenience wrapper that converts talk_t to string in provided buffer
 * @note No dynamic allocation - result stored in caller's buffer
 * @note Example:
 * @code
 * char buf[256];
 * obj_t obj = obj_create("network@eth0");
 * attr_t a = attr_create("ip");
 * const char *ip = cget_string(buf, sizeof(buf), obj, a);
 * if (ip) printf("IP: %s\n", ip);
 * obj_free(obj);
 * attr_free(a);
 * @endcode
 * @see cget for talk_t return version
 * @see cset_string for setting string values
 */
const char    *cget_string( char *buffer, int buflen, obj_t objp, attr_t attr );
const char    *cgets_string( char *buffer, int buflen, obj_t objp, const char *attr, ... );
const char    *sget_string( char *buffer, int buflen, const char *object, attr_t attr );
const char    *sgets_string( char *buffer, int buflen, const char *object, const char *attr, ... );



#endif   /* ----- #ifndef H_LAND_COM_H  ----- */
