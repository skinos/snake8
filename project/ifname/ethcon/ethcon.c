/*
 *  Description:  ip connection
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
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
    const char *obj;
	const char *ifdev;
    const char *object;
    char path[PATH_MAX];

    obj = obj_com( this );
    object = obj_name( this );
    ifname_info( obj, "%s shut", object );

    /* call the offline */
    scalls( NETWORK_COM, "offline", object );
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
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev != NULL && *ifdev != '\0' )
    {
    	scall( ifdev, "down", NULL );
		/* connect owns the LED when running */
		if ( spid( CONNECT_COM ) <= 0 )
		{
			scalls( GPIO_COM, "action", "network/offline,%s", ifdev );
		}
	}

    return ttrue;
}
boole _set( obj_t this, talk_t v, attr_t path )
{
	int i;
    boole ret;
	const char *object;

	object = obj_name( this );
	if ( NULL == strstr( object, LAN_COM ) )
	{
		ret = config_set( this, v, path );
		i = 0;
		reg_set_int( this, "connect_failed", i );
		/* Must stay after config_set: do not move _shut earlier — network/offline
		 * may SIGHUP connect, which immediately re-setups this ifname from disk. */
		_shut( this, NULL );
		_setup( this, NULL );
	}
	else
	{
		ret = config_set( this, v, path );
	}
    return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	talk_t cfg;
	talk_t ret;

	cfg = config_get( this, NULL );
	/* hide IPv6 knobs when kernel/global ipv6 register is off */
	if ( cfg != NULL && reg_int( NULL, "ipv6" ) != 1 )
	{
		json_delete_axp( cfg, "mode6" );
		json_delete_axp( cfg, "static6" );
		json_delete_axp( cfg, "dhcpc6" );
		json_delete_axp( cfg, "slaac" );
		json_delete_axp( cfg, "6in4" );
		json_delete_axp( cfg, "6rd" );
		json_delete_axp( cfg, "masq6" );
		json_delete_axp( cfg, "mtu6" );
		json_delete_axp( cfg, "ula" );
		json_delete_axp( cfg, "pdid" );
		json_delete_axp( cfg, "pdlen" );
		json_delete_axp( cfg, "dhcps6" );
	}
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
    int check;
    talk_t v;
	talk_t ret;
    talk_t cfg;
    const char *ptr;
	const char *obj;
	const char *mode;
	const char *ifdev;
    const char *object;
	const char *netdev;
	const char *mode6;
	int reset_times;
	int connect_failed;
	int failed_timeout;
	int failed_threshold;
	int failed_threshold2;
	int failed_threshold3;
	int failed_everytime;

	obj = obj_com( this );
    object = obj_name( this );
    /* offline first */
	scalls( NETWORK_COM, "offline", object );

	/*****************************************/
	/********** get the infomation ***********/
	/*****************************************/
    /* get the ifdev */
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
    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
		ifname_fault( obj, "%s cannot find configuration", object );
		sleep( 5 );
    	return terror;
    }
	json_set_string( cfg, "ifname", object );
    /* get the ifdev reset times */
	reset_times = reg_sint( ifdev, "reset_times" );

	/*****************************************/
	/***** get the connect mode **************/
	/*****************************************/
	mode = json_string( cfg, "mode" );
	if ( mode == NULL || *mode == '\0' )
	{
		mode = "dhcpc";
	}
	mode6 = json_string( cfg, "mode6" );
	if ( mode6 == NULL || *mode6 == '\0' )
	{
		mode6 = "disable";
	}
	/* no kernel IPv6: same as mode6=disable */
	if ( reg_int( NULL, "ipv6" ) != 1 )
	{
		mode6 = "disable";
	}
	/* set the mode */
	reg_set_string( this, "mode", mode );
	reg_set_string( this, "mode6", mode6 );
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        ifname_fault( obj, "%s netdev get error", object );
        talk_free( cfg );
        sleep( 5 );
        return tfalse;
    }



	/*****************************************/
	/******** up the ifdev with cfg **********/
	/*****************************************/
    ifname_info( obj, "%s up", object );
    ret = scallt( ifdev, "up", cfg );
	if ( ret == tfalse )
    {
        ifname_warn( obj, "%s up failed", object );
        talk_free( cfg );
        sleep( 5 );
        return tfalse;
    }
	else if ( ret == terror )
	{
		ifname_warn( obj, "%s ifdev %s not work when up", object, ifdev );
		talk_free( cfg );
		sleep( 5 );
		return terror;
	}
	/* connect owns the LED when running */
	if ( spid( CONNECT_COM ) <= 0 )
	{
		scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	}

	/*****************************************/
	/******* set the netdev mac **************/
	/*****************************************/
    ptr = json_string( cfg, "mac" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        scalls( ifdev, "setmac", ptr );
    }

	/*****************************************/
	/********** connect the ifdev ************/
	/*****************************************/
    ifname_debug( obj, "%s connect", object );
    ret = scallt( ifdev, "connect", cfg );
	if ( ret == tfalse )
    {
        ifname_fault( obj, "%s connect failed", object );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }
	else if ( ret == terror )
	{
		ifname_warn( obj, "%s ifdev %s not work when connect", object, ifdev );
		talk_free( cfg );
		sleep( 5 );
		return terror;
	}



	/*****************************************/
	/**** testing connect for the ifdev ******/
	/*****************************************/
    ifname_debug( obj, "%s connect test", object );
	failed_threshold = 60;       // 60
	failed_threshold2 = 180;     // 180
	failed_threshold3 = 600;     // 600
	failed_everytime = 1800;     // 1800
	ptr = json_string( cfg, "connect_failed_threshold" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold = atoi( ptr );
	}
	ptr = json_string( cfg, "connect_failed_threshold2" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold2 = atoi( ptr );
	}
	ptr = json_string( cfg, "connect_failed_threshold3" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_threshold3 = atoi( ptr );
	}
	ptr = json_string( cfg, "connect_failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	ptr = json_string( cfg, "need_connect" );
	if ( ptr != NULL && 0 == strcmp( ptr, "disable" ) )
	{
		failed_timeout = 10;
		for( check=0; check<failed_timeout; check++ )
		{
			ret = scall( ifdev, "connected", NULL );
			if ( ret == ttrue )
			{
				break;
			}
			else if ( ret == terror )
			{
				ifname_warn( obj, "%s ifdev %s not work when connected", object, ifdev );
				talk_free( cfg );
				sleep( 5 );
				return terror;
			}
			ifname_debug( obj, "%s connect failed %d", object, check );
			sleep( 1 );
		}
		if ( check >= failed_timeout )
		{
			ifname_debug( obj, "%s ignore the connect failed", object );
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
		for( check=0; check<failed_timeout; check++ )
		{
			ret = scall( ifdev, "connected", NULL );
			if ( ret == ttrue )
			{
				break;
			}
			else if ( ret == terror )
			{
				ifname_warn( obj, "%s ifdev %s not work when connected", object, ifdev );
				talk_free( cfg );
				sleep( 5 );
				return terror;
			}
			ifname_debug( obj, "%s connect failed %d", object, check );
			sleep( 1 );
		}
		if ( check >= failed_timeout )
		{
			if ( com_have( ifdev, "reset" ) == true )
			{
				ifname_fault( obj, "%s reset the %s when connect failed for %d times", object, ifdev, failed_timeout );
				scall( ifdev, "reset", NULL );
				sleep( 3 );
			}
			else
			{
				ifname_debug( obj, "%s down the %s when connect failed for %d times", object, ifdev, failed_timeout );
				scall( ifdev, "down", NULL );
			}
			talk_free( cfg );
			return tfalse;
		}
	}
	/* connect owns the LED when running */
	if ( spid( CONNECT_COM ) <= 0 )
	{
		scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
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
		if ( connect_failed == failed_threshold || connect_failed == failed_threshold2 || connect_failed == failed_threshold3|| (connect_failed%failed_everytime) == 0 )
		{
			if ( com_have( ifdev, "reset" ) == true )
			{
				ifname_fault( obj, "%s reset the %s when connect failed for %d times", object, ifdev, connect_failed );
				connect_failed++;
				reg_set_int( this, "connect_failed", connect_failed );
				scall( ifdev, "reset", NULL );
				sleep( 3 );
			}
			else
			{
				ifname_debug( obj, "%s down the %s when connect failed for %d times", object, ifdev, connect_failed );
				connect_failed++;
				reg_set_int( this, "connect_failed", connect_failed );
				scall( ifdev, "down", NULL );
			}
			talk_free( cfg );
			return tfalse;
		}
		ifname_debug( obj, "%s connect failed %d", object, connect_failed );
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
		/* ipv6 dhcpc6/auto/slaac: background service (pppoec waits for ipv6-up on pppX) */
		else if ( mode != NULL && 0 != strcmp( mode, "pppoec" ) && mode6 != NULL && ( 0 == strcmp( mode6, "dhcpc6" ) || 0 == strcmp( mode6, "auto" ) || 0 == strcmp( mode6, "slaac" ) ) )
		{
			sstart( object, "dhcp6", NULL, "%s-dhcp6", object );
		}
		/* ipv4 dhcp client setting */
		if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
		{
			ret = dhcp_client_connect( object, ifdev, netdev, json_json( cfg, "dhcpc" ) );
		}
		/* ipv4 pppoe setting */
		else if ( mode != NULL && 0 == strcmp( mode, "pppoec" ) )
		{
			int mtu;
			talk_t pppoe;

			pppoe = json_json( cfg, "pppoec" );
			mtu = json_number( cfg, "mtu" );
			if ( mtu > 0 )
			{
				json_set_number( pppoe, "mtu", mtu );
			}
			if ( mode6 != NULL && ( 0 == strcmp( mode6, "dhcpc6" ) || 0 == strcmp( mode6, "auto" ) || 0 == strcmp( mode6, "slaac" ) ) )
			{
				json_set_string( pppoe, "ipv6", "enable" );
			}
			ret = pppoe_client_connect( object, ifdev, netdev, pppoe );
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
	const char *ptr;
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
	/* pppoe ipv6-up passes the ppp netdev as the first param */
	ptr = param_string( param, 1 );
	if ( ptr != NULL && *ptr != '\0' )
	{
		netdev = ptr;
	}
	else
	{
		netdev = reg_sstring( ifdev, "netdev" );
	}
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
			ptr = "1000";
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
		scalls( ifdev, "online", object );
		/* connect owns the LED when running */
		if ( spid( CONNECT_COM ) <= 0 )
		{
			scalls( GPIO_COM, "action", "network/online,%s", ifdev );
		}
	}

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
	unsigned long i;
	const char *ptr;
	const char *ifdev;
	const char *object;

	object = obj_name( this );
    cfg = config_sgets( object, "keeplive" );
	if ( cfg == NULL )
	{
		return tfalse;
	}
	ptr = json_string( cfg, "action" );
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



/* only for apclient */
talk_t _aplist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "aplist", NULL );
	}
	return ret;
}
/* only for apclient */
talk_t _chlist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "chlist", NULL );
	}
	return ret;
}
/* only for apclient */
talk_t _securelist( obj_t this, param_t param )
{
	talk_t ret;
	const char *ifdev;

	ret = NULL;
	ifdev = reg_string( this, "ifdev" );
	if ( ifdev != NULL && *ifdev != '\0' )
	{
		ret = scall( ifdev, "securelist", NULL );
	}
	return ret;
}



