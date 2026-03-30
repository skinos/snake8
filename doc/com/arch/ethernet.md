## arch@ethernet — SoC Ethernet / switch

Registers per-port **`ethernet@lanN`** objects with **`network@frame`**, applies **`network_mode`**-specific switch/port JSON, and exposes link-level methods (`up`, `down`, `status`, `online`, …). Root object **`arch@ethernet`**; children **`ethernet@lan1`**, … Implementation: [`ethernet/ethernet.c`](ethernet/ethernet.c). Example defaults: [`odm/rk3568/ethernet.cfg`](odm/rk3568/ethernet.cfg).

### Configuration ( `arch@ethernet` )
The **saved configuration object** for `arch@ethernet` (query/set via `arch@ethernet`, `arch@ethernet:path`, merge `|{json}`, etc.). Structure is selected by register **`network_mode`** (fallback **`NETWORK_MODE`**).

```json
// Attributes introduction
{
    "default": {
        "mode":"switch or vlan",               // [ string ], physical switching mode
        "ifdev": {                             // kernel netdev name mapping per port
            "lan1":"kernel netdev name",       // [ string ]
            "lan2":"…"                         // [ string ]
        },
        "phy": { }                             // optional PHY configuration
    }
}
```

**Child objects** `ethernet@lanN` use the **`ifdev/<name>`** branch of the same mode JSON.

Examples, show all the configure
```shell
arch@ethernet
{
    "default":                                 # default mode configuration
    {
        "mode":"switch",                           # physical switching mode: switch
        "ifdev":                                   # port-to-netdev mapping
        {
            "lan1":"lan1",                             # port lan1 maps to kernel netdev lan1
            "lan2":"lan2",                             # port lan2 maps to kernel netdev lan2
            "lan3":"lan3",                             # port lan3 maps to kernel netdev lan3
            "lan4":"wan"                               # port lan4 maps to kernel netdev wan
        }
    }
}
```

Examples, merge `ifdev` map
```shell
arch@ethernet:default|{"mode":"switch","ifdev":{"lan1":"eth0","lan2":"eth1"}}
ttrue
```

### Component API
+ `netdev[]` **return kernel interface name** for this `ethernet@…` object.

+ `up[]` / `down[]` **bring interface administratively up/down** (no IP configuration here).

+ `connect[]` / `connected[]` **link-state helpers** (`connected` checks **`IFF_RUNNING`**).

+ `status[]` **JSON link / driver status** (see return shape in code).

+ `reset[]` / `setmac[]` / `hwnat[]` / `keeplive[]` **platform hooks** as implemented in [`ethernet/ethernet.c`](ethernet/ethernet.c).

+ `online[]` / `offline[]` **notify stack when carrier appears/disappears**.

### Lifecycle API
+ `setup[]` **register ports with `network@frame`, optional VLAN table, apply MAC overrides from register**, *succeed return `ttrue`*
    - Stock **`odm/rk3568/prj.json`** → **`init` → `bus` → `arch@ethernet.setup`**.

+ `shut[]` **unregister and tear down**, *succeed return `ttrue`*
    - **Not** listed in stock **`uninit`** for this sample; call explicitly or add in product **`prj.json`**.

### Joint Handlers
**None** declared in stock **`odm/rk3568/prj.json`** for **`arch@ethernet`** (product images may map **`network/online`**-style keys to **`online[]`** / **`offline[]`**).


### C Code Example
```c
#include "skin/skin.h"

static void example_ethernet_status(void)
{
    talk_t ret = scall("ethernet@lan1", "status", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
