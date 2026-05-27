## client@dhcps — DHCP Server Management

### Overview

Manage DHCP server (dnsmasq) and IPv6 DHCP relay (odhcpd) for local network interfaces. DHCP server settings are configured per logical **`ifname`** (e.g. `ifname@lan`). The component generates dnsmasq configuration from per-interface settings, supports static IP assignment via station bindings, and provides DHCP lease listing.

- manages dnsmasq and odhcpd lifecycle: setup, shutdown, reset, reload
- generates per-interface DHCP configuration from ifname settings
- supports static IP assignment, gateway, DNS, and classless static routes
- integrates with client@station for MAC-IP binding (dhcp-host entries)
- supports DNS proxy configuration
- provides DHCP lease listing from dnsmasq lease file



### Configuration reference ( client@dhcps )

DHCP server configuration is stored per-interface in each **`ifname@lan`** component's `dhcps` subtree. `client@dhcps` reads and writes these settings.

```json
// Attributes introduction 
{
    "interface name":                             // [ string ], e.g. "ifname@lan", "ifname@lan2"
    {
        "status":"DHCP server status",            // [ "disable", "enable" ]
        "startip":"pool start IP",                // [ ip address ], first IP of the DHCP range
        "endip":"pool end IP",                    // [ ip address ], last IP of the DHCP range
        "mask":"subnet mask",                     // [ ip address ], optional, defaults to interface static mask
        "lease":"lease time",                     // [ number ], seconds, values below 120 are raised to 120
        "gw":"gateway address",                   // [ ip address ], optional, router option sent to clients
        "dns":"primary DNS server",               // [ ip address ], optional, DNS server option sent to clients
        "dns2":"secondary DNS server",            // [ ip address ], optional, secondary DNS server
        "routeopt_table":"static routes",         // [ json ], optional, RFC 3442 classless static route option
        {
            "route name":                         // [ string ]
            {
                "target":"network address",       // [ ip address ]
                "mask":"CIDR bits",               // [ string ]
                "gw":"gateway"                    // [ ip address ]
            }
            // "...":{}  How many routes show how many properties
        },
        "options":"extra dnsmasq options",        // [ string ], optional, semicolon-separated dnsmasq config lines
        "dnsproxy":"DNS proxy settings"           // [ json ], optional, DNS proxy/redirect configuration
        {
            "dns":"DNS server to redirect to"     // [ ip address ]
        }
    }
    // "...":{}  How many interfaces show how many properties
}
```

#### Configuration example

Example, show all DHCP server configuration
```shell
client@dhcps
{
    "ifname@lan":
    {
        "status":"enable",
        "startip":"192.168.31.100",
        "endip":"192.168.31.254",
        "mask":"255.255.255.0",
        "lease":"86400",
        "gw":"192.168.31.1",
        "dns":"8.8.8.8",
        "dns2":"114.114.114.114"
    }
}
```

#### Configuration settings example

Example, modify DHCP pool for ifname@lan
```shell
client@dhcps:ifname@lan/startip=192.168.31.200
ttrue
```

Example, set DNS servers for ifname@lan
```shell
client@dhcps:ifname@lan/dns=8.8.8.8
ttrue
```

Example, merge set DHCP configure for ifname@lan
```shell
client@dhcps|{"ifname@lan":{"status":"enable","startip":"192.168.31.100","endip":"192.168.31.254","lease":"86400"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start DHCP services**
    - succeed return ttrue
    - on slave platforms, no DHCP service is started
    - starts dnsmasq and odhcpd (when available)

+ `shut[]` **stop DHCP services**
    - succeed return ttrue
    - stops dnsmasq and odhcpd


#### Query APIs

+ `list[]` **list current DHCP lease information**
    - succeed return [ json ], DHCP lease entries from dnsmasq lease file
    - returns empty JSON object `{}` if no lease information
    - each key is a client MAC; each value has ip and name
    ```json
    {
        "client MAC address":                 // [ string ]
        {
            "ip":"ip address",                // [ ip address ]
            "name":"client name"              // [ string ], hostname from DHCP
        }
        // "...":{}  How many clients show how many properties
    }
    ```

    Example, list all DHCP clients
    ```shell
    client@dhcps.list
    {
        "04:CF:8C:39:91:7A":
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",
            "ip":"192.168.31.140"
        },
        "40:31:3C:B5:6D:4C":
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C"
        }
    }
    ```


#### Control APIs

+ `reset[]` **restart DHCP services**
    - succeed return ttrue
    - on slave platforms, does nothing
    - backs up lease file, restarts dnsmasq and odhcpd, restores lease file
    - typically called after network interface changes

+ `on[]` **reload DHCP configuration**
    - succeed return ttrue
    - on slave platforms, does nothing
    - sends SIGHUP to dnsmasq and odhcpd to reload configuration without full restart



### Published Joint Events

The following joint events trigger DHCP service actions. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `network/on` | Sent when a local interface comes up. Triggers `client@dhcps.reset` to restart DHCP services. |
| `network/off` | Sent when a local interface goes down. Triggers `client@dhcps.reset` to restart DHCP services. |
| `network/online` | Sent when internet connectivity is established. Triggers `client@dhcps.on` to reload configuration. |
| `network/offline` | Sent when internet connectivity is lost. Triggers `client@dhcps.on` to reload configuration. |
