/*
 *    Description:  testcom from component template
 *         Author:  tmptools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "skin/skin.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX       Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
COM_ID                String, component name, Name of the directory where this component resides
COM_IDPATH            String, Full name of a component in the system, PROJECT_ID@COM_ID
*/

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h				implementation of general linker list
log.h	    		log call implementation
talk.h				implementation of common communication data types
param.h 			implementation of parameter structure and related functions
path.h				implementation of structure and related functions for object path and attribute path
utility.h   		miscellaneous function implementation
register.h			global register variable implementation
config.h			implementation function to get/set/list the config
project.h			provide unified project information operation interface for the system
com.h				implementation communication to other component function use talk structure or parameter structure
he2com.h			invokes the function implementation
serv.h		    	service call implementation
skinapi.h			define all the general component api
*/

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* Configure example key */
#define CFG_KEY_PROPERTY  "name"
/* Register example name */
#define REG_KEY_NAME      "loop"
/* Test program name */
#define TESTEXE_NAME      "testexe"
/* Component version - update this to verify deployment */
#define COMPONENT_VERSION "v1.0.4 - 2025-01-07"

/* usually used to initialize or deploy the component, is also usually registered in the project information file to be called at startup
 * This function can be called by the user at the he terminal, project@component.setup to call this function */
boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *ptr;
	const char *object;

    /* Get the component name */
	object = obj_name( this );
    /* Get the component configure */
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
		app_warn( "%s: setup failed when no configure [version: %s]", object, COMPONENT_VERSION );
        return tfalse;
    }
    /* Get the status attribute value */
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
    {
		app_info( "%s: setup start the service [version: %s]", object, COMPONENT_VERSION );
        /* Start the service for this component */
        cstart( this, "service", NULL, object );
    }
	else
	{
		app_info( "%s: setup no service start when configure disable [version: %s]", object, COMPONENT_VERSION );
	}

    /* Free the component configure */
    talk_free( cfg );
    return ttrue;
}

/* Typically used to shut down or exit this component, this method is called when the system shuts down, which can be registered in the project information file
 * This function can be called by the user at the he terminal, project@component.shut to call this function */
boole_t _shut( obj_t this, param_t param )
{
	const char *object;
	
    /* Get the component name */
	object = obj_name( this );
    /* Log for debugging */
    app_warn( "%s: shut called", object );
    /* Stop the service */
    sdelete( object );
    /* Exit, remember to return immediately */
    return ttrue;
}

/* Usually it is started as a service process in other functions, so it will always run, and if it exits the system it will restart it */
boole_t _service( obj_t this, param_t param )
{
    int i;
    talk_t cfg;
	const char *ptr;
	const char *object;
	char path[PATH_MAX];

    /* Get the component name */
	object = obj_name( this );
    /* Log for debugging */
    app_info( "%s: service started [version: %s]", object, COMPONENT_VERSION );
    /* Get the component configure */
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        app_fault( "%s: service cannot load config [version: %s]", object, COMPONENT_VERSION );
        return terror;
    }
    /* Get the name attribute value */
    ptr = json_string( cfg, CFG_KEY_PROPERTY );

    /***********************************************/
    /* Loop 10 times first, print log every second */
    /***********************************************/
    for ( i = 0; i < 10; i++ )
    {
        app_info( "%s: this is the %d time print name value %s", object, i+1, ptr != NULL ? ptr : "" );
        sleep( 1 );
    }

	/* Get the testexe program pathname */
	ptr = exe2path( path, sizeof(path), TESTEXE_NAME );
	if ( ptr == NULL )
	{
		app_fault( "%s: cannot find the program %s", object, TESTEXE_NAME );
		talk_free( cfg );
		return terror;
	}

    /*********************************************/
    /* then execl the testexe that loop and log  */
    /*********************************************/
	execl( path, TESTEXE_NAME, NULL );

	/* Failed to log */
    app_fault( "%s: execl %s failed [version: %s]", object, path, COMPONENT_VERSION );
	talk_free( cfg );
    return tfalse;
}

/* Typically used for show the status */
talk_t _status( obj_t this, param_t param )
{
	int i;
    int pid;
    talk_t ret;
	const char *object;

    /* Get the component name */
	object = obj_name( this );

    /* Create the return JSON */
    ret = json_create( NULL );
    /* Add component version */
    json_set_string( ret, "version", COMPONENT_VERSION );

    /* Check if the service is running */
    pid = spid( object );
    if ( pid > 0 )
    {
		// set the service state
        json_set_string( ret, "status", "running" );
		// set the service process pid
        json_set_number( ret, "pid", pid );
		// get the register named "loop" that increase in testexe
		i = reg_int( this, REG_KEY_NAME );
		if ( i > 0 )
		{
			/* set the loop value */
        	json_set_number( ret, "loop", i );
		}
    }
    else
    {
        json_set_string( ret, "status", "stopped" );
    }

    return ret;
}



/* start the service */
boole_t _start( obj_t this, param_t param )
{
	const char *object;
	
    /* Get the component name */
	object = obj_name( this );
    /* Log version for debugging */
    app_info( "%s: start called [version: %s]", object, COMPONENT_VERSION );
	/* Start the service for this component */
	cstart( this, "service", NULL, object );
    return ttrue;
}
/* Modify the loop */
talk_t _modify( obj_t this, param_t param )
{
	int i;
	const char *ptr;
	const char *object;

	/* get the value from parameter */
	ptr = param_string( param, 1 );
	if ( ptr == NULL )
	{
		return tfalse;
	}
	i = atoi( ptr );

    /* Get the component name */
	object = obj_name( this );
    app_warn( "%s: modify the loop to %d", object, i );
	/* set the register */
	reg_set_int( this, REG_KEY_NAME, i );

	return ttrue;
}



/* Functions that are triggered when viewing a component configuration are usually used to obtain the actual configuration and then calibrate and then return
 * This function can be called by the user at the he terminal, project@component to call this function */
talk_t _get( obj_t this, attr_t path )
{
    return config_get( this, path );
}

/* When you set a component parameter, you will be triggered to call this function, usually filtered by this function and then stored in the actual configuration
 * This function can be called by the user at the he terminal, project@component= to call this function */
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
	const char *object;

    /* Directly save the set parameters into the flash */
    ret = config_set( this, v, path );
    /* If the flash is successfully saved, restart the corresponding service by calling first _shut closing and then calling the _setup */
    if ( ret == true )
    {
		/* Get the component name */
		object = obj_name( this );
        app_warn( "%s: save the configuration and reset it", object );
        _shut( this, NULL );
        _setup( this, NULL );
    }
    return ret;
}

/* Typically used for local network on process */
talk_t _on( obj_t this, param_t param )
{
	talk_t v;
    const char *event;
    const char *ifname;
	const char *object;

    /* Get the component name */
	object = obj_name( this );
    /* Get the event name */
    event = param_string( param, 1 );
	/* Get the event info */
	v = param_talk( param, 2 );
	/* Get the event ifname */
	ifname = ( v != NULL ) ? json_string( v, "ifname" ) : NULL;
	/* Log */
	app_warn( "%s: detected local %s event %s", object, ifname != NULL ? ifname : "", event != NULL ? event : "" );
	return ttrue;
}
/* Typically used for extern network on process */
talk_t _onextern( obj_t this, param_t param )
{
	talk_t v;
    const char *event;
    const char *ifname;
	const char *object;

    /* Get the component name */
	object = obj_name( this );
    /* Get the event name */
    event = param_string( param, 1 );
	/* Get the event info */
	v = param_talk( param, 2 );
	/* Get the event ifname */
	ifname = ( v != NULL ) ? json_string( v, "ifname" ) : NULL;
	/* Log */
	app_warn( "%s: detected extern %s event %s, restarting service", object, ifname != NULL ? ifname : "", event != NULL ? event : "" );
	return ttrue;
}
/* Typically used for internet network on process */
talk_t _online( obj_t this, param_t param )
{
	talk_t v;
    const char *event;
    const char *ifname;
	const char *object;

    /* Get the component name */
	object = obj_name( this );
    /* Get the event name */
    event = param_string( param, 1 );
	/* Get the event info */
	v = param_talk( param, 2 );
	/* Get the event ifname */
	ifname = ( v != NULL ) ? json_string( v, "ifname" ) : NULL;
	/* Log */
	app_warn( "%s: detected internet %s event %s, restarting service", object, ifname != NULL ? ifname : "", event != NULL ? event : "" );
	/* Reset the service */
	creset( this, "service", NULL, object );
	return ttrue;
}



