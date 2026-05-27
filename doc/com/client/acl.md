## client@acl — Access Control List Management

### Overview

Manage per-interface access control lists (ACL) for local network traffic. Each logical **`ifname`** (e.g. `ifname@lan`) can have its own ACL with rules that control whether matching traffic is **dropped**, **accepted**, or **returned** for further processing. Rules can match by source address, destination address/port, protocol type, keywords, domain names, and time windows.

- manages ACL lifecycle: setup rules, shutdown rules, refresh on interface state changes
- supports TCP/UDP/all/domain/layer7 protocol filtering
- supports source matching by IP address, IP range, or MAC address
- supports destination matching by IP, IP range, port, port range, domain, or keyword
- supports time-based rules with date, time, monthday, and weekday constraints
- automatically refreshes ACL when LAN interfaces come up or down



### Configuration reference ( client@acl )

```json
// Attributes introduction 
{
    "interface name":                                     // [ string ], e.g. "ifname@lan", "ifname@lan2"
    {
        "status":"enable or disable the ACL",             // [ "disable", "enable" ]
        "rule":                                           // access control rules, valid when status is "enable"
        {
            "rule name":                                  // [ string ], user-defined rule name
            {
                "action":"rule action",                   // [ "drop", "accept", "return" ]
                                                              // "drop" to forbid
                                                              // "accept" to pass
                                                              // "return" to stop matching further rules
                "proto":"protocol type",                  // [ "domain", "tcp", "udp", "all", "layer7" ]
                                                              // "domain" for domain filtering, dest should be domain name
                                                              // "tcp" for TCP protocol
                                                              // "udp" for UDP protocol
                                                              // "layer7" for layer7 application protocol
                                                              // "all" or empty for all protocols
                "src":"source address",                   // [ string ], optional, who may send (LAN side); also accepts "source" key
                                                              // single IP: 192.168.8.222
                                                              // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                              // range of IP: 192.168.8.2-192.168.8.4
                                                              // single MAC: 00:23:43:13:34:40
                                                              // empty for all sources
                "dest":"destination address",             // [ string ], packets destination
                                                              // single IP: 202.96.11.32 (valid for tcp/udp/all)
                                                              // multiple IP: 2.3.1.2,4.34.2.1 (valid for tcp/udp/all)
                                                              // range of IP: 202.96.132.11-202.96.132.20 (valid for tcp/udp/all)
                                                              // domain: www.baidu.com (valid for domain)
                                                              // layer7 protocol name (valid for layer7)
                                                              // empty for all destinations (valid for tcp/udp/all)
                "destport":"destination port",            // [ string ], valid when proto is tcp or udp
                                                              // single port: 8080
                                                              // multiple port: 80,8000,8080
                                                              // range of port: 80-800
                                                              // empty for all ports
                "key":"keyword",                          // [ string ], keyword matching in packets, valid for tcp/udp/all
                "timer":"time-based rule",                // [ "disable", "enable" ]
                "timer_cfg":                              // time constraints, valid when timer is "enable"
                {
                    "datestart":"starting date",          // [ string ], format YYYY-MM-DD
                    "datestop":"ending date",             // [ string ], format YYYY-MM-DD
                    "timestart":"start time of day",      // [ string ], format hh:mm:ss
                    "timestop":"end time of day",         // [ string ], format hh:mm:ss
                    "monthdays":"month days",             // [ string ], optional, limits to certain days of month
                    "weekdays":"week days"                // [ string ], format 1,2,3,... (0 for Sunday)
                }
            }
            // "...":{}  How many rules show how many properties
        }
    }
    // "...":{}  How many interfaces show how many properties
}
```

#### Configuration example

Example, show current ACL settings
```shell
client@acl
{
    "ifname@lan":
    {
        "status":"enable",
        "rule":
        {
            "dis163":
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.163.com",
                "timer":"enable",
                "timer_cfg":
                {
                    "datestart":"",
                    "datestop":"",
                    "timestart":"00:00:00",
                    "timestop":"23:59:59",
                    "weekdays":"1,2,3,4,5,6,7"
                }
            },
            "dis164":
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.qq.com"
            }
        }
    }
}
```

#### Configuration settings example

Example, add a rule to block domain www.qq.com on ifname@lan
```shell
client@acl:ifname@lan/rule/disqq={"proto":"domain","dest":"www.qq.com","action":"drop"}
ttrue
```

Example, delete a rule named disqq
```shell
client@acl:ifname@lan/rule/disqq=
ttrue
```

Example, merge set ACL configure for ifname@lan
```shell
client@acl|{"ifname@lan":{"status":"enable","rule":{"block163":{"proto":"domain","dest":"www.163.com","action":"drop"}}}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize ACL rules for all local interfaces**
    - failed return tfalse
    - succeed return ttrue
    - skips default/parasite network modes
    - iterates all local interfaces, applies ACL rules where status is enable

+ `shut[]` **remove all ACL rules**
    - succeed return ttrue
    - removes iptables chains and rules for all local interfaces


#### Control APIs

+ `on[ info ]` **refresh ACL when a LAN interface comes up**
    - info ------------- [ json ], contains ifname field
    - succeed return ttrue
    - clears and reapplies ACL rules for the specified interface
    - skips default/parasite network modes

+ `off[ info ]` **tear down ACL when a LAN interface goes down**
    - info ------------- [ json ], contains ifname field
    - succeed return ttrue
    - removes ACL rules for the specified interface



### Published Joint Events

The following joint events trigger ACL refresh. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `network/on` | Sent when a local interface comes up. Triggers `client@acl.on` to refresh ACL rules for that interface. |
| `network/off` | Sent when a local interface goes down. Triggers `client@acl.off` to remove ACL rules for that interface. |
