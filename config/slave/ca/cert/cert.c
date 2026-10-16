/*
 *  Description:  certificate authority for OpenVPN and IPsec
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"
#include <dirent.h>



talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}
boole _set( obj_t this, talk_t v, attr_t path )
{
    return config_set( this, v, path );
}



/* list CA / server / tls / dh files that exist */
talk_t _key( obj_t this, param_t param )
{
    talk_t ret;
    char path[PATH_MAX];

    ret = json_create( NULL );
    config_path( path, sizeof(path), PROJECT_ID, "ca.crt" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "ca", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "ca.key" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "cakey", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "crt", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "key", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "p12", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "tls", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsv2" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "tlsv2", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "static", path );
    }
    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    if ( access( path, F_OK ) == 0 )
    {
        json_set_string( ret, "dh", path );
    }
    return ret;
}

/* list client-<name>.crt / .key / .p12 */
talk_t _list_client( obj_t this, param_t param )
{
    DIR *pdir;
    struct dirent *pent;
    talk_t ret;
    char dir[PATH_MAX];

    ret = json_create( NULL );
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    pdir = opendir( dir );
    if ( pdir == NULL )
    {
        return ret;
    }
    while ( ( pent = readdir( pdir ) ) != NULL )
    {
        int nlen;
        int namelen;
        talk_t one;
        char name[NAME_MAX];
        char path[PATH_MAX * 2];
        const char *fn;

        fn = pent->d_name;
        if ( fn == NULL )
        {
            continue;
        }
        nlen = (int)strlen( fn );
        /* client- + at least 1 char + .crt */
        if ( nlen < 12 || strncmp( fn, "client-", 7 ) != 0 || strcmp( fn + nlen - 4, ".crt" ) != 0 )
        {
            continue;
        }
        namelen = nlen - 7 - 4;
        if ( namelen <= 0 || namelen >= (int)sizeof(name) )
        {
            continue;
        }
        memcpy( name, fn + 7, namelen );
        name[namelen] = '\0';
        one = json_create( NULL );
        snprintf( path, sizeof(path), "%s%s", dir, fn );
        json_set_string( one, "crt", path );
        snprintf( path, sizeof(path), "%sclient-%s.key", dir, name );
        if ( access( path, F_OK ) == 0 )
        {
            json_set_string( one, "key", path );
        }
        snprintf( path, sizeof(path), "%sclient-%s.p12", dir, name );
        if ( access( path, F_OK ) == 0 )
        {
            json_set_string( one, "p12", path );
        }
        json_set_value( ret, name, one );
    }
    closedir( pdir );
    return ret;
}



talk_t _clear_ca( obj_t this, param_t param )
{
    char dir[PATH_MAX];
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "ca.crt" );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "ca.key" );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "ca.srl" );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    unlink( path );
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "rm -f %sclient-*.crt %sclient-*.key %sclient-*.p12 >/dev/null 2>&1", dir, dir, dir );
    return ttrue;
}
talk_t _clear_cakey( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "ca.key" );
    unlink( path );
    return ttrue;
}
talk_t _clear_cert( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    unlink( path );
    return ttrue;
}
talk_t _clear_key( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    unlink( path );
    return ttrue;
}
talk_t _clear_pkcs12( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    unlink( path );
    return ttrue;
}
talk_t _clear_tls( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    unlink( path );
    return ttrue;
}
talk_t _clear_tlsv2( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.tlsv2" );
    unlink( path );
    return ttrue;
}
talk_t _clear_static( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    unlink( path );
    return ttrue;
}
talk_t _clear_dh( obj_t this, param_t param )
{
    char path[PATH_MAX];

    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    unlink( path );
    return ttrue;
}



talk_t _import_ca( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "ca.crt" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_cakey( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "ca.key" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_cert( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.crt" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_key( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.key" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_pkcs12( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.p12" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_tls( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_tlsv2( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsv2" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_static( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}
talk_t _import_dh( obj_t this, param_t param )
{
    const char *file;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    file = param_string( param, 1 );
    if ( file == NULL || file[0] == '\0' || strchr( file, '\'' ) != NULL )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    if ( shell( "cp -f '%s' '%s'", file, path ) != 0 )
    {
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}



/* build a new CA, then drop certificates signed by the previous CA */
talk_t _make_ca( obj_t this, param_t param )
{
    talk_t cfg;
    FILE *fp;
    int days;
    int bits;
    const char *country;
    const char *province;
    const char *city;
    const char *org;
    const char *ou;
    const char *email;
    const char *cn;
    char dir[PATH_MAX];
    char cnf[PATH_MAX];
    char key[PATH_MAX];
    char crt[PATH_MAX];
    char srl[PATH_MAX];

    cfg = config_get( this, NULL );
    country = "CN";
    province = "GD";
    city = "ShenZhen";
    org = "Ashyelf";
    ou = "SkinOS";
    email = "";
    cn = "ca";
    days = 3650;
    bits = 2048;
    if ( cfg != NULL )
    {
        const char *s;

        s = json_string( cfg, "country" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            country = s;
        }
        s = json_string( cfg, "province" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            province = s;
        }
        s = json_string( cfg, "city" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            city = s;
        }
        s = json_string( cfg, "org" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            org = s;
        }
        s = json_string( cfg, "ou" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            ou = s;
        }
        s = json_string( cfg, "email" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            email = s;
        }
        s = json_string( cfg, "cn" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            cn = s;
        }
        s = json_string( cfg, "days" );
        if ( s != NULL && s[0] != '\0' )
        {
            days = atoi( s );
        }
        s = json_string( cfg, "bits" );
        if ( s != NULL && s[0] != '\0' )
        {
            bits = atoi( s );
        }
    }
    if ( days < 1 || days > 36500 )
    {
        days = 3650;
    }
    if ( bits != 2048 && bits != 4096 )
    {
        bits = 2048;
    }

    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( cnf, sizeof(cnf), PROJECT_ID, "ca.cnf" );
    config_path( key, sizeof(key), PROJECT_ID, "ca.key" );
    config_path( crt, sizeof(crt), PROJECT_ID, "ca.crt" );
    config_path( srl, sizeof(srl), PROJECT_ID, "ca.srl" );
    fp = fopen( cnf, "w" );
    if ( fp == NULL )
    {
        if ( cfg != NULL )
        {
            talk_free( cfg );
        }
        return tfalse;
    }
    fprintf( fp, "[ req ]\n" );
    fprintf( fp, "distinguished_name = dn\n" );
    fprintf( fp, "x509_extensions = v3_ca\n" );
    fprintf( fp, "prompt = no\n" );
    fprintf( fp, "utf8 = yes\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "[ dn ]\n" );
    fprintf( fp, "C = %s\n", country );
    fprintf( fp, "ST = %s\n", province );
    fprintf( fp, "L = %s\n", city );
    fprintf( fp, "O = %s\n", org );
    fprintf( fp, "OU = %s\n", ou );
    fprintf( fp, "CN = %s\n", cn );
    if ( email[0] != '\0' )
    {
        fprintf( fp, "emailAddress = %s\n", email );
    }
    fprintf( fp, "\n" );
    fprintf( fp, "[ v3_ca ]\n" );
    fprintf( fp, "basicConstraints = critical, CA:TRUE\n" );
    fprintf( fp, "keyUsage = critical, keyCertSign, cRLSign\n" );
    fprintf( fp, "subjectKeyIdentifier = hash\n" );
    fprintf( fp, "authorityKeyIdentifier = keyid:always,issuer\n" );
    fclose( fp );
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }

    unlink( key );
    unlink( crt );
    if ( shell( "openssl genrsa -out %s %d >/dev/null 2>&1", key, bits ) != 0 )
    {
        unlink( key );
        unlink( cnf );
        app_fault( "%s make_ca genrsa failed", COM_IDPATH );
        return tfalse;
    }
    if ( shell( "openssl req -new -x509 -key %s -out %s -days %d -config %s >/dev/null 2>&1", key, crt, days, cnf ) != 0 )
    {
        unlink( key );
        unlink( crt );
        unlink( cnf );
        app_fault( "%s make_ca req failed", COM_IDPATH );
        return tfalse;
    }
    unlink( cnf );
    chmod( key, S_IRUSR | S_IWUSR );
    chmod( crt, S_IRUSR | S_IWUSR );
    fp = fopen( srl, "w" );
    if ( fp != NULL )
    {
        fprintf( fp, "01\n" );
        fclose( fp );
    }
    config_path( key, sizeof(key), PROJECT_ID, "server.key" );
    unlink( key );
    config_path( crt, sizeof(crt), PROJECT_ID, "server.crt" );
    unlink( crt );
    config_path( crt, sizeof(crt), PROJECT_ID, "server.p12" );
    unlink( crt );
    shell( "rm -f %sclient-*.crt %sclient-*.key %sclient-*.p12 >/dev/null 2>&1", dir, dir, dir );
    app_info( "%s make_ca", COM_IDPATH );
    return ttrue;
}

/* server certificate: OpenVPN server and IPsec local certificate */
talk_t _make_server( obj_t this, param_t param )
{
    talk_t cfg;
    FILE *fp;
    int days;
    int bits;
    const char *country;
    const char *province;
    const char *city;
    const char *org;
    const char *ou;
    const char *email;
    const char *cn;
    const char *password;
    char passbuf[128];
    char dir[PATH_MAX];
    char cnf[PATH_MAX];
    char csr[PATH_MAX];
    char key[PATH_MAX];
    char crt[PATH_MAX];
    char p12[PATH_MAX];
    char cacrt[PATH_MAX];
    char cakey[PATH_MAX];
    char srl[PATH_MAX];
    char pass[PATH_MAX];

    config_path( cacrt, sizeof(cacrt), PROJECT_ID, "ca.crt" );
    config_path( cakey, sizeof(cakey), PROJECT_ID, "ca.key" );
    config_path( srl, sizeof(srl), PROJECT_ID, "ca.srl" );
    if ( access( cacrt, R_OK ) != 0 || access( cakey, R_OK ) != 0 )
    {
        return tfalse;
    }
    cfg = config_get( this, NULL );
    country = "CN";
    province = "GD";
    city = "ShenZhen";
    org = "Ashyelf";
    ou = "SkinOS";
    email = "";
    cn = "server";
    password = "";
    days = 3650;
    bits = 2048;
    if ( cfg != NULL )
    {
        const char *s;

        s = json_string( cfg, "country" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            country = s;
        }
        s = json_string( cfg, "province" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            province = s;
        }
        s = json_string( cfg, "city" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            city = s;
        }
        s = json_string( cfg, "org" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            org = s;
        }
        s = json_string( cfg, "ou" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            ou = s;
        }
        s = json_string( cfg, "email" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            email = s;
        }
        s = json_string( cfg, "server_cn" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            cn = s;
        }
        s = json_string( cfg, "password" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '\r' ) == NULL )
        {
            password = s;
        }
        s = json_string( cfg, "days" );
        if ( s != NULL && s[0] != '\0' )
        {
            days = atoi( s );
        }
        s = json_string( cfg, "bits" );
        if ( s != NULL && s[0] != '\0' )
        {
            bits = atoi( s );
        }
    }
    if ( days < 1 || days > 36500 )
    {
        days = 3650;
    }
    if ( bits != 2048 && bits != 4096 )
    {
        bits = 2048;
    }
    passbuf[0] = '\0';
    if ( password[0] != '\0' )
    {
        strncpy( passbuf, password, sizeof(passbuf) - 1 );
        passbuf[sizeof(passbuf) - 1] = '\0';
    }

    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( cnf, sizeof(cnf), PROJECT_ID, "server.cnf" );
    config_path( csr, sizeof(csr), PROJECT_ID, "server.csr" );
    config_path( key, sizeof(key), PROJECT_ID, "server.key" );
    config_path( crt, sizeof(crt), PROJECT_ID, "server.crt" );
    config_path( p12, sizeof(p12), PROJECT_ID, "server.p12" );
    fp = fopen( cnf, "w" );
    if ( fp == NULL )
    {
        if ( cfg != NULL )
        {
            talk_free( cfg );
        }
        return tfalse;
    }
    fprintf( fp, "[ req ]\n" );
    fprintf( fp, "distinguished_name = dn\n" );
    fprintf( fp, "prompt = no\n" );
    fprintf( fp, "utf8 = yes\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "[ dn ]\n" );
    fprintf( fp, "C = %s\n", country );
    fprintf( fp, "ST = %s\n", province );
    fprintf( fp, "L = %s\n", city );
    fprintf( fp, "O = %s\n", org );
    fprintf( fp, "OU = %s\n", ou );
    fprintf( fp, "CN = %s\n", cn );
    if ( email[0] != '\0' )
    {
        fprintf( fp, "emailAddress = %s\n", email );
    }
    fprintf( fp, "\n" );
    fprintf( fp, "[ v3_server ]\n" );
    fprintf( fp, "basicConstraints = CA:FALSE\n" );
    fprintf( fp, "nsCertType = server\n" );
    fprintf( fp, "keyUsage = digitalSignature, keyEncipherment\n" );
    fprintf( fp, "extendedKeyUsage = serverAuth\n" );
    fprintf( fp, "subjectKeyIdentifier = hash\n" );
    fprintf( fp, "authorityKeyIdentifier = keyid,issuer\n" );
    fclose( fp );
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }
    if ( access( srl, F_OK ) != 0 )
    {
        fp = fopen( srl, "w" );
        if ( fp != NULL )
        {
            fprintf( fp, "01\n" );
            fclose( fp );
        }
    }

    unlink( key );
    unlink( crt );
    unlink( p12 );
    if ( shell( "openssl genrsa -out %s %d >/dev/null 2>&1", key, bits ) != 0 )
    {
        unlink( key );
        unlink( cnf );
        app_fault( "%s make_server genrsa failed", COM_IDPATH );
        return tfalse;
    }
    if ( shell( "openssl req -new -key %s -out %s -config %s >/dev/null 2>&1", key, csr, cnf ) != 0 )
    {
        unlink( key );
        unlink( csr );
        unlink( cnf );
        app_fault( "%s make_server req failed", COM_IDPATH );
        return tfalse;
    }
    if ( shell( "openssl x509 -req -in %s -CA %s -CAkey %s -CAserial %s -out %s -days %d -extfile %s -extensions v3_server >/dev/null 2>&1", csr, cacrt, cakey, srl, crt, days, cnf ) != 0 )
    {
        unlink( key );
        unlink( crt );
        unlink( csr );
        unlink( cnf );
        app_fault( "%s make_server x509 failed", COM_IDPATH );
        return tfalse;
    }
    unlink( csr );
    unlink( cnf );
    chmod( key, S_IRUSR | S_IWUSR );
    chmod( crt, S_IRUSR | S_IWUSR );

    if ( passbuf[0] == '\0' )
    {
        if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name server -out %s -passout pass: >/dev/null 2>&1", key, crt, cacrt, p12 ) != 0 )
        {
            unlink( p12 );
            app_fault( "%s make_server pkcs12 failed", COM_IDPATH );
        }
    }
    else
    {
        config_path( pass, sizeof(pass), PROJECT_ID, "p12.pass" );
        fp = fopen( pass, "w" );
        if ( fp != NULL )
        {
            fprintf( fp, "%s\n", passbuf );
            fclose( fp );
            chmod( pass, S_IRUSR | S_IWUSR );
            if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name server -out %s -passout file:%s >/dev/null 2>&1", key, crt, cacrt, p12, pass ) != 0 )
            {
                unlink( p12 );
                app_fault( "%s make_server pkcs12 failed", COM_IDPATH );
            }
            unlink( pass );
        }
    }
    if ( access( p12, F_OK ) == 0 )
    {
        chmod( p12, S_IRUSR | S_IWUSR );
    }
    app_info( "%s make_server", COM_IDPATH );
    return ttrue;
}

/* pack the existing server certificate into PKCS#12 */
talk_t _make_pkcs12( obj_t this, param_t param )
{
    talk_t cfg;
    FILE *fp;
    const char *password;
    char passbuf[128];
    char key[PATH_MAX];
    char crt[PATH_MAX];
    char p12[PATH_MAX];
    char cacrt[PATH_MAX];
    char pass[PATH_MAX];

    config_path( cacrt, sizeof(cacrt), PROJECT_ID, "ca.crt" );
    config_path( crt, sizeof(crt), PROJECT_ID, "server.crt" );
    config_path( key, sizeof(key), PROJECT_ID, "server.key" );
    config_path( p12, sizeof(p12), PROJECT_ID, "server.p12" );
    if ( access( cacrt, R_OK ) != 0 || access( crt, R_OK ) != 0 || access( key, R_OK ) != 0 )
    {
        return tfalse;
    }
    password = "";
    cfg = config_get( this, NULL );
    if ( cfg != NULL )
    {
        const char *s;

        s = json_string( cfg, "password" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '\r' ) == NULL )
        {
            password = s;
        }
    }
    passbuf[0] = '\0';
    if ( password[0] != '\0' )
    {
        strncpy( passbuf, password, sizeof(passbuf) - 1 );
        passbuf[sizeof(passbuf) - 1] = '\0';
    }
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }

    unlink( p12 );
    if ( passbuf[0] == '\0' )
    {
        if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name server -out %s -passout pass: >/dev/null 2>&1", key, crt, cacrt, p12 ) != 0 )
        {
            unlink( p12 );
            return tfalse;
        }
    }
    else
    {
        config_path( pass, sizeof(pass), PROJECT_ID, "p12.pass" );
        fp = fopen( pass, "w" );
        if ( fp == NULL )
        {
            return tfalse;
        }
        fprintf( fp, "%s\n", passbuf );
        fclose( fp );
        chmod( pass, S_IRUSR | S_IWUSR );
        if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name server -out %s -passout file:%s >/dev/null 2>&1", key, crt, cacrt, p12, pass ) != 0 )
        {
            unlink( pass );
            unlink( p12 );
            return tfalse;
        }
        unlink( pass );
    }
    chmod( p12, S_IRUSR | S_IWUSR );
    return ttrue;
}

/* OpenVPN tls-auth / tls-crypt static key */
talk_t _make_tls( obj_t this, param_t param )
{
    int i;
    int n;
    FILE *fp;
    unsigned char raw[256];
    char dir[PATH_MAX];
    char path[PATH_MAX];

    fp = fopen( "/dev/urandom", "r" );
    if ( fp == NULL )
    {
        return tfalse;
    }
    n = (int)fread( raw, 1, sizeof(raw), fp );
    fclose( fp );
    if ( n != (int)sizeof(raw) )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsauth" );
    fp = fopen( path, "w" );
    if ( fp == NULL )
    {
        return tfalse;
    }
    fprintf( fp, "#\n# 2048 bit OpenVPN static key\n#\n" );
    fprintf( fp, "-----BEGIN OpenVPN Static key V1-----\n" );
    for ( i = 0; i < 16; i++ )
    {
        int j;

        for ( j = 0; j < 16; j++ )
        {
            fprintf( fp, "%02x", raw[i * 16 + j] );
        }
        fprintf( fp, "\n" );
    }
    fprintf( fp, "-----END OpenVPN Static key V1-----\n" );
    fclose( fp );
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

/* OpenVPN tls-crypt-v2 server key */
talk_t _make_tlsv2( obj_t this, param_t param )
{
    char dir[PATH_MAX];
    char path[PATH_MAX];

    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.tlsv2" );
    unlink( path );
    if ( shell( "openvpn --genkey tls-crypt-v2-server %s >/dev/null 2>&1", path ) != 0 )
    {
        unlink( path );
        app_fault( "%s make_tlsv2 failed", COM_IDPATH );
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

/* OpenVPN --secret static key */
talk_t _make_static( obj_t this, param_t param )
{
    int i;
    int n;
    FILE *fp;
    unsigned char raw[256];
    char dir[PATH_MAX];
    char path[PATH_MAX];

    fp = fopen( "/dev/urandom", "r" );
    if ( fp == NULL )
    {
        return tfalse;
    }
    n = (int)fread( raw, 1, sizeof(raw), fp );
    fclose( fp );
    if ( n != (int)sizeof(raw) )
    {
        return tfalse;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.static" );
    fp = fopen( path, "w" );
    if ( fp == NULL )
    {
        return tfalse;
    }
    fprintf( fp, "#\n# 2048 bit OpenVPN static key\n#\n" );
    fprintf( fp, "-----BEGIN OpenVPN Static key V1-----\n" );
    for ( i = 0; i < 16; i++ )
    {
        int j;

        for ( j = 0; j < 16; j++ )
        {
            fprintf( fp, "%02x", raw[i * 16 + j] );
        }
        fprintf( fp, "\n" );
    }
    fprintf( fp, "-----END OpenVPN Static key V1-----\n" );
    fclose( fp );
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}

/* Diffie-Hellman parameters for OpenVPN */
talk_t _make_dh( obj_t this, param_t param )
{
    talk_t cfg;
    int bits;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    bits = 2048;
    cfg = config_get( this, NULL );
    if ( cfg != NULL )
    {
        const char *s;

        s = json_string( cfg, "bits" );
        if ( s != NULL && s[0] != '\0' )
        {
            bits = atoi( s );
        }
        talk_free( cfg );
    }
    if ( bits != 2048 && bits != 4096 )
    {
        bits = 2048;
    }
    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( path, sizeof(path), PROJECT_ID, "server.dh" );
    unlink( path );
    if ( shell( "openssl dhparam -out %s %d >/dev/null 2>&1", path, bits ) != 0 )
    {
        unlink( path );
        app_fault( "%s make_dh failed", COM_IDPATH );
        return tfalse;
    }
    chmod( path, S_IRUSR | S_IWUSR );
    return ttrue;
}



talk_t _delete_client( obj_t this, param_t param )
{
    int i;
    int n;
    const char *name;
    char path[PATH_MAX];

    name = param_string( param, 1 );
    if ( name == NULL )
    {
        return tfalse;
    }
    n = (int)strlen( name );
    if ( n < 1 || n > 32 )
    {
        return tfalse;
    }
    for ( i = 0; i < n; i++ )
    {
        char c;

        c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == '_' || c == '-' )
        {
            continue;
        }
        return tfalse;
    }
    config_path( path, sizeof(path), PROJECT_ID, "client-%s.crt", name );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "client-%s.key", name );
    unlink( path );
    config_path( path, sizeof(path), PROJECT_ID, "client-%s.p12", name );
    unlink( path );
    return ttrue;
}

/* client certificate: OpenVPN client and IPsec peer */
talk_t _add_client( obj_t this, param_t param )
{
    talk_t cfg;
    FILE *fp;
    int i;
    int n;
    int days;
    int bits;
    const char *name;
    const char *country;
    const char *province;
    const char *city;
    const char *org;
    const char *ou;
    const char *email;
    const char *password;
    char passbuf[128];
    char dir[PATH_MAX];
    char cnf[PATH_MAX];
    char csr[PATH_MAX];
    char key[PATH_MAX];
    char crt[PATH_MAX];
    char p12[PATH_MAX];
    char cacrt[PATH_MAX];
    char cakey[PATH_MAX];
    char srl[PATH_MAX];
    char pass[PATH_MAX];

    name = param_string( param, 1 );
    if ( name == NULL )
    {
        return tfalse;
    }
    n = (int)strlen( name );
    if ( n < 1 || n > 32 )
    {
        return tfalse;
    }
    for ( i = 0; i < n; i++ )
    {
        char c;

        c = name[i];
        if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == '_' || c == '-' )
        {
            continue;
        }
        return tfalse;
    }
    config_path( cacrt, sizeof(cacrt), PROJECT_ID, "ca.crt" );
    config_path( cakey, sizeof(cakey), PROJECT_ID, "ca.key" );
    config_path( srl, sizeof(srl), PROJECT_ID, "ca.srl" );
    if ( access( cacrt, R_OK ) != 0 || access( cakey, R_OK ) != 0 )
    {
        return tfalse;
    }

    cfg = config_get( this, NULL );
    country = "CN";
    province = "GD";
    city = "ShenZhen";
    org = "Ashyelf";
    ou = "SkinOS";
    email = "";
    password = "";
    days = 3650;
    bits = 2048;
    if ( cfg != NULL )
    {
        const char *s;

        s = json_string( cfg, "country" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            country = s;
        }
        s = json_string( cfg, "province" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            province = s;
        }
        s = json_string( cfg, "city" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            city = s;
        }
        s = json_string( cfg, "org" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            org = s;
        }
        s = json_string( cfg, "ou" );
        if ( s != NULL && s[0] != '\0' && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            ou = s;
        }
        s = json_string( cfg, "email" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '#' ) == NULL )
        {
            email = s;
        }
        s = json_string( cfg, "password" );
        if ( s != NULL && strchr( s, '\n' ) == NULL && strchr( s, '\r' ) == NULL )
        {
            password = s;
        }
        s = json_string( cfg, "days" );
        if ( s != NULL && s[0] != '\0' )
        {
            days = atoi( s );
        }
        s = json_string( cfg, "bits" );
        if ( s != NULL && s[0] != '\0' )
        {
            bits = atoi( s );
        }
    }
    if ( days < 1 || days > 36500 )
    {
        days = 3650;
    }
    if ( bits != 2048 && bits != 4096 )
    {
        bits = 2048;
    }
    passbuf[0] = '\0';
    if ( password[0] != '\0' )
    {
        strncpy( passbuf, password, sizeof(passbuf) - 1 );
        passbuf[sizeof(passbuf) - 1] = '\0';
    }

    config_path( dir, sizeof(dir), PROJECT_ID, NULL );
    shell( "mkdir -p %s", dir );
    config_path( cnf, sizeof(cnf), PROJECT_ID, "client-%s.cnf", name );
    config_path( csr, sizeof(csr), PROJECT_ID, "client-%s.csr", name );
    config_path( key, sizeof(key), PROJECT_ID, "client-%s.key", name );
    config_path( crt, sizeof(crt), PROJECT_ID, "client-%s.crt", name );
    config_path( p12, sizeof(p12), PROJECT_ID, "client-%s.p12", name );
    fp = fopen( cnf, "w" );
    if ( fp == NULL )
    {
        if ( cfg != NULL )
        {
            talk_free( cfg );
        }
        return tfalse;
    }
    fprintf( fp, "[ req ]\n" );
    fprintf( fp, "distinguished_name = dn\n" );
    fprintf( fp, "prompt = no\n" );
    fprintf( fp, "utf8 = yes\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "[ dn ]\n" );
    fprintf( fp, "C = %s\n", country );
    fprintf( fp, "ST = %s\n", province );
    fprintf( fp, "L = %s\n", city );
    fprintf( fp, "O = %s\n", org );
    fprintf( fp, "OU = %s\n", ou );
    fprintf( fp, "CN = %s\n", name );
    if ( email[0] != '\0' )
    {
        fprintf( fp, "emailAddress = %s\n", email );
    }
    fprintf( fp, "\n" );
    fprintf( fp, "[ v3_client ]\n" );
    fprintf( fp, "basicConstraints = CA:FALSE\n" );
    fprintf( fp, "nsCertType = client\n" );
    fprintf( fp, "keyUsage = digitalSignature\n" );
    fprintf( fp, "extendedKeyUsage = clientAuth\n" );
    fprintf( fp, "subjectKeyIdentifier = hash\n" );
    fprintf( fp, "authorityKeyIdentifier = keyid,issuer\n" );
    fclose( fp );
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }
    if ( access( srl, F_OK ) != 0 )
    {
        fp = fopen( srl, "w" );
        if ( fp != NULL )
        {
            fprintf( fp, "01\n" );
            fclose( fp );
        }
    }

    unlink( key );
    unlink( crt );
    unlink( p12 );
    if ( shell( "openssl genrsa -out %s %d >/dev/null 2>&1", key, bits ) != 0 )
    {
        unlink( key );
        unlink( cnf );
        app_fault( "%s add_client genrsa failed", COM_IDPATH );
        return tfalse;
    }
    if ( shell( "openssl req -new -key %s -out %s -config %s >/dev/null 2>&1", key, csr, cnf ) != 0 )
    {
        unlink( key );
        unlink( csr );
        unlink( cnf );
        app_fault( "%s add_client req failed", COM_IDPATH );
        return tfalse;
    }
    if ( shell( "openssl x509 -req -in %s -CA %s -CAkey %s -CAserial %s -out %s -days %d -extfile %s -extensions v3_client >/dev/null 2>&1", csr, cacrt, cakey, srl, crt, days, cnf ) != 0 )
    {
        unlink( key );
        unlink( crt );
        unlink( csr );
        unlink( cnf );
        app_fault( "%s add_client x509 failed", COM_IDPATH );
        return tfalse;
    }
    unlink( csr );
    unlink( cnf );
    chmod( key, S_IRUSR | S_IWUSR );
    chmod( crt, S_IRUSR | S_IWUSR );

    if ( passbuf[0] == '\0' )
    {
        if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name %s -out %s -passout pass: >/dev/null 2>&1", key, crt, cacrt, name, p12 ) != 0 )
        {
            unlink( p12 );
            app_fault( "%s add_client pkcs12 failed", COM_IDPATH );
        }
    }
    else
    {
        config_path( pass, sizeof(pass), PROJECT_ID, "p12.pass" );
        fp = fopen( pass, "w" );
        if ( fp != NULL )
        {
            fprintf( fp, "%s\n", passbuf );
            fclose( fp );
            chmod( pass, S_IRUSR | S_IWUSR );
            if ( shell( "openssl pkcs12 -export -inkey %s -in %s -certfile %s -name %s -out %s -passout file:%s >/dev/null 2>&1", key, crt, cacrt, name, p12, pass ) != 0 )
            {
                unlink( p12 );
                app_fault( "%s add_client pkcs12 failed", COM_IDPATH );
            }
            unlink( pass );
        }
    }
    if ( access( p12, F_OK ) == 0 )
    {
        chmod( p12, S_IRUSR | S_IWUSR );
    }
    app_info( "%s add_client %s", COM_IDPATH, name );
    return ttrue;
}
