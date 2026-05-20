## modem@lte — LTE/NR Modem Management

### Overview

Manage LTE/NR modem baseband services. This component handles the modem-side operations including SIM management, AT command execution, network registration, and signal monitoring. It is the lower-layer device component used by **`ifname@lte`** for end-to-end dialing and routing. Each modem instance is bound to a specific modem driver (e.g. `modem@ec2x`, `modem@rm500u`) that implements the actual AT command interface.

- manages modem lifecycle: setup, shutdown, reset, service restart
- provides SIM card management: detection, PIN/PUK handling, ICCID/IMSI/IMEI queries
- supports dual-SIM failover with configurable thresholds and timed switching
- executes custom AT commands during setup and periodic watch phases
- exposes modem status including signal, PLMN, network type, and operator information



### Network Architecture

`modem@lte` is the **device layer** component that manages the LTE/NR baseband. It does NOT interact directly with `network@frame`; instead, `ifname@lte` (using `ifname@ltecon` as concom) delegates device operations to `modem@lte` via the ifdev binding. When `modem@lte` starts, it registers its netdev (e.g. `usb0`) with `network@frame`. Modem-side configs (`sms`, `gnss`, `atport`, `lock_*`, `custom_*`, `watch_interval`) are stored here but accessible through `ifname@lte` as a unified view.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( modem@lte )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ]
                                                                     // "enable" start modem service automatically with system
                                                                     // "disable" keep modem service stopped until manually started

    // SMS and auxiliary services
    "sms":"SMS function status",                                 // [ "disable", "enable" ]
    "gnss":"GNSS function status",                               // [ "disable", "enable" ]
    "atport":"AT port function status",                          // [ "disable", "enable" ]

    // Lock attributes
    "lock_nettype":"preferred RAT lock policy",                  // [ "auto", "2g", "3g", "4g", "nsa", "sa" ]
    "lock_imei":"lock IMEI function",                            // [ "disable", "enable", "specific imei string" ]
                                                                     // "enable" learn current value and store as lock target
                                                                     // "disable" no lock check
                                                                     // "<value>" enforce exact value match
    "lock_imsi":"lock IMSI function",                            // [ "disable", "enable", "specific imsi string" ]

    // Custom AT commands
    "custom_set":                             // custom AT commands executed once during setup
    {
        "custom name":"AT command"             // [ string ]: [ string ]
        // "...":"..."  How many commands show how many properties
    },
    "custom_watch":                           // custom AT commands executed periodically during watch
    {
        "custom name":"AT command"             // [ string ]: [ string ]
        // "...":"..."  How many commands show how many properties
    },

    // Watch attributes
    "watch_interval":"modem watch interval",                     // [ number ], unit is second, default is 8

    // Profile attributes (for standalone modem operation)
    "pin":"SIM PIN code",                                        // [ string ]
    "profile":"use custom profile",                              // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile, used when "profile" is "enable"
    {
        "dial":"dial number",                     // [ string ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // [ "pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password"                  // [ string ]
    }
}
```

#### Configuration example

Example, show full configuration of the first LTE modem
```shell
modem@lte
{
    "gnss":"enable",                   # enable GNSS function
    "custom_set":                      # execute AT+COPS=3,2 first, then AT+CPIN=1234 during setup
    {
        "1":"AT+COPS=3,2",
        "2":"AT+CPIN=1234"
    },
    "custom_watch":                    # execute AT+CPIN and ATI during watch cycle
    {
        "1":"AT+CPIN",
        "2":"ATI"
    }
}
```

#### Configuration settings example

Example, enable SMS for the first LTE modem
```shell
modem@lte:sms=enable
ttrue
```

Example, set custom APN profile
```shell
modem@lte:profile=enable
ttrue
```

Example, merge set modem configure( include "sms" "gnss" "watch_interval" )
```shell
modem@lte|{"sms":"enable","gnss":"enable","watch_interval":"10"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the modem service**
    - failed return tfalse
    - succeed return ttrue
    - checks status config, starts the modem service subprocess
    - driven by **`usbdrv@…`** / modem driver registration; not listed in the default modem package init table

+ `shut[]` **shut down the modem service**
    - succeed return ttrue
    - deletes from NMEA and network frames, disconnects, stops service


#### Query APIs

+ `status[]` **get modem status information**
    - failed return NULL
    - succeed return [ json ], modem status snapshot including SIM state, registration, RF quality, operator
    ```json
    {
        "status":"Current state",        // [ "nodevice", "setup", "register", "up", "idle", "noimei", "noimsi", "reset", "down" ]
                                             // "nodevice" modem device is not present
                                             // "setup" modem initialization in progress
                                             // "register" network registration in progress
                                             // "up" modem is ready for network access
                                             // "idle" modem enters idle/error state
                                             // "noimei" IMEI lock check failed or unavailable
                                             // "noimsi" IMSI lock check failed or unavailable
                                             // "reset" modem reset in progress
                                             // "down" modem service is stopped
        "imei":"IMEI number",           // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ string, "nosim", "pin", "puk" ]
                                            // string for iccid number
                                            // "nosim" SIM card not detected
                                            // "pin" SIM requires PIN code
                                            // "puk" SIM PIN is blocked, PUK required
        "mversion":"modem version",     // [ string ]
        "name":"modem name",            // [ string ]
        "plmn":"MCC and MNC",           // [ string, "noreg", "unreg", "dereg" ]
                                            // string for MCC and MNC
                                            // "noreg" cannot register to operator
                                            // "unreg" currently not registered
                                            // "dereg" registration rejected by operator
        "nettype":"network type",       // [ string ]
                                            // 2G usually shows GSM, GPRS, EDGE, CDMA
                                            // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                            // 4G usually shows LTE, FDD, TDD
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], "0" no signal, "1" weakest, "4" strongest
        "csq":"CSQ number",             // [ number ]
        "rssi":"signal intensity",      // [ number ], the unit is dBm
        "rsrp":"RSRP value",            // [ string ], Optional, format varies by module
        "rsrq":"RSRQ value",            // [ string ], Optional, format varies by module
        "sinr":"sinr value",            // [ string ], Optional, format varies by module
        "band":"current band",          // [ string ], Optional, format varies by module
        "ci":"cell identity",           // [ string ], Optional
        "lac":"location area code",     // [ string ], Optional
        "operator":"operator name",     // [ string ]
        "na":"5G network access status" // [ "enable" ], Optional, present when modem has 5G NA/NSA flag
    }
    ```

    Example, get status of the first LTE modem
    ```shell
    modem@lte.status
    {
        "imei":"867160040494084",
        "imsi":"460015356123463",
        "iccid":"89860121801097564807",
        "csq":"3",
        "signal":"3",
        "status":"up",
        "plmn":"46001",
        "nettype":"WCDMA",
        "rssi":"-107",
        "operator":"ChinaMobile"
    }
    ```

+ `tty[ type ]` **get modem tty devices**
    - type ------------- [ string ], optional, tty type: "stty", "mtty", "gtty"
    - failed return NULL
    - succeed return [ json ] when no type specified, [ string ] when type specified
    ```json
    {
        "stty":"status tty device",             // [ string ], AT/status channel
        "mtty":"misc tty device",               // [ string ], data/PPP or secondary AT channel
        "gtty":"GNSS tty device"                // [ string ], GNSS/NMEA channel (if available)
    }
    ```

    Example, get all tty devices of first LTE modem
    ```shell
    modem@lte.tty
    {
        "stty":"/dev/ttyUSB1",
        "mtty":"/dev/ttyUSB2",
        "gtty":"/dev/ttyUSB3"
    }
    ```

    Example, get specific tty device
    ```shell
    modem@lte.tty[ stty ]
    /dev/ttyUSB1
    ```

+ `sim[]` **get SIM card state**
    - failed return tfalse (no SIM card)
    - error return terror
    - succeed return ttrue (SIM works normally)
    - returns [ string ] "pin" when SIM requires PIN
    - returns [ string ] "puk" when SIM requires PUK

    Example, check SIM state
    ```shell
    modem@lte.sim
    ttrue
    ```

+ `imei[]` **get modem IMEI**
    - failed return NULL
    - succeed return [ string ], IMEI number

    Example, get IMEI of first LTE modem
    ```shell
    modem@lte.imei
    8986032474898527548
    ```

+ `imsi[]` **get SIM IMSI**
    - failed return NULL
    - succeed return [ string ], IMSI number

    Example, get IMSI of first LTE modem
    ```shell
    modem@lte.imsi
    460115664109131
    ```

+ `iccid[]` **get SIM ICCID**
    - failed return NULL
    - succeed return [ string ], ICCID number

    Example, get ICCID of first LTE modem
    ```shell
    modem@lte.iccid
    8986032474898527548
    ```

+ `plmn[]` **get current PLMN**
    - failed return NULL
    - succeed return [ string ], PLMN code

    Example, get PLMN of first LTE modem
    ```shell
    modem@lte.plmn
    46011
    ```

+ `signal[]` **get signal level**
    - failed return NULL (when signal is 0, returns NULL instead of 0)
    - succeed return [ number ], signal level 0-4

    Example, get signal of first LTE modem
    ```shell
    modem@lte.signal
    2
    ```

+ `operator[]` **get current operator profile**
    - failed return NULL
    - succeed return [ json ], operator profile used as default APN when profile_cfg is not set
    ```json
    {
        "name":"operator name",               // [ string ]
        "dial":"*dial number",                // [ string ]
        "apn":"APN code"                      // [ string ]
    }
    ```

    Example, get operator profile
    ```shell
    modem@lte.operator
    {
        "name":"中国联通",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ```

+ `netdev[]` **get modem network device name**
    - failed return NULL
    - succeed return [ string ], network device name (e.g. "usb0", "wwan0")

    Example, get network device of first LTE modem
    ```shell
    modem@lte.netdev
    usb0
    ```

+ `fun[]` **check if modem is functional**
    - succeed return ttrue when modem is in functional state (ATD_WATCH or ATD_READY)
    - failed return tfalse when modem is in non-functional state (ATD_NONE, ATD_CFUN, ATD_SETUP)

    Example, check if first LTE modem is functional
    ```shell
    modem@lte.fun
    ttrue
    ```

+ `custom_set[]` **get custom_set AT command results**
    - failed return NULL
    - succeed return [ json ], response of each custom_set AT command
    ```json
    {
        "custom name":"AT response"      // [ string ]: [ string ]
        // "...":"..."  How many commands show how many responses
    }
    ```

    Example, get custom_set results
    ```shell
    modem@lte.custom_set
    {
        "1":"OK",
        "2":"+CPIN: READY OK"
    }
    ```

+ `custom_watch[]` **get custom_watch AT command results**
    - failed return NULL
    - succeed return [ json ], response of each custom_watch AT command
    ```json
    {
        "custom name":"AT response"      // [ string ]: [ string ]
        // "...":"..."  How many commands show how many responses
    }
    ```

    Example, get custom_watch results
    ```shell
    modem@lte.custom_watch
    {
        "1":"+CPIN: READY OK",
        "2":"Quectel EC200T OK"
    }
    ```

+ `bsim_state[]` **get current SIM slot state**
    - succeed return [ string ], "main" or "back"

    Example, get current SIM slot
    ```shell
    modem@lte.bsim_state
    main
    ```


#### Control APIs

+ `pin[ pin code ]` **submit PIN code to unlock SIM**
    - pin code ---------- [ string ], optional, if not provided uses configured `pin`
    - failed return tfalse (SIM in PUK state or wrong PIN)
    - succeed return ttrue

    Example, unlock SIM with explicit PIN
    ```shell
    modem@lte.pin[ 123456 ]
    ttrue
    ```

    Example, unlock SIM using configured PIN
    ```shell
    modem@lte.pin[]
    ttrue
    ```

+ `at[ at command ]` **execute a raw AT command**
    - at command -------- [ string ]
    - failed return NULL
    - succeed return [ string ], modem AT response
    - passthrough/debug API for diagnostics

    Example, query SIM state
    ```shell
    modem@lte.at[ at+cpin? ]
    +CPIN: READY

    OK
    ```

    Example, query network registration
    ```shell
    modem@lte.at[ at+creg? ]
    +CREG: 2,1,"A538","1EB3FB7",2

    OK
    ```

+ `reset[]` **reset the modem**
    - failed return tfalse (when devbus is missing)
    - succeed return ttrue
    - triggers modem reset workflow and service restart sequence

    Example, reset the first LTE modem
    ```shell
    modem@lte.reset
    ttrue
    ```

+ `reset_clear[]` **clear modem reset counter**
    - succeed return ttrue
    - resets internal reset_times and reset_uptime counters to zero

    Example, clear reset counter
    ```shell
    modem@lte.reset_clear
    ttrue
    ```

+ `lock_imei[ value ]` **get or set IMEI lock target**
    - value ------------- [ string ], optional, IMEI to lock; omit to query current value
    - failed return NULL (when querying and not set)
    - succeed return [ string ] (current value when querying) or ttrue (when setting)

    Example, get current lock IMEI
    ```shell
    modem@lte.lock_imei
    867160040494084
    ```

    Example, set lock IMEI
    ```shell
    modem@lte.lock_imei[ 867160040494084 ]
    ttrue
    ```

+ `lock_imsi[ value ]` **get or set IMSI lock target**
    - value ------------- [ string ], optional, IMSI to lock; omit to query current value
    - failed return NULL (when querying and not set)
    - succeed return [ string ] (current value when querying) or ttrue (when setting)

    Example, get current lock IMSI
    ```shell
    modem@lte.lock_imsi
    460015356123463
    ```

    Example, set lock IMSI
    ```shell
    modem@lte.lock_imsi[ 460015356123463 ]
    ttrue
    ```

+ `order[ command, value ]` **execute a generic driver command**
    - command ----------- [ string ], driver command name
    - value ------------- [ json ], optional parameter passed to the driver
    - failed return NULL
    - succeed return [ string ], driver response
    - passthrough API that forwards arbitrary commands to the modem driver layer

    Example, execute a driver-specific command
    ```shell
    modem@lte.order[ nettype ]
    LTE
    ```

+ `bsim_back[]` **switch to backup SIM card**
    - failed return tfalse (when devbus is missing)
    - succeed return ttrue
    - deletes services, switches GPIO to backup SIM, resets modem

    Example, switch to backup SIM
    ```shell
    modem@lte.bsim_back
    ttrue
    ```

+ `bsim_main[]` **switch to main SIM card**
    - failed return tfalse (when devbus is missing)
    - succeed return ttrue
    - deletes services, switches GPIO to main SIM, resets modem

    Example, switch to main SIM
    ```shell
    modem@lte.bsim_main
    ttrue
    ```

+ `bsim_clear[]` **clear backup SIM counters**
    - succeed return ttrue
    - resets bsim_times and switch_uptime counters to zero

    Example, clear backup SIM counters
    ```shell
    modem@lte.bsim_clear
    ttrue
    ```

+ `bsim_over[ seconds ]` **timed failover to main SIM**
    - seconds ----------- [ number ], seconds to sleep before switching
    - succeed return ttrue
    - sleeps for specified seconds then switches to main SIM

    Example, failover to main SIM after 300 seconds
    ```shell
    modem@lte.bsim_over[ 300 ]
    ttrue
    ```

+ `sms_send[ number, message ]` **send an SMS message**
    - number ------------ [ string ], destination phone number
    - message ----------- [ string ], SMS text content
    - failed return tfalse (sms not enabled or sms_object missing)
    - succeed return ttrue
    - request forwarded to SMS service object

    Example, send an SMS
    ```shell
    modem@lte.sms_send[ 10086, hello ]
    ttrue
    ```

+ `sms_list[]` **list received SMS messages**
    - failed return tfalse (sms not enabled or sms_object missing)
    - succeed return [ json ], SMS list
    - request forwarded to SMS service object

+ `sms_delete[ index ]` **delete an SMS message**
    - index ------------- [ string ], SMS identifier to delete
    - failed return tfalse
    - succeed return ttrue
    - request forwarded to SMS service object


#### API availability by modem state

| API | unavailable states (returns NULL or terror) |
|-----|---------------------------------------------|
| `status` | always available |
| `tty` | always available |
| `fun` | always available |
| `imei`, `imsi`, `iccid`, `operator` | nodevice, down, reset |
| `sim`, `pin` | nodevice, down, reset, idle, noimei, noimsi |
| `plmn`, `signal` | nodevice, down, reset, idle, noimei, noimsi |
| `at` | nodevice, down, reset |
| `reset` | requires devbus to be present |
| `custom_set`, `custom_watch` | nodevice, down, reset |
| `order` | nodevice, down, reset |
| `lock_imei`, `lock_imsi` | always available |
| `reset_clear` | always available |
| `netdev` | always available |
| `bsim_back`, `bsim_main` | requires devbus to be present |
| `bsim_state`, `bsim_clear`, `bsim_over` | always available |
| `sms_send`, `sms_list`, `sms_delete` | requires `sms` to be "enable" and sms_object to be present |



### Published Joint Events

The following joint events are published when modem state changes. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `date/modify` | Sent when the modem's real-time clock is used to set the system time. Triggered during the watch phase when AT+CCLK response is parsed. The event parameter is the modem object name. |
