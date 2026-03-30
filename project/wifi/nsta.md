## wifi@nsta — 2.4G Station Management
Manage 2.4G Station


### Configuration ( `wifi@nsta` )
```json
// Attribute introduction
{
    "status":"enable or  disable the function",     // [ "enable", "disable" ]

    // wireless connect
    "peer":"SSID to connect",              // [ string ]
    "peer2":"SSID2 to connect",            // [ string ]
    "peer3":"SSID3 to connect",            // [ string ]
    "peermac":"BSSID to connect",          // [ mac address ]
    "peermode":"mode of connection",       // [ "hidden" ] Indicates that the peer end does not broadcast SSID. In hidden mode, channel must not be empty  
    "channel":"wireless channel",          // [ number ], 0-14, 0 for auto
    "secure":"mode of security",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] for no securiyt
                                                            // [ wpapsk ]  for WPAPSK
                                                            // [ wpa2psk ]  for WPA2PSK
                                                            // [ wpapskwpa2psk ] for WPA Mix
    "wpa_encrypt":"WAP encrypt",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] for AES
                                                            // [ tkip ] for TKIP
                                                            // [ tkipaes ] for auto
    "wpa_key":"WPA key",                   // [ string ], The value is a string of at least 8 characters. This parameter is mandatory if the "secure" be "wpapsk" or "wpa2psk" or "wpapskwpa2psk"
    "ssid_disable":"disable the ssid"      // [ "disable", "enable" ], disable the local ssid when connected

}
```

Example, show 2.4G Station all configure
```shell
{
    # WIFI peer
    "peer":"V520-D21D20",    # connect V520-D21D20
    "secure":"wpapsk",       # security is WPAPSK
    "wpa_encrypt":"aes",     # encrypt use AES
    "wpa_key":"87654321",    # password 87654321
}
```

Example, modify the SSID for 2.4G Station connect
```shell
wifi@nsta:peer=Myhotpot
ttrue
wifi@nsta:secure=wpapsk
ttrue
wifi@nsta:wpa_key=88888888
ttrue
# You can also use one command to complete the operation of the above three command
wifi@nsta|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```

Example, disable the 2.4G Station connect
```shell
wifi@nsta:status=disable
ttrue
```

Example, enable the 2.4G Station connect
```shell
wifi@nsta:status=enable
ttrue
```


### Component API
**Directly callable** APIs: `wifi@nsta.method`, `wifi@nsta2.method`, … (HE / eline / HTTP `/he`).

+ `status[]` **get the 2.4G Station infomation**   
    - failed return NULL
    - error return terror    
    - succeed return json to describes this infomation   
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current status",        // [ "uping", "down", "up" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed

        "peer":"Peer SSID",              // [ string ]
        "peermac":"Peer BSSID",          // [ MAC address ]
        "channel":"Peer channel",        // [ 1-14 ]
        "rate":"connect rate",           // [ number ], the unit is M
        "rssi":"Peer RSSI",              // [ number ], the unit is dBm
        "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
    }
    ```

    ```shell
    # examples, get the 2.4G Station infomation
    wifi@nsta.status
    {

        "status":"up",                     # connect is succeed
        "peer":"TP-link-2231",            # peer is TP-link-2231
        "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
        "channel":"10",                   # channel is 10
        "rate":"270",                     # rate is 270M
        "rssi":"-41",                     # rssi is -41dBm
        "signal":"3"                      # signal level is 3
    }
    ```

+ `netdev[]` **get the 2.4G Station netdev**   
    - failed return NULL
    - error return terror    
    - succeed return string to describes this infomation   

    Example, get the 2.4G Station netdev
    ```shell
    wifi@nsta.netdev
    ath11
    ```


+ `aplist[]` **use the 2.4G Station scan the surrounding AP**   
    - failed return NULL
    - error return terror    
    - succeed return json to describes this infomation       
    ```json
    // Attributes introduction of talk by the method return
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID name",                           // [ string ]
            "channel":"channel number",                   // [ number ], 0-14, 0 for auto
            "secure":"mode of security",                  // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                 // "disable" for no securiyt
                                                                 // "wpapsk"  for WPAPSK
                                                                 // "wpa2psk"  for WPA2PSK
                                                                 // "wpapskwpa2psk" for WPA Mix
            "wpa_encrypt":"WAP encrypt",                  // [ "aes", "tkip", "tkipaes" ]
                                                                 // "aes" for AES
                                                                 // "tkip" for TKIP
                                                                 // "tkipaes" for auto
            "signal":"signal level[0-4]",                 // [ "0", "1", "2", "3", "4" ]
            "chext":"extern channel",                     // [ "none", "below", "above" ]
            "mode":"wireless system"                      // [ string ]
        }
        // ... more AP
    }
    ```

    Example, get the surrounding AP from 2.4G Station scan
    ```shell
    wifi@nsta.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # frist AP SSID
            "channel":"6",                                 # first AP channel
            "secure":"wpapskwpa2psk",                      # secure mode is WPA mix
            "wpa_encrypt":"aes",                           # encrypt type is AES
            "qual":"70",                                   # signal is 70%
            "signal":"3",                                  # signal level is 3, range is 0-4
            "chext":"below",                               # extern channel is below
            "mode":"11b/g/n"
        },
        "B4:82:C5:40:22:41":                    # second AP by scanning
        {
            "ssid":"dimmalex-work",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "qual":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11b/g/n"
        },
        "8C:74:A0:D6:68:B0":                    # third AP by scanning
        {
            "ssid":"CMCC-ktfK",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "qual":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11b/g/n"
        }
    }
    ```

+ `shut[]` **shutdown the 2.4G Station**   
    - failed return tfalse
    - error return terror    
    - succeed return ttrue

    Example, shutdown the 2.4G Station
    ```shell
    wifi@nsta.shut
    ttrue
    ```

+ `setup[]` **setup the 2.4G Station**   
    - failed return tfalse
    - error return terror    
    - succeed return ttrue
    
    Example, setup the 2.4G Station
    ```shell
    wifi@nsta.setup
    ttrue
    ```

### Lifecycle API
+ `setup[]` / `shut[]` — when present for this object in `project/wifi`, they start/stop the underlying wireless service. The reference **wifi** FPK does not schedule **`init`/`uninit`** for these objects; bring-up is usually driven by the driver, **network** stack, or product integration.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_wifi_nsta(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@nsta", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@nsta", "value", "status");
    return ok ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* Example: scall("wifi@nsta", "status", NULL); then talk_free if JSON */
```
