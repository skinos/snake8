/*
 *  Description:  skin modem library
 *       Author:  dimmalex (dim), dimmalex@gmail.com
 *      Company:  ASHYELF
 */

#include "skin/skin.h"
#include "skinwifi.h"



/* wifi a object name allocation */
const char *wifia_alloc( const char *syspath, const char *id, talk_t matchcfg, char *buf, int buflen )
{
	int i;
	talk_t v;
	talk_t cfg;
	talk_t axp;
	talk_t laxp;
	const char *ptr;;
	const char *string;
	const char *object;;
	char name[NAME_MAX];
	char path[PATH_MAX];
	static char buffer[NAME_MAX];

	if ( syspath == NULL )
	{
		return NULL;
	}
	if ( buf == NULL || buflen <= 0 )
	{
		buf = buffer;
		buflen = sizeof(buffer);
	}

	/* search already bind */
	var2path( path, sizeof(path), "name" );
	cfg = file2json( path );
	if ( cfg == NULL )
	{
		cfg = json_create( NULL );
	}
	axp = NULL;
	while( NULL != ( axp = json_next( cfg, axp ) ) )
	{
		object = axp_name( axp );
		string = axp_string( axp );
		if ( 0 == strcmp( string, syspath ) )
		{
			strncpy( buf, object, buflen-1 );
			buf[buflen-1] = '\0';
			talk_free( cfg );
			return buf;
		}
	}

	/* search the match config bind */
	if ( matchcfg != NULL && id != NULL )
	{
		axp = NULL;
		while( NULL != ( axp = json_next( matchcfg, axp ) ) )
		{
			object = axp_name( axp );
			v = axp_json( axp );
			if( v == NULL )
			{
				continue;
			}
			laxp = NULL;
			while( NULL != ( laxp = json_next( v, laxp ) ) )
			{
				ptr = axp_name( laxp );
				if ( 0 == strcmp( ptr, id ) )
				{
					json_set_string( cfg, object, syspath );
					json2file( cfg, path );
					talk_free( cfg );
					strncpy( buf, object, buflen-1 );
					buf[buflen-1] = '\0';
					return buf;
				}
			}
		}
	}

	/* search the for new bind */
	i = 0;
	while( i < WIFI_OBJECT_MAX )
	{
		if ( i == 0 )
		{
			snprintf( name, sizeof(name), "%s", ARADIO_COM );
		}
		else
		{
			snprintf( name, sizeof(name), "%s%d", ARADIO_COM, i+1 );
		}
		/* skip the already bind */
		if ( json_string( cfg, name ) != NULL )
		{
			i++;
			continue;
		}
		/* skip the match config */
		axp = NULL;
		if ( matchcfg != NULL )
		{
			while( NULL != ( axp = json_next( matchcfg, axp ) ) )
			{
				object = axp_name( axp );
				v = axp_json( axp );
				if( v == NULL )
				{
					continue;
				}
				if ( object != NULL && 0 == strcmp( object, name ) )
				{
					break;
				}
			}
		}
		if ( axp != NULL )
		{
			i++;
			continue;
		}
		json_set_string( cfg, name, syspath );
		json2file( cfg, path );
		talk_free( cfg );
		strncpy( buf, name, buflen-1 );
		buf[buflen-1] = '\0';
		return buf;
	}

	talk_free( cfg );
	return NULL;
}
/* wifi a object name free */
void        wifia_free( const char *object )
{
	talk_t cfg;
	char path[PATH_MAX];

	var2path( path, sizeof(path), "name" );
	cfg = file2json( path );
	if ( cfg == NULL )
	{
		return;
	}
	if ( json_delete_axp ( cfg, object ) == true )
	{
		json2file( cfg, path );
	}
	talk_free( cfg );
}



/* usb network device path list find, return >0 is find device number, return 0 for nofound*/
#define WIFIA_NETLIST_MAX 128
int wifia_netlist( const char *syspath, char device[][NAME_MAX] )
{
	int ret;
	DIR *pdir;
	struct stat st;
	char path[PATH_MAX];
	struct dirent *pent;

	if ( syspath == NULL || device == NULL )
	{
		return 0;
	}
	/* open net subdir */
	snprintf( path, sizeof(path), "%s/net", syspath );
	if ( stat( path, &st ) != 0 || S_ISDIR( st.st_mode ) == 0 )
	{
		return 0;
	}
	pdir = opendir( path );
	if ( pdir == NULL )
	{
		return 0;
	}
	ret = 0;
	/* subdir have the tty file */
	while( NULL != ( pent = readdir( pdir ) ) )
	{
		if ( *pent->d_name == '\0' || *pent->d_name == '.' )
		{
			continue;
		}
		if ( ret >= WIFIA_NETLIST_MAX )
		{
			break;
		}
		snprintf( device[ret], NAME_MAX, "%s", pent->d_name );
		ret++;
	}
	closedir( pdir );
	return ret;
}



typedef struct country_set_st
{
    const char *country;
    int n;
    int a;
    int a_bw_max_mhz;
} country_set_t;
static country_set_t country_list[] =
{
    /*
     * a_bw_max_mhz: 5 GHz 规则在 linux wireless-regdb(db.txt) 中各频段 @ 后的最大带宽(MHz)取最大值；
     * 无 5 GHz 条目时记 20。CN 按国内在售/核准常见能力记 160（主线 regdb 曾为 80，若内核未放宽需自配 regdb）。
     */
    {"DB", 5, 7, 160 },
    {"AL", 1, 0, 160 },
    {"DZ", 1, 0, 160 },

    {"AR", 1, 3, 160 },
    {"AM", 1, 2, 20 },
    {"AU", 1, 0, 160 },

    {"AT", 1, 1, 160 },
    {"AZ", 1, 2, 80 },
    {"BH", 1, 0, 20 },

    {"BY", 1, 0, 160 },
    {"BE", 1, 1, 160 },
    {"BZ", 1, 4, 80 },

    {"BO", 1, 4, 80 },
    {"BR", 1, 1, 160 },
    {"BN", 1, 4, 80 },

    {"BG", 1, 1, 160 },
    {"CA", 0, 0, 80 },
    {"CL", 1, 0, 80 },

    {"CN", 1, 0, 160 },
    {"CO", 0, 0, 160 },
    {"CR", 1, 0, 20 },

    {"HR", 1, 2, 160 },
    {"CY", 1, 1, 160 },
    {"CZ", 1, 2, 160 },

    {"DK", 1, 1, 160 },
    {"DO", 0, 0, 80 },
    {"EC", 1, 0, 20 },

    {"EG", 1, 2, 40 },
    {"SV", 1, 0, 20 },
    {"EE", 1, 1, 160 },

    {"FI", 1, 1, 160 },
    {"FR", 1, 2, 160 },
    {"GE", 1, 2, 80 },

    {"DE", 1, 1, 160 },
    {"GR", 1, 1, 160 },
    {"GT", 0, 0, 80 },

    {"HN", 1, 0, 160 },
    {"HK", 1, 0, 160 },
    {"HU", 1, 1, 160 },

    {"IS", 1, 1, 160 },
    {"IN", 1, 0, 160 },
    {"ID", 1, 4, 20 },

    {"IR", 1, 4, 80 },
    {"IE", 1, 1, 160 },
    {"IL", 1, 0, 80 },

    {"IT", 1, 1, 160 },
    {"JP", 5, 9, 160 },
    {"JO", 1, 0, 80 },

    {"KZ", 1, 0, 80 },
    {"KP", 1, 5, 20 },
    {"KR", 1, 5, 160 },

    {"KW", 1, 0, 80 },
    {"LV", 1, 1, 160 },
    {"LB", 1, 1, 160 },

    {"LI", 1, 1, 160 },
    {"LT", 1, 1, 160 },
    {"LU", 1, 1, 160 },

    {"MO", 1, 0, 160 },
    {"MK", 1, 0, 160 },
    {"MY", 1, 0, 160 },

    {"MX", 0, 0, 160 },
    {"MC", 1, 2, 160 },
    {"MA", 1, 0, 80 },

    {"NL", 1, 1, 160 },
    {"NZ", 1, 0, 160 },
    {"NO", 0, 0, 160 },

    {"OM", 1, 0, 160 },
    {"PK", 1, 0, 80 },
    {"PA", 0, 0, 160 },

    {"PE", 1, 4, 160 },
    {"PH", 1, 4, 160 },
    {"PL", 1, 1, 160 },

    {"PT", 1, 1, 160 },
    {"PR", 0, 0, 160 },
    {"QA", 1, 0, 80 },

    {"RO", 1, 0, 160 },
    {"RU", 1, 0, 80 },
    {"SA", 1, 0, 160 },

    {"SG", 1, 0, 160 },
    {"SK", 1, 1, 160 },
    {"SI", 1, 1, 160 },

    {"ZA", 1, 1, 160 },
    {"ES", 1, 1, 160 },
    {"SE", 1, 1, 160 },

    {"CH", 1, 1, 160 },
    {"SY", 1, 0, 20 },
    {"TW", 1, 3, 160 },

    {"TH", 1, 0, 160 },
    {"TT", 1, 2, 160 },
    {"TN", 1, 1, 80 },

    {"TR", 1, 2, 160 },
    {"UA", 1, 0, 160 },
    {"AE", 1, 0, 160 },

    {"GB", 1, 1, 160 },
    {"US", 0, 7, 160 },
    {"UY", 1, 5, 80 },

    {"UZ", 0, 1, 80 },
    {"VE", 1, 5, 80 },
    {"VN", 1, 0, 80 },

    {"YE", 1, 0, 20 },
    {"ZW", 1, 0, 160 },
    {NULL, 0, 0, 160 },
};
static int a_band[12][30] =
{
    {36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 0},
    {52, 56, 60, 64, 149, 153, 157, 161, 0},
    {149, 153, 157, 161, 165, 0},
    {149, 153, 157, 161, 0},
    {36, 40, 44, 48, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165, 0},
    {52, 56, 60, 64, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 149, 153, 157, 161, 165, 0},
    {36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165, 0}
};
static int n_band[8][20] =
{
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,0},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,0},
    {10, 11,0},
    {10, 11, 12, 13,0},
    {14,0},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,0},
    {3, 4, 5, 6, 7, 8, 9, 0},
    {5, 6, 7, 8, 9, 10, 11, 12, 13,0},
};
/*
 * 5 GHz 主信道一行一条：max_bw = 几何上该 primary 能到的最大带宽(MHz)；
 * seg40/seg80/seg160 = vht & he 的 oper_centr_freq_seg0_idx；he160_seg1 = 连续 160 MHz 时 he seg1(连续 160 为 "0")。
 */
typedef struct
{
	int ch;
	int max_bw_mhz;
	const char *seg40;
	const char *seg80;
	const char *seg160;
	const char *he160_seg1;
} a_chan_t;
static const a_chan_t a_chan[] = {
	{ 36, 160, "38", "42", "50", "0" },
	{ 40, 160, "38", "42", "50", "0" },
	{ 44, 160, "46", "42", "50", "0" },
	{ 48, 160, "46", "42", "50", "0" },
	{ 52, 160, "54", "58", "50", "0" },
	{ 56, 160, "54", "58", "50", "0" },
	{ 60, 160, "62", "58", "50", "0" },
	{ 64, 160, "62", "58", "50", "0" },

	{ 100, 160, "102", "106", "114", "0" },
	{ 104, 160, "102", "106", "114", "0" },
	{ 108, 160, "110", "106", "114", "0" },
	{ 112, 160, "110", "106", "114", "0" },
	{ 116, 160, "118", "122", "114", "0" },
	{ 120, 160, "118", "122", "114", "0" },
	{ 124, 160, "126", "122", "114", "0" },
	{ 128, 160, "126", "122", "114", "0" },

	{ 132, 160, "134", "138", "114", "0" },
	{ 136, 160, "134", "138", "114", "0" },
	{ 140, 160, "142", "138", "114", "0" },
	{ 144, 80, "142", "138", "", "" },

	{ 149, 80, "151", "155", "", "" },
	{ 153, 80, "151", "155", "", "" },
	{ 157, 80, "159", "155", "", "" },
	{ 161, 80, "159", "155", "", "" },

	{ 165, 20, "", "", "", "" },
};

const char *wireless_11ac_segt_idx( int bw_mhz, int ch )
{
	unsigned int i;

	if ( ch <= 0 )
	{
		if ( bw_mhz <= 40 )
		{
			return "-2";
		}
		return "-6";
	}
	if ( bw_mhz != 40 && bw_mhz != 80 && bw_mhz != 160 )
	{
		return "";
	}
	for ( i = 0; i < sizeof( a_chan ) / sizeof( a_chan[0] ); i++ )
	{
		if ( a_chan[i].ch != ch )
		{
			continue;
		}
		if ( bw_mhz == 40 )
		{
			return a_chan[i].seg40;
		}
		if ( bw_mhz == 80 )
		{
			return a_chan[i].seg80;
		}
		return a_chan[i].seg160;
	}
	return "";
}

const char *wireless_11ax_he_chwidth( int bw_mhz )
{
	if ( bw_mhz >= 160 )
	{
		return "2";
	}
	if ( bw_mhz >= 80 )
	{
		return "1";
	}
	return "0";
}

const char *wireless_11ax_he_seg0_idx( int bw_mhz, int ch )
{
	if ( bw_mhz <= 20 )
	{
		return "0";
	}
	return wireless_11ac_segt_idx( bw_mhz, ch );
}

const char *wireless_11ax_he_seg1_idx( int bw_mhz, int ch )
{
	unsigned int i;

	if ( bw_mhz != 160 )
	{
		return "0";
	}
	if ( ch <= 0 )
	{
		return "0";
	}
	for ( i = 0; i < sizeof( a_chan ) / sizeof( a_chan[0] ); i++ )
	{
		if ( a_chan[i].ch != ch )
		{
			continue;
		}
		if ( a_chan[i].seg160 == NULL || a_chan[i].seg160[0] == '\0' )
		{
			return "0";
		}
		if ( a_chan[i].he160_seg1 != NULL && a_chan[i].he160_seg1[0] != '\0' )
		{
			return a_chan[i].he160_seg1;
		}
		return "0";
	}
	return "0";
}

const char *wireless_11ac_bandwidth( const char *bandwidth, int ch, const char *country )
{
	int want;
	int max_mhz;
	int reg_cap;
	unsigned int i;

	want = 20;

	if ( bandwidth != NULL && *bandwidth != '\0' )
	{
		want = atoi( bandwidth );
	}
	if ( ch <= 0 )
	{
		max_mhz = 160;
	}
	else
	{
		max_mhz = 20;

		for ( i = 0; i < sizeof( a_chan ) / sizeof( a_chan[0] ); i++ )
		{
			if ( a_chan[i].ch == ch )
			{
				max_mhz = a_chan[i].max_bw_mhz;
				break;
			}
		}
	}

	reg_cap = country2mhza( country );

	if ( max_mhz > reg_cap )
	{
		max_mhz = reg_cap;
	}
	if ( want > max_mhz )
	{
		want = max_mhz;
	}
	if ( want >= 160 )
	{
		return "160";
	}
	if ( want >= 80 )
	{
		return "80";
	}
	if ( want >= 40 )
	{
		return "40";
	}
	return "20";
}

int country2mhza( const char *country )
{
	int i;
	country_set_t *p;

	if ( country == NULL || *country == '\0' )
	{
		return 160;
	}
	for ( i = 0;; i++ )
	{
		p = &country_list[i];
		if ( p->country == NULL )
		{
			return 160;
		}
		if ( 0 == strcasecmp( p->country, country ) )
		{
			if ( p->a_bw_max_mhz == 20 || p->a_bw_max_mhz == 40
					|| p->a_bw_max_mhz == 80 || p->a_bw_max_mhz == 160 )
			{
				return p->a_bw_max_mhz;
			}
			return 160;
		}
	}
}
talk_t country2chlist( const char *country, int a )
{
    int i;
    talk_t ret;
    int *ch;
    country_set_t *p;
    char buf[NAME_MAX];

    if ( country == NULL )
    {
        return NULL;
    }
    i = 0;
    ch = NULL;
    for ( i=0;; i++ )
    {
        p = &country_list[i];
        if ( p->country == NULL )
        {
            return NULL;
        }
        if ( 0 == strcasecmp( p->country, country ) )
        {
            if ( a != 0 )
            {
                ch = a_band[p->a];
            }
            else
            {
                ch = n_band[p->n];
            }
            break;
        }
    }
    ret = json_create( NULL );
    if ( ch != NULL )
    {
        while( *ch != 0 )
        {
            snprintf( buf, sizeof(buf), "%d", *ch );
            json_set_value( ret, buf, json_create( NULL ) );
            ch++;
        }
    }
    return ret;
}



