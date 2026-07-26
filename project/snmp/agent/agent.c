/*
 *  Description: SNMP agent management (snmp@agent) — net-snmp snmpd.
 *       Author: dimmalex (dim), dimmalex@gmail.com
 *      Company: ASHYELF
 *
 *  Overview
 *  --------
 *  - _setup / _shut: lifecycle hooks; start supervised service when enabled.
 *  - _get / _set: persist JSON configuration via the skin config layer.
 *  - _service: child after cstart(); builds snmpd.conf then execlp snmpd.
 *
 *  Binary: OpenWrt package snmpd from swrt5 feeds (net-snmp), typically
 *  /usr/sbin/snmpd. DEPENDS:+snmpd in project Makefile.
 */

#include "skin/skin.h"

#define SNMPD_BIN          "/usr/sbin/snmpd"
#define SNMPD_CONFIG_FILE  PROJECT_CONF_DIR"/snmpd.conf"


/*
 * _setup -- apply configuration at boot or after _set.
 *
 * Skip on slave platforms. Require snmpd binary. When status is "enable",
 * start the supervised "service" child (ends in _service / execlp snmpd).
 */
boole_t _setup( obj_t this, param_t param )
{
	struct stat st;
	const char *ptr;
	const char *platform;

	platform = reg_string( NULL, "platform" );
	if ( platform != NULL && 0 == strcmp( platform, "slave" ) )
	{
		app_debug( "no snmp function on %s", platform );
		return ttrue;
	}
	if ( stat( SNMPD_BIN, &st ) != 0 )
	{
		app_debug( "%s not found", SNMPD_BIN );
		return tfalse;
	}
	ptr = config_gets_string( NULL, 0, this, "status" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		/* Avoid conflict with OpenWrt procd snmpd if present. */
		shell( "/etc/init.d/snmpd stop >/dev/null 2>&1" );
		cstart( this, "service", NULL, COM_IDPATH );
	}
	return ttrue;
}

/*
 * _shut -- stop supervised snmpd for this component instance.
 */
boole_t _shut( obj_t this, param_t param )
{
	sdelete( COM_IDPATH );
	return ttrue;
}

/*
 * _set -- persist config and restart service when changed.
 */
boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

	ret = config_set( this, v, path );
	if ( ret == true )
	{
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}

/* _get -- return current merged configuration. */
talk_t _get( obj_t this, attr_t path )
{
	return config_get( this, path );
}

/*
 * _service -- child entry: generate snmpd.conf and exec snmpd in foreground.
 *
 * Declared return tfalse satisfies the API after a failed exec path.
 * On successful execlp this process becomes snmpd and does not return.
 */
boole_t _service( obj_t this, param_t param )
{
	talk_t v;
	talk_t cfg;
	talk_t axp;
	char *unescape;
	const char *ptr;
	char listen[NAME_MAX];

	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return terror;
	}

	ptr = json_string( cfg, "port" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		snprintf( listen, sizeof(listen), "0.0.0.0:%s", ptr );
	}
	else
	{
		snprintf( listen, sizeof(listen), "0.0.0.0:161" );
	}
	string2file( SNMPD_CONFIG_FILE, "\n" );

	/* System identity (sysName / sysContact / sysLocation). */
	v = scalls( MACHINE_COM, "status", NULL );
	ptr = json_string( cfg, "name" );
	if ( ptr == NULL || *ptr == '\0' )
	{
		ptr = json_string( v, "name" );
	}
	if ( ptr == NULL || *ptr == '\0' )
	{
		ptr = "SkinOS";
	}
	string3file( SNMPD_CONFIG_FILE, "sysName %s\n", ptr );
	ptr = json_string( cfg, "contact" );
	if ( ptr == NULL || *ptr == '\0' )
	{
		ptr = "dimmalex@gmail.com";
	}
	string3file( SNMPD_CONFIG_FILE, "sysContact %s\n", ptr );
	ptr = json_string( cfg, "location" );
	if ( ptr == NULL || *ptr == '\0' )
	{
		ptr = "http://www.ashyelf.com";
	}
	string3file( SNMPD_CONFIG_FILE, "sysLocation %s\n", ptr );
	string3file( SNMPD_CONFIG_FILE, "\n" );
	talk_free( v );

	/* SNMPv1/v2c communities. */
	ptr = json_string( cfg, "rocommunity" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		string3file( SNMPD_CONFIG_FILE, "rocommunity %s default\n", ptr );
	}
	ptr = json_string( cfg, "rwcommunity" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		string3file( SNMPD_CONFIG_FILE, "rwcommunity %s default\n", ptr );
	}
	string3file( SNMPD_CONFIG_FILE, "\n" );

	/* SNMPv3 user. */
	ptr = json_string( cfg, "v3" );
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "disable" ) )
	{
		const char *key;
		const char *auth;
		const char *privacy;
		const char *password;
		const char *username;

		username = json_string( cfg, "username" );
		if ( username != NULL && *username != '\0' )
		{
			if ( 0 == strcmp( ptr, "noauth" ) )
			{
				string3file( SNMPD_CONFIG_FILE, "createUser %s\n", username );
				string3file( SNMPD_CONFIG_FILE, "rwuser %s noauth\n", username );
			}
			else if ( 0 == strcmp( ptr, "auth" ) )
			{
				auth = json_string( cfg, "auth" );
				password = json_string( cfg, "password" );
				string3file( SNMPD_CONFIG_FILE, "createUser %s %s %s\n", username, auth, password );
				string3file( SNMPD_CONFIG_FILE, "rwuser %s auth\n", username );
			}
			else if ( 0 == strcmp( ptr, "privacy" ) )
			{
				auth = json_string( cfg, "auth" );
				password = json_string( cfg, "password" );
				privacy = json_string( cfg, "privacy" );
				key = json_string( cfg, "key" );
				string3file( SNMPD_CONFIG_FILE, "createUser %s %s %s %s %s\n", username, auth, password, privacy, key );
				string3file( SNMPD_CONFIG_FILE, "rwuser %s priv\n", username );
			}
		}
	}
	string3file( SNMPD_CONFIG_FILE, "\n" );

	/* Custom OID shell extensions (net-snmp "exec"). */
	v = json_value( cfg, "custom" );
	if ( v != NULL )
	{
		talk_t x;
		const char *oid;
		const char *cmd;
		const char *id;

		axp = NULL;
		while ( NULL != ( axp = json_next( v, axp ) ) )
		{
			id = axp_name( axp );
			x = axp_value( axp );
			oid = json_string( x, "oid" );
			cmd = json_string( x, "cmd" );
			if ( id == NULL || oid == NULL || cmd == NULL
				|| *id == '\0' || *oid == '\0' || *cmd == '\0' )
			{
				continue;
			}
			string3file( SNMPD_CONFIG_FILE, "exec %s %s %s\n", oid, id, cmd );
		}
	}
	string3file( SNMPD_CONFIG_FILE, "\n" );

	/* Raw snmpd.conf lines, semicolon-separated. */
	ptr = json_string( cfg, "options" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		char *tok;
		char *tokkey;
		char buffer[LINE_MAX];

		unescape = json_unescape( ptr );
		memset( buffer, 0, sizeof(buffer) );
		strncpy( buffer, unescape, sizeof(buffer) - 1 );
		free( unescape );
		tokkey = tok = buffer;
		while ( tokkey != NULL && *tok != '\0' )
		{
			tokkey = strstr( tok, ";" );
			if ( tokkey != NULL )
			{
				*tokkey = '\0';
			}
			if ( *tok != '\0' )
			{
				string3file( SNMPD_CONFIG_FILE, "%s\n", tok );
			}
			if ( tokkey == NULL )
			{
				break;
			}
			tok = tokkey + 1;
		}
	}
	string3file( SNMPD_CONFIG_FILE, "\n" );

	talk_free( cfg );

	/* snmpd -a -f -Ls 0 -C -c <conf> 0.0.0.0:161 */
	shell( "mkdir -p /var/snmp" );
	setenv( "SNMP_PERSISTENT_FILE", SNMPD_CONFIG_FILE, 1 );
	execlp( SNMPD_BIN, "snmpd", "-a", "-f", "-Ls", "0", "-C", "-c", SNMPD_CONFIG_FILE, listen, (char*)0 );
	app_faulting( "execlp the snmpd(%s) error", SNMPD_BIN );
	return tfalse;
}
