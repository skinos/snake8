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
 * reset_times  | simcard                    | signal/PLMN | attach    | connect_failed
 * --------------|----------------------------|-------------|-----------|---------------
 *  0 (1st)     |  30s                       |  120s       | 120s      |  2 cycles
 *  1..5        | 180s each (~15min @ 3min)  |  300s(*)    | 300s(*)   |  5 cycles(*)
 *  6..8        | 300s each (~15min @ 5min)  |  600s(*)    | 600s(*)   | 15 cycles(*)
 *  9+          | 1800s                      | 1800s(*)    | 1800s(*)  | 37 cycles(*)
 *
 * (*) signal/attach/connect still map only reset_times 0 / 1 / 2 / 3+.
 * SIM stays on threshold2 for reset_times 1..5 and threshold3 for 6..8,
 * then everytime — card-swap friendly without lengthening each wait.
 * When a stage recovers, reset_reason is cleared and reset_times is zeroed.
 *
 * _service return (daemon):
 *   tfalse — START kept, this service is rerun (wait ifdev fun, ~5s).
 *   terror — START cleared, this service is not rerun.
 *            Ladder timeout does scall(ifdev, "reset") then return terror.
 *            reset[] power-cycles the module and bumps ifdev reset_times.
 *            The next _service starts only when ifname setup / sstart runs
 *            again (USB rematch → frame add, or READY watch sreset).
 *
 * Stage 1 — SIM card not detected:
 *   Default need_simcard is enabled. Each check sleeps 1s.
 *   1st reset after 30s; then five times at 180s (15min, every ~3min);
 *   then three times at 300s (15min); then every 1800s (30min).
 *   Then ifdev reset + terror as above.
 *
 * Stage 2 — Signal or PLMN not acquired:
 *   Default need_plmn and need_signal are enabled (both required).
 *   1st reset after 120s, 2nd after 300s, 3rd after 600s, then 1800s.
 *   Same ifdev reset + terror as Stage 1.
 *
 * Stage 3 — Network attach failed (non-PPP mode only):
 *   Default need_attach is enabled.
 *   1st reset after 120s (slower than SIM), 2nd after 300s, 3rd after 600s, then 1800s.
 *   Same ifdev reset + terror as Stage 1.
 *
 * Stage 4 — Connect failed (consecutive _service cycle failures):
 *   A per-cycle counter (connect_failed) accumulates across _service cycles.
 *   Resets the module when the counter hits 2, 5, 15, or every 37 cycles.
 *   Same ifdev reset + terror as Stage 1.
 *
 * On stage recovery (sim/signal/attach OK after that reason reset):
 *   clear reset_reason and reset_clear (times=0).
 *
 * con_service on ifdev: _setup publishes the sstart name before sstart.
 * Cleared in _shut before sdelete. After WAN is up, READY watch sresets
 * the service (5 min debounce) to redial.
 */

#include "skin/skin.h"
#include "skinnet/skinnet.h"
#include <ifaddrs.h>
#include <unistd.h>

boole_t _setup( obj_t this, param_t param )
{
	int tid;
	int need;
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
	need = 1;
	ptr = json_string( cfg, "need_simcard" );
	if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
	{
		need = 0;
	}
	reg_set_int( this, "need_simcard", need );
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
	reg_sset_string( ifdev, "con_service", object );
	sstart( object, "service", NULL, object );
    talk_free( cfg );
    return ttrue;
}
boole_t _shut( obj_t this, param_t param )
{
	talk_t cfg;
	talk_t profile;
	const char *ptr;
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
	/* drop name first so atd will not sreset during sdelete */
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		reg_sset_string( ifdev, "con_service", NULL );
	}
    /* stop the dhcp6 service */
    sdelete( "%s-dhcp6", object );
    /* stop the service */
    sdelete( object );

    /* delete online file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ol", object );
    unlink( path );
    /* delete upline file */
    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
    unlink( path );

    /* down the ifdev */
    if ( ifdev != NULL && *ifdev != '\0' )
    {
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
					|| 0 == strcmp( ptr, "ims" )
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
			|| 0 == strcmp( ptr, "ims" )
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
	/* hide IPv6 knobs when kernel/global ipv6 register is off */
	if ( cfg != NULL && reg_int( NULL, "ipv6" ) != 1 )
	{
		json_delete_axp( cfg, "mode6" );
		json_delete_axp( cfg, "static6" );
		json_delete_axp( cfg, "dhcpc6" );
		json_delete_axp( cfg, "slaac" );
		json_delete_axp( cfg, "masq6" );
		json_delete_axp( cfg, "mtu6" );
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
	return scall( ifdev, "netdev", NULL );
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
	const char *mode6;
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
	mode6 = json_string( cfg, "mode6" );
	if ( mode6 == NULL || *mode6 == '\0' )
	{
		mode6 = "disable";
	}
	/* na(5G) or eth(RmNet): default dhcpc; else default ppp */
	if ( mode == NULL || *mode == '\0' )
	{
		i = reg_sint( ifdev, "na" );
		if ( i <= 0 )
		{
			i = reg_sint( ifdev, "eth" );
		}
		if ( i > 0 )
		{
			mode = "dhcpc";
		}
		else
		{
			mode = "ppp";
		}
	}
	/* no netdev to ppp */
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
		if ( mode == NULL || 0 != strcmp( mode, "ppp" ) )
		{
			ifname_warn( obj, "%s modify the mode to ppp when cannot find netdev", object );
		}
		if ( mode6 != NULL && 0 != strcmp( mode6, "disable" ) )
		{
			ifname_warn( obj, "%s modify the mode6 to disable when cannot find netdev", object );
		}
    	mode = "ppp";
		mode6 = "disable";
		json_set_string( cfg, "mode", "ppp" );
    }
	/* ppp mode no ipv6 (odhcp6c needs a real netdev) */
	if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
	{
		if ( mode6 != NULL && 0 != strcmp( mode6, "disable" ) )
		{
			ifname_warn( obj, "%s modify the mode6 to disable when mode is ppp", object );
		}
		mode6 = "disable";
	}
	/* no kernel IPv6: same as mode6=disable */
	if ( reg_int( NULL, "ipv6" ) != 1 )
	{
		if ( mode6 != NULL && 0 != strcmp( mode6, "disable" ) )
		{
			ifname_warn( obj, "%s modify the mode6 to disable when kernel ipv6 is off", object );
		}
		mode6 = "disable";
	}
	/* set the mode */
	reg_set_string( this, "mode", mode );
	reg_set_string( this, "mode6", mode6 );



	/*****************************************/
	/**** testing simcard for the ifdev ******/
	/*****************************************/
    ifname_info( obj, "%s simcard detection", object );
	failed_threshold = 30;       // 30
	failed_threshold2 = 180;     // 180 (~3min)
	failed_threshold3 = 300;     // 300 (~5min)
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
		else if ( reset_times >= 1 && reset_times <= 5 )
		{
			/* threshold2: 5 x ~3min ~= 15min before threshold3 */
			failed_timeout = failed_threshold2;
		}
		else if ( reset_times >= 6 && reset_times <= 8 )
		{
			/* threshold3: 3 x ~5min ~= 15min before everytime */
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
	/* connect owns the LED when running */
	if ( spid( CONNECT_COM ) <= 0 )
	{
		scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	}
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
		/* tfalse: modem_off / back to cfun; abort this round and wait fun on next _service */
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
	failed_threshold = 120;      // 120  (2min)
	failed_threshold2 = 300;     // 300  (5min)
	failed_threshold3 = 600;     // 600  (10min)
	failed_everytime = 1800;     // 1800 (30min)
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
		failed_threshold = 120;      // 120 (slower than SIM first round)
		failed_threshold2 = 300;     // 300
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
	failed_threshold = 2;       // 2 cycles
	failed_threshold2 = 5;      // 5
	failed_threshold3 = 15;     // 15
	failed_everytime = 37;      // 37 — keep long tail for locked/no-heal cases
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
	/* connect owns the LED when running */
	if ( spid( CONNECT_COM ) <= 0 )
	{
		scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	}
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

	/* IPv6: only mode6=slaac enables kernel accept_ra; dhcpc6/auto use odhcp6c userland RA */
	if ( mode6 != NULL && 0 == strcmp( mode6, "slaac" ) )
	{
		slaac_ip_enable( netdev );
	}
	else
	{
		slaac_ip_disable( netdev );
	}
	/* static6 address on the netdev */
	if ( mode6 != NULL && 0 == strcmp( mode6, "static6" ) )
	{
		v = json_json( cfg, "static6" );
		static6_ip_enable( netdev, v );
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
		/* ipv6 static6 */
		if ( mode6 != NULL && 0 == strcmp( mode6, "static6" ) )
		{
			if ( mode_static6( object, ifdev, netdev, cfg, v ) == true )
			{
				scallt( NETWORK_COM, "upline", v );
			}
		}
		/* ipv6 dhcpc6/auto: run odhcp6c in this process */
		else if ( mode6 != NULL && ( 0 == strcmp( mode6, "dhcpc6" ) || 0 == strcmp( mode6, "auto" ) ) )
		{
			ret = dhcp6_client_connect( object, ifdev, netdev, json_json( cfg, "dhcpc6" ) );
		}
		/* ipv6 slaac: background poll then upline */
		else if ( mode6 != NULL && 0 == strcmp( mode6, "slaac" ) )
		{
			sstart( object, "dhcp6", NULL, "%s-dhcp6", object );
		}
		ret = ttrue;
		// prevent starting multiple setup
		sleep( 60 );
	}
	else
	{
		if ( mode6 != NULL && 0 == strcmp( mode6, "static6" ) )
		{
			if ( mode_static6( object, ifdev, netdev, cfg, v ) == true )
			{
				scallt( NETWORK_COM, "upline", v );
			}
		}
		/* ipv6 dhcpc6/auto/slaac: background service */
		else if ( mode6 != NULL && ( 0 == strcmp( mode6, "dhcpc6" ) || 0 == strcmp( mode6, "auto" ) || 0 == strcmp( mode6, "slaac" ) ) )
		{
			sstart( object, "dhcp6", NULL, "%s-dhcp6", object );
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
boole_t _dhcp6( obj_t this, param_t param )
{
	int i;
	int rc;
	talk_t v;
	talk_t ret;
	talk_t cfg;
	char *end;
	const char *obj;
	const char *ifdev;
	const char *netdev;
	const char *mode6;
	const char *object;
	char host[NI_MAXHOST];
	struct ifaddrs *ifa;
	struct ifaddrs *ifaddr;

	obj = obj_com( this );
	object = obj_name( this );
	/* get the ifname configure */
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return terror;
	}
	mode6 = reg_string( this, "mode6" );
	if ( mode6 == NULL || *mode6 == '\0' )
	{
		mode6 = json_string( cfg, "mode6" );
	}
	if ( mode6 == NULL || *mode6 == '\0' )
	{
		mode6 = "disable";
	}
	if ( reg_int( NULL, "ipv6" ) != 1 )
	{
		mode6 = "disable";
	}
	if ( 0 == strcmp( mode6, "disable" ) )
	{
		talk_free( cfg );
		return ttrue;
	}
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
	/* dhcpc6/auto: odhcp6c replaces this process */
	if ( 0 == strcmp( mode6, "dhcpc6" ) || 0 == strcmp( mode6, "auto" ) )
	{
		ret = dhcp6_client_connect( object, ifdev, netdev, json_json( cfg, "dhcpc6" ) );
		talk_free( cfg );
		return ret;
	}
	/* slaac: wait for a global address then upline */
	if ( 0 == strcmp( mode6, "slaac" ) )
	{
		slaac_ip_enable( netdev );
		for ( i = 0; i < 60; i++ )
		{
			host[0] = '\0';
			if ( getifaddrs( &ifaddr ) == 0 )
			{
				for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next )
				{
					if ( ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET6 )
					{
						continue;
					}
					if ( 0 != strcmp( ifa->ifa_name, netdev ) )
					{
						continue;
					}
					rc = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
					if ( rc != 0 )
					{
						continue;
					}
					end = strstr( host, "%" );
					if ( end != NULL )
					{
						*end = '\0';
					}
					/* skip link-local */
					if ( strncmp( host, "fe80:", 5 ) == 0 || strncmp( host, "FE80:", 5 ) == 0 )
					{
						host[0] = '\0';
						continue;
					}
					break;
				}
				freeifaddrs( ifaddr );
			}
			if ( host[0] != '\0' )
			{
				v = json_create( NULL );
				json_set_string( v, "mode6", "slaac" );
				json_set_string( v, "ifname", object );
				json_set_string( v, "ifdev", ifdev );
				json_set_string( v, "netdev", netdev );
				json_set_string( v, "addr", host );
				scallt( NETWORK_COM, "upline", v );
				talk_free( v );
				talk_free( cfg );
				pause();
				return ttrue;
			}
			sleep( 1 );
		}
		ifname_warn( obj, "%s slaac wait GUA timeout on %s", object, netdev );
		talk_free( cfg );
		return tfalse;
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
    const char *gw6;
    const char *object;
    const char *prefix;
    const char *ifdev;
    const char *netdev;
	const char *device;
	const char *mode;
	const char *mode6;
	const char *custom_dns;
	const char *dns;
	const char *dns2;
	const char *dns6;
	const char *dns62;
	const char *custom_dns6;
    char path[PATH_MAX];

	netdev = NULL;
    object = obj_name( this );
	/* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
	/* get mode */
	tid = reg_int( this, "tid" );
	mode = reg_string( this, "mode" );
	mode6 = reg_string( this, "mode6" );
	if ( reg_int( NULL, "ipv6" ) != 1 )
	{
		mode6 = "disable";
	}
    /* get the custom_dns */
	dns = reg_string( this, "dns" );
	dns2 = reg_string( this, "dns2" );
	custom_dns = reg_string( this, "custom_dns" );

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

    /* IPv6 status only when kernel IPv6 is on and mode6 is configured on */
	if ( reg_int( NULL, "ipv6" ) == 1 && mode6 != NULL && *mode6 != '\0' && 0 != strcmp( mode6, "disable" ) )
	{
		int t;
		int rc;
		int bi;
		int plen;
		char *end;
		unsigned char b;
		unsigned char *bytes;
		char host[NI_MAXHOST];
		struct ifaddrs *ifaddr, *ifa;

		prefix = reg_string( this, "prefix" );
		gw6 = reg_string( this, "gw6" );
		dns6 = reg_string( this, "dns6" );
		dns62 = reg_string( this, "dns62" );
		custom_dns6 = reg_string( this, "custom_dns6" );
		json_set_string( ret, "mode6", mode6 );
	    project_var_path( path, sizeof(path), NETWORK_PROJECT, "%s.ul", object );
		v = file2json( path );
		if ( json_check( v ) == true )
		{
			json_sync( v, ret );
		}
		ptr = json_string( v, "gw6" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			gw6 = ptr;
		}
		ptr = json_string( v, "dns6" );
		if ( custom_dns6 == NULL || 0 != strcmp( custom_dns6, "enable" ) )
		{
			if ( ptr != NULL && *ptr != '\0' )
			{
				dns6 = ptr;
			}
			ptr = json_string( v, "dns62" );
			if ( ptr != NULL && *ptr != '\0' )
			{
				dns62 = ptr;
			}
		}
		ptr = json_string( v, "prefix" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			prefix = ptr;
		}
		json_delete_axp( ret, "gw6" );
		json_delete_axp( ret, "dns6" );
		json_delete_axp( ret, "dns62" );
		json_delete_axp( ret, "prefix" );
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
						/* status shows CIDR with /length; % is zone id, already stripped */
						if ( ifa->ifa_netmask != NULL && ifa->ifa_netmask->sa_family == AF_INET6 )
						{
							plen = 0;
							bytes = ((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr.s6_addr;
							for ( bi = 0; bi < 16; bi++ )
							{
								b = bytes[bi];
								if ( b == 0xff )
								{
									plen += 8;
									continue;
								}
								while ( b & 0x80 )
								{
									plen++;
									b <<= 1;
								}
								break;
							}
							snprintf( host + strlen(host), sizeof(host) - strlen(host), "/%d", plen );
						}
						json_set_string( ret, path, host );
						t++;
					}
				}
				freeifaddrs(ifaddr);					
			}
		}
		if ( gw6 != NULL && *gw6 != '\0' )
		{
			json_set_string( ret, "gw6", gw6 );
		}
		if ( dns6 != NULL && *dns6 != '\0' )
		{
			json_set_string( ret, "dns6", dns6 );
		}
		if ( dns62 != NULL && *dns62 != '\0' )
		{
			json_set_string( ret, "dns62", dns62 );
		}
		if ( prefix != NULL && *prefix != '\0' )
		{
			json_set_string( ret, "prefix", prefix );
		}
		talk_free( v );
	}

    return ret;
}
talk_t _status( obj_t this, param_t param )
{
	int need;
	talk_t v;
	talk_t ret;
	talk_t axp;
	const char *ptr;
	const char *status;
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
			/* ifname up wins; modem up keeps ifname; else show modem */
			status = axp_string( axp );
			ptr = json_string( ret, "status" );
			if ( ptr != NULL )
			{
				if ( 0 == strcmp( ptr, "up" ) )
				{
					status = NULL;
				}
			}
			if ( status != NULL )
			{
				if ( 0 == strcmp( status, "up" ) )
				{
					status = NULL;
				}
			}
			if ( status != NULL )
			{
				json_set_string( ret, "status", status );
			}
			talk_free( axp );
            json_sync( v, ret );
            talk_free( v );
			/* Hide camped RF when SIM is required and not usable. */
			need = reg_oget_int( this, "need_simcard", 1 );
			if ( need != 0 )
			{
				ptr = json_string( ret, "iccid" );
				if ( ptr == NULL || *ptr == '\0'
					|| 0 == strcmp( ptr, "nosim" )
					|| 0 == strcmp( ptr, "pin" )
					|| 0 == strcmp( ptr, "puk" ) )
				{
					json_delete_axp( ret, "operator" );
					json_delete_axp( ret, "plmn" );
					json_delete_axp( ret, "signal" );
					json_delete_axp( ret, "signal2" );
					json_delete_axp( ret, "csq" );
					json_delete_axp( ret, "rssi" );
					json_delete_axp( ret, "rsrp" );
					json_delete_axp( ret, "rsrq" );
					json_delete_axp( ret, "sinr" );
				}
			}
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
	/* ipv6 already up: fold dns6/dns62 into the same resolv file */
	if ( reg_int( NULL, "ipv6" ) == 1 )
	{
		ptr = reg_string( this, "dns6" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			string3file( path, "nameserver %s\n", ptr );
		}
		ptr = reg_string( this, "dns62" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			string3file( path, "nameserver %s\n", ptr );
		}
	}
	/* local IPv4 from online event, else read netdev */
	ptr = json_string( v, "ip" );
	ipaddr[0] = '\0';
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "0.0.0.0" ) )
	{
		strncpy( ipaddr, ptr, sizeof(ipaddr)-1 );
		ipaddr[sizeof(ipaddr)-1] = '\0';
	}
	else
	{
		netdev_info( netdev, ipaddr, sizeof(ipaddr), NULL, 0, NULL, 0, NULL, 0 );
	}
	json_set_string( v, "ip", ipaddr );
	reg_set_string( this, "ip", ipaddr );
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
	if ( tid > 0 )
	{
		routes_ifname( tid, v );
	}
	/* tell the ifdev */
	if ( ifdev != NULL )
	{
		scallst( ifdev, "online", object, v );
		/* connect owns the LED when running */
		if ( spid( CONNECT_COM ) <= 0 )
		{
			scalls( GPIO_COM, "action", "network/online,%s", ifdev );
		}
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
	/* connect owns the LED when running */
	if ( spid( CONNECT_COM ) <= 0 )
	{
		scalls( GPIO_COM, "action", "network/offline,%s", ifdev );
	}

	return ttrue;
}
boole_t _upline( obj_t this, param_t param )
{
	int mtu6;
	talk_t v;
	talk_t cfg;
	talk_t value;
	const char *ptr;
	const char *obj;
	const char *gw6;
	const char *dns6;
	const char *dns62;
	const char *mode6;
	const char *masq6;
	const char *object;
	const char *netdev;
	const char *prefix;
	const char *custom_dns;
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
	mode6 = reg_string( this, "mode6" );
	if ( mode6 == NULL || *mode6 == '\0' )
	{
		mode6 = json_string( v, "mode6" );
	}
	if ( reg_int( NULL, "ipv6" ) != 1 )
	{
		mode6 = "disable";
	}
	if ( mode6 == NULL || *mode6 == '\0' || 0 == strcmp( mode6, "disable" ) )
	{
		talk_free( cfg );
		return ttrue;
	}
	/* detail block: auto uses dhcpc6{} */
	if ( mode6 != NULL && 0 == strcmp( mode6, "auto" ) )
	{
		value = json_json( cfg, "dhcpc6" );
	}
	else if ( mode6 != NULL && *mode6 != '\0' )
	{
		value = json_json( cfg, mode6 );
	}
	else
	{
		value = NULL;
	}
	gw6 = json_string( v, "gw6" );
	prefix = json_string( v, "prefix" );
	/* IPv6 DNS into regs + rewrite this ifname resolv */
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
	{
		dns6 = json_string( value, "dns" );
		dns62 = json_string( value, "dns2" );
		reg_set_string( this, "custom_dns6", "enable" );
	}
	else
	{
		dns6 = json_string( v, "dns6" );
		dns62 = json_string( v, "dns62" );
		reg_set_string( this, "custom_dns6", "disable" );
	}
	reg_set_string( this, "dns6", dns6 );
	reg_set_string( this, "dns62", dns62 );
	/* rewrite this ifname resolv from dns/dns2/dns6/dns62 */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	ptr = reg_string( this, "dns" );
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", ptr );
	}
	ptr = reg_string( this, "dns2" );
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", ptr );
	}
	if ( dns6 != NULL && *dns6 != '\0' )
	{
		string3file( path, "nameserver %s\n", dns6 );
	}
	if ( dns62 != NULL && *dns62 != '\0' )
	{
		string3file( path, "nameserver %s\n", dns62 );
	}
	/* optional IPv6 MTU */
	mtu6 = json_number( cfg, "mtu6" );
	if ( mtu6 > 0 && netdev != NULL && *netdev != '\0' )
	{
		snprintf( path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/mtu", netdev );
		number2file( path, mtu6 );
	}

	ifname_info( obj, "%s(%s) upline[ %s, %s, prefix=%s ]", object, netdev, gw6?:"", dns6?:"", prefix?:"" );
	reg_set_string( this, "gw6", gw6 );
	reg_set_string( this, "prefix", prefix );
	/* masq6=enable always NAT66; auto does not NAT (PD or relay GUA instead) */
	ip6tables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	masq6 = json_string( cfg, "masq6" );
	if ( masq6 == NULL || *masq6 == '\0' )
	{
		masq6 = "auto";
	}
	if ( 0 == strcmp( masq6, "enable" ) )
	{
		ip6tables( "-t nat -A %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
	}

	talk_free( cfg );
	return ttrue;
}
boole_t _downline( obj_t this, param_t param )
{
	const char *obj;
	const char *object;
	const char *netdev;
	const char *ptr;
	char path[PATH_MAX];

	obj = obj_com( this );
	object = obj_name( this );
	netdev = reg_string( this, "netdev" );
	/* clear local IPv6 state; rewrite resolv without v6 NS */
	reg_set_string( this, "dns6", NULL );
	reg_set_string( this, "dns62", NULL );
	reg_set_string( this, "gw6", NULL );
	reg_set_string( this, "prefix", NULL );
	reg_set_string( this, "custom_dns6", NULL );
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	ptr = reg_string( this, "dns" );
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", ptr );
	}
	ptr = reg_string( this, "dns2" );
	if ( ptr != NULL && *ptr != '\0' && 0 != strcmp( ptr, "0.0.0.0" ) )
	{
		string3file( path, "nameserver %s\n", ptr );
	}
	if ( netdev != NULL && *netdev != '\0' )
	{
		ip6tables( "-t nat -D %s -o %s -j MASQUERADE", MASQ_CHAIN, netdev );
		ifname_info( obj, "%s(%s) downline", object, netdev );
	}
	else
	{
		ifname_info( obj, "%s downline", object );
	}
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



