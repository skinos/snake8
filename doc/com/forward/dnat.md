## forward@dnat — Management of destination NAT
Management of destination NAT to proxy internet port on gateway for local client access

### Configuration ( `forward@dnat` )
```json
// Attributes introduction 
{

    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], proxy rules for this LAN ifname
    {
        "rule name":                    // [ string ], user can custom the rule name
        {
            "destip":"internet server ip address",    // [ ip address ]
            "destport":"internet server port",        // [ number ]
                                                                // signal port: 80
                                                                // range of port: 80-100
                                                                // multiple port: 80,8080,8000
            "protocol":"protocol type",               // [ "tcp", "udp", "tcpudp" ], "tcpudp" for TCP and UDP
            "targetport":"local proxy port"           // [ number ]
        },
        // ... more rule
    }
    // ... more ifname
}
```

Example, show current all of dnat rule
```shell
forward@dnat
{
    "ifname@lan":                       // for the LAN
    {
        "proxy1":                          // rule name is proxy1
        {
            "destip":"29.23.11.35",        // internet server ip is 29.23.11.35
            "destport":"28-90",            // internet server port is start 28, end at 90 
            "protocol":"tcpudp",           // proxy tcp and udp
            "targetport":"100"             // proxy the server on port 100 on this LAN ifname
        }
    },
    "ifname@lan2":                      // for the LAN2
    {
        "forweb":                          // rule name is forweb
        {
            "destip":"129.232.91.5",       // internet server ip is 129.232.91.5
            "destport":"80",               // internet server port is 80
            "protocol":"tcp",              // proxy tcp
            "targetport":"8000"            // proxy the server on port 8000 on this LAN ifname
        }
    }    
}
```

Example, add a rule named proxy2 to ifname@lan
```shell
forward@dnat:ifname@lan/proxy2={"destip":"202.96.134.144","destport":"53","protocol":"tcpudp","targetport":"500"}
ttrue
```   

Example, modify destport of rule named proxy2 to 55
```shell
forward@dnat:ifname@lan/proxy2/destport=55
ttrue
```   

Example, delete a rule named proxy2 from ifname@lan
```shell
forward@dnat:ifname@lan/proxy2=
ttrue
```   

Examples, change several attributes at once (**merge**)
```shell
forward@dnat|{"ifname@lan":{"proxy1":{"destip":"1.2.3.4","destport":"80","protocol":"tcp","targetport":"8080"}}}
ttrue
```

### Component API
Use standard **`forward@dnat`** get/set/merge for configuration (see above).

+ `on[]` **refresh destination-NAT proxy rules for a LAN ifname**, *succeed return ttrue*
    - Parameter **2** carries **`ifname`**; rules for that **ifname** are rebuilt from saved configuration (skips in **default** / **parasite** network modes).

+ `off[]` **tear down DNAT rules for an ifname**

### Lifecycle API
+ `setup[]` / `shut[]` — **not** wired in the default **init** / **uninit** schedule for this component; refresh via **`on[]`** / **`off[]`**.

### Joint Handlers
| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@dnat.on` |


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_dnat(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@dnat", "status") == NULL)
        return -1;
    return ssets_string("forward@dnat", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@dnat", "list", NULL); talk_free if JSON */
```
