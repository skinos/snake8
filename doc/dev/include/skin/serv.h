#ifndef H_LAND_SERV_H
#define H_LAND_SERV_H

/**
 * @file serv.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief service management client API (mcontrol + daemon.service status map)
 */

/* status int in daemon *.service map (daemon writer / sinfo·slist reader) */
#define SERVICE_ORDER_DELETE  6
#define SERVICE_ORDER_STOP    5
#define SERVICE_ORDER_OFFDEL  4
#define SERVICE_ORDER_OFF     3
#define SERVICE_ORDER_RESET   2
#define SERVICE_ORDER_START   1
#define SERVICE_ORDER_FINISH  0
#define SERVICE_ORDER_ERROR  (-1)



/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] com required object id string (e.g. "land@machine"); NULL is rejected
 * @param[in] api API method name
 * @param[in] param parameter structure
 * @param[in] nameformat printf-style service name; NULL generates NAME_RES_PREFIX"<rand>-<pid>"
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srun( int delay, const char *com, const char *api, param_t param, const char *nameformat, ... );
boole crun( int delay, obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] com required object id string (e.g. "land@machine"); NULL is rejected
 * @param[in] api API method name
 * @param[in] json talk_t parameter (slot "1")
 * @param[in] json2 second talk_t parameter (slot "2", srun2t only)
 * @param[in] nameformat printf-style service name; NULL generates NAME_RES_PREFIX"<rand>-<pid>"
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole srunt( int delay, const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole crunt( int delay, obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole srun2t( int delay, const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
boole crun2t( int delay, obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief register and start a service (stops and deletes existing service with same name first)
 * @param[in] delay delay in milliseconds before running
 * @param[in] name service name; NULL generates a random name via srun
 * @param[in] com required object id string (e.g. "land@machine"); NULL is rejected
 * @param[in] api API method name
 * @param[in] paramformat parameter format string (e.g. "opt1,opt2,opt3"); NULL means no param
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sruns( int delay, const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cruns( int delay, const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] com object id string (e.g. "land@machine"); may be NULL when only refreshing by name
 * @param[in] api API method name
 * @param[in] param parameter structure
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sreset( const char *com, const char *api, param_t param, const char *nameformat, ... );
boole creset( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] com object id string or obj_t for c*; may be NULL when only refreshing by name
 * @param[in] api API method name
 * @param[in] json talk_t parameter (slot "1")
 * @param[in] json2 second talk_t parameter (slot "2", sreset2t only)
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresett( const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole cresett( obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole sreset2t( const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
boole creset2t( obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
/**
 * @brief reset or start a service (registers if not exists, restarts if exists)
 * @param[in] name required service name; NULL is rejected
 * @param[in] com object id string (e.g. "land@machine"); may be NULL
 * @param[in] api method name
 * @param[in] paramformat parameter format string (e.g. "opt1,opt2,opt3"); NULL means no param
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sresets( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cresets( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief start a service (only starts if not already running)
 * @note Unlike run, com may be NULL: start by existing service name only (e.g. after stop).
 * @param[in] com object id string (e.g. "land@machine"); optional, NULL allowed
 * @param[in] api API method name; optional when only starting an existing registration
 * @param[in] param parameter structure
 * @param[in] nameformat printf-style service name; NULL generates NAME_RES_PREFIX"<rand>-<pid>"
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstart( const char *com, const char *api, param_t param, const char *nameformat, ... );
boole cstart( obj_t com, const char *api, param_t param, const char *nameformat, ... );
/**
 * @brief start a service (only starts if not already running)
 * @note Unlike run, com may be NULL: start by existing service name only (e.g. after stop).
 * @param[in] com object id string (e.g. "land@machine"); optional, NULL allowed
 * @param[in] api API method name
 * @param[in] json talk_t parameter (slot "1")
 * @param[in] json2 second talk_t parameter (slot "2", sstart2t only)
 * @param[in] nameformat printf-style service name; NULL generates NAME_RES_PREFIX"<rand>-<pid>"
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstartt( const char *com, const char *api, talk_t json, const char *nameformat, ... );
boole cstartt( obj_t com, const char *api, talk_t json, const char *nameformat, ... );
boole sstart2t( const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
boole cstart2t( obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ... );
boole sstartst( const char *com, const char *api, const char *json, talk_t json2, const char *nameformat, ... );
boole cstartst( obj_t com, const char *api, const char *json, talk_t json2, const char *nameformat, ... );
/**
 * @brief start a service (only starts if not already running)
 * @note Unlike run, com may be NULL: start by existing service name only (e.g. after stop).
 * @param[in] name service name; NULL generates a random name via sstart
 * @param[in] com object id string (e.g. "land@machine"); optional, NULL allowed
 * @param[in] api API method name
 * @param[in] paramformat parameter format string (e.g. "opt1,opt2,opt3"); NULL means no param
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstarts( const char *name, const char *com, const char *api, const char *paramformat, ... );
boole cstarts( const char *name, obj_t com, const char *api, const char *paramformat, ... );



/**
 * @brief delete a service
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sdelete( const char *nameformat, ... );
/**
 * @brief stop a service
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole sstop( const char *nameformat, ... );
/**
 * @brief mark a service off (do not kill; do not restart after exit; keep registration)
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole soff( const char *nameformat, ... );
/**
 * @brief mark a service offdel (do not kill; do not restart; delete registration after exit)
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole soffdel( const char *nameformat, ... );
/**
 * @brief get a service pid from the RO status map
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return service pid
 *		@retval >0 for succeed
 *		@retval <0 for failed or not running / not found
 */
pid_t spid( const char *nameformat, ... );
/**
 * @brief get a service information from the RO status map
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return talk_t with service information
 *		@retval talk_t json for succeed - caller must free with talk_free()
 *		@retval NULL if service not found
 *		@retval terror if status map path/attach failed
 */
talk_t sinfo( const char *nameformat, ... );
/**
 * @brief get a service detailed dump via daemon mcontrol
 * @param[in] nameformat required printf-style service name; NULL is rejected
 * @return talk_t with detailed service dump information
 *		@retval talk_t json for succeed - caller must free with talk_free()
 *		@retval NULL if service not found / empty dump
 *		@retval terror if daemon IPC failed (connect/alloc/call)
 *		@retval tpanic if daemon replied tpanic
 */
talk_t sdump( const char *nameformat, ... );
/**
 * @brief get all service list from the RO status map
 * @return talk_t json containing all registered services
 *		@retval talk_t json for succeed - caller must free with talk_free()
 *		@retval terror if status map path/attach failed
 */
talk_t slist( void );



#endif   /* ----- #ifndef H_LAND_SERV_H  ----- */
