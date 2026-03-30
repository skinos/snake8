## arch@pci — PCI device scan and driver binding

PCIe device discovery, match to **`pcidrv@*`** components, **`devlist` / `status` / `reset` / `resetd`**, same service pattern as **`arch@usb`** and **`arch@sdio`**. Implementation: [`pci/pci.c`](pci/pci.c).

### Configuration ( `arch@pci` )
```json
// Attributes introduction
{
    "search_time":"failure count before action",         // [ number ], default 15
    "miss_reset2exit":"tries before giving up",          // [ number ], default 3
    "disappear_reset2reboot":"disconnect events before reboot", // [ number ], default 3
    "mode_reboot":"semicolon-separated network_mode list"      // [ string ], modes that trigger reboot on device loss
}
```
JSON keys mirror the USB/SDIO family (`search_time`, `miss_reset2exit`, `disappear_reset2reboot`, `mode_reboot`, vendor/device match tables, …). Consult [`pci/pci.c`](pci/pci.c) and your ODM **`pci.cfg`** if shipped.

Examples, show all the configure
```shell
arch@pci
{
    "search_time":"15",                        # scan up to 15 rounds before action
    "miss_reset2exit":"3",                     # give up after 3 reset attempts
    "disappear_reset2reboot":"3"               # reboot after 3 device-disappear events
}
```

Examples, merge policy fields
```shell
arch@pci|{"miss_reset2exit":"2"}
ttrue
```

### Component API
+ `devlist[]` **PCI device list JSON**.

+ `status[]` **last persisted binding snapshot**.

+ `reset[ object ]` / `resetd[ … ]` **reset pipeline**.

### Lifecycle API
+ `setup[]` **start PCI `service`**, *succeed return `ttrue`*
    - Stock **`odm/rk3568/prj.json`** → **`init` → `device` → `arch@pci.setup`**.

+ `shut[]` **stop PCI supervision**.


### C Code Example
```c
#include "skin/skin.h"

static void example_pci_devlist(void)
{
    talk_t ret = scall("arch@pci", "devlist", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
