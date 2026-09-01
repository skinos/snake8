## arch@data — Factory EEPROM and Configuration Store

### Overview

**`arch@data`** mounts **OEM / config / userdata** partitions, seeds first-boot defaults from **`arch@custom`**, and exposes **factory EEPROM** fields (model, serial, MAC, IMEI). Backup, restore, default, and factory flows honour **`arch@lock`**. Prefer the **Component API** for those flows; edit EEPROM fields with the HE configuration grammar on **`arch@data`**.

- HE **`arch@data`** reads and writes the **Factory** MTD EEPROM (not a JSON file next to the component)
    > write **`"null"`** on a string field to clear it; MAC fields accept **`XX:XX:…`** or 12 hex digits
- **`config_lock`** / **`config_unlock`** persist a separate file flag **`config`** = **`disable`** / **`enable`** and the machine register **`config_lock`**
    > that flag is not part of the EEPROM object returned by **`arch@data`**
- **`default`** / **`release`** / **`factory`** are blocked when **`arch@lock`** **`default`** is **`enable`**; **`backup`** uses **`backup`**; **`restore`** / **`restore_default`** use **`restore`**
- **`setup`** is invoked from land / product bring-up (not from the stock arch **`init`** map); **`shut`** syncs and unmounts config and interval partitions


### Configuration reference ( arch@data )

```json
// Attributes introduction 
{
    "magic": "factory magic string",                        // [ string ], 16 bytes
    "language": "system language",                          // [ string ], 4 bytes; empty EEPROM uses "cn"
    "datecode": "factory date code",                        // [ string ], 12 bytes
    "model": "product model",                               // [ string ], 16 bytes; empty uses board custom
    "cmodel": "customer model",                             // [ string ], 16 bytes
    "sn": "serial number",                                  // [ string ], 48 bytes
    "lte_imei": "first modem IMEI",                         // [ string ]
    "lte_imsi": "first modem IMSI",                         // [ string ]
    "lte2_imei": "second modem IMEI",                       // [ string ]
    "lte2_imsi": "second modem IMSI",                       // [ string ]
    "1": "factory field 1",                                 // [ string ], 64 bytes
    "2": "factory field 2",                                 // [ string ], 64 bytes
    "3": "factory field 3",                                 // [ string ], 64 bytes
    "macid": "factory MAC without colon",                    // [ string ], e.g. "00037F1238FE"
    "mac": "factory MAC with colon",                        // [ string ], e.g. "00:03:7F:12:38:FE"
    "wanmac": "WAN MAC with colon",                         // [ string ]
    "lanmac": "LAN MAC with colon",                         // [ string ]
    "oem": "OEM default-archive size"                       // [ number ], present when a default file exists
}
```

#### Configuration example

Example, show factory EEPROM fields
```shell
arch@data
{
    "language":"en",                                        # system language
    "model":"H721",                                         # product model
    "sn":"H721A0001234",                                    # serial number
    "macid":"00037F1238FE",                                 # factory MAC (no colon)
    "mac":"00:03:7F:12:38:FE",                              # factory MAC (with colon)
    "wanmac":"00:03:7F:12:38:FE",                           # WAN MAC
    "lanmac":"00:03:7F:12:38:FF"                            # LAN MAC
}
```

#### Configuration settings example

Example, set the product model
```shell
arch@data:model=H721
ttrue
```

Example, merge set language and factory MAC( include "language" "macid" )
```shell
arch@data|{"language":"en","macid":"00037F1238FE"}
ttrue
```


### API Reference

#### Management APIs

+ `setup[]` **mount partitions, seed defaults, apply OEM / custom naming**
    - failed return tfalse
    - succeed return ttrue
    - Creates project tmp / register dirs, mounts OEM / config / interval volumes, merges factory and **`arch@custom`** defaults (hostname and SSID from MAC or SN), and sets machine register **`config_lock`** when file **`config`** is **`disable`**
    - Lifecycle method called during platform bring-up

+ `shut[]` **sync and unmount config and interval partitions**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `backup[ label ]` **archive the current configuration**
    - label -------------- [ string ], optional, archive name; default is **`date +%Y%m%d_%H%M%S`**
    - failed return NULL
    - succeed return [ json ], archive file name and path
    ```json
    {
        "file": "archive file name",                        // [ string ], e.g. "skinos_config_20260831_101500.tar"
        "path": "absolute archive path"                     // [ string ], under the project tmp directory
    }
    ```
    - Blocked when **`arch@lock`** **`backup`** is **`enable`** (**`errno`** **`EPERM`**)
    - Copies the config partition, drops default / nobackup files, and tars the remainder

    Example, backup with an automatic timestamp
    ```shell
    arch@data.backup
    {
        "file":"skinos_config_20260831_101500.tar",
        "path":"/tmp/skinos/skinos_config_20260831_101500.tar"
    }
    ```

+ `backup_eeprom[]` **archive the Factory EEPROM image**
    - failed return NULL
    - succeed return [ json ], tarball name and path
    ```json
    {
        "file": "eeprom tarball name",                      // [ string ], "<hardware>_<custom>_<version>.tar.gz"
        "path": "absolute tarball path"                     // [ string ]
    }
    ```
    - Requires machine registers **`hardware`**, **`custom`**, and **`version`**

    Example, backup the factory EEPROM
    ```shell
    arch@data.backup_eeprom
    {
        "file":"mt7621_h721_std.tar.gz",
        "path":"/tmp/skinos/mt7621_h721_std.tar.gz"
    }
    ```


#### Control APIs

+ `default[]` **mark configuration to be erased on the next default path**
    - failed return tfalse
    - succeed return ttrue
    - Blocked when **`arch@lock`** **`default`** is **`enable`**
    - Realigns **`wanmac`** / **`lanmac`** to **`mac`** when they differ, then removes the config version stamp

    Example, mark configuration for default
    ```shell
    arch@data.default
    ttrue
    ```

+ `release[]` **drop default-config files and then default**
    - failed return tfalse
    - succeed return ttrue
    - Removes **`.defaultv6`** from config and interval, marks interval for erase, then calls **`default`**

    Example, release OEM defaults and mark default
    ```shell
    arch@data.release
    ttrue
    ```

+ `factory[]` **drop OEM and default-config files and then default**
    - failed return tfalse
    - succeed return ttrue
    - Also removes OEM **`.defaultv6`** and marks OEM / interval for erase, then calls **`default`**

    Example, factory-reset stored defaults
    ```shell
    arch@data.factory
    ttrue
    ```

+ `restore[ filepath ]` **restore configuration from a backup archive**
    - filepath ----------- [ string ], path to a **`backup`** tar
    - failed return tfalse
    - succeed return ttrue
    - Blocked when **`arch@lock`** **`restore`** is **`enable`**
    - Checks platform (leading **`s`** ignored), **`hardware-custom-scope`**, and **`model`** before replacing the config partition

    Example, restore a local backup
    ```shell
    arch@data.restore[ /tmp/skinos/skinos_config_20260831_101500.tar ]
    ttrue
    ```

+ `restore_default[ filepath ]` **install an archive as the OEM default image**
    - filepath ----------- [ string ], path to a tar used as **`.defaultv6`**
    - failed return tfalse
    - succeed return ttrue
    - Blocked when **`arch@lock`** **`restore`** is **`enable`**
    - Validates platform / software / model like **`restore`**, then copies the archive to the OEM or config default slot

    Example, install a default image
    ```shell
    arch@data.restore_default[ /tmp/skinos/skinos_config_20260831_101500.tar ]
    ttrue
    ```

+ `current_default[]` **backup current config and install it as the default image**
    - failed return tfalse
    - succeed return ttrue
    - Calls **`backup`** then **`restore_default`** with the new archive path

    Example, snapshot current config as default
    ```shell
    arch@data.current_default
    ttrue
    ```

+ `config_lock[]` **lock configuration writes**
    - failed return tfalse
    - succeed return ttrue
    - Persists file **`config`** = **`disable`**, then sets machine register **`config_lock=1`**

    Example, lock configuration writes
    ```shell
    arch@data.config_lock
    ttrue
    ```

+ `config_unlock[]` **unlock configuration writes**
    - failed return tfalse
    - succeed return ttrue
    - Sets machine register **`config_lock=0`**, then persists file **`config`** = **`enable`**

    Example, unlock configuration writes
    ```shell
    arch@data.config_unlock
    ttrue
    ```
