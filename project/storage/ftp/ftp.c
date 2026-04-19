/*
 *  Description: FTP server management (storage@ftp) — ProFTPD integration.
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 *
 *  Overview
 *  --------
 *  - _setup / _shut: lifecycle hooks to register or unregister the child
 *    "service" process and to honour configuration (status, platform).
 *  - _set / _get: persist and read JSON configuration via the skin config layer.
 *  - _service: runs in the child after cstart(); builds proftpd.conf from the
 *    merged JSON then exec's /usr/sbin/proftpd (this process becomes proftpd).
 */

#include "skin/skin.h"


/*
 * _setup -- apply configuration at boot or after _set.
 *
 * Steps:
 *  1) Slave platforms skip FTP entirely (no binary check, no service start).
 *  2) Ensure /usr/sbin/proftpd exists; otherwise setup fails (tfalse).
 *  3) If JSON "status" is "enable", register and start the "service" API
 *     (fork/exec path ends in _service with execlp proftpd).
 *
 * Returns ttrue when FTP is intentionally disabled (slave) or when proftpd
 * exists and setup completed (service may or may not have been started).
 * Returns tfalse only when the proftpd binary is missing on non-slave builds.
 */
boole_t _setup( obj_t this, param_t param )
{
    struct stat st;
    const char *ptr;
	const char *platform;

	/* Read hardware role; slave nodes do not run the FTP server. */
	platform = reg_string( NULL, "platform" );
	if ( platform != NULL && 0 == strcmp( platform , "slave" ) )
	{
		default_debug( "no ftp function on %s", platform );
		return ttrue;
	}
    /* Binary must be present before we advertise or start the service. */
    if ( stat( "/usr/sbin/proftpd", &st ) != 0 )
    {
		default_debug( "/usr/sbin/proftpd not found" );
        return tfalse;
    }
    /* Start foreground service child only when operator enabled FTP. */
    ptr = config_gets_string( NULL, 0, this, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
		cstart( this, "service", NULL, COM_IDPATH );
    }
    return ttrue;
}

/*
 * _shut -- tear down service registration for this component instance.
 *
 * sdelete(COM_IDPATH) removes the service entry so the supervisor stops
 * tracking the proftpd child; used before rewriting config in _set.
 */
boole_t _shut( obj_t this, param_t param )
{
    sdelete( COM_IDPATH );
    return ttrue;
}

/*
 * _set -- replace part or all of runtime configuration (HE / API set).
 *
 * Order matters: stop old service bookkeeping, write new JSON to storage,
 * then _setup so enable/disable and restarts follow the new file.
 */
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

	_shut( this, NULL );
    ret = config_set( this, v, path );
	_setup( this, NULL );
    return ret;
}

/* _get -- return current merged configuration (caller frees talk_t if needed). */
talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}


/*
 * _service -- child entry: generate ProFTPD config and exec the daemon.
 *
 * Flow:
 *  A) Resolve path to proftpd.conf under this component's var area.
 *  B) Server identity: machine name from land@machine, else "Farm".
 *  C) Bind all addresses (0.0.0.0), load full JSON cfg, set DefaultRoot from
 *     "root" or PROJECT_MNT_DIR.
 *  D) Emit static server directives (port 21, passwd auth, chroot, etc.).
 *  E) Branch on "mode":
 *     - "anonymous": anonymous stanza; WRITE allowed only if permission=="all";
 *       chmod a+rwx on anonymous root (legacy permissive behaviour).
 *     - else (user mode): RootLogin on; list land@auth "nas" users; for each
 *       share under cfg["user"], build READ/WRITE AllowUser lists from share
 *       "permission" and optional per-username overrides in share["user"].
 *  F) Free JSON handles, then execlp proftpd -n -q -c <cfg>. On success this
 *     process never returns; on failure default_faulting logs and may exit.
 *
 * Declared return tfalse satisfies the API signature after a failed exec path.
 */
boole_t _service( obj_t this, param_t param )
{
    talk_t cfg;
	talk_t v;
	talk_t axp;
	talk_t uaxp;
	talk_t share;
	talk_t userlist;
	talk_t permlist;
    const char *ptr;
	const char *mode;
    talk_t anonymous;
    const char *path;
	const char *permission;
	const char *user;
	char cfgpath[LINE_MAX];
	char readlist[LINE_MAX];
	char writelist[LINE_MAX];

	/* Writable path for the generated config file. */
	var2path( cfgpath, sizeof(cfgpath), "proftpd.conf" );
	/* ProFTPD ServerName directive from gateway hostname when available. */
    ptr = sgets_string( NULL, 0, MACHINE_COM, "name" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        string2file( cfgpath, "ServerName %s\n", ptr );
    }
    else
    {
        string2file( cfgpath, "ServerName Farm\n" );
    }
    string3file( cfgpath, "DefaultAddress %s\n", "0.0.0.0" );
    cfg = config_get( this, NULL );
    /* Chroot jail root for authenticated users (default mount). */
    path = json_string( cfg, "root" );
	if ( path == NULL )
	{
		path = PROJECT_MNT_DIR;
	}
    /* Core daemon options: standalone, system passwd, FTP port, umask, chroot. */
    string3file( cfgpath, \
        "ServerType  standalone\n"\
        "DefaultServer on\n"\
        "Port 21\n"\
        "Umask 022\n"\
        "MaxInstances 100\n"\
        "UseReverseDNS off\n"\
        "IdentLookups off\n"\
        "DefaultRoot %s\n"\
        "RequireValidShell off\n"
        "DelayEngine on\n", path );

	/* Anonymous FTP vs normal (passwd) login. */
	mode = json_string( cfg, "mode" );
	if ( mode != NULL && 0 == strcmp( mode, "anonymous" ) )
	{
		anonymous = json_value( cfg, "anonymous" );
		path = json_string( anonymous, "path" );
		if ( path == NULL )
		{
			path = PROJECT_MNT_DIR;
		}
		permission = json_string( anonymous, "permission" );
        /* Map anonymous to nobody; restrict or allow WRITE under anonymous tree. */
        string3file( cfgpath, \
            "RootLogin off\n"\
            "<Anonymous %s>\n"\
            "   User nobody\n"\
            "   Group nogroup\n"\
            "   UserAlias anonymous nobody\n"\
            "   <Directory %s>\n"\
            "       <Limit WRITE>\n", path, path );

        if ( NULL != permission && 0 == strcmp( permission, "all" ) )
        {
            string3file( cfgpath, \
            "           AllowAll\n" );
        }
        else
        {
            string3file( cfgpath, \
            "           DenyAll\n" );
        }
            string3file( cfgpath, \
            "       </Limit>\n"\
            "   </Directory>\n"\
            "</Anonymous>\n" );
		/* Ensure anonymous upload tree is world-accessible (device policy). */
		shell( "chmod a+rwx %s", path );
	}
    else
    {
        /* Local system accounts may log in; ACLs narrow paths per <Directory>. */
        string3file( cfgpath, "RootLogin on\n" );
		/* All NAS-group accounts from auth; each share may grant read/write.
		 * Expect a valid JSON iterator; NULL would make the inner json_next unsafe. */
		userlist = scalls( AUTH_COM, "list", "nas" );
		share = json_json( cfg, "user");
		axp = NULL;
		while( NULL != ( axp = json_next( share, axp ) ) )
		{
			v = axp_json( axp );
			path = json_string( v, "path");
			if ( path == NULL )
			{
				path = PROJECT_MNT_DIR;
			}
			permission = json_string( v, "permission" );
			permlist = json_json( v, "user" );
			memset( readlist,0 ,sizeof(readlist) );
			memset( writelist,0 ,sizeof(writelist) );
			uaxp = NULL;
			while( NULL != ( uaxp = json_next( userlist, uaxp ) ) )
	        {
	            user = axp_name( uaxp );
	            /* Per-user override in share["user"][username]; else share default. */
	            ptr = json_string( permlist, user );
	            if ( ptr == NULL || *ptr == '\0' )
	            {
	            	ptr = permission;
	            }
	            if ( ptr != NULL && 0 == strcmp( ptr, "all" ) )
	            {
	                strcat( writelist, user );
	                strcat( writelist, " " );
	                strcat( readlist, user );
	                strcat( readlist, " " );
	            }
	            else if ( ptr != NULL && 0 == strcmp( ptr, "read" ) )
	            {
	                strcat( readlist, user);
	                strcat( readlist, " ");
	            }
	        }
			/* ProFTPD: default deny; AllowUser whitelists for READ and WRITE. */
			string3file( cfgpath, "<Directory %s>\n" , path );
			string3file( cfgpath, "	 AllowOverwrite on\n" );
			
			string3file( cfgpath, "	 <Limit READ>\n" );
			if ( strlen(readlist) > 0 )
			{
				string3file( cfgpath, "		 AllowUser %s\n", readlist);
			}
			string3file( cfgpath, "		 DenyAll\n" );
			string3file( cfgpath, "	 </Limit>\n" );
			
			string3file( cfgpath, "	 <Limit WRITE>\n" );
			if ( strlen(writelist) > 0)
			{
				string3file( cfgpath, "		 AllowUser %s\n" , writelist);
			}
			string3file( cfgpath, "		 DenyAll\n" );
			string3file( cfgpath, "	 </Limit>\n" );
			
			string3file( cfgpath, "</Directory>\n" );
			
		}
		talk_free( userlist );
    }
	talk_free( cfg );

    /* Replace this process with proftpd; no return on success. */
    execlp( "/usr/sbin/proftpd", "proftpd", "-n", "-q", "-c", cfgpath, (char*)0 );
    default_faulting( "execlp the proftpd(%s) error" , "/usr/sbin/proftpd" );

    return tfalse;
}


