/*
 *  Description:  wireless staion management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"

static int dbm2signal( int rssi )
{
	if ( rssi >= -55 )
	{
		return 4;
	}
	else if ( rssi >= -66 )
	{
		return 3;
	}
	else if ( rssi >= -88 )
	{
		return 2;
	}
	else if ( rssi > -99 )
	{
		return 1;
	}
    return 0;
}
static boole station_dev_apscan( const char *netdev )
{
	char path[1024];

    if ( netdev_flags( netdev, IFF_UP ) <= 0 )
    {
        ifconfig( "%s up", netdev );
    }
	snprintf( path, sizeof(path), "/tmp/.iwinfo_scan_%s", netdev );
	shell( "iwinfo %s scan > %s", netdev, path );
    return true;
}
static talk_t station_dev_apresult( const char *netdev )
{
	int i;
	FILE *fp;
	talk_t x;
	char *ptr;
	talk_t result;
	const char *s;
	char path[1024];
	char readbuf[1024];

	snprintf( path, sizeof(path), "/tmp/.iwinfo_scan_%s", netdev );
	/* parse */
	fp = fopen( path, "r");
	if( fp == NULL )
	{
		return NULL;
	}
	x = NULL;
	result = json_create( NULL );
	while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
	{
		if ( strncmp( readbuf, "Cell", 4 ) == 0 )
		{
			ptr = readbuf+19;
			*(ptr+17) = '\0';
			low2upp( ptr );
			x = json_create( NULL );
			json_set_json( result, ptr, x );
		}
		else if ( x != NULL )
		{
			ptr = strstr( readbuf, "ESSID: \"" );
			if ( ptr != NULL )
			{
				ptr += 8;
				i = strlen(ptr);
				ptr[i-2] = '\0';
				if ( *ptr != '\0' )
				{
					json_set_string( x, "ssid", ptr );
				}
				continue;
			}
			s = strstr( readbuf, "Mode:" );
			ptr = strstr( readbuf, "Channel:" );
			if ( s != NULL && ptr != NULL )
			{
				if ( sscanf( ptr, "%*[^:]: %d", &i ) == 1 )
				{
					json_set_number( x, "channel", i );
				}
				continue;
			}
			ptr = strstr( readbuf, "Signal:" );
			if ( ptr != NULL )
			{
				if ( sscanf( readbuf, "%*[^:]: %d", &i ) == 1 )
				{
					json_set_number( x, "rssi", i );
					i = dbm2signal( i );
					json_set_number( x, "signal", i );
				}
				continue;
			}
			ptr = strstr( readbuf, "Encryption:" );
			if ( ptr != NULL )
			{
				/* NONE */
				if ( strstr( readbuf, "none" ) != NULL )
				{
					json_set_string( x, "secure", "disable" );
				}
				/* wpapsk aes */
				else if ( strstr( readbuf, "WPA PSK (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "aes" );
				}
				/* wpapsk tkip */
				else if ( strstr( readbuf, "WPA PSK (TKIP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
				}
				/* wpapsk tkipaes */
				else if ( strstr( readbuf, "WPA PSK (TKIP, CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapsk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
				}
				/* wpa2psk aes */
				else if ( strstr( readbuf, "WPA2 PSK (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
				}
				/* wpa2psk tkip */
				else if ( strstr( readbuf, "WPA2 PSK (TKIP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
				}
				/* wpa2psk tkipaes */
				else if ( strstr( readbuf, "WPA2 PSK (TKIP, CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
				}
				/* wpa3psk aes */
				else if ( strstr( readbuf, "WPA3 SAE (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpa3psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
				}
				/* wpapskwpa2psk aes */
				else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (TKIP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
				}
				/* wpapskwpa2psk tkip */
				else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
				}
				/* wpapskwpa2psk tkipaes */
				else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (TKIP, CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkipaes" );
				}
				/* wpa2pskwpa3psk aes */
				else if ( strstr( readbuf, "mixed WPA2/WPA3 PSK/SAE (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpa2pskwpa3psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
				}
				continue;
			}
		}
	}
	fclose( fp );
	/* save the scanning */
	var2path( path, sizeof(path), "%s.apresult", netdev );
	json2file( result, path );
	return result;
}
static talk_t station_dev_appick( talk_t result, const char *ssid, const char *bssid, const char *ssid2, const char *ssid3 )
{
	int last;
	int rssp;
    talk_t v;
    talk_t axp;
	const char *ptr;
    const char *peermac;

    if ( bssid != NULL && *bssid != '\0' )
    {
        v = json_cut_value( result, bssid );
        if ( json_number( v, "rssi" ) > 0 )
        {
			return v;
        }
		talk_free( v );
		return NULL;
    }

	last = 0;
	peermac = NULL;
    axp = NULL;
    while( NULL != ( axp = json_next( result, axp ) ) )
    {
    	v = axp_json( axp );
        ptr = json_string( v, "ssid" );
        if ( ptr == NULL )
        {
            continue;
        }
        if ( ( ssid != NULL && 0 == strcmp( ssid, ptr ) ) || ( ssid2 != NULL && 0 == strcmp( ssid2, ptr ) ) || ( ssid3 != NULL && 0 == strcmp( ssid3, ptr ) ) )
        {
			rssp = json_number( v, "rssi" );
			if ( rssp > last )
			{
				last = rssp;
				peermac = axp_name( axp );
				json_set_string( v, "peermac", peermac );
			}
        }
    }
	if ( peermac != NULL )
	{
		return json_cut_value( result, peermac );
	}
	return NULL;
}
static talk_t station_dev_aplist( const char *netdev, const char *ssid, const char *bssid, const char *ssid2, const char *ssid3 )
{
	int times;
	talk_t pick;
	talk_t result;

	// mark scan
	times = 1;
	reg_sset_int( netdev, "scan", times );
	// start scan
    station_dev_apscan( netdev );
	// get the scan result
    result = station_dev_apresult( netdev );
	// unmark scan
	times = 0;
	reg_sset_int( netdev, "scan", times );
	// pick the peer
	if ( ssid != NULL || bssid != NULL || ssid2 != NULL || ssid3 != NULL )
	{
		pick = station_dev_appick( result, ssid, bssid, ssid2, ssid3 );
		talk_free( result );
		return pick;
	}
	return result;
}
static int station_dev_connected( obj_t this, const char *netdev )
{
	int ret;
	FILE *fp;
	char *ptr;
    char readbuf[256];
	char wpa_cli[256];

	/* get the command result */	
    snprintf( wpa_cli, sizeof(wpa_cli), "/tmp/.wpa_cli_%s_status", netdev );
    if (  shell( "wpa_cli -i %s status > %s", netdev, wpa_cli ) != 0 )
    {
        return 1;
    }
	/* parse the wpa_cli */
    fp = fopen( wpa_cli, "r");
    if( fp == NULL )
    {
        return 1;
    }
	ret = 1;
    readbuf[0] = '\0';
    while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
    {
        if ( strncmp( readbuf, "wpa_state=", 10 ) == 0 )
        {
        	ptr = readbuf+10;
			if ( strncmp( ptr, "COMPLETED", 9 ) == 0 )
			{
				ret = 0;
			}
        }
    }
    fclose( fp );
	return ret;
}



boole_t _setup( obj_t this, param_t param )
{
	const char *object;
	const char *netdev;

	object = obj_name( this );
	netdev = reg_string( this, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		wifi_debug( "%s(%s) add to network frame", object, netdev );
		scalls( NETWORK_COM, "add", "%s,%s", object, netdev );
		return ttrue;
	}

	return tfalse;
}
boole_t _shut( obj_t this, param_t param )
{
	const char *object;
	const char *netdev;

	object = obj_name( this );
	netdev = reg_string( this, "netdev" );
	if ( netdev != NULL && *netdev != '\0' )
	{
		wifi_debug( "%s delete from network frame", object );
		scalls( NETWORK_COM, "delete", object );
	}
	return ttrue;
}
boole _set( obj_t this, talk_t v, attr_t path )
{
	boole ret;

	_shut( this, NULL );
	ret = config_set( this, v, path );
	_setup( this, NULL );
	return ret;
}
talk_t _get( obj_t this, attr_t path )
{
	return config_get( this, path );
}



talk_t _netdev( obj_t this, param_t param )
{
	const char *netdev;

	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}
	return string2x( netdev );
}
boole_t _up( obj_t this, param_t param )
{
	int fd;
	talk_t v;
	talk_t cfg;
	talk_t opt;
	const char *ptr;
	const char *radio;
	const char *object;
	const char *netdev;
	const char *ifname;
	const char *status;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;
	const char *peermode;
	const char *channel;
	const char *nossid;
	const char *secure;
	const char *wpa_encrypt;
	const char *wpa_key;
	char path[PATH_MAX];

	/* get the configure */
	object = obj_name( this );
	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return terror;
	}
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return tfalse;
	}

    /* ignore it if up already */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	fd = lock_open( path, O_RDWR|O_CREAT|O_EXCL, 0666, -1 );
    if ( fd < 0 )
    {
		wifi_warn( "%s(%s) already up", object, netdev );
		talk_free( cfg );
		return ttrue;
    }
	wifi_info( "%s(%s) up", object, netdev );

	/* get the configure */
	status = NULL;
	peer = peer2 = peer3 = peermac = NULL;
	opt = param_talk( param, 1 );
	if ( opt != NULL )
	{
		peer = json_string( opt, "peer" );
		peer2 = json_string( opt, "peer2" );
		peer3 = json_string( opt, "peer3" );
		peermac = json_string( opt, "peermac" );
	}
	v = NULL;
	if ( (peer != NULL && *peer != '\0')
		|| (peer2 != NULL && *peer != '\0')
		|| (peer2 != NULL && *peer != '\0')
		|| (peermac != NULL && *peer != '\0') )
	{
		v = opt;
	}
	else
	{
		v = cfg;
		peer = json_string( cfg, "peer" );
		peer2 = json_string( cfg, "peer2" );
		peer3 = json_string( cfg, "peer3" );
		peermac = json_string( cfg, "peermac" );
		status = json_string( v, "status" );
	}

	/* keeplive stop */
	sstop( "%s-keeplive", netdev );
	/* relayd stop */
	sstop( "%s-relayd", netdev );
	/* wpa_supplicant stop */
	sstop( "%s-wpa", netdev );

	/* status[enable/disable] */
	if ( status != NULL && 0 == strcmp( status, "disable" ) )
	{
		talk_free( cfg );
		lock_close( fd );
		unlink( path );
		return ttrue;
	}
	/* peer */
	if ( ( peer == NULL || *peer == '\0' ) && ( peer2 == NULL || *peer2 == '\0' ) && ( peer == NULL || *peer3 == '\0' ) && ( peermac == NULL || *peermac == '\0' ) ) 
	{
		talk_free( cfg );
		lock_close( fd );
		unlink( path );
		return ttrue;
	}

	/* get all configure */
	ifname = json_string( v, "ifname" );
	peermode = json_string( v, "peermode" );
	channel = json_string( v, "channel" );
	nossid = json_string( v, "nossid" );
	/* save peer configure */
	secure = json_string( v, "secure" );
	wpa_encrypt = json_string( v, "wpa_encrypt" );
	wpa_key = json_string( v, "wpa_key" );
	reg_set_string( this, "peer", peer );
	reg_set_string( this, "secure", secure );
	reg_set_string( this, "wpa_encrypt", wpa_encrypt );
	reg_set_string( this, "wpa_key", wpa_key );
	/* save peer2 configure */
	secure = json_string( v, "secure2" );
	wpa_encrypt = json_string( v, "wpa_encrypt2" );
	wpa_key = json_string( v, "wpa_key2" );
	reg_set_string( this, "peer2", peer2 );
	reg_set_string( this, "secure2", secure );
	reg_set_string( this, "wpa_encrypt2", wpa_encrypt );
	reg_set_string( this, "wpa_key2", wpa_key );
	/* save peer3 configure */
	secure = json_string( v, "secure3" );
	wpa_encrypt = json_string( v, "wpa_encrypt3" );
	wpa_key = json_string( v, "wpa_key3" );
	reg_set_string( this, "peer3", peer3 );
	reg_set_string( this, "secure3", secure );
	reg_set_string( this, "wpa_encrypt3", wpa_encrypt );
	reg_set_string( this, "wpa_key3", wpa_key );
	/* save other configure */
	reg_set_string( this, "peermac", peermac );
	reg_set_string( this, "ifname", ifname );
	reg_set_string( this, "peermode", peermode );
	reg_set_string( this, "channel", channel );
	reg_set_string( this, "nossid", nossid );

	/* up the device */
	cstart( this, "wpa", NULL, "%s-wpa", netdev );
	sleep( 1 );
	cstart( this, "keeplive", NULL, "%s-keeplive", netdev );

	/* mark the up state */
	ptr = uptime_desc( NULL, 0 );
	if ( ptr != NULL )
	{
		write( fd, ptr, strlen(ptr) );
	}
	talk_free( cfg );
	lock_close( fd );
	return ttrue;
}
boole_t _connect( obj_t this, param_t param )
{
	return ttrue;
}
boole_t _connected( obj_t this, param_t param )
{
	talk_t ret;
	const char *netdev;

	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}

	/* test the connected */
	ret = tfalse;
	if ( station_dev_connected( this, netdev ) == 0 )
	{
		ret = ttrue;
	}

	return ret;
}
boole_t _down( obj_t this, param_t param )
{
	const char *object;
	const char *netdev;
    char path[PATH_MAX];

	object = obj_name( this );
	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	/* delete the keeplive */
	sdelete( "%s-keeplive", netdev );
	/* delete the relayd */
	sdelete( "%s-relayd", netdev );
	/* delete wpa_supplicant */
	sdelete( "%s-wpa", netdev );
    /* down the deivce */
	if ( netdev_flags( netdev, IFF_BROADCAST ) > 0 )
	{
		wifi_debug( "%s(%s) down", object, netdev );
		//ifconfig( "%s down", netdev );
	}
	/* delete the mark file */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	unlink( path );

	return ttrue;
}



boole_t _reset( obj_t this, param_t param )
{
	int reset_times;
	const char *object;
	const char *netdev;
    char path[PATH_MAX];

	object = obj_name( this );
	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return tfalse;
	}
	/* record the count */
	reset_times = reg_int( this, "reset_times" );
	wifi_fault( "%s reset %d times", object, reset_times+1 );
	reset_times++;
	reg_set_int( this, "reset_times", reset_times );
	/* delete the keeplive */
	sdelete( "%s-keeplive", netdev );
	/* delete the relayd */
	sdelete( "%s-relayd", netdev );
	/* delete wpa_supplicant */
	sdelete( "%s-wpa", netdev );
	/* delete the mark file */
	var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
	unlink( path );
	
	return ttrue;
}



talk_t _status( obj_t this, param_t param )
{
	talk_t ret;
	talk_t cfg;
	const char *netdev;
    char path[PATH_MAX];

	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return NULL;
	}

    ret = json_create( NULL );
	/* get the state */
	if ( netdev_flags( netdev, IFF_BROADCAST ) <= 0 )
	{
		json_set_string( ret, "status", "nodevice" );
		talk_free( cfg );
		return ret;
	}
	/* state get */
	if ( netdev_flags( netdev, IFF_UP ) <= 0 )
	{
		json_set_string( ret, "status", "down" );
		talk_free( cfg );
		return ret;
	}
	snprintf( path, sizeof(path), "%s-wpa", netdev );
	if ( spid( path ) >= 0 )
	{
		json_set_string( ret, "status", "uping" );
	}
	else
	{
		json_set_string( ret, "status", "down" );
	}

	{
		int i;
		FILE *fp;
		char *ptr;
		char tok[64];
		char readbuf[256];
		char path[PATH_MAX];
		
		/* get the command result */	
		snprintf( path, sizeof(path), "/tmp/.wpa_cli_%s_status", netdev );
		shell( "wpa_cli -i %s status > %s", netdev, path );
		/* parse the wpa_cli */
		fp = fopen( path, "r");
		if( fp != NULL )
		{
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				i = strlen( readbuf );
				readbuf[i-1] = '\0';
				if ( strncmp( readbuf, "wpa_state=", 10 ) == 0 )
				{
					ptr = readbuf+10;
					if ( strcmp( ptr, "SCANNING" ) == 0 )
					{
						json_set_string( ret, "state", "scanning" );
					}
					else if ( strcmp( ptr, "COMPLETED" ) == 0 )
					{
						json_set_string( ret, "status", "up" );
					}
				}
				else if ( strncmp( readbuf, "ssid=", 5 ) == 0 )
				{
					ptr = readbuf+5;
					json_set_string( ret, "peer", ptr );
				}
				else if ( strncmp( readbuf, "bssid=", 6 ) == 0 )
				{
					ptr = readbuf+6;
					low2upp( ptr );
					json_set_string( ret, "peermac", ptr );
				}
				else if ( strncmp( readbuf, "address=", 8 ) == 0 )
				{
					ptr = readbuf+8;
					low2upp( ptr );
					json_set_string( ret, "mac", ptr );
				}
			}
			fclose( fp );
		}
		/* parse the iwinfo */
		snprintf( path, sizeof(path), "/tmp/.iwinfo_%s_info", netdev );
		shell( "iwinfo %s info > %s", netdev, path );
		fp = fopen( path, "r");
		if( fp != NULL )
		{
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				if ( strstr( readbuf, "Signal:" ) )
				{
					int rssi = 0;
					int sinr = 0;
					i = sscanf( readbuf, "%*[^:]: %d %*[^:]: %d", &rssi, &sinr );
					if ( i >= 1 )
					{
						if ( rssi != 0 )
						{
							json_set_number( ret, "rssi", rssi );
							i = dbm2signal( rssi );
							json_set_number( ret, "signal", i );
						}
						if ( sinr != 0 )
						{
							json_set_number( ret, "sinr", sinr );
						}
					}
				}
				else if ( strstr( readbuf, "Bit Rate:" ) )
				{
					i = sscanf( readbuf, "%*[^:]: %s", tok );
					if ( i == 1 && 0 != strcasecmp( tok, "unknown" ) )
					{
						json_set_string( ret, "rate", tok );
					}
				}
				else if ( (ptr = strstr( readbuf, "Channel:" ) ) != NULL )
				{
					int ch = 0;
					i = sscanf( ptr, "%*[^:]: %d", &ch );
					if ( i == 1 )
					{
						json_set_number( ret, "channel", ch );
					}
				}
			}
			fclose( fp );
		}
	}

	talk_free( cfg );
	return ret;
}
boole_t _online( obj_t this, param_t param )
{
	int reset_times;
	const char *object;
	const char *netdev;
	const char *ifname;

	object = obj_name( this );
	/* clear the count */
	reset_times = 0;
	reg_set_int( this, "reset_times", reset_times );
	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}
	/* reset the relayd for in bridge */
	ifname = param_string( param, 1 );
	if ( ifname != NULL && strstr( ifname, LAN_COM ) != NULL )
	{
		wifi_info( "%s(%s) online reset the relayd", object, netdev );
		sreset( NULL, NULL, NULL, "%s-relayd", netdev );
	}
	return ttrue;
}
boole_t _offline( obj_t this, param_t param )
{
	const char *netdev;
	const char *ifname;

	/* get the netdev */
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}
	/* delete the relayd for in bridge */
	ifname = param_string( param, 1 );
	if ( ifname != NULL && strstr( ifname, LAN_COM ) != NULL )
	{
		sdelete( "%s-relayd", netdev );
	}
	return ttrue;
}



talk_t _aplist( obj_t this, param_t param )
{
	talk_t ret;
    const char *radio;
	const char *netdev;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;

	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return NULL;
	}
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return NULL;
	}

	/* get the paramter */
	peer = param_string( param, 1 );
	peermac = param_string( param, 2 );
	peer2 = param_string( param, 3 );
	peer3 = param_string( param, 4 );

	/* stop the hostapd to update the channel */
	//sstop( "%s-hostapd", radio );

	/* scanning */
	ret = station_dev_aplist( netdev, peer, peermac, peer2, peer3 );

	/* start the hostapd */
	//sstart( NULL, "NULL", NULL, "%s-hostapd", radio );

	return ret;
}
talk_t _chlist( obj_t this, param_t param )
{
	const char *radio;

	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return NULL;
	}
	return scall( radio, "chlist", param );
}
talk_t _securelist( obj_t this, param_t param )
{
	const char *radio;

	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return NULL;
	}
	return scall( radio, "securelist", param );
}



boole_t _wpa( obj_t this, param_t param )
{
	FILE *fp;
    const char *radio;
    const char *netdev;
	const char *peer;
	const char *peer2;
	const char *peer3;
	const char *peermac;
	const char *secure;
	const char *wpa_key;
	char path[PATH_MAX];
	char pidfile[PATH_MAX];
	const char *hostapdctl = NULL;
	char *wap_dir = "/var/run/wpa_supplicant";

	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return terror;
	}
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}

	/* get the hostapd ctrl file */
	hostapdctl = reg_sstring( radio, "ctl" );
    /* get the configure */
	project_var_path( pidfile, sizeof(pidfile), PROJECT_ID, "wpa_supplicant_%s.pid", netdev );
    project_var_path( path, sizeof(path), PROJECT_ID, "wpa_supplicant_%s.conf", netdev );
    fp = fopen( path, "w" );
    if( fp == NULL )
    {
        wifi_faulting( "fopen error on %s write", path );
		return terror;
    }
    fprintf( fp, "ap_scan=1\n" );
    fprintf( fp, "fast_reauth=1\n" );
	
    /* set the peer */
	peer = reg_string( this, "peer" );
	peer2 = reg_string( this, "peer2" );
	peer3 = reg_string( this, "peer3" );
	peermac = reg_string( this, "peermac" );
	if ( peer != NULL && *peer != '\0' )
	{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		secure = reg_string( this, "secure" );
		wpa_key = reg_string( this, "wpa_key" );
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=1\n" );
	    fprintf( fp, "}\n" );
	}
    /* set the peer2 */
	if ( peer2 != NULL && *peer2 != '\0' )
	{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer3 );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		secure = reg_string( this, "secure2" );
		wpa_key = reg_string( this, "wpa_key2" );
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}	
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=2\n" );
	    fprintf( fp, "}\n" );
	}
    /* set the peer3 */
	if ( peer3 != NULL && *peer3 != '\0' )
	{
	    fprintf( fp, "network={\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "scan_ssid=1\n" );
	    fprintf( fp, "\t\t");
	    fprintf( fp, "ssid=\"%s\"\n", peer3 );
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "bssid=%s\n", peermac );
		}
		secure = reg_string( this, "secure3" );
		wpa_key = reg_string( this, "wpa_key3" );
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=WPA\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "proto=RSN\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t");
			fprintf( fp, "psk=\"%s\"\n", wpa_key );
		}
		else
		{
			fprintf( fp, "\t\t");
			fprintf( fp, "key_mgmt=NONE\n" );
		}	
		fprintf( fp, "\t\t");
		fprintf( fp, "priority=2\n" );
	    fprintf( fp, "}\n" );
	}
    fclose( fp );

	/* stop the hostapd to update the channel */
	hostapdctl = NULL;
	sstop( "%s-hostapd", radio );

    /* exec the wpa_supplicant */
#if defined gPLATFORM__smtk || defined gPLATFORM__mtk || defined gPLATFORM__smtk2 || defined gPLATFORM__mtk2
	if ( hostapdctl == NULL )
	{
		wifi_debug( "wpa_supplicant -D nl80211 -i %s -c %s -P %s -C %s", netdev, path, pidfile, wap_dir );
    	execlp( "wpa_supplicant", "wpa_supplicant", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", wap_dir, (char *)0 );
	}
	else
	{
		wifi_debug( "wpa_supplicant -D nl80211 -i %s -c %s -P %s -C %s -H %s", netdev, path, pidfile, wap_dir, hostapdctl );
    	execlp( "wpa_supplicant", "wpa_supplicant", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", wap_dir, "-H", hostapdctl, (char *)0 );
	}
#else
	if ( hostapdctl == NULL )
	{
		wifi_debug( "wpa_supplicant -s -D nl80211 -i %s -c %s -P %s -C %s", netdev, path, pidfile, wap_dir );
		execlp( "wpa_supplicant", "wpa_supplicant", "-s", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", wap_dir, (char *)0 );
	}
	else
	{
		wifi_debug( "wpa_supplicant -s -D nl80211 -i %s -c %s -P %s -C %s -H %s", netdev, path, pidfile, wap_dir, hostapdctl );
		execlp( "wpa_supplicant", "wpa_supplicant", "-s", "-D", "nl80211", "-i", netdev, "-c", path, "-P", pidfile, "-C", wap_dir, "-H", hostapdctl, (char *)0 );
	}
#endif
    wifi_faulting( "run the wpa_supplicant error" );

    return tfalse;
}
boole_t _relayd( obj_t this, param_t param )
{
	const char *ptr;
    const char *object;
	const char *netdev;
	const char *gateway;
	char ip[NAME_MAX];
	char ifname[NAME_MAX];
	char bridge[NAME_MAX];
	char bridge_netdev[NAME_MAX];

	object = obj_name( this );
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}
	/* brdige id */
	ptr = scalls_string( bridge, sizeof(bridge), BRIDGE_COM, "bridge", object );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* brdige netdev */
	ptr = scalls_string( bridge_netdev, sizeof(bridge_netdev), bridge, "netdev", NULL );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* brdige ifname */
	ptr = scalls_string( ifname, sizeof(ifname), NETWORK_COM, "ifname", bridge );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* get the gateway when have */
	gateway = reg_sstring( ifname, "gateway" );
	/* brdige status */
	ip[0] = '\0';
	netdev_info( bridge_netdev, ip, sizeof(ip), NULL, 0, NULL, 0, NULL, 0 );
	sleep( 3 );

	/* run the relayd */
	if ( ip[0] != '\0' && gateway != NULL && gateway[0] != '\0' )
	{
		wifi_info( "%s(%s) relayd use %s on %s", object, netdev, gateway, ip );
		execlp( "relayd", "relayd", "-I", netdev, "-I", bridge_netdev, "-B", "-D", "-L", ip, "-G", gateway, (char*)0 );
	}
	else if ( ip[0] != '\0' )
	{
		wifi_info( "%s(%s) relayd on %s", object, netdev, ip );
		execlp( "relayd", "relayd", "-I", netdev, "-I", bridge_netdev, "-B", "-D", "-L", ip, (char*)0 );
	}
	else if ( gateway != NULL && gateway[0] != '\0' )
	{
		wifi_info( "%s(%s) relayd use %s", object, netdev, gateway );
		execlp( "relayd", "relayd", "-I", netdev, "-I", bridge_netdev, "-B", "-D", "-G", gateway, (char*)0 );
	}
	else
	{
		wifi_info( "%s(%s) relayd", object, netdev );
		execlp( "relayd", "relayd", "-I", netdev, "-I", bridge_netdev, "-B", "-D", (char*)0 );
	}
	wifi_faulting( "execlp the relayd(%s) error" , "relayd" );

	return tfalse;
}
boole_t _keeplive( obj_t this, param_t param )
{
	int i;
	talk_t v;
	const char *radio;
    const char *object;
	const char *netdev;
	const char *ifname;
	const char *nossid;
	const char *channel;
	char path[PATH_MAX];

	object = obj_name( this );
	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return terror;
	}
	netdev = reg_string( this, "netdev" );
	if ( netdev == NULL || *netdev == '\0' )
	{
		return terror;
	}

	/* get the ifname */
	ifname = reg_string( this, "ifname" );
	/* get the ssid_disable */
	nossid = reg_string( this, "nossid" );
    /* first check */
    wifi_debug( "check %s(%s) connect state", object, netdev );
    for ( i=0; i<90; i++ )
    {
        if ( station_dev_connected( this, netdev ) == 0 )
        {
            break;
        }
		wifi_debug( "%s(%s) connecting", object, netdev );
		sleep( 1 );
    }
	if ( i >= 90 )
	{
		wifi_warn( "%s(%s) connecting timeout", object, netdev );
		goto reset;
	}
    wifi_info( "%s(%s) connect succeed", object, netdev );
	/* run the relayd */
	sstart( object, "relayd", NULL, "%s-relayd", netdev );

	/* get the channel */
	v = _status( this, param );
	channel = json_string( v, "channel" );
	if ( channel != NULL )
	{
		reg_sset_string( radio, "channel", channel );
	}
	talk_free( v );

	/* start the hostapd */
	if ( nossid == NULL || 0 != strcmp( nossid, "enable" ) )
	{
		sleep( 3 );
		sstart( NULL, NULL, NULL, "%s-hostapd", radio );
	}

	/* check and check forever */
	while( 1 )
	{
		if ( station_dev_connected( this, netdev ) == 0 )
		{
			sleep( 5 );
		}
		else
		{
			wifi_info( "%s(%s) connection lost 1 time", object, netdev );
			sleep( 2 );
			if ( station_dev_connected( this, netdev ) == 0 )
			{
				continue;
			}
			wifi_info( "%s(%s) connection lost 2 time", object, netdev );
			sleep( 2 );
			if ( station_dev_connected( this, netdev ) == 0 )
			{
				continue;
			}
			wifi_info( "%s(%s) connection lost 3 time", object, netdev );
			sleep( 2 );
			if ( station_dev_connected( this, netdev ) == 0 )
			{
				continue;
			}
			wifi_info( "%s(%s) connection lost 4 time", object, netdev );
			sleep( 2 );
			if ( station_dev_connected( this, netdev ) == 0 )
			{
				continue;
			}
			wifi_info( "%s(%s) connection lost 4 time", object, netdev );
			sleep( 2 );
			if ( station_dev_connected( this, netdev ) == 0 )
			{
				continue;
			}
			wifi_info( "%s(%s) connection broken", object, netdev );
			break;
		}
	}

reset:
	wifi_info( "%s(%s) reconnect", object, netdev );
	/* stop the relayd */
	sstop( "%s-relayd", netdev );
	/* stop wpa_supplicant */
	sstop( "%s-wpa", netdev );
	/* reset the wisp ifname when wisp */
	if ( ifname != NULL && strstr( ifname, WISP_COM ) != NULL )
	{
		/* delete the mark file */
		var2path( path, sizeof(path), "%s-%s.up", COM_ID, netdev );
		unlink( path );
		sreset( NULL, NULL, NULL, ifname );
		return ttrue;
	}
	/* restart the wpa_supplicant when lan bridge */
	sstart( NULL, NULL, NULL, "%s-wpa", netdev );

	return tfalse;
}



