/*
 *  Description:  ISIOT-4xx gas detector Modbus reader for uart framework
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 *
 *  Data flow:
 *    1. Open Modbus RTU on the uart tty
 *    2. Every read_interval milliseconds, read gas registers
 *    3. Expose values via status / read (used by gas@jrzh and others)
 *
 *  Registration: uartdrv@isiot4xx (via prj.json "obj" section)
 *  Started by:   uart@frame via sstarts( uart_object, "uartdrv@isiot4xx", "service", ... )
 */

#include "skin/skin.h"
#include "skinuart/skinuart.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <modbus/modbus.h>
#include <stdio.h>

/* ---- logging macros ---- */
#define isiot_debug( ... )    uart_debug( uart_object, __VA_ARGS__ )
#define isiot_info( ... )     uart_info( uart_object, __VA_ARGS__ )
#define isiot_warn( ... )     uart_warn( uart_object, __VA_ARGS__ )
#define isiot_warning( ... )  uart_warning( uart_object, __VA_ARGS__ )
#define isiot_fault( ... )    uart_fault( uart_object, __VA_ARGS__ )
#define isiot_faulting( ... ) uart_faulting( uart_object, __VA_ARGS__ )

/* ---- gas definition table ---- */
typedef struct {
    const char *name;       /* gas name (used as config key) */
    int divisor;            /* raw_value / divisor = actual_value */
} gas_def_t;

static const gas_def_t gas_defs[] = {
    /* name   divisor */
    { "O2",   100 },
    { "CO",   1 },
    { "H2S",  10 },
    { "CH4",  10 },
    { "C6H6", 10 },
    { "H2",   10 },
};
#define GAS_DEFS_COUNT (sizeof(gas_defs) / sizeof(gas_defs[0]))
#define MODBUS_REG_COUNT  4

static const gas_def_t *gas_lookup( const char *name )
{
    int i;
    if ( name == NULL || *name == '\0' )
    {
        return NULL;
    }
    for( i = 0; i < (int)GAS_DEFS_COUNT; i++ )
    {
        if ( strcasecmp( gas_defs[i].name, name ) == 0 )
        {
            return &gas_defs[i];
        }
    }
    return NULL;
}

/* ---- global state ---- */
int modbus_addr = 1;
int read_interval = 100;
int modbus_timeout = 1;
modbus_t *mb_ctx = NULL;

int tty_fd = -1;
boole tty_power;
const char *uart_object;
struct event_base *uart_base;

const gas_def_t *cfg_reg_gas[MODBUS_REG_COUNT];
uint16_t gas_raw[MODBUS_REG_COUNT];
double gas_value[MODBUS_REG_COUNT];
long long gas_lasttime;



static int modbus_read_gas( void )
{
    uint16_t raw[MODBUS_REG_COUNT];
    int ret;
    int i;
    int retry;

    if ( mb_ctx == NULL )
    {
        return -1;
    }

    modbus_flush( mb_ctx );

    for( retry = 0; retry < 3; retry++ )
    {
        ret = modbus_read_registers( mb_ctx, 0, MODBUS_REG_COUNT, raw );
        if ( ret == MODBUS_REG_COUNT )
        {
            for( i = 0; i < MODBUS_REG_COUNT; i++ )
            {
                gas_raw[i] = raw[i];
                if ( cfg_reg_gas[i] != NULL )
                {
                    gas_value[i] = (double)raw[i] / cfg_reg_gas[i]->divisor;
                }
                else
                {
                    gas_value[i] = (double)raw[i];
                }
            }
            gas_lasttime = uptime_int();
            return 0;
        }
        if ( ret < 0 )
        {
            isiot_warn( "modbus read failed (retry %d): %s", retry, modbus_strerror(errno) );
        }
        else
        {
            isiot_warn( "modbus read incomplete (retry %d): got %d of %d", retry, ret, MODBUS_REG_COUNT );
        }
        modbus_flush( mb_ctx );
    }

    return -1;
}



void isiot4xx_tick( int fd, short what, void *arg )
{
    (void)fd;
    (void)what;
    (void)arg;
    modbus_read_gas();
}



talk_t isiot4xx_control( const char *cmd, talk_t value )
{
    talk_t ret;
    int i;

    (void)value;
    ret = NULL;

    if ( 0 == strcmp( cmd, "status" ) )
    {
        ret = json_create( NULL );
        json_set_number( ret, "last_read", gas_lasttime );
        for( i = 0; i < MODBUS_REG_COUNT; i++ )
        {
            if ( cfg_reg_gas[i] != NULL )
            {
                char valbuf[32];
                snprintf( valbuf, sizeof(valbuf), "%.2f", gas_value[i] );
                json_set_string( ret, cfg_reg_gas[i]->name, valbuf );
            }
        }
    }
    else if ( 0 == strcmp( cmd, "read" ) )
    {
        if ( modbus_read_gas() == 0 )
        {
            ret = json_create( NULL );
            for( i = 0; i < MODBUS_REG_COUNT; i++ )
            {
                if ( cfg_reg_gas[i] != NULL )
                {
                    char valbuf[32];
                    snprintf( valbuf, sizeof(valbuf), "%.2f", gas_value[i] );
                    json_set_string( ret, cfg_reg_gas[i]->name, valbuf );
                }
            }
        }
        else
        {
            ret = tfalse;
        }
    }

    return ret;
}

void isiot4xx_unix( int fd, short what, void *arg )
{
    talk_t r;
    talk_t a;
    int nread;
    socklen_t socklen;
    struct sockaddr_un sockaddr;
    char request_string[JSON_LINE_MAX];

    do
    {
        socklen = sizeof(sockaddr);
        nread = recvfrom( fd, request_string, sizeof(request_string)-1, 0,
                          (struct sockaddr*)&sockaddr, &socklen );
        if ( nread < 0 )
        {
            if ( errno != EWOULDBLOCK && errno != EINPROGRESS )
            {
                isiot_faulting( "unix recvfrom error" );
                event_base_loopbreak( uart_base );
                return;
            }
            break;
        }
        else if ( nread == 0 )
        {
            continue;
        }
        request_string[nread] = '\0';
        r = string2json( request_string );
        if ( r == NULL )
        {
            a = tfalse;
            errno = ENOMSG;
        }
        else
        {
            a = isiot4xx_control( json_string( r, "cmd" ), json_value( r, "v" ) );
        }
        talk2udp( fd, a, errno, (struct sockaddr*)&sockaddr, socklen, 0 );
        talk_free( r );
        talk_free( a );
    } while( nread >= 0 );
}

talk_t isiot4xx_call( const char *object, const char *cmd, talk_t v, int timeout )
{
    int i;
    int fd;
    int err;
    talk_t ret;
    talk_t req;
    struct stat st;
    char path[PATH_MAX];
    char control[PATH_MAX];

    if ( cmd == NULL || *cmd == '\0' )
    {
        talk_free( v );
        errno = EINVAL;
        return terror;
    }
    project_var_path( control, sizeof(control), PROJECT_ID, "%s.unix", object );
    if ( stat( control, &st ) != 0 )
    {
        talk_free( v );
        errno = ENOENT;
        return tpanic;
    }

    err = 0;
    ret = tpanic;
    req = json_create( NULL );
    json_set_json( req, "v", v );
    json_set_string( req, "cmd", cmd );
    project_var_path( path, sizeof(path), PROJECT_ID, "%s.unix-%d", object, getpid() );
    i = 0;
    while( i < timeout )
    {
        fd = unix_connect( control, path, SOCK_DGRAM );
        if ( fd < 0 )
        {
            err = errno;
            break;
        }
        if ( talk2udp( fd, req, 0, NULL, 0, timeout ) <= 0 )
        {
            err = errno;
        }
        else
        {
            ret = udp2talk( fd, NULL, NULL, timeout );
            close( fd );
            break;
        }
        close( fd );
        sleep( 1 );
        i++;
    }
    unlink( path );
    talk_free( req );
    if ( err )
    {
        errno = err;
    }
    return ret;
}

void isiot4xx_exit( int fd, short what, void *arg )
{
    (void)fd;
    (void)what;
    (void)arg;
    event_base_loopbreak( uart_base );
}

void isiot4xx_nodo( int fd, short what, void *arg )
{
    (void)fd;
    (void)what;
    (void)arg;
    return;
}



boole_t _service( obj_t this, param_t param )
{
    int sock;
    talk_t cfg;
    talk_t iot;
    const char *ptr;
    const char *devcom;
    const char *ttydev;
    struct timeval tv;
    struct event *ev_timer = NULL;
    struct event *ev_unix = NULL;
    struct event *ev_sigint = NULL;
    struct event *ev_sigterm = NULL;
    struct event *ev_sigpipe = NULL;
    struct event_config *econfig = NULL;
    char control_path[PATH_MAX];
    char buffer[PATH_MAX];
    int i;

    (void)this;
    uart_object = param_string( param, 1 );
    if ( uart_object == NULL )
    {
        isiot_fault( "cannot find the uart object" );
        return terror;
    }
    ttydev = param_string( param, 2 );
    devcom = param_string( param, 3 );
    if ( ttydev == NULL )
    {
        if ( devcom != NULL )
        {
            ttydev = scall_string( buffer, sizeof(buffer), devcom, "ttydev", NULL );
            if ( ttydev == NULL )
            {
                isiot_fault( "cannot find the ttydev" );
                sleep( 10 );
                return tfalse;
            }
        }
        else
        {
            isiot_fault( "cannot find the ttydev" );
            return terror;
        }
    }

    cfg = config_sget( uart_object, NULL );
    if ( cfg == NULL )
    {
        isiot_fault( "cannot find the configure" );
        return terror;
    }
    iot = json_value( cfg, "isiot4xx" );

    tty_fd = -1;
    tty_power = false;
    gas_lasttime = 0;
    memset( gas_raw, 0, sizeof(gas_raw) );
    memset( gas_value, 0, sizeof(gas_value) );
    memset( cfg_reg_gas, 0, sizeof(cfg_reg_gas) );

    ptr = json_string( iot, "modbus_addr" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        modbus_addr = atoi( ptr );
        if ( modbus_addr < 1 || modbus_addr > 253 )
        {
            modbus_addr = 1;
        }
    }
    ptr = json_string( iot, "read_interval" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        read_interval = atoi( ptr );
        if ( read_interval < 1 )
        {
            read_interval = 1;
        }
    }
    ptr = json_string( iot, "modbus_timeout" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        modbus_timeout = atoi( ptr );
        if ( modbus_timeout < 1 )
        {
            modbus_timeout = 1;
        }
    }

    cfg_reg_gas[0] = gas_lookup( "O2" );
    cfg_reg_gas[1] = gas_lookup( "CO" );
    cfg_reg_gas[2] = gas_lookup( "H2S" );
    cfg_reg_gas[3] = gas_lookup( "CH4" );

    for( i = 0; i < MODBUS_REG_COUNT; i++ )
    {
        char reg_key[32];
        const gas_def_t *g;

        snprintf( reg_key, sizeof(reg_key), "modbus_reg%d", i );
        ptr = json_string( iot, reg_key );
        if ( ptr != NULL && *ptr != '\0' )
        {
            g = gas_lookup( ptr );
            if ( g != NULL )
            {
                cfg_reg_gas[i] = g;
                isiot_info( "%s bound to %s", reg_key, g->name );
            }
            else
            {
                isiot_warn( "%s: unknown gas '%s', keeping default", reg_key, ptr );
            }
        }
    }

    econfig = event_config_new();
    event_config_require_features( econfig, EV_FEATURE_FDS );
    uart_base = event_base_new_with_config( econfig );
    event_config_free( econfig );
    if ( uart_base == NULL )
    {
        isiot_fault( "event_base_new failed" );
        talk_free( cfg );
        return tfalse;
    }

    ev_sigint = evsignal_new( uart_base, SIGINT, isiot4xx_exit, NULL );
    if ( ev_sigint == NULL || event_add( ev_sigint, NULL ) < 0 )
    {
        isiot_warn( "SIGINT handler setup failed" );
    }
    ev_sigterm = evsignal_new( uart_base, SIGTERM, isiot4xx_exit, NULL );
    if ( ev_sigterm == NULL || event_add( ev_sigterm, NULL ) < 0 )
    {
        isiot_warn( "SIGTERM handler setup failed" );
    }
    ev_sigpipe = evsignal_new( uart_base, SIGPIPE, isiot4xx_nodo, NULL );
    if ( ev_sigpipe == NULL || event_add( ev_sigpipe, NULL ) < 0 )
    {
        isiot_warn( "SIGPIPE handler setup failed" );
    }

    mb_ctx = modbus_new_rtu( ttydev, 9600, 'N', 8, 1 );
    if ( mb_ctx == NULL )
    {
        isiot_fault( "modbus_new_rtu failed" );
        goto failed;
    }
    modbus_set_slave( mb_ctx, modbus_addr );
    modbus_set_response_timeout( mb_ctx, modbus_timeout, 0 );

    if ( modbus_connect( mb_ctx ) == -1 )
    {
        isiot_fault( "modbus_connect failed: %s", modbus_strerror(errno) );
        modbus_free( mb_ctx );
        mb_ctx = NULL;
        goto failed;
    }

    ev_timer = event_new( uart_base, -1, EV_TIMEOUT|EV_PERSIST, isiot4xx_tick, NULL );
    if ( ev_timer == NULL )
    {
        isiot_fault( "timer event create failed" );
        goto failed;
    }
    tv.tv_sec = read_interval / 1000;
    tv.tv_usec = ( read_interval % 1000 ) * 1000;
    if ( evtimer_add( ev_timer, &tv ) < 0 )
    {
        isiot_fault( "timer event add failed" );
        goto failed;
    }

    isiot4xx_tick( -1, 0, NULL );

    project_var_path( control_path, sizeof(control_path), PROJECT_ID, "%s.unix", uart_object );
    sock = unix_listen( control_path, SOCK_DGRAM );
    if ( sock < 0 )
    {
        isiot_warning( "unix_listen(%s) error", control_path );
    }
    else
    {
        fd_nonblock( sock );
        socket_reuse( sock );
        ev_unix = event_new( uart_base, sock, EV_READ|EV_PERSIST, isiot4xx_unix, NULL );
        if ( ev_unix == NULL || event_add( ev_unix, NULL ) < 0 )
        {
            isiot_warn( "unix event setup failed" );
            close( sock );
        }
    }

    isiot_info( "isiot4xx service started on %s", ttydev );
    event_base_dispatch( uart_base );

    isiot_info( "isiot4xx service exiting" );

failed:
    if ( mb_ctx != NULL )
    {
        modbus_close( mb_ctx );
        modbus_free( mb_ctx );
        mb_ctx = NULL;
    }
    if ( ev_timer != NULL )
    {
        event_free( ev_timer );
    }
    if ( ev_unix != NULL )
    {
        event_free( ev_unix );
    }
    if ( ev_sigpipe != NULL )
    {
        event_free( ev_sigpipe );
    }
    if ( ev_sigterm != NULL )
    {
        event_free( ev_sigterm );
    }
    if ( ev_sigint != NULL )
    {
        event_free( ev_sigint );
    }
    if ( tty_fd >= 0 )
    {
        close( tty_fd );
    }
    if ( uart_base != NULL )
    {
        event_base_free( uart_base );
    }
    talk_free( cfg );
    if ( tty_power == true )
    {
        isiot_fault( "reset the tty %s", ttydev );
        scall( uart_object, "power", NULL );
    }
    return tfalse;
}



talk_t _status( obj_t this, param_t param )
{
    const char *object;

    (void)this;
    object = param_string( param, 1 );
    if ( object == NULL )
    {
        return NULL;
    }
    if ( spid( object ) <= 0 )
    {
        return NULL;
    }
    return isiot4xx_call( object, "status", NULL, 3 );
}

talk_t _read( obj_t this, param_t param )
{
    const char *object;

    (void)this;
    object = param_string( param, 1 );
    if ( object == NULL )
    {
        return terror;
    }
    if ( spid( object ) <= 0 )
    {
        return tfalse;
    }
    return isiot4xx_call( object, "read", NULL, 3 );
}



static const eapi_table_t exe_api_table[] = {
    { "service",  (comapi_t)_service },
    { "status",   (comapi_t)_status },
    { "read",     (comapi_t)_read }
};
MAIN2API( exe_api_table );
