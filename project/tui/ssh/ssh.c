/*
 * SSH server component using Dropbear (skinos / tui@ssh).
 *
 * Manages dropbear: host keys under /etc/dropbear, optional iptables
 * access control (same pattern as telnet), and a supervised child process.
 * This is not OpenSSH sshd.
 *
 * Filter chain name: PROJECT_ID_COM_ID  ->  e.g. tui_ssh
 *
 * Author: dimmalex (dim), dimmalex@gmail.com
 * Company: ASHYELF
 */

#include "skin/skin.h"

/*
 * _setup -- Apply configuration and start or refresh Dropbear.
 *
 * Called at boot (prj.json init) and after successful _set().
 *
 * Order of work:
 *  1) Skip on OpenWrt "wrt" scope or "slave" platform (no SSH by design).
 *  2) Require dropbear in PATH; otherwise fail setup.
 *  3) Load config; missing config -> succeed without doing more.
 *  4) If status != "enable", free config and succeed without starting dropbear.
 *  5) Ensure /etc/dropbear; copy DSS/RSA host keys from project config if present;
 *     run dropbearkey.sh when shipped with the package to generate missing keys.
 *  6) If manager list is non-empty, same iptables pattern as telnet (chain tui_ssh).
 *  7) Start/restart supervised service (child runs dropbear).
 *
 * Returns ttrue on success (including no-op paths), tfalse if dropbear is missing.
 */
boole_t _setup( obj_t this, param_t param )
{
    talk_t v;
	talk_t axp;
    talk_t cfg;
	struct stat st;
    const char *ptr;
    const char *port;
	boole manager_init;
	char path[PATH_MAX];
	const char *scope;
	const char *platform;
	struct in_addr iptest;
    const char *test_file = "/tmp/.dropbear_exsit";

	/* --- Platform gate: SSH disabled on wrt scope or slave --- */
	scope = reg_string( NULL, "scope" );
	platform = reg_string( NULL, "platform" );
	if ( ( scope != NULL && 0 == strcmp( scope , "wrt" ) )
		|| ( platform != NULL && 0 == strcmp( platform , "slave" ) ) )
	{
		default_debug( "no ssh function on %s or %s", platform, scope );
		return ttrue;
	}

    /* --- Require dropbear binary in PATH --- */
    shell( "which dropbear > %s", test_file );
    ptr = file2string( test_file, NULL, 0 );
    if ( ptr == NULL || strlen( ptr ) < 8 )
    {
        unlink( test_file );
        return tfalse;
    }
    unlink( test_file );

    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return ttrue;
    }
    ptr = json_string( cfg, "status" );
    if ( ptr == NULL || 0 != strcmp( ptr, "enable" ) )
    {
        talk_free( cfg );
        return ttrue;
    }

	/* --- Host key material for Dropbear --- */
	shell( "mkdir -p /etc/dropbear" );
	if ( config_path( path, sizeof(path), PROJECT_ID, "dsskey"CONFIG_FILE_POSTFIX ) != NULL )
	{
		shell( "cp %s /etc/dropbear/dropbear_dss_host_key", path );
	}
	if ( config_path( path, sizeof(path), PROJECT_ID, "rsakey"CONFIG_FILE_POSTFIX ) != NULL )
	{
		shell( "cp %s /etc/dropbear/dropbear_rsa_host_key", path );
	}
	ptr = exe2path( NULL, 0, "dropbearkey.sh" );
	if ( ptr != NULL && stat( ptr, &st ) == 0 )
	{
		shell( ptr );
	}

    /* --- Listen port; empty -> SSH default 22 --- */
	axp = NULL;
	manager_init = false;
	port = json_string( cfg, "port" );
	if ( port == NULL || *port == '\0' )
	{
		port = "22";
	}
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
			if ( manager_init == false )
			{
				manager_init = true;
				iptables( "-t filter -N %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -F %s_%s", PROJECT_ID, COM_ID );
				iptables( "-t filter -D INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
				iptables( "-t filter -A INPUT -p tcp --dport %s -j %s_%s", port, PROJECT_ID, COM_ID );
			}
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
	if ( manager_init == true )
	{
		iptables( "-D %s_%s -j DROP", PROJECT_ID, COM_ID );
		iptables( "-A %s_%s -j DROP", PROJECT_ID, COM_ID );
	}

    cstart( this, "service", NULL, COM_IDPATH );

    talk_free( cfg );
    return ttrue;
}

/*
 * _shut -- Remove iptables chain and stop supervised Dropbear.
 * INPUT rule is deleted by jump target (see telnet.c) for correct _set() ordering.
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
 * _set -- Persist config, restart service, refresh firewall.
 * Blocked on wrt/slave; otherwise same sequence as telnet component.
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
		default_debug( "no ssh function on %s or %s", platform, scope );
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

talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}

/*
 * _service -- Child process: exec dropbear foreground on configured port.
 * -F: do not fork; -K 300: keepalive interval in seconds.
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
        port = "22";
    }
    default_debug( "dropbear -F -p %s", port );
    execlp( "dropbear", "dropbear", "-F", "-p", port, "-K", "300", (char*)0 );

    default_faulting( "exec the dropbear error" );
    talk_free( cfg );
    return tfalse;
}

