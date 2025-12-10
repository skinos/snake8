/*
 *  Description: ftp server management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"



talk_t _setup( obj_t this, param_t param )
{
    struct stat st;
    const char *ptr;

    /* proftpd check */
    if ( stat( "/usr/sbin/proftpd", &st ) != 0 )
    {
        return tfalse;
    }
    ptr = config_sgets_string( NULL, 0, COM_IDPATH, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
		sstart( COM_IDPATH, "service", NULL, COM_IDPATH );
    }
    return ttrue;
}
talk_t _shut( obj_t this, param_t param )
{
    sdelete( COM_IDPATH );
    return ttrue;
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

	var2path( cfgpath, sizeof(cfgpath), "proftpd.conf" );
	/* basic settings */
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
    cfg = config_sget( COM_IDPATH, NULL );
    path = json_string( cfg, "root" );
	if ( path == NULL )
	{
		path = PROJECT_MNT_DIR;
	}
    string3file( cfgpath, \
        "ServerType  standalone\n"\
        "DefaultServer on\n"\
        "AuthUserFile /etc/passwd\n"\
        "Port 21\n"\
        "Umask 022\n"\
        "MaxInstances 100\n"\
        "UseReverseDNS off\n"\
        "IdentLookups off\n"\
        "DefaultRoot %s\n"\
        "RequireValidShell off\n"
        "DelayEngine on\n", path );

	/* mode select settings */
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
		shell( "chmod a+rwx %s", path );
	}
    else
    {
        string3file( cfgpath, "RootLogin on\n" );
		userlist = scalls( AUTH_COM, "list", "nas" );
		share = json_value( cfg, "user");
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
			permlist = json_value( v, "user" );
			memset( readlist,0 ,sizeof(readlist) );
			memset( writelist,0 ,sizeof(writelist) );
			uaxp = NULL;
			while( NULL != ( uaxp = json_next( userlist, uaxp ) ) )
	        {
	            user = axp_id( uaxp );
	            /* if the user have perm set, frist use */
	            ptr = json_string( permlist, user );
	            if ( ptr == NULL || *ptr == '\0' )
	            {
	            	ptr = permission;
	            }
	            if ( 0 == strcmp( ptr, "all" ) )
	            {
	                strcat( writelist, user );
	                strcat( writelist, " " );
	                strcat( readlist, user );
	                strcat( readlist, " " );
	            }
	            else if ( 0 == strcmp( ptr, "read" ) )
	            {
	                strcat( readlist, user);
	                strcat( readlist, " ");
	            }
	        }
			// now configure to file
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

    execlp( "/usr/sbin/proftpd", "proftpd", "-n", "-q", "-c", cfgpath, (char*)0 );
    default_faulting( "execlp the proftpd(%s) error" , "/usr/sbin/proftpd" );

    return tfalse;
}



