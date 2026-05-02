/*
 * Backup SIM service path for ltecon (bsim).
 */
#include "skin/skin.h"
#include "skinnet/skinnet.h"



boole_t bsim_service( obj_t this, param_t param, talk_t cfg, const char *ifdev, const char *object, const char *obj, const char *sim_state, int bsim_times )
{
	int i;
	int sig;
	int plmn;
	int check;
	talk_t v;
	talk_t ret;
	talk_t mcfg;
	talk_t profile;
	const char *ptr;
	const char *pin;
	const char *mode;
	const char *netdev;
	const char *method;
	const char *reason;
	int connect_failed;
	int failed_timeout;
	int failed_threshold;
	int failed_threshold2;
	int failed_threshold3;
	int failed_everytime;
	char plmn_string[NAME_MAX];
	char signal_string[NAME_MAX];
	talk_t bsim_cfg;

	/*****************************************/
	/**** get the backup infomation **********/
	/*****************************************/
	mcfg = cfg;
	bsim_cfg = json_json( cfg, "bsim_cfg" );
	if ( sim_state != NULL && 0 == strcmp( sim_state, "back" ) )
	{
		mcfg = bsim_cfg;
	}



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
	ptr = json_string( bsim_cfg, "simcard_failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "simcard_failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "simcard_failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "simcard_failed_everytime" );
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
		if ( bsim_times == 0 )
		{
			failed_timeout = failed_threshold;
		}
		else if ( bsim_times == 1 )
		{
			failed_timeout = failed_threshold2;
		}
		else if ( bsim_times == 2 )
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
			reg_set_string( this, "switch_reason", "sim" );
			if ( sim_state == NULL || 0 == strcmp( sim_state, "main" ) )
			{
				scall( ifdev, "bsim_back", NULL );
			}
			else
			{
				scall( ifdev, "bsim_main", NULL );
			}
			talk_free( cfg );
			return terror;
		}
	}
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	reason = reg_string( this, "switch_reason" );
	if ( reason != NULL && 0 == strcmp( reason, "sim" ) )
	{
		reg_set_string( this, "switch_reason", NULL );
		scall( ifdev, "bsim_clear", NULL );
		bsim_times = 0;
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
		ifname_info( obj, "%s custom profile setting", object );
		ret = scallt( ifdev, "up", profile );
		
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
	ptr = json_string( bsim_cfg, "signal_failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "signal_failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "signal_failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "signal_failed_everytime" );
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
		if ( bsim_times == 0 )
		{
			failed_timeout = failed_threshold;
		}
		else if ( bsim_times == 1 )
		{
			failed_timeout = failed_threshold2;
		}
		else if ( bsim_times == 2 )
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
			reg_set_string( this, "switch_reason", "signal" );
			if ( sim_state == NULL || 0 == strcmp( sim_state, "main" ) )
			{
				scall( ifdev, "bsim_back", NULL );
			}
			else
			{
				scall( ifdev, "bsim_main", NULL );
			}
			talk_free( cfg );
			return terror;
		}
	}
	reason = reg_string( this, "switch_reason" );
	if ( reason != NULL && 0 == strcmp( reason, "signal" ) )
	{
		reg_set_string( this, "switch_reason", NULL );
		scall( ifdev, "bsim_clear", NULL );
		bsim_times = 0;
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
		ptr = json_string( bsim_cfg, "attach_failed_threshold" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold = atoi( ptr );
		}
		ptr = json_string( bsim_cfg, "attach_failed_threshold2" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold2 = atoi( ptr );
		}
		ptr = json_string( bsim_cfg, "attach_failed_threshold3" );
		if ( ptr != NULL && *ptr != '\0' )
		{
			failed_threshold3 = atoi( ptr );
		}
		ptr = json_string( bsim_cfg, "attach_failed_everytime" );
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
			if ( bsim_times == 0 )
			{
				failed_timeout = failed_threshold;
			}
			else if ( bsim_times == 1 )
			{
				failed_timeout = failed_threshold2;
			}
			else if ( bsim_times == 2 )
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
				reg_set_string( this, "switch_reason", "attach" );
				if ( sim_state == NULL || 0 == strcmp( sim_state, "main" ) )
				{
					scall( ifdev, "bsim_back", NULL );
				}
				else
				{
					scall( ifdev, "bsim_main", NULL );
				}
				talk_free( cfg );
				return terror;
			}
		}
		reason = reg_string( this, "switch_reason" );
		if ( reason != NULL && 0 == strcmp( reason, "attach" ) )
		{
			reg_set_string( this, "switch_reason", NULL );
			scall( ifdev, "bsim_clear", NULL );
			bsim_times = 0;
		}
	}



	/*****************************************/
	/******** connect failed process *********/
	/*****************************************/
	failed_threshold = 3;       // 3*48 = 144
	failed_threshold2 = 7;      // 7*48 = 336
	failed_threshold3 = 15;     // 15*48 = 720
	failed_everytime = 37;      // 37*48 = 1800
	ptr = json_string( bsim_cfg, "failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( bsim_cfg, "failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	connect_failed = reg_int( this, "connect_failed" );
	if ( connect_failed > 0 )
	{
		if ( connect_failed == failed_threshold || connect_failed == failed_threshold2 || connect_failed == failed_threshold3|| (failed_everytime > 0 && (connect_failed%failed_everytime) == 0 ) )
		{
			connect_failed++;
			reg_set_int( this, "connect_failed", connect_failed );
			if ( sim_state == NULL || 0 == strcmp( sim_state, "main" ) )
			{
				scall( ifdev, "bsim_back", NULL );
			}
			else
			{
				scall( ifdev, "bsim_main", NULL );
			}
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



boole bsim_online( const char *ifdev, talk_t cfg )
{
	int failover;
	talk_t bsim_cfg;
	const char *sim_set;
	const char *sim_state;
	char name[NAME_MAX];
	char sim_buffer[NAME_MAX];
	
	if ( ifdev == NULL || cfg == NULL )
	{
		return false;
	}
	// clear
	scall( ifdev, "bsim_clear", NULL );
	// failover
	bsim_cfg = json_json( cfg, "bsim_cfg" );
	sim_set = json_string( bsim_cfg, "mode" );
	if ( sim_set == NULL || *sim_set == '\0' || 0 == strcmp( sim_set, "auto" ) )
	{
		failover = json_number( bsim_cfg, "failover" );
		if ( failover > 0 )
		{
			sim_state = scall_string( sim_buffer, sizeof(sim_buffer), ifdev, "bsim_state", NULL );
			if ( sim_state != NULL && 0 == strcmp( sim_state, "back" ) )
			{
				snprintf( name, sizeof(name), "%s-simover", ifdev );
				sstarts( name, ifdev, "bsim_over", "%d", failover );
				return true;
			}
		}
	}
	return false;
}

boole bsim_keepoff( const char *ifdev, talk_t cfg )
{
	talk_t bsim_cfg;
	const char *ptr;
	const char *sim_set;
	const char *sim_state;
	char sim_buffer[NAME_MAX];

	if ( ifdev == NULL || cfg == NULL )
	{
		return false;
	}
	// keeplive faild to switch
	bsim_cfg = json_json( cfg, "bsim_cfg" );
	sim_set = json_string( bsim_cfg, "mode" );
	if ( sim_set == NULL || *sim_set == '\0' || 0 == strcmp( sim_set, "auto" ) )
	{
		ptr = json_string( bsim_cfg, "keeplive_switch" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			sim_state = scall_string( sim_buffer, sizeof(sim_buffer), ifdev, "bsim_state", NULL );
			if ( sim_state != NULL && 0 == strcmp( sim_state, "back" ) )
			{
				scall( ifdev, "bsim_main", NULL );
			}
			else
			{
				scall( ifdev, "bsim_back", NULL );
			}
			return true;
		}
	}
	return false;
}



