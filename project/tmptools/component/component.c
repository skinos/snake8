/*
 * ============================================================================
 * Skinos — component template (copy directory → rename → register in prj.json)
 * ============================================================================
 *
 * What a component is
 *   One management unit: PROJECT_ID@COM_ID.  It can own JSON configuration,
 *   optional long-running service(s), and C callbacks invoked via HE / serv.
 *
 * Build provides (macros — do not #define yourself)
 *   PROJECT_ID   project name from prj.json
 *   COM_ID       this directory name
 *   COM_IDPATH   "PROJECT_ID@COM_ID"
 *   gPLATFORM, HARDWARE, gCUSTOM, gPLATFORM__*, gHARDWARE__*, gCUSTOM__*
 *
 * Typical copy workflow
 *   1) Copy this folder to <your_project>/<com_id>/
 *   2) Add "com": { "<com_id>": "description" } in prj.json
 *   3) Register APIs in skin/skinapi (or your project's registration layer)
 *   4) Keep only the callbacks you need; remove or stub the rest
 *
 * HE examples (replace myproj / mycom)
 *   myproj@mycom              → _get()
 *   myproj@mycom:attr=value   → _set()
 *   myproj@mycom.setup        → _setup()
 *   myproj@mycom.shut         → _shut()
 *
 * Useful headers (via skin/skin.h or add in mconfig)
 *   config.h   config_get / config_set
 *   com.h      cstart, creset, scall, …
 *   serv.h     service helpers
 *   register.h cross-process register values
 *   See land/skin/*.h and land documentation for full API lists.
 * ============================================================================
 */

#include "skin/skin.h"

/* -------------------------------------------------------------------------- */
/* Lifecycle                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * Called when the component is brought up (e.g. init registration in prj.json).
 * HE: project@component.setup
 */
boole_t _setup( obj_t this, param_t param )
{
	(void)param;
	app_info( "%s: _setup", COM_IDPATH );
	printf( "%s: _setup\n", COM_IDPATH );
	return ttrue;
}

/**
 * Called on shutdown path for this component.
 * HE: project@component.shut
 */
boole_t _shut( obj_t this, param_t param )
{
	(void)param;
	app_info( "%s: _shut", COM_IDPATH );
	printf( "%s: _shut\n", COM_IDPATH );
	return ttrue;
}

/* -------------------------------------------------------------------------- */
/* Configuration                                                              */
/* -------------------------------------------------------------------------- */

/**
 * Return configuration (whole object or subtree when path is set).
 * HE: project@component  or  project@component:attr/path
 */
talk_t _get( obj_t this, attr_t path )
{
	talk_t cfg;

	cfg = config_get( this, path );
	app_info( "%s: _get", COM_IDPATH );
	return cfg;
}

/**
 * Persist configuration; on success this template restarts via _shut + _setup.
 * HE: project@component=...  or  project@component:attr=value
 *
 * Customize: remove the restart pair if your component does not need it.
 */
boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

	ret = config_set( this, v, path );
	if ( ret == true )
	{
		app_info( "%s: _set saved, restarting (_shut → _setup)", COM_IDPATH );
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}

/* -------------------------------------------------------------------------- */
/* Optional: long-running service                                             */
/* -------------------------------------------------------------------------- */

/**
 * Often registered as a supervised service (survives until exit).
 * Replace pause() with your loop; return value affects serv policy.
 */
boole_t _service( obj_t this, param_t param )
{
	(void)this;
	(void)param;
	app_info( "%s: _service (blocked on pause() — replace with real work)", COM_IDPATH );
	pause();
	return tfalse;
}

/* -------------------------------------------------------------------------- */
/* Optional: joint / network event (example)                                  */
/* -------------------------------------------------------------------------- */

/**
 * Example joint handler.  param: (1) event name string, (2) payload talk_t.
 * Keep short — do not block the joint dispatcher.
 */
boole_t _online( obj_t this, param_t param )
{
	talk_t ms;
	char *payload_txt;
	const char *event;

	(void)this;
	event = param_string( param, 1 );
	ms = param_talk( param, 2 );

	payload_txt = json2string( ms );
	app_info( "%s: joint event=%s payload=%s", COM_IDPATH,
			event != NULL ? event : "(null)",
			payload_txt != NULL ? payload_txt : "(null)" );
	if ( payload_txt != NULL )
	{
		free( payload_txt );
	}
	return ttrue;
}
