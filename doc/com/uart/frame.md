## uart@frame — UART Infrastructure

### Overview

The UART infrastructure component manages serial port instances and their driver bindings. It implements a device-driver separation architecture where the framework creates and manages port instances (`uart@tty`, `uart@tty2`, …), and each instance is bound to a driver (`uartdrv@dtu`, `uartdrv@tui`, …) that implements the actual business logic.
- list UART ports with their device and driver bindings
- bind and start an instance when a hardware device appears (`register`), with optional `ttydev` / `drvcom` / instance name
- stop and unbind when the device disappears (`unregister`)
- resolve domain names to IP addresses for DTU client connections


### Device-Driver Separation Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                   uart@frame                                        │
│                          (UART Infrastructure Manager)                              │
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
│  │   uart@tty      │  │   uart@tty2     │  │   uart@tty3     │  ...                 │
│  │                 │  │                 │  │                 │                      │
│  │  ttydev ─────────┼─▶│  ttydev ─────────┼─▶│  ttydev ─────────┼─▶ /dev/ttyS0      │
│  │  devcom ─────────┼─▶│  devcom ─────────┼─▶│  devcom ─────────┼─▶ usb@tty-2-32    │
│  │  drvcom ─────────┼─▶│  drvcom ─────────┼─▶│  drvcom ─────────┼─▶ uartdrv@dtu     │
│  │                 │  │                 │  │                 │                      │
│  │  speed, parity  │  │  speed, parity  │  │  speed, parity  │                      │
│  │  databit, stop  │  │  databit, stop  │  │  databit, stop  │                      │
│  │  flow, dtu cfg  │  │  flow, dtu cfg  │  │  flow, dtu cfg  │                      │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘                      │
│           │                    │                    │                               │
└───────────┼────────────────────┼────────────────────┼───────────────────────────────┘
            │                    │                    │
            │ sstarts(object, drvcom, "service", object, ttydev)
            ▼                    ▼                    ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              Driver Layer (Drivers)                                 │
│                                                                                     │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐                      │
│  │  uartdrv@dtu    │  │  uartdrv@tui    │  │  uartdrv@xxx    │  ...                 │
│  │                 │  │                 │  │                 │                      │
│  │  service[]      │  │  service[]      │  │  service[]      │                      │
│  │    ↓            │  │    ↓            │  │    ↓            │                      │
│  │  DTU forwarding │  │  Terminal access│  │  Custom logic   │                      │
│  │  (serial↔net)   │  │  (serial↔CLI)   │  │                 │                      │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘                      │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              Hardware Layer (Devices)                               │
│                                                                                     │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐                      │
│  │  usb@tty-2-32   │  │  usb@tty-2-3    │  │  soc@uart0      │  ...                 │
│  │  (USB serial)   │  │  (USB serial)   │  │  (SoC UART)     │                      │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘                      │
│           │                    │                    │                               │
│           ▼                    ▼                    ▼                               │
│      /dev/ttyUSB8         /dev/ttyUSB3         /dev/ttyS0                           │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

**Relationships:**

| From | To | Relationship | Field/Mechanism |
|------|-----|--------------|-----------------|
| `uart@frame` | `uart@tty*` | manages | `register()` / `unregister()` / `setup()` / `shut()` |
| `uart@tty*` | Linux TTY device | references | `ttydev` register (e.g. `/dev/ttyS0`) |
| `uart@tty*` | `usb@tty-*` / `soc@uart*` | references | `devcom` register (device component name) |
| `uart@tty*` | `uartdrv@*` | references | `drvcom` config field (driver object name) |
| `uart@frame` | `uartdrv@*` | launches | `sstarts(object, drvcom, "service", object, ttydev)` (except `uartdrv@gnss`) |
| `uart@tty*` | `gnss@frame` | handoff | when `drvcom` is `uartdrv@gnss`: `register` / `unregister` |
| `usb@tty-*` | `uart@frame` | triggers | `uart@frame.register[devcom]` when USB device appears |



### Concepts

**Device-driver separation**

The UART subsystem separates the physical port (device) from the business logic (driver):

* **Device layer** (`uart@tty`, `uart@tty2`, …) — each instance holds the serial port configuration (speed, parity, databit, stopbit, flow) and the DTU forwarding rules (client/server/MQTT). Instances are registered by `uart@frame` at boot and stored as configuration objects.

* **Driver layer** (`uartdrv@dtu`, `uartdrv@tui`, …) — executable programs that implement the actual serial-to-network bridging or terminal access. The driver is bound to an instance via the `drvcom` configuration field. When the instance starts, the framework launches the driver executable and passes the instance name and TTY device path.

* **Binding chain** — at boot, `uart@frame.setup` iterates all configured instances. For each instance it resolves `ttydev` (the Linux serial device path, either directly or via `devcom`), reads `drvcom` (the driver object name), and starts the driver as a service: `sstarts(object, drvcom, "service", object, ttydev)`. On hotplug, `register` reuses a matching bound instance or claims the first unbound slot (`devcom` empty means unbound).

* **GNSS handoff** — `uartdrv@gnss` is a **drvcom marker only** (no UART exe). When set, `setup` calls `gnss@frame.register[bind, ttydev, gnssdrv@nmea]` (`bind` is `devcom` if set, else the UART object name); `shut` calls `gnss@frame.unregister[bind]`. GNSS owns parse/bridge via `gnssdrv@nmea`.

**Instance naming** — instances are named `uart@tty`, `uart@tty2`, `uart@tty3`, … up to `uart@tty8`. The suffix number corresponds to the hardware UART port. Hotplug may also bind a named instance via `uart@frame.register[devcom, ttydev, drvcom, object]`.

**Driver objects** — drivers are registered in `prj.json` under `obj` as `uartdrv@dtu`, `uartdrv@tui`, etc. The `drvcom` field in the instance configuration references these object names. Each driver exposes a `service` entry point that the framework calls to start the driver process.



### API Reference

#### Management APIs

+ `setup[]` **initialize the UART infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - iterates all configured UART instances, registers each with `com_register`, and calls `setup` on each

+ `shut[]` **shut down the UART infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - calls `shut` on every registered instance, then unregisters each

#### Control APIs

+ `register[ devcom, ttydev, drvcom, object ]` **bind/start a UART instance for a hotplugged device**
    - devcom ----------- [ string ], required, device component name (e.g. usb@tty-2-32)
    - ttydev ----------- [ string ], optional, Linux serial device path
    - drvcom ----------- [ string ], optional, driver object name (e.g. uartdrv@dtu)
    - object ----------- [ string ], optional, instance name (e.g. uart@tty)
    - failed return tfalse, no free/matching instance or setup failed
    - succeed return ttrue
    - if `object` is given: write provided fields into that instance register, `com_register`, then `setup`
    - else if an instance already has the same `devcom` (register, else config): optionally refresh `ttydev`/`drvcom`, then `setup`
    - else claim the first unbound instance (empty `devcom` in both register and config), write `devcom` (and optional `ttydev`/`drvcom`), then `setup`
    - a non-empty `devcom` means the instance is already bound

    Example, auto-bind an unbound uart@tty and start it
    ```shell
    uart@frame.register[ usb@tty-2-3 ]
    ttrue
    ```

    Example, bind a named instance with ttydev and drvcom
    ```shell
    uart@frame.register[ usb@tty-2-32, /dev/ttyUSB8, uartdrv@dtu, uart@tty ]
    ttrue
    ```

+ `unregister[ devcom ]` **stop and unbind the UART instance for a removed device**
    - devcom ----------- [ string ], the device component name to match
    - failed return tfalse, no matching instance or shut failed
    - succeed return ttrue
    - matches register `devcom` only (set by `register` or by instance `setup` from config)
    - after `shut`, clears the register `devcom` so the slot can be claimed again

    Example, stop and unbind the instance bound to usb@tty-2-3
    ```shell
    uart@frame.unregister[ usb@tty-2-3 ]
    ttrue
    ```

#### Query APIs

+ `list[]` **list all registered UART instances with their bindings**
    - failed return NULL
    - succeed return [ json ], a map of instance name to binding information
    ```json
    {
        "instance name":                       // [ string ]: { json }, UART instance name (e.g. uart@tty, uart@tty2)
        {                                          // instance binding information
            "ttydev": "serial device path",    // [ string ], the Linux serial device path
            "devcom": "device component",      // [ string ], the device component that provides ttydev
            "drvcom": "driver component"       // [ string ], the driver object name bound to this instance
        }
        // "...":{...}  How many instances show how many properties
    }
    ```

    Example, list all UART instances
    ```shell
    uart@frame.list
    {
        "uart@tty":
        {
            "ttydev":"/dev/ttyS0",
            "drvcom":"uartdrv@dtu"
        },
        "uart@tty2":
        {
            "ttydev":"/dev/ttyUSB8",
            "devcom":"usb@tty-2-3",
            "drvcom":"uartdrv@dtu"
        }
    }
    ```

+ `domain2ip[ domain, timeout ]` **resolve a domain name to IPv4 address**
    - domain ----------- [ string ], the domain name to resolve
    - timeout ---------- [ string ], optional, timeout in seconds, default be 10
    - failed return terror, resolution failed
    - succeed return ttrue, result stored in register: `reg_set_string(this, domain, ip)`

    Example, resolve a domain name with 20 second timeout
    ```shell
    uart@frame.domain2ip[ www.example.com, 20 ]
    ttrue
    ```
