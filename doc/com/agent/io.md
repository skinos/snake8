## IO Agent -- GPIO and Network IO Management
Manage device GPIO input/output, support IO state monitoring, trigger actions on state changes, and report IO state to remote servers via TCP/UDP/MQTT clients and servers

### **Configuration( `agent@io` )**

```json
{
    "status":"io agent service status",                    // [ "disable", "enable" ]

    // GPIO initialization map
    "init":                                                // define initial state for each GPIO
    {
        "g1":"gpio 1 initial state",                       // [ "00", "01", "02", "0f", "0r", "0b", "10", "11", "2N-ON-OFF" ]
                                                              // "00" or "0f": input falling edge trigger
                                                              // "01" or "0r": input rising edge trigger
                                                              // "02" or "0b": input both edge trigger
                                                              // "10": output low
                                                              // "11": output high
                                                              // "2N-ON-OFF": output timer, N is mode, ON is high duration(ms), OFF is low duration(ms)
        "g2":"gpio 2 initial state"
        // ... more GPIOs (up to g20)
    },

    // GPIO trigger actions
    "trigger":                                             // define actions when GPIO state changes
    {
        "g1":                                              // trigger config for gpio 1
        {
            "rising":                                      // actions on rising edge (0->1)
            {
                "1":"command to execute",                  // string command format: "agent@io.modify[g2,11]"
                "2":{"obj":"agent@io","op":"call","m":"modify","p":"g3,10"}  // JSON command format
            },
            "falling":                                     // actions on falling edge (1->0)
            {
                "1":"command to execute"
            },
            "both":                                        // actions on any edge change
            {
                "1":"command to execute"
            }
        }
        // ... more GPIO triggers
    },

    // TCP/UDP client (up to 9: client, client2, client3, ... client9)
    "client":
    {
        "status":"client status",                          // [ "disable", "enable" ]
        "proto":"transport protocol",                      // [ "tcp", "udp" ]
        "server":"remote server address",                  // [ string ], domain name or ip address
        "port":"remote server port",                       // [ number ]
        "interval":"reconnect interval in seconds",        // [ number ], default 10
        "id":"device identify for registration",           // [ string ], optional
        "user":"username for registration",                // [ string ], optional
        "vcode":"verification code for registration"       // [ string ], optional
    },

    // MQTT client (up to 9: mqtt, mqtt2, mqtt3, ... mqtt9)
    "mqtt":
    {
        "status":"mqtt client status",                     // [ "disable", "enable" ]
        "server":"MQTT broker address",                    // [ string ], domain name or ip address
        "port":"MQTT broker port",                         // [ number ], required; typical value 1883 (not defaulted if omitted)
        "mqtt_id":"MQTT client id",                        // [ string ], default is device macid
                                                              // set to "NULL" for random id with clean session
        "mqtt_username":"MQTT username",                   // [ string ], optional
        "mqtt_password":"MQTT password",                   // [ string ], optional
        "mqtt_keepalive":"MQTT keepalive in seconds",      // [ number ], default 60
        "mqtt_interval":"reconnect interval in seconds",   // [ number ], default 10
        "mqtt_publish":"MQTT publish topic",               // [ string ], topic for publishing IO state
        "mqtt_publish_qos":"MQTT publish QoS level",       // [ number ], 0, 1 or 2, default 0
        "mqtt_subscribe":                                  // MQTT subscribe topics
        {
            "topic/name":"qos level"                       // [ "0", "1", "2" ]
        }
    },

    // TCP/UDP server (up to 9: server, server2, server3, ... server9)
    "server":
    {
        "status":"server status",                          // [ "disable", "enable" ]
        "proto":"transport protocol",                      // [ "tcp", "udp" ]
        "port":"listen port",                              // [ number ]
        "timeout":"client connection timeout in seconds",  // [ number ], 0 for no timeout
        "interval":"retry interval in seconds",            // [ number ], default 10
        "limit":"max TCP connections (tcp server only)",   // [ number ]
        "id":"device identify",                            // [ string ], optional
        "user":"username",                                 // [ string ], optional
        "vcode":"verification code"                        // [ string ], optional
    }
}
```

Example, show all the configure
```shell
agent@io
{
    "status":"enable",
    "init":
    {
        "g1":"0b",                                         # gpio1 input both edge
        "g2":"11",                                         # gpio2 output high
        "g3":"10"                                          # gpio3 output low
    },
    "trigger":
    {
        "g1":
        {
            "rising":
            {
                "1":"agent@io.modify[g2,11]"               # when g1 rising, set g2 high
            },
            "falling":
            {
                "1":"agent@io.modify[g2,10]"               # when g1 falling, set g2 low
            }
        }
    },
    "client":
    {
        "status":"enable",
        "proto":"tcp",
        "server":"192.168.8.100",
        "port":"8899"
    },
    "mqtt":
    {
        "status":"enable",
        "server":"mqtt.example.com",
        "port":"1883",
        "mqtt_id":"mydevice001",
        "mqtt_username":"user1",
        "mqtt_password":"pass1",
        "mqtt_publish":"device/io/state",
        "mqtt_publish_qos":"1",
        "mqtt_subscribe":
        {
            "device/io/control":"1"
        }
    }
}
```

Example, enable io agent with gpio1 as input and gpio2 as output
```shell
agent@io={"status":"enable","init":{"g1":"0b","g2":"10"}}
ttrue
```

Example, configure a TCP client to report IO state
```shell
agent@io:client={"status":"enable","proto":"tcp","server":"192.168.8.100","port":"8899"}
ttrue
```

Example, configure MQTT client
```shell
agent@io:mqtt={"status":"enable","server":"mqtt.example.com","port":"1883","mqtt_publish":"device/io/state","mqtt_subscribe":{"device/io/cmd":"1"}}
ttrue
```



### **Component API**

**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `setup[]` **setup the io agent, start the service**
    setup will read the configuration, check if init map has any GPIO defined and status is not "disable", then start the background service process
    - succeed return ttrue
    - failed return tfalse

    Example, setup the io agent
    ```shell
    agent@io.setup
    ttrue
    ```

+ `shut[]` **shutdown the io agent service**
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the io agent
    ```shell
    agent@io.shut
    ttrue
    ```

+ `status[]` **get all GPIO output and input state**
    - failed return NULL, the service is not running
    - succeed return json with current GPIO states
    ```json
    // Attributes introduction of talk by the API return
    {
        "g1":"IO state code",                // state code format: first digit is mode, second is state
                                                // "00": input low
                                                // "01": input high
                                                // "10": output low
                                                // "11": output high
                                                // "12": output timer mode
        "g2":"IO state code"
        // ... more GPIOs
    }
    ```

    Example, get all GPIO status
    ```shell
    agent@io.status
    {
        "g1":"01",                                # gpio1 input, current state high
        "g2":"11",                                # gpio2 output high
        "g3":"10"                                 # gpio3 output low
    }
    ```

+ `current[]` **get current IO state as a string line**
    - failed return NULL, the service is not running
    - succeed return string with IO state line in format "gN=STATE;"

    Example, get current IO state line
    ```shell
    agent@io.current
    g1=01;g2=11;g3=10;
    ```

+ `dump[]` **get all GPIO state including cached input state**
    similar to status but uses cached input state instead of reading from hardware
    - failed return NULL, the service is not running
    - succeed return json with GPIO states
    ```json
    {
        "g1":"IO state code",                // "00": input low, "01": input high, "0x": input unknown
                                                // "10": output low, "11": output high, "12": output timer
        "g2":"IO state code"
    }
    ```

    Example, dump all GPIO state
    ```shell
    agent@io.dump
    {
        "g1":"01",
        "g2":"11",
        "g3":"0x"                                 # gpio3 input state unknown
    }
    ```

+ `list[]` **get network connection status for active peers (see notes below)**
    - failed return NULL, the service is not running
    - succeed return json; each key is a **connection name** from configuration (e.g. `client`, `client2`, `mqtt`, `server:peer-ip:peer-port` for UDP server)
    - **Not included:** TCP **listen** server units (`proto` TCP server) are skipped by implementation — only clients, MQTT clients, and UDP server child sockets appear
    - **UDP server:** entries use a composite key: `<server-name>:<peer-ip>:<peer-port>` instead of the bare server name
    ```json
    // Attributes introduction of talk by the API return
    {
        "connection name":
        {
            "rx":"total bytes received",           // [ number ]
            "tx":"total bytes sent",               // [ number ]
            "ip":"peer ip address",                // [ ip address ], only for client connections
            "connect":"connection state"            // [ "ok" ], present when connected
        }
        // ... more connections
    }
    ```

    Example, list all connections
    ```shell
    agent@io.list
    {
        "client":
        {
            "rx":"1024",
            "tx":"512",
            "ip":"192.168.8.100",
            "connect":"ok"
        },
        "mqtt":
        {
            "rx":"2048",
            "tx":"1024",
            "connect":"ok"
        }
    }
    ```

+ `modify[ io name, value ]` **modify a GPIO state at runtime**
    - io name ---- [ string ], GPIO name like "g1", "g2"
    - value ------ [ string ], target state code
        - "0b" or "02": switch to input both edge
        - "0f" or "00": switch to input falling edge
        - "0r" or "01": switch to input rising edge
        - "10": switch to output low
        - "11": switch to output high
        - "2N-ON-OFF": switch to output timer, ON=high duration(ms), OFF=low duration(ms)
    - succeed return ttrue
    - failed return tfalse

    Example, set gpio2 to output high
    ```shell
    agent@io.modify[g2,11]
    ttrue
    ```

    Example, set gpio1 to input both edge
    ```shell
    agent@io.modify[g1,0b]
    ttrue
    ```

    Example, set gpio3 to timer mode (500ms on, 500ms off)
    ```shell
    agent@io.modify[g3,2-500-500]
    ttrue
    ```

+ `report[]` **trigger an immediate IO state report to all connections**
    send SIGUSR1 signal to the service to report current IO state to all connected TCP/UDP/MQTT peers
    - succeed return ttrue, signal sent
    - failed return tfalse, service is not running

    Example, trigger immediate report
    ```shell
    agent@io.report
    ttrue
    ```

+ `service[]` **internal background service (not called directly)**
    this is the main event-driven service started by setup, it handles:
    1. Initialize GPIO according to init map (set input/output mode)
    2. Initialize libevent and mosquitto library
    3. Register signal handlers (SIGINT/SIGTERM for exit, SIGWINCH for IO change, SIGUSR1 for report)
    4. Create UNIX domain socket control interface for inter-process communication
    5. Create MQTT clients (mqtt, mqtt2, ... mqtt9) from configuration
    6. Create TCP/UDP clients (client, client2, ... client9) from configuration
    7. Create TCP/UDP servers (server, server2, ... server9) from configuration
    8. Run event loop to handle all IO events

    **IO state report protocol (TCP/UDP):**
    - Format: `gN=STATE;gN=STATE;...`
    - Example: `g1=01;g2=11;g3=10;`

    **IO remote control protocol (TCP/UDP):**
    - Format: `gN=STATE;gN=STATE;...` (same as report, must end with semicolon)
    - Example: `g1=10;g2=11;` (set g1 output low, g2 output high)

    **TCP/UDP client registration packet:**
    - Format: `macid=<macid>;id=<id>;user=<user>;vcode=<vcode>;`

    **MQTT client:**
    - Publish IO state to configured topic
    - Subscribe to configured topics for remote control
    - Support TLS with CA/cert/key files at: `<config_path>/io-<name>.ca`, `io-<name>.crt`, `io-<name>.key`

### **Lifecycle API**

+ `setup[]` / `shut[]` — **when implemented** for **`agent@io`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.
+



### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_agent_io(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@io", "status") == NULL)
        return -1;
    return ssets_string("agent@io", "enable", "status") ? 0 : -1;
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

/* e.g. scall("agent@io", "list", NULL); talk_free if JSON */
```

