## wifi@a — 5.8G Radio Management

### Overview

Manage 5.8G radio baseband for wireless access point functionality. Each platform registers a **`wifi@a`** component for its 5.8G WiFi baseband chip, providing a unified interface regardless of the underlying hardware.

- manages 5.8G radio lifecycle: setup, shutdown, hostapd configuration
- supports 802.11a/ac/ax wireless modes
- provides channel list, mode list, security list, and station list queries
- supports LDPC, STBC, and Short GI radio features



### Configuration reference ( wifi@a )

```json
// Attributes introduction 
{
    "status":"start at system startup",                        // [ "enable", "disable" ]
    "mode":"radio mode",                                       // [ "a", "ac", "5g" ]
                                                                    // "a" for 11A, bandwidths: 20, 40
                                                                    // "ac" for 11AC, bandwidths: 20, 40, 80, 160
                                                                    // "5g" for 11AX, bandwidths: 20, 40, 80, 160
    "bandwidth":"radio bandwidth",                             // [ "20", "40", "80", "160" ]
    "channel":"radio channel",                                 // [ 0, 36-165 ], 0 for auto select the channel
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
        "a":                             // 802.11a specific options
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "enable_background_radar":"background radar",      // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence"                         // [ string ]
        },
        "ac":                            // 802.11ac specific options
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "enable_background_radar":"background radar",      // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence",                        // [ string ]
            "tx_queue_data2_burst":"TX queue burst"            // [ string ]
        },
        "5g":                            // 5G specific options (for AX mode)
        {
            "spectrum_mgmt_required":"spectrum management",    // [ string ]
            "rssi_reject_assoc_rssi":"RSSI reject threshold",  // [ string ]
            "rssi_reject_assoc_timeout":"reject timeout",      // [ string ]
            "rssi_ignore_probe_request":"ignore probe",        // [ string ]
            "acs_exclude_dfs":"exclude DFS channels",          // [ string ]
            "enable_background_radar":"background radar",      // [ string ]
            "min_tx_power":"minimum TX power",                 // [ string ]
            "stationary_ap":"stationary AP mode",              // [ string ]
            "ht_coex":"HT coexistence",                        // [ string ]
            "tx_queue_data2_burst":"TX queue burst",           // [ string ]
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

Example, show 5.8G Radio all configure
```shell
wifi@a
{
    "mode":"ac",               # 5.8G Radio is 11AC
    "bandwidth":"80",          # 5.8G bandwidth 80M
    "channel":"165",           # 5.8G channel is 165
    "beacon":"100",            # 5.8G beacon interval is 100ms
    "dtim":"1",
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```

#### Configuration settings example

Example, modify the 5.8G Radio channel to auto
```shell
wifi@a:channel=0
ttrue
```

Example, modify the 5.8G Radio channel to 36
```shell
wifi@a:channel=36
ttrue
```

Example, merge set the 5.8G Radio configure( include "mode" "bandwidth" "channel" )
```shell
wifi@a|{"mode":"ac","bandwidth":"80","channel":"0"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the 5.8G radio**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the 5.8G radio**
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

+ `chlist[ mode, country ]` **get the 5.8G radio channel list**
    - mode ------------- [ string ], optional, radio mode (e.g. "a", "ac", "5g"), defaults to configured mode
    - country ---------- [ string ], optional, country code, defaults to configured country
    - failed return NULL
    - succeed return [ json ], available channels for the specified mode and country
    ```json
    {
        "channel number":{}       // [ number ]:{}
        // "...":{}  How many channels show how many properties
    }
    ```

    Example, get the 5.8G radio channel list
    ```shell
    wifi@a.chlist
    {
        "36":{},
        "40":{},
        "44":{},
        "48":{},
        "52":{},
        "56":{},
        "60":{},
        "64":{},
        "149":{},
        "153":{},
        "157":{},
        "161":{},
        "165":{}
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

    Example, get the 5.8G radio mode list
    ```shell
    wifi@a.modelist
    {
        "a":
        {
            "20":"",
            "40":""
        },
        "ac":
        {
            "20":"",
            "40":"",
            "80":"",
            "160":""
        },
        "5g":
        {
            "20":"",
            "40":"",
            "80":"",
            "160":""
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

    Example, get the 5.8G radio security list
    ```shell
    wifi@a.securelist
    {
        "disable":"",
        "owe":
        {
            "aes":""
        },
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
        },
        "wpa3psk":
        {
            "aes":""
        },
        "wpa2pskwpa3psk":
        {
            "aes":""
        }
    }
    ```

+ `stalist[]` **get list of clients on 5.8G radio**
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

    Example, get the 5.8G radio's client list
    ```shell
    wifi@a.stalist
    {
        "78:11:DC:92:D3:9E":                  # client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "88:C3:97:75:1B:C0":                 # client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "40:31:3C:4D:78:35":                 # client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@assid"
        }
    }
    ```



