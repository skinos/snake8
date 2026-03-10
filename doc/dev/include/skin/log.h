#ifndef H_LAND_LOG_H
#define H_LAND_LOG_H

/**
 * @file log.h
 * @author dimmalex@gmail.com
 * @version 7.0
 * @date 20220219
 * @brief log call implementation
 */



/// log line lenght
#define LANDLOG_LINE_MAX            4096
/// log file name
#define LANDLOG_FILENAME            "landlog.txt"

/// log level                      
#define LANDLOG_LEVEL_OFFSET        (0)                            /* MASK               ( 0000 0000 0000 0000 0000 0000 1111 1111 ) */
#define LANDLOG_LEVEL_MASK          (0xFF<<LANDLOG_LEVEL_OFFSET)
#define LANDLOG_FAULT               (1<<LANDLOG_LEVEL_OFFSET)       /* FAULT */
#define LANDLOG_WARN                (2<<LANDLOG_LEVEL_OFFSET)       /* WARNNING */
#define LANDLOG_INFO                (4<<LANDLOG_LEVEL_OFFSET)       /* INFOMATION */
#define LANDLOG_DEBUG               (8<<LANDLOG_LEVEL_OFFSET)       /* DEBUG */
#define LANDLOG_VERBOSE             (16<<LANDLOG_LEVEL_OFFSET)      /* VERBOSE */
/// log options
#define LANDLOG_OPTION_OFFSET       (8)                            /* MASK               ( 0000 0000 0000 0000 1111 1111 0000 0000 ) */
#define LANDLOG_OPTION_MASK         (0xFF<<LANDLOG_OPTION_OFFSET)
#define LANDLOG_TUI                 (1<<LANDLOG_OPTION_OFFSET)      /* output to tui */
#define LANDLOG_SYSLOG              (2<<LANDLOG_OPTION_OFFSET)      /* output to syslog */
#define LANDLOG_FILE                (4<<LANDLOG_OPTION_OFFSET)      /* output to file */
#define LANDLOG_TRACE               (8<<LANDLOG_OPTION_OFFSET)      /* output trace */
#define LANDLOG_OUTPUT_MASK         (0xF<<LANDLOG_OPTION_OFFSET)
#define LANDLOG_ERRNO               (16<<LANDLOG_OPTION_OFFSET)      /* ERRNO */
/// log type
#define LANDLOG_TYPE_OFFSET         (16)                             /* MASK             ( 0000 0000 1111 1111 0000 0000 0000 0000 ) */
#define LANDLOG_TYPE_MASK           (0xFF<<LANDLOG_TYPE_OFFSET)
/// log subtype
#define LANDLOG_SUBTYPE_OFFSET      (24)                             /* MASK             ( 1111 1111 0000 0000 0000 0000 0000 0000 ) */
#define LANDLOG_SUBTYPE_MASK        (0xFF<<LANDLOG_SUBTYPE_OFFSET)

/*********************************************/
/**************** Define log type ************/
/*********************************************/
#define LANDLOG_TYPE_MAX      40
#define LANDLOG_SUBTYPE_MAX   10
#define LANDLOG_LEVEL_DEF     (LANDLOG_FAULT|LANDLOG_WARN|LANDLOG_INFO)

/// default
#define LANDLOG_DEFAULT             (0x00)
#define LANDLOG_DEFAULT_NONE        (0x00)
#define LANDLOG_DEFAULT_SHELL       (0x01)
#define LANDLOG_DEFAULT_EXECUTE     (0x02)

/// land
#define LANDLOG_LAND                (0x01)
#define LANDLOG_LAND_DEFAULT        (0x00)
#define LANDLOG_LAND_AUTH           (0x01)
#define LANDLOG_LAND_INIT           (0x02)
#define LANDLOG_LAND_SERVICE        (0x03)
#define LANDLOG_LAND_FPK            (0x04)

/// arch
#define LANDLOG_ARCH                (0x02)
#define LANDLOG_ARCH_DEFAULT        (0x00)
#define LANDLOG_ARCH_DATA           (0x01)
#define LANDLOG_ARCH_ETHERNET       (0x02)
#define LANDLOG_ARCH_GPIO           (0x03)
#define LANDLOG_ARCH_USB            (0x04)
#define LANDLOG_ARCH_PCI            (0x05)
#define LANDLOG_ARCH_SDIO           (0x06)
#define LANDLOG_ARCH_WIFI           (0x07)
#define LANDLOG_ARCH_FIRMWARE       (0x08)

/// network
#define LANDLOG_NETWORK             (0x03)
#define LANDLOG_NETWORK_DEFAULT     (0x00)
#define LANDLOG_NETWORK_VLAN        (0x01)
#define LANDLOG_NETWORK_BRIDGE      (0x02)
#define LANDLOG_NETWORK_KEEPLIVE    (0x03)
#define LANDLOG_NETWORK_CONNECT     (0x04)
/// ifname
#define LANDLOG_IFNAME              (0x04)
#define LANDLOG_IFNAME_DEFAULT      (0x00)
#define LANDLOG_IFNAME_LAN          (0x01)
#define LANDLOG_IFNAME_LAN2         (0x02)
#define LANDLOG_IFNAME_LAN3         (0x03)
#define LANDLOG_IFNAME_LAN4         (0x04)
#define LANDLOG_IFNAME_WAN          (0x05)
#define LANDLOG_IFNAME_WAN2         (0x06)
#define LANDLOG_IFNAME_WAN3         (0x07)
#define LANDLOG_IFNAME_WAN4         (0x08)
#define LANDLOG_IFNAME_LTE          (0x09)
#define LANDLOG_IFNAME_LTE2         (0x0a)
#define LANDLOG_IFNAME_LTE3         (0x0b)
#define LANDLOG_IFNAME_LTE4         (0x0c)
#define LANDLOG_IFNAME_WISP         (0x0d)
#define LANDLOG_IFNAME_WISP2        (0x0e)

/// agent
#define LANDLOG_AGENT               (0x05)
#define LANDLOG_AGENT_DEFAULT       (0x00)
#define LANDLOG_AGENT_LOCAL         (0x01)
#define LANDLOG_AGENT_REMOTE        (0x02)
#define LANDLOG_AGENT_HECLIENT      (0x03)
#define LANDLOG_AGENT_PORTC         (0x04)
#define LANDLOG_AGENT_GTOG          (0x05)
#define LANDLOG_AGENT_IO            (0x06)
/// center
#define LANDLOG_CENTER              (0x06)
#define LANDLOG_CENTER_DEFAULT      (0x00)
#define LANDLOG_CENTER_HEPORT       (0x01)
#define LANDLOG_CENTER_PPORT        (0x02)
#define LANDLOG_CENTER_NPORT        (0x03)
#define LANDLOG_CENTER_USERWEBS     (0x05)

/// wui
#define LANDLOG_WUI                 (0x07)
#define LANDLOG_WUI_DEFAULT         (0x00)
#define LANDLOG_WUI_HTTPD           (0x01)

/// client
#define LANDLOG_CLIENT              (0x08)
#define LANDLOG_CLIENT_DEFAULT      (0x00)
#define LANDLOG_CLIENT_DHCPS        (0x01)
#define LANDLOG_CLIENT_STATION      (0x02)
#define LANDLOG_CLIENT_ACL          (0x03)
#define LANDLOG_CLIENT_FILTER       (0x04)
#define LANDLOG_CLIENT_TC           (0x05)

/// modem
#define LANDLOG_MODEM               (0x09)
#define LANDLOG_MODEM_DEFAULT       (0x00)
#define LANDLOG_MODEM_LTE           (0x01)
#define LANDLOG_MODEM_LTE2          (0x02)
#define LANDLOG_MODEM_LTE3          (0x03)
#define LANDLOG_MODEM_LTE4          (0x04)

/// UART
#define LANDLOG_UART                (0x0a)
#define LANDLOG_UART_DEFAULT        (0x00)
/// VPN
#define LANDLOG_VPN                 (0x0b)
#define LANDLOG_VPN_DEFAULT         (0x00)
#define LANDLOG_VPN_OVPN            (0x01)
/// VPN Server
#define LANDLOG_VPNS                (0x0c)
#define LANDLOG_VPNS_DEFAULT        (0x00)
#define LANDLOG_VPNS_OVPN           (0x01)

/// APP
#define LANDLOG_APP                 (0x0f)
#define LANDLOG_APP_DEFAULT         (0x00)



/**
 * @brief Internal actual logging function
 * @param[in] flags The identifier for this log
 * @param[in] filename Name of the current log file
 * @param[in] line The line number of the current logging code
 * @param[in] format log contents
 * @return none
 */
void landlog( unsigned int flags, const char *filename, int line, const char *format, ... );
/* record a log with flags */
#define journal( flags, ... )     landlog( (flags), (__FILE__), (__LINE__), __VA_ARGS__ )



/* defalut log function */
#define default_verbose( ... )      landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define default_debug( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define default_info( ... )         landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define default_warn( ... )         landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define default_warning( ... )      landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define default_fault( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define default_faulting( ... )     landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_NONE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* shell type log function */
#define shell_verbose( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define shell_debug( ... )          landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define shell_info( ... )           landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define shell_warn( ... )           landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define shell_warning( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define shell_fault( ... )          landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define shell_faulting( ... )       landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_SHELL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* execute type log function */
#define execute_verbose( ... )      landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define execute_debug( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define execute_info( ... )         landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define execute_warn( ... )         landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define execute_warning( ... )      landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define execute_fault( ... )        landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define execute_faulting( ... )     landlog( ((LANDLOG_DEFAULT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_DEFAULT_EXECUTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* land defalut log function */
#define land_verbose( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define land_debug( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define land_info( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define land_warn( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define land_warning( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define land_fault( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define land_faulting( ... )     landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* land auth type log function */
#define auth_verbose( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define auth_debug( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define auth_info( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define auth_warn( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define auth_warning( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define auth_fault( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define auth_faulting( ... )     landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_AUTH<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* land init type log function */
#define init_verbose( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define init_debug( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define init_info( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define init_warn( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define init_warning( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define init_fault( ... )        landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define init_faulting( ... )     landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_INIT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* land service type log function */
#define service_verbose( ... )   landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define service_debug( ... )     landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define service_info( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define service_warn( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define service_warning( ... )   landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define service_fault( ... )     landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define service_faulting( ... )  landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_SERVICE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* land fpk type log function */
#define fpk_verbose( ... )       landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define fpk_debug( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define fpk_info( ... )          landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define fpk_warn( ... )          landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define fpk_warning( ... )       landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define fpk_fault( ... )         landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define fpk_faulting( ... )      landlog( ((LANDLOG_LAND<<LANDLOG_TYPE_OFFSET)|(LANDLOG_LAND_FPK<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* arch defalut log function */
#define arch_verbose( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define arch_debug( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define arch_info( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define arch_warn( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define arch_warning( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define arch_fault( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define arch_faulting( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* data arch type log function */
#define data_verbose( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define data_debug( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define data_info( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define data_warn( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define data_warning( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define data_fault( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define data_faulting( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_DATA<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* ethernet arch type log function */
#define ethernet_verbose( ... )  landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ethernet_debug( ... )    landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ethernet_info( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ethernet_warn( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ethernet_warning( ... )  landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define ethernet_fault( ... )    landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ethernet_faulting( ... ) landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_ETHERNET<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* gpio arch type log function */
#define gpio_verbose( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gpio_debug( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gpio_info( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gpio_warn( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gpio_warning( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define gpio_fault( ... )        landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gpio_faulting( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_GPIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* usb arch type log function */
#define usb_verbose( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define usb_debug( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define usb_info( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define usb_warn( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define usb_warning( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define usb_fault( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define usb_faulting( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_USB<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* pci arch type log function */
#define pci_verbose( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pci_debug( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pci_info( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pci_warn( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pci_warning( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define pci_fault( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pci_faulting( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_PCI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* sdio arch type log function */
#define sdio_verbose( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define sdio_debug( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define sdio_info( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define sdio_warn( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define sdio_warning( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define sdio_fault( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define sdio_faulting( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_SDIO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* pci arch type log function */
#define wifi_verbose( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define wifi_debug( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define wifi_info( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define wifi_warn( ... )          landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define wifi_warning( ... )       landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define wifi_fault( ... )         landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define wifi_faulting( ... )      landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_WIFI<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* firmware arch type log function */
#define firmware_verbose( ... )  landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define firmware_debug( ... )    landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define firmware_info( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define firmware_warn( ... )     landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define firmware_warning( ... )  landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define firmware_fault( ... )    landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define firmware_faulting( ... ) landlog( ((LANDLOG_ARCH<<LANDLOG_TYPE_OFFSET)|(LANDLOG_ARCH_FIRMWARE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* network defalut log function */
#define network_verbose( ... )   landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define network_debug( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define network_info( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define network_warn( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define network_warning( ... )   landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define network_fault( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define network_faulting( ... )  landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* vlan network type log function */
#define vlan_verbose( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vlan_debug( ... )        landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vlan_info( ... )         landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vlan_warn( ... )         landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vlan_warning( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define vlan_fault( ... )        landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vlan_faulting( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_VLAN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* bridge network type log function */
#define bridge_verbose( ... )    landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define bridge_debug( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define bridge_info( ... )       landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define bridge_warn( ... )       landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define bridge_warning( ... )    landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define bridge_fault( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define bridge_faulting( ... )   landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_BRIDGE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* keeplive network type log function */
#define keeplive_verbose( ... )  landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define keeplive_debug( ... )    landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define keeplive_info( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define keeplive_warn( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define keeplive_warning( ... )  landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define keeplive_fault( ... )    landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define keeplive_faulting( ... ) landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_KEEPLIVE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* connect network type log function */
#define connect_verbose( ... )   landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define connect_debug( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define connect_info( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define connect_warn( ... )      landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define connect_warning( ... )   landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define connect_fault( ... )     landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define connect_faulting( ... )  landlog( ((LANDLOG_NETWORK<<LANDLOG_TYPE_OFFSET)|(LANDLOG_NETWORK_CONNECT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* wui http server type log function */
#define httpd_verbose( ... )     landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define httpd_debug( ... )       landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define httpd_info( ... )        landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define httpd_warn( ... )        landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define httpd_warning( ... )     landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define httpd_fault( ... )       landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define httpd_faulting( ... )    landlog( ((LANDLOG_WUI<<LANDLOG_TYPE_OFFSET)|(LANDLOG_WUI_HTTPD<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* client defalut log function */
#define client_verbose( ... )   landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define client_debug( ... )     landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define client_info( ... )      landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define client_warn( ... )      landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define client_warning( ... )   landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define client_fault( ... )     landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define client_faulting( ... )  landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* client dhcp server log function */
#define dhcps_verbose( ... )    landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define dhcps_debug( ... )      landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define dhcps_info( ... )       landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define dhcps_warn( ... )       landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define dhcps_warning( ... )    landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define dhcps_fault( ... )      landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define dhcps_faulting( ... )   landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_DHCPS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* client station server log function */
#define station_verbose( ... )  landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define station_debug( ... )    landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define station_info( ... )     landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define station_warn( ... )     landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define station_warning( ... )  landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define station_fault( ... )    landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define station_faulting( ... ) landlog( ((LANDLOG_CLIENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CLIENT_STATION<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* agent defalut log function */
#define agent_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define agent_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define agent_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define agent_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define agent_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define agent_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define agent_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent local log function */
#define local_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define local_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define local_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define local_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define local_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define local_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define local_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_LOCAL<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent remote log function */
#define remote_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define remote_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define remote_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define remote_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define remote_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define remote_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define remote_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_REMOTE<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent heclient log function */
#define heclient_verbose( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heclient_debug( ... )    landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heclient_info( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heclient_warn( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heclient_warning( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define heclient_fault( ... )    landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heclient_faulting( ... ) landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_HECLIENT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent portc log function */
#define portc_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define portc_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define portc_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define portc_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define portc_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define portc_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define portc_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_PORTC<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent gtog log function */
#define gtog_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gtog_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gtog_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gtog_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gtog_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define gtog_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define gtog_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_GTOG<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* agent io log function */
#define io_verbose( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define io_debug( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define io_info( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define io_warn( ... )      landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define io_warning( ... )   landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define io_fault( ... )     landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define io_faulting( ... )  landlog( ((LANDLOG_AGENT<<LANDLOG_TYPE_OFFSET)|(LANDLOG_AGENT_IO<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* center defalut log function */
#define center_verbose( ... )   landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define center_debug( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define center_info( ... )      landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define center_warn( ... )      landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define center_warning( ... )   landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define center_fault( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define center_faulting( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* center heport log function */
#define heport_verbose( ... )   landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heport_debug( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heport_info( ... )      landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heport_warn( ... )      landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heport_warning( ... )   landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define heport_fault( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define heport_faulting( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_HEPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* center pport log function */
#define pport_verbose( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pport_debug( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pport_info( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pport_warn( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pport_warning( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define pport_fault( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define pport_faulting( ... ) landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_PPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* center nport log function */
#define nport_verbose( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define nport_debug( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define nport_info( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define nport_warn( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define nport_warning( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define nport_fault( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define nport_faulting( ... ) landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_NPORT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* center userwebs log function */
#define userwebs_verbose( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define userwebs_debug( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define userwebs_info( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define userwebs_warn( ... )     landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define userwebs_warning( ... )  landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define userwebs_fault( ... )    landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define userwebs_faulting( ... ) landlog( ((LANDLOG_CENTER<<LANDLOG_TYPE_OFFSET)|(LANDLOG_CENTER_USERWEBS<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* vpn defalut log function */
#define vpn_verbose( ... )   landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpn_debug( ... )     landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpn_info( ... )      landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpn_warn( ... )      landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpn_warning( ... )   landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define vpn_fault( ... )     landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpn_faulting( ... )  landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* vpn for openvpn log function */
#define ovpn_verbose( ... )    landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpn_debug( ... )      landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpn_info( ... )       landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpn_warn( ... )       landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpn_warning( ... )    landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define ovpn_fault( ... )      landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpn_faulting( ... )   landlog( ((LANDLOG_VPN<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPN_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* vpns defalut log function */
#define vpns_verbose( ... )   landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpns_debug( ... )     landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpns_info( ... )      landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpns_warn( ... )      landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpns_warning( ... )   landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define vpns_fault( ... )     landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define vpns_faulting( ... )  landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
/* vpns for openvpn log function */
#define ovpns_verbose( ... )    landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpns_debug( ... )      landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpns_info( ... )       landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpns_warn( ... )       landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpns_warning( ... )    landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define ovpns_fault( ... )      landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define ovpns_faulting( ... )   landlog( ((LANDLOG_VPNS<<LANDLOG_TYPE_OFFSET)|(LANDLOG_VPNS_OVPN<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



/* app defalut log function */
#define app_verbose( ... )   landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_VERBOSE), (__FILE__), (__LINE__), __VA_ARGS__ )
#define app_debug( ... )     landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_DEBUG), (__FILE__), (__LINE__), __VA_ARGS__ )
#define app_info( ... )      landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_INFO), (__FILE__), (__LINE__), __VA_ARGS__ )
#define app_warn( ... )      landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN), (__FILE__), (__LINE__), __VA_ARGS__ )
#define app_warning( ... )   landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_WARN|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )
#define app_fault( ... )     landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT), (__FILE__), (__LINE__), __VA_ARGS__ )
#define app_faulting( ... )  landlog( ((LANDLOG_APP<<LANDLOG_TYPE_OFFSET)|(LANDLOG_APP_DEFAULT<<LANDLOG_SUBTYPE_OFFSET)|LANDLOG_FAULT|LANDLOG_ERRNO), (__FILE__), ( __LINE__ ), __VA_ARGS__ )



#endif   /* ----- #ifndef H_LAND_LOG_H  ----- */

