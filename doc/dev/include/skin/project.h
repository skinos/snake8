#ifndef H_LAND_PROJECT_H
#define H_LAND_PROJECT_H
/**
 * @file project.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief provide unified project information operation interface for the system
 */

 

#define PROJECT_INFOFILE "prj.json"
#define PROJECT_DEFAULT_VERSION "8.0.0"



/**
 * @brief scan filesystem for projects (updates cache)
 * @return json of project list
 * 		@retval json for succeed
 *          {
 *            // "name":"directory"
 *            "land":"/PRJ/land/",
 *            "arch":"/PRJ/arch/"
 *            ...
 *          }
 *  	@retval NULL for error, errno will be sets
 */
talk_t      project_scan( void );
/**
 * @brief get cached project list (use project_scan to refresh)
 * @return json of project list
 * 		@retval json for succeed
 *          {
 *            // "name":"path"
 *            "land":"/PRJ/land",
 *            "arch":"/PRJ/arch"
 *            ...
 *          }
 *  	@retval NULL for error, errno will be sets
 */
talk_t      project_list( void );
/**
 * @brief mark project cache as dirty (forces rescan on next project_list)
 * @return none
 */
void        project_dirty( void );
/**
 * @brief check the project all json format
 * @param[in] name project name
 * @param[in] prjpath project directory path (default: /PRJ)
 * @return the json format in project all legal
 * 		@retval true for valid, 
 * 		@retval false for invalid and errno will be set
 */
boole       project_check( const char *name, const char *prjpath );



/**
 * @brief get the corresponding directory for the project
 * @param[out] buffer buffer to store directory path
 * @param[in] buflen buffer size
 * @param[in] name project name
 * @return string of project directory
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_path( char *buffer, int buflen, const char *name );
#define project2path( buffer, buflen )  project_path( buffer, buflen, PROJECT_ID )
/**
 * @brief get the corresponding storage directory for the project
 * @param[out] buffer buffer to store directory path
 * @param[in] buflen buffer size
 * @param[in] name project name
 * @param[in] type storage type string (e.g., "config", "data", "log", "tmp")
 * @return string of project directory
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_storage( char *buffer, int buflen, const char *name, const char *type );
#define project2storage( buffer, buflen, type ) project_storage( buffer, buflen, PROJECT_ID, type )
/**
 * @brief get the execute pathname for project
 * @param[out] buffer buffer to store executable path
 * @param[in] buflen buffer size
 * @param[in] name project name
 * @param[in] execute executable filename
 * @return string of execute pathname
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_exe_path( char *buffer, int buflen, const char *name, const char *execute, ... );
#define exe2path( buffer, buflen, ... )  project_exe_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
#define project_ko_path project_exe_path
#define ko2path( buffer, buflen, ... )  project_exe_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
#define project_shell_path project_exe_path
#define shell2path( buffer, buflen, ... )  project_exe_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
#define project_misc_path project_exe_path
#define misc2path( buffer, buflen, ... )  project_exe_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
#define project_cfg_path project_exe_path
#define cfg2path( buffer, buflen, ... )  project_exe_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
/**
 * @brief get the variable pathname for project
 * @param[out] buffer buffer to store variable path
 * @param[in] buflen buffer size
 * @param[in] name project name
 * @param[in] variable variable filename
 * @return string of variable pathname
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_var_path( char *buffer, int buflen, const char *name, const char *variable, ... );
#define var2path( buffer, buflen, ... )  project_var_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
/**
 * @brief get the internal pathname for project
 * @param[out] buffer buffer to store internal path
 * @param[in] buflen buffer size
 * @param[in] name project name
 * @param[in] variable variable filename
 * @return string of internal pathname
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_internal_path( char *buffer, int buflen, const char *name, const char *variable, ... );
#define internal2path( buffer, buflen, ... )  project_internal_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )



/**
 * @brief add an initialization handler for a project
 * @param[in] name project name
 * @param[in] prjpath project directory path (default: /PRJ)
 * @param[in] level initialization level/priority string (e.g., "app", "app2"), lower values run first
 * @param[in] call API function to call (e.g., "network@eth0.setup")
 * @return the operation is succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, and errno code will be sets
 */
boole project_add_init( const char *name, const char *prjpath, const char *level, const char *call );
/**
 * @brief add an uninitialization handler for a project
 * @param[in] name project name
 * @param[in] prjpath project directory path (default: /PRJ)
 * @param[in] level uninitialization level/priority string
 * @param[in] call API function to call during shutdown
 * @return the operation is succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, and errno code will be sets
 */
boole project_add_uninit( const char *name, const char *prjpath, const char *level, const char *call );
/**
 * @brief add a joint (event) handler for a project
 * @param[in] name project name
 * @param[in] prjpath project directory path (default: /PRJ)
 * @param[in] level event name/identifier string
 * @param[in] call API function to call when event fires
 * @return the operation is succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, and errno code will be sets
 */
boole project_add_joint( const char *name, const char *prjpath, const char *level, const char *call );
/**
 * @brief register a component object mapping for a project
 * @param[in] name project name
 * @param[in] prjpath project directory path (default: /PRJ)
 * @param[in] object object name to register (e.g., "machine")
 * @param[in] com component path to map to (e.g., "land@machine")
 * @return the operation is succeed or failed
 *		@retval true for succeed
 *		@retval false for failed, and errno code will be sets
 */
boole project_add_object( const char *name, const char *prjpath, const char *object, const char *com );



/**
 * @brief get the internationalization (i18n) language json for a project
 * @param[in] project project name
 * @param[in] prefix language item prefix to filter (e.g., "network", "system")
 * @return talk_t json containing language key-value pairs
 *		@retval talk_t json for succeed - caller must free with talk_free()
 *		@retval NULL for failed, and errno code will be sets
 */
talk_t      project_i18n( const char *project, const char *prefix );
/**
 * @brief get a translated string from the language json
 * @param[in] lang language json returned by project_i18n()
 * @param[in] text language key to look up (e.g., "ip_address", "gateway")
 * @return translated string
 *		@retval string for succeed (pointer into lang json, do not free separately)
 *		@retval NULL for key not found
 * @note The returned string is owned by the lang json, becomes invalid after talk_free(lang)
 * @see project_i18n for obtaining the language json
 */
const char *project_i18n_get( talk_t lang, const char *text );



#endif   /* ----- #ifndef H_LAND_PROJECT_H  ----- */

