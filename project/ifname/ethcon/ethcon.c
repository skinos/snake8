/*
 *  Description:  ip connection
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "land/skin.h"
#include "landnet/landnet.h"
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

    /* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
        talk_free( cfg );
        return tfalse;
    }
	/* need the ifdev exist */
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}

    /* run the app connection */
    ifname_info( obj, "%s startup", object );
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
    	scall( ifdev, "down", NULL );
		scalls( GPIO_COM, "action", "network/offline,%s", ifdev );
	}

    return ttrue;
}
boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
	const char *object;

	object = obj_name( this );
	if ( NULL == strstr( object, LAN_COM ) )
	{
		_shut( this, NULL );
		ret = config_set( this, v, path );
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
    return config_get( this, path );
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
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        return NULL;
    }

    return string2x( netdev );
}
talk_t _state( obj_t this, param_t param )
{
	int tid;
	int delay;
    talk_t ret;
    talk_t v;
    struct stat st;
    boole keeplive;
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

    object = obj_name( this );
	/* get the ifdev */
	ifdev = reg_string( this, "ifdev" );
    /* get the keeplive */
	ptr = reg_string( this, "keeplive" );
    if ( ptr != NULL && ( 0 == strcmp( ptr, "icmp" ) || 0 == strcmp( ptr, "dns" ) || 0 == strcmp( ptr, "auto" ) ) )
    {
    	keeplive = true;
		delay = reg_int( this, "delay" );
    }
	else
	{
		keeplive = false;
	}
	netdev = NULL;
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
			netdev = register_svalue( ifdev, "netdev" );
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
        ret = file2talk( path );
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
            json_set_string( ret, "status", "up" );
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
            /* get the keeplive status */
            if ( keeplive == true )
            {
				if ( delay > 0 )
				{
                    json_set_number( ret, "delay", delay );
				}
				else
				{
                    json_set_string( ret, "delay", "failed" );
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
			netdev = register_svalue( ifdev, "netdev" );
			if ( netdev != NULL && *netdev != '\0' )
			{
				netdev_info( netdev, NULL, 0, NULL, 0, NULL, 0, mac, sizeof(mac) );
			}
		}
		json_set_string( ret, "mac", mac );
    }

    /* get the mode of configure */
	if ( tid != 0 )
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
		v = file2talk( path );
	    talk_patch( v, ret );
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
							strcpy( path, "addr" );
						}
						else
						{
							sprintf( path, "addr%d", t );
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
	if ( com_sexist( ifdev, "state" ) == true )
	{
		v = scalls( ifdev, "state", object );
        if ( v > tpanic )
        {
            ptr = json_string( ret, "netdev" );
            if ( ptr != NULL && *ptr != '\0' )
            {
                json_delete_axp( v, "netdev" );
            }
            talk_patch( v, ret );
            ptr = json_string( v, "state" );
            if ( ptr != NULL && 0 != strcmp( ptr, "connect" ) && 0 != strcmp( ptr, "up" ) )
            {
                json_set_string( ret, "status", ptr );
            }
            talk_free( v );
        }
    }
    return ret;
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



boole_t _service( obj_t this, param_t param )
{
    int ready;
    int check;
    talk_t v;
	talk_t ret;
    talk_t cfg;
    talk_t value;
    const char *ptr;
	const char *obj;
	const char *mode;
	const char *ifdev;
    const char *object;
	const char *netdev;
	const char *method;
	int connect_failed;
	int failed_timeout;
	int failed_threshold;
	int failed_threshold2;
	int failed_everytime;
	int *reg_connect_failed;

	obj = obj_com( this );
    object = obj_name( this );
    /* offline first */
	scalls( NETWORK_COM, "offline", object );

	/*****************************************/
	/********** get the infomation ***********/
	/*****************************************/
	/* get the ifdev */
	ifdev = register_pointer( this, "ifdev" );
    if ( ifdev == NULL || *ifdev == '\0' )
    {
		ifname_fault( obj, "cannot found %s ifdev", object );
		sleep( 5 );
        return tfalse;
    }
	if ( com_sexist( ifdev, NULL ) == false )
	{
		ifname_fault( obj, "%s ifdev %s inexistence", object, ifdev );
		sleep( 5 );
        return tfalse;
	}
    /* get the configure */
    cfg = config_get( this, NULL ); 
    if ( cfg == NULL )
    {
		ifname_fault( obj, "cannot found %s configure", object );
    	return terror;
    }
	failed_timeout = 45;
	failed_threshold = 3;
	failed_threshold2 = 15;
	failed_everytime = 24;
	ptr = json_string( cfg, "failed_timeout" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_timeout = atoi( ptr );
	}
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
	ptr = json_string( cfg, "failed_everytime" );
	if ( ptr != NULL && *ptr != '\0' )
	{
		failed_everytime = atoi( ptr );
	}
	mode = json_string( cfg, "mode" );
	if ( mode == NULL || *mode == '\0' )
	{
		mode = "dhcpc";
	}
	method = json_string( cfg, "method" );
	if ( method == NULL || *method == '\0' )
	{
		method = "disable";
	}
	if ( mode != NULL && 0 == strcmp( mode, "ppp" ) )
	{
		method = "disable";
	}
	reg_set_string( this, "mode", mode );
	reg_set_string( this, "method", method );
	/* get the count */
	connect_failed = 0;
	reg_connect_failed = register_pointer( this, "connect_failed" );
	if ( reg_connect_failed != NULL )
	{
		connect_failed = *reg_connect_failed;
	}
	else
	{
		reg_connect_failed = reg_set_int( this, "connect_failed", connect_failed );
	}

	/*****************************************/
	/******** connect failed process *********/
	/*****************************************/
	ret = ttrue;
	if ( connect_failed > 0 )
	{
		if ( connect_failed == failed_threshold )
		{
			ret = terror;
		}
		else if ( connect_failed == failed_threshold2 )
		{
			ret = terror;
		}
		else if ( (connect_failed%failed_everytime) == 0 )
		{
			ret = terror;
		}
		ifname_warn( obj, "%s cannot connect %d times", object, connect_failed );
	}
	connect_failed++;
	*reg_connect_failed = connect_failed;
	if ( ret == terror )
	{
		if ( com_sexist( ifdev, "reset" ) == true )
		{
			scall( ifdev, "reset", NULL );
		}
		talk_free( cfg );
		sleep( 5 );
		return tfalse;
	}

	/*****************************************/
	/******** up the ifdev with cfg **********/
	/*****************************************/
    /* up the ifdev */
	json_set_string( cfg, "ifname", object );
    ifname_info( obj, "%s up", ifdev );
    if ( scallt( ifdev, "up", cfg ) != ttrue )
    {
        ifname_warn( obj, "%s up failed", ifdev );
        talk_free( cfg );
        sleep( 5 );
        return tfalse;
    }
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
    /* need the netdev after ifdev up */
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        ifname_fault( obj, "%s netdev get error", ifdev );
        talk_free( cfg );
        sleep( 5 );
        return tfalse;
    }

	/*****************************************/
	/********** connect the ifdev ************/
	/*****************************************/
    /* connect the ifdev */
    ifname_info( obj, "%s connect", ifdev );
    if ( scall( ifdev, "connect", NULL ) != ttrue )
    {
        ifname_fault( obj, "%s connect failed", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }
    /* set the netdev mac */
    ptr = json_string( cfg, "mac" );
    if ( ptr != NULL && *ptr != '\0' )
    {
        scalls( ifdev, "setmac", ptr );
    }

	/*****************************************/
	/***** check the ifdev connected *********/
	/*****************************************/
	ready = 0;
	check = 0;
	while( check < failed_timeout )
	{
		if ( scall( ifdev, "connected", NULL ) == ttrue )
		{
			ready++;
			if ( ready >= 3 )
			{
				ifname_info( obj, "%s connected ready", ifdev );
				break;
			}
			usleep( 300000 );
			continue;
		}
		ready = 0;
		check++;
		sleep( 1 );
	}
	if ( check >= failed_timeout )
	{
		ifname_warn( obj, "%s connect timeout", ifdev );
		scall( ifdev, "down", NULL );
		talk_free( cfg );
		return tfalse;
	}

	/*****************************************/
	/**** ifname ip connect take care ********/
	/*****************************************/
	scalls( GPIO_COM, "action", "network/onlineing,%s", ifdev );
	/* static ip setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		v = json_value( cfg, "static" );
		static_ip_enable( netdev, v );
	}
	else if ( mode != NULL && 0 == strcmp( mode, "dhcpc" ) )
	{
		v = json_value( cfg, "dhcpc" );
		ptr = json_string( v, "static" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_value( cfg, "static" );
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
		v = json_value( cfg, "manual" );
		manual_ip_enable( netdev, v );
	}
	else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		v = json_value( cfg, "automatic" );
		ptr = json_string( v, "manual" );
		if ( ptr != NULL && 0 == strcmp( ptr, "enable" ) )
		{
			v = json_value( cfg, "manual" );
			manual_ip_enable( netdev, v );
		}
	}

	ret = tfalse;
	value = json_create( NULL );
	/* ipv4 static setting */
	if ( mode != NULL && 0 == strcmp( mode, "static" ) )
	{
		if ( mode_static( object, ifdev, netdev, cfg, value ) == true )
		{
			scallt( NETWORK_COM, "online", value );
		}
		/* ipv6 static setting */
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, value ) == true )
			{
				scallt( NETWORK_COM, "upline", value );
			}
		}
		/* ipv6 automatic setting */
		else if ( method != NULL && 0 == strcmp( method, "automatic" ) )
		{
			v = json_value( cfg, "manual" );
			ret = automatic_client_connect( object, ifdev, netdev, v );
		}
		ret = ttrue;
		// Prevent starting multiple setup
		sleep( 10 );
	}
	else
	{
		if ( method != NULL && 0 == strcmp( method, "manual" ) )
		{
			if ( method_manual( object, ifdev, netdev, cfg, value ) == true )
			{
				scallt( NETWORK_COM, "upline", value );
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
			v = json_value( cfg, "dhcpc" );
			ret = dhcp_client_connect( object, ifdev, netdev, v );
		}
		/* ipv4 pppoe setting */
		else if ( mode != NULL && 0 == strcmp( mode, "pppoec" ) )
		{
			const char *mtu;
			v = json_value( cfg, "pppoec" );
			mtu = json_string( cfg, "mtu" );
			if ( mtu != NULL && atoi(mtu) > 0 )
			{
				json_set_string( v, "mtu", mtu );
			}
			ret = pppoe_client_connect( object, ifdev, netdev, v );
		}
	}

	talk_free( value );
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
	if ( com_sexist( ifdev, NULL ) == false )
	{
        talk_free( cfg );
        return tfalse;
	}
    /* get the netdev */
	netdev = reg_sstring( ifdev, "netdev" );
    if ( netdev == NULL || *netdev == '\0' )
    {
        ifname_fault( obj, "%s netdev get error", ifdev );
        talk_free( cfg );
        sleep( 3 );
        return tfalse;
    }

	ret = terror;
	/* automatic setting */
	if ( method != NULL && 0 == strcmp( method, "automatic" ) )
	{
		ret = automatic_client_connect( object, ifdev, netdev, json_value( cfg, "manual" ) );
	}

    talk_free( cfg );
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
	const char *metric;
	char path[PATH_MAX];
	char ipaddr[NAME_MAX];

	v = param_talk( param, 1 );
	obj = obj_com( this );
	object = obj_name( this );
	/* get ifdev */
	ifdev = json_string( v, "ifdev" );
	/* get netdev */
	netdev = json_string( v, "netdev" );
	/* get the configure */
	cfg = config_get( this, NULL ); 
	if ( cfg == NULL )
	{
		return tfalse;
	}
	/* set the keeplive */
	ptr = json_string( json_value( cfg, "keeplive"), "type" );
	reg_set_string( this, "keeplive", ptr );
	/* set the metric */
	metric = json_string( cfg, "metric" );
	reg_set_string( this, "metric", metric );
	json_set_string( v, "metric", metric );
	/* get mode */
	mode = reg_string( this, "mode" );
	/* get gateway */
	gateway = json_string( v, "gw" );
	/* set the dns */
	snprintf( path, sizeof(path), "%s/%s", RESOLV_DIR, object );
	unlink( path );
	value = json_value( cfg, mode );
	custom_dns = json_string( value, "custom_dns" );
	if ( custom_dns != NULL && 0 == strcmp( custom_dns, "enable" ) )
	{
		dns = json_string( value, "dns" );
		dns2 = json_string( value, "dns2" );
		i = 1;
		reg_set_int( this, "custom_dns", i );
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
		i = 0;
		reg_set_int( this, "custom_dns", i );
	}
	if ( dns != NULL && *dns != '\0' )
	{
		string3file( path, "nameserver %s\n", dns );
		if ( gateway == NULL || 0 != strcmp( gateway, dns ) )
		{
			route_switch( dns, NULL, NULL, v, true );
		}
	}
	reg_set_string( this, "dns", dns );
	if ( dns2 != NULL && *dns2 != '\0' )
	{
		string3file( path, "nameserver %s\n", dns2 );
		if ( gateway == NULL || 0 != strcmp( gateway, dns2 ) )
		{
			route_switch( dns2, NULL, NULL, v, true );
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
	ptr = json_string( json_value( cfg, "keeplive" ), "type" );
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
	if ( strncmp( LAN_COM, object, strlen(LAN_COM) ) == 0 )
	{
		if ( mtu > 0 )
		{
			shell( "ifconfig %s mtu %s", netdev, mtu );
			reg_set_int( this, "mtu", mtu );
			pmtu_adjust_ifname( object, netdev, mtu );
		}
	}
	else
	{
		if ( mtu > 0 )
		{
			shell( "ifconfig %s mtu %s", netdev, mtu );
			reg_set_int( this, "mtu", mtu );
		}
		pmtu_adjust_ifname( object, netdev, mtu );
	}
	/* set ppp tx queue */
	if ( 0 == strncmp( netdev, "ppp", 3 ) )
	{
		value = json_value( cfg, "ppp" );
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
		routes_create_ifname( tid, v );
	}
	/* tell the ifdev */
	if ( ifdev != NULL )
	{
		scalls( ifdev, "online", object );
		scalls( GPIO_COM, "action", "network/online,%s", ifdev );
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
	talk_t v;
	talk_t cfg;
	talk_t value;
	const char *ptr;
	const char *object;
	const char *netdev;
	const char *method;
	const char *hop;
	const char *custom_resolve;
	const char *resolve;
	const char *resolve2;
	char path[PATH_MAX];

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
	value = json_value( cfg, method );
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
			resolve = "8.8.8.8";
		}
		resolve2 = json_string( v, "resolve2" );
		if ( resolve2 == NULL || *resolve2 == '\0' )
		{
			resolve2 = "114.114.114.114";
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

	ifname_info( "%s(%s) upline[ %s, %s ]", object, netdev, hop?:"", resolve?:"" );
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
	return tfalse;
}
boole_t _keepoff( obj_t this, param_t param )
{
	return tfalse;
}



