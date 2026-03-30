## agent@portc — Port client — remote port proxy
Connects to the port-proxy service so remote sessions can reach local **TCP**, **UDP**, or **serial** targets. Keeps standby links to the server so new proxy requests can be served quickly.

### Configuration ( `agent@portc` )
```json
{
    // server connection
    "server":"port proxy server address",                                  // [ string ], domain name or ip address
                                                                              // if not set, use heclient's server
    "port":"port proxy server port",                                       // [ number ], default 20005
    "user":"username for registration",                                    // [ string ], if not set, use heclient's user
    "vcode":"verification code",                                           // [ string ], if not set, use heclient's vcode

    // bindding extern network
    "extern":"bindding extern ifname to connect server",                   // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], optional
                                                                              // when not set, use default gateway to connect server

    // connection pool
    "pond":"number of idle connections in the pool",                       // [ number ], default 3

    // timeout control
    "connect_timeout":"connection timeout in seconds",                     // [ number ], default 15
    "idle_keeplive_interval":"keeplive interval for idle connections",      // [ number ], default 8, the unit is second
    "idle_keeplive_timeout":"timeout for idle connections",                 // [ number ], default 30, the unit is second
    "use_keeplive_timeout":"timeout for active proxy connections"           // [ number ], default 360, the unit is second
}
```

Example, show all the configure
```shell
agent@portc
{
    "port":"20005",                           # server port
    "pond":"3"                                # 3 idle connections in pool
}
```

Example, set the port proxy server and port
```shell
agent@portc={"server":"proxy.ashyelf.com","port":"20005","user":"ashyelf"}
ttrue
```

Example, modify the connection pool size to 5
```shell
agent@portc:pond=5
ttrue
```

Example, modify the idle keeplive interval to 15 seconds
```shell
agent@portc:idle_keeplive_interval=15
ttrue
```

Example, set the extern network interface
```shell
agent@portc:extern=ifname@lte
ttrue
```


### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `setup[]` **setup the port client, start the connection service**
    start the background service process to connect to the port proxy server
    - succeed return ttrue
    - failed return tfalse

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
        "status":"current status",             // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                                  // "uping" for connecting to server
                                                  // "down" for service is not running
                                                  // "online" for connected to server successfully
                                                  // "usererror" for username is wrong or not exist
                                                  // "vcodeerror" for verification code is wrong
        "server":"resolved server ip"           // [ ip address ], only available when status is "uping" or "online"
    }
    ```

    Example, get status when connected
    ```shell
    agent@portc.status
    {
        "status":"online",                        # connected to server
        "server":"114.132.219.158"                # resolved server ip
    }
    ```

    Example, get status when connecting
    ```shell
    agent@portc.status
    {
        "status":"uping",                         # connecting to server
        "server":"114.132.219.158"
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@portc.status
    {
        "status":"down"                           # service is not running
    }
    ```

    Example, get status when username is wrong
    ```shell
    agent@portc.status
    {
        "status":"usererror"                      # wrong username
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

+ `service[]` **internal (not called via HE)**
    Background worker started by **`setup[]`**: uses **`agent@heclient`** (or local fields) for server identity when unset, keeps a pool of **`pond`** idle server links, renews them as proxies attach/detach, and applies the timeout fields from configuration. **Auth errors** stop without auto-restart; **network / socket** issues and **extern not ready** are retried.

### Lifecycle API
+ `setup[]` / `shut[]` — **when implemented** for **`agent@portc`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_agent_portc(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@portc", "status") == NULL)
        return -1;
    return ssets_string("agent@portc", "enable", "status") ? 0 : -1;
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

/* e.g. scall("agent@portc", "list", NULL); talk_free if JSON */
```
