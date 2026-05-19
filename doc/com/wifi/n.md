## wifi@n — 2.4G Radio Management

### Overview

Manage 2.4G radio baseband for wireless access point functionality. Each platform registers a **`wifi@n`** component for its 2.4G WiFi baseband chip, providing a unified interface regardless of the underlying hardware.

- manages 2.4G radio lifecycle: setup, shutdown, hostapd configuration
- supports 802.11b/g/n/ax wireless modes
- provides channel list, mode list, security list, and station list queries
- supports LDPC, STBC, and Short GI radio features



### Configuration reference ( wifi@n )

```json
// Attributes introduction 
{
    "status":"start at system startup",                        // [ "enable", "disable" ]
    "mode":"radio mode",                                       // [ "bg", "n", "2g" ]
                                                                    // "bg" for 11BG
                                                                    // "n" for 11N, bandwidths: 20, 40
                                                                    // "2g" for 11AX, bandwidths: 20, 40
    "bandwidth":"radio bandwidth",                             // [ "20", "40" ]
    "channel":"radio channel",                                 // [ 0-14 ], 0 for auto select the channel
    "beacon":"beacon interval",                                // [ number ]
    "country":"country code",                                  // [ string ], e.g. "cn", "us", "jp"
    "dtim":"radio dtim",                                       // [ number ]
    "txpower":"transmit power",                                // [ number ], optional
    "ldpc":"use LDPC or not",                                  // [ "disable", "enable" ]
    "shortgi":"short GI",                                      // [ "disable", "enable" ]
    "stbc":"radio STBC",                                       // [ "disable", "enable" ]
    "extcha":"extension channel",                              // [ "below", "above" ], optional, for 40M bandwidth
    "rts_threshold":"RTS threshold",                           // [ number ], optional
    "frag_threshold":"fragmentation threshold",                // [ number ], optional

    // Advanced hostapd options (driver-specific)
    "option":                             // advanced hostapd configuration options
    {
        "g":                             // 802.11g specific options
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence"                         // [ string ]
        },
        "n":                             // 802.11n specific options
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence"                         // [ string ]
        },
        "2g":                            // 2.4G specific options (for AX mode)
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence",                        // [ string ]
            "he_bss_color":"HE BSS color",                     // [ string ]
            "he_su_beamformer":"HE SU beamformer",             // [ string ]
            "he_su_beamformee":"HE SU beamformee",             // [ string ]
            "he_mu_beamformer":"HE MU beamformer"              // [ string ]
        },
        "ht_capab":"HT capabilities",                          // [ string ]
        "vht_capab":"VHT capabilities"                         // [ string ]
    }
}
```

#### Configuration example

Example, show 2.4G Radio all configure
```shell
wifi@n
{
    "mode":"n",                # 2.4G Radio is 11N
    "bandwidth":"40",          # 2.4G bandwidth 40M
    "channel":"11",            # 2.4G channel is 11
    "beacon":"100",            # 2.4G beacon interval is 100ms
    "dtim":"1",
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```

#### Configuration settings example

Example, modify the 2.4G radio channel to auto
```shell
wifi@n:channel=0
ttrue
```

Example, modify the 2.4G radio channel to 11
```shell
wifi@n:channel=11
ttrue
```

Example, merge set the 2.4G Radio configure( include "mode" "bandwidth" "channel" )
```shell
wifi@n|{"mode":"n","bandwidth":"40","channel":"0"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the 2.4G radio**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the 2.4G radio**
    - failed return tfalse
    - succeed return ttrue

+ `hostapd[]` **configure and generate hostapd configuration**
    - failed return tfalse
    - succeed return ttrue
    - Generates hostapd configuration file based on current radio settings
    - Called internally by setup[]

+ `start_hostapd[]` **start hostapd service**
    - failed return tfalse
    - succeed return ttrue
    - Starts the hostapd daemon for wireless AP functionality

+ `stop_hostapd[]` **stop hostapd service**
    - failed return tfalse
    - succeed return ttrue
    - Stops the hostapd daemon


#### Query APIs

+ `chlist[ mode, country ]` **get the 2.4G radio channel list**
    - mode ------------- [ string ], optional, radio mode (e.g. "n", "2g"), defaults to configured mode
    - country ---------- [ string ], optional, country code, defaults to configured country
    - failed return NULL
    - succeed return [ json ], available channels
    ```json
    {
        "channel number":{}       // [ number ]:{}
        // "...":{}  How many channels show how many properties
    }
    ```

    Example, get the 2.4G radio channel list
    ```shell
    wifi@n.chlist
    {
        "1":{},
        "2":{},
        "3":{},
        "4":{},
        "5":{},
        "6":{},
        "7":{},
        "8":{},
        "9":{},
        "10":{},
        "11":{},
        "12":{},
        "13":{}
    }
    ```

+ `modelist[]` **get supported wireless modes and bandwidths**
    - failed return NULL
    - succeed return [ json ], supported modes with bandwidth options
    ```json
    {
        "mode name":              // [ string ]
        {
            "bandwidth":""        // [ string ]: [ string ]
            // "...":"..."  How many bandwidths show how many properties
        }
        // "...":{}  How many modes show how many properties
    }
    ```

    Example, get the 2.4G radio mode list
    ```shell
    wifi@n.modelist
    {
        "bg":"",
        "n":
        {
            "20":"",
            "40":""
        },
        "2g":
        {
            "20":"",
            "40":""
        }
    }
    ```

+ `securelist[]` **get supported security modes**
    - failed return NULL
    - succeed return [ json ], supported security modes with encryption options for the radio
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

    Example, get the 2.4G radio security list
    ```shell
    wifi@n.securelist
    {
        "disable":"",
        "wpapsk":
        {
            "aes":"",
            "tkip":"",
            "tkipaes":""
        },
        "wpa2psk":
        {
            "aes":"",
            "tkip":"",
            "tkipaes":""
        },
        "wpapskwpa2psk":
        {
            "aes":"",
            "tkip":"",
            "tkipaes":""
        }
    }
    ```

+ `stalist[]` **get list of clients on 2.4G radio**
    - failed return NULL
    - succeed return [ json ], connected client list
    ```json
    {
        "client MAC address":     // [ mac address ]
        {
            "apidx":"AP index",          // [ string ]
            "livetime":"online time",    // [ string ], format is hour:minute:second:day
            "rssi":"signal strength",    // [ number ], the unit is dBm
            "ifdev":"SSID component"     // [ string ]
        }
        // "...":{}  How many clients show how many properties
    }
    ```

    Example, get the 2.4G radio's client list
    ```shell
    wifi@n.stalist
    {
        "78:11:DC:92:D3:9E":                  # client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "88:C3:97:75:1B:C0":                 # client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "40:31:3C:4D:78:35":                 # client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@nssid"
        }
    }
    ```


