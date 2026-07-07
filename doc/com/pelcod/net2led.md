## uartdrv@net2led — Network Status to PTZ LED Display Driver

### Overview

The net2led driver reads uplink network status from the network framework and writes Pelco-D frames to a serial port, driving the PTZ on-screen network indicators (4G / WIFI). It is registered as `uartdrv@net2led` in `prj.json` and launched by `uart@tty.setup` when the instance's `drvcom` is set to `uartdrv@net2led`.
- load multi-link priority slots `"1"`..`"10"` from `network@frame` at startup
- every `report_interval` seconds, pick the active uplink and map status to Pelco-D
- write `FF 01 00 D3 xx yy SUM` frames to UART (9600 8N1 by default)
- support manual test of all protocol frames via IPC
- only `ifname@lte*` and `ifname@wisp*` produce serial output; `ifname@wan` and others are ignored



### Concepts

**Relationship to framework and instances**

`uartdrv@net2led` is a **driver** in the UART device-driver separation architecture:

* **Framework** (`uart@frame`) — manages instance lifecycle. At boot, `uart@frame.setup` iterates configured instances and starts each one.
* **Instance** (`uart@tty`, `uart@tty2`, …) — holds the serial port configuration and the `net2led` subtree. When `drvcom` is `uartdrv@net2led`, the instance's `setup` launches this driver as a service.
* **Driver** (`uartdrv@net2led`) — the executable that runs the periodic status poll and Pelco-D write loop.

**Startup chain**: `uart@frame.setup` → `uart@tty.setup` → `sstarts("uart@tty", "uartdrv@net2led", "service", "uart@tty", "/dev/ttyS0")` → `uartdrv@net2led.service` runs the event loop.

**Network uplink selection**

At service start, the driver calls `sget(network@frame)` and reads priority slots `"1"` through `"10"` (same as `network@connect`). On each tick:

1. Scan slots from `"1"` upward; use the **first** interface whose `status` is `"up"`.
2. If none are `"up"`, fall back to slot `"1"` status.
3. Only when the chosen `ifname` starts with `ifname@lte` or `ifname@wisp` write a Pelco-D frame; other types (e.g. `ifname@wan`) skip serial output.

**Status mapping** (see also `doc/云台显示对接协议.xlsx`):

| ifname prefix | Condition | Pelco-D (xx, yy) |
|---------------|-----------|------------------|
| `ifname@lte*` | `status` is `"up"` and `signal` is `"1"`..`"4"` | `(0x04, signal)` 4G 1~4 bars |
| `ifname@lte*` | not connected or no signal | `(0x04, 0x00)` 4G 0 bars |
| `ifname@wisp*` | `status` is `"up"` and `signal` is `"1"`..`"4"` | `(0x11, signal)` WIFI 1~4 bars |
| `ifname@wisp*` | not connected or no signal | `(0x11, 0x00)` WIFI off |

**Pelco-D frame format**: `FF 01 00 D3 xx yy SUM`, where `SUM = (0x01 + 0x00 + 0xD3 + xx + yy) & 0xFF`.

**IPC**: while running, the driver listens on a Unix datagram socket at `<var_dir>/pelcod/<uart_object>.unix`. The `test` API communicates through this socket.



### Configuration reference ( uart@tty for net2led )

When `drvcom` is `uartdrv@net2led`, the following configuration fields are read from the `uart@tty` instance object. Serial port settings are at the top level; net2led-specific parameters are under the `net2led` subtree.

```json
// Attributes introduction 
{
    "status": "instance enable or disable",              // [ "disable", "enable" ], default be "enable"
    "ttydev": "serial device path",                      // [ string ], Linux serial device path (e.g. /dev/ttyS0)
    "devcom": "device component name",                   // [ string ], the component that provides ttydev path
    "drvcom": "driver component name",                   // [ string ], must be "uartdrv@net2led" for this driver

    "speed": "serial baud rate",                         // [ "9600", "19200", "38400", "57600", "115200", ... ], default be "9600"
    "flow": "flow control type",                         // [ "disable", "hard", "soft" ], default be "disable"
    "parity": "parity mode",                             // [ "disable", "even", "odd" ], default be "disable"
    "databit": "data bits",                              // [ "5", "6", "7", "8" ], default be "8"
    "stopbit": "stop bits",                              // [ "1", "2" ], default be "1"

    "net2led":                                             // [ json ], net2led driver configuration
    {
        "report_interval": "status poll interval"        // [ number ], seconds between status polls, default be 5, minimum 1
    }
}
```

Multi-link slot order is **not** configured here; it is read from `network@frame` keys `"1"`..`"10"` at service start (see [`../../project/network/frame.md`](../../project/network/frame.md)).

#### Configuration example

Example, show all the uart@tty net2led configure
```shell
land@uart@tty
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS0",                     # serial device path
    "drvcom":"uartdrv@net2led",                # bound to net2led driver
    "speed":"9600",                            # baud rate 9600 per PTZ protocol
    "flow":"disable",                          # no flow control
    "parity":"disable",                        # no parity
    "databit":"8",                             # 8 data bits
    "stopbit":"1",                             # 1 stop bit
    "net2led":
    {
        "report_interval":"5"                  # poll every 5 seconds
    }
}
```

Example, typical network@frame multi-link slots (read by driver at startup)
```shell
network@frame
{
    "type":"hot4",
    "1":"ifname@wan",
    "2":"ifname@lte",
    "3":"ifname@wisp"
}
```

#### Configuration settings example

Example, change the report interval to 10 seconds
```shell
land@uart@tty:net2led/report_interval=10
ttrue
```

Example, merge set net2led configure
```shell
land@uart@tty|{"net2led":{"report_interval":"5"}}
ttrue
```



### Pelco-D protocol reference ( doc/云台显示对接协议.xlsx )

| Signal type | Display | Frame |
|-------------|---------|-------|
| 3G | No SIM | `FF 01 00 D3 00 00 D4` |
| 3G | 0~4 bars | `FF 01 00 D3 03 00 D7` .. `FF 01 00 D3 03 04 DB` |
| 4G | 0~4 bars | `FF 01 00 D3 04 00 D8` .. `FF 01 00 D3 04 04 DC` |
| WIFI | Off | `FF 01 00 D3 11 00 E5` |
| WIFI | 1~4 bars | `FF 01 00 D3 11 01 E6` .. `FF 01 00 D3 11 04 E9` |
| WIFI | Hotspot | `FF 01 00 D3 11 05 EA` |

Automatic reporting uses **4G** and **WIFI** rows only. 3G frames are available via the `test` API.



### API Reference

#### Management APIs

+ `service[ uart_object, ttydev, devcom ]` **run the net2led status poll and Pelco-D write loop**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - ttydev ------------ [ string ], the Linux serial device path (e.g. /dev/ttyS0)
    - devcom ------------ [ string ], optional, the device component that provides ttydev
    - Loads `network@frame` slots, opens the serial port, and runs the libevent timer loop
    - Does not return while the event loop is running; returns tfalse on serial open failure
    - Normally called by `uart@tty.setup`, not invoked directly

    Example, start the net2led service for uart@tty
    ```shell
    uartdrv@net2led.service[ uart@tty, /dev/ttyS0 ]
    ```



#### Control APIs

+ `test[ uart_object, type, level ]` **send a Pelco-D test frame to the serial port**
    - uart_object ------- [ string ], the instance name
    - type -------------- [ string ], optional, test category; omit both type and level to list available items
    - level ------------- [ string ], optional, signal level or mode within the category
    - failed return tfalse, service not running, invalid arguments, or serial write error
    - succeed return ttrue or [ json ] when listing available test items
    - Requires the service process to be running (IPC via Unix datagram socket)

    **type / level combinations**

    | type | level | Frame |
    |------|-------|-------|
    | `3g` | `nosim` | `FF 01 00 D3 00 00 D4` |
    | `3g` | `0`~`4` | 3G 0~4 bars |
    | `4g` | `0`~`4` | 4G 0~4 bars |
    | `wisp` or `wifi` | `off` or `disable` | WIFI off |
    | `wisp` or `wifi` | `1`~`4` | WIFI 1~4 bars |
    | `wisp` or `wifi` | `hotspot` | WIFI hotspot |
    | `all` | (omit) | send all predefined frames sequentially, 200 ms apart |

    Example, list all available test items
    ```shell
    uartdrv@net2led.test[ uart@tty ]
    {
        "3g_nosim":"3g_nosim",
        "3g_0":"3g_0",
        "3g_1":"3g_1",
        "3g_2":"3g_2",
        "3g_3":"3g_3",
        "3g_4":"3g_4",
        "4g_0":"4g_0",
        "4g_1":"4g_1",
        "4g_2":"4g_2",
        "4g_3":"4g_3",
        "4g_4":"4g_4",
        "wisp_off":"wisp_off",
        "wisp_1":"wisp_1",
        "wisp_2":"wisp_2",
        "wisp_3":"wisp_3",
        "wisp_4":"wisp_4",
        "wisp_hotspot":"wisp_hotspot",
        "all":"all"
    }
    ```

    Example, send 4G 3-bar signal frame
    ```shell
    uartdrv@net2led.test[ uart@tty, 4g, 3 ]
    ttrue
    ```

    Example, send WIFI hotspot frame
    ```shell
    uartdrv@net2led.test[ uart@tty, wisp, hotspot ]
    ttrue
    ```

    Example, send all protocol test frames in sequence
    ```shell
    uartdrv@net2led.test[ uart@tty, all ]
    ttrue
    ```
