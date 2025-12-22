#ifndef H_NETWORK_LANDNET_H
#define H_NETWORK_LANDNET_H
/**
 * @brief 网络相关的基本接口
 * @author dimmalex
 * @version 1.0
*/



/// log for ifname
#define ifname_verbose( ifname, ... )   ifname_log( LANDLOG_VERBOSE, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_debug( ifname, ... )     ifname_log( LANDLOG_DEBUG, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_info( ifname, ... )      ifname_log( LANDLOG_INFO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_warn( ifname, ... )      ifname_log( LANDLOG_WARN, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_warning( ifname, ... )   ifname_log( LANDLOG_WARN|LANDLOG_ERRNO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_fault( ifname, ... )     ifname_log( LANDLOG_FAULT, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define ifname_faulting( ifname, ... )  ifname_log( LANDLOG_FAULT|LANDLOG_ERRNO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )
/* ifname defalut log function */
void ifname_log( int level, const char *ifname, const char *file, int line, const char *format, ... );



/// ip convert for rule 
int iprule_ip_convert( const char *ip, char *dip, int len );
/// port convert for rule 
int iprule_port_convert( const char *port, char *dport, int len );
// ip port clear the wrong char
int iprule_ipport_avoid( const char *ipport, char *dipport, int len );



/// ipv4 ip settings
void   static_ip_enable( const char *device, talk_t cfg );
boole  mode_static( const char *object, const char *ifdev, const char *device, talk_t cfg, talk_t info );
boole_t dhcp_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t dhcp );
boole_t pppoe_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t pppoe );
boole_t ppp_client_connect( const char *ifname, const char *ifdev, talk_t ppp, talk_t profile );



/// ipv6 ip settings
void   slaac_ip_enable( const char *device );
void   slaac_ip_disable( const char *device );
void   manual_ip_enable( const char *device, talk_t cfg );
void   manual_ip_disable( const char *device );
boole  method_manual( const char *object, const char *ifdev, const char *device, talk_t cfg, talk_t info );
talk_t automatic_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t cfg );



/**
 * @brief switch the resolv config to ifname
 * @param[in] ifname ifname
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole resolv_switch( const char *ifname );



/**
 * @brief set the extern tx queue
 * @param[in] object extern object
 * @param[in] netdev interface name
 * @param[in] len queue length
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole txqueue_set_ifname( const char *object, const char *netdev, const char *len  );
/**
 * @brief set the extern path mtu
 * @param[in] object extern object
 * @param[in] netdev interface name
 * @param[in] mtu interface mtu
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole pmtu_adjust_ifname( const char *object, const char *netdev, int mtu  );
boole pmtu_clear_ifname( const char *object, const char *netdev, int mtu  );



#define CONNECT_PID_FILE  PROJECT_VAR_DIR"/.connect.pid"
#define CONNECT_UNIX_FILE PROJECT_VAR_DIR"/.connect.unix"
/**
 * @brief create shunts
 * @param[in] custom value
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_create( talk_t list );
/**
 * @brief disable shunts
 * @param[in] force force old connection
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_disable( boole force );
/**
 * @brief direct all stream to ifname
 * @param[in] force force old connection
 * @param[in] ifname ifname
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_fix_ifname( boole force, const char *ifname );
/**
 * @brief balancing stream on two ifname
 * @param[in] force force old connection
 * @param[in] ifname ifname
 * @param[in] ifname2 ifname
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_balancing_2ifname( boole force, const char *ifname, const char *ifname2 );
boole shunts_balancing_3ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3 );
boole shunts_balancing_4ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3, const char *ifname4 );
/**
 * @brief add fix some host to ifname
 * @param[in] condition
 * @param[in] ifname
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_bind_add( const char *condition, const char *ifname );
/**
 * @brief delete fix some host to ifname
 * @param[in] condition
 * @param[in] ifname
 * @return succeed or failed
 *		@retval true for succeed
 *		@retval false for failed
 */
boole shunts_bind_delete( const char *condition, const char *ifname );



#endif   /* ----- #ifndef H_NETWORK_LANDNET_H  ----- */

