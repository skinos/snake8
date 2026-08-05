## arch@custom — Product OEM Custom Defaults

### Overview

**`arch@custom`** holds **product / OEM defaults** consumed by land, arch, network, and the Web UI. It is a **configuration object** (no dedicated method APIs); query and change it with the HE configuration grammar on **`arch@custom`**.

- hostname and Wi-Fi SSID auto-naming from MAC or serial number, driven by **`arch@data`** at bring-up
- online firmware store URL and credentials, plus post-upgrade restart hint seconds for **`arch@firmware`**
- Web UI reboot progress duration via **`restart_time`** (loaded into **`window.custom`**)
- per-project enable/disable map consulted by **`land@machine`** / **`land@fpk`** when registering packages
- optional **`mode`** map that overrides **`network@frame.mode_list`** when non-empty
    > factory defaults are usually shipped as **`custom.cfg`** under the board’s **`config/.../arch/`** overlay



### Configuration reference ( arch@custom )

```json
// Attributes introduction 
{
    "restart_time": "WUI reboot max wait / progress estimate in seconds", // [ string ], used by he.reboot(); empty or missing uses 60
    "upgrade_time": "seconds before suggested restart after upgrade", // [ string ], returned as zz "wait" by arch@firmware; empty uses 90

    "name_custom": "hostname auto-name source",                     // [ "enable","disable","sn","imei" ]
                                          // "enable" or unset: build hostname from MAC (and name_prefix)
                                          // "sn": use serial number suffix instead of MAC
                                          // "disable" or "imei": do not auto-customize hostname
    "name_prefix": "hostname prefix string",                        // [ string ], used when name_custom builds a new name
    "name_mackey": "MAC or SN suffix width",                      // [ string ], e.g. "4" or "8"

    "nssid_custom": "2.4G SSID auto-name source",                   // [ "enable","disable","sn","imei" ], same semantics as name_custom
    "nssid_prefix": "2.4G SSID prefix",                             // [ string ]
    "nssid_mackey": "2.4G SSID MAC or SN suffix width",           // [ string ], e.g. "4" or "8"

    "assid_custom": "5 GHz SSID auto-name source",                  // [ "enable","disable","sn","imei" ], same semantics as name_custom
    "assid_prefix": "5 GHz SSID prefix",                            // [ string ]
    "assid_mackey": "5 GHz SSID MAC or SN suffix width",          // [ string ], e.g. "4" or "8"

    "firmware_store": "OTA firmware repository base URL",           // [ string ], FTP, HTTP, or HTTPS; empty uses platform default
    "firmware_store_user": "curl -u authentication string",       // [ string ], user:password form; empty uses platform default

    "project":           // [ json ], project enable map used at package register time
    {
        "project name": "enable or disable"  // [ string ]: [ "disable","enable" ], project id under the map
                                                   // "disable": skip registering this project
                                                   // unset or other: register normally
        // "...":"..."  How many projects show how many properties
    },
    "mode":              // [ json ], allowed network operation modes; when non-empty, replaces network@frame.mode_list
    {
        "mode name": ""  // [ string ]: [ string ], mode id key; value is typically empty string when allowed
        // "...":""  How many modes show how many properties
    }
}
```

#### Configuration example

Example, show a typical OEM custom configure
```shell
arch@custom
{
    "restart_time":"90",                       # WUI reboot max wait / progress estimate 90 seconds
    "upgrade_time":"120",                      # after zz upgrade, suggest wait 120 seconds before reconnect
    "name_custom":"enable",                    # auto-generate hostname from MAC
    "name_prefix":"SkinOS",                    # hostname prefix: SkinOS
    "name_mackey":"4",                         # use last 4 hex digits of MAC in hostname
    "nssid_custom":"enable",                   # auto-generate 2.4G SSID from MAC
    "nssid_prefix":"SkinOS",                   # 2.4G SSID prefix: SkinOS
    "nssid_mackey":"4",                        # use last 4 hex digits of MAC in SSID
    "assid_custom":"enable",                   # auto-generate 5 GHz SSID from MAC
    "assid_prefix":"SkinOS5G",                 # 5 GHz SSID prefix: SkinOS5G
    "assid_mackey":"4",                        # use last 4 hex digits of MAC in 5 GHz SSID
    "firmware_store":"ftp://repo.ashyelf.com", # OTA repository base URL
    "firmware_store_user":"dl:dl@ashyelf.com", # credentials for online_check and online_upgrade
    "project":                                 # project enable/disable map
    {
        "land":"enable",                           # land project is enabled
        "wifi":"enable",                           # wifi project is enabled
        "modem":"enable"                           # modem project is enabled
    },
    "mode":                                    # allowed network operation modes
    {
        "gateway":"",                              # gateway mode allowed
        "dgateway":""                              # dual-gateway mode allowed
    }
}
```

#### Configuration settings example

Example, set the WUI reboot max wait to 90 seconds
```shell
arch@custom:restart_time=90
ttrue
```

Example, set the post-upgrade restart hint to 120 seconds
```shell
arch@custom:upgrade_time=120
ttrue
```

Example, set the OTA firmware repository URL
```shell
arch@custom:firmware_store=ftp://firmware.example.com/releases
ttrue
```

Example, merge set OTA and upgrade timing settings( include "firmware_store" "firmware_store_user" "upgrade_time" )
```shell
arch@custom|{"firmware_store":"ftp://firmware.example.com/releases","firmware_store_user":"dl:password@example.com","upgrade_time":"120"}
ttrue
```

Example, merge set hostname auto-name settings( include "name_custom" "name_prefix" "name_mackey" )
```shell
arch@custom|{"name_custom":"enable","name_prefix":"SkinOS","name_mackey":"4"}
ttrue
```

Example, subtree merge for project enable map
```shell
arch@custom:project|{"wifi":"enable","tui":"disable"}
ttrue
```



### Concepts

**Configuration-only object**
* **`arch@custom`** has no Management / Query / Control method APIs.
* Read the whole object with **`arch@custom`**; set one field with **`arch@custom:key=value`**; merge with **`arch@custom|{…}`** or **`arch@custom:path|{…}`**.

**WUI reboot vs upgrade wait**
* **`restart_time`** is the **maximum wait** (and progress estimate, seconds) for **`he.reboot()`** when the page does not pass **`restartTime`**. The UI probes the device web and reloads when it is back; empty or missing uses **60**.
* **`upgrade_time`** is read by **`arch@firmware`** and returned as **`wait`** after a successful **`.zz`** flash; the upgrade UI may use that value (or **`upgrade_wait`**) as the max wait for **`he.upgrade_reboot()`**.
