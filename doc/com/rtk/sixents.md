## uartdrv@sixents — Sixents GNSS Differential Correction Driver

### Overview

The Sixents driver bridges a GNSS module connected via UART to the Sixents cloud NTRIP service, enabling centimeter-level RTK positioning. It is registered as `uartdrv@sixents` in `prj.json` and launched by `uart@tty.setup` when the instance's `drvcom` is set to `uartdrv@sixents`.
- read GGA position sentences from the GNSS module over serial
- forward GGA to the Sixents cloud over TLS
- receive RTCM correction data from the cloud and write it back to the serial port
- track GGA fix quality (GPS, DGPS, RTK-Fixed, RTK-Float)
- watchdog: trigger uart power reset if no GGA received for configurable timeout
- support custom GNSS module initialization commands
- query SDK state, GGA quality, and I/O counters via IPC



### Concepts

**Relationship to framework and instances**

`uartdrv@sixents` is a **driver** in the UART device-driver separation architecture:

* **Framework** (`uart@frame`) — manages instance lifecycle. At boot, `uart@frame.setup` iterates configured instances and starts each one.
* **Instance** (`uart@tty`, `uart@tty2`, …) — holds the serial port configuration and the Sixents credentials under the `sixents` subtree. When `drvcom` is `uartdrv@sixents`, the instance's `setup` launches this driver as a service.
* **Driver** (`uartdrv@sixents`) — the executable that runs the GGA/RTCM relay event loop. It reads the instance's configuration, opens the serial port, initializes the Sixents SDK, and bridges data between the GNSS module and the cloud.

**Startup chain**: `uart@frame.setup` → `uart@tty.setup` → `sstarts("uart@tty", "uartdrv@sixents", "service", "uart@tty", "/dev/ttyS1")` → `uartdrv@sixents.service` runs the event loop.

**Data flow**:
```
GNSS module --GGA--> UART --GGA--> sixents SDK --GGA--> Sixents cloud (TLS)
Sixents cloud --RTCM--> sixents SDK --RTCM--> UART --RTCM--> GNSS module
```

**SDK state machine**: INIT → START → RUNNING (GGA forwarded every 1s, RTCM received via callback) → STOP → FINAL → CLOSE. A 200ms timer drives the state machine.

**IPC**: while running, the driver listens on a Unix datagram socket at `<var_dir>/<project>/uart@tty.unix`. The `status`, `sdk_start`, `sdk_stop`, `tty_cmd`, and `sdk_gga` APIs communicate through this socket.



### Configuration reference ( uart@tty for Sixents )

When `drvcom` is `uartdrv@sixents`, the following configuration fields are read from the `uart@tty` instance object. Serial port settings are at the top level; Sixents-specific parameters are under the `sixents` subtree.

```json
// Attributes introduction 
{
    "status": "instance enable or disable",              // [ "disable", "enable" ], default be "enable"
    "ttydev": "serial device path",                      // [ string ], Linux serial device path (e.g. /dev/ttyS1)
    "devcom": "device component name",                   // [ string ], the component that provides ttydev path
    "drvcom": "driver component name",                   // [ string ], must be "uartdrv@sixents" for this driver

    "speed": "serial baud rate",                         // [ "9600", "19200", "38400", "57600", "115200", ... ], default be "9600"
    "flow": "flow control type",                         // [ "disable", "hard", "soft" ], default be "disable"
    "parity": "parity mode",                             // [ "disable", "even", "odd" ], default be "disable"
    "databit": "data bits",                              // [ "5", "6", "7", "8" ], default be "8"
    "stopbit": "stop bits",                              // [ "1", "2" ], default be "1"

    "sixents":                                             // [ json ], Sixents differential correction configuration
    {
        "ak": "Sixents Access Key",                        // [ string ], the AK credential from Sixents subscription, required
        "as": "Sixents Access Secret",                     // [ string ], the AS credential paired with AK, required
        "devid": "device identifier",                      // [ string ], unique device ID registered with Sixents, required
        "devtype": "device type",                          // [ string ], device model or type label, required
        "auth_port": "HTTPS auth server port",             // [ number ], default be 443
        "rtcm_port": "RTCM data server port",              // [ number ], default be 4402, selectable from 4401-4405
        "tty_cmd": "custom GNSS init command",             // [ string ], optional, if empty sends Unicore N4/UM982 default commands
        "gga_timeout": "GGA watchdog timeout"              // [ number ], seconds without GGA before uart power reset, default be 30, minimum 10
    }
}
```

#### Configuration example

Example, show all the uart@tty Sixents configure
```shell
land@uart@tty
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS1",                     # serial device path
    "drvcom":"uartdrv@sixents",                # bound to Sixents driver
    "speed":"115200",                          # baud rate 115200
    "flow":"disable",                          # no flow control
    "parity":"disable",                        # no parity
    "databit":"8",                             # 8 data bits
    "stopbit":"1",                             # 1 stop bit
    "sixents":
    {
        "ak":"your-ak-string",                 # Sixents Access Key
        "as":"your-as-string",                 # Sixents Access Secret
        "devid":"SN20260001",                  # device serial number
        "devtype":"GNSS-RTK-01",               # device type
        "auth_port":"443",                     # HTTPS auth port
        "rtcm_port":"4402",                    # RTCM data port
        "gga_timeout":"30"                     # GGA watchdog timeout in seconds
    }
}
```

#### Configuration settings example

Example, set the Sixents Access Key
```shell
land@uart@tty:sixents/ak=new-ak-value
ttrue
```

Example, merge set the Sixents configure( include "ak" "as" "devid" "rtcm_port" )
```shell
land@uart@tty|{"sixents":{"ak":"new-ak","as":"new-as","devid":"SN20260002","rtcm_port":"4403"}}
ttrue
```



### API Reference

#### Control APIs

+ `service[ uart_object, ttydev, devcom ]` **run the Sixents differential correction event loop**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - ttydev ------------ [ string ], the Linux serial device path (e.g. /dev/ttyS1)
    - devcom ------------ [ string ], optional, the device component that provides ttydev
    - Opens the serial port, sends GNSS init commands, loads root CA, initializes Sixents SDK, and runs the libevent event loop
    - Does not return while the event loop is running; returns tfalse on serial open failure
    - Normally called by `uart@tty.setup`, not invoked directly

    Example, start the Sixents service for uart@tty
    ```shell
    uartdrv@sixents.service[ uart@tty, /dev/ttyS1 ]
    ```

+ `sdk_start[ uart_object ]` **start the SDK if stopped**
    - uart_object ------- [ string ], the instance name
    - Resets the SDK state machine from STOP or IDLE to INIT; the SDK will re-initialize and re-authenticate on the next tick
    - failed return tfalse, service not running or SDK already running
    - succeed return ttrue

    Example, restart the Sixents SDK
    ```shell
    uartdrv@sixents.sdk_start[ uart@tty ]
    ttrue
    ```

+ `sdk_stop[ uart_object ]` **stop the SDK gracefully**
    - uart_object ------- [ string ], the instance name
    - Transitions the SDK to STOP state; the service process remains running but the SDK becomes inactive
    - failed return tfalse, service not running
    - succeed return ttrue

    Example, stop the Sixents SDK
    ```shell
    uartdrv@sixents.sdk_stop[ uart@tty ]
    ttrue
    ```

+ `tty_cmd[ uart_object, cmd ]` **send a raw command to the GNSS module via serial**
    - uart_object ------- [ string ], the instance name
    - cmd --------------- [ string ], the command string to send (e.g. "VERSION", "LOG GNGGA COM1 1")
    - failed return tfalse, service not running or invalid arguments
    - succeed return ttrue

    Example, query GNSS module firmware version
    ```shell
    uartdrv@sixents.tty_cmd[ uart@tty, VERSION ]
    ttrue
    ```

+ `sdk_gga[ uart_object, gga ]` **submit GGA data to the SDK manually**
    - uart_object ------- [ string ], the instance name
    - gga --------------- [ string ], the GGA sentence to submit (overrides serial-read GGA)
    - failed return tfalse, service not running or invalid arguments
    - succeed return ttrue

    Example, submit a test GGA sentence
    ```shell
    uartdrv@sixents.sdk_gga[ uart@tty, $GNGGA,061234.00,3959.7760,N,11602.3631,E,4,12,0.8,100.0,M,-8.0,M,,*47 ]
    ttrue
    ```

#### Query APIs

+ `status[ uart_object ]` **query the running status of the Sixents service**
    - uart_object ------- [ string ], the instance name
    - failed return NULL, service not running
    - succeed return [ json ], SDK state, GGA quality, and I/O counters
    ```json
    {
        "state": "SDK state machine phase",    // [ number ], 0=INIT, 1=START, 2=RUNNING, 3=STOP, 4=FINAL, 5=IDLE, 6=CLOSE
        "sdk_status": "SDK status code",       // [ number ], status code from Sixents SDK
        "gga": "current GGA sentence",         // [ string ], the last GGA sentence in buffer, empty if none received
        "gga_quality": "fix quality",          // [ string ], "none", "invalid", "GPS", "DGPS", "RTK-Fixed", "RTK-Float", "DeadReckoning"
        "tick": "tick count",                  // [ number ], number of 200ms ticks since entering RUNNING state
        "total_read": "bytes read from tty",   // [ number ], cumulative bytes read from serial port
        "total_write": "bytes written to tty"  // [ number ], cumulative bytes written to serial port (RTCM corrections)
    }
    ```

    Example, query Sixents service status
    ```shell
    uartdrv@sixents.status[ uart@tty ]
    {
        "state":2,                             # RUNNING - SDK is active
        "sdk_status":1401,                     # RTCM data received successfully
        "gga":"$GNGGA,061234.00,3959.7760,N,11602.3631,E,4,12,0.8,100.0,M,-8.0,M,,*47",
        "gga_quality":"RTK-Fixed",             # centimeter-level accuracy
        "tick":1523,                           # ~304 seconds since start
        "total_read":12345,                    # bytes read from GNSS module
        "total_write":6789                     # bytes written (RTCM corrections)
    }
    ```
