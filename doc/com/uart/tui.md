## uartdrv@tui — Eline Serial Terminal Driver

### Overview

The TUI driver attaches a serial port to the eline interactive terminal. It is registered as `uartdrv@tui` in `prj.json` and launched by `uart@tty.setup` when the instance's `drvcom` is set to `uartdrv@tui`.
- open the serial device with the instance's line settings in canonical mode
- redirect stdin/stdout/stderr to the serial file descriptor
- exec the eline program for interactive command-line access over serial



### Concepts

**Relationship to framework and instances**

`uartdrv@tui` is a **driver** in the UART device-driver separation architecture:

* **Framework** (`uart@frame`) — manages instance lifecycle. At boot, `uart@frame.setup` iterates configured instances and starts each one.
* **Instance** (`uart@tty`, `uart@tty2`, …) — holds the serial port configuration. When `drvcom` is `uartdrv@tui`, the instance's `setup` launches this driver as a service.
* **Driver** (`uartdrv@tui`) — the executable that opens the serial port in terminal mode and execs eline, providing interactive command-line access over the serial link.

**Startup chain**: `uart@frame.setup` → `uart@tty.setup` → `sstarts("uart@tty", "uartdrv@tui", "service", "uart@tty", "/dev/ttyS1")` → `uartdrv@tui.service` opens serial and execs eline.

**Difference from DTU**: the DTU driver runs a long-lived event loop bridging serial to network. The TUI driver opens the serial port in canonical (line-buffered) mode and replaces itself with eline, so the user gets an interactive shell prompt over the serial link.



### Configuration reference ( uart@tty for TUI )

When `drvcom` is `uartdrv@tui`, the following configuration fields are read from the `uart@tty` instance object. The TUI driver does not use the `dtu` subtree, `active` settings, or `frame_*` settings.

```json
// Attributes introduction 
{
    "status": "instance enable or disable",              // [ "disable", "enable" ], default be "enable"
    "ttydev": "serial device path",                      // [ string ], Linux serial device path (e.g. /dev/ttyS1)
    "devcom": "device component name",                   // [ string ], the component that provides ttydev path
    "drvcom": "driver component name",                   // [ string ], must be "uartdrv@tui" for this driver

    "speed": "serial baud rate",                         // [ "9600", "19200", "38400", "57600", "115200", ... ], default be "9600"
    "flow": "flow control type",                         // [ "disable", "hard", "soft" ], default be "disable"
    "parity": "parity mode",                             // [ "disable", "even", "odd" ], default be "disable"
    "databit": "data bits",                              // [ "5", "6", "7", "8" ], default be "8"
    "stopbit": "stop bits"                               // [ "1", "2" ], default be "1"
}
```

#### Configuration example

Example, show all the uart@tty TUI configure
```shell
land@uart@tty
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS1",                     # serial device path
    "drvcom":"uartdrv@tui",                    # bound to TUI driver
    "speed":"57600",                           # baud rate 57600
    "flow":"disable",                          # no flow control
    "parity":"disable",                        # no parity
    "databit":"8",                             # 8 data bits
    "stopbit":"1"                              # 1 stop bit
}
```

#### Configuration settings example

Example, set the serial baud rate to 115200
```shell
land@uart@tty:speed=115200
ttrue
```

Example, bind the instance to the TUI driver
```shell
land@uart@tty:drvcom=uartdrv@tui
ttrue
```



### API Reference

#### Control APIs

+ `service[ uart_object, ttydev, devcom ]` **attach the serial port to the eline terminal**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - ttydev ------------ [ string ], the Linux serial device path (e.g. /dev/ttyS1)
    - devcom ------------ [ string ], optional, the device component that provides ttydev
    - Opens the serial device in canonical mode with the instance's line settings (speed, parity, databit, stopbit, flow)
    - Redirects stdin, stdout, and stderr to the serial file descriptor
    - Execs `eline` — does not return on success
    - Returns tfalse on serial open failure
    - Normally called by `uart@tty.setup`, not invoked directly

    Example, start the TUI service for uart@tty
    ```shell
    uartdrv@tui.service[ uart@tty, /dev/ttyS1 ]
    ```
