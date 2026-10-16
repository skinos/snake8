/*
 *  Description:  OpenVPN server (ovpns@server) and per-client objects
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/md5.h>

#define RUNDIR              "/tmp/ovpns"
#define DEVNAME             "ovpns0"
#define SERIAL_FILE         RUNDIR"/serial.json"
#define STATUS_FILE         RUNDIR"/status"
#define MGMT_SOCK           RUNDIR"/mgmt"
#define CONF_FILE           RUNDIR"/server.conf"
#define PASSWD_FILE         RUNDIR"/passwd"
#define ASKPASS_FILE        RUNDIR"/askpass"
#define CCD_DIR             RUNDIR"/ccd"
#define CRL_DIR             RUNDIR"/crl"


static int mask_prefix( uint32_t mask, int *prefix )
{
    int i;
    int p;
    int seen0;

    p = 0;
    seen0 = 0;
    for ( i = 31; i >= 0; i-- )
    {
        if ( mask & ( 1u << i ) )
        {
            if ( seen0 )
            {
                return -1;
            }
            p++;
        }
        else
        {
            seen0 = 1;
        }
    }
    *prefix = p;
    return 0;
}

/* dec = dec * 16 + digit, decimal string without leading zeros */
static int dec_mul16_add( char *dec, int declen, int digit )
{
    int i;
    int carry;
    int n;
    char tmp[160];

    n = (int)strlen( dec );
    carry = digit;
    tmp[n] = '\0';
    for ( i = n - 1; i >= 0; i-- )
    {
        carry += ( dec[i] - '0' ) * 16;
        tmp[i] = (char)( '0' + ( carry % 10 ) );
        carry /= 10;
    }
    if ( carry > 0 )
    {
        if ( n + 2 >= declen )
        {
            return -1;
        }
        memmove( dec + 1, tmp, (size_t)n + 1 );
        dec[0] = (char)( '0' + carry );
    }
    else
    {
        memcpy( dec, tmp, (size_t)n + 1 );
    }
    return 0;
}

static int normalize_serial( const char *in, char *out, int outlen )
{
    const char *p;
    int hex;
    char dec[160];

    if ( in == NULL )
    {
        return -1;
    }
    p = in;
    while ( *p == ' ' || *p == '\t' )
    {
        p++;
    }
    if ( strncmp( p, "serial=", 7 ) == 0 )
    {
        p += 7;
    }
    if ( p[0] == '0' && ( p[1] == 'x' || p[1] == 'X' ) )
    {
        p += 2;
        hex = 1;
    }
    else
    {
        hex = 0;
        if ( strpbrk( p, "abcdefABCDEF" ) != NULL )
        {
            hex = 1;
        }
    }
    if ( *p == '\0' )
    {
        return -1;
    }
    if ( !hex )
    {
        const char *q;

        for ( q = p; *q != '\0'; q++ )
        {
            if ( !isdigit( (unsigned char)*q ) )
            {
                return -1;
            }
        }
        if ( (int)strlen( p ) >= outlen )
        {
            return -1;
        }
        strncpy( out, p, (size_t)outlen - 1 );
        out[outlen - 1] = '\0';
        {
            char *z;

            z = out;
            while ( *z == '0' && *( z + 1 ) != '\0' )
            {
                z++;
            }
            if ( z != out )
            {
                memmove( out, z, strlen( z ) + 1 );
            }
        }
        return 0;
    }
    strncpy( dec, "0", sizeof(dec) - 1 );
    for ( ; *p != '\0'; p++ )
    {
        int d;

        if ( *p >= '0' && *p <= '9' )
        {
            d = *p - '0';
        }
        else if ( *p >= 'a' && *p <= 'f' )
        {
            d = *p - 'a' + 10;
        }
        else if ( *p >= 'A' && *p <= 'F' )
        {
            d = *p - 'A' + 10;
        }
        else
        {
            return -1;
        }
        if ( dec_mul16_add( dec, (int)sizeof(dec), d ) != 0 )
        {
            return -1;
        }
    }
    {
        char *z;

        z = dec;
        while ( *z == '0' && *( z + 1 ) != '\0' )
        {
            z++;
        }
        if ( z != dec )
        {
            memmove( dec, z, strlen( z ) + 1 );
        }
    }
    if ( (int)strlen( dec ) >= outlen )
    {
        return -1;
    }
    strncpy( out, dec, (size_t)outlen - 1 );
    out[outlen - 1] = '\0';
    return 0;
}

static const char *client_cn( talk_t cfg, const char *object, char *buf, int n )
{
    const char *cn;
    const char *suf;

    cn = json_string( cfg, "cn" );
    suf = object != NULL ? object : "";
    if ( cn != NULL && *cn != '\0' )
    {
        suf = cn;
    }
    else if ( object != NULL )
    {
        const char *p;

        p = strrchr( object, PROJECT_OBJECT_GAPC );
        if ( p != NULL && *( p + 1 ) != '\0' )
        {
            suf = p + 1;
        }
    }
    strncpy( buf, suf, (size_t)n - 1 );
    buf[n - 1] = '\0';
    return buf;
}


/* Walk saved ovpns@* configs. Caller frees each cfg. */
static talk_t each_client( talk_t *list, talk_t *axp, const char **name )
{
    talk_t cfg;

    if ( *list == NULL )
    {
        *list = config_list( PROJECT_ID );
        *axp = NULL;
    }
    if ( *list == NULL )
    {
        return NULL;
    }
    while ( NULL != ( *axp = json_next( *list, *axp ) ) )
    {
        *name = axp_name( *axp );
        if ( *name == NULL || strcmp( *name, COM_IDPATH ) == 0 )
        {
            continue;
        }
        cfg = config_sget( *name, NULL );
        if ( cfg != NULL )
        {
            return cfg;
        }
    }
    return NULL;
}

static int normalize_revoke( talk_t cfg )
{
    talk_t rev;
    talk_t axp;
    talk_t seen;
    const char *key;
    const char *serial;
    char dec[160];

    rev = json_json( cfg, "revoke" );
    if ( rev == NULL )
    {
        return 0;
    }
    seen = json_create( NULL );
    axp = NULL;
    while ( NULL != ( axp = json_next( rev, axp ) ) )
    {
        talk_t one;

        key = axp_name( axp );
        one = axp_json( axp );
        serial = json_string( one, "serial" );
        if ( serial == NULL || *serial == '\0' )
        {
            vpn_warn( "revoke %s has an empty serial", key ? key : "" );
            talk_free( seen );
            return -1;
        }
        if ( normalize_serial( serial, dec, (int)sizeof(dec) ) != 0 )
        {
            vpn_warn( "revoke %s serial is not a number", key ? key : "" );
            talk_free( seen );
            return -1;
        }
        if ( json_string( seen, dec ) != NULL )
        {
            vpn_warn( "revoke serial %s is duplicated", dec );
            talk_free( seen );
            return -1;
        }
        json_set_string( seen, dec, key ? key : "" );
        json_set_string( one, "serial", dec );
    }
    talk_free( seen );
    return 0;
}

static int check_clients( talk_t server )
{
    talk_t list;
    talk_t axp;
    talk_t cfg;
    talk_t seen_cn;
    talk_t seen_ip;
    talk_t nets;
    const char *name;
    const char *auth;
    const char *dev;
    const char *topology;
    const char *pool_s;
    const char *pool_e;
    uint32_t network;
    uint32_t mask;
    uint32_t server_ip;
    uint32_t pstart;
    uint32_t pend;
    int have_pool;
    int tun;
    int net30;
    int prefix;
    char cn[256];
    char ipbuf[64];
    struct in_addr ina;

    auth = json_string( server, "auth" );
    if ( auth == NULL || *auth == '\0' )
    {
        auth = "certificate";
    }
    dev = json_string( server, "dev" );
    if ( dev == NULL || *dev == '\0' )
    {
        dev = "tun";
    }
    tun = strcmp( dev, "tun" ) == 0;
    topology = json_string( server, "topology" );
    if ( topology == NULL || *topology == '\0' )
    {
        topology = "subnet";
    }
    net30 = tun && strcmp( topology, "net30" ) == 0;
    have_pool = 0;
    network = 0;
    mask = 0;
    server_ip = 0;
    pstart = 0;
    pend = 0;
    if ( tun )
    {
        if ( ((json_string( server, "tunnelip" )) == NULL || *(json_string( server, "tunnelip" )) == '\0' || inet_pton(AF_INET, (json_string( server, "tunnelip" )), &ina) != 1 ? -1 : ((network) = ntohl(ina.s_addr), 0)) != 0
             || ((json_string( server, "tunnelmask" )) == NULL || *(json_string( server, "tunnelmask" )) == '\0' || inet_pton(AF_INET, (json_string( server, "tunnelmask" )), &ina) != 1 ? -1 : ((mask) = ntohl(ina.s_addr), 0)) != 0
             || mask_prefix( mask, &prefix ) != 0 || prefix < 8 || prefix > 30 )
        {
            vpn_warn( "tunnel network or mask is invalid" );
            return -1;
        }
        if ( ( network & mask ) != network )
        {
            vpn_warn( "tunnelip must be the network address" );
            return -1;
        }
        server_ip = network + 1;
        pool_s = json_string( server, "poolstart" );
        pool_e = json_string( server, "poolend" );
        if ( ( pool_s == NULL || *pool_s == '\0' ) != ( pool_e == NULL || *pool_e == '\0' ) )
        {
            vpn_warn( "pool start and end must both be set or both empty" );
            return -1;
        }
        if ( pool_s != NULL && *pool_s != '\0' )
        {
            if ( ((pool_s) == NULL || *(pool_s) == '\0' || inet_pton(AF_INET, (pool_s), &ina) != 1 ? -1 : ((pstart) = ntohl(ina.s_addr), 0)) != 0 || ((pool_e) == NULL || *(pool_e) == '\0' || inet_pton(AF_INET, (pool_e), &ina) != 1 ? -1 : ((pend) = ntohl(ina.s_addr), 0)) != 0 || pstart > pend )
            {
                vpn_warn( "address pool is invalid" );
                return -1;
            }
            if ( ( pstart & mask ) != ( network & mask ) || ( pend & mask ) != ( network & mask ) || pstart == server_ip || pend == server_ip )
            {
                vpn_warn( "address pool must stay inside the tunnel network and off the server address" );
                return -1;
            }
            have_pool = 1;
        }
    }
    else
    {
        talk_t st;
        const char *bridge;
        uint32_t bip;

        bridge = json_string( server, "bridge" );
        st = scalls( bridge, "status", NULL );
        if ( st == NULL
             || ((json_string( st, "ip" )) == NULL || *(json_string( st, "ip" )) == '\0' || inet_pton(AF_INET, (json_string( st, "ip" )), &ina) != 1 ? -1 : ((bip) = ntohl(ina.s_addr), 0)) != 0 || ((json_string( st, "mask" )) == NULL || *(json_string( st, "mask" )) == '\0' || inet_pton(AF_INET, (json_string( st, "mask" )), &ina) != 1 ? -1 : ((mask) = ntohl(ina.s_addr), 0)) != 0
             || ((json_string( server, "poolstart" )) == NULL || *(json_string( server, "poolstart" )) == '\0' || inet_pton(AF_INET, (json_string( server, "poolstart" )), &ina) != 1 ? -1 : ((pstart) = ntohl(ina.s_addr), 0)) != 0
             || ((json_string( server, "poolend" )) == NULL || *(json_string( server, "poolend" )) == '\0' || inet_pton(AF_INET, (json_string( server, "poolend" )), &ina) != 1 ? -1 : ((pend) = ntohl(ina.s_addr), 0)) != 0 )
        {
            if ( st != NULL )
            {
                talk_free( st );
            }
            vpn_warn( "tap pool is not ready" );
            return -1;
        }
        network = bip & mask;
        server_ip = bip;
        have_pool = 1;
        talk_free( st );
    }
    seen_cn = json_create( NULL );
    seen_ip = json_create( NULL );
    nets = json_create( NULL );
    list = NULL;
    axp = NULL;
    while ( NULL != ( cfg = each_client( &list, &axp, &name ) ) )
    {
        const char *tip;
        const char *tpeer;
        uint32_t cip;
        uint32_t cpeer;
        talk_t remote;
        talk_t raxp;

        client_cn( cfg, name, cn, (int)sizeof(cn) );
        if ( cn[0] == '\0' || strlen( cn ) > 200 || strchr( cn, '/' ) != NULL || strchr( cn, '\n' ) != NULL
             || strchr( cn, '\r' ) != NULL || (strcmp(cn, "DEFAULT") == 0) || (strcmp(cn, ".") == 0) || (strcmp(cn, "..") == 0) )
        {
            vpn_warn( "%s cn is invalid", name );
            goto fail;
        }
        if ( json_string( seen_cn, cn ) != NULL )
        {
            vpn_warn( "cn %s is used by two clients", cn );
            goto fail;
        }
        json_set_string( seen_cn, cn, name );
        tip = json_string( cfg, "tunnelip" );
        tpeer = json_string( cfg, "tunnelpeer" );
        if ( tun && tip != NULL && *tip != '\0' )
        {
            if ( !have_pool )
            {
                vpn_warn( "%s has a fixed address but the pool is the whole subnet", name );
                goto fail;
            }
            if ( ((tip) == NULL || *(tip) == '\0' || inet_pton(AF_INET, (tip), &ina) != 1 ? -1 : ((cip) = ntohl(ina.s_addr), 0)) != 0 || ( cip & mask ) != ( network & mask ) || cip == server_ip || cip == network )
            {
                vpn_warn( "%s tunnel address is outside the tunnel network", name );
                goto fail;
            }
            if ( cip >= pstart && cip <= pend )
            {
                vpn_warn( "%s tunnel address falls inside the pool", name );
                goto fail;
            }
            { ina.s_addr = htonl(cip); inet_ntop(AF_INET, &ina, ipbuf, (socklen_t)((int)sizeof(ipbuf))); };
            if ( json_string( seen_ip, ipbuf ) != NULL )
            {
                vpn_warn( "tunnel address %s is used twice", ipbuf );
                goto fail;
            }
            json_set_string( seen_ip, ipbuf, name );
            if ( net30 )
            {
                uint32_t base;

                if ( tpeer == NULL || *tpeer == '\0' )
                {
                    vpn_warn( "%s needs tunnelpeer in net30", name );
                    goto fail;
                }
                if ( ((tpeer) == NULL || *(tpeer) == '\0' || inet_pton(AF_INET, (tpeer), &ina) != 1 ? -1 : ((cpeer) = ntohl(ina.s_addr), 0)) != 0 )
                {
                    vpn_warn( "%s tunnelpeer is invalid", name );
                    goto fail;
                }
                base = cip & ~3u;
                if ( ( cip & 3u ) == 0 || ( cip & 3u ) == 3 || ( cpeer & ~3u ) != base )
                {
                    vpn_warn( "%s net30 pair is not the two hosts of one /30", name );
                    goto fail;
                }
                if ( ( ( cip == base + 1 ) && ( cpeer == base + 2 ) ) || ( ( cip == base + 2 ) && ( cpeer == base + 1 ) ) )
                {
                    /* pair matches */
                }
                else
                {
                    vpn_warn( "%s net30 pair is not the two hosts of one /30", name );
                    goto fail;
                }
                if ( ( base & mask ) != ( network & mask ) || ( ( base + 3 ) & mask ) != ( network & mask ) )
                {
                    vpn_warn( "%s /30 is outside the tunnel network", name );
                    goto fail;
                }
                if ( server_ip >= base && server_ip <= base + 3 )
                {
                    vpn_warn( "%s /30 contains the server address", name );
                    goto fail;
                }
                if ( !( base + 3 < pstart || base > pend ) )
                {
                    vpn_warn( "%s /30 overlaps the pool", name );
                    goto fail;
                }
                { ina.s_addr = htonl(cpeer); inet_ntop(AF_INET, &ina, ipbuf, (socklen_t)((int)sizeof(ipbuf))); };
                if ( json_string( seen_ip, ipbuf ) != NULL )
                {
                    vpn_warn( "tunnel address %s is used twice", ipbuf );
                    goto fail;
                }
                json_set_string( seen_ip, ipbuf, name );
            }
        }
        else if ( !tun && tip != NULL && *tip != '\0' )
        {
            if ( ((tip) == NULL || *(tip) == '\0' || inet_pton(AF_INET, (tip), &ina) != 1 ? -1 : ((cip) = ntohl(ina.s_addr), 0)) != 0 || ( cip & mask ) != ( network & mask ) || cip == server_ip )
            {
                vpn_warn( "%s address is outside the local network", name );
                goto fail;
            }
            if ( cip >= pstart && cip <= pend )
            {
                vpn_warn( "%s address falls inside the pool", name );
                goto fail;
            }
            { ina.s_addr = htonl(cip); inet_ntop(AF_INET, &ina, ipbuf, (socklen_t)((int)sizeof(ipbuf))); };
            if ( json_string( seen_ip, ipbuf ) != NULL )
            {
                vpn_warn( "address %s is used twice", ipbuf );
                goto fail;
            }
            json_set_string( seen_ip, ipbuf, name );
        }
        else if ( net30 && tpeer != NULL && *tpeer != '\0' )
        {
            vpn_warn( "%s has tunnelpeer without tunnelip", name );
            goto fail;
        }
        remote = json_json( cfg, "remote_net" );
        raxp = NULL;
        if ( remote != NULL && tun )
        {
            while ( NULL != ( raxp = json_next( remote, raxp ) ) )
            {
                talk_t item;
                talk_t naxp;
                uint32_t rip;
                uint32_t rmask;
                char key[80];

                item = axp_json( raxp );
                {
                    int nprefix;

                    if ( ((json_string( item, "target" )) == NULL || *(json_string( item, "target" )) == '\0' || inet_pton(AF_INET, (json_string( item, "target" )), &ina) != 1 ? -1 : ((rip) = ntohl(ina.s_addr), 0)) != 0
                         || ((json_string( item, "mask" )) == NULL || *(json_string( item, "mask" )) == '\0' || inet_pton(AF_INET, (json_string( item, "mask" )), &ina) != 1 ? -1 : ((rmask) = ntohl(ina.s_addr), 0)) != 0
                         || mask_prefix( rmask, &nprefix ) != 0 || nprefix == 0 )
                    {
                        vpn_warn( "%s remote_net is invalid", name );
                        goto fail;
                    }
                }
                naxp = NULL;
                while ( NULL != ( naxp = json_next( nets, naxp ) ) )
                {
                    uint32_t oip;
                    uint32_t omask;
                    const char *os;

                    os = json_string( axp_json( naxp ), "target" );
                    if ( ((os) == NULL || *(os) == '\0' || inet_pton(AF_INET, (os), &ina) != 1 ? -1 : ((oip) = ntohl(ina.s_addr), 0)) != 0 )
                    {
                        continue;
                    }
                    if ( ((json_string( axp_json( naxp ), "mask" )) == NULL || *(json_string( axp_json( naxp ), "mask" )) == '\0' || inet_pton(AF_INET, (json_string( axp_json( naxp ), "mask" )), &ina) != 1 ? -1 : ((omask) = ntohl(ina.s_addr), 0)) != 0 )
                    {
                        continue;
                    }
                    {
                        uint32_t a1;
                        uint32_t b1;

                        a1 = rip & rmask;
                        b1 = oip & omask;
                        if ( a1 <= ( b1 | ~omask ) && b1 <= ( a1 | ~rmask ) )
                        {
                            vpn_warn( "remote_net overlaps between clients" );
                            goto fail;
                        }
                    }
                }
                snprintf( key, sizeof(key), "%s-%s", name, axp_name( raxp ) ? axp_name( raxp ) : "net" );
                {
                    talk_t copy;

                    copy = json_create( NULL );
                    json_set_string( copy, "target", json_string( item, "target" ) );
                    json_set_string( copy, "mask", json_string( item, "mask" ) );
                    json_set_json( nets, key, copy );
                }
            }
        }
        talk_free( cfg );
    }
    if ( list != NULL )
    {
        talk_free( list );
    }
    talk_free( seen_cn );
    talk_free( seen_ip );
    talk_free( nets );
    (void)auth;
    return 0;
fail:
    talk_free( cfg );
    if ( list != NULL )
    {
        talk_free( list );
    }
    talk_free( seen_cn );
    talk_free( seen_ip );
    talk_free( nets );
    return -1;
}

/* Check JSON config before writing runtime files */
static int check_cfg( talk_t cfg )
{
    const char *auth;
    const char *dev;
    const char *hmac;
    const char *digest;
    char path[PATH_MAX];
    talk_t radius;
    talk_t pkcs;
    struct in_addr ina;

    auth = json_string( cfg, "auth" );
    if ( auth == NULL || *auth == '\0' )
    {
        auth = "certificate";
    }
    dev = json_string( cfg, "dev" );
    if ( dev == NULL || *dev == '\0' )
    {
        dev = "tun";
    }
    if ( strcmp( auth, "certificate" ) != 0 && strcmp( auth, "username" ) != 0
         && strcmp( auth, "certificate+username" ) != 0 && strcmp( auth, "static" ) != 0 )
    {
        vpn_warn( "unknown auth" );
        return -1;
    }
    if ( strcmp( dev, "tun" ) != 0 && strcmp( dev, "tap" ) != 0 )
    {
        vpn_warn( "dev must be tun or tap" );
        return -1;
    }

    {
        const char *clist;
        char buf[256];
        char *p;
        char *save;
        int n;

        clist = json_string( cfg, "cipher" );
        if ( clist == NULL || *clist == '\0' )
        {
            clist = "AES-256-GCM:AES-128-GCM";
        }
        strncpy( buf, clist, sizeof(buf) - 1 );
        buf[sizeof(buf) - 1] = '\0';
        n = 0;
        p = strtok_r( buf, ":", &save );
        while ( p != NULL )
        {
            if ( strcmp( p, "AES-256-GCM" ) != 0 && strcmp( p, "AES-128-GCM" ) != 0
                 && strcmp( p, "CHACHA20-POLY1305" ) != 0 && strcmp( p, "AES-256-CBC" ) != 0 && strcmp( p, "AES-128-CBC" ) != 0 )
            {
                vpn_warn( "cipher list is invalid" );
                return -1;
            }
            n++;
            p = strtok_r( NULL, ":", &save );
        }
        if ( n == 0 )
        {
            vpn_warn( "cipher list is invalid" );
            return -1;
        }
    }
    digest = json_string( cfg, "authdigest" );
    if ( digest == NULL || *digest == '\0' )
    {
        digest = "SHA256";
    }
    if ( strcmp( digest, "SHA256" ) != 0 && strcmp( digest, "SHA384" ) != 0 && strcmp( digest, "SHA512" ) != 0 )
    {
        vpn_warn( "auth digest is invalid" );
        return -1;
    }
    hmac = json_string( cfg, "hmac" );
    if ( hmac == NULL || *hmac == '\0' )
    {
        hmac = "disable";
    }
    if ( strcmp( auth, "static" ) != 0 && strcmp( hmac, "disable" ) != 0 )
    {
        if ( strcmp( hmac, "tls-auth" ) != 0 && strcmp( hmac, "tls-crypt" ) != 0 && strcmp( hmac, "tls-crypt-v2" ) != 0 )
        {
            vpn_warn( "hmac mode is invalid" );
            return -1;
        }
        config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
        if ( access( path, F_OK ) != 0 )
        {
            vpn_warn( "tls key file is missing" );
            return -1;
        }
    }
    if ( strcmp( auth, "static" ) == 0 )
    {
        config_path( path, sizeof(path), PROJECT_ID, "server.static" );
        if ( access( path, F_OK ) != 0 )
        {
            vpn_warn( "static key file is missing" );
            return -1;
        }
    }
    else
    {
        const char *pkcs_status;

        pkcs = json_json( cfg, "pkcs12" );
        pkcs_status = NULL;
        if ( pkcs != NULL )
        {
            pkcs_status = json_string( pkcs, "status" );
        }
        if ( pkcs_status != NULL && strcmp( pkcs_status, "enable" ) == 0 )
        {
            config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
            if ( access( path, F_OK ) != 0 )
            {
                vpn_warn( "pkcs12 file is missing" );
                return -1;
            }
        }
        else
        {
            config_path( path, sizeof(path), PROJECT_ID, "server.ca" );
            if ( access( path, F_OK ) != 0 )
            {
                vpn_warn( "CA file is missing" );
                return -1;
            }
            config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
            if ( access( path, F_OK ) != 0 )
            {
                vpn_warn( "server certificate is missing" );
                return -1;
            }
            config_path( path, sizeof(path), PROJECT_ID, "server.key" );
            if ( access( path, F_OK ) != 0 )
            {
                vpn_warn( "server key is missing" );
                return -1;
            }
        }
    }
    radius = json_json( cfg, "radius" );
    {
        const char *radius_status;

        radius_status = NULL;
        if ( radius != NULL )
        {
            radius_status = json_string( radius, "status" );
        }
        if ( radius_status != NULL && strcmp( radius_status, "enable" ) == 0
             && ( strcmp( auth, "username" ) == 0 || strcmp( auth, "certificate+username" ) == 0 ) )
        {
            if ( json_string( radius, "host" ) == NULL || json_string( radius, "host" )[0] == '\0'
                 || json_string( radius, "secret" ) == NULL || json_string( radius, "secret" )[0] == '\0' )
            {
                vpn_warn( "RADIUS host or secret is empty" );
                return -1;
            }
        }
    }
    if ( normalize_revoke( cfg ) != 0 )
    {
        return -1;
    }
    if ( strcmp( dev, "tap" ) == 0 )
    {
        const char *bridge;
        talk_t devtalk;
        talk_t st;
        const char *ifdev;
        const char *ip;
        const char *bmask;
        uint32_t bip;
        uint32_t bmsk;
        uint32_t ps;
        uint32_t pe;

        bridge = json_string( cfg, "bridge" );
        if ( bridge == NULL || *bridge == '\0' )
        {
            vpn_warn( "tap needs a local ifname" );
            return -1;
        }
        devtalk = scalls( bridge, "ifdev", NULL );
        ifdev = x2string( devtalk );
        if ( ifdev == NULL || strncmp( ifdev, "bridge@", 7 ) != 0 )
        {
            vpn_warn( "%s ifdev is not a bridge", bridge );
            if ( devtalk != NULL )
            {
                talk_free( devtalk );
            }
            return -1;
        }
        talk_free( devtalk );
        st = scalls( bridge, "status", NULL );
        ip = json_string( st, "ip" );
        bmask = json_string( st, "mask" );
        if ( ((ip) == NULL || *(ip) == '\0' || inet_pton(AF_INET, (ip), &ina) != 1 ? -1 : ((bip) = ntohl(ina.s_addr), 0)) != 0 || ((bmask) == NULL || *(bmask) == '\0' || inet_pton(AF_INET, (bmask), &ina) != 1 ? -1 : ((bmsk) = ntohl(ina.s_addr), 0)) != 0 )
        {
            vpn_warn( "%s has no address", bridge );
            if ( st != NULL )
            {
                talk_free( st );
            }
            return -1;
        }
        if ( json_string( cfg, "poolstart" ) == NULL || json_string( cfg, "poolstart" )[0] == '\0'
             || json_string( cfg, "poolend" ) == NULL || json_string( cfg, "poolend" )[0] == '\0'
             || ((json_string( cfg, "poolstart" )) == NULL || *(json_string( cfg, "poolstart" )) == '\0' || inet_pton(AF_INET, (json_string( cfg, "poolstart" )), &ina) != 1 ? -1 : ((ps) = ntohl(ina.s_addr), 0)) != 0
             || ((json_string( cfg, "poolend" )) == NULL || *(json_string( cfg, "poolend" )) == '\0' || inet_pton(AF_INET, (json_string( cfg, "poolend" )), &ina) != 1 ? -1 : ((pe) = ntohl(ina.s_addr), 0)) != 0
             || ps > pe || ( ps & bmsk ) != ( bip & bmsk ) || ( pe & bmsk ) != ( bip & bmsk ) || ps == bip || pe == bip )
        {
            vpn_warn( "tap pool must sit in the local network and avoid the gateway" );
            talk_free( st );
            return -1;
        }
        talk_free( st );
    }
    if ( strcmp( auth, "static" ) != 0 && check_clients( cfg ) != 0 )
    {
        return -1;
    }
    return 0;
}


/* Check config, then write /tmp/ovpns runtime files */
static int set_server_conf( talk_t cfg )
{
    FILE *fp;
    talk_t list;
    talk_t axp;
    talk_t client;
    talk_t pkcs;
    talk_t radius;
    const char *name;
    const char *auth;
    const char *dev;
    const char *hmac;
    const char *topology;
    const char *extern_name;
    const char *opt;
    char path[PATH_MAX];
    char first[64];
    char cn[256];
    char script[PATH_MAX];
    int tun;
    int net30;
    int username;
    int cbc;
    uint32_t network;
    uint32_t mask;
    uint32_t server_ip;
    char server_s[64];
    char peer_s[64];
    struct in_addr ina;

    if ( check_cfg( cfg ) != 0 )
    {
        return -1;
    }
    auth = json_string( cfg, "auth" );
    if ( auth == NULL || *auth == '\0' )
    {
        auth = "certificate";
    }
    dev = json_string( cfg, "dev" );
    if ( dev == NULL || *dev == '\0' )
    {
        dev = "tun";
    }
    tun = strcmp( dev, "tun" ) == 0;
    topology = json_string( cfg, "topology" );
    if ( topology == NULL || *topology == '\0' )
    {
        topology = "subnet";
    }
    net30 = tun && strcmp( topology, "net30" ) == 0;
    username = strcmp( auth, "username" ) == 0 || strcmp( auth, "certificate+username" ) == 0;
    if ( mkdir( RUNDIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0 && errno != EEXIST )
    {
        vpn_warn( "cannot create %s", RUNDIR );
        return -1;
    }
    shell( "rm -rf %s %s", CCD_DIR, CRL_DIR );
    mkdir( CCD_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    mkdir( CRL_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    fp = fopen( CONF_FILE, "w" );
    if ( fp == NULL )
    {
        vpn_warn( "cannot write server.conf" );
        return -1;
    }
    fprintf( fp, "proto %s\n", ((json_string( cfg, "proto" )) != NULL && *(json_string( cfg, "proto" )) != '\0' ? (json_string( cfg, "proto" )) : ("udp")) );
    fprintf( fp, "port %s\n", ((json_string( cfg, "port" )) != NULL && *(json_string( cfg, "port" )) != '\0' ? (json_string( cfg, "port" )) : ("1194")) );
    fprintf( fp, "dev-type %s\n", dev );
    fprintf( fp, "dev %s\n", DEVNAME );
    extern_name = json_string( cfg, "extern" );
    if ( extern_name != NULL && *extern_name != '\0' && strcmp( extern_name, "disable" ) != 0 )
    {
        talk_t st;
        const char *ip;

        st = scalls( extern_name, "status", NULL );
        ip = json_string( st, "ip" );
        if ( ip == NULL || *ip == '\0' )
        {
            vpn_warn( "extern %s has no address", extern_name );
            if ( st != NULL )
            {
                talk_free( st );
            }
            fclose( fp );
            return -1;
        }
        fprintf( fp, "local %s\n", ip );
        talk_free( st );
    }
    if ( strcmp( auth, "static" ) == 0 )
    {
        config_path( path, sizeof(path), PROJECT_ID, "server.static" );
        fprintf( fp, "secret %s\n", path );
        if ( ((json_string( cfg, "tunnelip" )) == NULL || *(json_string( cfg, "tunnelip" )) == '\0' || inet_pton(AF_INET, (json_string( cfg, "tunnelip" )), &ina) != 1 ? -1 : ((network) = ntohl(ina.s_addr), 0)) == 0
             && ((json_string( cfg, "tunnelmask" )) == NULL || *(json_string( cfg, "tunnelmask" )) == '\0' || inet_pton(AF_INET, (json_string( cfg, "tunnelmask" )), &ina) != 1 ? -1 : ((mask) = ntohl(ina.s_addr), 0)) == 0 )
        {
            server_ip = ( network & mask ) + 1;
            { ina.s_addr = htonl(server_ip); inet_ntop(AF_INET, &ina, server_s, (socklen_t)((int)sizeof(server_s))); };
            { ina.s_addr = htonl(server_ip + 1); inet_ntop(AF_INET, &ina, peer_s, (socklen_t)((int)sizeof(peer_s))); };
            if ( tun )
            {
                fprintf( fp, "ifconfig %s %s\n", server_s, peer_s );
            }
        }
    }
    else
    {
        pkcs = json_json( cfg, "pkcs12" );
        opt = NULL;
        if ( pkcs != NULL )
        {
            opt = json_string( pkcs, "status" );
        }
        if ( opt != NULL && strcmp( opt, "enable" ) == 0 )
        {
            const char *pw;

            config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
            fprintf( fp, "pkcs12 %s\n", path );
            pw = json_string( pkcs, "password" );
            if ( pw != NULL && *pw != '\0' )
            {
                FILE *ap;

                ap = fopen( ASKPASS_FILE, "w" );
                if ( ap != NULL )
                {
                    fprintf( ap, "%s\n", pw );
                    fclose( ap );
                    chown( ASKPASS_FILE, getuid(), getgid() );
                    chmod( ASKPASS_FILE, S_IRUSR | S_IWUSR );
                    fprintf( fp, "askpass %s\n", ASKPASS_FILE );
                }
            }
        }
        else
        {
            config_path( path, sizeof(path), PROJECT_ID, "server.ca" );
            fprintf( fp, "ca %s\n", path );
            config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
            fprintf( fp, "cert %s\n", path );
            config_path( path, sizeof(path), PROJECT_ID, "server.key" );
            fprintf( fp, "key %s\n", path );
        }
        config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
        if ( access( path, F_OK ) == 0 )
        {
            fprintf( fp, "dh %s\n", path );
        }
        else
        {
            fprintf( fp, "dh none\n" );
        }
        hmac = json_string( cfg, "hmac" );
        if ( hmac == NULL || *hmac == '\0' )
        {
            hmac = "disable";
        }
        if ( strcmp( hmac, "disable" ) != 0 )
        {
            config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
            /* tls-auth clients use key-direction 1; server must be 0. tls-crypt has no direction. */
            if ( strcmp( hmac, "tls-auth" ) == 0 )
            {
                fprintf( fp, "tls-auth %s 0\n", path );
            }
            else
            {
                fprintf( fp, "%s %s\n", hmac, path );
            }
        }
        if ( tun )
        {
            fprintf( fp, "topology %s\n", net30 ? "net30" : "subnet" );
            /* OpenVPN 2.6: --server already defines a pool; use nopool when a custom pool is set */
            if ( json_string( cfg, "poolstart" ) != NULL && json_string( cfg, "poolstart" )[0] != '\0'
                 && json_string( cfg, "poolend" ) != NULL && json_string( cfg, "poolend" )[0] != '\0' )
            {
                fprintf( fp, "server %s %s nopool\n", json_string( cfg, "tunnelip" ), json_string( cfg, "tunnelmask" ) );
                fprintf( fp, "ifconfig-pool %s %s\n", json_string( cfg, "poolstart" ), json_string( cfg, "poolend" ) );
            }
            else
            {
                fprintf( fp, "server %s %s\n", json_string( cfg, "tunnelip" ), json_string( cfg, "tunnelmask" ) );
            }
        }
        else
        {
            talk_t st;
            const char *bridge;

            bridge = json_string( cfg, "bridge" );
            st = scalls( bridge, "status", NULL );
            fprintf( fp, "server-bridge %s %s %s %s\n",
                     json_string( st, "ip" ), json_string( st, "mask" ),
                     json_string( cfg, "poolstart" ), json_string( cfg, "poolend" ) );
            talk_free( st );
        }
        fprintf( fp, "client-config-dir %s\n", CCD_DIR );
        if ( strcmp( auth, "username" ) != 0 )
        {
            talk_t rev;
            talk_t raxp;

            fprintf( fp, "crl-verify %s dir\n", CRL_DIR );
            rev = json_json( cfg, "revoke" );
            raxp = NULL;
            if ( rev != NULL )
            {
                while ( NULL != ( raxp = json_next( rev, raxp ) ) )
                {
                    const char *serial;
                    FILE *empty;
                    char sp[PATH_MAX];

                    serial = json_string( axp_json( raxp ), "serial" );
                    if ( serial == NULL || *serial == '\0' )
                    {
                        continue;
                    }
                    snprintf( sp, sizeof(sp), CRL_DIR"/%s", serial );
                    empty = fopen( sp, "w" );
                    if ( empty != NULL )
                    {
                        fclose( empty );
                    }
                }
            }
        }
        if ( strcmp( auth, "username" ) == 0 )
        {
            fprintf( fp, "verify-client-cert none\n" );
        }
        if ( username )
        {
            fprintf( fp, "username-as-common-name\n" );
            project_path( script, sizeof(script), PROJECT_ID );
            fprintf( fp, "auth-user-pass-verify %s/auth.sh via-file\n", script );
            fprintf( fp, "script-security 2\n" );
        }
        opt = json_string( cfg, "client2client" );
        if ( opt != NULL && strcmp( opt, "enable" ) == 0 )
        {
            fprintf( fp, "client-to-client\n" );
        }
        opt = json_string( cfg, "clientdup" );
        if ( opt != NULL && strcmp( opt, "enable" ) == 0 )
        {
            fprintf( fp, "duplicate-cn\n" );
        }
        fprintf( fp, "max-clients %s\n", ((json_string( cfg, "maxclient" )) != NULL && *(json_string( cfg, "maxclient" )) != '\0' ? (json_string( cfg, "maxclient" )) : ("50")) );
        opt = json_string( cfg, "redirect" );
        if ( tun && opt != NULL && strcmp( opt, "enable" ) == 0 )
        {
            fprintf( fp, "push \"redirect-gateway def1\"\n" );
        }
        if ( tun && json_string( cfg, "dns" ) != NULL && json_string( cfg, "dns" )[0] != '\0' )
        {
            fprintf( fp, "push \"dhcp-option DNS %s\"\n", json_string( cfg, "dns" ) );
        }
        if ( tun && json_string( cfg, "dns2" ) != NULL && json_string( cfg, "dns2" )[0] != '\0' )
        {
            fprintf( fp, "push \"dhcp-option DNS %s\"\n", json_string( cfg, "dns2" ) );
        }
        if ( tun )
        {

        {
            talk_t _nets;
            talk_t _naxp;

            _nets = json_json( cfg, "local_net" );
            _naxp = NULL;
            while ( _nets != NULL && NULL != ( _naxp = json_next( _nets, _naxp ) ) )
            {
                talk_t item;
                const char *target;
                const char *mask;

                item = axp_json( _naxp );
                target = json_string( item, "target" );
                mask = json_string( item, "mask" );
                if ( target != NULL && mask != NULL && *target != '\0' && *mask != '\0' )
                {
                    fprintf( fp, "push \"route %s %s\"\n", target, mask );
                }
            }
        }

        }
    }
    if ( !username )
    {
        fprintf( fp, "script-security 2\n" );
    }

    {
        const char *clist;
        char buf[256];
        char *p;
        char *save;
        int n;

        clist = json_string( cfg, "cipher" );
        if ( clist == NULL || *clist == '\0' )
        {
            clist = "AES-256-GCM:AES-128-GCM";
        }
        strncpy( buf, clist, sizeof(buf) - 1 );
        buf[sizeof(buf) - 1] = '\0';
        n = 0;
        cbc = 0;
        first[0] = '\0';
        p = strtok_r( buf, ":", &save );
        while ( p != NULL )
        {
            if ( n == 0 )
            {
                strncpy( first, p, sizeof(first) - 1 );
                first[sizeof(first) - 1] = '\0';
            }
            if ( strstr( p, "CBC" ) != NULL )
            {
                cbc = 1;
            }
            n++;
            p = strtok_r( NULL, ":", &save );
        }
        /* --secret cannot use AEAD ciphers or --data-ciphers */
        if ( strcmp( auth, "static" ) == 0 )
        {
            if ( strstr( first, "CBC" ) == NULL )
            {
                strncpy( first, "AES-256-CBC", sizeof(first) - 1 );
                first[sizeof(first) - 1] = '\0';
            }
            fprintf( fp, "cipher %s\n", first );
            cbc = 0;
        }
        else
        {
            fprintf( fp, "data-ciphers %s\n", clist );
            fprintf( fp, "cipher %s\n", first );
        }
    }

    if ( cbc )
    {
        fprintf( fp, "auth %s\n", ((json_string( cfg, "authdigest" )) != NULL && *(json_string( cfg, "authdigest" )) != '\0' ? (json_string( cfg, "authdigest" )) : ("SHA256")) );
    }
    opt = json_string( cfg, "float" );
    if ( opt == NULL || strcmp( opt, "disable" ) != 0 )
    {
        fprintf( fp, "float\n" );
    }
    opt = json_string( cfg, "lzo" );
    if ( opt != NULL && strcmp( opt, "disable" ) != 0 )
    {
        fprintf( fp, "comp-lzo %s\n", opt );
    }
    fprintf( fp, "keepalive %s %s\n", ((json_string( cfg, "interval" )) != NULL && *(json_string( cfg, "interval" )) != '\0' ? (json_string( cfg, "interval" )) : ("10")), ((json_string( cfg, "timeout" )) != NULL && *(json_string( cfg, "timeout" )) != '\0' ? (json_string( cfg, "timeout" )) : ("120")) );
    fprintf( fp, "persist-key\n" );
    fprintf( fp, "persist-tun\n" );
    fprintf( fp, "management %s unix\n", MGMT_SOCK );
    fprintf( fp, "status %s 1\n", STATUS_FILE );
    /* client-connect is only valid in --mode server; static key is point to point */
    if ( strcmp( auth, "static" ) != 0 )
    {
        project_path( script, sizeof(script), PROJECT_ID );
        fprintf( fp, "client-connect %s/connect.sh\n", script );
        fprintf( fp, "client-disconnect %s/disconnect.sh\n", script );
        fprintf( fp, "setenv object %s\n", COM_IDPATH );
    }
    fprintf( fp, "verb 3\n" );

    /* write options lines */
    {
        const char *s;
        const char *e;
        int n;
        const char *optlines = json_string( cfg, "options" );

        s = optlines;
        while ( s != NULL && *s != '\0' )
        {
            while ( *s == ';' || *s == '\n' || *s == '\r' || *s == ' ' || *s == '\t' )
            {
                s++;
            }
            if ( *s == '\0' )
            {
                break;
            }
            e = s;
            while ( *e != '\0' && *e != ';' && *e != '\n' && *e != '\r' )
            {
                e++;
            }
            n = (int)( e - s );
            while ( n > 0 && ( s[n - 1] == ' ' || s[n - 1] == '\t' ) )
            {
                n--;
            }
            if ( n > 0 )
            {
                fprintf( fp, "%.*s\n", n, s );
            }
            s = e;
        }
    }

    if ( strcmp( auth, "static" ) != 0 && tun )
    {
        list = NULL;
        axp = NULL;
        while ( NULL != ( client = each_client( &list, &axp, &name ) ) )
        {

        {
            talk_t _nets;
            talk_t _naxp;

            _nets = json_json( client, "remote_net" );
            _naxp = NULL;
            while ( _nets != NULL && NULL != ( _naxp = json_next( _nets, _naxp ) ) )
            {
                talk_t item;
                const char *target;
                const char *mask;

                item = axp_json( _naxp );
                target = json_string( item, "target" );
                mask = json_string( item, "mask" );
                if ( target != NULL && mask != NULL && *target != '\0' && *mask != '\0' )
                {
                    fprintf( fp, "route %s %s\n", target, mask );
                }
            }
        }

            talk_free( client );
        }
        if ( list != NULL )
        {
            talk_free( list );
        }
    }
    fclose( fp );

    if ( strcmp( auth, "static" ) != 0 )
    {
        const char *lanmask;

        lanmask = json_string( cfg, "tunnelmask" );
        if ( !tun )
        {
            talk_t st;

            st = scalls( json_string( cfg, "bridge" ), "status", NULL );
            lanmask = json_string( st, "mask" );
            if ( lanmask != NULL )
            {
                strncpy( first, lanmask, sizeof(first) - 1 );
                first[sizeof(first) - 1] = '\0';
                lanmask = first;
            }
            if ( st != NULL )
            {
                talk_free( st );
            }
        }
        list = NULL;
        axp = NULL;
        while ( NULL != ( client = each_client( &list, &axp, &name ) ) )
        {
            client_cn( client, name, cn, (int)sizeof(cn) );

            /* one ccd file */
            {
                FILE *cfp;
                char cpath[PATH_MAX];
                const char *tip;
                const char *tpeer;
                const char *redir;
                const char *dns;
                const char *dns2;
                const char *cst;
                int reset;
                int server_redir;

                snprintf( cpath, sizeof(cpath), CCD_DIR"/%s", cn );
                cfp = fopen( cpath, "w" );
                if ( cfp == NULL )
                {
                    vpn_warn( "cannot write %s", cpath );
                }
                else
                {
                    cst = json_string( client, "status" );
                    if ( cst != NULL && strcmp( cst, "disable" ) == 0 )
                    {
                        fprintf( cfp, "disable\n" );
                    }
                    else
                    {
                        cst = json_string( client, "push_reset" );
                        reset = cst != NULL && strcmp( cst, "enable" ) == 0;
                        if ( reset )
                        {
                            fprintf( cfp, "push-reset\n" );
                        }
                        tip = json_string( client, "tunnelip" );
                        tpeer = json_string( client, "tunnelpeer" );
                        if ( tun && tip != NULL && *tip != '\0' )
                        {
                            if ( net30 && tpeer != NULL && *tpeer != '\0' )
                            {
                                fprintf( cfp, "ifconfig-push %s %s\n", tip, tpeer );
                            }
                            else if ( !net30 )
                            {
                                fprintf( cfp, "ifconfig-push %s %s\n", tip, lanmask );
                            }
                        }
                        else if ( !tun && tip != NULL && *tip != '\0' && lanmask != NULL && *lanmask != '\0' )
                        {
                            fprintf( cfp, "ifconfig-push %s %s\n", tip, lanmask );
                        }
                        if ( tun )
                        {
                            {
                                talk_t _nets;
                                talk_t _naxp;

                                _nets = json_json( client, "local_net" );
                                _naxp = NULL;
                                while ( _nets != NULL && NULL != ( _naxp = json_next( _nets, _naxp ) ) )
                                {
                                    talk_t item;
                                    const char *target;
                                    const char *mask;

                                    item = axp_json( _naxp );
                                    target = json_string( item, "target" );
                                    mask = json_string( item, "mask" );
                                    if ( target != NULL && mask != NULL && *target != '\0' && *mask != '\0' )
                                    {
                                        fprintf( cfp, "push \"route %s %s\"\n", target, mask );
                                    }
                                }
                            }
                            {
                                talk_t _nets;
                                talk_t _naxp;

                                _nets = json_json( client, "remote_net" );
                                _naxp = NULL;
                                while ( _nets != NULL && NULL != ( _naxp = json_next( _nets, _naxp ) ) )
                                {
                                    talk_t item;
                                    const char *target;
                                    const char *mask;

                                    item = axp_json( _naxp );
                                    target = json_string( item, "target" );
                                    mask = json_string( item, "mask" );
                                    if ( target != NULL && mask != NULL && *target != '\0' && *mask != '\0' )
                                    {
                                        fprintf( cfp, "iroute %s %s\n", target, mask );
                                    }
                                }
                            }
                            dns = json_string( client, "dns" );
                            dns2 = json_string( client, "dns2" );
                            if ( ( dns != NULL && *dns != '\0' ) || ( dns2 != NULL && *dns2 != '\0' ) )
                            {
                                if ( !reset )
                                {
                                    fprintf( cfp, "push-remove \"dhcp-option DNS\"\n" );
                                }
                                if ( dns != NULL && *dns != '\0' )
                                {
                                    fprintf( cfp, "push \"dhcp-option DNS %s\"\n", dns );
                                }
                                if ( dns2 != NULL && *dns2 != '\0' )
                                {
                                    fprintf( cfp, "push \"dhcp-option DNS %s\"\n", dns2 );
                                }
                            }
                            redir = json_string( client, "redirect" );
                            cst = json_string( cfg, "redirect" );
                            server_redir = cst != NULL && strcmp( cst, "enable" ) == 0;
                            if ( reset )
                            {
                                if ( redir != NULL && strcmp( redir, "enable" ) == 0 )
                                {
                                    fprintf( cfp, "push \"redirect-gateway def1\"\n" );
                                }
                            }
                            else if ( redir != NULL && strcmp( redir, "enable" ) == 0 && !server_redir )
                            {
                                fprintf( cfp, "push \"redirect-gateway def1\"\n" );
                            }
                            else if ( redir != NULL && strcmp( redir, "disable" ) == 0 && server_redir )
                            {
                                fprintf( cfp, "push-remove redirect-gateway\n" );
                            }
                        }
                        {
                            const char *s;
                            const char *e;
                            int n;
                            const char *optlines = json_string( client, "options" );

                            s = optlines;
                            while ( s != NULL && *s != '\0' )
                            {
                                while ( *s == ';' || *s == '\n' || *s == '\r' || *s == ' ' || *s == '\t' )
                                {
                                    s++;
                                }
                                if ( *s == '\0' )
                                {
                                    break;
                                }
                                e = s;
                                while ( *e != '\0' && *e != ';' && *e != '\n' && *e != '\r' )
                                {
                                    e++;
                                }
                                n = (int)( e - s );
                                while ( n > 0 && ( s[n - 1] == ' ' || s[n - 1] == '\t' ) )
                                {
                                    n--;
                                }
                                if ( n > 0 )
                                {
                                    fprintf( cfp, "%.*s\n", n, s );
                                }
                                s = e;
                            }
                        }
                    }
                    fclose( cfp );
                }
            }

            talk_free( client );
        }
        if ( list != NULL )
        {
            talk_free( list );
        }
        opt = json_string( cfg, "listonly" );
        if ( opt != NULL && strcmp( opt, "enable" ) == 0 )
        {
            FILE *df;

            df = fopen( CCD_DIR"/DEFAULT", "w" );
            if ( df != NULL )
            {
                fprintf( df, "disable\n" );
                fclose( df );
            }
        }
    }
    radius = json_json( cfg, "radius" );
    fp = fopen( PASSWD_FILE, "w" );
    if ( fp != NULL )
    {
        opt = NULL;
        if ( radius != NULL )
        {
            opt = json_string( radius, "status" );
        }
        if ( username && ( opt == NULL || strcmp( opt, "enable" ) != 0 ) )
        {
            list = NULL;
            axp = NULL;
            while ( NULL != ( client = each_client( &list, &axp, &name ) ) )
            {
                const char *pw;

                client_cn( client, name, cn, (int)sizeof(cn) );
                pw = json_string( client, "password" );
                fprintf( fp, "%s %s\n", cn, pw ? pw : "" );
                talk_free( client );
            }
            if ( list != NULL )
            {
                talk_free( list );
            }
        }
        fclose( fp );
        chmod( PASSWD_FILE, S_IRUSR | S_IWUSR );
    }
    unlink( SERIAL_FILE );
    return 0;
}

static int radius_send( const char *host, int port, int wait_sec, unsigned char *pkt, int pktlen )
{
    int fd;
    int n;
    struct sockaddr_in sa;
    struct timeval tv;
    char ip[64];
    unsigned char resp[64];

    if ( inet_pton( AF_INET, host, &sa.sin_addr ) != 1 )
    {
        if ( domain2ip( host, ip, (int)sizeof(ip), wait_sec > 0 ? wait_sec : 5 ) == NULL )
        {
            return -1;
        }
        if ( inet_pton( AF_INET, ip, &sa.sin_addr ) != 1 )
        {
            return -1;
        }
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons( (uint16_t)port );
    fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 )
    {
        return -1;
    }
    tv.tv_sec = wait_sec > 0 ? wait_sec : 5;
    tv.tv_usec = 0;
    setsockopt( fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) );
    if ( sendto( fd, pkt, (size_t)pktlen, 0, (struct sockaddr *)&sa, sizeof(sa) ) != pktlen )
    {
        close( fd );
        return -1;
    }
    n = (int)recv( fd, resp, sizeof(resp), 0 );
    close( fd );
    if ( n < 1 )
    {
        return -1;
    }
    return resp[0];
}

static int radius_attr( unsigned char *pkt, int off, int type, const unsigned char *val, int vlen )
{
    if ( off + 2 + vlen > 4096 )
    {
        return -1;
    }
    pkt[off] = (unsigned char)type;
    pkt[off + 1] = (unsigned char)( 2 + vlen );
    memcpy( pkt + off + 2, val, (size_t)vlen );
    return off + 2 + vlen;
}

static int radius_access( const char *host, const char *secret, int port, int wait_sec, const char *user, const char *pass )
{
    unsigned char pkt[512];
    unsigned char auth[16];
    unsigned char epwd[128];
    int epwdlen;
    int off;
    int i;
    int fd;
    int slen;
    unsigned char md[16];
    unsigned char tmp[256];

    fd = open( "/dev/urandom", O_RDONLY );
    if ( fd < 0 || read( fd, auth, 16 ) != 16 )
    {
        if ( fd >= 0 )
        {
            close( fd );
        }
        return -1;
    }
    close( fd );
    slen = (int)strlen( secret );
    memset( epwd, 0, sizeof(epwd) );
    {
        unsigned char plain[128];
        unsigned char chain[16];
        int n;
        int pos;

        memset( plain, 0, sizeof(plain) );
        n = (int)strlen( pass );
        if ( n > 128 )
        {
            n = 128;
        }
        memcpy( plain, pass, (size_t)n );
        if ( n == 0 )
        {
            n = 16;
        }
        n = ( n + 15 ) & ~15;
        memcpy( chain, auth, 16 );
        for ( pos = 0; pos < n; pos += 16 )
        {
            memcpy( tmp, secret, (size_t)slen );
            memcpy( tmp + slen, chain, 16 );
            MD5( tmp, (size_t)slen + 16, md );
            for ( i = 0; i < 16; i++ )
            {
                epwd[pos + i] = plain[pos + i] ^ md[i];
            }
            memcpy( chain, epwd + pos, 16 );
        }
        epwdlen = n;
    }
    pkt[0] = 1;
    pkt[1] = 1;
    memcpy( pkt + 4, auth, 16 );
    off = 20;
    off = radius_attr( pkt, off, 1, (const unsigned char *)user, (int)strlen( user ) );
    off = radius_attr( pkt, off, 2, epwd, epwdlen );
    off = radius_attr( pkt, off, 32, (const unsigned char *)"ovpns", 5 );
    if ( off < 0 )
    {
        return -1;
    }
    pkt[2] = (unsigned char)( ( off >> 8 ) & 0xff );
    pkt[3] = (unsigned char)( off & 0xff );
    return radius_send( host, port, wait_sec, pkt, off ) == 2;
}

static void radius_acct( talk_t cfg, const char *status, const char *user, const char *session, const char *rx, const char *tx )
{
    talk_t radius;
    unsigned char pkt[512];
    unsigned char md[16];
    unsigned char tmp[600];
    const char *host;
    const char *secret;
    const char *st;
    int port;
    int wait_sec;
    int off;
    int slen;
    int code;

    radius = json_json( cfg, "radius" );
    st = NULL;
    if ( radius != NULL )
    {
        st = json_string( radius, "status" );
    }
    if ( st == NULL || strcmp( st, "enable" ) != 0 )
    {
        return;
    }
    st = json_string( radius, "accounting" );
    if ( st == NULL || strcmp( st, "enable" ) != 0 )
    {
        return;
    }
    host = json_string( radius, "host" );
    secret = json_string( radius, "secret" );
    if ( host == NULL || secret == NULL )
    {
        return;
    }
    port = atoi( ((json_string( radius, "acctport" )) != NULL && *(json_string( radius, "acctport" )) != '\0' ? (json_string( radius, "acctport" )) : ("1813")) );
    wait_sec = atoi( ((json_string( radius, "wait" )) != NULL && *(json_string( radius, "wait" )) != '\0' ? (json_string( radius, "wait" )) : ("5")) );
    code = (status != NULL && strcmp(status, "start") == 0) ? 1 : 2;
    memset( pkt, 0, sizeof(pkt) );
    pkt[0] = 4;
    pkt[1] = 7;
    off = 20;
    off = radius_attr( pkt, off, 1, (const unsigned char *)user, (int)strlen( user ) );
    {
        unsigned char stv[4];

        memset( stv, 0, sizeof(stv) );
        stv[3] = (unsigned char)code;
        off = radius_attr( pkt, off, 40, stv, 4 );
    }
    if ( session != NULL )
    {
        off = radius_attr( pkt, off, 44, (const unsigned char *)session, (int)strlen( session ) );
    }
    if ( rx != NULL && *rx != '\0' )
    {
        unsigned char octets[4];
        unsigned long v;

        v = strtoul( rx, NULL, 10 );
        octets[0] = (unsigned char)( ( v >> 24 ) & 0xff );
        octets[1] = (unsigned char)( ( v >> 16 ) & 0xff );
        octets[2] = (unsigned char)( ( v >> 8 ) & 0xff );
        octets[3] = (unsigned char)( v & 0xff );
        off = radius_attr( pkt, off, 42, octets, 4 );
    }
    if ( tx != NULL && *tx != '\0' )
    {
        unsigned char octets[4];
        unsigned long v;

        v = strtoul( tx, NULL, 10 );
        octets[0] = (unsigned char)( ( v >> 24 ) & 0xff );
        octets[1] = (unsigned char)( ( v >> 16 ) & 0xff );
        octets[2] = (unsigned char)( ( v >> 8 ) & 0xff );
        octets[3] = (unsigned char)( v & 0xff );
        off = radius_attr( pkt, off, 43, octets, 4 );
    }
    if ( off < 0 )
    {
        return;
    }
    pkt[2] = (unsigned char)( ( off >> 8 ) & 0xff );
    pkt[3] = (unsigned char)( off & 0xff );
    slen = (int)strlen( secret );
    memcpy( tmp, pkt, (size_t)off );
    memcpy( tmp + off, secret, (size_t)slen );
    MD5( tmp, (size_t)off + (size_t)slen, md );
    memcpy( pkt + 4, md, 16 );
    radius_send( host, port, wait_sec, pkt, off );
}


boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    talk_t list;
    talk_t axp;
    const char *obj;
    const char *ptr;
    const char *name;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return ttrue;
    }
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return ttrue;
    }
    list = config_list( PROJECT_ID );
    axp = NULL;
    while ( list != NULL && NULL != ( axp = json_next( list, axp ) ) )
    {
        name = axp_name( axp );
        if ( name != NULL && strcmp( name, COM_IDPATH ) != 0 )
        {
            com_register( name, COM_IDPATH, 0 );
        }
    }
    if ( list != NULL )
    {
        talk_free( list );
    }

    ptr = json_string( cfg, "status" );
    if ( ptr == NULL || strcmp( ptr, "enable" ) != 0 )
    {
        talk_free( cfg );
        return ttrue;
    }
    scalls( NETWORK_COM, "register", "%s,%s", COM_IDPATH, COM_IDPATH );    
    sstart( COM_IDPATH, "service", NULL, COM_IDPATH );

    talk_free( cfg );
    return ttrue;
}

boole_t _shut( obj_t this, param_t param )
{
    const char *obj;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return ttrue;
    }
    sstop( COM_IDPATH );
    return ttrue;
}

talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}

boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

    ret = config_set( this, v, path );
    if ( ret == true )
    {
        scall( COM_IDPATH, "shut", NULL );
        scall( COM_IDPATH, "setup", NULL );
    }
    return ret;
}

/* Main line: load -> check -> write conf/ccd -> exec openvpn */
talk_t _service( obj_t this, param_t param )
{
    talk_t cfg;
    char root[PATH_MAX];
    char upsh[PATH_MAX];
    char downsh[PATH_MAX];
    char spath[PATH_MAX];
    const char *scripts[] = { "server.up.sh", "server.down.sh", "auth.sh", "connect.sh", "disconnect.sh" };
    const char *obj;
    int i;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }

    /* 1. load config */
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return tfalse;
    }

    /* 2. check + write OpenVPN files under /tmp/ovpns */
    if ( set_server_conf( cfg ) != 0 )
    {
        talk_free( cfg );
        return tfalse;
    }
    talk_free( cfg );

    /* 3. scripts must be executable */
    for ( i = 0; i < 5; i++ )
    {
        project_path( spath, sizeof(spath), PROJECT_ID );
        strcat( spath, "/" );
        strcat( spath, scripts[i] );
        chmod( spath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
    }

    /* 4. become openvpn — prefer /usr/sbin so a shell wrapper on PATH cannot drop setuid */
    project_path( root, sizeof(root), PROJECT_ID );
    snprintf( upsh, sizeof(upsh), "%s/server.up.sh", root );
    snprintf( downsh, sizeof(downsh), "%s/server.down.sh", root );
    if ( access( "/usr/sbin/openvpn", X_OK ) == 0 )
    {
        execl( "/usr/sbin/openvpn", "openvpn",
                "--config", CONF_FILE,
                "--up", upsh,
                "--down", downsh,
                (char *)0 );
    }
    else
    {
        execlp( "openvpn", "openvpn",
                "--config", CONF_FILE,
                "--up", upsh,
                "--down", downsh,
                (char *)0 );
    }
    vpn_faulting( "execl openvpn error" );
    return tfalse;
}
talk_t _online( obj_t this, param_t param )
{
    talk_t v;
    talk_t cfg;
    const char *netdev;
    const char *dev;
    const char *bridge;
    talk_t devtalk;
    const char *ifdev;
    char bridgename[NAME_MAX];
    const char *obj;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    v = param_talk( param, 1 );
    if ( v == NULL )
    {
        return tfalse;
    }
    netdev = json_string( v, "netdev" );
    if ( netdev != NULL && *netdev != '\0' )
    {
        reg_set_string( this, "netdev", netdev );
    }
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return ttrue;
    }
    dev = json_string( cfg, "dev" );
    bridge = json_string( cfg, "bridge" );
    if ( dev != NULL && strcmp( dev, "tap" ) == 0 && bridge != NULL && *bridge != '\0' )
    {
        bridgename[0] = '\0';
        devtalk = scalls( bridge, "ifdev", NULL );
        ifdev = x2string( devtalk );
        if ( ifdev != NULL && *ifdev != '\0' )
        {
            strncpy( bridgename, ifdev, sizeof(bridgename) - 1 );
            bridgename[sizeof(bridgename) - 1] = '\0';
        }
        if ( devtalk != NULL )
        {
            talk_free( devtalk );
        }
        if ( bridgename[0] != '\0' )
        {
            scalls( bridgename, "addif", COM_IDPATH );
        }
    }
    talk_free( cfg );
    return ttrue;
}
boole_t _offline( obj_t this, param_t param )
{
    talk_t cfg;
    const char *dev;
    const char *bridge;
    talk_t devtalk;
    const char *ifdev;
    char bridgename[NAME_MAX];
    const char *obj;

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return ttrue;
    }
    dev = json_string( cfg, "dev" );
    bridge = json_string( cfg, "bridge" );
    if ( dev != NULL && strcmp( dev, "tap" ) == 0 && bridge != NULL && *bridge != '\0' )
    {
        bridgename[0] = '\0';
        devtalk = scalls( bridge, "ifdev", NULL );
        ifdev = x2string( devtalk );
        if ( ifdev != NULL )
        {
            strncpy( bridgename, ifdev, sizeof(bridgename) - 1 );
            bridgename[sizeof(bridgename) - 1] = '\0';
        }
        if ( devtalk != NULL )
        {
            talk_free( devtalk );
        }
        if ( bridgename[0] != '\0' )
        {
            scalls( bridgename, "delif", COM_IDPATH );
        }
    }
    talk_free( cfg );
    return ttrue;
}

talk_t _add( obj_t this, param_t param )
{
    talk_t cfg;
    talk_t server;
    talk_t v;
    const char *obj;
    const char *name;
    char object[NAME_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    name = param_string( param, 1 );
    if ( name == NULL || *name == '\0' || strcmp( name, COM_ID ) == 0 )
    {
        return tfalse;
    }
    server = config_get( this, NULL );
    if ( server != NULL )
    {
        const char *auth;

        auth = json_string( server, "auth" );
        if ( auth != NULL && strcmp( auth, "static" ) == 0 )
        {
            talk_free( server );
            return tfalse;
        }
        talk_free( server );
    }
    snprintf( object, sizeof(object), PROJECT_ID"%c%s", PROJECT_OBJECT_GAPC, name );
    cfg = config_sget( object, NULL );
    if ( cfg != NULL )
    {
        talk_free( cfg );
        return tfalse;
    }
    v = json_create( NULL );
    json_set_string( v, "status", "enable" );
    json_set_string( v, "cn", "" );
    json_set_string( v, "tunnelip", "" );
    json_set_string( v, "tunnelpeer", "" );
    json_set_string( v, "password", "" );
    json_set_json( v, "local_net", json_create( NULL ) );
    json_set_json( v, "remote_net", json_create( NULL ) );
    json_set_string( v, "redirect", "" );
    json_set_string( v, "push_reset", "disable" );
    json_set_string( v, "dns", "" );
    json_set_string( v, "dns2", "" );
    json_set_string( v, "options", "" );
    if ( config_sset( object, v, NULL ) != true )
    {
        talk_free( v );
        return tfalse;
    }
    talk_free( v );
    com_register( object, COM_IDPATH, 0 );
    return string2x( object );
}
talk_t _delete( obj_t this, param_t param )
{
    const char *obj;
    const char *name;
    char object[NAME_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    name = param_string( param, 1 );
    if ( name == NULL || *name == '\0' )
    {
        return tfalse;
    }
    if ( strchr( name, PROJECT_OBJECT_GAPC ) != NULL )
    {
        strncpy( object, name, sizeof(object) - 1 );
        object[sizeof(object) - 1] = '\0';
    }
    else
    {
        snprintf( object, sizeof(object), PROJECT_ID"%c%s", PROJECT_OBJECT_GAPC, name );
    }
    if ( strcmp( object, COM_IDPATH ) == 0 )
    {
        return tfalse;
    }
    com_unregister( object );
    config_sset( object, NULL, NULL );
    return ttrue;
}
talk_t _list( obj_t this, param_t param )
{
    talk_t ret;
    talk_t list;
    talk_t axp;
    talk_t cfg;
    const char *obj;
    const char *name;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return NULL;
    }
    ret = json_create( NULL );
    list = config_list( PROJECT_ID );
    if ( list == NULL )
    {
        return ret;
    }
    axp = NULL;
    while ( NULL != ( axp = json_next( list, axp ) ) )
    {
        name = axp_name( axp );
        if ( name == NULL || strcmp( name, COM_IDPATH ) == 0 )
        {
            continue;
        }
        cfg = config_sget( name, NULL );
        if ( cfg != NULL )
        {
            json_set_json( ret, name, cfg );
        }
    }
    talk_free( list );
    return ret;
}

talk_t _status( obj_t this, param_t param )
{
    FILE *fp;
    talk_t ret;
    talk_t byname;
    talk_t serials;
    char line[512];
    const char *obj;
    int in_clients;
    int in_routes;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return NULL;
    }
    ret = json_create( NULL );
    if ( spid( COM_IDPATH ) < 0 )
    {
        json_set_string( ret, "status", "down" );
        json_set_json( ret, "client", json_create( NULL ) );
        return ret;
    }
    json_set_string( ret, "status", "up" );
    byname = json_create( NULL );
    serials = file2json( SERIAL_FILE );
    fp = fopen( STATUS_FILE, "r" );
    in_clients = 0;
    in_routes = 0;
    if ( fp != NULL )
    {
        while ( fgets( line, (int)sizeof(line), fp ) != NULL )
        {
            char *nl;
            char *p1;
            char *p2;
            char *p3;
            char *p4;
            char *p5;

            nl = strchr( line, '\n' );
            if ( nl != NULL )
            {
                *nl = '\0';
            }
            if ( strncmp( line, "Common Name,", 12 ) == 0 )
            {
                in_clients = 1;
                in_routes = 0;
                continue;
            }
            if ( strncmp( line, "Virtual Address,", 16 ) == 0 )
            {
                in_clients = 0;
                in_routes = 1;
                continue;
            }
            if ( strncmp( line, "ROUTING TABLE", 13 ) == 0 || strncmp( line, "GLOBAL STATS", 12 ) == 0 || strncmp( line, "END", 3 ) == 0 )
            {
                if ( strncmp( line, "ROUTING TABLE", 13 ) == 0 )
                {
                    in_clients = 0;
                }
                else
                {
                    in_clients = 0;
                    in_routes = 0;
                }
                continue;
            }
            if ( in_clients )
            {
                talk_t one;

                p1 = strtok( line, "," );
                p2 = strtok( NULL, "," );
                p3 = strtok( NULL, "," );
                p4 = strtok( NULL, "," );
                p5 = strtok( NULL, "," );
                if ( p1 == NULL || p2 == NULL )
                {
                    continue;
                }
                one = json_create( NULL );
                json_set_string( one, "name", p1 );
                json_set_string( one, "real", p2 );
                json_set_string( one, "rx", p3 ? p3 : "0" );
                json_set_string( one, "tx", p4 ? p4 : "0" );
                json_set_string( one, "since", p5 ? p5 : "" );
                json_set_string( one, "virtual", "" );
                json_set_string( one, "serial", ( serials != NULL && json_string( serials, p2 ) != NULL ) ? json_string( serials, p2 ) : "" );
                json_set_json( byname, p1, one );
            }
            else if ( in_routes )
            {
                talk_t one;
                const char *cname;

                p1 = strtok( line, "," );
                p2 = strtok( NULL, "," );
                if ( p1 == NULL || p2 == NULL )
                {
                    continue;
                }
                cname = p2;
                one = json_json( byname, cname );
                if ( one != NULL && ( json_string( one, "virtual" ) == NULL || json_string( one, "virtual" )[0] == '\0' ) )
                {
                    json_set_string( one, "virtual", p1 );
                }
            }
        }
        fclose( fp );
    }
    if ( serials != NULL )
    {
        talk_free( serials );
    }
    json_set_json( ret, "client", byname );
    return ret;
}

talk_t _disconnect( obj_t this, param_t param )
{
    const char *obj;
    const char *real;
    int fd;
    struct sockaddr_un un;
    char cmd[128];
    char buf[256];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    real = param_string( param, 1 );
    if ( real == NULL || *real == '\0' || strlen( real ) > 80 )
    {
        return tfalse;
    }
    fd = socket( AF_UNIX, SOCK_STREAM, 0 );
    if ( fd < 0 )
    {
        return tfalse;
    }
    memset( &un, 0, sizeof(un) );
    un.sun_family = AF_UNIX;
    strncpy( un.sun_path, MGMT_SOCK, sizeof(un.sun_path) - 1 );
    if ( connect( fd, (struct sockaddr *)&un, sizeof(un) ) != 0 )
    {
        close( fd );
        return tfalse;
    }
    recv( fd, buf, sizeof(buf) - 1, 0 );
    snprintf( cmd, sizeof(cmd), "kill %s\n", real );
    if ( send( fd, cmd, strlen( cmd ), 0 ) < 0 )
    {
        close( fd );
        return tfalse;
    }
    recv( fd, buf, sizeof(buf) - 1, 0 );
    close( fd );
    return ttrue;
}

talk_t _import_ca( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.ca" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_cert( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_key( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_tlsauth( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_dh( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_pkcs12( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _import_static( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char path[PATH_MAX];

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL || *file == '\0' )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, NULL );
    shell( "mkdir -p %s", path );
    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    if ( shell( "cp -f %s %s", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

talk_t _clear_ca( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.ca" );
    unlink( path );
    return ttrue;
}

talk_t _clear_cert( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    unlink( path );
    return ttrue;
}

talk_t _clear_key( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    unlink( path );
    return ttrue;
}

talk_t _clear_tlsauth( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    unlink( path );
    return ttrue;
}

talk_t _clear_dh( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    unlink( path );
    return ttrue;
}

talk_t _clear_pkcs12( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    unlink( path );
    return ttrue;
}

talk_t _clear_static( obj_t this, param_t param )
{
    const char *obj;
    char path[PATH_MAX];

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    unlink( path );
    return ttrue;
}

talk_t _key( obj_t this, param_t param )
{
    talk_t ret;
    const char *obj;
    char path[PATH_MAX];
    const char *names[] = { "ca", "crt", "key", "tlsauth", "dh", "p12", "static", NULL };
    const char *keys[] = { "ca", "crt", "key", "tlsauth", "dh", "pkcs12", "static", NULL };
    int i;

    (void)param;
    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return NULL;
    }
    ret = json_create( NULL );
    for ( i = 0; names[i] != NULL; i++ )
    {
        config_path( path, sizeof(path), PROJECT_ID, "server.%s", names[i] );
        if ( access( path, F_OK ) == 0 )
        {
            json_set_string( ret, keys[i], path );
        }
    }
    return ret;
}

talk_t _verify( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    FILE *fp;
    char user[256];
    char pass[256];
    char *nl;
    talk_t cfg;
    talk_t radius;
    const char *auth;
    const char *st;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return tfalse;
    }
    file = param_string( param, 1 );
    if ( file == NULL )
    {
        return tfalse;
    }
    fp = fopen( file, "r" );
    if ( fp == NULL )
    {
        return tfalse;
    }
    if ( fgets( user, (int)sizeof(user), fp ) == NULL )
    {
        fclose( fp );
        return tfalse;
    }
    if ( fgets( pass, (int)sizeof(pass), fp ) == NULL )
    {
        pass[0] = '\0';
    }
    fclose( fp );
    nl = strchr( user, '\n' );
    if ( nl ) *nl = '\0';
    nl = strchr( pass, '\n' );
    if ( nl ) *nl = '\0';
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return tfalse;
    }
    auth = json_string( cfg, "auth" );
    if ( auth == NULL || *auth == '\0' )
    {
        auth = "certificate";
    }
    if ( strcmp( auth, "username" ) != 0 && strcmp( auth, "certificate+username" ) != 0 )
    {
        talk_free( cfg );
        return tfalse;
    }
    radius = json_json( cfg, "radius" );
    st = NULL;
    if ( radius != NULL )
    {
        st = json_string( radius, "status" );
    }
    if ( st != NULL && strcmp( st, "enable" ) == 0 )
    {
        int port;
        int wait_sec;
        int ok;

        port = atoi( ((json_string( radius, "port" )) != NULL && *(json_string( radius, "port" )) != '\0' ? (json_string( radius, "port" )) : ("1812")) );
        wait_sec = atoi( ((json_string( radius, "wait" )) != NULL && *(json_string( radius, "wait" )) != '\0' ? (json_string( radius, "wait" )) : ("5")) );
        ok = radius_access( json_string( radius, "host" ), json_string( radius, "secret" ), port, wait_sec, user, pass );
        talk_free( cfg );
        return ok ? ttrue : tfalse;
    }
    talk_free( cfg );
    {
        talk_t list;
        talk_t axp;
        talk_t one;
        const char *cname;
        const char *pw;
        char cn[256];
        int ok;

        ok = 0;
        list = NULL;
        axp = NULL;
        while ( NULL != ( one = each_client( &list, &axp, &cname ) ) )
        {
            client_cn( one, cname, cn, (int)sizeof(cn) );
            pw = json_string( one, "password" );
            if ( pw == NULL )
            {
                pw = "";
            }
            if ( strcmp( cn, user ) == 0 && strcmp( pw, pass ) == 0 )
            {
                ok = 1;
            }
            talk_free( one );
            if ( ok )
            {
                break;
            }
        }
        if ( list != NULL )
        {
            talk_free( list );
        }
        return ok ? ttrue : tfalse;
    }
}

talk_t _note( obj_t this, param_t param )
{
    const char *file;
    const char *obj;
    char lines[6][256];
    talk_t map;
    talk_t cfg;
    FILE *fp;
    int n;
    int i;
    char *nl;

    obj = obj_com( this );
    if ( obj == NULL || strcmp( obj, COM_ID ) != 0 )
    {
        return ttrue;
    }
    file = param_string( param, 1 );
    n = 0;
    fp = file != NULL ? fopen( file, "r" ) : NULL;
    if ( fp != NULL )
    {
        for ( i = 0; i < 6; i++ )
        {
            if ( fgets( lines[i], 256, fp ) == NULL )
            {
                lines[i][0] = '\0';
                break;
            }
            nl = strchr( lines[i], '\n' );
            if ( nl != NULL )
            {
                *nl = '\0';
            }
            n++;
        }
        fclose( fp );
    }
    if ( n < 1 )
    {
        return ttrue;
    }
    cfg = config_get( this, NULL );
    if ( (strcmp(lines[0], "connect") == 0) && n >= 4 )
    {
        if ( mkdir( RUNDIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0 && errno != EEXIST )
        {
            if ( cfg != NULL )
            {
                talk_free( cfg );
            }
            return ttrue;
        }
        map = file2json( SERIAL_FILE );
        if ( map == NULL )
        {
            map = json_create( NULL );
        }
        json_set_string( map, lines[2], lines[3] );
        json2file( map, SERIAL_FILE );
        talk_free( map );
        if ( cfg != NULL )
        {
            radius_acct( cfg, "start", lines[1], lines[2], NULL, NULL );
        }
    }
    else if ( (strcmp(lines[0], "disconnect") == 0) && cfg != NULL && n >= 3 )
    {
        radius_acct( cfg, "stop", lines[1], lines[2], n > 3 ? lines[3] : "", n > 4 ? lines[4] : "" );
    }
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }
    return ttrue;
}

static const eapi_table_t exe_api_table[] = {
    { "setup", (comapi_t)_setup },
    { "shut", (comapi_t)_shut },
    { "service", (comapi_t)_service },
    { "online", (comapi_t)_online },
    { "offline", (comapi_t)_offline },
    { "add", (comapi_t)_add },
    { "delete", (comapi_t)_delete },
    /* "list" is reserved on exe (method table); WUI/clients use clientlist */
    { "clientlist", (comapi_t)_list },
    { "status", (comapi_t)_status },
    { "disconnect", (comapi_t)_disconnect },
    { "import_ca", (comapi_t)_import_ca },
    { "import_cert", (comapi_t)_import_cert },
    { "import_key", (comapi_t)_import_key },
    { "import_tlsauth", (comapi_t)_import_tlsauth },
    { "import_dh", (comapi_t)_import_dh },
    { "import_pkcs12", (comapi_t)_import_pkcs12 },
    { "import_static", (comapi_t)_import_static },
    { "clear_ca", (comapi_t)_clear_ca },
    { "clear_cert", (comapi_t)_clear_cert },
    { "clear_key", (comapi_t)_clear_key },
    { "clear_tlsauth", (comapi_t)_clear_tlsauth },
    { "clear_dh", (comapi_t)_clear_dh },
    { "clear_pkcs12", (comapi_t)_clear_pkcs12 },
    { "clear_static", (comapi_t)_clear_static },
    { "key", (comapi_t)_key },
    { "verify", (comapi_t)_verify },
    { "note", (comapi_t)_note },
};
MAIN2COM( exe_api_table, _get, _set );
