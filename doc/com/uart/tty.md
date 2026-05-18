## uart@tty — UART Port Object

Each serial port is a **saved configuration object** named **`uart@tty`**, **`uart@tty2`**, …
(see `prj.json` **`wui`**). The object stores line settings, an optional **`dtu`** subtree
(TCP/UDP/MQTT when **`drvcom`** is **`uartdrv@dtu`**), and binds to a driver via **`drvcom`**
(**`uartdrv@dtu`**, **`uartdrv@tui`**, …).

Startup is normally driven by **`uart@frame.setup`**, which calls **`uart@tty.setup`** for each
enabled port; that launches the driver **`service`** with this object’s JSON.

---

### Configuration ( `uart@tty` )

The **saved configuration object** for `uart@tty` (query/set via `uart@tty`, `uart@tty:path`, merge `|{json}`, etc.).


Reading or replacing the whole object uses normal Skin **`config_get` / `config_set`**. A successful **`set`**
on the port triggers **`shut`** then **`setup`** for that object (`uart_dev.c` **`_set`**), reloading the driver.

When **`drvcom`** is **`uartdrv@dtu`**, up to **nine** of each kind are loaded under **`dtu`**:
**`client`** / **`client2`…`client9`**, **`server`** / **`server2`…`server9`**, **`mqtt`** / **`mqtt2`…`mqtt9`**.

```json
// Attributes introduction of json by the component configure
{
    "status":"tty function enable or disable",                      // [ "enable", "disable" ]
    "convert":"hardeware function custom",                          // [ "disable", "enable" ]
    "devcom":"uart device component",                               // [ string ], only use for other project provide the ttydev
    "ttydev":"uart tty device",                                     // [ string ], Linux serial device path when not using devcom
    "drvcom":"use the component for function",                      // [ string ], e.g. uartdrv@dtu, uartdrv@tui (see prj.json obj)
    "extern":"reset when the extern online",                        // [ "disable", "default", "ifname@wan", ... ]
                                                                        // "disable" for no reset when ifname online
                                                                        // "default" for reset when the gateway online 
                                                                        // "ifname@wan", "ifname@lte", ... for reset when the ifname online

    "speed":"tty device speed",                                       // [ "9600", "19200", ... ]
    "flow":"tty device flow type",                                    // [ "disable", "hard", "soft" ]
    "parity":"tty device parity",                                     // [ "disable", "even", "odd" ]
    "databit":"tty device data bit",                                  // [ "5", "6", "7", "8" ]
    "stopbit":"tty device stop bit",                                  // [ "1", "2" ]

    "active":"enable or disable send some data to tty device for active the tty",   // [ "disable", "enable", "idle", "timing" ]
                                                                                        // "disable" for no active data
                                                                                        // "enable" for send active data once
                                                                                        // "idle" for send active data when idle interval
                                                                                        // "timing" for send active data timer
    "active_interval":"active data send interval",                                  // [ number ], seconds, with enable idle timing
    "active_string":"active data be send",                                          // [ hex string ], payload bytes as hex

    "frame_maxsize":"read a frame the max size data from uart",                     // [ number ], max frame bytes toward network (DTU)
    "frame_interval":"interval bewteen frame from uart",                            // [ number ], inter-byte timeout in ms (DTU)

    "dtu":                                                  // present when drvcom is uartdrv@dtu
    {
        "client":                                           // TCP or UDP client slot 1
        {
            "status":"enable or disable this client",         // [ "disable", "enable" ]
            "extern":"reset when the extern online",          // [ "disable", "default", "ifname@wan", ... ]
                                                                        // "disable" for no reset when ifname online
                                                                        // "default" for reset when the gateway online 
                                                                        // "ifname@wan", "ifname@lte", ... for reset when the ifname online
            "proto":"tcp or udp protocol",                         // [ "tcp", "udp" ]
            "server":"server address",                             // [ string ], domain or IPv4 literal
            "port":"server port",                                  // [ number ]

            "login":"login packet type",                           // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for no login packet
                                                                        // "hex" login_string well be hex string
                                                                        // "ascii" login_string well be ascii string
                                                                        // "mac" use the device macid for login packet
            "login_string":"login packet content",                 // [ string ]

            "keeplive":"keeplive type",                            // [ "disable", "idle", "enable", "timing" ]
                                                                        // "disable" for no keeplive packet
                                                                        // "idle" idle to keeplive
                                                                        // "timing" timing send keeplive    
                                                                        // "enable" same "timing"
            "keeplive_interval":"keeplive interval",               // [ number ], seconds
            "keeplive_string":"keeplive packet content",           // [ hex string ]

            "frame_start":"frame prefix type",                     // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for frame prefix
                                                                        // "hex" frame_start_string well be hex string
                                                                        // "ascii" frame_start_string well be ascii string
                                                                        // "mac" use the device macid for frame prefix      
            "frame_start_string":"frame prefix content",           // [ string ]

            "frame_end":"frame postfix type",                      // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for frame prefix
                                                                        // "hex" frame_end_string well be hex string
                                                                        // "ascii" frame_end_string well be ascii string
                                                                        // "mac" use the device macid for frame prefix      
            "frame_end_string":"frame postfix content"             // [ string ]
        
        },
        // more the TCP or UDP client named clientX, X be number 1-9
    
        "mqtt":                                             // MQTT client slot 1
        {
            "status":"enable or disable this client",         // [ "disable", "enable" ]
            "extern":"reset when the extern online",          // [ "disable", "default", "ifname@wan", ... ]
                                                                        // "disable" for no reset when ifname online
                                                                        // "default" for reset when the gateway online 
                                                                        // "ifname@wan", "ifname@lte", ... for reset when the ifname online
            "server":"server address",                             // [ string ], domain or IPv4 literal
            "port":"server port",                                  // [ number ]
            "mqtt_id":"device identify",                      // [ string ]
            "mqtt_username":"mqtt username",                  // [ string ]
            "mqtt_password":"mqtt password",                  // [ string ]
            "mqtt_interval":"mqtt interval",                  // [ nubmer ]
            "mqtt_keepalive":"mqtt keepalive",                // [ number ]
            "mqtt_publish":"mqtt publish topic",              // [ string ]
            "mqtt_publish_qos":"mqtt publish qos",            // [ number ]
            "mqtt_subscribe":
            {
                "subscribe topic":"topic qos",      // [ string ]:[ number ]
                // "subscribe topic":"topic qos"     How many subscribe topic need setting save how many properties
            }
        },
        // more the mqtt client named clientX, X be number 1-9

        "server":                                           // TCP or UDP listen slot 1
        {
            "status":"enable or disable this server",         // [ "disable", "enable" ]
            "proto":"tcp or udp protocol",                         // [ "tcp", "udp" ]
            "port":"server port",                                  // [ number ]
            "limit":"concurrence client",                     // [ number ]

            "login":"login packet type",                           // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for no login packet
                                                                        // "hex" login_string well be hex string
                                                                        // "ascii" login_string well be ascii string
                                                                        // "mac" use the device macid for login packet
            "login_string":"login packet content",                 // [ string ]

            "keeplive":"keeplive type",                            // [ "disable", "idle", "enable", "timing" ]
                                                                        // "disable" for no keeplive packet
                                                                        // "idle" idle to keeplive
                                                                        // "timing" timing send keeplive    
                                                                        // "enable" same "timing"
            "keeplive_interval":"keeplive interval",               // [ number ], seconds
            "keeplive_string":"keeplive packet content",           // [ hex string ]

            "frame_start":"frame prefix type",                     // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for frame prefix
                                                                        // "hex" frame_start_string well be hex string
                                                                        // "ascii" frame_start_string well be ascii string
                                                                        // "mac" use the device macid for frame prefix      
            "frame_start_string":"frame prefix content",           // [ string ]

            "frame_end":"frame postfix type",                      // [ "disable", "hex", "ascii", "mac" ]
                                                                        // "disable" for frame prefix
                                                                        // "hex" frame_end_string well be hex string
                                                                        // "ascii" frame_end_string well be ascii string
                                                                        // "mac" use the device macid for frame prefix      
            "frame_end_string":"frame postfix content"             // [ string ]

        }
        // more the server named serverX, X be number 1-9
    }
}
```

Examples, query all configuration of the first UART

```shell
~ # he 'uart@tty'                    # Query configurations of uart@tty (first UART port)
{                                 # Return a complete JSON object
    "status":"enable",                              # enable the first UART
    "convert":"disable",
    "devcom":"",
    "ttydev":"/dev/ttyS1",
    "drvcom":"uartdrv@dtu",
    "extern":"disable",
    "speed":"57600",
    "flow":"disable",
    "parity":"disable",
    "databit":"8",
    "stopbit":"1",
    "active":"disable",
    "active_interval":"60",
    "active_string":"",
    "frame_maxsize":"",
    "frame_interval":"",
    "dtu":
    {
        "client":
        {
            "status":"enable",
            "extern":"disable",
            "proto":"tcp",
            "server":"192.168.8.250",
            "port":"800",
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
        "client2":
        {
            "status":"disable",
            "extern":"disable",
            "proto":"udp",
            "server":"",
            "port":"",
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
        "server":
        {
            "status":"enable",
            "extern":"disable",
            "proto":"tcp",
            "port":"7000",
            "limit":"5",
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
            "status":"disable",
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
~ #
```

Examples, disable the first DTU TCP client

```shell
~ # he 'uart@tty:dtu/client/status=disable'                    # Set dtu client status to disable
ttrue                                                          # ttrue is returned, the change is successful
~ #
```

Examples, set the first DTU server listen port to 8000

```shell
~ # he 'uart@tty:dtu/server/port=8000'                    # Set dtu server port
ttrue                                                     # ttrue is returned, the change is successful
~ #
```

---

### Component API ( `uart@tty` )

These methods are invoked on the **port object** (e.g. **`uart@tty`**), not on **`uart@frame`**.

+ `ttydev[]` **resolved TTY path**
    - none or failed return NULL
    - return string talk for resolved ttydev (or via devcom)

    Example, query tty device path
    ```shell
    uart@tty.ttydev
    /dev/ttyS1
    ```

+ `devcom[]` **device component name**
    - none or failed return NULL
    - return string talk

+ `drvcom[]` **driver component name**
    - none or failed return NULL
    - return string talk

+ `status[]` **driver status**
    - If drvcom is uartdrv@dtu, this object returns NULL — use **`uartdrv@dtu.status[ uart_object ]`** instead.
    - Otherwise return JSON from **`scall(drvcom, "status", uart_object)`**

+ `reset[ event_name, { "ifname": "…" } ]` **event-driven reset**
    - event_name ----------- [ string ], e.g. network/online
    - second parameter ----- JSON object with ifname (required where compared)
    - When drvcom is not uartdrv@dtu: forward **`scallst(drvcom, "reset", uart_object, v)`**
    - When drvcom is uartdrv@dtu: use top-level extern; if extern is default and event_name is network/online, sreset; else if extern equals ifname, sreset
    - For DTU link reconnect by dtu.*.extern, use **`uartdrv@dtu.reset`** below

+ `clear_ca[ slot ]` **remove TLS CA file for a DTU slot**
    - slot ----------------- [ string ], e.g. client, server, client2
    - failed return tfalse
    - succeed return ttrue

    Example, clear client CA
    ```shell
    uart@tty.clear_ca[ client ]
    ttrue
    ```

+ `clear_cert[ slot ]` **remove TLS client cert file for a DTU slot**
    - slot ----------------- [ string ]
    - failed return tfalse
    - succeed return ttrue

+ `clear_key[ slot ]` **remove TLS private key file for a DTU slot**
    - slot ----------------- [ string ]
    - failed return tfalse
    - succeed return ttrue

+ `key[]` **list TLS material paths per client/server slot**
    - return JSON with nested ca/crt/key paths where files exist

---

## uartdrv@dtu — DTU / MQTT / TCP / UDP Driver

`uartdrv@dtu` is the **executable** mapped from **`uartdrv@dtu`** in **`prj.json`** **`obj`**. It bridges one UART to multiple TCP/UDP clients and servers and MQTT clients using **libevent** and **libmosquitto**. The **`dtu`** subtree in **`uart@tty*`** configuration is consumed when this driver runs.

---

### Component API ( `uartdrv@dtu` )

+ `service[ uart_object ]` **run the DTU event loop**
    - uart_object ---------- [ string ], e.g. uart@tty, must match the saved configuration object name
    - opens serial, creates peers, listens on Unix datagram **`$(project_var)/$(project)/$(uart_object).unix`**
    - does not return while event_base_dispatch runs

    Example, start is normally done by uart@tty setup (illustrative)
    ```shell
    uartdrv@dtu.service[ uart@tty ]
    ```

+ `status[ uart_object ]` **query link statistics**
    - uart_object ---------- [ string ]
    - failed return NULL (process not running)
    - return JSON per peer rx, tx, connect, optional ip

    Example, query DTU status when process is running
    ```shell
    uartdrv@dtu.status[ uart@tty ]
    {
        "tty":{ "rx":"100", "tx":"200", "connect":"ok" },
        "client":{ "rx":"50", "tx":"50", "connect":"ok", "ip":"192.168.8.250" }
    }
    ```

+ `reset[ uart_object, { "ifname": "…" } ]` **reconnect selected TCP/UDP/MQTT clients**
    - uart_object ---------- [ string ]
    - JSON ----------------- must contain ifname string; compared to each dtu.*.extern
    - for each subsection with non-empty extern, if extern is default or equals ifname, and key is client or mqtt (not server*), close and reopen that client
    - failed return tfalse when process down, no matches, or IPC error; otherwise dtu_call result

    Example, reset DTU clients tied to default or matching extern after WAN up
    ```shell
    uartdrv@dtu.reset[ uart@tty, { "ifname":"ifname@wan" } ]
    ttrue
    ```

**Internal control socket** (datagram JSON to the same **`*.unix`** path as service):

```json
{ "cmd": "status" }
```

```json
{ "cmd": "reset", "v": { "client": "reset", "mqtt2": "reset" } }
```

Keys in **v** are **dtu** subsection names; values are ignored but keys must exist. Only TCP client, UDP client, and MQTT client peers are restarted.

**Build option:** define **`DTU_DNS_EVDNS=1`** and link **libevent_extra** for **evdns** hostname resolution on DTU clients and MQTT; default **0** uses **domain2ip** / register (**`dtu.h`**).

### Lifecycle ( `uartdrv@dtu` )

`uartdrv@dtu` has **no** separate **`setup`** / **`shut`** exports on the executable table: the process lifetime is the **`service`** call. Stopping the service is done by the platform (**signal**, **`land@service`**, etc.). **`uart@tty.shut`** tears down the port from the **`uart@frame`** side.

---

## uartdrv@tui — Eline Serial Driver

+ `service[ uart_object ]` **attach serial to eline**
    - uart_object ---------- [ string ]
    - opens ttydev with termios, dup2 to stdin/stdout, execlp("eline", …)
    - on success does not return

---

### Lifecycle API ( `uart@tty` )

+ `setup[]` / `shut[]` — invoked from **`uart@frame`** or manually; see **`frame.md`** and **`uart_dev`** **`_set`**.

---

### C Code Example

**Call component methods**

```c
#include "skin/skin.h"

static void print_uart_call_error(const char *api, talk_t ret)
{
	if (ret == tfalse || ret == terror || ret == tpanic)
	{
		printf("%s failed, errno=%d\n", api, errno);
	}
}
```

##### `ttydev[]`

```c
talk_t ret = scall("uart@tty", "ttydev", NULL);
if (ret > tpanic)
{
	printf("%s\n", x2string(ret));
	talk_free(ret);
}
else
	print_uart_call_error("ttydev", ret);
```

##### `clear_ca[ slot ]`

```c
talk_t ret = scalls("uart@tty", "clear_ca", "client");
if (ret != ttrue)
	print_uart_call_error("clear_ca", ret);
```

##### `uartdrv@dtu.status[ uart_object ]`

```c
talk_t ret = scalls("uartdrv@dtu", "status", "uart@tty");
if (ret > tpanic)
{
	/* inspect JSON */
	talk_free(ret);
}
else
	print_uart_call_error("status", ret);
```

##### `uartdrv@dtu.reset[ uart_object, json ]`

```c
talk_t ret = scalls("uartdrv@dtu", "reset", "uart@tty,{\"ifname\":\"ifname@wan\"}");
if (ret > tpanic)
	talk_free(ret);
else if (ret != ttrue)
	print_uart_call_error("reset", ret);
```
