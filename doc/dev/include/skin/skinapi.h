#ifndef  H_LAND_API_H
#define  H_LAND_API_H

/**
 * @file skinapi.h
 * @author dimmalex@gmail.com
 * @version 7.5
 * @date 20240903
 * @brief skinos system commom api base the ccall
 */



/**
 * @brief exit with fault message when memory allocation fails (fatal error)
 * @param[in] i exit code passed to exit()
 * @note Logs a fault message "memory oops" before exiting
 * @note This is intended for unrecoverable out-of-memory situations
 */
#define memory_exit( i )                do { default_fault("memory oops");exit( i ); }while(0)
/**
 * @brief log a fault message for format/parsing errors
 * @param[in] string the invalid format string that caused the error
 * @note Logs a fault message "format oops: <string>" via default_fault
 */
#define format_error( string )          do { default_fault("format oops: %s", string); }while(0)

/**
 * @brief list all registered feature packages (FPK)
 * @param[in] ... printf-style parameter format string and arguments passed to FPK_COM "list" API
 * @return talk_t result from FPK list API
 * 	@retval talk_t json list for succeed - caller must free with talk_free()
 * 	@retval tpanic for calling error
 * @see fpk_register, fpk_unregister
 */
#define fpk_list( ... )                 scalls( FPK_COM, "list", __VA_ARGS__ )
/**
 * @brief register a feature package
 * @param[in] ... printf-style parameter format string and arguments passed to FPK_COM "register" API
 * @return talk_t result from FPK register API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see fpk_list, fpk_unregister
 */
#define fpk_register( ... )             scalls( FPK_COM, "register", __VA_ARGS__ )
/**
 * @brief unregister a feature package
 * @param[in] ... printf-style parameter format string and arguments passed to FPK_COM "unregister" API
 * @return talk_t result from FPK unregister API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see fpk_list, fpk_register
 */
#define fpk_unregister( ... )           scalls( FPK_COM, "unregister", __VA_ARGS__ )

/**
 * @brief list all registered initialization handlers
 * @param[in] ... printf-style parameter format string and arguments passed to INIT_COM "list" API
 * @return talk_t result from init list API
 * 	@retval talk_t json list for succeed - caller must free with talk_free()
 * 	@retval tpanic for calling error
 * @see init_register
 */
#define init_list( ... )                scalls( INIT_COM, "list", __VA_ARGS__ )
/**
 * @brief register an initialization handler to be called during system startup
 * @param[in] item initialization level/priority string (e.g., "10", "20"), lower values run first
 * @param[in] call component API to call (e.g., "network@eth0.setup")
 * @return talk_t result from init register API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see init_list
 */
#define init_register( item, call )     scall2s( INIT_COM, "register", item, call )

/**
 * @brief list all registered uninitialization handlers
 * @param[in] ... printf-style parameter format string and arguments passed to INIT_COM "list" API
 * @return talk_t result from uninit list API
 * 	@retval talk_t json list for succeed - caller must free with talk_free()
 * 	@retval tpanic for calling error
 * @see uninit_register
 */
#define uninit_list( ... )              scalls( INIT_COM, "list", __VA_ARGS__ )
/**
 * @brief register an uninitialization handler to be called during system shutdown
 * @param[in] item uninitialization level/priority string (e.g., "10", "20"), lower values run first
 * @param[in] call component API to call (e.g., "network@eth0.shutdown")
 * @return talk_t result from uninit register API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see uninit_list
 */
#define uninit_register( item, call )   scall2s( UNINIT_COM, "register", item, call )

/**
 * @brief list all registered joint (event) handlers
 * @param[in] ... printf-style parameter format string and arguments passed to JOINT_COM "list" API
 * @return talk_t result from joint list API
 * 	@retval talk_t json list for succeed - caller must free with talk_free()
 * 	@retval tpanic for calling error
 * @see joint_register, joint_unregister, joint_calls, joint_callt
 */
#define joint_list( ... )               scalls( JOINT_COM, "list", __VA_ARGS__ )
/**
 * @brief register a joint (event) handler for inter-component communication
 * @param[in] item event name/identifier string (e.g., "network_up", "wan_connected")
 * @param[in] call component API to call when event fires (e.g., "firewall@rule.apply")
 * @return talk_t result from joint register API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see joint_unregister, joint_calls, joint_callt
 */
#define joint_register( item, call )    scall2s( JOINT_COM, "register", item, call )
/**
 * @brief unregister a joint (event) handler
 * @param[in] item event name/identifier string
 * @param[in] call component API to unregister (e.g., "firewall@rule.apply")
 * @return talk_t result from joint unregister API
 * 	@retval ttrue for succeed
 * 	@retval tfalse/terror for failed
 * @see joint_register
 */
#define joint_unregister( item, call )  scall2s( JOINT_COM, "unregister", item, call )
/**
 * @brief trigger a joint event with a string parameter, calling all registered handlers
 * @param[in] joint event name/identifier string (e.g., "network_up")
 * @param[in] string string parameter passed to all registered handlers
 * @return talk_t result from joint call API
 * 	@retval ttrue for succeed (all handlers called)
 * 	@retval tfalse/terror for failed
 * @see joint_callt for talk_t parameter version
 * @see joint_register to register handlers
 */
#define joint_calls( joint, string )    scall2s( JOINT_COM, "call", joint, string )
/**
 * @brief trigger a joint event with a talk_t (json) parameter, calling all registered handlers
 * @param[in] joint event name/identifier string (e.g., "network_up")
 * @param[in] json talk_t (json) parameter passed to all registered handlers
 * @return talk_t result from joint call API
 * 	@retval ttrue for succeed (all handlers called)
 * 	@retval tfalse/terror for failed
 * @see joint_calls for string parameter version
 * @see joint_register to register handlers
 */
#define joint_callt( joint, json )      scallst( JOINT_COM, "call", joint, json )

/**
 * @brief get machine runtime configuration value
 * @param[in] ... variadic attribute path arguments passed to sgets() (e.g., "name", or "%s/%s", "wan", "ip")
 * @return talk_t configuration value
 * 	@retval talk_t for succeed - caller must free with talk_free()
 * 	@retval NULL for no value
 * 	@retval tpanic for calling error
 * @note Shortcut for sgets(MACHINE_COM, ...)
 * @see machine_status
 */
#define machine_config( ... )           sgets( MACHINE_COM, __VA_ARGS__ )
/**
 * @brief get machine status information
 * @param[in] ... printf-style parameter format string and arguments passed to MACHINE_COM "status" API
 * @return talk_t status information
 * 	@retval talk_t json for succeed - caller must free with talk_free()
 * 	@retval tpanic for calling error
 * @note Shortcut for scalls(MACHINE_COM, "status", ...)
 * @see machine_config
 */
#define machine_status( ... )           scalls( MACHINE_COM, "status", __VA_ARGS__ )
/**
 * @brief restart all services (soft restart, no hardware reboot)
 * @param[in] delay delay in seconds before restart (0 for immediate)
 * @param[in] key authorization key string (empty string "" if not required)
 * @return talk_t result
 * 	@retval ttrue for restart command accepted
 * 	@retval tfalse/terror for failed
 * @note This restarts software services only, does not reboot the hardware
 * @see machine_reboot for hardware reboot
 * @see machine_default for factory reset
 */
#define machine_restart( delay, key )   scalls( MACHINE_COM, "restart", "%d,%s", delay, key?:"" )
/**
 * @brief reboot the hardware device
 * @param[in] delay delay in seconds before reboot (0 for immediate)
 * @param[in] key authorization key string (empty string "" if not required)
 * @return talk_t result
 * 	@retval ttrue for reboot command accepted
 * 	@retval tfalse/terror for failed
 * @note This performs a full hardware reboot
 * @see machine_restart for soft restart
 * @see machine_default for factory reset
 */
#define machine_reboot( delay, key )    scalls( MACHINE_COM, "reboot", "%d,%s", delay, key?:"" )
/**
 * @brief reset the device to factory default settings and reboot
 * @param[in] delay delay in seconds before reset (0 for immediate)
 * @param[in] key authorization key string (empty string "" if not required)
 * @return talk_t result
 * 	@retval ttrue for default command accepted
 * 	@retval tfalse/terror for failed
 * @warning This erases all user configuration and restores factory defaults
 * @see machine_restart for soft restart
 * @see machine_reboot for hardware reboot
 */
#define machine_default( delay, key )   scalls( MACHINE_COM, "default", "%d,%s", delay, key?:"" )



#endif   /* ----- #ifndef H_LAND_API_H  ----- */

