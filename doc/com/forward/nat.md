## forward@nat — NAT/Port Mapping Management

### Overview

Manage NAT port mapping from external **ifname** to local network clients. Prefer the **Component API** below instead of editing raw configuration when possible.

- supports per-ifname NAT rules and DMZ host
- skipped in **default** / **parasite** network modes



### Configuration reference ( forward@nat )

```json
// Attributes introduction 
{
    "ifname@…":                    // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], NAT rules for this external ifname
    {
        "mode":"NAT mode",               // [ "forward", "dnat" ], "forward" for bi-directional NAT, "dnat" for dest NAT
        "dmzhost":"DMZ host ip address", // [ ip address ]
        "rule":                             // [ json ], nat rule list
        {
            "rule name":                        // [ string ]: { json }, user can custom the rule name
            {
                "targetport":"target port",         // [ string ], port on the external ifname
                                                        // single port: 8080
                                                        // multiple port: 80,8000,8080
                                                        // range of port: 80-800
                "protocol":"protocol type",         // [ "tcp", "udp", "tcpudp", "icmp" ], "tcpudp" for TCP and UDP
                "destip":"destination address",     // [ ip address ], the local network ip address
                "destport":"destination port"       // [ string ]
                                                        // single port: 8080
                                                        // multiple port: 80,8000,8080
                                                        // range of port: 80-800
            }
            // "...":{ ... }  How many rule show how many properties
        }
    }
    // "...":{ ... }  How many ifname show how many properties
}
```

#### Configuration example

Example, show all nat settings

```shell
forward@nat
{
    "ifname@wan":
    {
        "mode":"dnat",
        "rule":
        {
            "rdesktop":
            {
                "targetport":"3389",
                "protocol":"tcp",
                "destip":"192.168.31.250",
                "destport":""
            }
        }
    },
    "ifname@lte":
    {
        "rule":
        {
            "ssh":
            {
                "targetport":"22",
                "protocol":"tcp",
                "destip":"192.168.31.230",
                "destport":""
            }
        },
        "dmzhost":""
    },
    "ifname@lte2":
    {
        "dmzhost":"192.168.31.250"
    }
}
```

#### Configuration settings example

Example, add a nat rule named "telnet" that maps LTE tcp port 23 to 192.168.31.230 port 23

```shell
forward@nat:ifname@lte/rule/telnet={"targetport":"23","protocol":"tcp","destip":"192.168.31.230","destport":"23"}
ttrue
```

Example, merge several settings at once

```shell
forward@nat|{"ifname@wan":{"mode":"dnat","rule":{"web":{"targetport":"80","protocol":"tcp","destip":"192.168.1.10","destport":"8080"}}}}
ttrue
```



### API Reference

#### Management APIs

+ `on[]` **refresh NAT/port mapping for an external ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**; NAT rules for that external ifname are rebuilt from saved configuration
    - skipped in **default** / **parasite** network modes

+ `off[]` **tear down NAT for an external ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**


### Published Joint Events

| Joint key | Method |
|-----------|--------|
| `network/onextern` | `forward@nat.on` |
| `network/onvpn` | `forward@nat.on` |
