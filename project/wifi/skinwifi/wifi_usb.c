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
} country_set_t;
static country_set_t country_list[] =
{
    {"DB", 5, 7 },
    {"AL", 1, 0 },
    {"DZ", 1, 0},

    {"AR", 1, 3},
    {"AM", 1, 2},
    {"AU", 1, 0},

    {"AT", 1, 1},
    {"AZ", 1, 2},
    {"BH", 1, 0},

    {"BY", 1, 0},
    {"BE", 1, 1},
    {"BZ", 1, 4},

    {"BO", 1, 4},
    {"BR", 1, 1},
    {"BN", 1, 4},

    {"BG", 1, 1},
    {"CA", 0, 0},
    {"CL", 1, 0},

    {"CN", 1, 0},
    {"CO", 0, 0},
    {"CR", 1, 0},

    {"HR", 1, 2},
    {"CY", 1, 1},
    {"CZ", 1, 2},

    {"DK", 1, 1},
    {"DO", 0, 0},
    {"EC", 1, 0},

    {"EG", 1, 2},
    {"SV", 1, 0},
    {"EE", 1, 1},

    {"FI", 1, 1},
    {"FR", 1, 2},
    {"GE", 1, 2},

    {"DE", 1, 1},
    {"GR", 1, 1},
    {"GT", 0, 0},

    {"HN", 1, 0},
    {"HK", 1, 0},
    {"HU", 1, 1},

    {"IS", 1, 1},
    {"IN", 1, 0},
    {"ID", 1, 4},

    {"IR", 1, 4},
    {"IE", 1, 1},
    {"IL", 1, 0},

    {"IT", 1, 1},
    {"JP", 5, 9},
    {"JO", 1, 0},

    {"KZ", 1, 0},
    {"KP", 1, 5},
    {"KR", 1, 5},

    {"KW", 1, 0},
    {"LV", 1, 1},
    {"LB", 1, 1},

    {"LI", 1, 1},
    {"LT", 1, 1},
    {"LU", 1, 1},

    {"MO", 1, 0},
    {"MK", 1, 0},
    {"MY", 1, 0},

    {"MX", 0, 0},
    {"MC", 1, 2},
    {"MA", 1, 0},

    {"NL", 1, 1},
    {"NZ", 1, 0},
    {"NO", 0, 0},

    {"OM", 1, 0},
    {"PK", 1, 0},
    {"PA", 0, 0},

    {"PE", 1, 4},
    {"PH", 1, 4},
    {"PL", 1, 1},

    {"PT", 1, 1},
    {"PR", 0, 0},
    {"QA", 1, 0},

    {"RO", 1, 0},
    {"RU", 1, 0},
    {"SA", 1, 0},

    {"SG", 1, 0},
    {"SK", 1, 1},
    {"SI", 1, 1},

    {"ZA", 1, 1},
    {"ES", 1, 1},
    {"SE", 1, 1},

    {"CH", 1, 1},
    {"SY", 1, 0},
    {"TW", 1, 3},

    {"TH", 1, 0},
    {"TT", 1, 2},
    {"TN", 1, 1},

    {"TR", 1, 2},
    {"UA", 1, 0},
    {"AE", 1, 0},

    {"GB", 1, 1},
    {"US", 0, 7},
    {"UY", 1, 5},

    {"UZ", 0, 1},
    {"VE", 1, 5},
    {"VN", 1, 0},

    {"YE", 1, 0},
    {"ZW", 1, 0},
    {NULL, 0, 0},
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
talk_t country2chlist( const char *country, int a )
{
    int i;
    talk_t ret;
    int *ch;
    country_set_t *p;
    char buf[NAME_MAX];

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



