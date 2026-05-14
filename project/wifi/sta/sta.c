/*
 *  Description:  wireless station management
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"
#define STA_PEER_MAX 3



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

	if ( netdev == NULL || *netdev == '\0' )
	{
		return false;
	}

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
			if ( strlen( ptr ) < 17 )
			{
				continue;
			}
			ptr[17] = '\0';
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
				if ( i >= 2 )
				{
					ptr[i-2] = '\0';
				}
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
				/* wpapskwpa2psk tkip (iwinfo: mixed ... TKIP) */
				else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (TKIP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "tkip" );
				}
				/* wpapskwpa2psk aes (iwinfo: mixed ... CCMP) */
				else if ( strstr( readbuf, "mixed WPA/WPA2 PSK (CCMP)" ) != NULL )
				{
					json_set_string( x, "secure", "wpapskwpa2psk" );
					json_set_string( x, "wpa_encrypt", "aes" );
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
		if ( v != NULL )
		{
			if ( json_number( v, "rssi" ) != 0 )
			{
				return v;
			}
			talk_free( v );
		}
		return NULL;
    }

	last = -200;
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
		if ( result == NULL )
		{
			return NULL;
		}
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
	char *end;
	char path[256];
    char readbuf[256];

	ret = 1;
	/* use iw sta link */	
	snprintf( path, sizeof(path), "/tmp/.iw_%s_link", netdev );
	shell( "iw dev %s link > %s", netdev, path );
	/* parse the iw sta link */
	fp = fopen( path, "r");
	if( fp == NULL )
	{
		return 1;
	}
	readbuf[0] = '\0';
	while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
	{
		if ( NULL != ( ptr = strstr( readbuf, "Connected to" ) ) )
		{
			ptr += 13;
			end = strchr( ptr, ' ' );
			if ( end != NULL )
			{
				*end = '\0';
			}
			if ( strlen( ptr ) == 17 )
			{
				ret = 0;
				break;
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
	int i;
	talk_t v;
	talk_t cfg;
	talk_t opt;
	char *netdev;
	const char *radio;
	const char *object;
	const char *ifname;
	const char *status;
	const char *peer;
	const char *peermac;
	const char *nossid;
	char kmac[48];
	char kpeer[48];
	char ksecure[48];
	char kwkey[48];
	char kwenc[48];
	char kpmode[48];

	/* get the configure */
	object = obj_name( this );
	radio = reg_string( this, "radio" );
	if ( radio == NULL || *radio == '\0' )
	{
		return terror;
	}
	netdev = register_pointer( this, "netdev" );
	if ( netdev == NULL )
	{
		return terror;
	}
	if ( *netdev == '\0' )
	{
		return terror;
	}
	cfg = config_get( this, NULL );
	if ( cfg == NULL )
	{
		return terror;
	}
	/* wait if another process is in up/down; then run this up */
	if ( register_lockw( this, netdev ) != true )
	{
		wifi_faulting( "%s(%s) register_lockw failed", object, netdev );
		talk_free( cfg );
		return tfalse;
	}
	/* keeplive stop */
	sstop( "%s-keeplive", netdev );
	/* relayd stop */
	sstop( "%s-relayd", netdev );
	/* wpa_supplicant stop */
	sstop( "%s-wpa", netdev );

	wifi_info( "%s(%s) up", object, netdev );
	/* get the configure: prefer opt when any peer* or peermac is set there */
	opt = param_talk( param, 1 );
	v = NULL;
	if ( opt != NULL )
	{
		for ( i=0; i<STA_PEER_MAX; i++ )
		{
			if ( i == 0 )
			{
				snprintf( kpeer, sizeof(kpeer), "peer" );
				snprintf( kmac, sizeof(kmac), "peermac" );
				snprintf( ksecure, sizeof(ksecure), "secure" );
				snprintf( kwkey, sizeof(kwkey), "wpa_key" );
				snprintf( kwenc, sizeof(kwenc), "wpa_encrypt" );
				snprintf( kpmode, sizeof(kpmode), "peermode" );
			}
			else
			{
				snprintf( kpeer, sizeof(kpeer), "peer%d", i+1 );
				snprintf( kmac, sizeof(kmac), "peermac%d", i+1 );
				snprintf( ksecure, sizeof(ksecure), "secure%d", i+1 );
				snprintf( kwkey, sizeof(kwkey), "wpa_key%d", i+1 );
				snprintf( kwenc, sizeof(kwenc), "wpa_encrypt%d", i+1 );
				snprintf( kpmode, sizeof(kpmode), "peermode%d", i+1 );
			}
			peer = json_string( opt, kpeer );
			peermac = json_string( opt, kmac );
			if ( (peer != NULL && *peer != '\0') || (peermac != NULL && *peermac != '\0') )
			{
				break;
			}
		}
		if ( i < STA_PEER_MAX )
		{
			v = opt;
		}
	}
	if ( v == NULL )
	{
		v = cfg;
		status = json_string( v, "status" );
		if ( status != NULL && 0 == strcmp( status, "disable" ) )
		{
			talk_free( cfg );
			register_unlock( this, netdev );
			return terror;
		}
		for ( i=0; i<STA_PEER_MAX; i++ )
		{
			if ( i == 0 )
			{
				snprintf( kpeer, sizeof(kpeer), "peer" );
				snprintf( kmac, sizeof(kmac), "peermac" );
				snprintf( ksecure, sizeof(ksecure), "secure" );
				snprintf( kwkey, sizeof(kwkey), "wpa_key" );
				snprintf( kwenc, sizeof(kwenc), "wpa_encrypt" );
				snprintf( kpmode, sizeof(kpmode), "peermode" );
			}
			else
			{
				snprintf( kpeer, sizeof(kpeer), "peer%d", i+1 );
				snprintf( kmac, sizeof(kmac), "peermac%d", i+1 );
				snprintf( ksecure, sizeof(ksecure), "secure%d", i+1 );
				snprintf( kwkey, sizeof(kwkey), "wpa_key%d", i+1 );
				snprintf( kwenc, sizeof(kwenc), "wpa_encrypt%d", i+1 );
				snprintf( kpmode, sizeof(kpmode), "peermode%d", i+1 );
			}
			peer = json_string( v, kpeer );
			peermac = json_string( v, kmac );
			if ( (peer != NULL && *peer != '\0') || (peermac != NULL && *peermac != '\0') )
			{
				break;
			}
		}
		if ( i >= STA_PEER_MAX )
		{
			wifi_warn( "%s(%s) no peer settings", object, netdev );
			talk_free( cfg );
			register_unlock( this, netdev );
			return terror;
		}
	}

	/* get all configure */
	ifname = json_string( v, "ifname" );
	nossid = json_string( v, "nossid" );
	for ( i=0; i<STA_PEER_MAX; i++ )
	{
		if ( i == 0 )
		{
			snprintf( kpeer, sizeof(kpeer), "peer" );
			snprintf( kmac, sizeof(kmac), "peermac" );
			snprintf( ksecure, sizeof(ksecure), "secure" );
			snprintf( kwkey, sizeof(kwkey), "wpa_key" );
			snprintf( kwenc, sizeof(kwenc), "wpa_encrypt" );
			snprintf( kpmode, sizeof(kpmode), "peermode" );
		}
		else
		{
			snprintf( kpeer, sizeof(kpeer), "peer%d", i+1 );
			snprintf( kmac, sizeof(kmac), "peermac%d", i+1 );
			snprintf( ksecure, sizeof(ksecure), "secure%d", i+1 );
			snprintf( kwkey, sizeof(kwkey), "wpa_key%d", i+1 );
			snprintf( kwenc, sizeof(kwenc), "wpa_encrypt%d", i+1 );
			snprintf( kpmode, sizeof(kpmode), "peermode%d", i+1 );
		}
		reg_set_string( this, kpeer, json_string( v, kpeer ) );
		reg_set_string( this, kmac, json_string( v, kmac ) );
		reg_set_string( this, ksecure, json_string( v, ksecure ) );
		reg_set_string( this, kwenc, json_string( v, kwenc ) );
		reg_set_string( this, kwkey, json_string( v, kwkey ) );
		reg_set_string( this, kpmode, json_string( v, kpmode ) );
	}
	reg_set_string( this, "ifname", ifname );
	reg_set_string( this, "nossid", nossid );

	/* up the device */
	cstart( this, "wpa", NULL, "%s-wpa", netdev );
	sleep( 1 );
	cstart( this, "keeplive", NULL, "%s-keeplive", netdev );

	register_unlock( this, netdev );

	talk_free( cfg );
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
	char *netdev;

	object = obj_name( this );
	/* get the netdev */
	netdev = register_pointer( this, "netdev" );
	if ( netdev == NULL )
	{
		return terror;
	}
	if ( *netdev == '\0' )
	{
		return terror;
	}

	/* wait if another process is in up/down; then run this down */
	if ( register_lockw( this, netdev ) != true )
	{
		wifi_faulting( "%s(%s) register_lockw failed", object, netdev );
		return tfalse;
	}

	/* delete the keeplive */
	sdelete( "%s-keeplive", netdev );
	/* delete the relayd */
	sdelete( "%s-relayd", netdev );
	/* delete wpa_supplicant */
	sdelete( "%s-wpa", netdev );
	/* down the device */
	if ( netdev_flags( netdev, IFF_BROADCAST ) > 0 )
	{
		wifi_debug( "%s(%s) down", object, netdev );
		ifconfig( "%s down", netdev );
	}

	register_unlock( this, netdev );

	return ttrue;
}



boole_t _reset( obj_t this, param_t param )
{
	int reset_times;
	const char *object;
	char *netdev;

	object = obj_name( this );
	/* get the netdev */
	netdev = register_pointer( this, "netdev" );
	if ( netdev == NULL )
	{
		return terror;
	}
	if ( *netdev == '\0' )
	{
		return terror;
	}

	/* wait if another process is in up/down; then run this down */
	if ( register_lockw( this, netdev ) != true )
	{
		wifi_faulting( "%s(%s) register_lockw failed", object, netdev );
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

	register_unlock( this, netdev );

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
		int bandwidth_done;
		FILE *fp;
		char *ptr;
		char *end;
		char tok[64];
		char readbuf[256];
		char path[PATH_MAX];

		/* use iw sta link */	
		snprintf( path, sizeof(path), "/tmp/.iw_%s_link", netdev );
		shell( "iw dev %s link > %s", netdev, path );
		/* parse the iw sta link */
		fp = fopen( path, "r");
		if( fp != NULL )
		{
			bandwidth_done = 0;
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				i = strlen( readbuf );
				if ( i > 0 && readbuf[i-1] == '\n' )
				{
					readbuf[i-1] = '\0';
				}
				if ( NULL != ( ptr = strstr( readbuf, "Connected to" ) ) )
				{
					ptr += 13;
					end = strchr( ptr, ' ' );
					if ( end != NULL )
					{
						*end = '\0';
					}
					low2upp( ptr );
					if ( strlen( ptr ) == 17 )
					{
						json_set_string( ret, "peermac", ptr );
						json_set_string( ret, "status", "up" );
					}
				}
				else if ( NULL != ( ptr = strstr( readbuf, "SSID: " ) ) )
				{
					ptr += 6;
					json_set_string( ret, "peer", ptr );
				}
				else if ( NULL != ( ptr = strstr( readbuf, "beacon interval:" ) ) )
				{
					ptr += 16;
					while ( *ptr == ' ' || *ptr == '\t' )
					{
						ptr++;
					}
					if ( sscanf( ptr, "%63[^ \t\n]", tok ) == 1 )
					{
						json_set_string( ret, "beacon", tok );
					}
				}
				else if ( NULL != ( ptr = strstr( readbuf, "beacon int:" ) ) )
				{
					ptr += 11;
					while ( *ptr == ' ' || *ptr == '\t' )
					{
						ptr++;
					}
					if ( sscanf( ptr, "%63[^ \t\n]", tok ) == 1 )
					{
						json_set_string( ret, "beacon", tok );
					}
				}
				else if ( bandwidth_done == 0 && NULL != strstr( readbuf, "rx bitrate:" ) )
				{
					if ( NULL != strstr( readbuf, "160MHz" ) )
					{
						json_set_string( ret, "bandwidth", "160" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "80MHz" ) )
					{
						json_set_string( ret, "bandwidth", "80" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "40MHz" ) )
					{
						json_set_string( ret, "bandwidth", "40" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "20MHz" ) )
					{
						json_set_string( ret, "bandwidth", "20" );
						bandwidth_done = 1;
					}
				}
				else if ( bandwidth_done == 0 && NULL != strstr( readbuf, "tx bitrate:" ) )
				{
					if ( NULL != strstr( readbuf, "160MHz" ) )
					{
						json_set_string( ret, "bandwidth", "160" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "80MHz" ) )
					{
						json_set_string( ret, "bandwidth", "80" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "40MHz" ) )
					{
						json_set_string( ret, "bandwidth", "40" );
						bandwidth_done = 1;
					}
					else if ( NULL != strstr( readbuf, "20MHz" ) )
					{
						json_set_string( ret, "bandwidth", "20" );
						bandwidth_done = 1;
					}
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
			int ch = 0;
			int rssi = 0;
			int sinr = 0;
			readbuf[0] = '\0';
			while( fgets( readbuf, sizeof(readbuf)-1, fp ) != NULL )
			{
				if ( NULL != ( ptr = strstr( readbuf, "Channel:" ) ) )
				{
					i = sscanf( ptr, "%*[^:]: %d", &ch );
					if ( i == 1 )
					{
						json_set_number( ret, "channel", ch );
					}
				}
				else if ( NULL != ( ptr = strstr( readbuf, "Signal:" ) ) )
				{
					i = sscanf( ptr, "%*[^:]: %d %*[^:]: %d", &rssi, &sinr );
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
				else if ( NULL != ( ptr = strstr( readbuf, "Bit Rate:" ) ) )
				{
					i = sscanf( ptr, "%*[^:]: %63s", tok );
					if ( i == 1 && 0 != strcasecmp( tok, "unknown" ) )
					{
						json_set_string( ret, "rate", tok );
					}
					break;
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

	/* get the parameter */
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
	int i;
	FILE *fp;
	char kmac[48];
	char kpeer[48];
	char ksecure[48];
	char kwkey[48];
	char kwenc[48];
	char kpmode[48];
	char path[PATH_MAX];
	char pidfile[PATH_MAX];
    const char *radio;
    const char *netdev;
	const char *peer;
	const char *peermac;
	const char *peermode;
	const char *secure;
	const char *wpa_encrypt;
	const char *wpa_key;
	const char *pairwise;
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

	/* stop the hostapd to update the channel */
	scall( radio, "stop_hostapd", NULL );
	//hostapdctl = reg_sstring( radio, "ctl" );

	/* make the configure for mutil-peer */
	for ( i = 0; i < STA_PEER_MAX; i++ )
	{
		if ( i == 0 )
		{
			snprintf( kpeer, sizeof(kpeer), "peer" );
			snprintf( kmac, sizeof(kmac), "peermac" );
			snprintf( ksecure, sizeof(ksecure), "secure" );
			snprintf( kwkey, sizeof(kwkey), "wpa_key" );
			snprintf( kwenc, sizeof(kwenc), "wpa_encrypt" );
			snprintf( kpmode, sizeof(kpmode), "peermode" );
		}
		else
		{
			snprintf( kpeer, sizeof(kpeer), "peer%d", i+1 );
			snprintf( kmac, sizeof(kmac), "peermac%d", i+1 );
			snprintf( ksecure, sizeof(ksecure), "secure%d", i+1 );
			snprintf( kwkey, sizeof(kwkey), "wpa_key%d", i+1 );
			snprintf( kwenc, sizeof(kwenc), "wpa_encrypt%d", i+1 );
			snprintf( kpmode, sizeof(kpmode), "peermode%d", i+1 );
		}
		peer = reg_string( this, kpeer );
		peermac = reg_string( this, kmac );
		if ( (peer == NULL || *peer == '\0') && (peermac == NULL || *peermac == '\0') )
		{
			continue;
		}
		fprintf( fp, "network={\n" );
		if ( peer != NULL && *peer != '\0' )
		{
			fprintf( fp, "\t\t ssid=\"%s\"\n", peer );
		}
		if ( peermac != NULL && *peermac != '\0' )
		{
			fprintf( fp, "\t\t bssid=%s\n", peermac );
		}
		peermode = reg_string( this, kpmode );
		if ( peermode != NULL && 0 == strcmp( peermode, "hidden" ) )
		{
			fprintf( fp, "\t\t scan_ssid=1\n" );
		}
		else
		{
			fprintf( fp, "\t\t scan_ssid=1\n" );
		}
		secure = reg_string( this, ksecure );
		wpa_key = reg_string( this, kwkey );
		wpa_encrypt = reg_string( this, kwenc );
		if ( secure != NULL && 0 == strcmp( secure, "wpapsk" ) )
		{
			fprintf( fp, "\t\t key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t proto=WPA\n" );
			fprintf( fp, "\t\t psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2psk" ) )
		{
			fprintf( fp, "\t\t key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t proto=RSN\n" );
			fprintf( fp, "\t\t psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpapskwpa2psk" ) )
		{
			fprintf( fp, "\t\t key_mgmt=WPA-PSK\n" );
			fprintf( fp, "\t\t psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa3psk" ) )
		{
			fprintf( fp, "\t\t key_mgmt=SAE\n" );
			fprintf( fp, "\t\t proto=RSN\n" );
			fprintf( fp, "\t\t pairwise=CCMP\n" );
			fprintf( fp, "\t\t ieee80211w=2\n" );
			fprintf( fp, "\t\t psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "wpa2pskwpa3psk" ) )
		{
			pairwise = "CCMP TKIP";
			if ( wpa_encrypt != NULL && 0 == strcmp( wpa_encrypt, "aes" ) )
			{
				pairwise = "CCMP";
			}
			if ( wpa_encrypt != NULL && 0 == strcmp( wpa_encrypt, "tkip" ) )
			{
				pairwise = "TKIP";
			}
			fprintf( fp, "\t\t key_mgmt=WPA-PSK SAE\n" );
			fprintf( fp, "\t\t proto=RSN\n" );
			fprintf( fp, "\t\t ieee80211w=1\n" );
			fprintf( fp, "\t\t pairwise=%s\n", pairwise );
			fprintf( fp, "\t\t psk=\"%s\"\n", wpa_key );
		}
		else if ( secure != NULL && 0 == strcmp( secure, "owe" ) )
		{
			fprintf( fp, "\t\t key_mgmt=OWE\n" );
			fprintf( fp, "\t\t proto=RSN\n" );
			fprintf( fp, "\t\t ieee80211w=2\n" );
			fprintf( fp, "\t\t pairwise=CCMP\n" );
		}
		else
		{
			fprintf( fp, "\t\t key_mgmt=NONE\n" );
		}
		fprintf( fp, "\t\t priority=%d\n", i == 0 ? 1 : 2 );
		fprintf( fp, "}\n" );
	}
    fclose( fp );

    /* exec the wpa_supplicant */
#if defined gPLATFORM__smtk2 || defined gPLATFORM__mtk2
	if ( hostapdctl == NULL || *hostapdctl == '\0' )
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
	if ( hostapdctl == NULL || *hostapdctl == '\0' )
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
	/* bridge id */
	ptr = scalls_string( bridge, sizeof(bridge), BRIDGE_COM, "bridge", object );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* bridge netdev */
	ptr = scalls_string( bridge_netdev, sizeof(bridge_netdev), bridge, "netdev", NULL );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* bridge ifname */
	ptr = scalls_string( ifname, sizeof(ifname), NETWORK_COM, "ifname", bridge );
	if ( ptr == NULL )
	{
		return ttrue;
	}
	/* get the gateway when have */
	gateway = reg_sstring( ifname, "gateway" );
	/* bridge status */
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
	const char *beacon;
	const char *channel;
	const char *bandwidth;

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
	if ( v != NULL )
	{
		bandwidth = json_string( v, "bandwidth" );
		if ( bandwidth != NULL )
		{
			reg_sset_string( radio, "bandwidth", bandwidth );
		}
		channel = json_string( v, "channel" );
		if ( channel != NULL )
		{
			reg_sset_string( radio, "channel", channel );
		}
		beacon = json_string( v, "beacon" );
		if ( beacon != NULL )
		{
			reg_sset_string( radio, "beacon", beacon );
		}
		talk_free( v );
	}

	/* start the hostapd */
	if ( nossid == NULL || 0 != strcmp( nossid, "enable" ) )
	{
		sleep( 2 );
		scall( radio, "start_hostapd", NULL );
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
			wifi_info( "%s(%s) connection lost 5 time", object, netdev );
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
		sreset( NULL, NULL, NULL, ifname );
		return ttrue;
	}
	/* reup the netdev */
	ifconfig( "%s down", netdev );
	ifconfig( "%s up", netdev );
	/* restart the wpa_supplicant when lan bridge */
	sstart( NULL, NULL, NULL, "%s-wpa", netdev );

	return tfalse;
}



