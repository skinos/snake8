#ifndef H_MODEM_SKINMODEM_H
#define H_MODEM_SKINMODEM_H
/**
 * @brief modem相关的基本接口
 * @author dimmalex
 * @version 1.0
*/



/// log for modem
extern const char       *atd_object;
#define modem_verbose( ... )   modem_log( LANDLOG_VERBOSE, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_debug( ... )     modem_log( LANDLOG_DEBUG, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_info( ... )      modem_log( LANDLOG_INFO, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_warn( ... )      modem_log( LANDLOG_WARN, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_warning( ... )   modem_log( LANDLOG_WARN|LANDLOG_ERRNO, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_fault( ... )     modem_log( LANDLOG_FAULT, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
#define modem_faulting( ... )  modem_log( LANDLOG_FAULT|LANDLOG_ERRNO, atd_object, (__FILE__), (__LINE__),  __VA_ARGS__ )
/* modem defalut log function */
void    modem_log( int level, const char *object, const char *file, int line, const char *format, ... );
#define OPERATOR_CONFIG "modem@operator"
/* modem defalut log function */
talk_t  modem_operator( talk_t state );
/* rsrp 2 rssi */
int     rsrp2rssi( int rsrp );
/* rsrp rssi 2 sinr */
void    rsrprssi2sinr( talk_t state );
/* Prefer RSRP bars when present; else RSSI. Writes signal/signal2. */
boole   rssirsrp2signal( talk_t state );



#define MODEM_OBJECT_MAX 10
/* modem object name allocation */
const char *modem_alloc( const char *syspath, const char *id, talk_t matchcfg, char *buf, int buflen );
/* modem object other name allocation */
const char *modem_relate( const char *object, const char *prefix, char *buf, int buflen );
/* modem object name free */
void        modem_free( const char *object );
/* usb tty device path list find, return >0 is find device number, return 0 for nofound*/
int         modem_ttylist( const char *syspath, char device[][NAME_MAX] );
/* usb net device path list find, return >0 is find device number, return 0 for nofound*/
int         modem_netlist( const char *syspath, char device[][NAME_MAX] );



/* function type atd reply the client request */
typedef int    (*atd_reply_t)( talk_t ack, int err, void *unit );
/* function type for check the ack of at command completeness */
typedef int    (*atcmd_intact_t)( void *atcmd, talk_t state, talk_t cfg );
/* function type for parser the ack of at command */
typedef talk_t (*atcmd_parser_t)( void *atcmd, talk_t state, talk_t cfg );
/* function type for register a at command to atd */
typedef void  *(*atcmd_create_t)( boole insert, 
					int exnum, const char **expect,
					atcmd_intact_t intact, atcmd_parser_t parser,
					int timeout, int retry,
					void *arg, void *reply, void *unit,
					const char *cmdformat, ... );
/* at command struct */
#define ATCMD_LINE_MAX 20
#define ATCMD_EXPECT_MAX 20
typedef struct atcmd_struct
{
    link_struct glink;

	int retry;
	int timeout;

	void *arg;
	char cmd[LINE_MAX];

	void *unit;
	void *reply;

    struct timeval tv;
    struct event *timer;

	atcmd_intact_t intact;
	atcmd_parser_t parser;

	int   line;
	char *ack[ATCMD_LINE_MAX];

	int   exnum;
	char *expect[ATCMD_EXPECT_MAX];
} atcmd_st;
typedef atcmd_st* atcmd_t;



/* Intact: ack is complete (parser runs). Failed → wait for more UART. */
#define INTACT_FAILED      0x00
#define INTACT_SUCCEED     0x01
/* Last +C*REG status for PARSE_COPS when COPS has no PLMN: 1 ok, 2 noreg, 3 dereg, 4 unreg. */
extern int PARSE_creg_code;
/* "OK" or "ERROR" in any line. */
int INTACT_OK( void *ctx, talk_t state, talk_t cfg );
/* expect[] token or "ERROR". Empty expect → succeed now. */
int INTACT_LINE( void *ctx, talk_t state, talk_t cfg );
/* Same as INTACT_LINE after at least 2 ack lines. */
int INTACT_LINE2( void *ctx, talk_t state, talk_t cfg );
/* Same as INTACT_LINE after at least 3 ack lines. */
int INTACT_LINE3( void *ctx, talk_t state, talk_t cfg );
/*
 * Parsers. ERROR / no OK → tfalse or NULL (retry, stay in this ATD).
 * OK2* return an ATD session verb, not a boolean.
 */
/* OK → ttrue, continue AT queue. */
talk_t PARSE_OK( void *ctx, talk_t state, talk_t cfg );
/* OK → terror / ATD_EXIT; _service tfalse, daemon reruns same tty. */
talk_t PARSE_OK2EXIT( void *ctx, talk_t state, talk_t cfg );
/* OK → state["exit"]="search" / ATD_EXIT; devbus.search, _service terror. */
talk_t PARSE_OK2SEARCH( void *ctx, talk_t state, talk_t cfg );
/* OK → tpanic / ATD_RESET; GPIO usb.reset, _service terror. */
talk_t PARSE_OK2RESET( void *ctx, talk_t state, talk_t cfg );
/* AT+CGSN. Writes state["imei"]. */
talk_t PARSE_CGNS( void *ctx, talk_t state, talk_t cfg );
/* AT+CPIN?. READY → ttrue; PIN/PUK → "pin"/"puk"; ERROR → iccid "nosim". */
talk_t PARSE_CARD( void *ctx, talk_t state, talk_t cfg );
/* PIN set ack. OK → ttrue, ERROR → tfalse. */
talk_t PARSE_PIN( void *ctx, talk_t state, talk_t cfg );
/* AT+CIMI. Writes state["imsi"]. */
talk_t PARSE_CIMI( void *ctx, talk_t state, talk_t cfg );
/* AT+CCID. Writes state["iccid"]. */
talk_t PARSE_CCID( void *ctx, talk_t state, talk_t cfg );
/* AT+C*REG?. Sets PARSE_creg_code; lac/ci when registered (not all-F). */
talk_t PARSE_CREG( void *ctx, talk_t state, talk_t cfg );
/* AT+COPS?. Writes state["plmn"], or noreg/dereg/unreg from PARSE_creg_code. */
talk_t PARSE_COPS( void *ctx, talk_t state, talk_t cfg );
/* AT+CSQ. Writes state["csq"]; 99 (unknown) is ignored. */
talk_t PARSE_CSQ( void *ctx, talk_t state, talk_t cfg );
/* AT+CCLK?. Sets system time and timezone from the module clock. */
talk_t PARSE_CCLK( void *ctx, talk_t state, talk_t cfg );
/* send a at command in atd or in modem driver */
extern atcmd_create_t atd_register;
#define ATD_OK( timeout, retry, reply, unix, ... )                          ((atcmd_create_t)(atd_register))( false, 0, NULL, INTACT_OK, PARSE_OK, timeout, retry, NULL, reply, unix, __VA_ARGS__ )
#define ATD_AT( parser, timeout, retry, reply, unix, ... )                  ((atcmd_create_t)(atd_register))( false, 0, NULL, INTACT_OK, parser, timeout, retry, NULL, reply, unix, __VA_ARGS__ )
#define ATD_CMD( parser, timeout, retry, arg, reply, unix, ... )            ((atcmd_create_t)(atd_register))( false, 0, NULL, INTACT_OK, parser, timeout, retry, arg, reply, unix, __VA_ARGS__ )
#define ATD_SEND( intact, parser, timeout, retry, arg, reply, unix, ... )   ((atcmd_create_t)(atd_register))( false, 0, NULL, intact, parser, timeout, retry, arg, reply, unix, __VA_ARGS__ )
#define ATD_INSERT( intact, parser, timeout, retry, arg, reply, unix, ... ) ((atcmd_create_t)(atd_register))( true, 0, NULL, intact, parser, timeout, retry, arg, reply, unix, __VA_ARGS__ )



typedef int (*modem_init_t)( void *reg, const char *obj, talk_t state, talk_t cfg );
typedef int (*modem_handler_t)( talk_t v, atd_reply_t reply, void *unit );
typedef int (*modem_urc_t)( int line, char *ack[] );



#endif   /* ----- #ifndef H_MODEM_SKINMODEM_H  ----- */
