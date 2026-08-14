#ifndef  H_LAND_LANDHEAD_H
#define  H_LAND_LANDHEAD_H

/**
 * @file skinhead.h
 * @author dimmalex@gmail.com
 * @version 8.0
 * @date 20220219
 * @brief skinos system commom macro
 * @details define the marco for common length/file/type/mode/project/components, include the skin system header file
 * @see his brother stdhead.h and his child skin.h
 * @note define boole data type, redefine the false and true value
 */

 

 /// Maximum identifier name length (256 characters)
#ifndef NAME_MAX
#define NAME_MAX          (256)
#endif
/// Maximum file path length (512 characters)
#ifndef PATH_MAX
#define PATH_MAX          (512)
#endif
/// Maximum command line length (1024 characters)
#ifndef LINE_MAX
#define LINE_MAX          (1024)
#endif
/// Soft cap (bytes) for single-line JSON / read growth in socket helpers (e.g. util_socket.c)
#define JSON_LINE_MAX   (65535)

/// default file mode
#define REGULAR_FILE_MODE (0660)
/// default directory mode
#define OPEN_DIR_MODE     (0775)
#define REGULAR_DIR_MODE  (0770)



/// Additional error code: process was killed (133)
#define EBYKILLED         (133)
/// Exit codes mapping to talk_t special values
#define EXIT_tnull     	  (100)
#define EXIT_ttrue     	  (101)
#define EXIT_tfalse       (102)
#define EXIT_terror       (103)
#define EXIT_EPERM        (104)
#define EXIT_ECOM         (105)
#define EXIT_EFUNC        (106)
/// redefine the true
#ifdef true
    #undef true
#endif
#define true              (1)
/// redefine the false
#ifdef false
    #undef false
#endif
#define false             (0)
/// define boole data type for boolean
typedef int boole;



/// Project and component separator (e.g., "land@machine")
#define PROJECT_OBJECT_GAPS   "@"
#define PROJECT_OBJECT_GAPC   '@'
/// Project and file path separator (e.g., "land/file.conf")
#define PROJECT_FILE_GAPS     "/"
#define PROJECT_FILE_GAPC     '/'
/// Component and config attribute separator (e.g., "eth0:ip")
#define OBJECT_CONFIG_GAPS    ":"
#define OBJECT_CONFIG_GAPC    ':'
#define CONFIG_CONFIG_GAPS    "/"
#define CONFIG_CONFIG_GAPC    '/'
#define CONFIG_SET_GAPC       '='
#define CONFIG_SET_GAPS       "="
#define CONFIG_OR_GAPC        '|'
#define CONFIG_OR_GAPS        "|"
/// Component and API method separator (e.g., "eth0.status")
#define OBJECT_API_GAPS       "."
#define OBJECT_API_GAPC       '.'
#define API_PARAM_STARTS      "["
#define API_PARAM_STARTC      '['
#define API_PARAM_ENDS        "]"
#define API_PARAM_ENDC        ']'
#define PARAM_PARAM_GAPS      ","
#define PARAM_PARAM_GAPC      ','
/// define system reserve name prefix
#define NAME_RES_PREFIX       "~"
#define NAME_RES_PREFIXC      '~'



/// hardware project
#define ARCH_PROJECT    "arch"
#define PDRIVER_PROJECT "pdriver"
/** usb device management component */
#define USB_ITEM        "usb"
#define USBDRV_PROJECT  "usbdrv"
#define USBDRV_ITEM     USBDRV_PROJECT
/** pci device management component */
#define PCI_ITEM        "pci"
#define PCIDRV_PROJECT  "pcidrv"
#define PCIDRV_ITEM     PCIDRV_PROJECT
/** sdio device management component */
#define SDIO_ITEM       "sdio"
#define SDIODRV_PROJECT "sdiodrv"
#define SDIODRV_ITEM    SDIODRV_PROJECT
/** device gpio management component */
#define GPIO_COM        ARCH_PROJECT""PROJECT_OBJECT_GAPS"gpio"
/** device data management component */
#define DATA_COM        ARCH_PROJECT""PROJECT_OBJECT_GAPS"data"
/** device config management component */
#define CONFIG_COM      ARCH_PROJECT""PROJECT_OBJECT_GAPS"config"
/** device firmware management component */
#define FIRMWARE_COM    ARCH_PROJECT""PROJECT_OBJECT_GAPS"firmware"
/** device factory management component */
#define FACTORY_COM     ARCH_PROJECT""PROJECT_OBJECT_GAPS"factory"
/** hardware platform network management component */
#define NET_CONFIG      ARCH_PROJECT""PROJECT_OBJECT_GAPS"net"
/** hardware platform switch management component */
#define ETHERNET_COM    ARCH_PROJECT""PROJECT_OBJECT_GAPS"ethernet"
/** hardware platform usb management component */
#define USB_COM         ARCH_PROJECT""PROJECT_OBJECT_GAPS"usb"
/** hardware platform pci management component */
#define PCI_COM         ARCH_PROJECT""PROJECT_OBJECT_GAPS"pci"
/** hardware platform sdio management component */
#define SDIO_COM        ARCH_PROJECT""PROJECT_OBJECT_GAPS"sdio"
/** system custom management component */
#define CUSTOM_COM      ARCH_PROJECT""PROJECT_OBJECT_GAPS"custom"
/** configure lock management component */
#define LOCK_COM        ARCH_PROJECT""PROJECT_OBJECT_GAPS"lock"

/// core project
#define LAND_PROJECT    "land"
#define DAEMON_EXE      LAND_PROJECT""PROJECT_OBJECT_GAPS"daemon"
#define HEART_COM       LAND_PROJECT""PROJECT_OBJECT_GAPS"heart"
/** component management component */
#define COM_COM         LAND_PROJECT""PROJECT_OBJECT_GAPS"com"
/** machine basic property management component */
#define MACHINE_COM     LAND_PROJECT""PROJECT_OBJECT_GAPS"machine"
/** service management component */
#define SERVICE_COM     LAND_PROJECT""PROJECT_OBJECT_GAPS"service"
/** fpk management component */
#define FPK_COM         LAND_PROJECT""PROJECT_OBJECT_GAPS"fpk"
/** init/uninit management component */
#define INIT_COM        LAND_PROJECT""PROJECT_OBJECT_GAPS"init"
#define UNINIT_COM      LAND_PROJECT""PROJECT_OBJECT_GAPS"uninit"
/** joint management component */
#define JOINT_COM       LAND_PROJECT""PROJECT_OBJECT_GAPS"joint"
/** auth management component */
#define AUTH_COM        LAND_PROJECT""PROJECT_OBJECT_GAPS"auth"
/** register management component */
#define REG_COM         LAND_PROJECT""PROJECT_OBJECT_GAPS"register"
#define MACHINE_REGFILE "machine"
#define COM_REGFILE     "com"
#define CONFIG_REGFILE  "config"

/// clock/date project
#define CLOCK_PROJECT   "clock"
/** date management component */
#define DATE_COM        CLOCK_PROJECT""PROJECT_OBJECT_GAPS"date"

/// terminal user interface project
#define TUI_PROJECT     "tui"
/** telnet management component */
#define TELNETD_COM     TUI_PROJECT""PROJECT_OBJECT_GAPS"telnet"
/** ssh management component */
#define SSHD_COM        TUI_PROJECT""PROJECT_OBJECT_GAPS"ssh"
/// web server project
#define WEBS_PROJECT    "webs"
/// web user interface project
#define WUI_PROJECT     "wui"
/** web user interface management component */
#define WUI_COM         WUI_PROJECT""PROJECT_OBJECT_GAPS"admin"

/// agent project
#define AGENT_PROJECT    "agent"
/** agent local component */
#define AGENT_LOCAL_COM       AGENT_PROJECT""PROJECT_OBJECT_GAPS"local"
/** agent he client component */
#define AGENT_HECLIENT_COM    AGENT_PROJECT""PROJECT_OBJECT_GAPS"heclient"
#define HECLIENT_COM          AGENT_PROJECT""PROJECT_OBJECT_GAPS"heclient"
/** agent port client component */
#define AGENT_PORTC_COM       AGENT_PROJECT""PROJECT_OBJECT_GAPS"portc"
#define PORTC_COM             AGENT_PROJECT""PROJECT_OBJECT_GAPS"portc"
/** agent network component */
#define AGENT_GTOG_COM        AGENT_PROJECT""PROJECT_OBJECT_GAPS"gtog"
#define GTOG_COM              AGENT_PROJECT""PROJECT_OBJECT_GAPS"gtog"
/** agent io component */
#define AGENT_IO_COM          AGENT_PROJECT""PROJECT_OBJECT_GAPS"io"

/// center project
#define CENTER_PROJECT    "center"
/** center heport component */
#define CENTERT_HEPORT_COM    CENTER_PROJECT""PROJECT_OBJECT_GAPS"heport"
#define HEPORT_COM            CENTER_PROJECT""PROJECT_OBJECT_GAPS"heport"
/** center api component */
#define CENTER_API_COM        CENTER_PROJECT""PROJECT_OBJECT_GAPS"api"
#define API_COM               CENTER_PROJECT""PROJECT_OBJECT_GAPS"api"
/** center pport component */
#define CENTER_PPORT_COM      CENTER_PROJECT""PROJECT_OBJECT_GAPS"pport"
#define PPORT_COM             CENTER_PROJECT""PROJECT_OBJECT_GAPS"pport"
/// network project
#define NETWORK_PROJECT "network"
#define NETWORK_MODE    "default"
/// network app project
#define BRIDGE_NAME     "bridge"
#define VLAN_NAME       "vlan"
/// ifname project
#define IFNAME_PROJECT  "ifname"
#define IFNAME_NAME     IFNAME_PROJECT
/** network frame component */
#define NETWORK_COM     NETWORK_PROJECT""PROJECT_OBJECT_GAPS"frame"
#define CONNECT_COM     NETWORK_PROJECT""PROJECT_OBJECT_GAPS"connect"
#define KEEPLIVE_COM    NETWORK_PROJECT""PROJECT_OBJECT_GAPS"keeplive"
#define BRIDGE_COM      NETWORK_PROJECT""PROJECT_OBJECT_GAPS"bridge"
#define VLAN_COM        NETWORK_PROJECT""PROJECT_OBJECT_GAPS"vlan"
#define HOSTS_COM       NETWORK_PROJECT""PROJECT_OBJECT_GAPS"hosts"
/** ifname component */
#define LAN_COM         IFNAME_NAME""PROJECT_OBJECT_GAPS"lan"
#define LAN1_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"lan1"
#define LAN2_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"lan2"
#define LAN3_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"lan3"
#define LAN4_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"lan4"
#define WLTE_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"lte"
#define WLTE2_COM       IFNAME_NAME""PROJECT_OBJECT_GAPS"lte2"
#define WLTE3_COM       IFNAME_NAME""PROJECT_OBJECT_GAPS"lte3"
#define WLTE4_COM       IFNAME_NAME""PROJECT_OBJECT_GAPS"lte4"
#define WAN_COM         IFNAME_NAME""PROJECT_OBJECT_GAPS"wan"
#define WAN2_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"wan2"
#define WAN3_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"wan3"
#define WAN4_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"wan4"
#define WISP_COM        IFNAME_NAME""PROJECT_OBJECT_GAPS"wisp"
#define WISP2_COM       IFNAME_NAME""PROJECT_OBJECT_GAPS"wisp2"

/// forward project
#define FORWARD_PROJECT "forward"
#define NAT_COM         FORWARD_PROJECT""PROJECT_OBJECT_GAPS"nat"
#define DNAT_COM        FORWARD_PROJECT""PROJECT_OBJECT_GAPS"dnat"
#define FIREWALL_COM    FORWARD_PROJECT""PROJECT_OBJECT_GAPS"firewall"
#define RULE_COM        FORWARD_PROJECT""PROJECT_OBJECT_GAPS"rule"
#define ROUTE_COM       FORWARD_PROJECT""PROJECT_OBJECT_GAPS"route"
#define ROUTES_COM      FORWARD_PROJECT""PROJECT_OBJECT_GAPS"routes"
#define ROUTE_LOCAL_COM    FORWARD_PROJECT""PROJECT_OBJECT_GAPS"local"
#define ROUTE_MAIN_COM     FORWARD_PROJECT""PROJECT_OBJECT_GAPS"main"
#define ROUTE_DEFAULT_COM  FORWARD_PROJECT""PROJECT_OBJECT_GAPS"default"

/// sensor device management project
#define CLIENT_PROJECT      "client"
/** station/client management component */
#define STATION_COM         CLIENT_PROJECT""PROJECT_OBJECT_GAPS"station"
#define STATION_IPCAT_COM   CLIENT_PROJECT""PROJECT_OBJECT_GAPS"ipcat"
#define STATION_DHCPS_COM   CLIENT_PROJECT""PROJECT_OBJECT_GAPS"dhcps"
#define STATION_ACL_COM     CLIENT_PROJECT""PROJECT_OBJECT_GAPS"acl"
#define STATION_FILTER_COM  CLIENT_PROJECT""PROJECT_OBJECT_GAPS"filter"
#define STATION_TC_COM      CLIENT_PROJECT""PROJECT_OBJECT_GAPS"tc"

/// wireless project
#define WIFI_PROJECT    "wifi"
#define WIFI_AP_COM     "wifi@ap"
#define WIFI_STA_COM    "wifi@sta"
/** wireless ifname component */
#define NRADIO_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"n"
#define NSSID_COM       WIFI_PROJECT""PROJECT_OBJECT_GAPS"nssid"
#define NSSID2_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"nssid2"
#define NSSID3_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"nssid3"
#define NSTA_COM        WIFI_PROJECT""PROJECT_OBJECT_GAPS"nsta"
#define ARADIO_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"a"
#define ASSID_COM       WIFI_PROJECT""PROJECT_OBJECT_GAPS"assid"
#define ASSID2_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"assid2"
#define ASSID3_COM      WIFI_PROJECT""PROJECT_OBJECT_GAPS"assid3"
#define ASTA_COM        WIFI_PROJECT""PROJECT_OBJECT_GAPS"asta"

/// modem management project
#define MODEM_PROJECT    "modem"
/** modem operator component */
#define OPERATOR_COM     MODEM_PROJECT""PROJECT_OBJECT_GAPS"operator"
/** modem device component */
#define MODEM_COM        MODEM_PROJECT""PROJECT_OBJECT_GAPS"atd"
/** modem ifname component */
#define LTE_COM          MODEM_PROJECT""PROJECT_OBJECT_GAPS"lte"
#define LTE2_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"lte2"
#define LTE3_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"lte3"
#define LTE4_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"lte4"
/** modem sms service component */
#define SMSD_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"smsd"
/** modem sms object component */
#define SMS_COM          MODEM_PROJECT""PROJECT_OBJECT_GAPS"sms"
#define SMS2_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"sms2"
#define SMS3_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"sms3"
#define SMS4_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"sms4"
/** modem atport service component */
#define ATPROXY_COM      MODEM_PROJECT""PROJECT_OBJECT_GAPS"atproxy"
/** modem atport object component */
#define ATPORT_COM       MODEM_PROJECT""PROJECT_OBJECT_GAPS"atport"
#define ATPORT2_COM      MODEM_PROJECT""PROJECT_OBJECT_GAPS"atport2"
#define ATPORT3_COM      MODEM_PROJECT""PROJECT_OBJECT_GAPS"atport3"
#define ATPORT4_COM      MODEM_PROJECT""PROJECT_OBJECT_GAPS"atport4"
/** modem backup sim service component */
#define BSIMCARD_COM     MODEM_PROJECT""PROJECT_OBJECT_GAPS"bsimcard"
/** modem backup sim object component */
#define BSIM_COM         MODEM_PROJECT""PROJECT_OBJECT_GAPS"bsim"
#define BSIM2_COM        MODEM_PROJECT""PROJECT_OBJECT_GAPS"bsim2"
#define BSIM3_COM        MODEM_PROJECT""PROJECT_OBJECT_GAPS"bsim3"
#define BSIM4_COM        MODEM_PROJECT""PROJECT_OBJECT_GAPS"bsim4"

/// netcard(NIC) management project
#define NETCARD_PROJECT    "netcard"
/** netcard device component */
#define NETCARD_COM        NETCARD_PROJECT""PROJECT_OBJECT_GAPS"device"
/** netcard ifname component */
#define NIC_COM            NETCARD_PROJECT""PROJECT_OBJECT_GAPS"nic"
#define NIC2_COM           NETCARD_PROJECT""PROJECT_OBJECT_GAPS"nic2"

/// gnss project
#define GNSS_PROJECT       "gnss"
/** gnss frame component */
#define GNSS_COM           GNSS_PROJECT""PROJECT_OBJECT_GAPS"frame"
/** gnss device instances */
#define NMEA_COM           GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea"
#define NMEA2_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea2"
#define NMEA3_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea3"
#define NMEA4_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea4"
#define NMEA5_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea5"
#define NMEA6_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea6"
#define NMEA7_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea7"
#define NMEA8_COM          GNSS_PROJECT""PROJECT_OBJECT_GAPS"nmea8"
/// gnss driver management component
#define GNSSDRV_PROJECT    "gnssdrv"
#define GNSSDRV_ITEM       GNSSDRV_PROJECT
#define GNSSDRV_NMEA       GNSSDRV_PROJECT""PROJECT_OBJECT_GAPS"nmea"

/// uart management project
#define UART_PROJECT       "uart"
/** uart frame component */
#define UART_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"frame"
#define TTY_COM         UART_PROJECT""PROJECT_OBJECT_GAPS"tty"
#define TTY2_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty2"
#define TTY3_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty3"
#define TTY4_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty4"
#define TTY5_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty5"
#define TTY6_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty6"
#define TTY7_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty7"
#define TTY8_COM        UART_PROJECT""PROJECT_OBJECT_GAPS"tty8"
/// uart driver management component
#define UARTDRV_PROJECT   "uartdrv"
#define UARTDRV_ITEM      UARTDRV_PROJECT
#define UARTDRV_DTU       UARTDRV_PROJECT""PROJECT_OBJECT_GAPS"dtu"
#define UARTDRV_TUI       UARTDRV_PROJECT""PROJECT_OBJECT_GAPS"tui"
#define UARTDRV_GNSS      UARTDRV_PROJECT""PROJECT_OBJECT_GAPS"gnss"
#define UARTDRV_MODBUS    UARTDRV_PROJECT""PROJECT_OBJECT_GAPS"modbus"

/// uart modbus driver management component
#define MODBUSDRV_PROJECT  "modbusdrv"
#define MODBUSDRV_ITEM     MODBUSDRV_PROJECT

/// i2c device management project
#define I2CDRV_PROJECT     "i2cdrv"
#define I2CDRV_ITEM        I2CDRV_PROJECT

/// sensor device management project
#define SENSOR_PROJECT     "sensor"
#define SENSOR_ITEM        SENSOR_PROJECT



/// system important path
#define PROJECT_DIR 			 "/usr/share/skinos"
#define PROJECT_ETC_DIR 		 "/etc"
#if defined gPLATFORM__slave
	#define PROJECT_TMP_DIR 		 "/tmp/skinos"
	#define PROJECT_VAR_DIR 		 "/var/skinos"
	#define PROJECT_MNT_DIR 		 "/mnt/skinos"
	#define PROJECT_LIB_DIR          "/usr/local/lib"
	#define PROJECT_BIN_DIR          "/usr/local/bin"
	#define PROJECT_CFG_DIR 		 PROJECT_MNT_DIR"/config"
	#define PROJECT_REGISTER_LIB_DIR PROJECT_LIB_DIR
	#define PROJECT_REGISTER_BIN_DIR PROJECT_BIN_DIR
#else
	#define PROJECT_TMP_DIR 		 "/tmp"
	#define PROJECT_VAR_DIR 		 "/var"
	#define PROJECT_MNT_DIR 		 "/mnt"
	#define PROJECT_LIB_DIR          "/usr/lib"
	#define PROJECT_BIN_DIR          "/usr/bin"
	#define PROJECT_CFG_DIR          PROJECT_ETC_DIR"/config"
	#define PROJECT_REGISTER_LIB_DIR PROJECT_VAR_DIR"/lib"
	#define PROJECT_REGISTER_BIN_DIR PROJECT_VAR_DIR"/bin"
#endif
#define PROJECT_REG_DIR              PROJECT_TMP_DIR"/.reg"
#define PROJECT_SER_DIR              PROJECT_TMP_DIR"/.ser"
#define PROJECT_COM_DIR              PROJECT_TMP_DIR"/.com"
#define PROJECT_CAH_DIR              PROJECT_TMP_DIR"/.cah"
#define PROJECT_CONF_DIR             PROJECT_TMP_DIR"/.conf"
#define PROJECT_OEM_DIR              PROJECT_MNT_DIR"/.oem"
#define PROJECT_FAC_DIR              PROJECT_MNT_DIR"/.fac"
#define PROJECT_DBS_DIR 		     PROJECT_MNT_DIR"/dbs"
#define PROJECT_INT_DIR              PROJECT_MNT_DIR"/internal"
#define PROJECT_APP_DIR              PROJECT_MNT_DIR"/internal/skinos"
/* land@machine cfgversion / gpversion files (also bumped by config_set) */
#define MACHINE_CFGVERSION_PATH      PROJECT_CFG_DIR"/version"
#define MACHINE_GPVERSION_PATH       PROJECT_CFG_DIR"/gpversion"
#define SHELL_API_HEADER             PROJECT_DIR"/land/api.sh"
#define SHELL_SERVICE_HEADER         PROJECT_DIR"/land/serv.sh"
#define CONFIG_FILE_POSTFIX          ".cfg"
/// FPK important directory
#define FPK_LIB_DIR                  "lib"
#define FPK_BIN_DIR                  "bin"
#define FPK_ETC_DIR                  "etc"
#define FPK_INT_DIR                  "internal"
#define FPK_ROOTFS_DIR               "rootfs"
#define FPK_CONFIG_DIR               "config"
/// system important filename
#define RANDOM_FILENAME              "/etc/urandom.seed"
#define CRACKID_MAC                  "00:01:02:03:04:00"
#define ETC_HOSTS_FILE               "/etc/hosts"
#define ETC_DNS_FILE                 "/etc/resolv.conf"
#define ETC_DNS6_FILE                "/etc/resolv.conf.ipv6"
#define RESOLV_DIR                   PROJECT_TMP_DIR"/resolv"
#define PROC_ARP_FILE                "/proc/net/arp"
#define MASQ_CHAIN                   "masq"
/// URL for system
#define RATE_TEST_URL                "ftp://test.ashyelf.com"
#define FIRMWARE_STORE_URL           "ftp://repo.ashyelf.com"
#define FIRMWARE_VERIFY_URL          "http://verify.ashyelf.com"



/// include important header file first level
#include "link.h"
#include "talk.h"
#include "param.h"
#include "path.h"
#include "utility.h"
#include "config.h"
#include "dbs.h"
#include "project.h"
#include "com.h"
#include "register.h"
#include "mxtalk.h"
#include "munix.h"
#include "mcontrol.h"
/// include important header file second level
#include "log.h"
#include "he2com.h"
/// include important header file third level
#include "serv.h"



#endif   /* ----- #ifndef H_LAND_LANDHEAD_H  ----- */

