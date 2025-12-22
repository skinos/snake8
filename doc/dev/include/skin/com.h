#ifndef H_LAND_COM_H
#define H_LAND_COM_H

/**
 * @file com.h
 * @author dimmalex@gmail.com
 * @version 7.5
 * @date 20240903
 * @brief component call implementation
 * @details implementation communication to other component function use talk structure or parameter structure
 */



/// function type of get the configure of component
typedef talk_t (*comget_t)( obj_t, attr_t );
/// function type of set the configure of component
typedef boole  (*comset_t)( obj_t, talk_t, attr_t );
/// function type of get the configure of component
typedef talk_t (*comfetch_t)( obj_t, attr_t, attr_t );
/// function type of set the configure of component
typedef boole  (*comsave_t)( obj_t, attr_t, talk_t, attr_t );
/// universal api function type of component
typedef void*  (*comapi_t)( obj_t, param_t );
#define COM_API_PREFIX "_"
#define COM_TALK_API( func, obj, param )   talk_t _##func( obj, param )
#define COM_BOOLE_API( func, obj, param )  boole_t _##func( obj, param )
#define COM_GET_API( obj, attr )           talk_t _get( obj, attr )
#define COM_SET_API( obj, v, attr )        boole _set( obj, v, attr )
obj_t       shell_object( void );
param_t     shell_param( void );
const char *shell_api( void );
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

/// get the project component list
talk_t com_project_list( void );
/// register a component
boole  com_register( const char *target, const char *origin );
/// unregister a component
boole  com_unregister( const char *target );
/// get the register component list
talk_t com_register_list( void );
/// get component file pathname
char   com_path( obj_t obj, char *buffer, int buflen );
/// get all component list
talk_t com_list( const char *project );
/// open a component
com_t  com_open( obj_t obj );
com_t  com_sopen( const char *com );
/// search the symbol
void  *com_symbol( com_t com, const char *name );
/// close a component
void   com_close( com_t com );
/// determine whether a component exists
boole  com_exist( obj_t obj, const char *api );
boole  com_sexist( const char *com, const char *api );



/**
 * @brief call a component API
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] parameter, a pointer of parameter structure
 * @return call result or tpanic
 *		@retval json for peer succeed, return json must free after used
 *		@retval ttrue for peer return
 *		@retval NULL for peer return, the errno code may be sets
 *		@retval tfalse for peer return, the errno code may be sets
 *  	@retval terror for peer got wrong, the errno code will be sets
 *  	@retval tpanic for calling error, and errno code will be sets
 */
void         *ccall( obj_t com, const char *api,        param_t parameter );
void         *scall( const char *com, const char *api, param_t parameter );
/**
 * @brief call a component API use various type of parameter
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] json, a pointer of talk structure for first option
 * @param[in] json2, a pointer of talk structure for second option
 * @param[in] pointer, a pointer for first option
 * @param[in] pointer2, a pointer for second option
 * @param[in] pointer3, a pointer for third option
 * @param[in] pointe4r, a pointer for fourth option
 * @return call result or tpanic
 * 		@retval same the ccall/scall
 */
void         *ccallt( obj_t com, const char *api,          talk_t json );
void         *scallt( const char *com, const char *api,    talk_t json );
void         *ccallst( obj_t com, const char *api,         const char *json, talk_t json2 );
void         *scallst( const char *com, const char *api,  const char *json, talk_t json2 );
void         *ccall2t( obj_t com, const char *api,         talk_t json, talk_t json2 );
void         *scall2t( const char *com, const char *api,  talk_t json, talk_t json2  );
void         *ccall4p( obj_t obj, const char *api,         void *pointer, void *pointer2, void *pointer3, void *pointer4 );
void         *scall4p( const char *obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4 );
/**
 * @brief call a component API use string type of parameter
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] parameter, string format parameter
 * @return call result or tpanic
 * 		@retval same the ccall/scall
 */
void         *ccalls( obj_t com, const char *api,          const char *paramformat, ... );
void         *scalls( const char *com, const char *api,   const char *paramformat, ... );
void         *ccall2s( obj_t com, const char *api,         const char *option, const char *option2 );
void         *scall2s( const char *com, const char *api, const char *option, const char *option2 );
/**
 * @brief call a component API, get the string return
 * @param[out] buffer, buffer the string will be store here
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] parameter, a pointer of parameter structure
 * @return call result or NULL
 * 		@retval string for component API return succeed
 * 		@retval NULL for component none return
 * 		@retval NULL for call failed, and errno code will be sets
 */
const char   *ccall_string( char *buffer, int buflen, obj_t com, const char *api,       param_t parameter );
const char   *scall_string( char *buffer, int buflen, const char *com, const char *api, param_t parameter );
/**
 * @brief call a component API, get the string return
 * @param[out] buffer, buffer the string will be store here
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] json, a pointer of talk for first parameter
 * @return call result or NULL
 * 		@retval string for component API return succeed
 * 		@retval NULL for component none return
 * 		@retval NULL for call failed, and errno code will be sets
 */
const char   *ccallt_string( char *buffer, int buflen, obj_t com, const char *api,       talk_t json );
const char   *scallt_string( char *buffer, int buflen, const char *com, const char *api, talk_t json );
/**
 * @brief call a component API, get the string return
 * @param[out] buffer, buffer the string will be store here
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] api, name of API for component
 * @param[in] parameter, string format parameter
 * @return call result or NULL
 * 		@retval string for component API return succeed
 * 		@retval NULL for component none return
 * 		@retval NULL for call failed, and errno code will be sets
 */
const char   *ccalls_string( char *buffer, int buflen, obj_t com, const char *api,       const char *paramformat, ... );
const char   *scalls_string( char *buffer, int buflen, const char *com, const char *api, const char *paramformat, ... );



/**
 * @brief set component configure with json value
 * @param[in] com, a object pointer or string description for object
 * @param[in] value, a json
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole cset( obj_t com,          talk_t value, attr_t attr );
boole csets( obj_t com,         talk_t value, const char *attr, ... );
boole sset( const char *com,   talk_t value, attr_t attr );
boole ssets( const char *com, talk_t value, const char *attr, ... );
/**
 * @brief set a component configure with string value
 * @param[in] com, a object pointer or string description for object
 * @param[in] value, string
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole cset_string( obj_t com,            const char *value, attr_t attr );
boole csets_string( obj_t com,           const char *value, const char *attr, ... );
boole sset_string( const char *com,   const char *value, attr_t attr );
boole ssets_string( const char *com, const char *value, const char *attr, ... );



/**
 * @brief get a component configure value in json(dynamic allocation)
 * @param[in] com, a object pointer or string description for object
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the component configure
 * 		@retval talk for component configure
 * 		@retval NULL for none component configure
 *  	@retval tpanic for invaild options, the errno code will be sets
 */
void          *cget( obj_t com, attr_t attr );
void          *cgets( obj_t com, const char *attr, ... );
void          *sget( const char *com, attr_t attr );
void          *sgets( const char *com, const char *attr, ... );
/**
 * @brief get a component configure value in string
 * @param[out] buffer, buffer the string will be store here
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the component configure
 * 		@retval string for get component configure succeed
 * 		@retval NULL for none component configure
 * 		@retval NULL for failed, and errno code will be sets
 */
const char    *cget_string( char *buffer, int buflen, obj_t com, attr_t attr );
const char    *cgets_string( char *buffer, int buflen, obj_t com, const char *attr, ... );
const char    *sget_string( char *buffer, int buflen, const char *com, attr_t attr );
const char    *sgets_string( char *buffer, int buflen, const char *com, const char *attr, ... );



/**
 * @brief save component database with json value
 * @param[in] com, a object pointer or string description for object
 * @param[in] fa, file path
 * @param[in] value, a json
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole csave( obj_t com, attr_t fa,          talk_t value, attr_t attr );
boole csaves( obj_t com, attr_t fa,         talk_t value, const char *attr, ... );
boole ssave( const char *com, const char *fa,   talk_t value, attr_t attr );
boole ssaves( const char *com, const char *fa, talk_t value, const char *attr, ... );
/**
 * @brief save a component database with string value
 * @param[in] com, a object pointer or string description for object
 * @param[in] fa, file path
 * @param[in] value, string
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the operation is succeed or failed
 * 		@retval true for succeed
 *  	@retval false for failed, and errno code will be sets
 */
boole csave_string( obj_t com, attr_t fa,            const char *value, attr_t attr );
boole csaves_string( obj_t com, attr_t fa,           const char *value, const char *attr, ... );
boole ssave_string( const char *com, const char *fa,   const char *value, attr_t attr );
boole ssaves_string( const char *com, const char *fa, const char *value, const char *attr, ... );



/**
 * @brief fetch a component database value in json(dynamic allocation)
 * @param[in] com, a object pointer or string description for object
 * @param[in] fa, file path
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the component configure
 * 		@retval talk for component configure
 * 		@retval NULL for none component configure
 *  	@retval tpanic for invaild options, the errno code will be sets
 */
void          *cfetch( obj_t com, attr_t fa, attr_t attr );
void          *cfetchs( obj_t com, attr_t fa, const char *attr, ... );
void          *sfetch( const char *com, const char *fa, attr_t attr );
void          *sfetchs( const char *com, const char *fa, const char *attr, ... );
/**
 * @brief fetch a component database value in string
 * @param[out] buffer, buffer the string will be store here
 * @param[in] buflen, size of buffer
 * @param[in] com, a object pointer or string description for object
 * @param[in] fa, file path
 * @param[in] attr, a attribute or string for description of which attribute
 * @return return the component configure
 * 		@retval string for get component configure succeed
 * 		@retval NULL for none component configure
 * 		@retval NULL for failed, and errno code will be sets
 */
const char    *cfetch_string( char *buffer, int buflen, obj_t com, attr_t fa, attr_t attr );
const char    *cfetchs_string( char *buffer, int buflen, obj_t com, attr_t fa, const char *attr, ... );
const char    *sfetch_string( char *buffer, int buflen, const char *com, const char *fa, attr_t attr );
const char    *sfetchs_string( char *buffer, int buflen, const char *com, const char *fa, const char *attr, ... );



#endif   /* ----- #ifndef H_LAND_COM_H  ----- */

