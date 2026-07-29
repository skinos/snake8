## agent@io — IO Agent -- GPIO and Network IO Management
Manage device GPIO input/output, support IO state monitoring, trigger actions on state changes, and report IO state to remote servers via TCP/UDP/MQTT clients and servers

### Configuration ( `agent@io` )
```json
{
    "status":"io agent service status",                    // [ "disable", "enable" ]
    "extern":"outbound interface before client/mqtt connect", // [ string ]: [ "disable","default","ifname@wan",... ]
                                                              // empty or omit: same as "default"
                                                              // "disable": no wait / no host route / no reset joint
                                                              // "default": wait default gateway, reset on network/online
                                                              // "ifname@wan", "ifname@lte", ...: bind that interface, reset on network/onextern when ifname matches

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
    "extern":"default",                                   # wait for default gateway before client/mqtt connect
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

Example, set the extern network interface to ifname@lte
```shell
agent@io:extern=ifname@lte
ttrue
```

### Component API
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
    - Unregisters runtime **`network/online`** and **`network/onextern`** reset handlers
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the io agent
    ```shell
    agent@io.shut
    ttrue
    ```

+ `reset[ event, event data ]` **restart the io background service when the bound extern changes**
    - event ----------------------- [ string ], joint event name (for example network/online)
    - event data ------------------ [ json ], event payload; must include **`ifname`**
    - Used as a joint handler registered by the service when **`extern`** is not **`disable`**
    - **`extern=default`**: acts only when event is **`network/online`**
    - **specific interface**: acts when event **`ifname`** equals configured **`extern`**
    - failed return tfalse
    - succeed return ttrue

    Example, reset when default gateway comes online (normally invoked by joint)
    ```shell
    agent@io.reset[ network/online, {"ifname":"ifname@wan"} ]
    ttrue
    ```

    Example, reset when bound interface ifname@lte comes online
    ```shell
    agent@io.reset[ network/onextern, {"ifname":"ifname@lte"} ]
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

### Lifecycle API
+ `setup[]` / `shut[]` — start/stop the component service. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.

When **`extern`** is not **`disable`**, the background service also registers at runtime:

| Event | Handler | When |
|-------|---------|------|
| `network/online` | `agent@io.reset` | **`extern=default`** (or empty) |
| `network/onextern` | `agent@io.reset` | **`extern`** is a specific interface name |

* Service waits for the selected gateway or interface IP before starting peers.
* Adds host routes to enabled **client** / **mqtt** server addresses via that path.
* If the interface is not ready yet, the service exits **`ttrue`** (no busy restart); joint **`reset`** restarts it when the path comes up.

