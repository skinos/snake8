## forward@dnat — Destination NAT Proxy Management

### Overview

Manage destination NAT proxy rules to expose internet services on the gateway for local client access. Prefer the **Component API** below instead of editing raw configuration when possible.

- proxy internet server ports to local LAN clients
- supports TCP, UDP, and TCP+UDP protocols
- skipped in **default** / **parasite** network modes



### Configuration reference ( forward@dnat )

```json
// Attributes introduction 
{
    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], proxy rules for this LAN ifname
    {
        "rule name":                    // [ string ]: { json }, user can custom the rule name
        {
            "destip":"internet server ip address",    // [ ip address ]
            "destport":"internet server port",        // [ string ]
                                                          // single port: 80
                                                          // range of port: 80-100
                                                          // multiple port: 80,8080,8000
            "protocol":"protocol type",               // [ "tcp", "udp", "tcpudp" ], "tcpudp" for TCP and UDP
            "targetport":"local proxy port"           // [ number ]
        }
        // "...":{ ... }  How many rule show how many properties
    }
    // "...":{ ... }  How many ifname show how many properties
}
```

#### Configuration example

Example, show current all of dnat rules

```shell
forward@dnat
{
    "ifname@lan":
    {
        "proxy1":
        {
            "destip":"29.23.11.35",
            "destport":"28-90",
            "protocol":"tcpudp",
            "targetport":"100"
        }
    },
    "ifname@lan2":
    {
        "forweb":
        {
            "destip":"129.232.91.5",
            "destport":"80",
            "protocol":"tcp",
            "targetport":"8000"
        }
    }
}
```

#### Configuration settings example

Example, add a rule named proxy2 to ifname@lan

```shell
forward@dnat:ifname@lan/proxy2={"destip":"202.96.134.144","destport":"53","protocol":"tcpudp","targetport":"500"}
ttrue
```

Example, merge several settings at once

```shell
forward@dnat|{"ifname@lan":{"proxy1":{"destip":"1.2.3.4","destport":"80","protocol":"tcp","targetport":"8080"}}}
ttrue
```



### API Reference

#### Management APIs

+ `on[]` **refresh destination-NAT proxy rules for a LAN ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**; rules for that ifname are rebuilt from saved configuration
    - skipped in **default** / **parasite** network modes

+ `off[]` **tear down DNAT rules for an ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**


### Published Joint Events

| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@dnat.on` |
