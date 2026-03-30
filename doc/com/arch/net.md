## arch@net — Network frame binding

Maps **operation-mode-specific** topology JSON (bridges, `extern`, `connect`, …) into the running **`network_mode`** slice. The object is **`arch@net`**; configuration is **sharded by mode** (same pattern as `arch@ethernet`). Implementation: [`net/net.c`](net/net.c).

### Configuration ( `arch@net` )
The **saved configuration object** for `arch@net` (query/set via `arch@net`, `arch@net:path`, merge `|{json}`, etc.). Reads and writes are keyed by register **`network_mode`**; if that register is empty, **`_get` / `_set` fail**.

```json
// Attributes introduction
// Top-level keys are mode names (e.g. "default", "gateway", "dgateway", …).
// Shape is product-specific; see ODM net.cfg (e.g. odm/rk3568/net.cfg).
{
    "default": {
        "bridge": { },                         // bridge configuration per mode
        "extern": { },                         // external uplink interface mapping
        "local": { }                           // local interface mapping
    }
}
```

Examples, show all the configure (after `network_mode` is set)
```shell
arch@net
{
    "bridge":                                  # bridge configuration for current mode
    {
        "lan":                                     # LAN bridge
        {
            "member":                                  # bridge member interfaces
            {
                "ethernet@lan2":"",                        # ethernet port 2
                "ethernet@lan3":"",                        # ethernet port 3
                "wifi@nssid":"",                           # 2.4G SSID
                "wifi@assid":""                            # 5.8G SSID
            },
            "stp":"disable"                            # STP is disabled
        }
    },
    "local":                                   # local interface mapping
    {
        "ifname@lan":                              # LAN logical interface
        {
            "concom":"ifname@ethcon",                  # connection component
            "ifdev":"bridge@lan"                       # bound to bridge@lan
        }
    },
    "extern":                                  # external uplink mapping
    {
        "ifname@wan":                              # WAN logical interface
        {
            "concom":"ifname@ethcon",                  # connection component
            "ifdev":"ethernet@lan1"                    # bound to ethernet port 1
        }
    }
}
```

Examples, merge under one mode subtree
```shell
arch@net:gateway|{"bridge":{"lan":{"stp":"enable"}}}
ttrue
```


### C Code Example
```c
#include "skin/skin.h"

static int example_arch_net(void)
{
    talk_t v = sgets("arch@net", NULL);
    if (v == NULL || v <= tpanic)
        return -1;
    talk_free(v);
    return 0;
}
```
