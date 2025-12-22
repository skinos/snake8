#ifndef H_LAND_SERV_H
#define H_LAND_SERV_H

/**
 * @file serv.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief service management implementation
 */



/// talk to service management
talk_t serv_call( const char *cmd, talk_t v, int timeout );



/**
 * @brief register a service, stop and delete a same name service first
 * @param[in] delay, delay some second to run
 * @param[in] com, a object or string description for object
 * @param[in] method, method name
 * @param[in] param, a pointer of parameter structure
 * @param[in] nameformat, string for service name
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srun( int delay, const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole crun( int delay, obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief register a service, stop and delete a same name service first
 * @param[in] delay, delay some second to run
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] json, a pointer of talk for first parameter
 * @param[in] json2, a pointer of talk for second parameter
 * @param[in] nameformat, string for service name
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srunt( int delay, const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole crunt( int delay, obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole srun2t( int delay, const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...  );
boole crun2t( int delay, obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief register a service, stop and delete a same name service first
 * @param[in] delay, delay some second to run
 * @param[in] name, string for service name
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] paramformat, string for parameter list( option1,option2,option3,... )
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sruns( int delay, const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cruns( int delay, const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief reset a service or start a service, register the service when service noexist
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] param, a pointer of parameter structure
 * @param[in] nameformat, string for service name
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sreset( const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole creset( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief reset a service or start a service, register the service when service noexist
 * @param[in] com, a pointer of object structure of component
 * @param[in] api, method name
 * @param[in] json, a pointer of talk for first parameter
 * @param[in] json2, a pointer of talk for second parameter
 * @param[in] nameformat, string for service name
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresett( const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole cresett( obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole sreset2t( const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...  );
boole creset2t( obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief reset a service or start a service, register the service when service noexist
 * @param[in] name, string for service name
 * @param[in] com, a object or string description for object
 * @param[in] api method name
 * @param[in] paramformat, string for parameter list( option1,option2,option3,... )
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresets( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cresets( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief start a service
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] param, a pointer of parameter structure
 * @param[in] nameformat, string for service name
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstart( const char *com, const char *api, param_t param, const char *nameformat, ...	);
boole cstart( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief start a service
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] json, a pointer of talk for first parameter
 * @param[in] json2, a pointer of talk for second parameter
 * @param[in] nameformat, string for service name
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
 * @brief start a service
 * @param[in] name, string for service name
 * @param[in] com, a object or string description for object
 * @param[in] api, method name
 * @param[in] paramformat, string for parameter list( option1,option2,option3,... )
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstarts( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cstarts( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/*
* @brief delete a service
* @param[in] nameformat, string for service name
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole sdelete( const char *nameformat, ... );
/*
* @brief stop a service
* @param[in] nameformat, string for service name
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole sstop( const char *nameformat, ... );
/*
* @brief take off a service
* @param[in] nameformat, string for service name
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole soff( const char *nameformat, ... );
/*
* @brief take off and delete a service
* @param[in] nameformat, string for service name
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
boole soffdel( const char *nameformat, ... );
/*
* @brief get a service pid
* @param[in] nameformat, string for service name
* @return service pid
*	   @retval >=0 for succeed
*	   @retval <0 for failed
*/
pid_t spid( const char *nameformat, ... );
/*
* @brief get a service infomation
* @param[in] nameformat, string for service name
* @return succeed or failed
*	   @retval true for succeed
*	   @retval false for failed
*/
talk_t sinfo( const char *nameformat, ... );
talk_t sdump( const char *nameformat, ... );
/*
* @brief get all service list
* @return service list
*	   @retval talk for succeed
*	   @retval NULL for failed
*/
talk_t slist( void );



#endif   /* ----- #ifndef H_LAND_SERV_H  ----- */

