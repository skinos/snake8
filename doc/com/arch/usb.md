## arch@usb — USB Device Match and Bring-up

### Overview

**`arch@usb`** scans **`/sys/bus/usb/devices`**, matches devices to **`usbdrv@*`** drivers, persists the binding under **`var/usbdevice`**, and powers modem USB ports through **`arch@gpio`** **`<object>_reset`**. Prefer the **Component API** for list / reset; edit the **`match`** table when the USB path or expected object changes.

- **`setup`** starts **`service`** unless **`status`** is **`disable`**
- each **`service`** cycle: GPIO power on → scan about **120 s** (40 × 3 s) → GPIO power off unmatched objects → decide retry / exit / reboot
    > already-bound devices are kept up from the last **`usbdevice`** snapshot
- match is delegated to each **`usbdrv@*`** **`usb_match`**; **`match`** JSON keys are the objects to bind (typically **`modem@lte`**, **`modem@lte2`**)
- failure policy is counted across cycles and stored on the object register


### Configuration reference ( arch@usb )

```json
// Attributes introduction 
{
    "status": "start the USB service",                      // [ "disable" ], omit or other to start
    "search_time": "empty-bus cycles before action",        // [ number ], default 10
    "miss_reset2exit": "unmatched cycles before exit",      // [ number ], default 3
    "disappear_reset2reboot": "lost-device cycles before reboot", // [ number ], default 5
    "mode_reboot": "network modes that reboot on empty bus", // [ string ], semicolon-separated; default "misp;nmisp;dmisp"
    "match":                                                // [ json ], expected objects and USB path tokens
    {
        "object name":                                      // [ string ]: { json }, e.g. "modem@lte"
        {
            "usb path": ""                                  // [ string ]: [ string ], sysfs device id such as "1-2"
            // "...":""  How many paths show how many properties
        }
        // "...":{ ... }  How many objects show how many properties
    }
}
```

#### Configuration example

Example, show a dual-modem USB match table
```shell
arch@usb
{
    "search_time":"10",                                     # empty bus: 10 cycles then reboot or exit
    "miss_reset2exit":"3",                                  # unmatched: exit after 3 cycles
    "disappear_reset2reboot":"5",                           # lost device: reboot after 5 cycles
    "mode_reboot":"misp;nmisp;dmisp",                       # reboot on empty bus only in these modes
    "match":
    {
        "modem@lte":
        {
            "1-2":"",                                       # first modem on USB path 1-2
            "1-2.2":""
        },
        "modem@lte2":
        {
            "1-1":"",                                       # second modem on USB path 1-1
            "2-1":""
        }
    }
}
```

#### Configuration settings example

Example, disable the USB manager
```shell
arch@usb:status=disable
ttrue
```

Example, merge set match paths for the first modem( include "1-2" "1-2.2" )
```shell
arch@usb|{"match":{"modem@lte":{"1-2":"","1-2.2":""}}}
ttrue
```


### Concepts

**Restart and exit**
* Empty bus: after **`search_time`** cycles, reboot when **`network_mode`** is in **`mode_reboot`**, otherwise exit the service.
* Device count dropped: after **`disappear_reset2reboot`** cycles, reboot (**`usb_lost`**).
* Devices present but not all matched: after **`miss_reset2exit`** cycles, exit (no reboot).
* Driver **`usb_match`** returns **`terror`**: reboot after 180 s (**`usb_crack`**).


### API Reference

#### Management APIs

+ `setup[]` **start the USB search service**
    - failed return tfalse
    - succeed return ttrue
    - Returns **`ttrue`** without starting when **`status`** is **`disable`**
    - Product **`init`** → **`device`** calls this

+ `shut[]` **stop the USB search service**
    - failed return tfalse
    - succeed return ttrue

+ `service[]` **one search cycle: power, scan, match, persist**
    - failed return tfalse
    - succeed return ttrue
    - Returns **`terror`** when a reboot is scheduled
    - Started by **`setup`**; not intended for manual invocation


#### Query APIs

+ `hublist[]` **list USB host controllers**
    - failed return NULL
    - succeed return [ json ], one property per **`usbN`** hub
    ```json
    {
        "hub id":                                           // [ string ]: { json }, sysfs name such as "usb1"
        {
            "id": "hub sysfs name",                         // [ string ]
            "vid": "USB vendor id",                         // [ string ], 4 hex digits
            "pid": "USB product id",                        // [ string ], 4 hex digits
            "syspath": "sysfs directory",                   // [ string ]
            "busid": "platform bus id"                      // [ string ], present when the symlink can be parsed
        }
        // "...":{ ... }  How many hubs show how many properties
    }
    ```

    Example, list USB hubs
    ```shell
    arch@usb.hublist
    {
        "usb1":
        {
            "id":"usb1",
            "vid":"1d6b",
            "pid":"0002",
            "syspath":"/sys/bus/usb/devices/usb1"
        }
    }
    ```

+ `devlist[]` **list current USB devices**
    - failed return NULL
    - succeed return [ json ], one property per device (hubs and interface nodes omitted)
    ```json
    {
        "device id":                                        // [ string ]: { json }, sysfs name such as "1-2"
        {
            "id": "device sysfs name",                      // [ string ]
            "vid": "USB vendor id",                         // [ string ], 4 hex digits
            "pid": "USB product id",                        // [ string ], 4 hex digits
            "syspath": "sysfs directory",                   // [ string ]
            "busid": "platform bus id"                      // [ string ], present when the symlink can be parsed
        }
        // "...":{ ... }  How many devices show how many properties
    }
    ```

    Example, list USB devices
    ```shell
    arch@usb.devlist
    {
        "1-2":
        {
            "id":"1-2",
            "vid":"2c7c",
            "pid":"0125",
            "syspath":"/sys/bus/usb/devices/1-2"
        }
    }
    ```

+ `status[]` **last persisted binding snapshot**
    - failed return NULL
    - succeed return [ json ], contents of **`var/usbdevice`** (device list plus bound **`object`**)
    ```json
    {
        "device id":                                        // [ string ]: { json }
        {
            "id": "device sysfs name",                      // [ string ]
            "vid": "USB vendor id",                         // [ string ]
            "pid": "USB product id",                        // [ string ]
            "syspath": "sysfs directory",                   // [ string ]
            "object": "bound object name"                   // [ string ], e.g. "modem@lte"
        }
        // "...":{ ... }  How many bound devices show how many properties
    }
    ```

    Example, show the last USB binding
    ```shell
    arch@usb.status
    {
        "1-2":
        {
            "id":"1-2",
            "vid":"2c7c",
            "pid":"0125",
            "syspath":"/sys/bus/usb/devices/1-2",
            "object":"modem@lte"
        }
    }
    ```


#### Control APIs

+ `reset[ object ]` **reset one bound USB object**
    - object ------------- [ string ], bound object such as **`modem@lte`**
    - failed return tfalse
    - succeed return ttrue
    - Spawns **`resetd`** as **`<object>-resetd`** so the caller is not killed with the service

    Example, reset the first LTE modem USB
    ```shell
    arch@usb.reset[ modem@lte ]
    ttrue
    ```

+ `resetd[ object ]` **reset worker: shut driver, power cycle, restart search**
    - object ------------- [ string ], bound object
    - failed return terror
    - succeed return ttrue
    - Stops **`service`**, calls **`usb_shutdown`** on the bound **`usbdrv@*`**, GPIO power off, then starts **`service`** again
    - Started by **`reset`**; not intended for manual invocation
