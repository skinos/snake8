#ifndef H_WIFI_SKINWIFI_H
#define H_WIFI_SKINWIFI_H
/**
 * @brief wifi相关的基本接口
 * @author dimmalex
 * @version 1.0
*/



#define WIFI_OBJECT_MAX 10
#define WIFI_NSTA_MAC_OFFSET 8
#define WIFI_NSSID_MAC_OFFSET 9
#define WIFI_ASTA_MAC_OFFSET 12
#define WIFI_ASSID_MAC_OFFSET 13
/* modem object name allocation */
const char *wifia_alloc( const char *syspath, const char *id, talk_t matchcfg, char *buf, int buflen );
/* modem object name free */
void        wifia_free( const char *object );
/* usb network device path list find, return >0 is find device number, return 0 for nofound*/
int         wifia_netlist( const char *syspath, char device[][NAME_MAX] );


/* list the channel */
talk_t country2chlist( const char *country, int a );
/* max 11a (5 GHz) channel width in MHz (20/40/80/160) from country table; unknown -> 160 */
int    country2mhza( const char *country );
/* hostapd vht_oper_centr_freq_seg0_idx for 40/80/160; ch<=0 -> -2 / -6; else "" if unknown */
const char *wireless_11ac_segt_idx( int bw_mhz, int ch );
/* hostapd he_oper_chwidth: 0 = 20/40, 1 = 80, 2 = 160 (same encoding as vht_oper_chwidth on 5 GHz). */
const char *wireless_11ax_he_chwidth( int bw_mhz );
/* he_oper_centr_freq_seg0_idx: same indices as VHT for 5 GHz */
const char *wireless_11ax_he_seg0_idx( int bw_mhz, int ch );
/* he_oper_centr_freq_seg1_idx: contiguous 160 from a_chan.he160_seg1; else "0" */
const char *wireless_11ax_he_seg1_idx( int bw_mhz, int ch );
/* Clamp requested bandwidth to country + per-channel geometry (a_chan table in wifi_usb.c). */
const char *wireless_11ac_bandwidth( const char *bandwidth, int ch, const char *country );



#endif   /* ----- #ifndef H_WIFI_SKINWIFI_H  ----- */

