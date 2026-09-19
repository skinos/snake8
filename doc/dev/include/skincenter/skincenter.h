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

/*
 * HH result semantics (string/json/talk_hh_execute and string/talk_hh_submit cb):
 *
 *   ret              meaning
 *   --------------   ----------------------------------------------------------
 *   NULL             Peer returned empty success (not a local error).
 *   ttrue            Peer returned boolean success.
 *   talk > tpanic    Peer returned payload (string / JSON); caller talk_free().
 *   tfalse           Peer returned failure.
 *   terror           Peer returned error (keep errno from peer if present).
 *   tpanic           Local/call failure, wait miss, or libevent_hh_cancel() (see errno).
 *
 *   errno (when ret is tfalse/terror/tpanic; 0 means peer gave no code):
 *   EWOULDBLOCK /    No peer reply in time (sync: udp2talk wait; async: ack timer
 *   EAGAIN /         exhausted after retries). Same meaning on both paths.
 *   EINPROGRESS
 *   ECANCELED        Async only: libevent_hh_cancel() (local abort, not peer fail).
 *   EINVAL           Bad args or bad talk to serialize.
 *                    sync execute: usually tfalse+EINVAL; talk_hh_execute
 *                    json2string fail → tpanic+EINVAL.
 *                    submit: NULL session, no cb.
 *   ENOENT           heport control unix missing, etc.
 *   other            From peer encoding, connect/send, or libevent arm failure.
 *                    Async: errno is saved on the session and restored before cb
 *                    (so close/event cleanup cannot wipe peer/local errno).
 *
 * Success test used by callers (e.g. nport):
 *   (ret == NULL || ret == ttrue || ret > tpanic)
 *
 * Async submit only:
 *   Non-NULL session = in flight; cb runs once then session is destroyed.
 *   NULL session     = immediate reject (bad args / no heport / cannot arm); cb not called.
 *   After max_tries: wait miss → cb(tpanic) + EWOULDBLOCK; otherwise cb(last peer/local fail code).
 *   cancel() → cb(tpanic) + ECANCELED.
 */



/**
 * @brief call heport service control (Unix JSON: list/knock/dump)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
 talk_t heport_call( const char *cmd, talk_t v, int timeout );
 /**
 * @brief Execute a string HE command via heport (blocking).
 * @param macid device mac
 * @param cmd string HE command
 * @param timeout seconds to wait for reply (also bounds connect/send retry loop)
 * @return see "HH result semantics" above
 */
talk_t string_hh_execute( const char *macid, const char *cmd, int timeout );
/**
 * @brief Execute string HE and print the result (line/CLI helper).
 * @return 0 on success (NULL/ttrue/JSON); negative on tfalse/terror/tpanic
 */
int    line_hh_command( const char *macid, const char *cmd, int timeout );

/**
 * @brief Execute a JSON HE command via heport (blocking). Same semantics as string_hh_execute.
 */
talk_t json_hh_execute( const char *macid, talk_t v, int timeout );
/**
 * @brief Execute a talk/JSON HE list via heport (blocking). Same semantics as string_hh_execute.
 */
talk_t talk_hh_execute( const char *macid, talk_t helist, int timeout );

/**
 * Async HH completion callback. Session is invalid after return; caller must talk_free(ret) if ret > tpanic.
 * @param ret see "HH result semantics" above (including NULL peer success and tpanic+EWOULDBLOCK on wait miss)
 */
typedef void (*libevent_hh_done_t)( talk_t ret, void *arg );
typedef struct libevent_hh_struct *libevent_hh_t;

/**
 * Submit talk/JSON HE on event_base (non-blocking). Copies he; caller may talk_free(he) after return.
 * @return in-flight session, or NULL if rejected immediately (cb not called)
 * @note Local unix path heport.unix-<pid>-<fd>. Retries with a new fd up to max_tries.
 */
libevent_hh_t talk_hh_submit( struct event_base *base, const char *macid, talk_t he, int timeout_sec, int max_tries, libevent_hh_done_t cb, void *arg );
/**
 * Submit string HE on event_base. Same semantics as talk_hh_submit / string_hh_execute results in cb.
 */
libevent_hh_t string_hh_submit( struct event_base *base, const char *macid, const char *cmd, int timeout_sec, int max_tries, libevent_hh_done_t cb, void *arg );
/**
 * Cancel in-flight submit: cb(tpanic) once with errno=ECANCELED, then destroy session.
 */
void libevent_hh_cancel( libevent_hh_t s );



/**
 * @brief call pport service control (Unix JSON: list/tcp_map/udp_map/...)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
talk_t pport_call( const char *cmd, talk_t v, int timeout );
/**
 * Submit pport control on event_base (non-blocking). Same unix JSON as pport_call.
 * Takes ownership of v. Session is libevent_hh_t; cancel with talk_pport_cancel.
 * @return in-flight session, or NULL if rejected immediately (cb not called; v freed)
 */
libevent_hh_t talk_pport_submit( struct event_base *base, const char *cmd, talk_t v, int timeout_sec, int max_tries, libevent_hh_done_t cb, void *arg );
/**
 * Cancel in-flight submit: cb(tpanic) once with errno=ECANCELED, then destroy session.
 */
 #define talk_pport_cancel libevent_hh_cancel



/**
 * @brief call nport service control (Unix JSON: network_knock/endpoint_knock/dump/...)
 * @param[in] cmd control command name
 * @param[in] v argument json (ownership taken)
 * @param[in] timeout timeout in seconds
 * @return talk result
 */
talk_t nport_call( const char *cmd, talk_t v, int timeout );
/**
 * Submit nport control on event_base (non-blocking). Same unix JSON as nport_call.
 * Takes ownership of v. Session is libevent_hh_t; cancel with talk_nport_cancel.
 * @return in-flight session, or NULL if rejected immediately (cb not called; v freed)
 */
libevent_hh_t talk_nport_submit( struct event_base *base, const char *cmd, talk_t v, int timeout_sec, int max_tries, libevent_hh_done_t cb, void *arg );
/**
 * Cancel in-flight submit: cb(tpanic) once with errno=ECANCELED, then destroy session.
 */
#define talk_nport_cancel libevent_hh_cancel



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

