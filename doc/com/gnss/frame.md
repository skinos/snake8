## gnss@frame — GNSS Infrastructure

### Overview

Manage GNSS port instances and their driver bindings. Prefer the **Component API** below instead of editing raw configuration when possible.
- list instances with `ttydev` / `devcom` / `drvcom` bindings
- bind and start an instance when a hardware device appears (`register`), with optional `ttydev` / `drvcom` / instance name
- stop and unbind when the device disappears (`unregister`)
- start the bound `gnssdrv@*` driver via `sstarts` when an instance is enabled


### Device-Driver Separation Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                   gnss@frame                                        │
│                          (GNSS Infrastructure Manager)                              │
│                                                                                     │
│  register(devcom [, ttydev, drvcom, object]) / unregister(devcom)                            │
│  list[]                                                                             │
└──────────────────────────────┬──────────────────────────────────────────────────────┘
                               │
                               │ manages (register/unregister/setup/shut)
                               ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              Device Layer (Instances)                               │
│                                                                                     │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐                      │
│  │   gnss@nmea     │  │   gnss@nmea2    │  │   gnss@nmea3    │  ...                 │
│  │                 │  │                 │  │                 │                      │
│  │  ttydev ─────────┼─▶│  ttydev ─────────┼─▶│  ttydev ─────────┼─▶ /dev/ttyUSB0    │
│  │  devcom ─────────┼─▶│  devcom ─────────┼─▶│  devcom ─────────┼─▶ usb@tty-2-32    │
│  │  drvcom ─────────┼─▶│  drvcom ─────────┼─▶│  drvcom ─────────┼─▶ gnssdrv@nmea    │
│  │                 │  │                 │  │                 │                      │
│  │  nmea {…}       │  │  nmea {…}       │  │  nmea {…}       │                      │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘                      │
│           │                    │                    │                               │
└───────────┼────────────────────┼────────────────────┼───────────────────────────────┘
            │                    │                    │
            │ sstarts(object, drvcom, "service", object, ttydev)
            ▼                    ▼                    ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              Driver Layer (Drivers)                                 │
│                                                                                     │
│  ┌─────────────────┐  ┌─────────────────┐                                           │
│  │  gnssdrv@nmea   │  │  gnssdrv@xxx    │  ...                                      │
│  │  service[]      │  │  service[]      │                                           │
│  └─────────────────┘  └─────────────────┘                                           │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              Hardware Layer (Devices)                               │
│                                                                                     │
│  ┌─────────────────┐  ┌─────────────────┐                                           │
│  │  usb@tty-2-32   │  │  soc@uart0      │  ...                                      │
│  └────────┬────────┘  └────────┬────────┘                                           │
│           ▼                    ▼                                                    │
│      /dev/ttyUSB8         /dev/ttyS0                                                │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

**Relationships:**

| From | To | Relationship | Field/Mechanism |
|------|-----|--------------|-----------------|
| `gnss@frame` | `gnss@nmea*` | manages | `register()` / `unregister()` / `setup()` / `shut()` |
| `gnss@nmea*` | Linux TTY device | references | `ttydev` register (e.g. `/dev/ttyUSB0`) |
| `gnss@nmea*` | `usb@tty-*` / `soc@uart*` | references | `devcom` register |
| `gnss@nmea*` | `gnssdrv@*` | references | `drvcom` config field |
| `gnss@frame` | `gnssdrv@*` | launches | `sstarts(object, drvcom, "service", object, ttydev)` |
| `usb@tty-*` | `gnss@frame` | triggers | `gnss@frame.register[devcom]` when the device appears |



### Configuration reference ( gnss@nmea )

Instance configuration is stored under each `gnss@nmea` … `gnss@nmea8` object (not under `gnss@frame`).

```json
// Attributes introduction
{
    "status":"instance status",              // [ "disable", "enable" ], default "disable"
    "ttydev":"serial device path",           // [ string ], optional Linux TTY path (e.g. "/dev/ttyUSB0")
    "devcom":"device component",             // [ string ], optional hardware object that provides ttydev
    "drvcom":"driver component",             // [ string ], driver object (e.g. "gnssdrv@nmea")
    "extern":"network depend",               // [ "default", ifname object, "" ], joint reset binding
    "nmea":                                  // [ json ], driver subtree for gnssdrv@nmea (see gnssdrv@nmea doc)
    {
        "tty_cmd":"startup serial commands", // [ string ], semicolon-separated
        "fix_timeout":"fix watchdog seconds",  // [ number ], default "30"; no GGA or RMC triggers power
        "nmea_sock":"local NMEA unix forward", // [ "disable", "enable" ], default "disable"; enable uses project_var_path("%s.nmea", object)

        "client":                            // [ json ], TCP/UDP client slot 1
        {
            "status":"client enable",        // [ "disable", "enable" ]
            "extern":"reset on network",     // [ "default", ifname object, "" ]
            "proto":"protocol",              // [ "tcp", "udp" ]
            "server":"server address",       // [ string ]
            "port":"server port",            // [ string ]
            "filter":"sentence type filter", // [ string ], e.g. "GGA;RMC"; empty = all
            "head":"prefix before cycle"  // [ string ], optional
        }
        // "...":{...}  How many client slots (client, client2, ..., client9) show how many properties

        "server":                            // [ json ], TCP/UDP server slot 1
        {
            "status":"server enable",        // [ "disable", "enable" ]
            "proto":"protocol",              // [ "tcp", "udp" ]
            "port":"listen port",            // [ string ]
            "limit":"max TCP clients",       // [ number ], TCP only, default "5"
            "timeout":"idle timeout",        // [ number ], optional
            "filter":"sentence type filter", // [ string ], e.g. "GGA;RMC"; empty = all
            "head":"prefix before cycle"  // [ string ], optional
        }
        // "...":{...}  How many server slots (server, server2, ..., server9) show how many properties
    }
}
```

#### Configuration example

Example, disabled first GNSS slot
```shell
gnss@nmea
{
    "status":"disable",
    "ttydev":"",
    "devcom":"",
    "drvcom":"gnssdrv@nmea",
    "extern":"default",
    "nmea":
    {
        "tty_cmd":"",
        "fix_timeout":"30",
        "nmea_sock":"disable",
        "client":
        {
            "status":"disable",
            "proto":"tcp",
            "server":"",
            "port":"",
            "filter":"",
            "head":""
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5",
            "filter":"",
            "head":""
        }
    }
}
```

#### Configuration settings example

Example, bind a USB serial GNSS module
```shell
gnss@nmea:ttydev=/dev/ttyUSB0
ttrue
```

```shell
gnss@nmea:drvcom=gnssdrv@nmea
ttrue
```



### Concepts

**Device-driver separation**

The GNSS subsystem separates the logical port (device instance) from protocol applications (drivers):

* **Device layer** (`gnss@nmea`, `gnss@nmea2`, …) — each instance holds `ttydev` / `devcom` / `drvcom` and the driver subtree (key equals `gnssdrv@` suffix, e.g. `nmea`). Instances are registered by `gnss@frame` at boot via `com_register` onto `gnss@frame`.

* **Driver layer** (`gnssdrv@nmea`, …) — executables registered in `prj.json` under `obj`. When the instance starts, the framework launches `drvcom.service[object,ttydev]`.

* **Hardware layer** — Linux TTY paths and/or device components (`usb@tty-*`, `soc@uart*`). Hotplug callers invoke `gnss@frame.register[devcom]` / `unregister[devcom]`. Empty `devcom` means unbound (eligible for auto-claim on `register`); non-empty means already bound.

**Binding chain** — at boot, `gnss@frame.setup` iterates `config_list("gnss")`. For each configured object it registers the instance and calls `setup`. Disabled unbound instances return without starting a driver; a runtime bind (`register` / UART handoff writing `reg.devcom`/`reg.ttydev`) may still start. On hotplug, `register` reuses a matching bound instance or claims the first unbound slot.

**UART handoff** — when an `uart@tty*` instance uses `drvcom=uartdrv@gnss`, UART `setup`/`shut` call `gnss@frame.register` / `unregister` (bind key = UART `devcom` or UART object name) with `drvcom=gnssdrv@nmea`.



### API Reference

#### Management APIs

+ `setup[]` **initialize the GNSS infrastructure**   
    - failed return tfalse
    - succeed return ttrue
    - iterates all configured GNSS instances, registers each with `com_register`, and calls `setup` on each

+ `shut[]` **shut down the GNSS infrastructure**   
    - failed return tfalse
    - succeed return ttrue
    - calls `shut` on every configured instance, then unregisters each


#### Control APIs

+ `register[ devcom, ttydev, drvcom, object ]` **bind/start a GNSS instance for a hotplugged device**   
    - devcom ----------- [ string ], required, device component name (e.g. usb@tty-2-32)
    - ttydev ----------- [ string ], optional, Linux serial device path
    - drvcom ----------- [ string ], optional, driver object name (e.g. gnssdrv@nmea)
    - object ----------- [ string ], optional, instance name (e.g. gnss@nmea)
    - failed return tfalse, no free/matching instance or setup failed
    - succeed return ttrue
    - if `object` is given: write provided fields into that instance register, `com_register`, then `setup`
    - else if an instance already has the same `devcom` (register, else config): optionally refresh `ttydev`/`drvcom`, then `setup`
    - else claim the first unbound instance (empty `devcom` in both register and config), write `devcom` (and optional `ttydev`/`drvcom`), then `setup`
    - a non-empty `devcom` means the instance is already bound

    Example, auto-bind an unbound gnss@nmea and start it
    ```shell
    gnss@frame.register[ usb@tty-2-32 ]
    ttrue
    ```

    Example, bind a named instance with ttydev and drvcom
    ```shell
    gnss@frame.register[ usb@tty-2-32, /dev/ttyUSB8, gnssdrv@nmea, gnss@nmea ]
    ttrue
    ```

+ `unregister[ devcom ]` **stop and unbind the GNSS instance for a removed device**   
    - devcom ----------- [ string ], the device component name to match
    - failed return tfalse, no matching instance or shut failed
    - succeed return ttrue
    - matches register `devcom` only (set by `register` or by instance `setup` from config)
    - after `shut`, clears the register `devcom` so the slot can be claimed again

    Example, stop and unbind the instance bound to usb@tty-2-32
    ```shell
    gnss@frame.unregister[ usb@tty-2-32 ]
    ttrue
    ```


#### Query APIs

+ `list[]` **list all registered GNSS instances with their bindings**   
    - failed return NULL
    - succeed return [ json ], map of instance name to binding information
    - only objects that have a config (`config_sget`) are listed; infrastructure such as `gnss@frame` itself is omitted

    ```json
    {
        "instance name":                       // [ string ]: { json }, GNSS instance (e.g. gnss@nmea)
        {
            "ttydev": "serial device path",    // [ string ], Linux serial device path
            "devcom": "device component",      // [ string ], device component that provides ttydev
            "drvcom": "driver component"       // [ string ], driver object bound to this instance
        }
        // "...":{...}  How many instances show how many properties
    }
    ```

    Example, list all GNSS instances
    ```shell
    gnss@frame.list
    {
        "gnss@nmea":
        {
            "ttydev":"/dev/ttyUSB0",
            "drvcom":"gnssdrv@nmea"
        },
        "gnss@nmea2":
        {
            "ttydev":"/dev/ttyUSB8",
            "devcom":"usb@tty-2-32",
            "drvcom":"gnssdrv@nmea"
        }
    }
    ```
