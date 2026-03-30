## arch@usb — USB device match and modem bring-up

Supervised **`service`** loop: power/match USB devices to **`usbdrv@*`** drivers, persist state under **`var/usbdevice`**, optional **reboot/exit** when expected devices are missing. Implementation: [`usb/usb.c`](usb/usb.c). Example match table: [`odm/rk3568/usb.cfg`](odm/rk3568/usb.cfg).

### Configuration ( `arch@usb` )
```json
// Attributes introduction
{
    "status":"disable to skip starting the USB service",   // [ "disable" ], optional
    "search_time":"failure count before reboot/exit",      // [ number ], default 15 in code
    "mode_reboot":"semicolon-separated network_mode list", // default "misp;nmisp;dmisp"
    "miss_reset2exit":"tries before giving up when device set incomplete",  // default 3
    "disappear_reset2reboot":"disconnect events before reboot",             // default 3
    "match": {
        "type":"busid or product-specific",                // [ string ], match method
        "modem@lte": { "fd880000": "" }                    // USB bus-id to driver mapping
    }
}
```

Examples, show all the configure
```shell
arch@usb
{
    "search_time":"15",                        # scan up to 15 rounds before action
    "miss_reset2exit":"3",                     # give up after 3 reset attempts
    "disappear_reset2reboot":"3",              # reboot after 3 device-disappear events
    "mode_reboot":"misp;nmisp;dmisp",          # reboot only in these network modes
    "match":                                   # USB device match table
    {
        "type":"busid",                            # match method: by USB bus ID
        "modem@lte":                               # match rules for first LTE modem driver
        {
            "fd880000":"",                             # USB bus fd880000
            "fd8c0000":""                              # USB bus fd8c0000
        },
        "modem@lte2":                              # match rules for second LTE modem driver
        {
            "fd800000":"",                             # USB bus fd800000
            "fd840000":""                              # USB bus fd840000
        }
    }
}
```

Examples, disable USB manager
```shell
arch@usb:status=disable
ttrue
```

Examples, merge match table
```shell
arch@usb|{"match":{"type":"busid","modem@lte":{"fd880000":""}}}
ttrue
```

### Component API
+ `hublist[]` **USB hub topology JSON**.

+ `devlist[]` **current device list JSON** (VID/PID/bus id, …).

+ `status[]` **last persisted `usbdevice` JSON** from **`var`**.

+ `reset[ object ]` **spawn `resetd` child** for a bound driver object.

+ `resetd[ … ]` **internal worker** for reset path (see code).

### Lifecycle API
+ `setup[]` **start `service` unless `status` is `disable`**, *succeed return `ttrue`*
    - Stock **`odm/rk3568/prj.json`** → **`init` → `bus` → `arch@usb.setup`**.

+ `shut[]` **stop supervision**, *succeed return `ttrue`*
    - **Not** in stock **`uninit`** for this sample **`prj.json`**.


### C Code Example
```c
#include "skin/skin.h"

static void example_usb_list(void)
{
    talk_t ret = scall("arch@usb", "devlist", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
