## uart@tty — UART Port Instance

### Overview

Each serial port is a configuration object named `uart@tty`, `uart@tty2`, `uart@tty3`, … up to `uart@tty8`. Each instance stores the serial port line settings and binds to a driver via the `drvcom` field. Different drivers use different sub-keys for their own configuration.
- configure serial port parameters (speed, parity, databit, stopbit, flow)
- bind to a driver via `drvcom` (see below for available drivers)
- driver-specific configuration lives under a sub-key named after the driver (e.g. `dtu`, `sixents`)
- manage TLS certificates for secure connections
- query driver status and trigger event-driven resets

**Available drivers**:

| drvcom value | Driver | Sub-key | Description |
|---|---|---|---|
| `uartdrv@dtu` | **[dtu.md](dtu.md)** | `dtu` | Serial-to-network bridge (TCP/UDP/MQTT) |
| `uartdrv@sixents` | **[sixents.md](sixents.md)** | `sixents` | GNSS differential correction (RTK) |
| `uartdrv@tui` | **[tui.md](tui.md)** | *(none)* | Eline serial terminal |
| `uartdrv@gnss` | **[../gnss/nmea.md](../gnss/nmea.md)** | *(marker)* | Hand off to `gnss@frame` → `gnssdrv@nmea` (no UART exe) |



### Configuration reference ( uart@tty )

```json
// Attributes introduction 
{
    "status": "instance enable or disable",              // [ "disable", "enable" ], default be "enable"
    "devcom": "device component name",                   // [ string ], the component that provides the ttydev path (e.g. usb@tty-2-3)
    "ttydev": "serial device path",                      // [ string ], Linux serial device path (e.g. /dev/ttyS1), used when devcom is not set
    "drvcom": "driver component name",                   // [ string ], the driver object to bind (e.g. uartdrv@dtu, uartdrv@tui)
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

    "frame_maxsize": "max frame size",                   // [ number ], maximum bytes per frame toward network, used by DTU driver
    "frame_interval": "inter-frame interval",            // [ number ], inter-byte timeout in milliseconds, used by DTU driver

    "sixents":                                             // [ json ], Sixents GNSS differential correction configuration, present when drvcom is uartdrv@sixents
    {
        "ak": "Sixents Access Key",                        // [ string ], the AK credential from Sixents subscription, required
        "as": "Sixents Access Secret",                     // [ string ], the AS credential paired with AK, required
        "devid": "device identifier",                      // [ string ], unique device ID registered with Sixents, required
        "devtype": "device type",                          // [ string ], device model or type label, required
        "auth_port": "HTTPS auth server port",             // [ number ], default be 443
        "rtcm_port": "RTCM data server port",              // [ number ], default be 4402, selectable from 4401-4405
        "tty_cmd": "custom GNSS init command",             // [ string ], optional, if empty sends Unicore N4/UM982 default commands
        "gga_timeout": "GGA watchdog timeout"              // [ number ], seconds without GGA before uart power reset, default be 30, minimum 10
    },

    "dtu":                                               // [ json ], DTU forwarding configuration, present when drvcom is uartdrv@dtu
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

**uart@tty with DTU driver** — serial-to-network bridge with TCP client and server
```shell
land@uart@tty
{
    "status":"enable",                         # port is enabled
    "convert":"disable",                       # hardware flow control disabled
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
            "proto":"tcp",                     # TCP protocol
            "server":"192.168.8.250",          # server address
            "port":"800",                      # server port
            "login":"disable",                 # no login packet
            "keeplive":"disable"               # no keepalive
        },
        "server":
        {
            "status":"enable",                 # TCP server enabled
            "proto":"tcp",                     # TCP protocol
            "port":"7000",                     # listen port
            "limit":"5"                        # max 5 clients
        }
    }
}
```

**uart@tty2 with Sixents driver** — GNSS RTK differential correction
```shell
land@uart@tty2
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS2",                     # serial device path
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
        "gga_timeout":"30"                     # GGA watchdog timeout
    }
}
```

**uart@tty3 with TUI driver** — eline serial terminal (simplest, no driver sub-key)
```shell
land@uart@tty3
{
    "status":"enable",                         # port is enabled
    "ttydev":"/dev/ttyS3",                     # serial device path
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

Example, set the DTU client server address and port
```shell
land@uart@tty:dtu/client/server=192.168.8.100
ttrue
```

Example, merge set the DTU client configure( include "status" "proto" "server" "port" )
```shell
land@uart@tty|{"dtu":{"client":{"status":"enable","proto":"tcp","server":"192.168.8.100","port":"8000"}}}
ttrue
```

Example, set the Sixents Access Key on uart@tty2
```shell
land@uart@tty2:sixents/ak=new-ak-value
ttrue
```

Example, bind uart@tty3 to the TUI driver
```shell
land@uart@tty3:drvcom=uartdrv@tui
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the UART port instance**
    - failed return tfalse
    - succeed return ttrue
    - Reads configuration, resolves ttydev and drvcom, starts the driver service
    - Called by `uart@frame.setup` at boot or when configuration changes

+ `shut[]` **shut down the UART port instance**
    - failed return tfalse
    - succeed return ttrue
    - Stops the driver service and unregisters the port

#### Query APIs

+ `ttydev[]` **get the resolved serial device path**
    - failed return NULL
    - succeed return [ string ], the Linux serial device path (e.g. /dev/ttyS1)

    Example, get the tty device path
    ```shell
    uart@tty.ttydev
    /dev/ttyS1
    ```

+ `devcom[]` **get the device component name**
    - failed return NULL
    - succeed return [ string ], the device component name (e.g. usb@tty-2-3)

    Example, get the device component
    ```shell
    uart@tty.devcom
    usb@tty-2-3
    ```

+ `drvcom[]` **get the driver component name**
    - failed return NULL
    - succeed return [ string ], the driver object name (e.g. uartdrv@dtu)

    Example, get the driver component
    ```shell
    uart@tty.drvcom
    uartdrv@dtu
    ```

+ `status[]` **get the driver status**
    - failed return NULL, driver not running or driver does not support status
    - succeed return [ json ], driver status information (delegates to the driver's status API)

    Example, get DTU driver status
    ```shell
    uart@tty.status
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

+ `key[]` **list TLS certificate paths for driver client/server slots**
    - failed return NULL
    - succeed return [ json ], a map of slot name to TLS file paths (ca, crt, key) where files exist

    Example, list TLS certificates
    ```shell
    uart@tty.key
    {
        "client":
        {
            "ca":"/skinos/cfg/uart-tty-client.ca",
            "crt":"/skinos/cfg/uart-tty-client.crt",
            "key":"/skinos/cfg/uart-tty-client.key"
        }
    }
    ```

#### Control APIs

+ `reset[ event, ifname ]` **trigger an event-driven reset on the driver**
    - event ------------ [ string ], the event name (e.g. network/online, network/onextern)
    - ifname ----------- [ string ], JSON object with "ifname" field specifying the interface name
    - failed return tfalse
    - succeed return ttrue

    Example, reset DTU clients when WAN comes online
    ```shell
    uart@tty.reset[ network/online, {"ifname":"ifname@wan"} ]
    ttrue
    ```

+ `import_ca[ slot, file ]` **import a TLS CA certificate for a driver slot**
    - slot ------------- [ string ], the slot name (e.g. client, server, mqtt)
    - file ------------- [ string ], the source file path to copy
    - failed return tfalse
    - succeed return ttrue

    Example, import CA certificate for client slot
    ```shell
    uart@tty.import_ca[ client, /tmp/ca.pem ]
    ttrue
    ```

+ `import_cert[ slot, file ]` **import a TLS client certificate for a driver slot**
    - slot ------------- [ string ], the slot name
    - file ------------- [ string ], the source file path to copy
    - failed return tfalse
    - succeed return ttrue

    Example, import client certificate
    ```shell
    uart@tty.import_cert[ client, /tmp/client.crt ]
    ttrue
    ```

+ `import_key[ slot, file ]` **import a TLS private key for a driver slot**
    - slot ------------- [ string ], the slot name
    - file ------------- [ string ], the source file path to copy
    - failed return tfalse
    - succeed return ttrue

    Example, import client private key
    ```shell
    uart@tty.import_key[ client, /tmp/client.key ]
    ttrue
    ```

+ `clear_ca[ slot ]` **remove the TLS CA certificate for a driver slot**
    - slot ------------- [ string ], the slot name
    - failed return tfalse
    - succeed return ttrue

    Example, remove client CA certificate
    ```shell
    uart@tty.clear_ca[ client ]
    ttrue
    ```

+ `clear_cert[ slot ]` **remove the TLS client certificate for a driver slot**
    - slot ------------- [ string ], the slot name
    - failed return tfalse
    - succeed return ttrue

    Example, remove client certificate
    ```shell
    uart@tty.clear_cert[ client ]
    ttrue
    ```

+ `clear_key[ slot ]` **remove the TLS private key for a driver slot**
    - slot ------------- [ string ], the slot name
    - failed return tfalse
    - succeed return ttrue

    Example, remove client private key
    ```shell
    uart@tty.clear_key[ client ]
    ttrue
    ```

+ `power[]` **power cycle the UART port hardware**
    - failed return tfalse
    - succeed return ttrue
    - Toggles the GPIO power pin off for 1 second then back on

    Example, power cycle the UART port
    ```shell
    uart@tty.power
    ttrue
    ```
