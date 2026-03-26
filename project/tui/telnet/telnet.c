/*
 * Telnet server component (skinos / tui@telnet).
 *
 * Manages BusyBox or system telnetd: persistent configuration, optional
 * per-source iptables access control, and a supervised child process.
 *
 * Filter chain name: PROJECT_ID_COM_ID  ->  e.g. tui_telnet
 *
 * Author: dimmalex (dim), dimmalex@gmail.com
 * Company: HP
 */

#include "skin/skin.h"

/*
 * _setup -- Apply configuration and start or refresh the telnet service.
 *
 * Called at boot (prj.json init) and after successful _set().
 *
 * Order of work:
 *  1) Skip entirely on OpenWrt scope or slave platform (no telnet by design).
 *  2) Require telnetd in PATH; otherwise fail setup.
 *  3) Load config; missing config -> succeed without doing more.
 *  4) If status != "enable", free config and succeed without starting telnetd.
 *  5) If manager list is non-empty, install iptables: INPUT -> custom chain,
 *     ACCEPT for each allowed IPv4 or MAC, final DROP in chain.
 *  6) Start/restart supervised service (child runs telnetd).
 *
 * Returns ttrue on success (including intentional no-op paths), tfalse if
 * telnetd binary is missing.
 */
boole_t _setup( obj_t this, param_t param )
{
    talk_t v;
	talk_t axp;
    talk_t cfg;
    const char *ptr;
    const char *port;
	boole manager_init;
	const char *scope;
	const char *platform;
	struct in_addr iptest;
    const char *test_file = "/tmp/.telnetd_exsit";

	/* --- Platform gate: telnet disabled on OpenWrt "wrt" scope or "slave" --- */
	scope = reg_string( NULL, "scope" );
	platform = reg_string( NULL, "platform" );
	if ( ( scope != NULL && 0 == strcmp( scope , "wrt" ) )
		|| ( platform != NULL && 0 == strcmp( platform , "slave" ) ) )
	{
		default_debug( "no telnet function on %s or %s", platform, scope );
		return ttrue;
	}

    /* --- Locate telnetd: write "which" output to temp file and read it back --- */
    shell( "which telnetd > %s", test_file );
    ptr = file2string( test_file, NULL, 0 );
    if ( ptr == NULL || strlen( ptr ) < 8 )
    {
        unlink( test_file );
        return tfalse;
    }
    unlink( test_file );

    /* --- Load full component configuration object --- */
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return ttrue;
    }
    /* --- Only run service and firewall hooks when explicitly enabled --- */
    ptr = json_string( cfg, "status" );
    if ( ptr == NULL || 0 != strcmp( ptr, "enable" ) )
    {
        talk_free( cfg );
        return ttrue;
    }
    /* --- Listen port for iptables and child; empty -> standard telnet 23 --- */
	axp = NULL;
	manager_init = false;
	port = json_string( cfg, "port" );
	if ( port == NULL || *port == '\0' )
	{
		port = "23";
	}
    /* --- manager: either JSON object (iterate keys) or legacy ";"-separated string --- */
    v = json_value( cfg, "manager" );
	if ( json_check( v ) == true )
	{
		while( NULL != ( axp = json_next( v, axp ) ) )
		{
			ptr = axp_string( axp );
			if ( ptr == NULL || *ptr == '\0' )
			{
				continue;
			}
			/* First allowed client: create chain, wire INPUT dport -> chain */
			if ( manager_init == false )
			{
				manager_init = true;
				iptables( "-t filter -N %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -D INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
				iptables( "-t filter -A INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
			}
			/* Valid IPv4 -> match source IP; else treat token as MAC for layer-2 match */
			if ( inet_pton( AF_INET, ptr, &iptest ) == 1 )
			{
				iptables( "-A %s_%s -s %s -j ACCEPT", PROJECT_ID, COM_ID, ptr );
			}
			else
			{
				iptables( "-A %s_%s -m mac --mac-source %s -j ACCEPT", PROJECT_ID, COM_ID, ptr );
			}
		}
	}
	else
	{
		ptr = json_string( cfg, "manager" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			char *tok;
			char *tokkey;
			char buffer[LINE_MAX];
		
			memset( buffer, 0, sizeof(buffer) );
			strncpy( buffer, ptr, sizeof(buffer)-1 );
			tokkey = tok = buffer;
			while( tokkey != NULL && *tok != '\0' )
			{
				tokkey = strstr( tok, ";" );
				if ( tokkey != NULL )
				{
					*tokkey = '\0';
				}

				if ( manager_init == false )
				{
					manager_init = true;
					iptables( "-t filter -N %s_%s", PROJECT_ID, COM_ID );
					iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
					iptables( "-t filter -D INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
					iptables( "-t filter -A INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
				}
				if ( inet_pton( AF_INET, tok, &iptest ) == 1 )
				{
					iptables( "-A %s_%s -s %s -j ACCEPT", PROJECT_ID, COM_ID, tok );
				}
				else
				{
					iptables( "-A %s_%s -m mac --mac-source %s -j ACCEPT", PROJECT_ID, COM_ID, tok );
				}

				if ( tokkey == NULL )
				{
					break;
				}
				tok = tokkey + 1;
			}
		}

	}
	/* --- After all ACCEPT rules, default deny inside the custom chain --- */
	if ( manager_init == true )
	{
		iptables( "-D %s_%s -j DROP", PROJECT_ID, COM_ID );
		iptables( "-A %s_%s -j DROP", PROJECT_ID, COM_ID );
	}

    /* --- Fork supervised child that execs telnetd (see _service) --- */
    cstart( this, "service", NULL, COM_IDPATH );

    talk_free( cfg );
    return ttrue;
}

/*
 * _shut -- Tear down iptables rules and stop the supervised service.
 *
 * Deletes the custom filter chain and INPUT jump by target name (not by port)
 * so _set() can still remove the old jump after config_set() wrote a new port.
 *
 * Then removes the service supervisor entry for this component.
 */
boole_t _shut( obj_t this, param_t param )
{
    iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -D INPUT -j  %s_%s", PROJECT_ID, COM_ID );
    iptables( "-t filter -X %s_%s", PROJECT_ID, COM_ID );
    sdelete( COM_IDPATH );
    return ttrue;
}

/*
 * _set -- Persist configuration changes and re-apply runtime state.
 *
 * Refused on wrt/slave (same as _setup gate). On success: write config,
 * shut down old instance, setup again, then refresh global firewall component.
 */
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
	const char *scope;
	const char *platform;

	scope = reg_string( NULL, "scope" );
	platform = reg_string( NULL, "platform" );
	if ( ( scope != NULL && 0 == strcmp( scope , "wrt" ) )
		|| ( platform != NULL && 0 == strcmp( platform , "slave" ) ) )
	{
		default_debug( "no telnet function on %s or %s", platform, scope );
		return false;
	}

    ret = config_set( this, v, path );
    if ( ret == true )
    {
        _shut( this, NULL );
        _setup( this, NULL );
		scalls( FIREWALL_COM, "setup", NULL );
    }
    return ret;
}

/* _get -- Return configuration (whole object or subtree by path). */
talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}

/*
 * _service -- Service entry point executed in the child after cstart().
 *
 * Replaces this process with telnetd in foreground (-F) on the configured
 * port (default 23). Never returns on success; on exec failure logs and
 * returns tfalse after freeing config.
 */
boole_t _service( obj_t this, param_t param )
{
    talk_t cfg;
    const char *port;

    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return terror;
    }
    port = json_string( cfg, "port" );
    if ( port == NULL || *port == '\0' )
    {
        port = "23";
    }

	default_debug( "telnetd -F -p %s", port );
	execlp( "telnetd", "telnetd", "-F", "-p", port, (char*)0 );

    default_faulting( "exec the telnetd error" );
    talk_free( cfg );
    return tfalse;
}

