/*
 * ============================================================================
 * Skinos — standalone executable template (copy → register in prj.json "exe")
 * ============================================================================
 *
 * Difference from component/
 *   Built as a separate binary.  The framework starts it with execl and passes
 *   context through execute_object(), execute_api(), execute_param(), execute_pipe().
 *
 * Build provides
 *   PROJECT_ID, EXE_ID, EXE_IDPATH  (and often COM_ID / COM_IDPATH if shared)
 *   Plus gPLATFORM, HARDWARE, gCUSTOM, …
 *
 * main() is provided by MAIN2COM(...) from land/skin/com.h:
 *   - eapi_table_t rows → method APIs (setup/shut/…)
 *   - getfn/setfn → HE get/set (same signatures as dl com _get/_set)
 *   MAIN2API(table) is a thin wrapper: MAIN2COM(table, config_get, config_set).
 *
 * Adding a new entry point
 *   - Implement _myapi(obj_t, param_t) (return boole_t / talk_t like the others).
 *   - Add one row to exe_api_table: { "myapi", (comapi_t)_myapi },
 *   - Do NOT put "get"/"set" in the table; wire them via MAIN2COM(..., _get, _set).
 *
 * HE examples (replace myproj / myexe with PROJECT_ID / EXE_ID)
 *   myproj@myexe.setup    → _setup()   (same dot-method form as components)
 *   myproj@myexe.shut     → _shut()
 *   myproj@myexe.service  → _service()
 *   myproj@myexe.online   → _online()  (often from joint; HE form if registered)
 *   myproj@myexe          → _get()     (whole config)
 *   myproj@myexe:key=val  → _set()
 *
 * In Linux shell:  he 'myproj@myexe.setup'  (see land/he.md for quoting rules).
 * ============================================================================
 */

#include "skin/skin.h"

/* -------------------------------------------------------------------------- */
/* API implementations — add or remove to match your registration             */
/* -------------------------------------------------------------------------- */

/**
 * Bring-up for this executable object (init / deploy).
 * HE: myproj@myexe.setup
 */
boole_t _setup( obj_t this, param_t param )
{
    const char *object;

    object = obj_name( this );
	app_info( "%s: _setup", object );
	printf( "%s: _setup\n", object );
	return ttrue;
}

/**
 * Shutdown path for this executable object.
 * HE: myproj@myexe.shut
 */
boole_t _shut( obj_t this, param_t param )
{
    const char *object;

    object = obj_name( this );
	app_info( "%s: _shut", object );
	printf( "%s: _shut\n", object );
	return ttrue;
}

/**
 * Long-running mode (supervised process). Replace pause() with your loop.
 * HE: myproj@myexe.service
 */
boole_t _service( obj_t this, param_t param )
{
    const char *object;

    object = obj_name( this );
	app_info( "%s: _service (pause — replace with real daemon loop)", object );
	pause();
	return tfalse;
}

/**
 * Joint-style callback (event name + payload in param). Finish quickly.
 * HE: myproj@myexe.online
 *      (often invoked from joint registration; same HE shape if exposed.)
 */
boole_t _online( obj_t this, param_t param )
{
	talk_t ms;
	char *payload_txt;
	const char *event;
    const char *object;

    object = obj_name( this );
	event = param_string( param, 1 );
	ms = param_talk( param, 2 );
	payload_txt = json2string( ms );
	app_info( "%s: _online event=%s payload=%s", object,
			event != NULL ? event : "(null)",
			payload_txt != NULL ? payload_txt : "(null)" );
	if ( payload_txt != NULL )
	{
		free( payload_txt );
	}
	return ttrue;
}

/**
 * Return configuration (whole object or subtree when path is set).
 * HE: myproj@myexe  or  myproj@myexe:attr/path
 * Same signature as dl com (COM_GET_API).
 */
talk_t _get( obj_t this, attr_t path )
{
	talk_t cfg;
	const char *object;

	object = obj_name( this );
	cfg = config_get( this, path );
	app_info( "%s: _get", object );
	return cfg;
}

/**
 * Persist configuration; on success this template restarts via _shut + _setup.
 * HE: myproj@myexe=...  or  myproj@myexe:attr=value
 * Same signature as dl com (COM_SET_API).
 */
boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;
	const char *object;

	object = obj_name( this );
	ret = config_set( this, v, path );
	if ( ret == true )
	{
		app_info( "%s: _set saved, restarting (_shut → _setup)", object );
		_shut( this, NULL );
		_setup( this, NULL );
	}
	return ret;
}

/* -------------------------------------------------------------------------- */
/* API name → handler. Methods only; get/set are wired by MAIN2COM.           */
/* -------------------------------------------------------------------------- */

static const eapi_table_t exe_api_table[] = {
	{ "setup",   (comapi_t)_setup },
	{ "shut",    (comapi_t)_shut },
	{ "online",  (comapi_t)_online },
	{ "service", (comapi_t)_service },
};

MAIN2COM( exe_api_table, _get, _set );
