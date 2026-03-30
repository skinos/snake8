## forward@firewall — Management of System Firewall
Management of firewall for limit access from external **ifname** (internet)

### Configuration ( `forward@firewall` )
```json
// Attributes introduction 
{
    "ifname@…":                  // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], firewall for this external ifname
    {
        "status":"disable or enable the firewall",          // [ "enable", "disable" ]
        "default":"action for default access",              // [ "drop", "accept" ]

        "icmp_access":"ICMP protocol access",        // [ "disable", "enable" ]
        "espah_access":"ESP/AH protocol access",     // [ "disable", "enable" ]
        "telnet_access":"TELNET Server access",      // [ "disable", "enable" ]
        "ssh_access":"SSH Server access",            // [ "disable", "enable" ]
        "wui_access":"WEB Server access",            // [ "disable", "enable" ]

        "nat_through":"NAT rule settings at forward@nat  passthrough auto",    // [ "disable", "enable" ]
        "icmp_through":"ICMP protocol passthrough",                            // [ "disable", "enable" ]
        "espah_through":"ESP/AH protocol passthrough",                         // [ "disable", "enable" ]

        "rule":                              // firewall rule settings
        {
            "rule name":                                   // [ string ], user can custom the rule name
            {
                "action":"drop or accept or return",                     // [ "drop", "accept", "return" ], "drop" for forbid, "accept" for pass, "return" for don't match it with after rule
                "src":"source address",                                  // [ string ]:
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // single MAC: 00:23:43:13:34:40
                                                                                  // space for all ip address
                "srcport":"source port",                                 // [ number ]: valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space for all port

                "protocol":"protocol type",                              // [ "tcp", "udp", "all", ... ]
                "dest":"destination address",                            // [ string ]:
                                                                                  // single IP: 202.96.11.32
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1,72.32,192.1
                                                                                  // range of IP: 202.96.132.11-202.96.132.20
                                                                                  // space for all ip address

                "destport":"destination port"                           // [ number ]: valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space for all port
            }
            // ... more rule
        }
    }
    // ... more ifname
}
```  

Example, show current all of firewall settings
```shell
forward@firewall
{
    "ifname@lte":                 // first LTE network firewall settings
    {
        "status":"enable",             // enable the firewall
        "default":"drop",              // default action is drop
        "rule":
        {
            "pcweb":                        // rule named "pcweb", accept dest 192.168.8.222 and destport TCP 80 access
            {
                "action":"accept",
                "dest":"192.168.8.222",
                "protocol":"tcp",
                "destport":"80"
            }
        }
    },
    "ifname@lte2":               // second NR/LTE network firewall settings
    {
        "status":"disable",           // disable the firewall
        "default":"drop",
        "rule":
        {
        }
    }
}
```

Example, add the rule named webwork for ifname@lte, that make 113.23.64.28 can access the gateway web port 80 from ifname@lte
```shell
forward@firewall:ifname@lte/rule/webwork={"action":"accept","src":"113.23.64.28","protocol":"tcp","destport":"80"}
ttrue
```

Example, add the rule named webpass for ifname@lte, that make 113.23.64.28 can access the web port 80 of 192.168.8.250 from ifname@lte
```shell
forward@firewall:ifname@lte/rule/webpass={"action":"accept","src":"113.23.64.28","protocol":"tcp","dest":"192.168.8.250","destport":"80"}
ttrue
```

Example, enable the first LTE network firewall
```shell
forward@firewall:ifname@lte/status=enable
ttrue
```

Example, modify default action from the first LTE network to drop
```shell
forward@firewall:ifname@lte/default=drop
ttrue
```

Example, delete the rule named webwork for ifname@lte
```shell
forward@firewall:ifname@lte/rule/webwork=
ttrue
```

Example, delete the rule named webpass for ifname@lte
```shell
forward@firewall:ifname@lte/rule/webpass=
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
forward@firewall|{"ifname@lte":{"status":"enable","default":"drop"}}
ttrue
```

### Component API
Use standard **`forward@firewall`** get/set/merge for configuration.

+ `on[]` **refresh inbound firewall rules for an external ifname**, *succeed return ttrue*
    - Parameter **2** carries **`ifname`**; the firewall for that **ifname** is rebuilt from saved configuration (skips in **default** / **parasite** network modes).

+ `off[]` **tear down firewall for an ifname**

### Lifecycle API
+ `setup[]` / `shut[]` — **not** wired in the default **init** / **uninit** schedule for this component; use **`on[]`** / **`off[]`**.

### Joint Handlers
| Joint key | Method |
|-----------|--------|
| `network/onextern` | `forward@firewall.on` |
| `network/onvpn` | `forward@firewall.on` |


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_firewall(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@firewall", "status") == NULL)
        return -1;
    return ssets_string("forward@firewall", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@firewall", "list", NULL); talk_free if JSON */
```
