## uartdrv@dtu — DTU Serial-to-Network Driver

### Overview

The DTU driver bridges a serial port to multiple TCP/UDP clients, TCP/UDP servers, and MQTT clients. It is registered as `uartdrv@dtu` in `prj.json` and launched by `uart@tty.setup` when the instance's `drvcom` is set to `uartdrv@dtu`.
- bridge serial data to TCP/UDP client connections (up to 9 slots)
- bridge serial data to TCP/UDP server listeners (up to 9 slots)
- bridge serial data to MQTT publish/subscribe (up to 9 slots)
- support TLS/SSL for TCP client and MQTT connections
- support login packets, keepalive, frame prefix/postfix for protocol framing
- support serial active data injection (idle, timing, or one-shot modes)
- query link statistics and reset individual connections via IPC



### Concepts

**Relationship to framework and instances**

`uartdrv@dtu` is a **driver** in the UART device-driver separation architecture:

* **Framework** (`uart@frame`) — manages instance lifecycle. At boot, `uart@frame.setup` iterates configured instances and starts each one.
* **Instance** (`uart@tty`, `uart@tty2`, …) — holds the serial port configuration and the DTU forwarding rules under the `dtu` subtree. When `drvcom` is `uartdrv@dtu`, the instance's `setup` launches this driver as a service.
* **Driver** (`uartdrv@dtu`) — the executable that runs the actual serial-to-network bridging event loop. It reads the instance's configuration, opens the serial port, creates TCP/UDP/MQTT peers, and forwards data bidirectionally.

**Startup chain**: `uart@frame.setup` → `uart@tty.setup` → `sstarts("uart@tty", "uartdrv@dtu", "service", "uart@tty", "/dev/ttyS1")` → `uartdrv@dtu.service` runs the event loop.

**IPC**: while running, the driver listens on a Unix datagram socket at `<var_dir>/<project>/uart@tty.unix`. The `status` and `reset` APIs communicate through this socket.



### Configuration reference ( uart@tty for DTU )

When `drvcom` is `uartdrv@dtu`, the following configuration fields are read from the `uart@tty` instance object.

```json
// Attributes introduction 
{
    "status": "instance enable or disable",              // [ "disable", "enable" ], default be "enable"
    "ttydev": "serial device path",                      // [ string ], Linux serial device path (e.g. /dev/ttyS1)
    "devcom": "device component name",                   // [ string ], the component that provides ttydev path
    "drvcom": "driver component name",                   // [ string ], must be "uartdrv@dtu" for this driver
    "extern": "reset trigger on network event",          // [ "disable", "default", "<ifname>" ], default be "disable"
                                                              // "disable": no reset on network event
                                                              // "default": reset when gateway comes online
                                                              // "ifname@wan", "ifname@lte", etc.: reset when that interface comes online

    "speed": "serial baud rate",                         // [ "9600", "19200", "38400", "57600", "115200", ... ], default be "9600"
    "flow": "flow control type",                         // [ "disable", "hard", "soft" ], default be "disable"
    "parity": "parity mode",                             // [ "disable", "even", "odd" ], default be "disable"
    "databit": "data bits",                              // [ "5", "6", "7", "8" ], default be "8"
    "stopbit": "stop bits",                              // [ "1", "2" ], default be "1"

    "active": "serial active data mode",                 // [ "disable", "enable", "idle", "timing" ], default be "disable"
                                                              // "disable": no active data sent
                                                              // "enable": send active data once at startup
                                                              // "idle": send active data when serial is idle
                                                              // "timing": send active data at fixed interval
    "active_interval": "active data interval",           // [ number ], interval in seconds, used with enable/idle/timing modes
    "active_string": "active data payload",              // [ hex string ], the hex-encoded bytes to send

    "frame_maxsize": "max frame size",                   // [ number ], maximum bytes per frame toward network
    "frame_interval": "inter-frame interval",            // [ number ], inter-byte timeout in milliseconds

    "dtu":                                               // [ json ], DTU forwarding configuration
    {
        "client":                                        // [ json ], TCP/UDP client slot 1
        {
            "status": "client enable or disable",        // [ "disable", "enable" ]
            "extern": "reset trigger on network event",  // [ "disable", "default", "<ifname>" ]
            "proto": "protocol",                         // [ "tcp", "udp" ]
            "server": "server address",                  // [ string ], domain name or IPv4 address
            "port": "server port",                       // [ number ]

            "login": "login packet type",                // [ "disable", "hex", "ascii", "mac" ]
                                                              // "disable": no login packet
                                                              // "hex": login_string is hex-encoded
                                                              // "ascii": login_string is ASCII text
                                                              // "mac": use device MAC address as login packet
            "login_string": "login packet content",      // [ string ]

            "keeplive": "keepalive type",                // [ "disable", "idle", "enable", "timing" ]
                                                              // "disable": no keepalive
                                                              // "idle": send keepalive when idle
                                                              // "timing": send keepalive at fixed interval
                                                              // "enable": same as timing
            "keeplive_interval": "keepalive interval",   // [ number ], seconds
            "keeplive_string": "keepalive payload",      // [ hex string ]

            "frame_start": "frame prefix type",          // [ "disable", "hex", "ascii", "mac" ]
            "frame_start_string": "frame prefix content", // [ string ]
            "frame_end": "frame postfix type",           // [ "disable", "hex", "ascii", "mac" ]
            "frame_end_string": "frame postfix content"  // [ string ]
        }
        // "...":{...}  How many client slots (client, client2, ..., client9) show how many properties

        "mqtt":                                          // [ json ], MQTT client slot 1
        {
            "status": "MQTT enable or disable",          // [ "disable", "enable" ]
            "extern": "reset trigger on network event",  // [ "disable", "default", "<ifname>" ]
            "server": "MQTT broker address",             // [ string ], domain name or IPv4 address
            "port": "MQTT broker port",                  // [ number ]
            "mqtt_id": "MQTT client ID",                 // [ string ]
            "mqtt_username": "MQTT username",            // [ string ]
            "mqtt_password": "MQTT password",            // [ string ]
            "mqtt_interval": "publish interval",         // [ number ], seconds
            "mqtt_keepalive": "MQTT keepalive",          // [ number ], seconds
            "mqtt_publish": "publish topic",             // [ string ]
            "mqtt_publish_qos": "publish QoS",           // [ number ], 0, 1, or 2
            "mqtt_subscribe":                            // [ json ], subscription topics
            {
                "topic name": "QoS level"                // [ string ]: [ number ], topic and its QoS
                // "...":"..."  How many topics show how many properties
            }
        }
        // "...":{...}  How many MQTT slots (mqtt, mqtt2, ..., mqtt9) show how many properties

        "server":                                        // [ json ], TCP/UDP server slot 1
        {
            "status": "server enable or disable",        // [ "disable", "enable" ]
            "proto": "protocol",                         // [ "tcp", "udp" ]
            "port": "listen port",                       // [ number ]
            "limit": "max concurrent clients",           // [ number ]

            "login": "login packet type",                // [ "disable", "hex", "ascii", "mac" ]
            "login_string": "login packet content",      // [ string ]
            "keeplive": "keepalive type",                // [ "disable", "idle", "enable", "timing" ]
            "keeplive_interval": "keepalive interval",   // [ number ], seconds
            "keeplive_string": "keepalive payload",      // [ hex string ]
            "frame_start": "frame prefix type",          // [ "disable", "hex", "ascii", "mac" ]
            "frame_start_string": "frame prefix content", // [ string ]
            "frame_end": "frame postfix type",           // [ "disable", "hex", "ascii", "mac" ]
            "frame_end_string": "frame postfix content"  // [ string ]
        }
        // "...":{...}  How many server slots (server, server2, ..., server9) show how many properties
    }
}
```

#### Configuration example

Example, show all the uart@tty DTU configure
```shell
land@uart@tty
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS1",                     # serial device path
    "drvcom":"uartdrv@dtu",                    # bound to DTU driver
    "extern":"disable",                        # no reset on network event
    "speed":"57600",                           # baud rate 57600
    "flow":"disable",                          # no flow control
    "parity":"disable",                        # no parity
    "databit":"8",                             # 8 data bits
    "stopbit":"1",                             # 1 stop bit
    "active":"disable",                        # no active data
    "dtu":
    {
        "client":
        {
            "status":"enable",                 # TCP client enabled
            "extern":"disable",                # no reset on network event
            "proto":"tcp",                     # TCP protocol
            "server":"192.168.8.250",          # server address
            "port":"800",                      # server port
            "login":"disable",                 # no login packet
            "login_string":"",
            "keeplive":"disable",              # no keepalive
            "keeplive_interval":"30",
            "keeplive_string":"",
            "frame_start":"disable",           # no frame prefix
            "frame_start_string":"",
            "frame_end":"disable",             # no frame postfix
            "frame_end_string":""
        },
        "server":
        {
            "status":"enable",                 # TCP server enabled
            "proto":"tcp",                     # TCP protocol
            "port":"7000",                     # listen port
            "limit":"5",                       # max 5 clients
            "login":"disable",
            "login_string":"",
            "keeplive":"disable",
            "keeplive_interval":"30",
            "keeplive_string":"",
            "frame_start":"disable",
            "frame_start_string":"",
            "frame_end":"disable",
            "frame_end_string":""
        },
        "mqtt":
        {
            "status":"disable",                # MQTT disabled
            "extern":"disable",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_interval":"10",
            "mqtt_keepalive":"60",
            "mqtt_publish":"",
            "mqtt_publish_qos":"0",
            "mqtt_subscribe":{}
        }
    }
}
```

#### Configuration settings example

Example, set the serial baud rate to 115200
```shell
land@uart@tty:speed=115200
ttrue
```

Example, set the DTU client server address
```shell
land@uart@tty:dtu/client/server=192.168.8.100
ttrue
```

Example, merge set the DTU client configure( include "status" "proto" "server" "port" )
```shell
land@uart@tty|{"dtu":{"client":{"status":"enable","proto":"tcp","server":"192.168.8.100","port":"8000"}}}
ttrue
```



### API Reference

#### Control APIs

+ `service[ uart_object, ttydev, devcom ]` **run the DTU event loop for a UART instance**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - ttydev ------------ [ string ], the Linux serial device path (e.g. /dev/ttyS1)
    - devcom ------------ [ string ], optional, the device component that provides ttydev
    - Opens the serial port with the instance's line settings, creates TCP/UDP/MQTT peers from the `dtu` configuration subtree, and runs the libevent event loop
    - Does not return while the event loop is running; returns tfalse on serial open failure
    - Normally called by `uart@tty.setup`, not invoked directly

    Example, start the DTU service for uart@tty
    ```shell
    uartdrv@dtu.service[ uart@tty, /dev/ttyS1 ]
    ```

#### Query APIs

+ `status[ uart_object ]` **query link statistics for all peers**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - failed return NULL, the DTU process is not running
    - succeed return [ json ], per-peer statistics including bytes transferred and connection state
    ```json
    {
        "peer name":                             // [ string ]: { json }, peer identifier (tty, client, server, mqtt, etc.)
        {                                          // peer statistics
            "rx": "bytes received",              // [ number ], total bytes received, -1 when disconnected
            "tx": "bytes sent",                  // [ number ], total bytes sent, -1 when disconnected
            "connect": "connection state",       // [ string ], "ok" when connected
            "ip": "peer IP address"              // [ string ], present for TCP/UDP clients with active connection
        }
        // "...":{...}  How many peers show how many properties
    }
    ```

    Example, query DTU status for uart@tty
    ```shell
    uartdrv@dtu.status[ uart@tty ]
    {
        "tty":
        {
            "rx":1024,
            "tx":2048,
            "connect":"ok"
        },
        "client":
        {
            "rx":512,
            "tx":256,
            "connect":"ok",
            "ip":"192.168.8.250"
        }
    }
    ```

+ `reset[ uart_object, ifname ]` **reconnect selected TCP/UDP/MQTT clients on network event**
    - uart_object ------- [ string ], the instance name (e.g. uart@tty)
    - ifname ------------ [ string ], JSON object with "ifname" field specifying the network interface that came online
    - For each client/mqtt slot in the `dtu` configuration: if `extern` is "default" (and event is network/online) or `extern` matches the ifname value, closes and reopens that connection
    - Server listeners are not affected by reset
    - failed return tfalse, process not running or no matching clients
    - succeed return ttrue

    Example, reset DTU clients when WAN interface comes online
    ```shell
    uartdrv@dtu.reset[ uart@tty, {"ifname":"ifname@wan"} ]
    ttrue
    ```
