#ifndef H_CENTER_SKINCTR_H
#define H_CENTER_SKINCTR_H
/**
 * @brief Center相关的基本接口
 * @author dimmalex
 * @version 1.0
*/

#define HEPORT_NAME "heport"
#define PPORT_NAME "pport"
#define NPORT_NAME "nport"
#define HEPORT_HOSTNAME "HEPORT"

#define USER_DEVICE_DIR        "dev"                 /// DEVPORT_DIR/username/dev/
#define USER_NETWORK_DIR       "net"                 /// DEVPORT_DIR/username/net/
#define USER_FIRMWARE_DIR      "firmware"            /// DEVPORT_DIR/username/firmware/
#define USER_CONFIG_FILENAME   "config"              /// DEVPORT_DIR/username/config
#define USER_TCPMAP_FILENAME   "tcpmap"              /// DEVPORT_DIR/username/tcpmap
#define USER_UDPMAP_FILENAME   "udpmap"              /// DEVPORT_DIR/username/udpmap
#define DEVICE_REG_FILENAME    "reg"                 /// DEVPORT_DIR/username/dev/00037f121240/reg
#define DEVICE_CONFIG_FILENAME "config"              /// DEVPORT_DIR/username/dev/00037f121240/config
#define DEVICE_HEARTBEAT_FILENAME    "heartbeat"     /// DEVPORT_DIR/username/dev/00037f121240/heartbeat



/* 网页服务端口 */
extern int server_port;           // 20000(TCP)
/* 设备接入端口 */
extern int device_port;           // 20002(TCP)
/* API控制端口 */
extern int control_port;          // 20003(TCP)
/* pport接入端口 */
extern int pport_port;            // 20005(TCP)
extern int pport_dynamic_start;   // 20006(TCP)
extern int pport_static_start;    // 25000(TCP)



/* protocol gap */
#define HEPORT_CMD_GAPC     0x2d   // ENQ(Enquiry)        -
#define HECLIENT_ACK_GAPC   0x2b   // ACK(Acknowledge)    +
#define DATA_ITEM_GAPC      0x7c   // US(Unit Separator)  |
#define DATA_END_GAPC       0x00   // NUL(NULL)           \0

/**
 * @brief call the heport to execute a string he command
 * @param[in] mac, device mac identify
 * @param[in] cmd, string he command
 * @param[in] timeout, timeout to wait in second
 * @return json command result
 */
talk_t string_hh_execute( const char *macid, const char *cmd, int timeout );
/**
 * @brief call the heport to execute a string he command at line command
 * @param[in] mac, device mac identify
 * @param[in] cmd, string he command
 * @param[in] timeout, timeout to wait in second
 * @return succeed or failed, print the return at the terminal
 *		@retval 0 for succeed
 *		@retval other for failed
 */
int    line_hh_command( const char *macid, const char *cmd, int timeout );

/**
 * @brief call the heport to execute a json he command
 * @param[in] mac, device mac identify
 * @param[in] cmd, json he command
 * @param[in] timeout, timeout to wait in second
 * @return json command result
 */
talk_t json_hh_execute( const char *macid, talk_t v, int timeout );
/**
 * @brief call the heport to execute a talk that include list of he command
 * @param[in] mac, device mac identify
 * @param[in] cmd, json he command
 * @param[in] timeout, timeout to wait in second
 * @return json command result
 */
talk_t talk_hh_execute( const char *macid, talk_t helist, int timeout );

/**
 * @brief call heport service control (Unix JSON: list/knock/dump)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
talk_t heport_call( const char *cmd, talk_t v, int timeout );

/**
 * @brief call pport service control (Unix JSON: list/tcp_map/udp_map/...)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
talk_t pport_call( const char *cmd, talk_t v, int timeout );

/**
 * @brief call nport service control (Unix JSON: network_knock/endpoint_knock/dump/...)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
talk_t nport_call( const char *cmd, talk_t v, int timeout );



/**
 * @brief snprintf then reject unsafe path strings
 * @param buf output buffer
 * @param buflen size of buf
 * @param fmt printf format (same as snprintf)
 * @return like snprintf (>=0), or -1 with errno=EINVAL if format fails or result is unsafe
 * @details Drop-in for path snprintf. Rejects ".." and shell/control metacharacters
 *          (space ; & | ` $ ( ) < > " ' \\ * ? [ ] { } ! # ~ and controls) in the
 *          finished string. Does not ban '/' — nested names like user "a/b" still need
 *          a separate username whitelist if that must be forbidden.
 */
int safe_snprintf( char *buf, size_t buflen, const char *fmt, ... );



#endif   /* ----- #ifndef H_CENTER_SKINCTR_H  ----- */

