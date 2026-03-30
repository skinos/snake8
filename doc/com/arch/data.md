## arch@data — Platform storage, OEM, and factory data

Mounts **factory / OEM / config / userdata** partitions, seeds default configuration, and exposes **factory EEPROM** read/write plus **backup / restore / default / factory reset** flows.

### Configuration ( `arch@data` )
```json
// Attributes introduction
{
    "language":"system language",               // [ string ], e.g. "en", "cn"
    "model":"product model string",             // [ string ], OEM model identifier
    "macid":"factory MAC address (no colon)",   // [ string ], e.g. "00037F1238FE"
    "mac":"factory MAC address (with colon)"    // [ string ], e.g. "00:03:7F:12:38:FE"
}
```

Examples, show all the configure
```shell
arch@data
{
    "language":"en",                           # system language is English
    "model":"R8",                              # product model: R8
    "macid":"00037F1238FE",                    # factory MAC (no colon format)
    "mac":"00:03:7F:12:38:FE"                  # factory MAC (with colon format)
}
```

Examples, set model string
```shell
arch@data:model=MYMODEL
ttrue
```

Examples, merge
```shell
arch@data|{"language":"en","macid":"00037F1238FE"}
ttrue
```

### Component API
+ `default[]` **mark configuration for erase on next default path** (honours **`land@…` lock `default`** when enabled).

+ `backup[ [timestamp_label] ]` **archive configuration** to the backup area; optional label argument.

+ `restore[ archive_path ]` **restore from backup** (validates platform/software/model compatibility in code).

+ `release[]` / `factory[]` **factory-flow helpers** (see logs and lock checks in [`data/data.c`](data/data.c)).

+ `restore_default[]` / `current_default[]` **default-image management**.

+ `backup_eeprom[]` **EEPROM / serial backup read** (returns JSON; **`talk_free`** when applicable).

### Lifecycle API
+ `setup[]` **create directory tree, mount MMC partitions, merge OEM/factory/custom defaults** — heavy platform bring-up (see [`data/data.c`](data/data.c)).
    - **Not** listed in stock **`odm/rk3568/prj.json`**; invoked from **land** / product **`init`** chains on real images.

+ `shut[]` **sync and unmount config + interval partitions**.

### C Code Example
```c
#include "skin/skin.h"

static int example_factory_mac(void)
{
    return ssets_string("arch@data", "00037F1238FE", "macid") ? 0 : -1;
}
```
