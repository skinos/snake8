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
 * @brief Define main() for a standalone executable: read shell context, dispatch by API name, exit.
 * @param table Array of eapi_table_t. Must be an array identifier (e.g. static const eapi_table_t foo[]),
 *              not a pointer, so sizeof(table)/sizeof((table)[0]) yields the entry count.
 * @note Expects shell_object / shell_param / shell_api / shell_pipe; maps return like ccall peers.
 */
#define MAIN2API( table ) \
int main( int argc, const char **argv ) \
{ \
	int exit_code; \
	int pipe_fd; \
	talk_t ret; \
	obj_t this; \
	param_t param; \
	const char *api; \
	size_t i; \
	(void)argc; \
	(void)argv; \
	this = shell_object(); \
	param = shell_param(); \
	api = shell_api(); \
	pipe_fd = shell_pipe(); \
	ret = tfalse; \
	if ( api != NULL ) \
	{ \
		for ( i = 0; i < sizeof( table ) / sizeof( (table)[0] ); i++ ) \
		{ \
			if ( 0 == strcmp( api, (table)[i].name ) ) \
			{ \
				ret = (talk_t)( (table)[i].fn( this, param ) ); \
				break; \
			} \
		} \
	} \
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
	else \
	{ \
		exit_code = EXIT_terror; \
	} \
	if ( pipe_fd > 0 ) \
	{ \
		talk2fd( pipe_fd, ret, errno ); \
		close( pipe_fd ); \
	} \
	param_free( param ); \
	obj_free( this ); \
	talk_free( ret ); \
	return exit_code; \
}

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
 * @brief get the object pointer of the current shell calling context
 * @return obj_t of the current shell context
 * 	@retval obj_t for succeed
 *  @retval NULL when not in a shell context
 * @note Only valid inside a component API invoked via shell (he command)
 */
obj_t       shell_object( void );
/**
 * @brief get the parameter structure of the current shell calling context
 * @return param_t of the current shell context
 * 	@retval param_t for succeed
 *  @retval NULL when not in a shell context or no parameters
 * @note Only valid inside a component API invoked via shell (he command)
 */
param_t     shell_param( void );
/**
 * @brief get the API method name of the current shell calling context
 * @return API method name string
 * 	@retval string for succeed
 *  @retval NULL when not in a shell context
 * @note Only valid inside a component API invoked via shell (he command)
 */
const char *shell_api( void );
/**
 * @brief get the pipe file descriptor of the current shell calling context
 * @return pipe file descriptor
 * 	@retval >=0 for succeed (valid file descriptor)
 *  @retval <0 when not in a shell context or no pipe
 * @note Only valid inside a component API invoked via shell (he command)
 */
int         shell_pipe( void );



/// com structure: this structure is a handler of components
typedef struct com_st
{
	// link node for add to link
    link_struct link;
	// reference count
	int ref;
	// component file type
#define COM_FILE_KO      1
#define COM_FILE_LIB     2
#define COM_FILE_SHELL   3
#define COM_FILE_EXECUTE 4
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
	// function of save the database of component 
    comsave_t saver;
	// function of fetch the database of component 
    comfetch_t fetcher;
} com_struct;
typedef com_struct *com_t;

/**
 * @brief get all component list for project registered in the system
 * @return json list of project components
 * 	@retval talk_t json for succeed - caller must free with talk_free()
 *  @retval NULL for error, errno will be set
 */
talk_t com_project_list( void );
/**
 * @brief register a component alias (maps target name to origin component)
 * @param[in] target target component path to register (e.g., "myproject@alias")
 * @param[in] origin origin component path being aliased (e.g., "myproject@realcom")
 * @return operation result
 * 	@retval true for succeed
 *  @retval false for failed, errno will be set
 */
boole  com_register( const char *target, const char *origin );
/**
 * @brief unregister a component alias
 * @param[in] target target component path to unregister (e.g., "myproject@alias")
 * @return operation result
 * 	@retval true for succeed
 *  @retval false for failed, errno will be set
 */
boole  com_unregister( const char *target );
/**
 * @brief get the registered component alias list
 * @return json list of registered component aliases
 * 	@retval talk_t json for succeed - caller must free with talk_free()
 *  @retval NULL for error or empty, errno will be set
 */
talk_t com_register_list( void );
/**
 * @brief get component file pathname for the given object
 * @param[in] obj object pointer identifying the component
 * @param[out] buffer buffer to store the component file path
 * @param[in] buflen size of buffer
 * @return component file type
 * 	@retval COM_FILE_KO (1) for kernel module (.ko)
 * 	@retval COM_FILE_LIB (2) for shared library (.com)
 * 	@retval COM_FILE_SHELL (3) for shell script (.ash)
 * 	@retval COM_FILE_EXECUTE (4) for executable (no extension)
 * 	@retval 0 for component not found
 * 	@retval negative for error (invalid arguments), errno will be set
 * @note The actual path is stored in buffer, not returned as string pointer
 */
char   com_path( obj_t obj, char *buffer, int buflen );
/**
 * @brief get all component list for a specified project
 * @param[in] project project name (e.g., "land", "network"), NULL for all projects
 * @return json list of components
 * 	@retval talk_t json for succeed - caller must free with talk_free()
 *  @retval NULL for error or no components, errno will be set
 */
talk_t com_list( const char *project );
/**
 * @brief open a component by object pointer and load its handler
 * @param[in] obj object pointer identifying the component (e.g., obj_create("land@machine"))
 * @return component handler
 * 	@retval com_t for succeed
 *  @retval NULL for failed (component not found or load error), errno will be set
 * @note Must call com_close() to release the handler when done
 * @see com_sopen for string-based component specification
 * @see com_close to release the component handler
 */
com_t  com_open( obj_t obj );
/**
 * @brief open a component by string path and load its handler
 * @param[in] com component path string (e.g., "land@machine")
 * @return component handler
 * 	@retval com_t for succeed
 *  @retval NULL for failed (component not found or load error), errno will be set
 * @note Must call com_close() to release the handler when done
 * @see com_open for obj_t-based component specification
 * @see com_close to release the component handler
 */
com_t  com_sopen( const char *com );
/**
 * @brief search for a symbol (function) in the component's shared library
 * @param[in] com component handler returned by com_open()/com_sopen()
 * @param[in] name symbol name to search for (e.g., "_status", "_setup")
 * @return pointer to the symbol
 * 	@retval function pointer for succeed
 *  @retval NULL for symbol not found, errno will be set
 * @note Symbol names are prefixed with COM_API_PREFIX ("_") by convention
 */
void  *com_symbol( com_t com, const char *name );
/**
 * @brief close a component handler and release its resources
 * @param[in] com component handler to close
 * @return none
 * @note Decrements reference count; actual unload occurs when count reaches 0
 * @see com_open, com_sopen
 */
void   com_close( com_t com );
/**
 * @brief determine whether a component exists and optionally check for a specific API
 * @param[in] obj object pointer identifying the component
 * @param[in] api API method name to check (e.g., "status"), NULL to only check component existence
 * @return existence result
 * 	@retval true for component (and API if specified) exists
 *  @retval false for not found, errno will be set
 * @see com_sexist for string-based component specification
 */
boole  com_exist( obj_t obj, const char *api );
/**
 * @brief determine whether a component exists and optionally check for a specific API (string version)
 * @param[in] com component path string (e.g., "land@machine")
 * @param[in] api API method name to check (e.g., "status"), NULL to only check component existence
 * @return existence result
 * 	@retval true for component (and API if specified) exists
 *  @retval false for not found, errno will be set
 * @see com_exist for obj_t-based component specification
 */
boole  com_sexist( const char *com, const char *api );



/**
 * @brief call a component API using parameter structure
 * @param[in] com object pointer (obj_t) or string description (e.g., "land@machine")
 * @param[in] api name of API to call on the component (e.g., "status", "setup")
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
void         *ccall( obj_t com, const char *api,        param_t parameter );
void         *scall( const char *com, const char *api, param_t parameter );

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
void         *ccallt( obj_t com, const char *api,          talk_t json );
void         *scallt( const char *com, const char *api,    talk_t json );
void         *ccallst( obj_t com, const char *api,         const char *json, talk_t json2 );
void         *scallst( const char *com, const char *api,  const char *json, talk_t json2 );
void         *ccall2t( obj_t com, const char *api,         talk_t json, talk_t json2 );
void         *scall2t( const char *com, const char *api,  talk_t json, talk_t json2  );

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
void         *ccall4p( obj_t obj, const char *api,         void *pointer, void *pointer2, void *pointer3, void *pointer4 );
void         *scall4p( const char *obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4 );

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
void         *ccalls( obj_t com, const char *api,          const char *paramformat, ... );
void         *scalls( const char *com, const char *api,   const char *paramformat, ... );

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
void         *ccall2s( obj_t com, const char *api,         const char *option, const char *option2 );
void         *scall2s( const char *com, const char *api, const char *option, const char *option2 );

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
const char   *ccall_string( char *buffer, int buflen, obj_t com, const char *api,       param_t parameter );
const char   *scall_string( char *buffer, int buflen, const char *com, const char *api, param_t parameter );

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
const char   *ccallt_string( char *buffer, int buflen, obj_t com, const char *api,       talk_t json );
const char   *scallt_string( char *buffer, int buflen, const char *com, const char *api, talk_t json );

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
const char   *ccalls_string( char *buffer, int buflen, obj_t com, const char *api,       const char *paramformat, ... );
const char   *scalls_string( char *buffer, int buflen, const char *com, const char *api, const char *paramformat, ... );



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
 * @note Difference from csave: cset can affects runtime, csave persists to database
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
 * @see csave for persistent database storage
 * @see sset, csets, ssets for alternative interfaces
 */
boole cset( obj_t com,          talk_t value, attr_t attr );
boole csets( obj_t com,         talk_t value, const char *attr, ... );
boole sset( const char *com,   talk_t value, attr_t attr );
boole ssets( const char *com, talk_t value, const char *attr, ... );

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
boole cset_string( obj_t com,            const char *value, attr_t attr );
boole csets_string( obj_t com,           const char *value, const char *attr, ... );
boole sset_string( const char *com,   const char *value, attr_t attr );
boole ssets_string( const char *com, const char *value, const char *attr, ... );



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
 * @note Difference from cfetch: cget can reads runtime config, cfetch reads from database
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
 * @see cfetch for persistent database retrieval
 * @see cget_string for string return version
 */
void          *cget( obj_t com, attr_t attr );
void          *cgets( obj_t com, const char *attr, ... );
void          *sget( const char *com, attr_t attr );
void          *sgets( const char *com, const char *attr, ... );

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
const char    *cget_string( char *buffer, int buflen, obj_t com, attr_t attr );
const char    *cgets_string( char *buffer, int buflen, obj_t com, const char *attr, ... );
const char    *sget_string( char *buffer, int buflen, const char *com, attr_t attr );
const char    *sgets_string( char *buffer, int buflen, const char *com, const char *attr, ... );



/**
 * @brief save component database with json value (persistent storage)
 * @param[in] com object pointer or string description for object
 * @param[in] fa file path or namespace for the database
 * @param[in] value json value to save
 * @param[in] attr attribute path within the database
 * @return operation result
 *	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note This saves to persistent database (unlike cset which can runtime)
 * @note The value is copied internally, caller retains ownership
 * @note fa parameter specifies the database file/namespace
 * @note Difference from cset: csave persists to disk, cset can runtime
 * @note Example:
 * @code
 * // Save configuration to persistent database
 * obj_t obj = obj_create("network@eth0");
 * attr_t f = attr_create("config");
 * talk_t v = string2x("192.168.1.1";
 * attr_t a = attr_create("ip");
 * csave(obj, f, v, a);
 * obj_free(obj);
 * attr_free(f);
 * talk_free(v);
 * attr_free(a);
 * 
 * // Using string paths
 * talk_t v = string2x("192.168.1.1";
 * attr_t a = attr_create("ip"); 
 * ssave("network@eth0", "config", v, a);
 * talk_free(v);
 * attr_free(a); 
 * @endcode
 * @see cfetch for retrieving from database
 * @see cset for runtime configuration
 * @see ssave, csaves, ssaves for alternative interfaces
 */
boole csave( obj_t com, attr_t fa,          talk_t value, attr_t attr );
boole csaves( obj_t com, attr_t fa,         talk_t value, const char *attr, ... );
boole ssave( const char *com, const char *fa,   talk_t value, attr_t attr );
boole ssaves( const char *com, const char *fa, talk_t value, const char *attr, ... );

/**
 * @brief save component database with string value (persistent storage)
 * @param[in] com object pointer or string description for object
 * @param[in] fa file path or namespace for the database
 * @param[in] value string value to save
 * @param[in] attr attribute path within the database
 * @return operation result
 *	@retval true for succeed
 *  	@retval false for failed, errno will be set
 * @note Convenience wrapper that converts string to json internally
 * @note The string is copied internally
 * @note Same as csave but accepts string instead of talk_t
 * @see csave for talk_t version
 * @see cfetch_string for retrieving string values
 */
boole csave_string( obj_t com, attr_t fa,            const char *value, attr_t attr );
boole csaves_string( obj_t com, attr_t fa,           const char *value, const char *attr, ... );
boole ssave_string( const char *com, const char *fa,   const char *value, attr_t attr );
boole ssaves_string( const char *com, const char *fa, const char *value, const char *attr, ... );



/**
 * @brief fetch component database value as talk_t (persistent storage)
 * @param[in] com object pointer or string description for object
 * @param[in] fa file path or namespace for the database
 * @param[in] attr attribute path within the database
 * @return database value
 *	@retval talk_t for component database value - caller must free with talk_free()
 *	@retval NULL for no value in database
 *  	@retval tpanic for invalid options, errno will be set
 * @note This reads from persistent database (unlike cget which reads runtime config)
 * @note The returned talk_t is dynamically allocated, caller must free it
 * @note fa parameter specifies the database file/namespace
 * @note Difference from cget: cfetch reads from persistent storage, cget reads runtime
 * @note Example:
 * @code
 * // Read configuration from persistent database
 * obj_t obj = obj_create("network@eth0");
 * attr_t f = attr_create("config");
 * attr_t a = attr_create("ip");
 * talk_t ip = cfetch(obj, f, a);
 * if (ip && ip > tpanic) {
 *     printf("Saved IP: %s\n", x2string(ip));
 *     talk_free(ip);
 * }
 * obj_free(obj);
 * attr_free(f);
 * attr_free(a);
 * 
 * // Using string paths with variadic attribute
 * talk_t ip = sfetchs("network@eth0", "config", "%s/%s", "wan", "ip");
 * if (ip && ip > tpanic) talk_free(ip);
 * @endcode
 * @see csave for saving to database
 * @see cget for runtime configuration retrieval
 * @see cfetch_string for string return version
 */
void          *cfetch( obj_t com, attr_t fa, attr_t attr );
void          *cfetchs( obj_t com, attr_t fa, const char *attr, ... );
void          *sfetch( const char *com, const char *fa, attr_t attr );
void          *sfetchs( const char *com, const char *fa, const char *attr, ... );

/**
 * @brief fetch component database value as string (persistent storage)
 * @param[out] buffer buffer to store the string value
 * @param[in] buflen size of buffer
 * @param[in] com object pointer or string description for object
 * @param[in] fa file path or namespace for the database
 * @param[in] attr attribute path within the database
 * @return string result
 *	@retval string (pointer to buffer) for succeed
 *	@retval NULL for no value or error, errno will be set
 * @note Convenience wrapper that converts talk_t to string in provided buffer
 * @note No dynamic allocation - result stored in caller's buffer
 * @note Example:
 * @code
 * char buf[256];
 * obj_t obj = obj_create("network@eth0");
 * attr_t f = attr_create("config");
 * attr_t a = attr_create("ip");
 * const char *ip = cfetch_string(buf, sizeof(buf), obj, f, a);
 * if (ip) printf("Saved IP: %s\n", ip);
 * obj_free(obj);
 * attr_free(f);
 * attr_free(a); 
 * @endcode
 * @see cfetch for talk_t return version
 * @see csave_string for saving string values
 */
const char    *cfetch_string( char *buffer, int buflen, obj_t com, attr_t fa, attr_t attr );
const char    *cfetchs_string( char *buffer, int buflen, obj_t com, attr_t fa, const char *attr, ... );
const char    *sfetch_string( char *buffer, int buflen, const char *com, const char *fa, attr_t attr );
const char    *sfetchs_string( char *buffer, int buflen, const char *com, const char *fa, const char *attr, ... );



#endif   /* ----- #ifndef H_LAND_COM_H  ----- */
