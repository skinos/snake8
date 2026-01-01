#ifndef  H_LAND_API_H
#define  H_LAND_API_H

/**
 * @file skinapi.h
 * @author dimmalex@gmail.com
 * @version 7.5
 * @date 20240903
 * @brief skinos system commom api base the ccall
 */



/// exit when memory lack
#define memory_exit( i )                do { default_fault("memory oops");exit( i ); }while(0)
#define format_error( string )          do { default_fault("format oops: %s", string); }while(0)

/// fpk api
#define fpk_list( ... )                 scalls( FPK_COM, "list", __VA_ARGS__ )
#define fpk_register( ... )             scalls( FPK_COM, "register", __VA_ARGS__ )
#define fpk_unregister( ... )           scalls( FPK_COM, "unregister", __VA_ARGS__ )

/// init api
#define init_list( ... )                scalls( INIT_COM, "list", __VA_ARGS__ )
#define init_register( item, call )     scall2s( INIT_COM, "register", item, call )

/// uninit api
#define uninit_list( ... )              scalls( INIT_COM, "list", __VA_ARGS__ )
#define uninit_register( item, call )   scall2s( UNINIT_COM, "register", item, call )

/// joint api
#define joint_list( ... )               scalls( INIT_COM, "list", __VA_ARGS__ )
#define joint_register( item, call )    scall2s( JOINT_COM, "register", item, call )
#define joint_unregister( item, call )  scall2s( JOINT_COM, "unregister", item, call )
#define joint_calls( joint, string )    scall2s( JOINT_COM, "call", joint, string )
#define joint_callt( joint, json )      scallst( JOINT_COM, "call", joint, json )

/// machine api
#define machine_config( ... )           sgets( MACHINE_COM, __VA_ARGS__ )
#define machine_status( ... )           scalls( MACHINE_COM, "status", __VA_ARGS__ )
#define machine_restart( delay, key )   scalls( MACHINE_COM, "restart", "%d,%s", delay, key?:"" )
#define machine_reboot( delay, key )    scalls( MACHINE_COM, "reboot", "%d,%s", delay, key?:"" )
#define machine_default( delay, key )   scalls( MACHINE_COM, "default", "%d,%s", delay, key?:"" )



#endif   /* ----- #ifndef H_LAND_API_H  ----- */

