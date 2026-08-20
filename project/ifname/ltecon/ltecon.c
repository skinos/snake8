/*
 *  Description:  lte connection
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 *
 * === Default Reset Behavior ===
 *
 * Three independent checks can trigger a module reset (scall ifdev "reset").
 * Each check runs once per _service cycle with 1-second sleep between retries.
 * After a successful check, the reset counter for that stage is cleared.
 *
 * reset_times  | simcard   | signal/PLMN | attach    | connect_failed
 * --------------|-----------|-------------|-----------|---------------
 *  0 (1st)     |  60s      |  120s       |  60s      |  3 cycles
 *  1 (2nd)     | 180s      |  300s       | 180s      |  7 cycles
 *  2 (3rd)     | 300s      |  600s       | 300s      | 15 cycles
 *  3+          | 1800s     | 1800s       | 1800s     | 37 cycles
 *
 * Stage 1 — SIM card not detected:
 *   Default need_simcard is enabled. Each check sleeps 1s.
 *   1st reset after 60 failed checks (60s), 2nd after 180s, 3rd after 300s,
 *   subsequent after 1800s (30min). Returns terror → _service exits and
 *   restarts, incrementing reset_times.
 *
 * Stage 2 — Signal or PLMN not acquired:
 *   Default need_plmn and need_signal are enabled (both required).
 *   1st reset after 120s, 2nd after 300s, 3rd after 600s, then 1800s.
 *
 * Stage 3 — Network attach failed (non-PPP mode only):
 *   Default need_attach is enabled.
 *   1st reset after 60s, 2nd after 180s, 3rd after 300s, then 1800s.
 *
 * Stage 4 — Connect failed (consecutive _service cycle failures):
 *   A per-cycle counter (connect_failed) accumulates across restarts.
 *   Resets the module when the counter hits 3, 7, 15, or every 37 cycles.
 *   Each cycle includes stages 1-3, so one cycle ≈ 60+120+60 = 240s minimum.
 *   First connect-failure reset ≈ 3 * 240s = 12min.
 *
 * Worst case (all checks maxed out):
 *   A single _service cycle can take up to 60+120+60 = 240s before
 *   reaching the connect-failed counter. With failed_everytime=37,
 *   the longest interval between resets is 37 * 240s ≈ 2.5 hours.
 */

#include "skin/skin.h"
#include "skinnet/skinnet.h"
#include <ifaddrs.h>

boole_t _setup( obj_t this, param_t param )
{
	int tid;
    talk_t cfg;
    const char *ptr;
    const char *obj;
    const char *object;
	const char *ifdev;

    obj = obj_com( this );
    object = obj_name( this );

    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return ttrue;
	}
    ptr = json_string( cfg, "status" );
    if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
    {
        talk_free( cfg );
		return ttrue;
    }
	/* set the tid */
	ptr = json_string( cfg, "tid" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		tid = atoi( ptr );
		reg_set_int( this, "tid", tid );
	}
	/* set the keeplive */
	ptr = json_string( json_json( cfg, "keeplive"), "type" );
	reg_set_string( this, "keeplive", ptr );

    /* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
		ifname_warn( obj, "%s cannot find ifdev", object );
        talk_free( cfg );
        return tfalse;
    }
	/* need the ifdev exist */
	if ( com_have( ifdev, NULL ) == false )
	{
		ifname_warn( obj, "%s ifdev %s nonexistent", object, ifdev );
        talk_free( cfg );
        return tfalse;
	}

    /* run the app connection */
    ifname_info( obj, "%s setup", object );
	sstart( object, "service", NULL, object );
    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	talk_t cfg;
    const char *obj;
	const char *ifdev;
    const char *object;
    char path[PATH_MAX];

    obj = obj_com( this );
    object = obj_name( this );
    ifname_info( obj, "%s shut", object );

    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
    /* call the offline */
    scalls( NETWORK_COM, "offline", object );
    /* stop the automatic service */
    sdelete( "%s-automatic", object );
    /* stop the service */
    sdelete( object );

    /* delete online file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    unlink( path );
    /* delete upline file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
    unlink( path );

    /* down the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev != NULL && *ifdev != '\0' )
    {
    	talk_t profile;
    	const char *ptr;
    	profile = NULL;
		ptr = json_string( cfg, "profile" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			profile = json_json( cfg, "profile_cfg" );
		}
    	scallt( ifdev, "down", profile );
	}

	talk_free( cfg );
    return ttrue;
}
boole _set( obj_t this, talk_t v, attr_t path )
{
	int i;
    boole ret;
    boole dret;
    talk_t x;
    talk_t axp;
	talk_t cfg;
	talk_t dcfg;
	const char *ptr;
	const char *ifdev;

	ifdev = reg_string( this, "ifdev" );
	ret = dret = false;
    ptr = attr_layer( path, 1 );
    if ( ptr == NULL || *ptr == '\0' )
    {
		if ( v == NULL )
		{
			// delete all the configure
			ret = config_set( this, NULL, NULL );
		}
		else
		{
			// separately all the configure
			cfg = json_create( NULL );
			dcfg = json_create( NULL );
            axp = NULL;
            while ( NULL != ( axp = json_next( v, axp ) ) )
            {
				ptr = axp_name( axp );
				x = axp_value( axp );
				if ( 0 == strcmp( ptr, "sms" )
					|| 0 == strcmp( ptr, "gnss" )
					|| 0 == strcmp( ptr, "atport" )
					|| 0 == strcmp( ptr, "lock_nettype" )
					|| 0 == strcmp( ptr, "lock_imei" )
					|| 0 == strcmp( ptr, "lock_imsi" )

					|| 0 == strcmp( ptr, "custom_set" )
					|| 0 == strcmp( ptr, "custom_watch" )

					|| 0 == strcmp( ptr, "watch_interval" )
					)
				{
					json_set_value( dcfg, ptr, talk_dup(x) );
				}
                else
                {
                    json_set_value( cfg, ptr, talk_dup(x) );
                }
            }
			// set to modem config
			x = config_sget( ifdev, NULL );
			if ( talk_equal( x, dcfg ) == false )
			{
				dret = config_sset( ifdev, dcfg, NULL );
			}
			talk_free( x );
            talk_free( dcfg );
			// set the ifname config
			x = config_get( this, NULL );
			if ( talk_equal( x, cfg ) == false )
			{
	            ret = config_set( this, cfg, NULL );
			}
			talk_free( x );
            talk_free( cfg );
		}
    }
	else
	{
		if ( 0 == strcmp( ptr, "sms" )
			|| 0 == strcmp( ptr, "gnss" )
			|| 0 == strcmp( ptr, "atport" )
			|| 0 == strcmp( ptr, "lock_nettype" )
			|| 0 == strcmp( ptr, "lock_imei" )
			|| 0 == strcmp( ptr, "lock_imsi" )

			|| 0 == strcmp( ptr, "custom_set" )
			|| 0 == strcmp( ptr, "custom_watch" )

			|| 0 == strcmp( ptr, "watch_interval" )
			)
		{
			dret = config_sset( ifdev, v, path );
		}
        else
        {
            ret = config_set( this, v, path );
        }
	}

	// clear the reconnect count
	i = 0;
	reg_set_int( this, "connect_failed", i );
	// reload the ifdev
    if ( dret == true )
    {
		creset( NULL, NULL, NULL, ifdev );
		ret = true;
    }
	/* Must stay after config_set — do not move _shut earlier: network/offline
	 * may SIGHUP connect, which immediately re-setups this ifname from disk. */
	_shut( this, NULL );
	_setup( this, NULL );
    return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	talk_t ret;
	talk_t cfg;
	talk_t dcfg;
	const char *ifdev;

	// get the ifname configure
	cfg = config_get( this, NULL );
	// combination the downlayer configure
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev != NULL && *ifdev != '\0' )
    {
        dcfg = sget( ifdev, NULL );
        if ( cfg == NULL )
        {
            cfg = dcfg;
        }
        else if ( dcfg != NULL )
        {
			// delete the the repeated attr
			json_delete_axp( dcfg, "status" );
			json_delete_axp( dcfg, "pin" );
			json_delete_axp( dcfg, "profile" );
			json_delete_axp( dcfg, "profile_cfg" );
			// combination
            json_sync( dcfg, cfg );
            talk_free( dcfg );
        }
    }
	// pick the attr value
    ret = attr_cut( cfg, path );
    if ( ret != cfg )
    {
        talk_free( cfg );
    }
	return ret;
}    



talk_t _ifdev( obj_t this, param_t param )
{
	const char *ifdev;

	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }
    return string2x( ifdev );
}
talk_t _netdev( obj_t this, param_t param )
{
	const char *ifdev;
	const char *netdev;

    /* get the netdev */
	netdev = reg_string( this, "netdev" );
    if ( netdev != NULL && *netdev != '\0' )
    {
    	return string2x( netdev );
    }
    /* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }
    /* get the ifdev netdev */
	return scall( ifdev, "ifdev", NULL );
}
boole_t _service( obj_t this, param_t param )
{
    int i;
	int sig;
	int plmn;
	int check;
	talk_t v;
	talk_t ret;
    talk_t cfg;
	talk_t mcfg;
	talk_t profile;
	const char *ptr;
	const char *apn;
	const char *obj;
	const char *pin;
	const char *mode;
	const char *ifdev;
    const char *object;
	const char *netdev;
	const char *method;
	const char *reason;
	int reset_times;
	int connect_failed;
	int failed_timeout;
	int failed_threshold;
	int failed_threshold2;
	int failed_threshold3;
	int failed_everytime;
	char plmn_string[NAME_MAX];
	char signal_string[NAME_MAX];

	obj = obj_com( this );
    object = obj_name( this );
    /* offline first */
	scalls( NETWORK_COM, "offline", object );

	/*****************************************/
	/********** get the infomation ***********/
	/*****************************************/
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
		ifname_fault( obj, "%s cannot find ifdev", object );
		sleep( 5 );
        return tfalse;
    }
	if ( com_have( ifdev, NULL ) == false )
	{
		ifname_fault( obj, "%s ifdev %s does not exist", object, ifdev );
		sleep( 5 );
        return tfalse;
	}
	ret = scall( ifdev, "fun", NULL );
	if ( ret != ttrue )
	{
		ifname_warn( obj, "%s wait the ifdev %s fun", object, ifdev );
		sleep( 5 );
        return tfalse;
	}

    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
		ifname_fault( obj, "%s cannot find configuration", object );
    	return terror;
    }
    /* get the ifdev reset times */
	reset_times = reg_sint( ifdev, "reset_times" );

	/***********************************/
	/******** Backup SIM START *********/
	/***********************************/
	mcfg = cfg;
	ptr = json_string( cfg, "bsim" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		int bsim_times;
		talk_t bsim_cfg;
		const char *sim_set;
		const char *sim_state;
		char sim_buffer[NAME_MAX];
		
		bsim_cfg = json_json( cfg, "bsim_cfg" );
		sim_set = json_string( bsim_cfg, "mode" );
		sim_state = scall_string( sim_buffer, sizeof(sim_buffer), ifdev, "bsim_state", NULL );
		if ( sim_set != NULL && 0 == strcmp( sim_set, "main" ) )
		{
			if ( sim_state != NULL && 0 != strcmp( sim_state, "main" ) )
			{
				scall( ifdev, "bsim_main", NULL );
				talk_free( cfg );
				return ttrue;
			}
		}
		else if ( sim_set != NULL && 0 == strcmp( sim_set, "back" ) )
		{
			if ( sim_state == NULL || 0 != strcmp( sim_state, "back" ) )
			{
				scall( ifdev, "bsim_back", NULL );
				talk_free( cfg );
				return ttrue;
			}
			mcfg = bsim_cfg;
		}
		else
		{
			boole_t bsim_service( obj_t this, param_t param, talk_t cfg, const char *ifdev, const char *object, const char *obj, const char *sim_state, int bsim_times );
			bsim_times = reg_sint( ifdev, "bsim_times" );
			return bsim_service( this, param, cfg, ifdev, object, obj, sim_state, bsim_times );
		}
	}
	else
	{
		/* bsim disabled: clear switch counters and fall back to main if on backup */
		const char *sim_state;
		char sim_buffer[NAME_MAX];

		scall( ifdev, "bsim_clear", NULL );
		reg_set_string( this, "switch_reason", NULL );
		sim_state = scall_string( sim_buffer, sizeof(sim_buffer), ifdev, "bsim_state", NULL );
		if ( sim_state != NULL && 0 == strcmp( sim_state, "back" ) )
		{
			scall( ifdev, "bsim_main", NULL );
			talk_free( cfg );
			return ttrue;
		}
	}
	/***********************************/
	/******** Backup SIM END ***********/
	/***********************************/



	/*****************************************/
	/***** get the connect mode **************/
	/*****************************************/
	mode = json_string( cfg, "mode" );
	method = json_string( cfg, "method" );
	if ( method == NULL || *method == '\0' )
	{
		method = "disable";
	}
	/* 5g default dhcp and 4g default ppp */
	i = reg_sint( ifdev, "na" );
    if ( i > 0 )
    {
		if ( mode == NULL || *mode == '\0' )
		{
			mode = "dhcpc";
		}
    }
	else
	{
		if ( mode == NULL || *mode == '\0' )
		{
			mode = "ppp";
		}
	}
	/* no netdev to ppp */
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
		ifname_warn( obj, "%s modify the mode to ppp when cannot find netdev", object );
    	mode = "ppp";
		method = "disable";
		json_set_string( cfg, "mode", "ppp" );
    }
	/* ppp mode no ipv6 */
	if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
	{
		method = "disable";
	}
	/* set the mode */
	reg_set_string( this, "mode", mode );
	reg_set_string( this, "method", method );



	/*****************************************/
	/**** testing simcard for the ifdev ******/
	/*****************************************/
    ifname_info( obj, "%s simcard detection", object );
	failed_threshold = 60;       // 60
	failed_threshold2 = 180;     // 180
	failed_threshold3 = 300;     // 300
	failed_everytime = 1800;     // 1800
	ptr = json_string( cfg, "simcard_failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( cfg, "simcard_failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( cfg, "simcard_failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( cfg, "simcard_failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
simagain:
	ptr = json_string( cfg, "need_simcard" );
	if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
	{
		failed_timeout = 10;
		for( check=1; check<=failed_timeout; check++ )
		{
			ret = scall( ifdev, "sim", NULL );
			if ( ret == ttrue )
			{
				break;
			}
			else if ( ret == terror )
			{
				ifname_warn( obj, "%s ifdev %s not work", object, ifdev );
				talk_free( cfg );
				return terror;
			}
			else if ( ret > tpanic )
			{
				ptr = x2string( ret );
				if ( ptr != NULL && 0 == strcmp( ptr, "pin" ) )
				{
					pin = json_string( mcfg, "pin" );
					ret = scalls( ifdev, "pin", pin );
					if ( ret == ttrue )
					{
						talk_free( ret );
						goto simagain;
					}
					pause();
					talk_free( ret );
					talk_free( cfg );
					return terror;
				}
				else if ( ptr != NULL && 0 == strcmp( ptr, "puk" ) )
				{
					pause();
					talk_free( ret );
					talk_free( cfg );
					return terror;
				}
				talk_free( ret );
			}
			ifname_warn( obj, "%s simcard failed %d/%d", object, check, failed_timeout );
			sleep( 1 );
		}
		if ( check > failed_timeout )
		{
			ifname_info( obj, "%s ignore the simcard failed", object );
		}
	}
	else
	{
		if ( reset_times == 0 )
		{
			failed_timeout = failed_threshold;
		}
		else if ( reset_times == 1 )
		{
			failed_timeout = failed_threshold2;
		}
		else if ( reset_times == 2 )
		{
			failed_timeout = failed_threshold3;
		}
		else
		{
			failed_timeout = failed_everytime;
		}
		for( check=1; check<=failed_timeout; check++ )
		{
			ret = scall( ifdev, "sim", NULL );
			if ( ret == ttrue )
			{
				break;
			}
			else if ( ret == terror )
			{
				ifname_warn( obj, "%s ifdev %s not work", object, ifdev );
				talk_free( cfg );
				return terror;
			}
			else if ( ret > tpanic )
			{
				ptr = x2string( ret );
				if ( ptr != NULL && 0 == strcmp( ptr, "pin" ) )
				{
					pin = json_string( mcfg, "pin" );
					ret = scalls( ifdev, "pin", pin );
					if ( ret == ttrue )
					{
						talk_free( ret );
						goto simagain;
					}
					pause();
					talk_free( ret );
					talk_free( cfg );
					return terror;
				}
				else if ( ptr != NULL && 0 == strcmp( ptr, "puk" ) )
				{
					pause();
					talk_free( ret );
					talk_free( cfg );
					return terror;
				}
				talk_free( ret );
			}
			ifname_warn( obj, "%s simcard failed %d/%d", object, check, failed_timeout );
			sleep( 1 );
		}
		if ( check > failed_timeout )
		{
			reg_set_string( this, "reset_reason", "sim" );
			ifname_fault( obj, "%s reset the %s when simcard failed for %d times", object, ifdev, failed_timeout );
			scall( ifdev, "reset", NULL );
			talk_free( cfg );
			return terror;
		}
	}
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	reason = reg_string( this, "reset_reason" );
	if ( reason != NULL && 0 == strcmp( reason, "sim" ) )
	{
		reg_set_string( this, "reset_reason", NULL );
		scall( ifdev, "reset_clear", NULL );
		reset_times = 0;
	}

	/*****************************************/
	/**** get the custom profile for up ******/
	/*****************************************/
	profile = NULL;
	ptr = json_string( mcfg, "profile" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		profile = json_json( mcfg, "profile_cfg" );
	}

	/*****************************************/
	/**** set the custom profile for up ******/
	/*****************************************/
	if ( profile != NULL )
	{
		apn = json_string( profile, "apn" );
		ifname_info( obj, "%s set the profile APN(%s)", object, apn?:"" );
		ret = scallt( ifdev, "up", profile );
		/* tfalse: modem_off in progress; abort this round and wait fun on next _service */
		if ( ret != ttrue )
		{
			ifname_info( obj, "%s custom profile modem_off, retry later", object );
			talk_free( cfg );
			sleep( 5 );
			return tfalse;
		}
	}

	/*****************************************/
	/**** testing signal for the ifdev *******/
	/*****************************************/
	plmn_string[0] = signal_string[0] = '\0';
    ifname_info( obj, "%s plmn or signal detection", object );
	failed_threshold = 120;      // 120
	failed_threshold2 = 300;     // 300
	failed_threshold3 = 600;     // 600
	failed_everytime = 1800;     // 1800
	ptr = json_string( cfg, "signal_failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( cfg, "signal_failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( cfg, "signal_failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( cfg, "signal_failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	i = 0b11;
	ptr = json_string( cfg, "need_plmn" );
	if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
	{
		i &= ~0b01;
	}
	ptr = json_string( cfg, "need_signal" );
	if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
	{
		i &= ~0b10;
	}
	if ( i == 0 )
	{
		failed_timeout = 10;
		for( check=1; check<=failed_timeout; check++ )
		{
			ret = scall( ifdev, "plmn", NULL );
			if ( ret == ttrue )
			{
				break;
			}
			else if ( ret == terror )
			{
				ifname_warn( obj, "%s ifdev %s not work when plmn", object, ifdev );
				talk_free( cfg );
				return terror;
			}
			else if ( ret > tpanic )
			{
				ptr = x2string( ret );
				if ( ptr != NULL )
				{
					strncpy( plmn_string, ptr, sizeof(plmn_string) - 1 );
					plmn_string[sizeof(plmn_string) - 1] = '\0';
					plmn = atoi( ptr );
				}
				else
				{
					plmn = 0;
				}
				talk_free( ret );
				if ( plmn > 0 )
				{
					break;
				}
			}
			ifname_info( obj, "%s plmn failed", object );
			sleep( 1 );
		}
		if ( check > failed_timeout )
		{
			ifname_info( obj, "%s ignore the plmn failed", object );
		}
	}
	else
	{
		if ( reset_times == 0 )
		{
			failed_timeout = failed_threshold;
		}
		else if ( reset_times == 1 )
		{
			failed_timeout = failed_threshold2;
		}
		else if ( reset_times == 2 )
		{
			failed_timeout = failed_threshold3;
		}
		else
		{
			failed_timeout = failed_everytime;
		}
		for( check=1; check<=failed_timeout; check++ )
		{
			if ( i == 0b01 )
			{
				ret = scall( ifdev, "plmn", NULL );
				if ( ret == ttrue )
				{
					break;
				}
				else if ( ret == terror )
				{
					ifname_warn( obj, "%s ifdev %s not work when plmn", object, ifdev );
					talk_free( cfg );
					return terror;
				}
				else if ( ret > tpanic )
				{
					ptr = x2string( ret );
					if ( ptr != NULL )
					{
						strncpy( plmn_string, ptr, sizeof(plmn_string) - 1 );
						plmn_string[sizeof(plmn_string) - 1] = '\0';
						plmn = atoi( ptr );
					}
					else
					{
						plmn = 0;
					}
					talk_free( ret );
					if ( plmn > 0 )
					{
						break;
					}
				}
				ifname_info( obj, "%s plmn failed %d/%d", object, check, failed_timeout );
			}
			else if ( i == 0b10 )
			{
				ret = scall( ifdev, "signal", NULL );
				if ( ret == ttrue )
				{
					break;
				}
				else if ( ret == terror )
				{
					ifname_warn( obj, "%s ifdev %s not work when signal", object, ifdev );
					talk_free( cfg );
					return terror;
				}
				else if ( ret > tpanic )
				{
					ptr = x2string( ret );
					if ( ptr != NULL )
					{
						strncpy( signal_string, ptr, sizeof(signal_string) - 1 );
						signal_string[sizeof(signal_string) - 1] = '\0';
						sig = atoi( ptr );
					}
					else
					{
						sig = 0;
					}
					talk_free( ret );
					if ( sig > 0 )
					{
						break;
					}
				}
				ifname_info( obj, "%s signal failed %d/%d", object, check, failed_timeout );
			}
			else
			{
				v = scall( ifdev, "plmn", NULL );
				ret = scall( ifdev, "signal", NULL );
				if ( v == ttrue && ret == ttrue )
				{
					break;
				}
				if ( v > tpanic && ret > tpanic )
				{
					plmn = 0;
					sig = 0;
					ptr = x2string( v );
					if ( ptr != NULL )
					{
						strncpy( plmn_string, ptr, sizeof(plmn_string) - 1 );
						plmn_string[sizeof(plmn_string) - 1] = '\0';
						plmn = atoi( ptr );
					}
					talk_free( v );
					ptr = x2string( ret );
					if ( ptr != NULL )
					{
						strncpy( signal_string, ptr, sizeof(signal_string) - 1 );
						signal_string[sizeof(signal_string) - 1] = '\0';
						sig = atoi( ptr );
					}
					talk_free( ret );
					if ( plmn > 0 && sig > 0 )
					{
						break;
					}
				}
				else
				{
					if ( v == terror )
					{
						ifname_warn( obj, "%s ifdev %s not work when plmn", object, ifdev );
						talk_free( cfg );
						return terror;
					}
					else if ( v > tpanic )
					{
						ptr = x2string( v );
						if ( ptr != NULL )
						{
							strncpy( plmn_string, ptr, sizeof(plmn_string) - 1 );
							plmn_string[sizeof(plmn_string) - 1] = '\0';
						}
						talk_free( v );
					}
					else
					{
						ifname_info( obj, "%s plmn failed %d/%d", object, check, failed_timeout );
					}
					if ( ret == terror )
					{
						ifname_warn( obj, "%s ifdev %s not work when signal", object, ifdev );
						talk_free( cfg );
						return terror;
					}
					else if ( ret > tpanic )
					{
						ptr = x2string( ret );
						if ( ptr != NULL )
						{
							strncpy( signal_string, ptr, sizeof(signal_string) - 1 );
							signal_string[sizeof(signal_string) - 1] = '\0';
						}
						talk_free( ret );
					}
					else
					{
						ifname_info( obj, "%s signal failed %d/%d", object, check, failed_timeout );
					}
				}
			}
			sleep( 1 );
		}
		if ( check > failed_timeout )
		{
			reg_set_string( this, "reset_reason", "signal" );
			ifname_fault( obj, "%s reset the %s when signal or plmn failed for %d times", object, ifdev, failed_timeout );
			scall( ifdev, "reset", NULL );
			talk_free( cfg );
			return terror;
		}
	}
	reason = reg_string( this, "reset_reason" );
	if ( reason != NULL && 0 == strcmp( reason, "signal" ) )
	{
		reg_set_string( this, "reset_reason", NULL );
		scall( ifdev, "reset_clear", NULL );
		reset_times = 0;
	}
	ifname_info( obj, "%s get the plmn %s signal %s", object, plmn_string, signal_string );

	/*****************************************/
	/**** set the auto profile for up ********/
	/*****************************************/
	if ( profile == NULL )
	{
		ifname_info( obj, "%s auto profile setting", object );
		scall( ifdev, "up", NULL );
	}

	/*****************************************/
	/**** attach the network for connect *****/
	/*****************************************/
	if ( 0 != strcmp( mode, "ppp" ) )
	{
	    ifname_info( obj, "%s connect", object );
	    scallt( ifdev, "connect", profile );
	}



	/*****************************************/
	/**** testing connect for the ifdev ******/
	/*****************************************/
	if ( 0 != strcmp( mode, "ppp" ) )
	{
	    ifname_info( obj, "%s attach", object );
		failed_threshold = 60;       // 60
		failed_threshold2 = 180;     // 180
		failed_threshold3 = 600;     // 600
		failed_everytime = 1800;     // 1800
		ptr = json_string( cfg, "attach_failed_threshold" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold = atoi( ptr );
		}
		ptr = json_string( cfg, "attach_failed_threshold2" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold2 = atoi( ptr );
		}
		ptr = json_string( cfg, "attach_failed_threshold3" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold3 = atoi( ptr );
		}
		ptr = json_string( cfg, "attach_failed_everytime" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_everytime = atoi( ptr );
		}
		ptr = json_string( cfg, "need_attach" );
		if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
		{
			failed_timeout = 10;
			for( check=1; check<=failed_timeout; check++ )
			{
				if ( scallt( ifdev, "connected", profile ) == ttrue )
				{
					break;
				}
				ifname_info( obj, "%s attach failed %d/%d", object, check, failed_timeout );
				sleep( 1 );
			}
			if ( check > failed_timeout )
			{
				ifname_info( obj, "%s ignore the attach failed", object );
			}
		}
		else
		{
			if ( reset_times == 0 )
			{
				failed_timeout = failed_threshold;
			}
			else if ( reset_times == 1 )
			{
				failed_timeout = failed_threshold2;
			}
			else if ( reset_times == 2 )
			{
				failed_timeout = failed_threshold3;
			}
			else
			{
				failed_timeout = failed_everytime;
			}
			for( check=1; check<=failed_timeout; check++ )
			{
				ret = scallt( ifdev, "connected", profile );
				if ( ret == ttrue )
				{
					break;
				}
				else if ( ret == terror )
				{
					ifname_warn( obj, "%s ifdev %s not work when connected", object, ifdev );
					talk_free( cfg );
					return terror;
				}
				ifname_info( obj, "%s attach failed %d/%d", object, check, failed_timeout );
				sleep( 1 );
			}
			if ( check > failed_timeout )
			{
				reg_set_string( this, "reset_reason", "attach" );
				ifname_fault( obj, "%s reset the %s when attach failed for %d times", object, ifdev, failed_timeout );
				scall( ifdev, "reset", NULL );
				talk_free( cfg );
				return terror;
			}
		}
		reason = reg_string( this, "reset_reason" );
		if ( reason != NULL && 0 == strcmp( reason, "attach" ) )
		{
			reg_set_string( this, "reset_reason", NULL );
			scall( ifdev, "reset_clear", NULL );
			reset_times = 0;
		}
	}



	/*****************************************/
	/******** connect failed process *********/
	/*****************************************/
	failed_threshold = 3;       // 3*48 = 144
	failed_threshold2 = 7;      // 7*48 = 336
	failed_threshold3 = 15;     // 15*48 = 720
	failed_everytime = 37;      // 37*48 = 1800
	ptr = json_string( cfg, "failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( cfg, "failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	connect_failed = reg_int( this, "connect_failed" );
	if ( connect_failed > 0 )
	{
		if ( connect_failed == failed_threshold || connect_failed == failed_threshold2 || connect_failed == failed_threshold3|| (failed_everytime > 0 && (connect_failed%failed_everytime) == 0 ) )
		{
			ifname_fault( obj, "%s reset the %s when connect failed for %d times", object, ifdev, connect_failed );
			connect_failed++;
			reg_set_int( this, "connect_failed", connect_failed );
			scall( ifdev, "reset", NULL );
			talk_free( cfg );
			return terror;
		}
		ifname_info( obj, "%s connect failed %d", object, connect_failed );
	}
	connect_failed++;
	reg_set_int( this, "connect_failed", connect_failed );



	/*****************************************/
	/**** ifname ip connect take care ********/
	/*****************************************/
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	/* static ip setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		v = json_json( cfg, "static" );
		static_ip_enable( netdev, v );
	}
	else if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
	{
		v = json_json( cfg, "dhcpc" );
		ptr = json_string( v, "static" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_json( cfg, "static" );
			static_ip_enable( netdev, v );
		}
	}

	/* slaac setting */
	if ( method != NULL && 0 == strcmp( method, "slaac" ) )
	{
		slaac_ip_enable( netdev );
	}
	else
	{
		slaac_ip_disable( netdev );
	}
	/* manual ip setting */
	if ( method != NULL && 0 == strcmp( method, "manual" ) )
	{
		/* set the static ip */
		v = json_json( cfg, "manual" );
		manual_ip_enable( netdev, v );
	}
	else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		v = json_json( cfg, "automatic" );
		ptr = json_string( v, "manual" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_json( cfg, "manual" );
			manual_ip_enable( netdev, v );
		}
	}

	ret = tfalse;
	v = json_create( NULL );
	/* ipv4 static setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		if ( mode_static( object, ifdev, netdev, cfg, v ) == true )
		{
			scallt( NETWORK_COM, "online", v );
		}
		/* ipv6 static setting */
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, v ) == true )
			{
				scallt( NETWORK_COM, "upline", v );
			}
		}
		/* ipv6 automatic setting */
		else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
		{
			ret = automatic_client_connect( object, ifdev, netdev, json_json( cfg, "manual" ) );
		}
		ret = ttrue;
		// prevent starting multiple setup
		sleep( 60 );
	}
	else
	{
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, v ) == true )
			{
				scallt( NETWORK_COM, "upline", v );
			}
		}
		/* ipv6 automatic setting */
		else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
		{
			sstart( object, "automatic", NULL, "%s-automatic", object );
		}
		/* ipv4 dhcp client setting */
		if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
		{
			ret = dhcp_client_connect( object, ifdev, netdev, json_json( cfg, "dhcpc" ) );
		}
		/* ipv4 ppp setting */
		else if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
		{
			int mtu;
			talk_t ppp;

			ptr = reg_sstring( ifdev, "mtty" );
			if ( ptr == NULL || *ptr == '\0' )
			{
				ret = terror;
				ifname_faulting( obj, "%s cannot find mtty port", object ); 
			}
			else
			{
				ppp = json_json( cfg, "ppp" );
				json_set_string( ppp, "mtty", ptr );
				mtu = json_number( cfg, "mtu" );
				if ( mtu > 0 )
				{
					json_set_number( ppp, "mtu", mtu );
				}
				if ( profile == NULL )
				{
					profile = scall( ifdev, "operator", NULL );
					ret = ppp_client_connect( object, ifdev, ppp, profile );
					talk_free( profile );
				}
				else
				{
					ret = ppp_client_connect( object, ifdev, ppp, profile );
				}
			}
		}
	}

	/* free the exit */
	talk_free( v );
    talk_free( cfg );
    return ret;
}
boole_t _automatic( obj_t this, param_t param )
{
	talk_t ret;
    talk_t cfg;
	const char *obj;
	const char *ifdev;
	const char *netdev;
	const char *method;
	const char *object;

	obj = obj_com( this );
    object = obj_name( this );
    /* get the ifname configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
        return terror;
    }
	method = json_string( cfg, "method" );
	/* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	/* need the ifdev exist */
	if ( com_have( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}
    /* get the netdev */
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        ifname_fault( obj, "%s netdev get error", object );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }

	ret = terror;
	/* automatic setting */
	if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		ret = automatic_client_connect( object, ifdev, netdev, json_json( cfg, "manual" ) );
	}

    talk_free( cfg );
    return ret;
}



talk_t _state( obj_t this, param_t param )
{
	int tid;
	int delay;
    talk_t ret;
    talk_t v;
    struct stat st;
    const char *ptr;
    const char *object;
    const char *ifdev;
    const char *netdev;
	const char *device;
	const char *mode;
	const char *method;
	const char *custom_dns;
	const char *dns;
	const char *dns2;
	const char *custom_resolve;
	const char *resolve;
	const char *resolve2;
    char path[PATH_MAX];

	netdev = NULL;
    object = obj_name( this );
	/* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
	/* get mode */
	tid = reg_int( this, "tid" );
	mode = reg_string( this, "mode" );
	method = reg_string( this, "method" );
    /* get the custom_dns */
	dns = reg_string( this, "dns" );
	dns2 = reg_string( this, "dns2" );
	custom_dns = reg_string( this, "custom_dns" );
	resolve = reg_string( this, "resolve" );
	resolve2 = reg_string( this, "resolve2" );
	custom_resolve = reg_string( this, "custom_resolve" );

    /* get the ipv4 online status */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    if ( stat( path, &st ) != 0 )
    {
        ret = json_create( NULL );
		if ( ifdev != NULL && *ifdev != '\0' )
		{
			json_set_string( ret, "ifdev", ifdev );
			/* get the netdev */
			netdev = reg_sstring( ifdev, "netdev" );
			if ( netdev != NULL && *netdev != '\0' )
			{
				json_set_string( ret, "netdev", netdev );
				/* get the mac */
				if ( netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, path, sizeof(path) ) == 0 )
				{
					json_set_string( ret, "mac", path );
				}
			}
		}
        if ( spid( object ) >= 0 )
        {
            json_set_string( ret, "status", "uping" );
        }
        else
        {
            json_set_string( ret, "status", "down" );
        }
    }
    else
    {
        char ip[20];
		char mac[20];
        char dstip[20];
        char mask[20];
        unsigned long long rt_bytes, rt_packets, rt_errs, rt_drops, tt_bytes, tt_packets, tt_errs, tt_drops;
        ip[0] = dstip[0] = mask[0] = mac[0] = '\0';
        rt_bytes = rt_packets = rt_errs = rt_drops = tt_bytes = tt_packets = tt_errs = tt_drops = 0;
        ret = file2json( path );
        netdev = device = json_string( ret, "netdev" );
		if ( NULL == strstr( device, "ppp" ) )
		{
			netdev_info( device, ip, sizeof(ip), NULL, 0, mask, sizeof(mask), mac, sizeof(mac) );
		}
		else
		{
			netdev_info( device, ip, sizeof(ip), dstip, sizeof(dstip), mask, sizeof(mask), mac, sizeof(mac) );
		}
        if ( netdev_flags( device, IFF_UP ) <= 0 || *ip == '\0' )
        {
            if ( spid( object ) >= 0 )
            {
                json_set_string( ret, "status", "uping" );
            }
            else
            {
                json_set_string( ret, "status", "down" );
            }
        }
        else
        {
			/* get the keeplive */
			ptr = reg_string( this, "keeplive" );
			if ( ptr != NULL && ( 0 == strcmp( ptr, "icmp" ) || 0 == strcmp( ptr, "dns" ) ) )
			{
				delay = reg_int( this, "delay" );
				if ( delay > 0 )
				{
					json_set_string( ret, "status", "up" );
                    json_set_number( ret, "delay", delay );
				}
				else if ( delay < 0 )
				{
                    json_set_string( ret, "status", "failed" );
				}
				else
				{
                    json_set_string( ret, "status", "block" );
				}
			}
			else if ( ptr != NULL && 0 == strcmp( ptr, "auto" ) )
			{
				delay = reg_int( this, "delay" );
				if ( delay == KEEPLIVE_RECV_MODE )
				{
					json_set_string( ret, "status", "up" );
				}
				else if ( delay > 0 )
				{
					json_set_string( ret, "status", "up" );
                    json_set_number( ret, "delay", delay );
				}
				else if ( delay < 0 )
				{
                    json_set_string( ret, "status", "failed" );
				}
				else
				{
                    json_set_string( ret, "status", "block" );
				}
			}
			else
			{
				json_set_string( ret, "status", "up" );
			}
			/* address */
            json_set_string( ret, "ip", ip );
            json_set_string( ret, "mask", mask );
			if ( *dstip != '\0' )
			{
				json_set_string( ret, "dstip", dstip );
			}
            /* custom dns */
			if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
			{
				if ( dns != NULL && *dns != '\0' )
				{
					json_set_string( ret, "dns", dns );
				}
				if ( dns2 != NULL && *dns2 != '\0' )
				{
					json_set_string( ret, "dns2", dns2 );
				}
			}
            /* get the livetime */
			ptr = json_string( ret, "ontime" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				json_set_string( ret, "livetime", livetime_desc( atoll(ptr), path, sizeof(path) ) );
			}
			/* get the flow */
			netdev_flew( device, &rt_bytes, &rt_packets, &rt_errs, &rt_drops, &tt_bytes, &tt_packets, &tt_errs, &tt_drops );
			snprintf( path, sizeof(path), "%llu", rt_bytes );
			json_set_string( ret, "rx_bytes", path );
			snprintf( path, sizeof(path), "%llu", rt_packets );
			json_set_string( ret, "rx_packets", path );
			snprintf( path, sizeof(path), "%llu", tt_bytes );
			json_set_string( ret, "tx_bytes", path );
			snprintf( path, sizeof(path), "%llu", tt_packets );
			json_set_string( ret, "tx_packets", path );
        }
		/* get the mac */
		if ( 0 == strcmp( mac, "00:00:00:00:00:00" ) || 0 == strcasecmp( mac, "ff:ff:ff:ff:ff:ff" ) ) // ppp interface mac
		{
			/* get the netdev */
			netdev = reg_sstring( ifdev, "netdev" );
			if ( netdev != NULL && *netdev != '\0' )
			{
				netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) );
			}
		}
		json_set_string( ret, "mac", mac );
    }

    /* get the mode of configure */
	if ( tid > 0 )
	{
		json_set_number( ret, "tid", tid );
	}
	if ( mode != NULL && *mode != '\0' )
	{
		json_set_string( ret, "mode", mode );
	}

    /* get the ipv6 online status */
	if ( method != NULL && *method != '\0' && 0 != strcmp( method, "disable" ) )
	{
		int t;
		int rc;
		char *end;
		char host[NI_MAXHOST];
		struct ifaddrs *ifaddr, *ifa;

		json_set_string( ret, "method", method );
	    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
		v = file2json( path );
	    json_sync( v, ret );
		talk_free( v );
		if ( netdev != NULL && *netdev != '\0' )
		{
			if ( getifaddrs( &ifaddr ) == 0 )
			{
				t = 1;
				for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
				{
					if ( ifa->ifa_addr == NULL )
					{
						continue;
					}
					if ( ifa->ifa_addr->sa_family != AF_INET6 )
					{
						continue;
					}
					if ( 0 != strcmp( ifa->ifa_name, netdev ) )
					{
						continue;
					}
					rc = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
					if ( rc == 0 )
					{
						if ( t <= 1 )
						{
							strncpy( path, "addr", sizeof(path)-1 );
							path[sizeof(path)-1] = '\0';
						}
						else
						{
							snprintf( path, sizeof(path), "addr%d", t );
						}
						end = strstr( host, "%" );
						if ( end != NULL )
						{
							*end = '\0';
						}
						json_set_string( ret, path, host );
						t++;
						//printf("dev: %-8s address: <%s> scope %d\n", ifa->ifa_name, host, in6->sin6_scope_id);
					}
				}
				freeifaddrs(ifaddr);					
			}
		}
		if ( custom_resolve != NULL && 0 == strcmp( custom_resolve, "enable" ) )
		{
			if ( resolve != NULL && *resolve != '\0' )
			{
				json_set_string( ret, "resolve", resolve );
			}
			if ( resolve2 != NULL && *resolve2 != '\0' )
			{
				json_set_string( ret, "resolve2", resolve2 );
			}
		}
	}

    return ret;
}
talk_t _status( obj_t this, param_t param )
{
	talk_t v;
	talk_t ret;
	talk_t axp;
	const char *ptr;
	const char *ifdev;
	const char *object;

	/* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        return NULL;
    }
	/* get the ifname status */
	ret = _state( this, param );
	if ( ret == NULL )
	{
		return NULL;
	}
	object = obj_name( this );
    /* get the ifdev or main ifdev info */
	if ( com_have( ifdev, "status" ) == true )
	{
		v = scalls( ifdev, "status", object );
        if ( v > tpanic )
        {
			json_delete_axp( v, "netdev" );
			axp = json_cut_axp( v, "status" );
			ptr = axp_string( axp );
            if ( ptr != NULL && 0 != strcmp( ptr, "up" ) )
            {
                json_set_string( ret, "status", ptr );
            }
			talk_free( axp );
            json_sync( v, ret );
            talk_free( v );
        }
    }
	else
	{
		json_set_string( ret, "status", "nodevice" );
	}
    return ret;
}
boole_t _online( obj_t this, param_t param )
{
	int i;
	int tid;
    int mtu;
	talk_t v;
	talk_t cfg;
	talk_t value;
	const char *obj;
	const char *ptr;
	const char *ifdev;
	const char *object;
	const char *netdev;
	const char *mode;
	const char *gateway;
	const char *dns;
	const char *dns2;
	const char *custom_dns;
	char path[PATH_MAX];
	char ipaddr[NAME_MAX];

	v = param_talk( param, 1 );
	obj = obj_com( this );
	object = obj_name( this );
	/* get ifdev netdev */
	ifdev = json_string( v, "ifdev" );
	netdev = json_string( v, "netdev" );
	if ( netdev == NULL )
	{
		return tfalse;
	}
	/* get the configure */
	cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return tfalse;
	}
	/* get the metric */
	i = json_number( cfg, "metric" );
	if ( i > 0 )
	{
		json_set_number( v, "metric", i );
	}
	/* get mode */
	mode = reg_string( this, "mode" );
	/* get gateway */
	gateway = json_string( v, "gw" );
	/* set the dns */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	value = json_json( cfg, mode );
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
	{
		dns = json_string( value, "dns" );
		dns2 = json_string( value, "dns2" );
		reg_set_string( this, "custom_dns", "enable" );
	}
	else
	{
		dns = json_string( v, "dns" );
		dns2 = json_string( v, "dns2" );
		if ( dns == NULL || *dns == '\0' )
		{
			dns = "8.8.8.8";
		}
		if ( dns2 == NULL || *dns2 == '\0' )
		{
			dns2 = "114.114.114.114";
		}
		ptr = json_string( value, "domain" );
		if ( ptr != NULL )
		{
			string3file( path, "search %s\n", ptr );
		}
		reg_set_string( this, "custom_dns", "disable" );
	}
    if ( dns != NULL && *dns != '\0' && 0 != strcmp( dns, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", dns );
		if ( gateway == NULL || 0 != strcmp( gateway, dns ) )
		{
			routes_switch( DNS_TABLE_ID, dns, NULL, NULL, v, true );
		}
	}
	reg_set_string( this, "dns", dns );
    if ( dns2 != NULL && *dns2 != '\0' && 0 != strcmp( dns2, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", dns2 );
		if ( gateway == NULL || 0 != strcmp( gateway, dns2 ) )
		{
			routes_switch( DNS_TABLE_ID, dns2, NULL, NULL, v, true );
		}
	}
	reg_set_string( this, "dns2", dns2 );
	/* set the gateway */
	netdev_info( netdev, ipaddr, sizeof(ipaddr), NULL, 0, NULL, 0, NULL, 0 );
	if ( gateway != NULL && *gateway != '\0' )
	{
		ifname_info( obj, "%s(%s) %s online[ %s, %s ]", object, netdev, ipaddr, gateway?:"", dns?:"" );
		reg_set_string( this, "gateway", gateway );
	}
	else
	{
		ifname_info( obj, "%s(%s) %s online", object, netdev, ipaddr );
		reg_set_string( this, "gateway", NULL );
	}

	/* clear the failed count when no keeplive */
	ptr = json_string( json_json( cfg, "keeplive" ), "type" );
	if ( ptr == NULL || 0 == strcmp( ptr, "disable" ) )
	{
		i = 0;
		reg_set_int( this, "connect_failed", i );
	}

	/* set masq */
	iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	ptr = json_string( cfg, "masq" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		iptables("-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}
	/* set mtu */
	mtu = json_number( cfg, "mtu" );
	if ( mtu > 0 )
	{
		ifconfig( "%s mtu %d", netdev, mtu );
		reg_set_int( this, "mtu", mtu );
		pmtu_adjust_ifname( object, netdev, mtu );
	}
	else
	{
		if ( 0 != strncmp( object, LAN_COM, strlen(LAN_COM) ) )
		{
			pmtu_adjust_ifname( object, netdev, 0 );
		}
	}
	/* set ppp tx queue */
	if ( 0 == strncmp( netdev, "ppp", 3 ) )
	{
		value = json_json( cfg, "ppp" );
		ptr = json_string( value, "txqueuelen" );
		if ( ptr == NULL || *ptr == '\0' )
		{
			ptr = "500";
		}
		txqueue_set_ifname( object, netdev, ptr );
	}
	/* tid route table init */
	tid = reg_int( this, "tid" );
	if ( tid != 0 )
	{
		routes_ifname( tid, v );
	}
	/* tell the ifdev */
	if ( ifdev != NULL )
	{
		scallst( ifdev, "online", object, v );
		scalls( GPIO_COM, "action", "network/online,%s", ifdev );
	}

	/***********************************/
	/******** Backup SIM START *********/
	/***********************************/
	ptr = json_string( cfg, "bsim" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		boole bsim_online( const char *ifdev, talk_t cfg );
		bsim_online( ifdev, cfg );
	}
	/***********************************/
	/******** Backup SIM END ***********/
	/***********************************/

	talk_free( cfg );
	return ttrue;
}
talk_t _offline( obj_t this, param_t param )
{
	int mtu;
	const char *obj;
	const char *ifdev;
	const char *netdev;
	const char *object;
	char path[PATH_MAX];

	obj = obj_com( this );
	object = obj_name( this );
	/* clear dns file */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		/* clear the masq */
		iptables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
		/* clear the tcp mss */
		mtu = reg_int( this, "mtu" );
		if ( strncmp( LAN_COM, object, strlen(LAN_COM) ) == 0 )
		{
			if ( mtu > 0 )
			{
				pmtu_clear_ifname( object, netdev, mtu );
			}
		}
		else
		{
			pmtu_clear_ifname( object, netdev, mtu );
		}
		ifname_info( obj, "%s(%s) offline", object, netdev );
	}
	else
	{
		ifname_info( obj, "%s offline", object );
	}
	/* tell the ifdev */
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		scalls( ifdev, "offline", object );
	}
	/* led */
	scalls( GPIO_COM, "action", "network/offline,%s", ifdev );

	return ttrue;
}
boole_t _upline( obj_t this, param_t param )
{
	talk_t v;
	talk_t cfg;
	talk_t value;
	const char *ptr;
	const char *obj;
	const char *object;
	const char *netdev;
	const char *method;
	const char *hop;
	const char *custom_resolve;
	const char *resolve;
	const char *resolve2;
	char path[PATH_MAX];

	obj = obj_com( this );
	object = obj_name( this );
	v = param_talk( param, 1 );
	/* get netdev */
	netdev = json_string( v, "netdev" );
	/* get the configure */
	cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return tfalse;
	}
	/* get mode */
	method = reg_string( this, "method" );
	/* get gateway */
	hop = json_string( v, "hop" );
	/* get the custom_resolve */
	snprintf( path, sizeof(path), "%s/%s.ipv6", RESOLV_DIR, object );
	unlink( path );
	value = json_json( cfg, method );
	custom_resolve = json_string( value, "custom_resolve" );
	if ( custom_resolve != NULL && 0 == strcmp( custom_resolve, "enable" ) )
	{
		resolve = json_string( value, "resolve" );
		resolve2 = json_string( value, "resolve2" );
		reg_set_string( this, "custom_resolve", "enable" );
	}
	else
	{
		resolve = json_string( v, "resolve" );
		if ( resolve == NULL || *resolve == '\0' )
		{
			resolve = "2001:4860:4860::8888"; // GOOGLE
		}
		resolve2 = json_string( v, "resolve2" );
		if ( resolve2 == NULL || *resolve2 == '\0' )
		{
			resolve2 = "2001:dc7:1000::1";    //CNNIC
		}
		ptr = json_string( value, "domain" );
		if ( ptr != NULL )
		{
			string3file( path, "search %s\n", ptr );
		}
		reg_set_string( this, "custom_resolve", "disable" );
	}
	if ( resolve != NULL && *resolve != '\0' )
	{
		string3file( path, "nameserver %s\n", resolve );
	}
	reg_set_string( this, "resolve", resolve );
	if ( resolve2 != NULL && *resolve2 != '\0' )
	{
		string3file( path, "nameserver %s\n", resolve2 );
	}
	reg_set_string( this, "resolve2", resolve2 );

	ifname_info( obj, "%s(%s) upline[ %s, %s ]", object, netdev, hop?:"", resolve?:"" );
	/* masquerade */
	ip6tables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	ptr = json_string( cfg, "masquerade" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		ip6tables("-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}

	talk_free( cfg );
	return ttrue;
}
boole_t _keepon( obj_t this, param_t param )
{
	int i;

	i = 0;
	reg_set_int( this, "connect_failed", i );
	return ttrue;
}
boole_t _keepoff( obj_t this, param_t param )
{
	talk_t cfg;
	talk_t keeplive;
	unsigned long i;
	const char *ptr;
	const char *ifdev;
	const char *object;

	object = obj_name( this );
    cfg = config_sgets( object, NULL );

	/***********************************/
	/******** Backup SIM START *********/
	/***********************************/
	ptr = json_string( cfg, "bsim" );
	if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
	{
		ifdev = reg_string( this, "ifdev" );
		boole bsim_keepoff( const char *ifdev, talk_t cfg );
		if ( bsim_keepoff( ifdev, cfg ) == true )
		{
			talk_free( cfg );
			return ttrue;
		}
	}
	/***********************************/
	/******** Backup SIM END ***********/
	/***********************************/

	keeplive = json_json( cfg, "keeplive" );
	if ( keeplive == NULL )
	{
		talk_free( cfg );
		return tfalse;
	}
	ptr = json_string( keeplive, "action" );
	if ( ptr != NULL && 0 == strcmp( ptr, "reboot" ) )
	{
		i = uptime_int();
		if ( i < 180 )
		{
		    keeplive_warn( "%s keeplive check failed, reset connection instead of rebooting system when uptime (%d) is too low", object, i );
			sreset( NULL, NULL, NULL, object );
		}
		else
		{
		    keeplive_warn( "%s keeplive check failed, must reboot the system", object );
			machine_restart( 1, "keeplive_failed" );
		}
	}
	else if ( ptr != NULL && 0 == strcmp( ptr, "reset" ) )
	{
		ifdev = reg_string( this, "ifdev" );
		if ( ifdev != NULL && *ifdev != '\0' )
		{
		    keeplive_warn( "%s keeplive check failed, must reset the %s", object, ifdev );
			scall( ifdev, "reset", NULL );
		}
	}
	else
	{
	    keeplive_warn( "%s keeplive check failed, must reset connection", object );
		sreset( NULL, NULL, NULL, object );
	}
	talk_free( cfg );
	return ttrue;
}



/* only for ifdev */
talk_t _operator( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "operator", param );
	}
	return ret;
}
boole_t _reset( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = tfalse;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "reset", param );
	}
	return ret;
}
talk_t _lock_imei( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "lock_imei", param );
	}
	return ret;
}
talk_t _lock_imsi( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "lock_imsi", param );
	}
	return ret;
}
talk_t  _custom_set( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "custom_set", param );
	}
	return ret;
}
talk_t  _custom_watch( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "custom_watch", param );
	}
	return ret;
}



