## forward@main — Default Route Table Management
Management of system route table, modifying this configuration directly is not recommended, It is recommended to manage through the method

### Configuration ( `forward@main` )
```json
// Attributes introduction 
{
    "rule name":        // [ string ], user can custom the rule name
    {
        "target":"select the packet use source ip address",            // [ ip address, network ]
        "mask":"select the packet use source mask of ip address",      // [ netmask ], necessary when "target" be network
        "gw":"gateway ip address",                                     // [ ip address ]
        "metric":"route hop",                                          // [ number ]
        "ifname":"select the packet output ifname",                    // [ "ifname@lan", "ifname@lan2", "ifname@wan", "ifname@lte", ... ], logical ifname
    }
    // ... more rule
}
```

Example, show current all rule settings of route of default
```shell
forward@main
{
    "myCustomRule1":    # first rule name is "myCustomRule1"
    {                                # make dest 192.168.1.0/255.255.255.0 to ifname@wan's 192.168.8.22, mark the metric be 2
        "target":"192.168.1.0",
        "mask":"255.255.255.0",
        "gw":"192.168.8.22",
        "metric":"2",
        "ifname":"ifname@wan"
    }
    "youCustomRule":    # second rule name is "youCustomRule"
    {                               # make all access to ifname@lan's 192.168.9.22
        "gw":"192.168.9.22",
        "ifname":"ifname@lan"
    }
}
```  


### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `status[]` **get the current default route table**
    - failed return NULL, error return terror
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of json by the method return
    {
        "rule name":        // [ string ], user custom the rule name, the system rule start with "~"
        {
            "target":"select the packet use source ip address",            // [ ip address, network ]
            "mask":"select the packet use source mask of ip address",      // [ netmask ], necessary when "target" be network
            "gw":"gateway ip address",                                     // [ ip address ]
            "ifname":"select the packet output ifname",                    // [ "ifname@lan", "ifname@lan2", ... ], logical ifname
            "netdev":"network device",                                     // [ string ]
            "flags":"route flags",                                         // [ number ]
            "metric":"route hop",                                          // [ number ]
            "ref":"reference count",                                       // [ number ]
            "use":"use count",                                             // [ number ]
            "status":"rule state"                                          // [ "up", "down" ], "up" for enable, "down" for disable
        }
        // ... more rule
    }
    ```

    Example, get the current route rule
    ```shell
    forward@main.status
    {
        "myCustomRoute1":             // this is user add rule named "myCustomRoute1"
        {
            "target":"192.168.0.0",
            "mask":"255.255.255.0",
            "gw":"192.168.8.2",
            "ifname":"ifname@wan",
            "netdev":"eth0.2",
            "flags":1,
            "metric":2,
            "ref":0,
            "use":0,
            "status":"up"
        },
        "~auto1":                    // this is system rule
        {
            "target":"127.0.0.1",
            "mask":"255.255.255.0",
            "gw":"0.0.0.0",
            "netdev":"lo",
            "flags":1,
            "metric":0,
            "ref":0,
            "use":0,
            "status":"up"
        }
    }
    ```   

+ `add[ name, [target], [mask], [gateway], [ifname], [metric] ]` **add route rule**
    - name ----------- [ string ], route rule name  
    - target --------- [ network, ip address ]
    - mask ----------- [ network mask ]
    - gateway -------- [ ip address ]
    - ifname --------- [ "ifname@lan", "ifname@lte", ... ] you can get that list by call the network@frame.list
    - metric --------- [ number ]
    - succeed return ttrue
    - failed return tfalse

    Example, add a rule named office1, make that address 192.168.2.12 route to  192.168.9.40 of LAN
    ```shell
    forward@main.add[ office1, 192.168.2.12, 255.255.255.0, 192.168.9.40, ifname@lan ]
    ttrue
    ```   

    Example, add a rule named office2, make that all ddress route to 192.168.9.41 of LAN
    ```shell
    forward@main.add[ office2, , , 192.168.9.41, ifname@lan ]
    ttrue
    ```   

+ `delete[ name ]` **delete route rule**
    - name ----------- [ string ], route rule name  
    - succeed return ttrue
    - failed return tfalse
    
    Example, delete the custom route named office2
    ```shell
    forward@main.delete[ office2 ]
    ttrue
    ```

    Example, delete the custom route named office1
    ```shell
    forward@main.delete[ office1 ]
    ttrue
    ```

### Lifecycle API
+ `setup[]` **apply saved static routes**, *succeed return ttrue* — normally scheduled as **`init` → `app` → `forward@main.setup`** in the default forward package. May also be called manually.
+ `shut[]` — not listed in stock **`uninit`**; add per product.

### Joint Handlers
| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@main.on` |
| `network/onextern` | `forward@main.on` |
| `network/onvpn` | `forward@main.on` |

+ `on[]` **re-apply matching static routes after a network event**, *succeed return ttrue*
    - Parameter **2** is a JSON object; when **`ifname`** is present, only rules tied to that logical **ifname** are reconsidered and re-added if needed.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_main(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@main", "status") == NULL)
        return -1;
    return ssets_string("forward@main", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@main", "list", NULL); talk_free if JSON */
```
