## arch@gpio — GPIO and LED control

Board GPIO numbering, LED patterns, **`in` / `out` / `timer` / `listen` / `action`**, and a long-running **`service`** for netlink/button handling. Implementation: [`gpio/gpio.c`](gpio/gpio.c), optional [`gpio-init.sh`](gpio-init.sh), [`odm/rk3568/gpio.cfg`](odm/rk3568/gpio.cfg) (example ODM defaults).

### Configuration ( `arch@gpio` )
The **saved configuration object** for `arch@gpio` (query/set via `arch@gpio`, `arch@gpio:path`, merge `|{json}`, etc.).

```json
// Attributes introduction
{
    "led":"disable or omit for normal LED policy",   // [ "disable" ], factory mode overrides via register
    "sys":"GPIO number for system LED",              // [ string / number as string ], ODM-specific
    "sys_out":"active level 0 or 1",                 // [ string ]
    "modem@lte_state":"93",                          // example: LTE LED GPIO map (see gpio.cfg)
    "modem@lte_state_out":"1"
}
```

Additional keys are **GPIO alias → pin number** strings consumed at **`setup[]`** and stored in the object register for use by **`in[]`** / **`out[]`**.

Examples, show all the configure
```shell
arch@gpio
{
    "sys":"92",                                # system LED GPIO pin 92
    "sys_out":"1",                             # system LED active level: high
    "modem@lte_state":"93",                    # LTE modem state LED GPIO pin 93
    "modem@lte_state_out":"1",                 # LTE state LED active level: high
    "modem@lte_reset":"6",                     # LTE modem reset GPIO pin 6
    "modem@lte_reset_out":"1",                 # LTE reset active level: high
    "modem@lte2_state":"91",                   # second LTE modem state LED GPIO pin 91
    "modem@lte2_state_out":"1",                # second LTE state LED active level: high
    "modem@lte2_reset":"148",                  # second LTE modem reset GPIO pin 148
    "modem@lte2_reset_out":"1",                # second LTE reset active level: high
    "wifi@asta_state":"90",                    # 5.8G station state LED GPIO pin 90
    "wifi@asta_state_out":"1"                  # 5.8G station state LED active level: high
}
```

Examples, merge several GPIO mappings
```shell
arch@gpio|{"sys":"92","sys_out":"1"}
ttrue
```

### Component API
**Directly callable** APIs: `arch@gpio.method` from HE / eline / HTTP `/he`.

+ `in[ name_or_number ]` **sample a GPIO input**, *returns JSON or scalar read; `NULL` / error on bad pin*
    - Pass a **numeric GPIO** or a **configured alias** from `arch@gpio` JSON.

+ `out[ name_or_level, value ]` **drive a GPIO output** (details in code — comma-separated parameters).

+ `timer[ … ]` **configure timer-style blink patterns** for named outputs (e.g. `sys`).

+ `listen[ … ]` / `action[]` **button / event hooks** via netlink (see [`gpio/gpio.c`](gpio/gpio.c)).

### Lifecycle API
+ `setup[]` **load GPIO map, apply LED policy, run `gpio-init.sh` if present, start `service`**, *succeed return `ttrue`*
    - Stock **`odm/rk3568/prj.json`** → **`init` → `arch` → `arch@gpio.setup`**.

+ `shut[]` **stop supervised `service`**, *succeed return `ttrue`*
    - Stock **`uninit` → `arch` → `arch@gpio.shut`**.

**Note:** The same **`prj.json`** references **`arch@hotplug.setup`** under **`init` → `arch`**, but stock **`com`** does not define a **`hotplug`** component in this tree—provide it in ODM/product packages if required.


### C Code Example
```c
#include "skin/skin.h"

static void example_gpio_out(void)
{
    (void)scalls("arch@gpio", "out", "power,1");
}
```
