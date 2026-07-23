## gnssdrv@nmea — GNSS NMEA Parser and TCP/UDP Bridge

### Overview

Parse NMEA sentences from a GNSS serial port and bridge complete fix cycles to TCP/UDP peers. Registered as `gnssdrv@nmea` and started by `gnss@nmea.setup` when `drvcom` is `gnssdrv@nmea`.
- UART is read line-by-line (`evbuffer_readln`); each line is optionally pushed to `nmea_sock`
- sentences accumulate into a **cycle** until the next GGA (or next RMC on RMC-only streams), or **500ms** idle
- when a cycle completes: update `latest_pos` from GGA/RMC/GSA, then forward the cycle to TCP/UDP peers
- peers apply `filter` (e.g. `GGA;RMC`) and optional `head` once before the first matched line of that cycle
- power-cycle the instance via `gnss@nmea.power` when no GGA/RMC arrives within `fix_timeout`



### Configuration reference ( gnss@nmea for NMEA driver )

When `drvcom` is `gnssdrv@nmea`, the following fields are read from the `gnss@nmea` instance object. Serial line settings (`speed` / `parity` / …) stay on the instance root and are passed to `serial_open`.

```json
// Attributes introduction 
{
    "status":"instance status",              // [ "disable", "enable" ], default "disable"
    "ttydev":"serial device path",           // [ string ], Linux TTY path (e.g. "/dev/ttyUSB0")
    "devcom":"device component",             // [ string ], optional hardware object that provides ttydev
    "drvcom":"driver component",             // [ string ], must be "gnssdrv@nmea" for this driver
    "extern":"network depend",               // [ "default", ifname object, "" ], used by non-driver reset path
    "speed":"serial baud rate",              // [ string ], if empty, fall back to devcom config
    "parity":"parity mode",                  // [ "disable", "even", "odd" ], if empty, fall back to devcom
    "databit":"data bits",                   // [ "5", "6", "7", "8" ], if empty, fall back to devcom
    "stopbit":"stop bits",                   // [ "1", "2" ], if empty, fall back to devcom
    "flow":"flow control",                   // [ "disable", "hard", "soft" ], if empty, fall back to devcom

    "nmea":                                  // [ json ], NMEA driver subtree
    {
        "tty_cmd":"startup serial commands", // [ string ], semicolon-separated commands sent at start (CRLF appended)
        "fix_timeout":"fix watchdog seconds",  // [ number ], default "30", minimum 10; no GGA or RMC triggers power
        "nmea_sock":"local NMEA unix forward", // [ "disable", "enable" ], default "disable"; when enable, path is project_var_path("%s.nmea", object)

        "client":                            // [ json ], TCP/UDP client slot 1
        {
            "status":"client enable",        // [ "disable", "enable" ]
            "extern":"reset on network",     // [ "default", ifname object, "" ], reconnect when matching online
            "proto":"protocol",              // [ "tcp", "udp" ]
            "server":"server address",       // [ string ], IPv4 or hostname
            "port":"server port",            // [ string ]
            "filter":"sentence type filter", // [ string ], semicolon-separated types (e.g. "GGA;RMC"); empty = forward all lines in the cycle
            "head":"prefix before cycle"     // [ string ], optional text sent once before the first matched line of each cycle (CRLF added if missing)
        }
        // "...":{...}  How many client slots (client, client2, ..., client9) show how many properties

        "server":                            // [ json ], TCP/UDP server slot 1
        {
            "status":"server enable",        // [ "disable", "enable" ]
            "proto":"protocol",              // [ "tcp", "udp" ]
            "port":"listen port",            // [ string ]
            "limit":"max TCP clients",       // [ number ], TCP only, default "5"
            "timeout":"idle timeout",        // [ number ], optional seconds for TCP child / UDP peer idle
            "filter":"sentence type filter", // [ string ], semicolon-separated types (e.g. "GGA;RMC"); empty = forward all lines in the cycle
            "head":"prefix before cycle"     // [ string ], optional text sent once before the first matched line of each cycle
        }
        // "...":{...}  How many server slots (server, server2, ..., server9) show how many properties
    }
}
```

#### Configuration example

Example, enable GNSS with TCP client forward
```shell
gnss@nmea
{
    "status":"enable",
    "ttydev":"/dev/ttyUSB0",
    "drvcom":"gnssdrv@nmea",
    "extern":"default",
    "speed":"9600",
    "nmea":
    {
        "tty_cmd":"",
        "fix_timeout":"30",
        "nmea_sock":"disable",
        "client":
        {
            "status":"enable",
            "extern":"default",
            "proto":"tcp",
            "server":"192.168.8.100",
            "port":"9000",
            "filter":"GGA;RMC",
            "head":"$MYDEV,1*00"
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
}
```

#### Configuration settings example

Example, set TCP client destination
```shell
gnss@nmea:nmea/client/server=192.168.8.100
ttrue
```

Example, enable TCP client and port together
```shell
gnss@nmea|{"nmea":{"client":{"status":"enable","proto":"tcp","server":"192.168.8.100","port":"9000"}}}
ttrue
```



### Concepts

**Relationship to framework and instances**

`gnssdrv@nmea` is a **driver** in the GNSS device-driver separation architecture:

* **Framework** (`gnss@frame`) — manages instance lifecycle and hotplug `register` / `unregister`
* **Instance** (`gnss@nmea` … `gnss@nmea8`) — holds TTY binding and the `nmea` subtree
* **Driver** (`gnssdrv@nmea`) — parses NMEA cycles and forwards them to TCP/UDP peers

**Startup chain**: `gnss@frame.setup` → `gnss@nmea.setup` → `sstarts(object, "gnssdrv@nmea", "service", object, ttydev)`.

**Cycle forward**: UART lines form a cycle (GGA/RMC boundary or 500ms idle); on commit the cycle updates `latest_pos` and is forwarded to peers with `filter`/`head`. Peer-to-UART remains raw byte write-back.



### API Reference

#### Management APIs

+ `service[ gnss_object, ttydev, devcom ]` **run the NMEA parse and bridge event loop**
    - gnss_object ------- [ string ], instance name (e.g. gnss@nmea)
    - ttydev ------------ [ string ], Linux serial device path
    - devcom ------------ [ string ], optional device component used to resolve ttydev
    - Opens the serial port, starts client/server slots from `nmea`, parses NMEA, and runs libevent until exit
    - Normally called by `gnss@nmea.setup`, not invoked directly

    Example, start the driver for gnss@nmea
    ```shell
    gnssdrv@nmea.service[ gnss@nmea, /dev/ttyUSB0 ]
    ```


#### Query APIs

+ `status[ gnss_object ]` **query position and peer counters**
    - gnss_object ------- [ string ], instance name (e.g. gnss@nmea)
    - failed return NULL when the driver process is not running
    - succeed return [ json ]
    - `pos` is absent until the first complete NMEA cycle has been parsed
    - raw NMEA `time` / `date` are converted into `utc` only; they are not returned
    ```json
    {
        "pos":                                   // [ json ], latest parsed position (absent until first cycle)
        {
            "utc":"UTC date-time",               // [ string ], HH:MM:SS:mm:dd:YYYY, same form as land@machine.status current
                                                    // built from GGA/RMC time and RMC date; missing date fills 00/0000
            "lat":"latitude",                    // [ string ], decimal degrees; north positive, south negative
            "lon":"longitude",                   // [ string ], decimal degrees; east positive, west negative
            "alt":"altitude",                    // [ string ], metres above mean sea level (GGA)
            "geoid":"geoid separation",          // [ string ], metres (GGA)
            "valid":"fix validity",              // [ "0", "1" ], usable fix flag from quality_code
                                                    // "0": no usable fix (quality_code is 0 or missing)
                                                    // "1": fix considered valid (quality_code > 0)
            "quality_code":"GGA quality",        // [ number ], NMEA GGA quality indicator (field 6)
                                                    // 0: invalid, 1: GPS(SPS), 2: DGPS, 3: PPS
                                                    // 4: RTK-Fixed, 5: RTK-Float, 6: DeadReckoning
                                                    // other: vendor-specific
            "quality":"fix quality text",       // [ string ], mapped from quality_code
                                                    // "invalid": code 0
                                                    // "GPS": code 1
                                                    // "DGPS": code 2
                                                    // "3": code 3 (PPS, numeric text)
                                                    // "RTK-Fixed": code 4
                                                    // "RTK-Float": code 5
                                                    // "DeadReckoning": code 6
                                                    // other: decimal text of the code
            "fix_mode":"GSA fix type",          // [ "1", "2", "3" ], NMEA GSA field 2
                                                    // "1": no fix
                                                    // "2": 2D fix (lat/lon only)
                                                    // "3": 3D fix (lat/lon/alt)
            "sats":"satellites in use",          // [ number ], GGA satellites used in the solution (not in view)
            "hdop":"HDOP",                       // [ string ], horizontal dilution of precision (GGA or GSA); smaller is better
            "pdop":"PDOP",                       // [ string ], position dilution of precision (GSA)
            "vdop":"VDOP",                       // [ string ], vertical dilution of precision (GSA)
            "speed":"speed over ground",         // [ string ], knots (RMC)
            "course":"course over ground"        // [ string ], degrees true (RMC)
        },
        "cycle_count":"parsed cycles",           // [ number ], completed parse/forward cycles since this driver process started
        "fix_age":"seconds since last fix",     // [ number ], uptime seconds since last cycle commit; -1 if never fixed
        "nmea_sock":"local unix path",           // [ string ], present only when nmea_sock is enable
        "tty":                                   // [ json ], serial peer stats
        {
            "rx":"bytes from UART",              // [ number ], bytes read from the serial/PTY
            "tx":"bytes to UART",                // [ number ], bytes written to the serial
            "connect":"ok"                       // [ string ], present when the TTY is open
        },
        "peer name":                             // [ string ]: { json }, network peer slot name (e.g. client, server2:…)
        {
            "rx":"bytes from peer",              // [ number ]
            "tx":"bytes to peer",                // [ number ]
            "ip":"peer address",                 // [ string ], optional
            "connect":"ok"                       // [ string ], present when the peer socket is connected
        }
        // "...":{...}  How many network peers show how many properties
    }
    ```

    Example, query status
    ```shell
    gnssdrv@nmea.status[ gnss@nmea ]
    {
        "pos":
        {
            "utc":"11:11:44:07:23:2026",    # UTC 11:11:44 on 2026-07-23
            "lat":"22.54315667",            # latitude 22.54315667° N
            "lon":"114.05811167",           # longitude 114.05811167° E
            "alt":"50.0",                   # altitude 50.0 m above MSL
            "geoid":"0.0",                  # geoid separation 0.0 m
            "valid":"1",                    # usable fix (quality_code > 0)
            "quality_code":"1",             # GGA quality 1 = GPS(SPS)
            "quality":"GPS",                # text for quality_code 1
            "fix_mode":"3",                # GSA 3D fix
            "sats":"12",                    # 12 satellites used in solution
            "hdop":"0.9",                   # HDOP 0.9 (good)
            "pdop":"1.2",                   # PDOP 1.2
            "vdop":"0.8",                   # VDOP 0.8
            "speed":"0.50",                 # 0.50 knots over ground
            "course":"145.00"               # course 145° true
        },
        "cycle_count":"2",                  # 2 parse cycles since driver start
        "nmea_sock":"/var/gnss/gnss@nmea.nmea",  # local NMEA unix forward path
        "fix_age":"0",                     # last fix committed 0 seconds ago
        "tty":
        {
            "rx":"1242",                    # 1242 bytes read from serial
            "tx":"0",                       # 0 bytes written to serial
            "connect":"ok"                  # serial fd is open
        },
        "client":                           # client peer slot
        {
            "rx":"0",                       # 0 bytes from peer
            "tx":"930",                     # 930 bytes of NMEA forwarded to peer
            "ip":"127.0.0.1",               # peer address
            "connect":"ok"                  # client socket is connected
        }
    }
    ```


#### Control APIs

+ `tty_cmd[ gnss_object, cmd ]` **send a command string to the GNSS UART**
    - gnss_object ------- [ string ], instance name
    - cmd --------------- [ string ], payload; driver appends CRLF
    - failed return tfalse / terror when process not running or cmd empty
    - succeed return ttrue

    Example
    ```shell
    gnssdrv@nmea.tty_cmd[ gnss@nmea, $PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ]
    ttrue
    ```

+ `reset[ gnss_object, ifname ]` **reconnect matching TCP/UDP clients on network event**
    - gnss_object ------- [ string ], instance name
    - ifname ------------ [ json ], object with `"ifname"` for the interface that came online
    - For each `client*` slot: if `extern` is `"default"` or matches `ifname`, close and reopen that client
    - Server listeners are not affected
    - Invoked by `gnss@nmea.reset` when `drvcom` is `gnssdrv@nmea`

    Example
    ```shell
    gnssdrv@nmea.reset[ gnss@nmea, {"ifname":"ifname@wan"} ]
    ttrue
    ```
