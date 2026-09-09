## clock@date — System date and time

### Overview

Manage system wall clock, timezone, and NTP client synchronization for `clock@date`.
- Prefer the APIs below for status, manual time set, and one-shot NTP sync
- When `ntpclient` is `enable`, a supervised service starts after WAN **online** (or after config set)
- Optional `adjust` mode runs continuous `-s -l` for higher precision after the first hard set


### Configuration reference ( clock@date )

```json
// Attributes introduction 
{
    "inittime":"boot fallback wall time",                 // [ string ], hour:minute:second:month:day:year, applied in setup with timezone
    "timezone":"time zone",                               // [ string ], -12 to 12, West 12 to East 12, half zones like 3:30 / -3:30
    "ntpclient":"start NTP client service",               // [ "disable","enable" ], default be "enable" in package cfg
    "adjust":"NTP high-precision lock mode",              // [ "disable","enable" ], default be "disable"
                                                                // "disable": periodic one-shot hard sync (-s) using ntpinterval
                                                                // "enable": one-shot -s on first reachable server, then stay in -s -l
                                                                //           (hard set + frequency lock each probe)
    "ntpserver":"NTP server",                             // [ string ], tried first
    "ntpserver2":"NTP server 2",                          // [ string ], optional fallback
    "ntpserver3":"NTP server 3",                          // [ string ], optional fallback
    // "ntpserverN":"..."  How many servers show how many properties (N from 2 upward)
    "ntpinterval":"NTP interval in seconds"               // [ number ]
                                                                // adjust=disable: seconds between one-shot -s syncs; <=0 means pause after first success
                                                                // adjust=enable: -s -l probe period; valid 15..600, otherwise clamped to 15 with a warning
}
```

#### Configuration example

Example, show all the configure
```shell
clock@date
{
    "timezone":"8",                   # East 8
    "ntpclient":"enable",             # start NTP client service
    "adjust":"disable",               # periodic one-shot sync
    "ntpserver":"ntp1.aliyun.com",    # try in order until one succeeds
    "ntpserver2":"ntp2.aliyun.com",
    "ntpserver3":"ntp3.aliyun.com",
    "ntpinterval":"86400"             # one-shot sync every 86400 seconds
}
```

#### Configuration settings example

Example, set time zone to West 5
```shell
clock@date:timezone=-5
ttrue
```

Example, disable the NTP client
```shell
clock@date:ntpclient=disable
ttrue
```

Example, enable high-precision adjust lock with 15s probe interval
```shell
clock@date|{"ntpclient":"enable","adjust":"enable","ntpinterval":"15","ntpserver":"192.168.32.230"}
ttrue
```

Example, merge set timezone and NTP client( include "timezone" "ntpclient" "ntpserver" )
```shell
clock@date|{"timezone":"8","ntpclient":"enable","ntpserver":"pool.ntp.org"}
ttrue
```



### Concepts

NTP service behavior when `ntpclient` is `enable`:
- Servers are tried in order: `ntpserver`, `ntpserver2`, …
- First server that completes a successful one-shot sync is used
- `adjust=disable`: sleep `ntpinterval` (or `pause` if <=0), then sync again
- `adjust=enable`: after the first successful `-s`, stay on that server with continuous `-s -l` at probe period `ntpinterval` (clamped to 15..600); each probe hard-sets the clock and adjusts frequency; the process runs until killed; if it exits, wait about 10 seconds and retry from the server list
- Manual `ntpsync[]` always performs one-shot sync only (does not enter adjust lock)


### Joint Events Hook

| Joint key | Invokes |
|-----------|---------|
| `network/online` | `clock@date.online` |

When WAN is marked online, `online[]` starts the NTP client service if `ntpclient` is `enable`.



### API Reference

#### Management APIs

+ `setup[]` **apply saved timezone and optional inittime**
    - failed return tfalse
    - succeed return ttrue
    - Not run automatically during `init` in the default clock package; call from integration if timezone must be applied early

+ `shut[]` **stop the NTP client service and kill ntpclient**
    - failed return tfalse
    - succeed return ttrue
    - Not run automatically on `uninit` in the default integration; call explicitly if needed on shutdown

+ `online[]` **start NTP client service when ntpclient is enable**
    - failed return tfalse
    - succeed return ttrue
    - Invoked by joint `network/online`


#### Query APIs

+ `status[]` **get date information**
    - failed return NULL
    - error return terror
    - succeed return [ json ], date information

    ```json
    {
        "source":"time source tag",                       // [ "ntp", "set", "lte", "gps", "rtc" ], empty if unset
                                                                // ntp has highest priority among sync sources
        "current":"current date",                         // [ string ], hour:minute:second:month:day:year
        "livetime":"system live time",                    // [ string ], hour:minute:second:day
        "uptime":"system uptime in seconds"               // [ number ]
    }
    ```

    Example, get the current date status
    ```shell
    clock@date.status
    {
        "current":"12:29:41:05:10:2022",         # 12:29:41 on May 10, 2022
        "livetime":"00:01:58:0",                 # live 1 minute 58 seconds
        "uptime":"118"                           # uptime 118 seconds
    }
    ```

+ `current[]` **get current time fields**
    - failed return NULL
    - succeed return [ json ], current time

    ```json
    {
        "sec":"seconds since 1970-01-01 00:00:00",        // [ number ]
        "usec":"current microsecond",                     // [ number ]
        "hour":"local hour of day",                       // [ number ], 0-23
        "minute":"local minute",                          // [ number ], 0-59
        "second":"local second",                          // [ number ], 0-59
        "ms":"local millisecond",                         // [ number ], 0-999
        "minuteswest":"minutes west of Greenwich",        // [ number ]
        "dsttime":"type of DST correction"                // [ number ]
    }
    ```

    Example, get current time
    ```shell
    clock@date.current
    {
        "sec":"1747327771",
        "usec":"803417",
        "hour":"14",
        "minute":"48",
        "second":"31",
        "ms":"803",
        "minuteswest":"-480",
        "dsttime":"0"
    }
    ```


#### Control APIs

+ `current[ current date, time zone ]` **set current date and/or time zone**
    - current date ------- [ string ], optional, hour:minute:second:month:day:year
    - time zone ---------- [ string ], optional, -12 to 12 including half zones
    - failed return tfalse
    - succeed return ttrue
    - Omitting both arguments is the query form under Query APIs

    Example, set current date 11:12:23 on July 8, 2019
    ```shell
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```

    Example, set time zone to China East 8
    ```shell
    clock@date.current[ , 8 ]
    ttrue
    ```

+ `ntpsync[ NTP server ]` **one-shot sync with NTP server**
    - NTP server --------- [ string ], optional, use configured servers in order when omitted
    - failed return tfalse
    - succeed return ttrue
    - error return terror
    - Always one-shot hard sync; does not start adjust lock mode

    Example, sync with an explicit server
    ```shell
    clock@date.ntpsync[ time.windows.com ]
    ttrue
    ```

    Example, sync using configured servers
    ```shell
    clock@date.ntpsync
    ttrue
    ```



### Published Joint Events

The following joint events are published when the wall clock is changed. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `date/modify` | Sent after a successful wall-clock change. Argument is a short source tag such as `set` (manual `current[…]`) or `ntp` (successful NTP one-shot sync). |
