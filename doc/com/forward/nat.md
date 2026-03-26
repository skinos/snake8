## Management of System NAT/DMZ
Management of NAT to PORT map from external **ifname** to local network client

### **Configuration( `forward@nat` )**


```json
// Attributes introduction 
{
    "ifname@…":                    // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], NAT rules for this external ifname
    {
        "mode":"NAT mode",               // [ "forward", "dnat" ], "forward" for bi-directional NAT, "dnat" for dest NAT
        "dmzhost":"DMZ host ip address", // [ ip address ]
        "rule":                             // nat rule list
        {
            "rule name":                        // [ string ]:{}
            {
                "targetport":"target port",         // [ number ], port on the external ifname
                                                        // single port: 8080
                                                        // multiple port: 80,8000,8080
                                                        // range of port: 80-800
                "protocol":"protocol type",         // [ "tcp", "udp", "tcpudp", "icmp" ], "tcpudp" for TCP and UDP
                "destip":"destination address",     // [ ip address ], the local network ip address
                "destport":"destination port"       // [ number ]
                                                        // single port: 8080
                                                        // multiple port: 80,8000,8080
                                                        // range of port: 80-800
            }
            // ... more rule
        }
    }
    // ... more ifname
}
```  

Example, show all nat settings
```shell
forward@nat
{
    "ifname@wan":   // for WAN
    {
        "mode":"dnat",                        // dnat mode
        "rule":
        {
            "rdesktop":                       // map the ifname@wan 3389 to 192.168.31.250 TCP 3389
            {
                "targetport":"3389",
                "protocol":"tcp",
                "destip":"192.168.31.250",
                "destport":""
            }
        }
    },
    "ifname@lte":    // for LTE
    {
        "rule":
        {
            "ssh":                           // map the ifname@lte 22 to 192.168.31.230 22
            {
                "targetport":"22",
                "protocol":"tcp",
                "destip":"192.168.31.230",
                "destport":""
            },
            "devport":
            {
                "targetport":"10101",       // map the ifname@lte 10101 to 192.168.31.230 10101
                "protocol":"tcp",
                "destip":"192.168.31.230",
                "destport":""
            }
        },
        "dmzhost":""
    },
    "ifname@lte2":    // for LTE2
    {
        "dmzhost":"192.168.31.250"          // map the ifname@lte2 all tcp/udp protocol to 192.168.31.250
    }
}
```

Example, add a nat rule named "telnet" that make LTE tcp port 23 map to 23 of 192.168.31.230
```shell
forward@nat:ifname@lte/rule/telnet={"targetport":"23","protocol":"tcp","destip":"192.168.31.230","destport":"23"}
ttrue
```

Example, add a nat rule named "web" that make WAN tcp port 80 map to 8080 of 192.168.31.231
```shell
forward@nat:ifname@wan/rule/web={"targetport":"80","protocol":"tcp","destip":"192.168.31.231","destport":"8080"}
ttrue
```

Example, add a nat rule named "dns" that make WAN tcp port 53 map to 53 of 192.168.31.231
```shell
forward@nat:ifname@wan/rule/dns={"targetport":"53","protocol":"udp","destip":"192.168.31.231"}
ttrue
```

Example, add a nat rule named "test" that make WAN tcp port 1000-1200 map to 1000-1200 of 192.168.31.231
```shell
forward@nat:ifname@wan/rule/test={"targetport":"1000-1200","protocol":"tcp","destip":"192.168.31.231"}
ttrue
```

Example, delete the rule named web at the WAN
```shell
forward@nat:ifname@wan/rule/web=
ttrue
```

Example, delete the rule named telnet at the WAN
```shell
forward@nat:ifname@lte/rule/telnet=
ttrue
```

Example, modify the LTE dmz host to 192.168.31.250
```shell
forward@nat:ifname@lte/dmzhost=192.168.31.250
ttrue
```

Example, delete the LTE dmz host
```shell
forward@nat:ifname@lte/dmzhost=
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
forward@nat|{"ifname@wan":{"mode":"dnat","rule":{"web":{"targetport":"80","protocol":"tcp","destip":"192.168.1.10","destport":"8080"}}}}
ttrue
```

### **Component API**

Configuration is also read/written via standard **`forward@nat`** get/set/merge (see **Configuration** above).

+ `on[]` **refresh NAT/port mapping for an external ifname**, *succeed return ttrue*
    - Parameter **2** carries **`ifname`**; NAT rules for that external **ifname** are rebuilt from saved configuration (skips in **default** / **parasite** network modes).

+ `off[]` **tear down NAT for an external ifname**

### **Lifecycle API**

+ `setup[]` / `shut[]` — **not** listed in the default **init** / **uninit** schedule for **`forward@nat`**; rules are applied from **`on[]`** / **`off[]`** and configuration changes.

### **Joint handlers**

| Joint key | Method |
|-----------|--------|
| `network/onextern` | `forward@nat.on` |
| `network/onvpn` | `forward@nat.on` |


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_nat(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@nat", "status") == NULL)
        return -1;
    return ssets_string("forward@nat", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@nat", "list", NULL); talk_free if JSON */
```

