## arch@custom — Product custom defaults

OEM/custom defaults: hostname and SSID auto-naming, **online upgrade** store URL credentials, **`land@machine`** project enable/disable map, and allowed **`network@frame`** operation modes. Persisted like any HE object; consumed at boot by **land** and **arch** code paths. Macro: **`CUSTOM_COM`** → **`arch@custom`** (`core/land/skin/skinhead.h`). Sample file: [`custom.cfg`](custom.cfg).

### Configuration ( `arch@custom` )
```json
// Attributes introduction
{
    "verify":"enable or disable verify step",           // [ "enable", "disable" ]
    "restart_time":"max seconds before restart allowed", // [ number ], used by machine policy
    "upgrade_time":"seconds budget for online upgrade", // [ number ], `arch@firmware` / WUI

    "name_custom":"hostname source",                    // [ "enable", "disable", "sn", "imei" ]
    "name_prefix":"hostname prefix string",
    "name_mackey":"MAC suffix width",                 // e.g. "4" or "8" hex digits

    "nssid_custom":"2.4G SSID auto pattern",
    "nssid_prefix":"2.4G SSID prefix",
    "nssid_mackey":"MAC suffix width",

    "assid_custom":"5 GHz SSID auto pattern",
    "assid_prefix":"5 GHz SSID prefix",
    "assid_mackey":"MAC suffix width",

    "firmware_store":"online upgrade base URL or store id",
    "firmware_store_user":"credentials string for store",

    "project": {
        "land":"enable",
        "clock":"disable"
    },
    "mode": {
        "gateway":"",
        "dgateway":""
    }
}
```

The **`project`** map is consulted during **`land@machine.setup`** to skip selected projects. The **`mode`** map restricts **`network@frame.mode_list`** when non-empty.

Examples, show all the configure
```shell
arch@custom
{
    "restart_time":"25",                       # allow restart after 25 seconds uptime
    "upgrade_time":"30",                       # online upgrade timeout is 30 seconds
    "name_custom":"enable",                    # auto-generate hostname from MAC
    "name_prefix":"SkinOS",                    # hostname prefix: SkinOS
    "name_mackey":"4",                         # use last 4 hex digits of MAC in hostname
    "nssid_custom":"enable",                   # auto-generate 2.4G SSID from MAC
    "nssid_prefix":"SkinOS",                   # 2.4G SSID prefix: SkinOS
    "nssid_mackey":"4",                        # use last 4 hex digits of MAC in SSID
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

Examples, set upgrade timeout
```shell
arch@custom:upgrade_time=120
ttrue
```

Examples, merge several fields
```shell
arch@custom|{"verify":"enable","upgrade_time":"90"}
ttrue
```

Examples, subtree merge for `project`
```shell
arch@custom:project|{"wifi":"enable","tui":"disable"}
ttrue
```

### C Code Example
```c
#include "skin/skin.h"

static int example_custom_upgrade_time(void)
{
    char buf[32];
    if (sgets_string(buf, sizeof(buf), "arch@custom", "upgrade_time") == NULL)
        return -1;
    return ssets_string("arch@custom", "90", "upgrade_time") ? 0 : -1;
}
```
