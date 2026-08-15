#ifndef H_LAND_SERV_H
#define H_LAND_SERV_H

/**
 * @file serv.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief service management implementation
 */



/**
 * @brief send a command to the service management daemon and get response
 * @param[in] cmd service management command string (e.g., "list", "info", "stop")
 * @param[in] v talk_t parameter for the command (ownership transferred, will be freed internally)
 * @param[in] timeout timeout in seconds for waiting response
 * @return talk_t response from service management daemon
 * 		@retval talk_t json for succeed - caller must free with talk_free()
 * 		@retval ttrue for succeed with no data
 * 		@retval terror for invalid arguments (NULL cmd), errno will be set
 * 		@retval tpanic for timeout or communication error, errno will be set
 */
talk_t serv_call( const char *cmd, talk_t v, int timeout );



/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] param parameter structure
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srun( int delay, const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole crun( int delay, obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] json talk_t parameter
 * @param[in] json2 second talk_t parameter
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srunt( int delay, const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole crunt( int delay, obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole srun2t( int delay, const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...  );
boole crun2t( int delay, obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] name service name
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] paramformat parameter format string (e.g., "opt1,opt2,opt3")
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sruns( int delay, const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cruns( int delay, const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] param parameter structure
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sreset( const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole creset( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] com object pointer
 * @param[in] api API method name
 * @param[in] json talk_t parameter
 * @param[in] json2 second talk_t parameter
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresett( const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole cresett( obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole sreset2t( const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...  );
boole creset2t( obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] name service name
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api method name
 * @param[in] paramformat parameter format string (e.g., "opt1,opt2,opt3")
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresets( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cresets( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief start a service (only starts if not already running)
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] param parameter structure
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstart( const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole cstart( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief start a service (only starts if not already running)
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] json talk_t parameter
 * @param[in] json2 second talk_t parameter
 * @param[in] nameformat service name format string
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstartt( const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole cstartt( obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole sstart2t( const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...  );
boole cstart2t( obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
boole sstartst( const char *com, const char *api, const char *json, talk_t json2, const char *nameformat, ...  );
boole cstartst( obj_t com, const char *api, const char *json, talk_t json2, const char *nameformat, ...  );
/**
 * @brief start a service (only starts if not already running)
 * @param[in] name service name
 * @param[in] com object pointer or string (e.g., "land@machine")
 * @param[in] api API method name
 * @param[in] paramformat parameter format string (e.g., "opt1,opt2,opt3")
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstarts( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cstarts( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
* @brief delete a service
* @param[in] nameformat service name format string
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole sdelete( const char *nameformat, ... );
/**
* @brief stop a service
* @param[in] nameformat service name format string
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole sstop( const char *nameformat, ... );
/**
* @brief mark a service off (do not kill; do not restart after exit; keep registration)
* @param[in] nameformat service name format string
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole soff( const char *nameformat, ... );
/**
* @brief mark a service offdel (do not kill; do not restart; delete registration after exit)
* @param[in] nameformat service name format string
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole soffdel( const char *nameformat, ... );
/**
* @brief get a service pid
* @param[in] nameformat service name format string
* @return service pid
*	   @retval >=0 for succeed
*	   @retval <0 for failed
*/
pid_t spid( const char *nameformat, ... );
/**
* @brief get a service information
* @param[in] nameformat service name format string
* @return talk_t with service information
*	   @retval talk_t json for succeed - caller must free with talk_free()
*	   @retval NULL for failed or service not found
*/
talk_t sinfo( const char *nameformat, ... );
/**
 * @brief get a service detailed dump information
 * @param[in] nameformat service name format string
 * @return talk_t with detailed service dump information
 *	   @retval talk_t json for succeed - caller must free with talk_free()
 *	   @retval NULL for failed or service not found
 */
talk_t sdump( const char *nameformat, ... );
/**
* @brief get all service list
* @return talk_t json containing all registered services
*	   @retval talk_t json for succeed - caller must free with talk_free()
*	   @retval NULL for failed or no services
*/
talk_t slist( void );



#endif   /* ----- #ifndef H_LAND_SERV_H  ----- */

