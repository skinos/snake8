## wifi@asta — 5.8G Wireless Station Management

### Overview

Manage 5.8G wireless station (client) connections. This component provides wireless station functionality for connecting to remote APs, including AP scanning, channel listing, and security mode listing. It is used as a lower-level device (ifdev) by **`ifname@wisp`** for WISP network management.

- manages wireless station lifecycle: setup, shutdown, connection monitoring
- supports multiple peer configurations for failover
- provides AP scanning, channel listing, and security mode listing
- includes keeplive mechanism for connection monitoring
- supports WPA/WPA2/WPA3 security modes



### Configuration reference ( wifi@asta )

```json
// Attributes introduction 
{
    "status":"enable or disable the function",                   // [ "enable", "disable" ]
    "nossid":"disable the ssid",                                 // [ "disable", "enable" ], disable the local ssid when connected

    // Wireless connect first peer
    "peer":"SSID to connect",                                    // [ string ]
    "peermac":"BSSID to connect",                                // [ mac address ]
    "peermode":"mode of connection",                             // [ "hidden" ], peer AP does not broadcast SSID; channel must be set in hidden mode
    "secure":"mode of security",                                 // [ "disable", "owe", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
                                                                      // "disable" for no security
                                                                      // "owe" for Opportunistic Wireless Encryption
                                                                      // "wpapsk" for WPAPSK
                                                                      // "wpa2psk" for WPA2PSK
                                                                      // "wpa3psk" for WPA3PSK
                                                                      // "wpapskwpa2psk" for WPA1/WPA2 PSK Auto
                                                                      // "wpa2pskwpa3psk" for WPA2/WPA3 PSK Auto
    "wpa_encrypt":"WAP encrypt",                                 // [ "aes", "tkip", "tkipaes" ]
                                                                      // "aes" for AES
                                                                      // "tkip" for TKIP
                                                                      // "tkipaes" for auto
    "wpa_key":"WPA key",                                         // [ string ], minimum 8 characters, mandatory if secure is wpapsk/wpa2psk/wpa3psk/wpapskwpa2psk/wpa2pskwpa3psk

    // Wireless connect second peer (failover)
    "peer2":"SSID2 to connect",                                  // [ string ]
    "peermac2":"BSSID2 to connect",                              // [ mac address ]
    "peermode2":"mode of connection",                            // [ "hidden" ]
    "secure2":"mode of security",                                // [ "disable", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
    "wpa_encrypt2":"WAP encrypt",                                // [ "aes", "tkip", "tkipaes" ]
    "wpa_key2":"WPA key",                                        // [ string ]

    // Wireless connect third peer (failover)
    "peer3":"SSID3 to connect",                                  // [ string ]
    "peermac3":"BSSID3 to connect",                              // [ mac address ]
    "peermode3":"mode of connection",                            // [ "hidden" ]
    "secure3":"mode of security",                                // [ "disable", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
    "wpa_encrypt3":"WAP encrypt",                                // [ "aes", "tkip", "tkipaes" ]
    "wpa_key3":"WPA key"                                         // [ string ]
}
```

#### Configuration example

Example, show 5.8G Station all configure
```shell
wifi@asta
{
    "peer":"V520-D21D20-5G",    # connect V520-D21D20-5G
    "secure":"wpapsk",          # security is WPAPSK
    "wpa_encrypt":"aes",        # encrypt use AES
    "wpa_key":"87654321"        # password 87654321
}
```

#### Configuration settings example

Example, modify the SSID for 5.8G Station connect
```shell
wifi@asta:peer=Myhotpot
ttrue
```

Example, modify the security and password
```shell
wifi@asta:secure=wpapsk
ttrue
```

Example, merge set the 5.8G Station configure( include "peer" "secure" "wpa_key" )
```shell
wifi@asta|{"peer":"Myhotpot","secure":"wpapsk","wpa_key":"88888888"}
ttrue
```

Example, disable the 5.8G Station connect
```shell
wifi@asta:status=disable
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the 5.8G Station**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the 5.8G Station**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get the 5.8G Station information**
    - failed return NULL
    - succeed return [ json ], station status information
    ```json
    {
        "status":"Current status",        // [ "nodevice", "uping", "down", "up" ]
                                             // "nodevice" means the network device does not exist
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "up" for the network is connect succeed
        "peer":"Peer SSID",              // [ string ]
        "peermac":"Peer BSSID",          // [ mac address ]
        "channel":"Peer channel",        // [ number ], 0-165
        "rate":"connect rate",           // [ number ], the unit is M
        "rssi":"Peer RSSI",              // [ number ], the unit is dBm
        "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], 0 for no signal, 1 for weakest, 4 for strongest
        "sinr":"signal to noise ratio",  // [ number ], Optional
        "bandwidth":"channel bandwidth", // [ "20", "40", "80", "160" ], Optional
        "beacon":"beacon interval"       // [ string ], Optional
    }
    ```

    Example, get the 5.8G Station information
    ```shell
    wifi@asta.status
    {
        "status":"up",                     # connect is succeed
        "peer":"TP-link-2231-5G",         # peer is TP-link-2231-5G
        "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
        "channel":"36",                   # channel is 36
        "rate":"270",                     # rate is 270M
        "rssi":"-41",                     # rssi is -41dBm
        "signal":"3"                      # signal level is 3
    }
    ```

+ `netdev[]` **get the 5.8G Station netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the 5.8G Station netdev
    ```shell
    wifi@asta.netdev
    ath11
    ```

+ `aplist[ peer, peermac, peer2, peer3 ]` **scan surrounding APs**
    - peer ------------- [ string ], optional, filter by SSID
    - peermac ---------- [ string ], optional, filter by BSSID
    - peer2 ------------ [ string ], optional, filter by SSID2
    - peer3 ------------ [ string ], optional, filter by SSID3
    - failed return NULL
    - succeed return [ json ], scanned AP information
    ```json
    {
        "AP BSSID":                       // [ mac address ]
        {
            "ssid":"SSID name",               // [ string ]
            "channel":"channel number",       // [ number ], 0-165, 0 for auto
            "secure":"mode of security",      // [ "disable", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
            "wpa_encrypt":"WAP encrypt",      // [ "aes", "tkip", "tkipaes" ]
            "sig":"signal level(%)",          // [ number ]
            "signal":"signal level[0-4]",     // [ "0", "1", "2", "3", "4" ]
            "chext":"extern channel",         // [ "none", "below", "above" ]
            "mode":"wireless system"          // [ string ]
        }
        // "...":{}  How many APs show how many properties
    }
    ```

    Example, get the surrounding AP from 5.8G Station scan
    ```shell
    wifi@asta.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # first AP SSID
            "channel":"36",                                # first AP channel
            "secure":"wpapskwpa2psk",                      # secure mode is WPA mix
            "wpa_encrypt":"aes",                           # encrypt type is AES
            "sig":"70",                                    # signal is 70%
            "signal":"3",                                  # signal level is 3, range is 0-4
            "chext":"below",                               # extern channel is below
            "mode":"11a/n/ac ABOVE"
        },
        "B4:82:C5:80:22:81":                    # second AP by scanning
        {
            "ssid":"dimmalex-work",
            "channel":"42",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11a/n/ac"
        }
    }
    ```

+ `chlist[]` **get available wireless channels**
    - failed return NULL
    - succeed return [ json ], available wireless channels
    ```json
    {
        "channel number":{}       // [ number ]:{}
        // "...":{}  How many channels show how many properties
    }
    ```

+ `securelist[]` **get supported security modes**
    - failed return NULL
    - succeed return [ json ], supported security modes
    ```json
    {
        "secure mode":            // [ string ]
        {
            "encrypt":""          // [ string ]: [ string ]
            // "...":"..."  How many encryptions show how many properties
        }
        // "...":{}  How many modes show how many properties
    }
    ```


#### Other

+ `reset[]` **reset the wireless station**
    - failed return tfalse
    - succeed return ttrue
    - Stops keeplive, relayd, and wpa_supplicant

    Example, reset the 5.8G Station
    ```shell
    wifi@asta.reset
    ttrue
    ```
