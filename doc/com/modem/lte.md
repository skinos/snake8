## modem@lte — LTE/NR Modem Management
Manage LTE/NR modem baseband services.

This document describes the modem-side component (`modem@lte`, `modem@lte2`) only.
It focuses on baseband control, SIM/AT operations, and modem runtime status.
For end-to-end dialing, routing, and `network@frame` integration, use the logical LTE interface document [`../ifname/lte.md`](../ifname/lte.md) (`ifname@lte`, …).

### Configuration ( `modem@lte` )
**modem@lte** is first LTE modem  
**modem@lte2** is second LTE modem

```json
// Attributes introduction 
{
    // Status for baseband
    // "enable": start modem service automatically with system
    // "disable": keep modem service stopped until manually started
    "status":"start at system startup",    // [ "enable", "disable" ]

    // SMS function for baseband
    // Enable this when you need modem SMS send/receive capability
    "sms":"SMS status",                        // [ "disable", "enable" ]

    // GNSS function for baseband
    // Enable this when you need GNSS/NMEA output from modem
    // Note: handled by NMEA framework externally, not processed by atd itself
    "gnss":"GNSS status",                      // [ "disable", "enable" ]

    // AT port function for baseband
    // Expose modem AT transparent service for external tools
    "atport":"AT port status",                 // [ "disable", "enable" ]

    // lock attributes
    // lock_nettype: preferred RAT lock policy, applied by modem driver (not processed by atd itself)
    // lock_imei/lock_imsi:
    //   "enable" -> learn current value and store it as lock target
    //   "<value>" -> enforce exact value match
    //   "disable" -> no lock check
    "lock_nettype":"network type",             // [ "auto", "2g", "3g", "4g", "nsa", "sa" ]
    "lock_imei":"lock imei function",          // [ "disable", "enable", "specific imei string" ]
    "lock_imsi":"lock imsi function",          // [ "disable", "enable", "specific imsi string" ]

    // custom at command
    // custom_set: execute once during setup stage
    // custom_watch: execute periodically during watch stage
    // Response text is saved into status JSON under custom_* result fields
    "custom_set":                              // custom at setting list at the modem setup
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...more AT command
    },
    "custom_watch":                            // custom at watch list at the modem watch
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...more AT command
    },

    // watch attributes
    // Polling interval for periodic modem watch task
    "watch_interval":"modem watch interval",  // [ number ], unit: second, default is 8

    // attributes for work alone to use
    // pin/profile/profile_cfg are used by modem-side autonomous setup flow
    "pin":"SIM PIN code",                      // [ string ]
    "profile":"use custom profile",            // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile, used when "profile" is "enable"
    {
        "dial":"dial number",                     // [ number ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // [ "pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password"                  // [ string ]
    }

}
```

Example: show full configuration of the first LTE modem
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
        "2":"ATI"
        "1":"AT+CPIN",
    }
}
```  

Example: enable SMS for the first LTE modem
```shell
modem@lte:sms=enable
ttrue
```  

### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
**modem@lte** is first LTE modem
**modem@lte2** is second LTE modem

+ `status[]` **get modem status information**
    - failed: return NULL
    - error: return terror
    - success: return a JSON status object
    - this is the primary health snapshot for upper layers
      (SIM state, registration state, RF quality, operator, etc.)
    ```json
    // Attributes introduction of json by the method return
    {
        "status":"Current state",        // [ "setup", "register", "up", "idle", "noimei", "noimsi", "reset", "down", "nodevice" ]
                                             // "setup" modem initialization in progress
                                             // "register" network registration in progress
                                             // "up" modem is ready for network access
                                             // "idle" modem enters idle/error state
                                             // "reset" modem reset in progress
                                             // "down" modem service is stopped
                                             // "nodevice" modem device is not present
        "imei":"IMEI number",           // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                // number for iccid
                                                // "nosim" SIM card not detected
                                                // "pin" SIM requires PIN code
                                                // "puk" SIM PIN is blocked, PUK required
        "mversion":"modem version",     // [ string ]
        "name":"modem name",            // [ string ]
        "plmn":"MCC and MNC",           // [ number, "noreg", "unreg", "dereg" ]
                                                // number for MCC and MNC
                                                // "noreg" cannot register to operator
                                                // "unreg" currently not registered
                                                // "dereg" registration rejected by operator
        "nettype":"network type",        // The format varies depending on the module
                                         // 2G usually shows GSM, GPRS, EDGE, CDMA
                                         // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G usually shows LTE, FDD, TDD
        "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
        "csq":"CSQ number",              // [ number ]
        "rssi":"signal intensity",       // [ number ], the unit is dBm
        "rsrp":"RSRP value",             // Optional, The format varies depending on the module
        "rsrq":"RSRQ value",             // Optional, The format varies depending on the module
        "sinr":"sinr value",             // Optional, The format varies depending on the module 
        "band":"current band",           // Optional, The format varies depending on the module
        "ci":"cell identity",            // Optional, [ string ]
        "lac":"location area code",      // Optional, [ string ]
        "operator":"operator name",      // [ string ]
        "na":"5G network access status"  // Optional, [ "enable" ], present when modem has NA/NSA(5G) network access flag set
    }
    ```

    Example: get status of the first LTE modem
    ```shell
    modem@lte.status
    {
        "imei":"867160040494084",          # imei is 867160040494084
        "imsi":"460015356123463",          # imsi is 460015356123463
        "iccid":"89860121801097564807",    # iccid is 89860121801097564807
        "csq":"3",                         # CSQ nubmer is 3
        "signal":"3",                      # signal level is 3
        "status":"up",                     # modem is ready for internet connection
        "plmn":"46001",                    # plmn is 46001
        "nettype":"WCDMA",                 # nettype is WCDMA
        "rssi":"-107",                     # signal intensity is -107
        "operator":"ChinaMobile"           # operator name is ChinaMobile
    }
    ```

+ `tty[ [type] ]` **get modem tty devices**
    - type ----------- [ string ], tty type 
    - failed: return NULL
    - error: return terror
    - success: return tty path when type is specified
    - success: return tty JSON object when no type is specified
    - typical usage:
      - `stty`: AT/status channel
      - `mtty`: data/PPP or secondary AT channel
      - `gtty`: GNSS/NMEA channel (if available)
    ```json
    // Attributes introduction of json by the method return
    {
        "stty":"status tty device",             // [ string ]
        "mtty":"misc tty device",               // [ string ]
        "gtty":"GNSS tty device"                // [ string ] 
    }
    ```

    Example, get the first lte modem tty list
    ```shell
    modem@lte.tty
    {
        "stty":"/dev/ttyUSB1",
        "mtty":"/dev/ttyUSB2",
        "gtty":"/dev/ttyUSB3"
    }
    ```

    Example, get the first lte modem status tty
    ```shell
    modem@lte.tty[ stty ]
    /dev/ttyUSB1
    ```

    Example, get the first lte modem GNSS tty
    ```shell
    modem@lte.tty[ gtty ]
    /dev/ttyUSB3
    ```

+ `sim[]` **get SIM card state**
    - no SIM card: return tfalse
    - error: return terror
    - SIM works normally: return ttrue
    - SIM requires PIN: return "pin"
    - SIM requires PUK: return "puk"
    - this API is frequently polled by network orchestrator before dialing
    
    Example: get SIM state of the first LTE modem
    ```shell
    modem@lte.sim
    ttrue
    ```

    Example: get SIM state of the second LTE modem
    ```shell
    modem@lte2.sim
    pin
    ```

+ `pin[ [pin code] ]` **submit PIN code to unlock SIM**
    - pin code ----------- [ string ]
    - failed: return tfalse
    - error: return terror
    - success: return ttrue
    - if no pin code is provided, configured modem-side `pin` is used
    - if SIM is in PUK state, the operation is refused and returns tfalse
    
    Example: unlock SIM with explicit PIN
    ```shell
    modem@lte.pin[ 123456 ]
    ttrue
    ```

    Example: unlock SIM without argument (use configured `pin`)
    ```shell
    modem@lte2.pin[]
    ttrue
    ```

+ `imei[]` **get modem IMEI**
    - failed: return NULL
    - error: return terror
    - success: return IMEI string
    
    Example, get the imei of first lte modem
    ```shell
    modem@lte.imei
    8986032474898527548
    ```

+ `imsi[]` **get SIM IMSI**
    - failed: return NULL
    - error: return terror
    - success: return IMSI string
    
    Example, get the imsi of first lte modem
    ```shell
    modem@lte.imsi
    460115664109131
    ```

+ `iccid[]` **get SIM ICCID**
    - failed: return NULL
    - error: return terror
    - success: return ICCID string
    
    Example, get the iccid of first lte modem
    ```shell
    modem@lte.iccid
    8986032474898527548
    ```

+ `plmn[]` **get the plmn of modem network**
    - failed: return NULL
    - error: return terror
    - success: return PLMN string
    
    Example, get the plmn of first lte modem
    ```shell
    modem@lte.plmn
    46011
    ```

+ `signal[]` **get the signal of modem network**
    - failed: return NULL
    - error: return terror
    - success: return signal level number, 0-4, 0 means no signal, 4 means strongest signal
    - note: when signal is 0, returns NULL instead of 0
    
    Example, get the signal of first lte modem
    ```shell
    modem@lte.signal
    2
    ```

+ `operator[]` **get the modem current operator profile**
    - failed: return NULL
    - error: return terror
    - success: return operator profile JSON
    - used by upper layers as default APN/profile when profile_cfg is not explicitly provided
    ```json
    // Attributes introduction of json by the method return
    {
        "name":"operator name",               // [ string ]
        "dial":"*dial number",                // [ string ]
        "apn":"APN code"                      // [ string ]
    }
    ```

    Example, get the first lte modem profile
    ```shell
    modem@lte.operator
    {
        "name":"中国联通",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ```


+ `at[ at command ]` **execute a raw AT command**
    - at command ----------- [ string ]
    - failed: return NULL
    - error: return terror
    - success: return modem AT response string
    - this is a passthrough/debug API, recommended for diagnostics instead of normal control flow
    
    Example: query SIM state with a raw AT command
    ```shell
    modem@lte.at[at+cpin?]
    +CPIN: READY

    OK
    ```

    Example: query network registration with a raw AT command
    ```shell
    modem@lte2.at[at+creg?]
    +CREG: 2,1,"A538","1EB3FB7",2

    OK
    ```

+ `reset[]` **reset the modem**
    - failed: return tfalse
    - error: return terror
    - success: return ttrue
    - this triggers modem reset workflow and service restart sequence
    
    Example, reset the first lte modem
    ```shell
    modem@lte.reset
    ttrue
    ```

    Example, reset the second lte modem
    ```shell
    modem@lte2.reset
    ttrue
    ```

+ `reset_clear[]` **clear modem reset counter**
    - success: return ttrue
    - clears the internal reset_times and reset_uptime counters
    - useful after manual intervention to prevent unnecessary escalation
    
    Example, clear the reset counter of first lte modem
    ```shell
    modem@lte.reset_clear
    ttrue
    ```

+ `netdev[]` **get modem network device name**
    - failed: return NULL
    - success: return network device name string (e.g. "usb0", "wwan0")
    
    Example, get the network device of first lte modem
    ```shell
    modem@lte.netdev
    usb0
    ```

+ `custom_set[]` **get custom_set AT command execution results**
    - failed: return NULL
    - success: return JSON object containing the response of each custom_set AT command
    - the keys match those configured in the `custom_set` configuration
    
    Example, get the custom_set results of first lte modem
    ```shell
    modem@lte.custom_set
    {
        "1":"OK",
        "2":"+CPIN: READY OK"
    }
    ```

+ `custom_watch[]` **get custom_watch AT command execution results**
    - failed: return NULL
    - success: return JSON object containing the response of each custom_watch AT command
    - the keys match those configured in the `custom_watch` configuration
    
    Example, get the custom_watch results of first lte modem
    ```shell
    modem@lte.custom_watch
    {
        "1":"+CPIN: READY OK",
        "2":"Quectel EC200T OK"
    }
    ```

+ `lock_imei[ [value] ]` **get or set the IMEI lock target value**
    - no argument: return the currently stored lock IMEI string, or NULL if not set
    - with argument: set the lock IMEI value, success return ttrue, failed return tfalse
    
    Example, get the lock IMEI of first lte modem
    ```shell
    modem@lte.lock_imei
    867160040494084
    ```

    Example, set the lock IMEI of first lte modem
    ```shell
    modem@lte.lock_imei[ 867160040494084 ]
    ttrue
    ```

+ `lock_imsi[ [value] ]` **get or set the IMSI lock target value**
    - no argument: return the currently stored lock IMSI string, or NULL if not set
    - with argument: set the lock IMSI value, success return ttrue, failed return tfalse
    
    Example, get the lock IMSI of first lte modem
    ```shell
    modem@lte.lock_imsi
    460015356123463
    ```

    Example, set the lock IMSI of first lte modem
    ```shell
    modem@lte.lock_imsi[ 460015356123463 ]
    ttrue
    ```

+ `order[ command, [value] ]` **execute a generic driver command**
    - command ----------- [ string ], driver command name
    - value ------------- [ JSON ], optional parameter passed to the driver
    - failed: return NULL
    - success: return driver response
    - this is a passthrough API that forwards arbitrary commands to the modem driver layer
    
    Example, execute a driver-specific command
    ```shell
    modem@lte.order[ nettype ]
    LTE
    ```

+ `sms_send[ number, message ]` **send an SMS message**
    - requires `sms` configuration to be "enable"
    - failed: return tfalse
    - success: return ttrue
    - the request is forwarded to the SMS service object
    
    Example, send an SMS
    ```shell
    modem@lte.sms_send[ 10086, hello ]
    ttrue
    ```

+ `sms_list[]` **list received SMS messages**
    - requires `sms` configuration to be "enable"
    - failed: return tfalse
    - success: return SMS list JSON
    - the request is forwarded to the SMS service object

+ `sms_delete[ index ]` **delete an SMS message**
    - requires `sms` configuration to be "enable"
    - index ----------- SMS index to delete
    - failed: return tfalse
    - success: return ttrue
    - the request is forwarded to the SMS service object

+ `fun[]` **check if modem is functional**
    - success: return ttrue when modem is in functional state (ATD_WATCH or ATD_READY)
    - failed: return tfalse when modem is in non-functional state (ATD_NONE, ATD_CFUN, ATD_SETUP)

    Example, check if first LTE modem is functional
    ```shell
    modem@lte.fun
    ttrue
    ```

+ `bsim_back[]` **switch to backup SIM card**
    - failed: return tfalse (when devbus is missing)
    - success: return ttrue
    - switches modem to backup SIM card and resets

    Example, switch to backup SIM
    ```shell
    modem@lte.bsim_back
    ttrue
    ```

+ `bsim_main[]` **switch to main SIM card**
    - failed: return tfalse (when devbus is missing)
    - success: return ttrue
    - switches modem to main SIM card and resets

    Example, switch to main SIM
    ```shell
    modem@lte.bsim_main
    ttrue
    ```

+ `bsim_state[]` **get current SIM slot state**
    - success: return [ string ], "main" or "back"

    Example, get current SIM slot
    ```shell
    modem@lte.bsim_state
    main
    ```

+ `bsim_clear[]` **clear backup SIM counters**
    - success: return ttrue
    - resets bsim_times and switch_uptime counters to zero

    Example, clear backup SIM counters
    ```shell
    modem@lte.bsim_clear
    ttrue
    ```

+ `bsim_over[ seconds ]` **timed failover to main SIM**
    - seconds ---------- [ number ], seconds to sleep before switching
    - success: return ttrue
    - sleeps for specified seconds then switches to main SIM

    Example, failover to main SIM after 300 seconds
    ```shell
    modem@lte.bsim_over[ 300 ]
    ttrue
    ```


#### API availability by modem state
Not all APIs are available in every modem state. The following table shows when each API returns an error/NULL due to state restrictions:

| API | unavailable states (returns NULL or terror) |
|---|---|
| `status` | always available (returns state-specific JSON for all states) |
| `tty` | always available (reads from registry directly) |
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
| `netdev` | always available (reads from registry directly) |
| `bsim_back`, `bsim_main` | requires devbus to be present |
| `bsim_state`, `bsim_clear` | always available |
| `bsim_over` | always available |
| `sms_send`, `sms_list`, `sms_delete` | requires `sms` to be "enable" and sms_object to be present |

### Lifecycle API
+ `setup[]` / `shut[]` — driven by **`usbdrv@…`** / modem driver registration; **not** listed in the default modem package **`init`** table.
+ Consult modem driver component docs (**`modem@ec2x`**, **`modem@rm500u`**, …) for bring-up.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_modem_lte(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "modem@lte", "status") == NULL)
        return -1;
    return ssets_string("modem@lte", "enable", "status") ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* e.g. scall("modem@lte", "list", NULL); talk_free if JSON */
```
