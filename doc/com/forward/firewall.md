## forward@firewall — Inbound Firewall Management

### Overview

Manage inbound firewall rules for external **ifname** (internet). Prefer the **Component API** below instead of editing raw configuration when possible.

- per-ifname firewall with default action (drop/accept)
- built-in access controls for ICMP, ESP/AH, telnet, SSH, web management
- NAT passthrough option to auto-allow `forward@nat` rules
- skipped in **default** / **parasite** network modes



### Configuration reference ( forward@firewall )

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

        "nat_through":"NAT rule passthrough",                          // [ "disable", "enable" ]
        "icmp_through":"ICMP protocol passthrough",                    // [ "disable", "enable" ]
        "espah_through":"ESP/AH protocol passthrough",                 // [ "disable", "enable" ]

        "rule":                              // [ json ], firewall rule list
        {
            "rule name":                                   // [ string ]: { json }, user can custom the rule name
            {
                "action":"drop or accept or return",                     // [ "drop", "accept", "return" ]
                "src":"source address",                                  // [ string ]
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // single MAC: 00:23:43:13:34:40
                                                                                  // empty for all ip address
                "srcport":"source port",                                 // [ string ], valid when protocol is tcp or udp
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // empty for all port
                "protocol":"protocol type",                              // [ "tcp", "udp", "all", ... ]
                "dest":"destination address",                            // [ string ]
                                                                                  // single IP: 202.96.11.32
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1
                                                                                  // range of IP: 202.96.132.11-202.96.132.20
                                                                                  // empty for all ip address
                "destport":"destination port"                            // [ string ], valid when protocol is tcp or udp
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // empty for all port
            }
            // "...":{ ... }  How many rule show how many properties
        }
    }
    // "...":{ ... }  How many ifname show how many properties
}
```

#### Configuration example

Example, show current all of firewall settings

```shell
forward@firewall
{
    "ifname@lte":
    {
        "status":"enable",
        "default":"drop",
        "rule":
        {
            "pcweb":
            {
                "action":"accept",
                "dest":"192.168.8.222",
                "protocol":"tcp",
                "destport":"80"
            }
        }
    },
    "ifname@lte2":
    {
        "status":"disable",
        "default":"drop",
        "rule":
        {
        }
    }
}
```

#### Configuration settings example

Example, add the rule named webwork for ifname@lte

```shell
forward@firewall:ifname@lte/rule/webwork={"action":"accept","src":"113.23.64.28","protocol":"tcp","destport":"80"}
ttrue
```

Example, merge several settings at once

```shell
forward@firewall|{"ifname@lte":{"status":"enable","default":"drop"}}
ttrue
```



### API Reference

#### Management APIs

+ `on[]` **refresh inbound firewall rules for an external ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**; the firewall for that ifname is rebuilt from saved configuration
    - skipped in **default** / **parasite** network modes

+ `off[]` **tear down firewall for an ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**


### Published Joint Events

| Joint key | Method |
|-----------|--------|
| `network/onextern` | `forward@firewall.on` |
| `network/onvpn` | `forward@firewall.on` |
