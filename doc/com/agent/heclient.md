## agent@heclient — HE Client — Heport remote management client
Connects to the remote Heport service for administration: remote commands, status reporting, and coordinated control of related agent components (**`agent@portc`**, **`agent@gtog`**, etc.).

### Configuration ( `agent@heclient` )
```json
{
    // basic
    "status":"connect to heport server for remote management",   // [ "disable", "enable" ]
    "server":"heport server address",                            // [ string ], domain name or ip address
    "port":"heport server port",                                 // [ number ], default 20002
    "user":"username for bindding to the account",               // [ string ]
    "vcode":"verification code for the account",                 // [ string ], optional

    // bindding extern network
    "extern":"bindding extern ifname to connect server",             // call network@frame.list[extern] get the list
                                                                   // [ "disable", "default", "ifname@wan", ... ]
                                                                        // "disable" for no reset when ifname online
                                                                        // "default" for reset when the gateway online 
                                                                        // "ifname@wan", "ifname@lte", ... for reset when the ifname online


    // connection control
    "connect_timeout":"timeout for connection in seconds",       // [ number ], default 20
    "keeplive_interval":"keeplive interval in seconds",          // [ number ], default 10
    "keeplive_timeout":"timeout for keeplive in seconds"         // [ number ], default 35

    // heart beat
    "heart":
    {
        "he command":"update internal"                           // [ string ]:[ number ]
        // ... more the object
    }
}
```

Example, show all the configure
```shell
agent@heclient
{
    "status":"enable",                        # remote management is enabled
    "server":"cls.ashyelf.com",               # heport server address
    "port":"20002",                           # heport server port
    "user":"ashyelf",                         # bindding to account ashyelf
    "type":"router",                          # device type is router
    "connect_timeout":"20",                   # connection timeout 20 seconds
    "keeplive_interval":"10",                 # keeplive interval 10 seconds
    "keeplive_timeout":"35"                   # keeplive timeout 35 seconds
}
```

Example, enable the he client and bindding to account dimmalex@gmail.com
```shell
agent@heclient={"status":"enable","server":"devport.ashyelf.com","port":"20002","user":"dimmalex@gmail.com","vcode":"123456"}
ttrue
```

Example, disable the he client
```shell
agent@heclient:status=disable
ttrue
```

Example, modify the heport server to heport.ashyelf.com
```shell
agent@heclient:server=heport.ashyelf.com
ttrue
```

Example, modify the keeplive interval to 30 seconds
```shell
agent@heclient:keeplive_interval=30
ttrue
```

Example, set the extern network interface to ifname@lte
```shell
agent@heclient:extern=ifname@lte
ttrue
```


### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `setup[]` **setup the he client, start the connection service**
    setup will read the configuration, if status is "enable", start the background service process to connect to the heport server
    - succeed return ttrue
    - failed return tfalse

    Example, setup the he client
    ```shell
    agent@heclient.setup
    ttrue
    ```

+ `shut[]` **shutdown the he client and all managed components**
    Stops the background client and tears down managed **`agent@gtog`** / **`agent@portc`** state as configured.
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the he client
    ```shell
    agent@heclient.shut
    ttrue
    ```

+ `status[]` **get the he client current status information**
    - failed return NULL
    - succeed return json to describes status information
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status",        // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                             // "uping" for connecting to server
                                             // "down" for service is not running
                                             // "online" for connected to server successfully
                                             // "usererror" for username is wrong or not exist
                                             // "vcodeerror" for verification code is wrong
        "server":"resolved heport server ip" // [ ip address ], only available when status is "uping" or "online"
    }
    ```

    Example, get the he client status when connected
    ```shell
    agent@heclient.status
    {
        "status":"online",                    # connected to server successfully
        "server":"114.132.219.158"            # heport server resolved ip address
    }
    ```

    Example, get the status when connecting
    ```shell
    agent@heclient.status
    {
        "status":"uping",                     # connecting to server
        "server":"114.132.219.158"
    }
    ```

    Example, get the status when service is stopped
    ```shell
    agent@heclient.status
    {
        "status":"down"                       # service is not running
    }
    ```

    Example, get the status when username is wrong
    ```shell
    agent@heclient.status
    {
        "status":"usererror"                  # username wrong, the service will exit
    }
    ```

    Example, get the status when vcode is wrong
    ```shell
    agent@heclient.status
    {
        "status":"vcodeerror"                 # verification code wrong, the service will exit
    }
    ```

+ `update[]` **notify the service to re-send device information to server**
    Asks the running client to refresh and upload a device snapshot (machine, gateway, optional GNSS/sensors, etc.) to the server.
    - succeed return ttrue, the service process is running and signal sent
    - failed return tfalse, the service process is not running

    Example, update the device information to server
    ```shell
    agent@heclient.update
    ttrue
    ```

+ `adjust[ {adjust configuration} ]` **adjust the configuration and state of other components**
    adjust will modify the configuration of specified components and restart them accordingly. 
    If the new configuration is the same as existing, it will only start or stop the component based on its status field.
    If the configuration is different, it will shut down the component, apply new configuration, then start it
    - {adjust configuration} ------ json
    ```json
    // Attributes introduction of json pass to API
    {
        "component name":                           // [ string ], the full component object name (e.g. "agent@portc", "agent@gtog")
        {
            // the complete configuration for this component
        }
        // ...more other component configurations
    }
    ```
    - succeed return ttrue
    - failed return tfalse

    Example, adjust to enable the port client, disable the network client
    ```shell
    agent@heclient.adjust[{"agent@portc":{"status":"enable"},"agent@gtog":{"status":"disable"}}]
    ttrue
    ```

    Example, adjust to enable gtog with server configuration
    ```shell
    agent@heclient.adjust[{"agent@gtog":{"status":"enable","server":"192.168.1.1","port":"20000"}}]
    ttrue
    ```

+ `service[]` **internal (not called via HE)**
    Background worker started by **`setup[]`**: maintains the session to the Heport service, registers the device, keeps the link alive, executes remote **`object.method`** requests, and returns results. On **account / verification** errors the client stops without auto-restart; on **connect / timeout / extern not ready** it retries according to platform policy.

### Lifecycle API
+ `setup[]` / `shut[]` — **when implemented** for **`agent@heclient`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.

### Joint Handlers
| Joint key | Method |
|-----------|--------|
| `network/online` | `agent@heclient.setup` |
| `machine/status` | `agent@heclient.update` |


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_agent_heclient(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@heclient", "status") == NULL)
        return -1;
    return ssets_string("agent@heclient", "enable", "status") ? 0 : -1;
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

/* e.g. scall("agent@heclient", "list", NULL); talk_free if JSON */
```
