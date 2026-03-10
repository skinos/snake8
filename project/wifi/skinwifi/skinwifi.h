#ifndef H_WIFI_SKINWIFI_H
#define H_WIFI_SKINWIFI_H
/**
 * @brief wifi相关的基本接口
 * @author dimmalex
 * @version 1.0
*/



#define WIFI_OBJECT_MAX 10
/* modem object name allocation */
const char *wifia_alloc( const char *syspath, const char *id, talk_t matchcfg, char *buf, int buflen );
/* modem object name free */
void        wifia_free( const char *object );
/* usb network device path list find, return >0 is find device number, return 0 for nofound*/
int         wifia_netlist( const char *syspath, char device[][NAME_MAX] );


/* list the channel */
talk_t country2chlist( const char *country, int a );



#endif   /* ----- #ifndef H_WIFI_SKINWIFI_H  ----- */

