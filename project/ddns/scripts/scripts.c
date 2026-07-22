/*
 *  Description: ddns-scripts management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"



boole_t _setup( obj_t this, param_t param )
{
    const char *ptr;

    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "client/status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        sruns( 0, COM_IDPATH"-client", COM_IDPATH, "service", "client"  );
    }

    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "client2/status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        sruns( 0, COM_IDPATH"-client2", COM_IDPATH, "service", "client2" );
    }

    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "client3/status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
        sruns( 0, COM_IDPATH"-client3", COM_IDPATH, "service", "client3" );
    }

    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	const char *ptr;
	char path[PATH_MAX];

	ptr = "client";
    sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path);
	ptr = "client2";
    sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path);
	ptr = "client3";
    sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path);
    return ttrue;
}
talk_t _service( obj_t this, param_t param )
{
    talk_t cfg;
	const char *id;
	const char *ptr;
    const char *ifname;
    const char *isp;
    const char *domain;
    const char *username;
    const char *password;
    char scripts[PATH_MAX];
    char connect_ip[NAME_MAX];
    char connect_interface[NAME_MAX];

	id = param_string( param, 1 );
    cfg = config_sgets( COM_IDPATH, id );
    /* get the ext ifname */
    memset( connect_ip, 0, sizeof(connect_ip) );
    memset( connect_interface, 0, sizeof(connect_interface) );
    ifname = json_string( cfg, "extern" );
    if ( ifname != NULL && *ifname != '\0' )
    {
        ptr = scall_string( connect_interface, sizeof(connect_interface), ifname, "netdev", NULL );
        if ( ptr == NULL )
        {
            default_warn( "%s cannot find extern interface", COM_IDPATH );
            talk_free( cfg );
            sleep( 5 );
            return tfalse;
        }
        if ( netdev_info( ptr, connect_ip, sizeof(connect_ip), NULL, 0, NULL, 0, NULL, 0 ) != 0 )
        {
            default_warn( "%s cannot find ip address on %s", COM_IDPATH, ptr );
            talk_free( cfg );
            sleep( 5 );
            return tfalse;
        }
    }
    else
    {
        if ( gateway_info( NULL, connect_interface ) == false )
        {
            default_warn( "%s cannot find gateway route", COM_IDPATH );
            talk_free( cfg );
            sleep( 5 );
            return tfalse;
        }
        if ( netdev_info( connect_interface, connect_ip, sizeof(connect_ip), NULL, 0, NULL, 0, NULL, 0 ) != 0 )
        {
            default_warn( "%s cannot find gateway ip address", COM_IDPATH );
            talk_free( cfg );
            sleep( 5 );
            return tfalse;
        }
    }

	isp = json_string( cfg, "isp" );
	domain = json_string( cfg, "domain" );
	username = json_string( cfg, "username" );
	password = json_string( cfg, "password" );
	if ( isp == NULL || *isp == '\0' || domain == NULL || *domain == '\0' || username == NULL || *username == '\0' || password == NULL || *password == '\0' )
	{
		talk_free( cfg );
		return terror;
	}

	misc2path( scripts, sizeof(scripts), "libddns/dynamic_dns_updater.sh" );
	execlp( scripts, "ddns-scripts", "-S", id, "-v", "1", "start", (char *)0 );
	default_faulting( "execlp the ddns-scripts error" );

    talk_free( cfg );
    return tfalse;
}
talk_t _status( obj_t this, param_t param )
{
	talk_t v;
	talk_t ret;
	const char *ptr;
	char path[PATH_MAX];

	ret = json_create( NULL );

	ptr = "client";
	var2path( path, sizeof(path), "%s", ptr );
	v = file2json( path );
	json_set_value( ret, ptr, v );
	ptr = "client2";
	var2path( path, sizeof(path), "%s", ptr );
	v = file2json( path );
	json_set_value( ret, ptr, v );
	ptr = "client3";
	var2path( path, sizeof(path), "%s", ptr );
	v = file2json( path );
	json_set_value( ret, ptr, v );

	return ret;
}
talk_t _isplist( obj_t this, param_t param )
{
	char *ptr;
    DIR *pdir;
    talk_t list;
    struct dirent *pent;
	char dir[PATH_MAX];
	char ispname[NAME_MAX];

    list = json_create(NULL);
	misc2path( dir, sizeof(dir), "shareddns/default" );
	/* list the ./shareddns */
    pdir = opendir( dir );
    if ( pdir != NULL )
    {
        while( NULL != ( pent = readdir( pdir ) ) )
        {
            if ( *pent->d_name == '\0' || *pent->d_name == '.' )
            {
                continue;
            }
			strncpy( ispname, pent->d_name, sizeof(ispname) );
			ptr = strstr( ispname, ".json" );
			if ( ptr == NULL )
			{
				continue;
			}
			*ptr = '\0';
			json_set_string( list, ispname, pent->d_name );
        }
        closedir( pdir );
    }
	return list;
}



boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

    ret = config_sset( COM_IDPATH, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	return config_sget( COM_IDPATH, path );
}
