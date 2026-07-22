/**
 * @file skinnet.h
 * @brief Network basic interface header file
 * @author dimmalex
 * @version 1.0
 */

#ifndef H_NETWORK_SKINNET_H
#define H_NETWORK_SKINNET_H

#include "skin/skin.h"

#define KEEPLIVE_RECV_MODE -1000


/**
 * @defgroup LogMacros Interface Logging Macros
 * @brief Logging macro definitions based on interface name
 * @{
 */

/**
 * @brief Verbose level logging macro
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_verbose( ifname, ... )   ifname_log( LANDLOG_VERBOSE, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Debug level logging macro
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_debug( ifname, ... )     ifname_log( LANDLOG_DEBUG, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Info level logging macro
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_info( ifname, ... )      ifname_log( LANDLOG_INFO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_warn( ifname, ... )      ifname_log( LANDLOG_WARN, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Warning level logging macro with errno
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_warning( ifname, ... )   ifname_log( LANDLOG_WARN|LANDLOG_ERRNO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_fault( ifname, ... )     ifname_log( LANDLOG_FAULT, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Fault level logging macro with errno
 * @param[in] ifname Interface name
 * @param[in] ... Variable arguments, format string and parameters
 */
#define ifname_faulting( ifname, ... )  ifname_log( LANDLOG_FAULT|LANDLOG_ERRNO, ifname, (__FILE__), (__LINE__),  __VA_ARGS__ )

/**
 * @brief Interface default logging function
 * @param[in] level Log level
 * @param[in] ifname Interface name
 * @param[in] file Source file name
 * @param[in] line Line number
 * @param[in] format Format string
 * @param[in] ... Variable arguments
 */
void ifname_log( int level, const char *ifname, const char *file, int line, const char *format, ... );

/** @} */ /* LogMacros */



/**
 * @defgroup RuleConvert Rule Conversion Functions
 * @brief IP and port format conversion functions for iptables rules
 * @{
 */

/**
 * @brief Convert IP address to iptables command format
 * @param[in] ip Original IP address string
 * @param[out] dip Converted IP address buffer
 * @param[in] len Buffer length
 * @return Conversion result flag
 * @retval 0 No special flag
 * @retval 1 Contains range flag (original string has '-' or ':')
 * @note Replace ':' with '-' to support IP range representation
 */
int iprule_ip_convert( const char *ip, char *dip, int len );

/**
 * @brief Convert port number to iptables command format
 * @param[in] port Original port string
 * @param[out] dport Converted port buffer
 * @param[in] len Buffer length
 * @return Conversion result flag
 * @retval 0 No special flag
 * @retval 1 Contains multi-port flag (original string has ';' or ',')
 * @note Replace ';' with ',' and '-' with ':' to support port range and list
 */
int iprule_port_convert( const char *port, char *dport, int len );

/**
 * @brief Clean illegal characters from IP port string
 * @param[in] ipport Original IP port string
 * @param[out] dipport Cleaned string buffer
 * @param[in] len Buffer length
 * @return Cleaning result
 * @retval 0 String is valid
 * @retval 1 Found illegal character and terminated
 * @note Only allows digits, '.', '-', ',', ';', ':' characters
 */
int iprule_ipport_avoid( const char *ipport, char *dipport, int len );

/** @} */ /* RuleConvert */



/**
 * @defgroup IPv4Settings IPv4 Network Settings
 * @brief IPv4 address configuration and connection management
 * @{
 */

/**
 * @brief Enable static IP configuration
 * @param[in] device Network device name
 * @param[in] cfg Configuration info (JSON format)
 * @note Configure primary IP and virtual IPs (ip2-ip10), and send ARP announcement
 */
void static_ip_enable( const char *device, talk_t cfg );

/**
 * @brief Configure static IP mode
 * @param[in] object Object name
 * @param[in] ifdev Interface device name
 * @param[in] netdev Network device name
 * @param[in] cfg Configuration info (JSON format)
 * @param[out] v Output info (JSON format)
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Set static IP mode related parameters (gateway, DNS, domain, etc.)
 */
boole mode_static( const char *object, const char *ifdev, const char *netdev, talk_t cfg, talk_t v );

/**
 * @brief Start DHCP client connection
 * @param[in] ifname Interface name
 * @param[in] ifdev Interface device name
 * @param[in] device Network device name
 * @param[in] dhcp DHCP configuration info (JSON format)
 * @return Execution result
 * @retval true Started successfully
 * @retval terror Error
 * @retval tfalse Execution failed
 * @note This function forks child process to execute udhcpc, will not return after configuration
 */
boole_t dhcp_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t dhcp );

/**
 * @brief Start PPPoE client connection
 * @param[in] ifname Interface name
 * @param[in] ifdev Interface device name
 * @param[in] device Network device name
 * @param[in] pppoe PPPoE configuration info (JSON format)
 * @return Execution result
 * @retval true Started successfully
 * @retval tfalse Execution failed
 * @note This function forks child process to execute pppd, will not return after configuration
 */
boole_t pppoe_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t pppoe );

/**
 * @brief Start PPP dial-up connection (for 3G/4G/LTE)
 * @param[in] ifname Interface name
 * @param[in] ifdev Interface device name
 * @param[in] ppp PPP configuration info (JSON format)
 * @param[in] profile Dial-up profile (JSON format)
 * @return Execution result
 * @retval true Started successfully
 * @retval tfalse Execution failed
 * @note This function forks child process to execute pppd, will not return after configuration
 */
boole_t ppp_client_connect( const char *ifname, const char *ifdev, talk_t ppp, talk_t profile );

/** @} */ /* IPv4Settings */



/**
 * @defgroup IPv6Settings IPv6 Network Settings
 * @brief IPv6 address configuration and connection management
 * @{
 */

/**
 * @brief Enable SLAAC (Stateless Address Autoconfiguration)
 * @param[in] device Network device name
 * @note Set /proc/sys/net/ipv6/conf/{device}/accept_ra to 2, accept router advertisements
 */
void slaac_ip_enable( const char *device );

/**
 * @brief Disable SLAAC
 * @param[in] device Network device name
 * @note Set /proc/sys/net/ipv6/conf/{device}/accept_ra to 0
 */
void slaac_ip_disable( const char *device );

/**
 * @brief Enable manually configured IPv6 address
 * @param[in] device Network device name
 * @param[in] cfg Configuration info (JSON format, contains addr and prefix)
 * @note Use ip command to add IPv6 address
 */
void manual_ip_enable( const char *device, talk_t cfg );

/**
 * @brief Disable manually configured IPv6 address
 * @param[in] device Network device name
 * @note Currently declared but not implemented
 */
void manual_ip_disable( const char *device );

/**
 * @brief Configure manual IPv6 mode
 * @param[in] object Object name
 * @param[in] ifdev Interface device name
 * @param[in] device Network device name
 * @param[in] cfg Configuration info (JSON format)
 * @param[out] info Output info (JSON format)
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Set manual IPv6 mode related parameters (hop, resolve, etc.)
 */
boole method_manual( const char *object, const char *ifdev, const char *device, talk_t cfg, talk_t info );

/**
 * @brief Start automatic IPv6 client connection (DHCPv6)
 * @param[in] ifname Interface name
 * @param[in] ifdev Interface device name
 * @param[in] device Network device name
 * @param[in] cfg Configuration info (JSON format)
 * @return Execution result
 * @retval true Started successfully
 * @retval terror Error
 * @retval tfalse Execution failed
 * @note This function forks child process to execute odhcp6c, will not return after configuration
 */
talk_t automatic_client_connect( const char *ifname, const char *ifdev, const char *device, talk_t cfg );

/** @} */ /* IPv6Settings */



/**
 * @defgroup DNSConfig DNS Configuration
 * @brief DNS resolution configuration management
 * @{
 */

/**
 * @brief Switch DNS resolution configuration to specified interface
 * @param[in] ifname Interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Create symbolic link to point /etc/resolv.conf to interface-specific DNS config
 */
boole resolv_switch( const char *ifname );

/** @} */ /* DNSConfig */



/**
 * @defgroup InterfaceConfig Interface Configuration
 * @brief Network interface parameter configuration
 * @{
 */

/**
 * @brief Set interface transmit queue length
 * @param[in] object Object name
 * @param[in] netdev Network interface name
 * @param[in] len Queue length string
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole txqueue_set_ifname( const char *object, const char *netdev, const char *len );

/**
 * @brief Adjust interface path MTU (add iptables rule)
 * @param[in] object Object name
 * @param[in] netdev Network interface name
 * @param[in] mtu MTU value
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note When MTU is less than 1400, add TCP MSS clamping rule
 */
boole pmtu_adjust_ifname( const char *object, const char *netdev, int mtu );

/**
 * @brief Clear interface path MTU setting (delete iptables rule)
 * @param[in] object Object name
 * @param[in] netdev Network interface name
 * @param[in] mtu MTU value
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole pmtu_clear_ifname( const char *object, const char *netdev, int mtu );

/** @} */ /* InterfaceConfig */



/**
 * @defgroup ShuntsConfig Shunts Configuration
 * @brief Multi-WAN load balancing and policy routing configuration
 * @{
 */

/** @brief Connection process Unix domain socket path */
#define CONNECT_UNIX_FILE PROJECT_VAR_DIR"/.connect.unix"

/**
 * @brief Create shunts rule chain
 * @param[in] list Custom rules
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Create shunts and assign chains, set connection mark save and restore rules
 */
boole shunts_create( talk_t list );

/**
 * @brief Disable shunts rules
 * @param[in] force Whether to force clear old connections
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_disable( boole force );

/**
 * @brief Fix all traffic to specified interface
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname Interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Mark all new connections with the routing mark of the specified interface
 */
boole shunts_fix_ifname( boole force, const char *ifname );

/**
 * @brief Load balancing between two interfaces
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname First interface name
 * @param[in] ifname2 Second interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 * @note Use statistic module to distribute traffic in round-robin manner
 */
boole shunts_balancing_2ifname( boole force, const char *ifname, const char *ifname2 );

/**
 * @brief Load balancing among three interfaces
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname First interface name
 * @param[in] ifname2 Second interface name
 * @param[in] ifname3 Third interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_balancing_3ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3 );

/**
 * @brief Load balancing among four interfaces
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname First interface name
 * @param[in] ifname2 Second interface name
 * @param[in] ifname3 Third interface name
 * @param[in] ifname4 Fourth interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_balancing_4ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3, const char *ifname4 );

/**
 * @brief Load balancing among five interfaces
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname First interface name
 * @param[in] ifname2 Second interface name
 * @param[in] ifname3 Third interface name
 * @param[in] ifname4 Fourth interface name
 * @param[in] ifname5 Fifth interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_balancing_5ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3, const char *ifname4, const char *ifname5 );

/**
 * @brief Load balancing among six interfaces
 * @param[in] force Whether to force clear old connections
 * @param[in] ifname First interface name
 * @param[in] ifname2 Second interface name
 * @param[in] ifname3 Third interface name
 * @param[in] ifname4 Fourth interface name
 * @param[in] ifname5 Fifth interface name
 * @param[in] ifname6 Sixth interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_balancing_6ifname( boole force, const char *ifname, const char *ifname2, const char *ifname3, const char *ifname4, const char *ifname5, const char *ifname6 );

/**
 * @brief Add bind rule (bind traffic matching condition to specified interface)
 * @param[in] condition iptables match condition string
 * @param[in] ifname Interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_bind_add( const char *condition, const char *ifname );

/**
 * @brief Delete bind rule
 * @param[in] condition iptables match condition string
 * @param[in] ifname Interface name
 * @return Success or failure
 * @retval true Success
 * @retval false Failure
 */
boole shunts_bind_delete( const char *condition, const char *ifname );

/** @} */ /* ShuntsConfig */



#endif   /* ----- #ifndef H_NETWORK_SKINNET_H  ----- */

