## uart@frame — UART Infrastructure

### Overview

The UART infrastructure component manages serial port instances and their driver bindings. It implements a device-driver separation architecture where the framework creates and manages port instances (`uart@tty`, `uart@tty2`, …), and each instance is bound to a driver (`uartdrv@dtu`, `uartdrv@tui`, …) that implements the actual business logic.
- register and unregister UART port instances at runtime
- list all registered UART ports with their device and driver bindings
- start and stop individual ports by device component name
- resolve domain names to IP addresses for DTU client connections



### Concepts

**Device-driver separation**

The UART subsystem separates the physical port (device) from the business logic (driver):

* **Device layer** (`uart@tty`, `uart@tty2`, …) — each instance holds the serial port configuration (speed, parity, databit, stopbit, flow) and the DTU forwarding rules (client/server/MQTT). Instances are registered by `uart@frame` at boot and stored as configuration objects.

* **Driver layer** (`uartdrv@dtu`, `uartdrv@tui`, …) — executable programs that implement the actual serial-to-network bridging or terminal access. The driver is bound to an instance via the `drvcom` configuration field. When the instance starts, the framework launches the driver executable and passes the instance name and TTY device path.

* **Binding chain** — at boot, `uart@frame.setup` iterates all configured instances. For each instance it resolves `ttydev` (the Linux serial device path, either directly or via `devcom`), reads `drvcom` (the driver object name), and starts the driver as a service: `sstarts(object, drvcom, "service", object, ttydev)`.

**Instance naming** — instances are named `uart@tty`, `uart@tty2`, `uart@tty3`, … up to `uart@tty8`. The suffix number corresponds to the hardware UART port. Additional instances can be registered dynamically via `uart@frame.register` for USB-to-serial adapters or other external ports.

**Driver objects** — drivers are registered in `prj.json` under `obj` as `uartdrv@dtu`, `uartdrv@tui`, etc. The `drvcom` field in the instance configuration references these object names. Each driver exposes a `service` entry point that the framework calls to start the driver process.



### API Reference

#### Management APIs

+ `setup[]` **initialize the UART infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - When called on `uart@frame`: iterates all configured UART instances, registers each with `com_register`, and calls `setup` on each
    - When called on `uart@tty*` (instance): reads configuration, resolves `ttydev` and `drvcom`, starts the driver service

+ `shut[]` **shut down the UART infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - When called on `uart@frame`: calls `shut` on every registered instance, then unregisters each
    - When called on `uart@tty*` (instance): stops the driver service

#### Control APIs

+ `register[ object, ttydev, devcom, drvcom ]` **register a UART instance with optional bindings**
    - object ----------- [ string ], the instance name (e.g. uart@tty3)
    - ttydev ----------- [ string ], optional, the Linux serial device path (e.g. /dev/ttyUSB8)
    - devcom ----------- [ string ], optional, the device component that provides ttydev (e.g. usb@tty-2-32)
    - drvcom ----------- [ string ], optional, the driver object name (e.g. uartdrv@dtu)
    - failed return tfalse
    - succeed return ttrue

    Example, register uart@tty3 with a USB serial device
    ```shell
    uart@frame.register[ uart@tty3, /dev/ttyUSB8 ]
    ttrue
    ```

    Example, register with all bindings specified
    ```shell
    uart@frame.register[ uart@tty-2-32, /dev/ttyUSB8, usb@tty-2-32, uartdrv@dtu ]
    ttrue
    ```

+ `unregister[ object ]` **unregister a UART instance**
    - object ----------- [ string ], the instance name to unregister
    - failed return tfalse
    - succeed return ttrue

    Example, unregister uart@tty3
    ```shell
    uart@frame.unregister[ uart@tty3 ]
    ttrue
    ```

+ `add[ devcom ]` **start the UART port whose device component matches**
    - devcom ----------- [ string ], the device component name to match against each instance's `devcom` register
    - failed return tfalse, no matching instance or setup failed
    - succeed return ttrue

    Example, start the port bound to usb@tty-2-3
    ```shell
    uart@frame.add[ usb@tty-2-3 ]
    ttrue
    ```

+ `delete[ devcom ]` **stop the UART port whose device component matches**
    - devcom ----------- [ string ], the device component name to match
    - failed return tfalse, no matching instance or shut failed
    - succeed return ttrue

    Example, stop the port bound to usb@tty-2-3
    ```shell
    uart@frame.delete[ usb@tty-2-3 ]
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
