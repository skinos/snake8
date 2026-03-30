## arch@firmware — Firmware upgrade

Flash / FPK install, **`.zz` tarball** style upgrades, TFTP, and **online** upgrade checks. Respects **`land@…`** (or lock component) **`upgrade`** lock. Emits **`machine/status`** joint events during long operations. Implementation: [`firmware/firmware.c`](firmware/firmware.c).

### Configuration ( `arch@firmware` )
**No** separate persistent JSON object for **`arch@firmware`** in stock sources—behavior is driven by **method arguments**, **register**, and **`LOCK_COM` / `CUSTOM_COM`** reads inside [`firmware/firmware.c`](firmware/firmware.c).

*(If your product adds a cfg file and wires `config_get`, document it here.)*

### Component API
+ `fpk[ filename, offset, size ]` **install an FPK** via **`FPK_COM` `install`** with optional slice (`offset`/`size` default to full file).

+ `zz[ … ]` **multi-image tarball upgrade** under `/tmp/.zztar` with per-file result JSON (see code for full argument list and **`msgs`** shape).

+ `tftp_upgrade[ … ]` **TFTP-based flash paths** for defined partition names.

+ `online_check[]` **query remote manifest**, returns JSON with **`version`**, **`url`**, **`changelog`**, … when successful.

+ `online_upgrade[]` **download and apply** remote upgrade (sets register **`machine_state`** phases; uses **`joint_calls("machine/status", NULL)`**).


### Published Joint Events
+ **`machine/status`** — emitted during online / zz-style upgrades when **`register_value_set`** updates **`machine_state`** (e.g. **`downloading`**, **`upgrading`**, **`succeed`**, **`failed`**). Implemented with **`joint_calls("machine/status", NULL)`** in [`firmware/firmware.c`](firmware/firmware.c).

### C Code Example
```c
#include "skin/skin.h"

static void example_fpk(const char *path)
{
    talk_t ret = scalls("arch@firmware", "fpk", path);
    if (ret > tpanic)
        talk_free(ret);
}
```
