## arch@lock — Feature Locks

### Overview

**`arch@lock`** is a **configuration object** of safety switches. **`arch@data`**, **`arch@firmware`**, and **`land@machine`** read it through **`config_sget`** (macro **`LOCK_COM`** → **`arch@lock`**). Query and change it with the HE configuration grammar; there are no method APIs.

- **`upgrade`**: blocks **`arch@firmware`** **`fpk`** / **`zz`** and upgrade paths that apply firmware
- **`default`**: blocks **`arch@data`** **`default`** / **`release`** / **`factory`** and related machine flows
- **`backup`**: blocks **`arch@data.backup`**
- **`restore`**: blocks **`arch@data.restore`** and **`restore_default`**
    > the global configuration **write** lock is not here; use **`arch@data.config_lock`** / **`config_unlock`**


### Configuration reference ( arch@lock )

```json
// Attributes introduction 
{
    "upgrade": "firmware upgrade lock",                     // [ "disable","enable" ]
                                          // "disable": upgrades allowed (default when unset)
                                          // "enable": arch@firmware rejects apply paths
    "default": "factory-default lock",                      // [ "disable","enable" ]
                                          // "enable": blocks arch@data default / release / factory
    "backup": "configuration backup lock",                  // [ "disable","enable" ]
                                          // "enable": blocks arch@data.backup
    "restore": "configuration restore lock"                 // [ "disable","enable" ]
                                          // "enable": blocks restore and restore_default
}
```

#### Configuration example

Example, show all feature locks
```shell
arch@lock
{
    "upgrade":"disable",                                    # upgrades allowed
    "default":"disable",                                    # default / factory allowed
    "backup":"disable",                                     # backup allowed
    "restore":"disable"                                     # restore allowed
}
```

#### Configuration settings example

Example, enable the upgrade lock
```shell
arch@lock:upgrade=enable
ttrue
```

Example, merge set upgrade and backup locks( include "upgrade" "backup" )
```shell
arch@lock|{"upgrade":"enable","backup":"enable"}
ttrue
```


### Concepts

**Configuration-only object**
* **`arch@lock`** has no Management / Query / Control method APIs.
* Read the whole object with **`arch@lock`**; set one field with **`arch@lock:key=value`**; merge with **`arch@lock|{…}`**.
* **`"enable"`** means the named function is **locked** (rejected). Unset or **`"disable"`** means allowed.
