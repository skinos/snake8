#ifndef H_LAND_PROJECT_H
#define H_LAND_PROJECT_H
/**
 * @file link.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief provide unified project information operation interface for the system
 */

 

#define PROJECT_INFOFILE "prj.json"
#define PROJECT_DEFAULT_VERSION "8.0.0"



/**
 * @brief scan the list of project in the system
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
 * @brief list all of project in the system
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
 * @brief set project list dirty, scanning at project_list() will be call
 * @return none
 */
void        project_dirty( void );
/**
 * @brief check the proejct all json format
 * @param[in] name, project name
 * @param[in] prjpath, specifies the directory of /prj, default is /prj
 * @return the json format in project all legal
 		$retval true for vaild, 
 		@retval false for invaild and errno will be set
 */
boole       project_check( const char *name, const char *prjpath );



/**
 * @brief get the corresponding directory for the project
 * @param[out] buffer, buffer will be saved of project directory
 * @param[in] buflen, size of buffer
 * @param[in] name, project name
 * @return string of project directory
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_path( char *buffer, int buflen, const char *name );
#define project2path( buffer, buflen )  project_path( buffer, buflen, PROJECT_ID )
/**
 * @brief get the corresponding storage directory for the project
 * @param[out] buffer, buffer will be saved of project directory
 * @param[in] buflen, size of buffer
 * @param[in] name, project name
 * @return string of project directory
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_storage( char *buffer, int buflen, const char *name, const char *type );
#define project2storage( buffer, buflen, type ) project_storage( buffer, buflen, PROJECT_ID, type )
/**
 * @brief get the execute pathname for project
 * @param[out] buffer, buffer will be saved of execute pathname
 * @param[in] buflen, size of buffer
 * @param[in] name, project name
 * @param[in] execute, execute file
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
 * @param[out] buffer, buffer will be saved of variable pathname
 * @param[in] buflen, size of buffer
 * @param[in] name, project name
 * @param[in] variable, variable file
 * @return string of execute pathname
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_var_path( char *buffer, int buflen, const char *name, const char *variable, ... );
#define var2path( buffer, buflen, ... )  project_var_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )
/**
 * @brief get the interval pathname for project
 * @param[out] buffer, buffer will be saved of interval pathname
 * @param[in] buflen, size of buffer
 * @param[in] name, project name
 * @param[in] variable, variable file
 * @return string of execute pathname
 * 		@retval string for succeed
 *  	@retval NULL for error, errno will be set
 */
const char *project_internal_path( char *buffer, int buflen, const char *name, const char *variable, ... );
#define internal2path( buffer, buflen, ... )  project_internal_path( buffer, buflen, PROJECT_ID, __VA_ARGS__ )



/**
 * @brief add the init/uninit/event operation
 * @param[out] name, project name
 * @param[in] prjpath, specifies the directory of /prj, default is /prj
 * @param[in] level, init level or uninit level or event name
 * @param[in] call, call a API
 * @return the operation is succeed or failed
 *		@retval ttrue for succeed
 *		@retval tfalse for failed, and errno code will be sets
 */
boole project_add_init( const char *name, const char *prjpath, const char *level, const char *call );
boole project_add_uninit( const char *name, const char *prjpath, const char *level, const char *call );
boole project_add_joint( const char *name, const char *prjpath, const char *level, const char *call );
boole project_add_object( const char *name, const char *prjpath, const char *object, const char *com );



/**
 * @brief get the language json
 * @param[out] project, project name
 * @param[in] prefix, language item
 * @return a talk for language json
 *		@retval ttrue for succeed
 *		@retval tfalse for failed, and errno code will be sets
 */
talk_t      project_i18n( const char *project, const char *prefix );
const char *project_i18n_get( talk_t lang, const char *text );



#endif   /* ----- #ifndef H_LAND_PROJECT_H  ----- */

