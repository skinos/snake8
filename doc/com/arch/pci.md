## arch@pci — PCI Device Scan and Driver Binding

### Overview

**`arch@pci`** scans **`/sys/bus/pci/devices`**, optionally merges **SoC-fixed** endpoints from **`soc`**, matches devices to **`pcidrv@*`** drivers, persists the binding under **`var/pcidevice`**, and powers matched objects through **`arch@gpio`** **`<object>_reset`**. Wi-Fi chips such as **`pcidrv@mt7603`** bind here and then register **`wifi@n`** / **`wifi@a`**.

- **`setup`** starts **`service`** unless **`status`** is **`disable`**
- each **`service`** cycle: GPIO power on → scan about **20 s** (10 × 2 s) → GPIO power off unmatched objects → decide retry / exit / reboot
    > already-bound devices are kept up from the last **`pcidevice`** snapshot
- match is delegated to each **`pcidrv@*`** **`pci_match`**; **`match`** JSON is optional extra hinting for those drivers
- **`soc`** entries are injected into the live device list so on-chip PCI functions can be bound even when sysfs naming differs


### Configuration reference ( arch@pci )

```json
// Attributes introduction 
{
    "status": "start the PCI service",                      // [ "disable" ], omit or other to start
    "search_time": "empty-bus cycles before action",        // [ number ], default 15
    "miss_reset2exit": "unmatched cycles before exit",      // [ number ], default 3
    "disappear_reset2reboot": "lost-device cycles before reboot", // [ number ], default 3
    "mode_reboot": "network modes that reboot on empty bus", // [ string ], semicolon-separated; default "wisp;nwisp;dwisp"
    "soc":                                                  // [ json ], extra PCI functions to merge into the scan
    {
        "pci address":                                      // [ string ]: { json }, e.g. "0000:00:00.0"
        {
            "id": "pci address",                            // [ string ]
            "vid": "PCI vendor id",                         // [ string ], e.g. "0x14c3"
            "pid": "PCI device id"                          // [ string ], e.g. "0x7981"
        }
        // "...":{ ... }  How many soc functions show how many properties
    },
    "match":                                                // [ json ], optional object hints for pcidrv@* pci_match
    {
        "object name":                                      // [ string ]: { json }
        {
            "token": ""                                     // [ string ]: [ string ]
            // "...":""  How many tokens show how many properties
        }
        // "...":{ ... }  How many objects show how many properties
    }
}
```

#### Configuration example

Example, show an MT7981 SoC PCI function
```shell
arch@pci
{
    "search_time":"15",                                     # empty bus: 15 cycles then reboot or exit
    "miss_reset2exit":"3",                                  # unmatched: exit after 3 cycles
    "disappear_reset2reboot":"3",                           # lost device: reboot after 3 cycles
    "soc":
    {
        "0000:00:00.0":
        {
            "id":"0000:00:00.0",
            "vid":"0x14c3",                                 # MediaTek
            "pid":"0x7981"                                  # MT7981 / MT7976 radio
        }
    }
}
```

#### Configuration settings example

Example, disable the PCI manager
```shell
arch@pci:status=disable
ttrue
```

Example, merge set miss-exit retries
```shell
arch@pci:miss_reset2exit=2
ttrue
```


### Concepts

**Restart and exit**
* Empty bus: after **`search_time`** cycles, reboot when **`network_mode`** is in **`mode_reboot`**, otherwise exit the service.
* Device count dropped: after **`disappear_reset2reboot`** cycles, reboot (**`pci_lost`**).
* Devices present but not all matched: after **`miss_reset2exit`** cycles, exit (no reboot).
* Driver **`pci_match`** returns **`terror`**: reboot after 60 s (**`pci_crack`**).


### API Reference

#### Management APIs

+ `setup[]` **start the PCI search service**
    - failed return tfalse
    - succeed return ttrue
    - Returns **`ttrue`** without starting when **`status`** is **`disable`**
    - Product **`init`** → **`bus`** calls this

+ `shut[]` **stop the PCI search service**
    - failed return tfalse
    - succeed return ttrue

+ `service[]` **one search cycle: power, scan, match, persist**
    - failed return tfalse
    - succeed return ttrue
    - Returns **`terror`** when a reboot is scheduled
    - Started by **`setup`**; not intended for manual invocation


#### Query APIs

+ `devlist[]` **list current PCI devices plus soc entries**
    - failed return NULL
    - succeed return [ json ], sysfs devices merged with **`soc`**
    ```json
    {
        "pci address":                                      // [ string ]: { json }
        {
            "id": "pci address",                            // [ string ]
            "vid": "PCI vendor id",                         // [ string ]
            "pid": "PCI device id",                         // [ string ]
            "syspath": "sysfs directory"                    // [ string ], omitted on soc-only entries that lack sysfs
        }
        // "...":{ ... }  How many devices show how many properties
    }
    ```

    Example, list PCI devices
    ```shell
    arch@pci.devlist
    {
        "0000:00:00.0":
        {
            "id":"0000:00:00.0",
            "vid":"0x14c3",
            "pid":"0x7981",
            "syspath":"/sys/bus/pci/devices/0000:00:00.0"
        }
    }
    ```

+ `status[]` **last persisted binding snapshot**
    - failed return NULL
    - succeed return [ json ], contents of **`var/pcidevice`**
    ```json
    {
        "pci address":                                      // [ string ]: { json }
        {
            "id": "pci address",                            // [ string ]
            "vid": "PCI vendor id",                         // [ string ]
            "pid": "PCI device id",                         // [ string ]
            "syspath": "sysfs directory",                   // [ string ]
            "object": "bound object name"                   // [ string ], e.g. "wifi@n"
        }
        // "...":{ ... }  How many bound devices show how many properties
    }
    ```

    Example, show the last PCI binding
    ```shell
    arch@pci.status
    {
        "0000:01:00.0":
        {
            "id":"0000:01:00.0",
            "vid":"0x14c3",
            "pid":"0x7603",
            "syspath":"/sys/bus/pci/devices/0000:01:00.0",
            "object":"wifi@n"
        }
    }
    ```


#### Control APIs

+ `reset[ object ]` **reset one bound PCI object**
    - object ------------- [ string ], bound object such as **`wifi@n`**
    - failed return tfalse
    - succeed return ttrue
    - Spawns **`resetd`** as **`<object>-resetd`**

    Example, reset the 2.4 GHz radio object
    ```shell
    arch@pci.reset[ wifi@n ]
    ttrue
    ```

+ `resetd[ object ]` **reset worker: shut driver, power cycle, restart search**
    - object ------------- [ string ], bound object
    - failed return terror
    - succeed return ttrue
    - Stops **`service`**, calls **`pci_shutdown`** on the bound **`pcidrv@*`**, GPIO power off, then starts **`service`** again
    - Started by **`reset`**; not intended for manual invocation
