/*
 * Description:  make project
 * 		Author:  dimmalex (dim), dimmalex@gmail.com
 * 	   Company:  ASHYELF
 */
#include "skin/skin.h"
#define TEMPLATE_PATH          PROJECT_DIR"/"PROJECT_ID
#define TEMPLATE_COM_SHELL     "component.ash"
#define TEMPLATE_HTML_FILE     "page.html"
#define TEMPLATE_HTML_FILE_KEY "tmptools@com"
#define TEMPLATE_LANGEN_FILE   "en.json"
#define TEMPLATE_LANGCN_FILE   "cn.json"



static boole project_create_json( const char *name, const char *apppath )
{
	talk_t v;
	size_t n;
	char intro[LINE_MAX+1];
	char prjpath[PATH_MAX+1];
	char prjinfo[PATH_MAX+1];

	if ( name == NULL )
	{
		fprintf( stderr, "error: argument lack\n" );
		return false;
	}
	/* create the app directory */
	if ( apppath == NULL )
	{
		apppath = PROJECT_APP_DIR;
	}
	if ( access( apppath, R_OK ) != 0 )
	{
		if ( mkdir( apppath, REGULAR_DIR_MODE ) != 0 )
		{
			fprintf( stderr, "error: cannot mkdir the %s\n", apppath );
			return false;
		}
	}
	/* create the app directory */
	snprintf( prjpath, sizeof(prjpath), "%s/%s", apppath, name );
	if ( access( prjpath, R_OK ) != 0 )
	{
		if ( mkdir( prjpath, REGULAR_DIR_MODE ) != 0 )
		{
			fprintf( stderr, "error: cannot mkdir the %s\n", prjpath );
			return false;
		}
	}
	/* create the project json */
	snprintf( prjinfo, sizeof(prjinfo), "%s/%s/%s", apppath, name, PROJECT_INFOFILE );
    /* get the intro */
    printf( "project %s introduction: ", name );
    fflush( stdout );
    fgets( intro, sizeof(intro), stdin );
	n = strlen( intro );
	if ( n > 0 && ( intro[n-1] == '\n' || intro[n-1] == '\r' ) )
	{
		intro[n-1] = '\0';
	}
	/* create the project info */
	v = file2json( prjinfo );
	if ( v == NULL )
	{
		v = json_create( NULL );
	}
	json_set_string( v, "name", name );
	json_set_string( v, "version", PROJECT_DEFAULT_VERSION );
	json_set_string( v, "author", "tmptools" );
	json_set_string( v, "intro", intro );
	// open file
	if ( json_save( v, prjinfo ) == false )
	{
		fprintf( stderr, "error: cannot fopen the %s\n", prjinfo );
		talk_free( v );
		return false;
	}

	project_dirty();
	talk_free( v );
	return true;
}

static boole project_create_wui( const char *name, const char *prjpath, const char *wuiname )
{
	size_t n;
	talk_t v;
	talk_t cfg;
	talk_t app;
	talk_t lang;
	char path[PATH_MAX];
	char page[PATH_MAX];
	char cnjson[PATH_MAX];
	char enjson[PATH_MAX];

    /* get the prj info */
	snprintf( path, sizeof(path), "%s/"PROJECT_INFOFILE, prjpath );
	cfg = file2json( path );
	if ( cfg == NULL )
	{
		fprintf( stderr, "error: %s json format error\n", path );
		return false;
	}
    /* create */
	snprintf( page, sizeof(page), "%s.html", wuiname );
	snprintf( cnjson, sizeof(cnjson), "%s-cn.json", wuiname );
	snprintf( enjson, sizeof(enjson), "%s-en.json", wuiname );
	v = json_json( cfg, "wui" );
	if ( v == NULL )
	{
		v = json_create( NULL );
		json_set_json( cfg, "wui", v );
	}
	/* add the wui (key is the page id, e.g. testcom — not the project name) */
	json_delete_axp( v, wuiname );
	app = json_create( NULL );
	json_set_json( v, wuiname, app );
	/* html */
	json_set_string( app, "page", page );
	/* lang */
	lang = json_create( NULL );
	json_set_json( app, "lang", lang );
	json_set_string( lang, "cn", cnjson );
	json_set_string( lang, "en", enjson );

	/* make the html file */
	snprintf( page, sizeof(page), "%s/%s.html", prjpath, wuiname );
	{
		FILE *fp;
		char buf[LINE_MAX];

		unlink( page );
		buf[LINE_MAX-1] = '\0';
		fp = fopen( TEMPLATE_PATH"/"TEMPLATE_HTML_FILE, "r" );
		if ( fp != NULL )
		{
			while ( NULL != fgets( buf, sizeof(buf)-1, fp ) )
			{
				if ( NULL != strstr( buf, TEMPLATE_HTML_FILE_KEY ) )
				{
					string3file( page, "\tvar comname = \"%s@%s\"\n", name, wuiname );
				}
				else
				{
					string3file( page, buf );
				}
			}
			fclose( fp );
		}
	}

	/* make the lang file */
	shell( "cp %s/%s %s/%s", TEMPLATE_PATH, TEMPLATE_LANGEN_FILE, prjpath, enjson );
	shell( "cp %s/%s %s/%s", TEMPLATE_PATH, TEMPLATE_LANGCN_FILE, prjpath, cnjson );

	/* get the chinese menu title */
	printf( "web menu title(Chinese): " );
	fflush( stdout );
	fgets( page, sizeof(page), stdin );
	n = strlen( page );
	if ( n > 0 && ( page[n-1] == '\n' || page[n-1] == '\r' ) )
	{
		page[n-1] = '\0';
	}
	json_set_string( app, "cn", page );
	/* get the english menu title */
	printf( "web menu name(English): " );
	fflush( stdout );
	fgets( page, sizeof(page), stdin );
	n = strlen( page );
	if ( n > 0 && ( page[n-1] == '\n' || page[n-1] == '\r' ) )
	{
		page[n-1] = '\0';
	}
	json_set_string( app, "en", page );

	/* write the PROJECT_INFOFILE */
	json_save( cfg, path );
	talk_print( app );

	talk_free( cfg );
	return true;
}



static const char *helpstr = \
"argument error\n"\
"help:\n"\
"      prj create      project_name                       create a project template\n"\
"      prj delete      project_name                       delete the project\n"\
"      prj add_com     project_name com_name              create a shell component template\n"\
"      prj add_init    project_name init_level   com_api  register a component api at system init call\n"\
"      prj add_uninit  project_name uninit_level com_api  register a component api at system shutdown call\n"\
"      prj add_joint   project_name joint_event  com_api  register a component api at joint cast\n"\
"      prj add_object  project_name object_name com_name  create a dynamic component depend on exist component\n"\
"      prj add_wui     project_name wui_name              create a Web UI (WUI) page template for project\n"\
"      prj check       project_name                       check project prj.json format\n"\
"      prj pack        project_name                       pack the project into fpk\n";

int main( int argc, const char **argv )
{
    int ret;
    struct stat st;
    const char *ptr;
    const char *type;
    const char *name;
	const char *hardware;
	char cwd[PATH_MAX+1];
    char path[PATH_MAX+1];
    char prjpath[PATH_MAX+1];

    if ( argc < 3 )
    {
        fprintf( stderr, "%s", helpstr );
        return -1;
    }
    ret = 0;
    type = argv[1];
    name = argv[2];
	if ( type == NULL || name == NULL )
	{
        fprintf( stderr, "%s", helpstr );
		return -1;
	}



	/* create the project */
    if ( 0 == strcmp( type, "create" ) )
    {
		if ( project_create_json( name, NULL ) == false )
		{
			return -1;
		}
		if ( project_path( path, sizeof(path), name ) == NULL )
		{
			return -1;
		}
		printf( "The project directory %s\n", path );
    }
    /* delete the project */
    else if ( 0 == strcmp( type, "delete" ) )
    {
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		shell( "rm -fr %s", prjpath );
	}
    /* check the project json format */
    else if ( 0 == strcmp( type, "check" ) )
    {
		if ( project_check( name, NULL ) == false )
		{
            return -1;
		}
	}
    /* pack the project to fpk */
    else if ( 0 == strcmp( type, "pack" ) )
    {
		if ( project_path( path, sizeof(path), name ) == NULL )
		{
			fprintf( stderr, "error: %s no exist\n", name );
			return -1;
		}
		/* refuse to pack trees that live under read-only / system project dir (see PROJECT_DIR in skinhead.h) */
		if ( strncmp( path, PROJECT_DIR, strlen(PROJECT_DIR) ) == 0 )
		{
			fprintf( stderr, "error: project %s path is under %s; pack expects a writable app tree (e.g. under %s)\n", name, PROJECT_DIR, PROJECT_APP_DIR );
			return -1;
		}
		hardware = reg_string( NULL, "hardware" );
		cwd[0] = '\0';
		getcwd( cwd, sizeof(cwd) );
		if ( chdir( path ) != 0 )
		{
			fprintf( stderr, "error: cannot enter to project %s directory \n", name );
			return -1;
		}
		snprintf( path, sizeof(path), "%s/%s-%s-%s.fpk", PROJECT_TMP_DIR, name, PROJECT_DEFAULT_VERSION, hardware );
		shell( "tar zcf %s *", path );
		if ( cwd[0] != '\0' )
		{
			chdir( cwd );
		}
		printf( "The packaging is located in this %s\n", path );
	}



    /* create the project com */
    else if ( 0 == strcmp( type, "add_com" ) )
    {
        ptr = argv[3];
        if ( ptr == NULL )
        {
            fprintf( stderr, "%s", helpstr );
            return -1;
        }
		/* create project */
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
        if ( stat( prjpath, &st ) != 0 )
        {
			if ( project_create_json( name, PROJECT_APP_DIR ) == false )
			{
				return -1;
			}
        }
		snprintf( path, sizeof(path), "%s/%s/%s.ash", PROJECT_APP_DIR, name, ptr );
		/* create shell component */
		shell( "cp %s/%s %s", TEMPLATE_PATH, TEMPLATE_COM_SHELL, path );
		snprintf( cwd, sizeof(cwd), "%s%c%s", name, PROJECT_OBJECT_GAPC, ptr );
		com_register( cwd, path );
		printf( "The component file %s\n", path );
    }
    /* create the project wui */
    else if ( 0 == strcmp( type, "add_wui" ) )
    {
        ptr = argv[3];
        if ( ptr == NULL )
        {
            fprintf( stderr, "%s", helpstr );
            return -1;
        }
		/* create project */
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
        if ( stat( prjpath, &st ) != 0 )
        {
			if ( project_create_json( name, PROJECT_APP_DIR ) == false )
			{
				return -1;
			}
        }
		/* create wui */
        project_create_wui( name, prjpath, ptr );
		printf( "The web page file %s/%s.html\n", prjpath, ptr );
    }



    /* create the project init */
    else if ( 0 == strcmp( type, "add_init" ) )
    {
		if ( argc < 5 || argv[3] == NULL || argv[4] == NULL )
		{
			fprintf( stderr, "%s", helpstr );
			return -1;
		}
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_init( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( INIT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project uninit */
    else if ( 0 == strcmp( type, "add_uninit" ) )
    {
		if ( argc < 5 || argv[3] == NULL || argv[4] == NULL )
		{
			fprintf( stderr, "%s", helpstr );
			return -1;
		}
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_uninit( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( UNINIT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project joint */
    else if ( 0 == strcmp( type, "add_joint" ) )
    {
		if ( argc < 5 || argv[3] == NULL || argv[4] == NULL )
		{
			fprintf( stderr, "%s", helpstr );
			return -1;
		}
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_joint( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		scalls( JOINT_COM, "register", "%s,%s", argv[3], argv[4] );
	}
    /* create the project object */
    else if ( 0 == strcmp( type, "add_object" ) )
    {
		if ( argc < 5 || argv[3] == NULL || argv[4] == NULL )
		{
			fprintf( stderr, "%s", helpstr );
			return -1;
		}
        snprintf( prjpath, sizeof(prjpath), PROJECT_APP_DIR"/%s", name );
		if ( project_add_object( name, prjpath, argv[3], argv[4] ) == false )
		{
            return -1;
		}
		com_register( argv[3], argv[4] );
	}



    else
    {
        fprintf( stderr, "%s", helpstr );
    }

    return ret;
}



