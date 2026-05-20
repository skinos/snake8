## wifi@nssid — 2.4G SSID Management

### Overview

Manage 2.4G wireless SSID (access point) interfaces. Each 2.4G radio can have multiple SSIDs, providing separate wireless networks on the same radio. Usually **`wifi@nssid`** is the first 2.4G SSID. If there are multiple 2.4G SSIDs in the system, **`wifi@nssid2`** is the second, and numbering increases sequentially.

- manages SSID lifecycle: setup, shutdown, connection monitoring
- supports WPA/WPA2/WPA3 security modes
- provides access control (ACL) with white list and black list
- supports client isolation and SSID broadcast control
- provides client list and client disconnect functionality



### Network Architecture

`wifi@nssid` is a **LAN-side device layer** component — a 2.4G wireless access point. Unlike station components, it does NOT act as an uplink. Its netdev is added to `bridge@lan`, which `ifname@lan` uses as its ifdev. It does NOT register directly with `network@frame`; the bridge handles registration as a "local" interface. It is controlled by the `wifi@n` radio component.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( wifi@nssid )

```json
// Attributes introduction 
{
    "status":"SSID status",                                    // [ "enable", "disable" ]
    "ssid":"SSID name",                                        // [ string ]
    "isolated":"isolated clients",                             // [ "enable", "disable" ]
    "broadcast":"broadcast SSID",                              // [ "enable", "disable" ]
    "secure":"secure mode",                                    // [ "disable", "owe", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
                                                                    // "disable" for no security
                                                                    // "owe" for Opportunistic Wireless Encryption
                                                                    // "wpapsk" for WPAPSK
                                                                    // "wpa2psk" for WPA2PSK
                                                                    // "wpa3psk" for WPA3PSK
                                                                    // "wpapskwpa2psk" for WPA1/WPA2 PSK Auto
                                                                    // "wpa2pskwpa3psk" for WPA2/WPA3 PSK Auto
    "wpa_encrypt":"WPA encrypt mode",                          // [ "aes", "tkip", "tkipaes" ]
                                                                    // "aes" for AES
                                                                    // "tkip" for TKIP
                                                                    // "tkipaes" for Auto
    "wpa_key":"WPA password",                                  // [ string ], minimum 8 characters
    "wpa_rekey":"WPA key renegotiate time",                    // [ number ], the unit is second, empty for no renegotiate

    "acl":"access control function",                           // [ "disable", "accept", "drop" ]
                                                                    // "disable" for no ACL
                                                                    // "accept" for white list
                                                                    // "drop" for black list
    "acl_table":                             // white list or black list, valid when acl is "accept" or "drop"
    {
        "MAC Address":"",                   // [ mac address ]: [ string ]
        // "...":"..."  How many MAC addresses show how many properties
    },
    "maxsta":"maximum number of clients",                      // [ number ], empty for no limit
    "wds":"WDS station mode",                                  // [ "enable", "disable" ]

    "wmm":"WMM state",                                         // [ "enable", "disable" ]
    "options":                             // custom attribute list for radio chip
    {
        "custom attribute":"value",          // [ string ]: [ string ]
        // "...":"..."  How many attributes show how many properties
    }
}
```

#### Configuration example

Example, show first 2.4G SSID all configure
```shell
wifi@nssid
{
    "status":"enable",              # enable the SSID
    "ssid":"5228-test-2.4g",        # SSID name is 5228-test-2.4g
    "isolated":"disable",           # clients are not quarantined, client can access each other
    "broadcast":"enable",           # broadcast the SSID name
    "secure":"wpapskwpa2psk",       # secure mode is WPA Auto
    "wpa_encrypt":"tkipaes",        # WPA encrypt is Auto
    "wpa_key":"22222222",           # WPA encrypt key is 22222222
    "wpa_rekey":"",                 # WPA encrypt key no renegotiate
    "acl":"accept",                 # white list
    "acl_table":                    # white list content, only these MAC addresses can access
    {
        "00:22:33:11:33:22":"",
        "00:22:33:11:33:23":"",
        "00:22:33:11:33:24":"",
        "00:22:33:11:33:EB":""
    },
    "maxsta":"64",                  # supports simultaneous access by a maximum of 64 clients
    "wmm":"enable"                  # enable the WMM
}
```

#### Configuration settings example

Example, modify the first 2.4G SSID name
```shell
wifi@nssid:ssid=myNewSSID
ttrue
```

Example, disable the first 2.4G SSID
```shell
wifi@nssid:status=disable
ttrue
```

Example, merge set the first 2.4G SSID secure configure( include "secure" "wpa_encrypt" "wpa_key" )
```shell
wifi@nssid|{"secure":"wpapsk","wpa_encrypt":"tkipaes","wpa_key":"88888888"}
ttrue
```

Example, disable the second 2.4G SSID
```shell
wifi@nssid2:status=disable
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the SSID**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the SSID**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get the SSID information**
    - failed return NULL
    - succeed return [ json ], SSID status information
    ```json
    {
        "status":"current status",              // [ "nodevice", "up", "down" ]
                                                    // "nodevice" means the network device does not exist
                                                    // "up" for enable
                                                    // "down" for disable
        "secure":"secure mode",                 // [ "disable", "owe", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
        "rx_bytes":"received bytes",            // [ number ]
        "rx_packets":"received packets",        // [ number ]
        "rx_errs":"received error packets",     // [ number ]
        "rx_drops":"received drop packets",     // [ number ]
        "tx_bytes":"sent bytes",                // [ number ]
        "tx_packets":"sent packets",            // [ number ]
        "tx_errs":"sent error packets",         // [ number ]
        "tx_drops":"sent drop packets",         // [ number ]
        "mac":"MAC address",                    // [ mac address ]
        "ssid":"SSID name",                     // [ string ]
        "bssid":"BSSID",                        // [ mac address ]
        "channel":"current channel"             // [ number ]
    }
    ```

    Example, get the first 2.4G SSID status
    ```shell
    wifi@nssid.status
    {
        "status":"up",
        "secure":"wpapskwpa2psk",
        "rx_bytes":"767164641",
        "rx_packets":"22258095",
        "rx_errs":"489663",
        "rx_drops":"0",
        "tx_bytes":"369735875",
        "tx_packets":"2036548",
        "tx_errs":"0",
        "tx_drops":"0",
        "mac":"00:03:7F:12:88:70",
        "ssid":"dimmalex-home",
        "bssid":"00:03:7F:12:88:70",
        "channel":"11"
    }
    ```

+ `stalist[]` **get list of clients**
    - failed return NULL
    - succeed return [ json ], connected client list
    ```json
    {
        "client MAC address":     // [ mac address ]
        {
            "livetime":"online time",    // [ string ], format is hour:minute:second:day
            "rssi":"signal strength"     // [ number ], the unit is dBm
        }
        // "...":{}  How many clients show how many properties
    }
    ```

    Example, get the first 2.4G SSID's client list
    ```shell
    wifi@nssid.stalist
    {
        "78:11:DC:92:D3:9E":                  # client 1
        {
            "livetime":"14:53:17:2",
            "rssi":"-52"
        },
        "88:C3:97:75:1B:C0":                 # client 2
        {
            "livetime":"14:53:14:2",
            "rssi":"-52"
        },
        "40:31:3C:4D:78:35":                 # client 3
        {
            "livetime":"14:52:22:2",
            "rssi":"-61"
        }
    }
    ```


#### Control APIs

+ `stabeat[ mac ]` **disconnect a client**
    - mac -------------- [ string ], client MAC address
    - failed return tfalse
    - succeed return ttrue

    Example, disconnect the client 00:03:7F:13:BD:30 from first 2.4G SSID
    ```shell
    wifi@nssid.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```
