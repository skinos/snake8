## arch@lock — Feature locks

Global **safety switches** that **`arch@data`**, **`arch@firmware`**, and **`land@machine`** consult via **`config_sget( LOCK_COM, … )`**. Macro **`LOCK_COM`** → **`arch@lock`**.

Global **configuration write lock** is **not** here: use **`arch@data.config_lock` / `config_unlock`** and machine reg **`config_lock`** (see [`data.md`](data.md)).

### Configuration ( `arch@lock` )
```json
// Attributes introduction
{
    "upgrade":"disable or enable upgrade lock",          // [ "disable", "enable" ] — blocks `arch@firmware` entry points
    "default":"disable or enable factory-default lock",  // [ "disable", "enable" ] — blocks `arch@data` default / release / factory and related machine flows
    "backup":"disable or enable backup lock",            // [ "disable", "enable" ]
    "restore":"disable or enable restore lock"           // [ "disable", "enable" ] — restore and restore_default on `arch@data`
}
```

Examples, show all the configure
```shell
arch@lock
{
    "upgrade":"disable",                       # upgrade lock: off
    "default":"disable",                       # factory-default lock: off
    "backup":"disable",                        # backup lock: off
    "restore":"disable"                        # restore lock: off
}
```

Examples, enable upgrade lock
```shell
arch@lock:upgrade=enable
ttrue
```

Examples, merge
```shell
arch@lock|{"upgrade":"enable","backup":"enable"}
ttrue
```

### C Code Example
```c
#include "skin/skin.h"

static boole upgrade_locked(void)
{
    char buf[16];
    const char *p = sgets_string(buf, sizeof(buf), "arch@lock", "upgrade");
    return (p != NULL && strcmp(p, "enable") == 0);
}
```
