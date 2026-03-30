## arch@sdio — SDIO bus scan and Wi‑Fi/BT driver binding

Same structural pattern as **`arch@usb` / `arch@pci`**: supervised **`service`**, device lists, **`devlist` / `status` / `reset` / `resetd`**. Binds SDIO children such as **`sdiodrv@ap6398`** per **`prj.json` `obj`**. Implementation: [`sdio/sdio.c`](sdio/sdio.c).

### Configuration ( `arch@sdio` )
```json
// Attributes introduction
{
    "search_time":"failure count before action",         // [ number ], default 15
    "miss_reset2exit":"tries before giving up",          // [ number ], default 3
    "disappear_reset2reboot":"disconnect events before reboot", // [ number ], default 3
    "mode_reboot":"semicolon-separated network_mode list"      // [ string ], modes that trigger reboot on device loss
}
```
Product-specific JSON (search time, reset policy, match maps). Follow the keys used in [`sdio/sdio.c`](sdio/sdio.c) (`search_time`, `miss_reset2exit`, `disappear_reset2reboot`, `mode_reboot`, …) and ODM cfg if present.

Examples, show all the configure
```shell
arch@sdio
{
    "search_time":"15",                        # scan up to 15 rounds before action
    "miss_reset2exit":"5",                     # give up after 5 reset attempts
    "disappear_reset2reboot":"3"               # reboot after 3 device-disappear events
}
```

Examples, merge numeric policy
```shell
arch@sdio|{"search_time":"20","miss_reset2exit":"5"}
ttrue
```

### Component API
+ `devlist[]` **enumerated SDIO devices**.

+ `status[]` **persisted binding state** (path inside code — same family as USB).

+ `reset[ object ]` / `resetd[ … ]` **driver reset helpers**.

### Lifecycle API
+ `setup[]` **start SDIO `service`**, *succeed return `ttrue`*
    - Stock **`odm/rk3568/prj.json`** → **`init` → `device` → `arch@sdio.setup`**.

+ `shut[]` **stop SDIO supervision**.


### C Code Example
```c
#include "skin/skin.h"

static void example_sdio_status(void)
{
    talk_t ret = scall("arch@sdio", "status", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
