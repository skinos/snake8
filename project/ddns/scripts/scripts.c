/*
 *  Description: ddns-scripts management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"



boole_t _setup( obj_t this, param_t param )
{
	talk_t cfg;
	talk_t axp;
	talk_t unit;
	boole need_online;
	boole need_onextern;
	const char *st;
	const char *ext;
	const char *name;
	char path[PATH_MAX];
	char svc[NAME_MAX];

	need_online = false;
	need_onextern = false;
	snprintf( path, sizeof(path), "%s%c%s", COM_IDPATH, OBJECT_API_GAPC, "reset" );

	cfg = config_sget( COM_IDPATH, NULL );
	axp = NULL;
	while ( NULL != ( axp = json_next( cfg, axp ) ) )
	{
		name = axp_name( axp );
		if ( name == NULL || strncmp( name, "client", 6 ) != 0 )
		{
			continue;
		}
		unit = axp_json( axp );
		st = json_string( unit, "status" );
		if ( st == NULL || 0 != strcmp( st, "enable" ) )
		{
			continue;
		}
		/* empty -> default, same as agent@heclient */
		ext = json_string( unit, "extern" );
		if ( ext == NULL || *ext == '\0' )
		{
			ext = "default";
		}
		if ( 0 == strcmp( ext, "disable" ) )
		{
			continue;
		}
		if ( 0 == strcmp( ext, "default" ) )
		{
			need_online = true;
		}
		else
		{
			need_onextern = true;
		}
	}
	if ( need_online == true )
	{
		joint_register( "network/online", path );
	}
	else
	{
		joint_unregister( "network/online", path );
	}
	if ( need_onextern == true )
	{
		joint_register( "network/onextern", path );
	}
	else
	{
		joint_unregister( "network/onextern", path );
	}

	/* joint first, then start — avoid race with network events */
	axp = NULL;
	while ( NULL != ( axp = json_next( cfg, axp ) ) )
	{
		name = axp_name( axp );
		if ( name == NULL || strncmp( name, "client", 6 ) != 0 )
		{
			continue;
		}
		unit = axp_json( axp );
		st = json_string( unit, "status" );
		if ( st == NULL || 0 != strcmp( st, "enable" ) )
		{
			continue;
		}
		snprintf( svc, sizeof(svc), "%s-%s", COM_IDPATH, name );
		sruns( 0, svc, COM_IDPATH, "service", name );
	}
	talk_free( cfg );
	return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	const char *ptr;
	char path[PATH_MAX];

	snprintf( path, sizeof(path), "%s%c%s", COM_IDPATH, OBJECT_API_GAPC, "reset" );
	joint_unregister( "network/online", path );
	joint_unregister( "network/onextern", path );

	ptr = "client";
	sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path );
	ptr = "client2";
	sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path );
	ptr = "client3";
	sdelete( COM_IDPATH"-%s", ptr );
	var2path( path, sizeof(path), "%s", ptr );
	unlink( path );
	return ttrue;
}
talk_t _reset( obj_t this, param_t param )
{
	talk_t v;
	talk_t cfg;
	talk_t axp;
	talk_t unit;
	const char *st;
	const char *ext;
	const char *name;
	const char *event;
	const char *ifname;
	char svc[NAME_MAX];

	event = param_string( param, 1 );
	if ( event == NULL )
	{
		return tfalse;
	}
	v = param_talk( param, 2 );
	if ( v == NULL )
	{
		return tfalse;
	}
	ifname = json_string( v, "ifname" );
	if ( ifname == NULL || *ifname == '\0' )
	{
		return tfalse;
	}

	cfg = config_sget( COM_IDPATH, NULL );
	axp = NULL;
	while ( NULL != ( axp = json_next( cfg, axp ) ) )
	{
		name = axp_name( axp );
		if ( name == NULL || strncmp( name, "client", 6 ) != 0 )
		{
			continue;
		}
		unit = axp_json( axp );
		st = json_string( unit, "status" );
		if ( st == NULL || 0 != strcmp( st, "enable" ) )
		{
			continue;
		}
		ext = json_string( unit, "extern" );
		if ( ext == NULL || *ext == '\0' )
		{
			ext = "default";
		}
		if ( 0 == strcmp( ext, "disable" ) )
		{
			continue;
		}
		if ( 0 == strcmp( ext, "default" ) )
		{
			if ( 0 != strcmp( event, "network/online" ) )
			{
				continue;
			}
		}
		else if ( 0 != strcmp( ext, ifname ) )
		{
			continue;
		}
		snprintf( svc, sizeof(svc), "%s-%s", COM_IDPATH, name );
		sreset( NULL, NULL, NULL, svc );
	}
	talk_free( cfg );
	return ttrue;
}
talk_t _service( obj_t this, param_t param )
{
	talk_t cfg;
	talk_t gwinfo;
	const char *id;
	const char *ptr;
	const char *ifname;
	const char *isp;
	const char *domain;
	const char *username;
	const char *password;
	char scripts[PATH_MAX];

	id = param_string( param, 1 );
	cfg = config_sgets( COM_IDPATH, id );
	/*
	 * Extern for interface IP readiness; joint is registered in _setup.
	 * No status/ip: ttrue so supervisor clears START; wait network joint reset.
	 */
	ifname = json_string( cfg, "extern" );
	if ( ifname == NULL || *ifname == '\0' )
	{
		ifname = "default";
	}
	if ( 0 != strcmp( ifname, "disable" ) )
	{
		gwinfo = NULL;
		if ( 0 == strcmp( ifname, "default" ) )
		{
			gwinfo = scall( NETWORK_COM, "gateway", NULL );
		}
		else
		{
			gwinfo = scall( ifname, "status", NULL );
		}
		if ( gwinfo == NULL )
		{
			default_warn( "%s extern %s no status", COM_IDPATH, ifname );
			talk_free( cfg );
			return ttrue;
		}
		ptr = json_string( gwinfo, "ip" );
		if ( ptr == NULL || *ptr == '\0' )
		{
			default_warn( "%s extern %s no ip address", COM_IDPATH, ifname );
			talk_free( cfg );
			talk_free( gwinfo );
			return ttrue;
		}
		talk_free( gwinfo );
	}

	isp = json_string( cfg, "isp" );
	domain = json_string( cfg, "domain" );
	username = json_string( cfg, "username" );
	password = json_string( cfg, "password" );
	if ( isp == NULL || *isp == '\0' || domain == NULL || *domain == '\0'
		|| username == NULL || *username == '\0' || password == NULL || *password == '\0' )
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

	list = json_create( NULL );
	misc2path( dir, sizeof(dir), "shareddns/default" );
	pdir = opendir( dir );
	if ( pdir != NULL )
	{
		while ( NULL != ( pent = readdir( pdir ) ) )
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

	_shut( this, NULL );
	ret = config_sset( COM_IDPATH, v, path );
	_setup( this, NULL );
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	return config_sget( COM_IDPATH, path );
}
