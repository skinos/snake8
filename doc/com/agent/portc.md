## agent@portc — Port client — remote port proxy
Connects to the port-proxy service so remote sessions can reach local **TCP**, **UDP**, or **serial** targets. Keeps standby links to the server so new proxy requests can be served quickly.

Standby keeplive: client sends `k`, server echoes `k`. Via heport `adjust`: `mode`, `active_pond`, `idle_pond`, `nomate_timeout` (ping interval `(nomate_timeout-1)/3`), `connect_timeout = mating_timeout - 1`, `mate_timeout` (same as center@pport).

**`mode`**: `pond` (default, current) or `mux` (future multiplex entry). Unset/`pond` runs as today; `mux` or unknown value → service fails and exits (not implemented). No capability negotiation — center hard-switches when ready.

**UDP payload framing** (mated `hand_proto` `u`, same as `center@pport`): after the mate line / `m` ACK, datagrams on the proxy TCP use `[u16be length][payload]` (`length` 0..65535). TCP/serial hands stay raw byte streams. Must match center version.

### Configuration ( `agent@portc` )
```json
{
    "status":"port proxy client service",                                  // [ "disable","enable" ], service master switch
                                                                              // "disable": background service not started
                                                                              // "enable": connect pool when setup runs
                                                                              // normally set by heport from center@pport (enable/disable)

    // server connection
    "server":"port proxy server address",                                  // [ string ], domain name or ip address
                                                                              // if set: resolve DNS (same as heclient) and show IP in status
                                                                              // if not set: use heclient's resolved server; status omits server
    "port":"port proxy server port",                                       // [ number ], default 20005
    "user":"username for registration",                                    // [ string ], if not set, use heclient's user
    "vcode":"verification code",                                           // [ string ], if not set, use heclient's vcode

    // transport / connection pool
    "mode":"proxy transport mode",                                         // [ "pond", "mux" ], default "pond" (from heport)
                                                                              // "pond": one proxy TCP per session (current)
                                                                              // "mux": reserved; service exits until implemented
    "active_pond":"standby pool size while proxy sessions are active", // [ number ], default 3
                                                                              // while any mate/mating/drain link exists: keep this many idle standbys
                                                                              // <=0 or unset: use 3
    "idle_pond":"standby pool size when fully idle",                   // [ number ], default 1
                                                                              // when no mate/mating/drain: shrink/grow to this many standbys
                                                                              // shrink runs after ~6 idle create checks; <=0 or unset: use 1

    // timeout control
    "connect_timeout":"connect / TCP hand connect timeout",                // [ number ], default 14, minimum 1, the unit is second
                                                                              // normally set by heport as center@pport mating_timeout - 1
    "nomate_timeout":"idle timeout for standby links",                     // [ number ], default 46, minimum 10, the unit is second
                                                                              // normally set by heport from center@pport nomate_timeout
                                                                              // ping interval = (nomate_timeout-1)/3
    "mate_timeout":"idle timeout for mated proxy connections",             // [ number ], default 180, minimum 1, the unit is second
                                                                              // normally set by heport as center@pport mate_timeout
}
```

Example, show all the configure
```shell
agent@portc
{
    "status":"enable",                        # service enabled (from heport when center@pport is on)
    "port":"20005",                           # server port
    "mode":"pond",                            # pond now; mux reserved
    "active_pond":"3",                        # keep 3 standbys while sessions active
    "idle_pond":"1",                          # keep 1 standby when fully idle
    "nomate_timeout":"46"                     # from heport / local default
}
```

Example, set the port proxy server and port
```shell
agent@portc={"server":"proxy.ashyelf.com","port":"20005","user":"ashyelf"}
ttrue
```

Example, keep 3 standbys when busy and 2 when idle
```shell
agent@portc:active_pond=3
ttrue
agent@portc:idle_pond=2
ttrue
```

### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `setup[]` **setup the port client, start the connection service when status is enable**
    start the background service process to connect to the port proxy server
    - succeed return ttrue
    - failed return tfalse
    - When **`status`** is not **`enable`**, return ttrue without starting the service

    Example, setup the port client
    ```shell
    agent@portc.setup
    ttrue
    ```

+ `shut[]` **shutdown the port client**
    stop the background service process
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the port client
    ```shell
    agent@portc.shut
    ttrue
    ```

+ `status[]` **get the port client current status**
    - failed return NULL
    - succeed return json to describes status information
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status",             // [ "uping", "down", "online" ]
                                                  // "uping" for connecting to server
                                                  // "down" for service is not running
                                                  // "online" for connected to server successfully
        "server":"resolved server ip"           // [ ip address ], only when configure server is set and status is uping/online
                                                   // omitted when server is inherited from heclient
    }
    ```

    Example, get status when connected with local server configure
    ```shell
    agent@portc.status
    {
        "status":"online",                        # connected to server
        "server":"114.132.219.158"                # resolved from configure server
    }
    ```

    Example, get status when connecting with local server configure
    ```shell
    agent@portc.status
    {
        "status":"uping",                         # connecting to server
        "server":"114.132.219.158"
    }
    ```

    Example, get status when server is inherited from heclient
    ```shell
    agent@portc.status
    {
        "status":"online"                         # no server field
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@portc.status
    {
        "status":"down"                           # service is not running
    }
    ```

+ `list[]` **get all proxy connections information**
    - failed return NULL
    - succeed return json with all connection details
    ```json
    // Attributes introduction of talk by the API return
    {
        "file descriptor number":                               // [ number ], the fd of the server connection
        {
            "local_ip":"local ip used to connect server",       // [ ip address ]
            "local_port":"local port used to connect server",   // [ number ]
            "tx":"bytes sent to server",                        // [ number ]
            "rx":"bytes received from server",                  // [ number ]
            "last":"last receive time (uptime mark)",           // [ number ]
            "port":"server side port number",                   // [ number ], only when mated
            "hand_ip":"proxy target ip address",                // [ ip address ], only when mated
            "hand_port":"proxy target port or serial device",   // [ string ], port number or "uart@serial", only when mated
            "hand_proto":"proxy protocol type",                 // [ "t", "u", "d" ], t=tcp, u=udp, d=serial device, only when mated
            "hand_tx":"bytes sent to proxy target",             // [ number ], only when mated
            "hand_rx":"bytes received from proxy target"        // [ number ], only when mated
        }
        // ... more connections
    }
    ```

    Example, list all connections with one idle and one active proxy
    ```shell
    agent@portc.list
    {
        "5":                                                    # fd 5, idle connection
        {
            "local_ip":"192.168.8.1",
            "local_port":"45678",
            "tx":"128",
            "rx":"64",
            "last":"12345"
        },
        "7":                                                    # fd 7, active proxy to local web
        {
            "local_ip":"192.168.8.1",
            "local_port":"45680",
            "tx":"4096",
            "rx":"8192",
            "last":"12350",
            "port":"80",
            "hand_ip":"192.168.8.1",
            "hand_port":"80",
            "hand_proto":"t",
            "hand_tx":"2048",
            "hand_rx":"4096"
        }
    }
    ```

