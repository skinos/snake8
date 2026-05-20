## ifname@lan — Local/LAN Network Management

### Overview

Manage local (LAN) networks. This component depends on a local network interface or switch (SoC), typically via **`arch`** (`ethernet`, bridge/VLAN wiring), and the **network** project (`network@frame` registration — see [`../network/frame.md`](../network/frame.md)).
Usually `ifname@lan` is the first local network. If there are multiple local networks, `ifname@lan2` is the second local network, and numbering increases sequentially.

- manages LAN interface lifecycle: setup, shutdown, status query
- supports static IPv4 and DHCPv6/SLAAC IPv6 addressing
- provides DHCP server for local clients
- no NAT masquerade (it IS the LAN, not NATted)



### Network Architecture

`ifname@lan` is a **local interface** registered by `network@frame` during boot. It uses `ifname@ethcon` as concom and `bridge@lan` as ifdev. Unlike extern interfaces, it does NOT apply NAT masquerade and is NOT subject to multi-uplink scheduling. It can run a DHCP server to assign IPs to local clients. When the LAN comes up or down, it publishes `network/on` / `network/off` joint events.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( ifname@lan )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ], enable means auto-setup after boot

    // IPv4
    "mode":"IPV4 address mode",                                  // [ "dhcpc", "static" ]
                                                                      // "dhcpc" for DHCP client mode
                                                                      // "static" for manual IPv4 setting
    "static":                                 // detail configuration for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 netmask >
        "ip2":"IPv4 address 2",                     // < ipv4 address >
        "mask2":"IPv4 netmask 2",                   // < ipv4 netmask >
        "ip3":"IPv4 address 3",                     // < ipv4 address >
        "mask3":"IPv4 netmask 3",                   // < ipv4 netmask >
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // detail configuration for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ], temporary fallback address
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ], accept classless static routes
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], valid when "custom_dns" is "enable"
    },
    "dhcps":                                               // detail configuration for DHCP server settings
    {
        "status":"Whether to start the DHCP service",                      // [ "disable", "enable" ]
        "startip":"The start address within the IPv4 allocation pool",     // [ ipv4 address ]
        "endip":"IPv4 assigns the end address within the pool",            // [ ipv4 address ]
        "mask":"IPv4 assigns a subnet mask within a pool",                 // [ ipv4 netmask ]
        "lease":"lease time for assigned addresses",                       // [ number ], unit is seconds
        "gw":"Specifies the IPv4 gateway",                                 // [ ipv4 address ], default is local network IP address
        "dns":"Specifies the IPv4 DNS",                                    // [ ipv4 address ], default is local network IP address
        "dns2":"Specifies the IPv4 backup dns",                            // [ ipv4 address ]
        "options":"dnsmasq original options"                               // [ string ], multiple options are separated by semicolons
    },

    // IPv6
    "method":"IPv6 address mode",             // [ "disable", "manual", "automatic" ]
                                                    // "disable" means IPv6 is disabled
                                                    // "manual" means static IPv6 settings
                                                    // "automatic" means DHCPv6
    "manual":                                 // detail configuration for "method" is "manual"
    {
        "addr":"IPv6 address",                      // < ipv6 address >
        "prefix":"IPv6 prefix",                     // < number >, 1-128
        "hop":"IPv6 gateway",                       // [ ipv6 address ]
        "resolve":"IPv6 DNS",                       // [ ipv6 address ]
        "resolve2":"IPv6 DNS2"                      // [ ipv6 address ]
    },
    "automatic":                             // detail configuration for "method" is "automatic"
    {
        "custom_resolve":"Custom DNS",                   // [ "disable", "enable" ]
        "resolve":"Custom DNS1",                         // [ ipv6 address ], valid when "custom_resolve" is "enable"
        "resolve2":"Custom DNS2"                         // [ ipv6 address ], valid when "custom_resolve" is "enable"
    },
    "addrpool":
    {
        "status":"DHCPv6 service type",                                      // [ "disable", "enable" ]
        "startaddr":"The start address within the IPv6 allocation pool",     // [ ipv6 address ]
        "endaddr":"IPv6 assigns the end address within the pool",            // [ ipv6 address ]
        "prefix":"IPv6 assigns a subnet mask within a pool",                 // [ ipv6 address ]
        "leasetime":"lease time for assigns",                                // [ number ], the unit is second
        "hop":"Specifies the IPv6 gateway",                                  // [ ipv6 address ], default is local network IP address
        "resolve":"Specifies the IPv6 DNS",                                  // [ ipv6 address ], default is local network IP address
        "resolve2":"Specifies the IPv6 backup dns"                           // [ ipv6 address ]
    }
}
```

#### Configuration example

Example, show all configuration of the first local network
```shell
ifname@lan
{
    "mode":"static",                                 # mode is static
    "static":
    {
        "ip":"192.168.1.1",                          # IPv4 address is 192.168.1.1
        "mask":"255.255.255.0"                       # IPv4 netmask is 255.255.255.0
    },
    "method":"automatic",                            # IPv6 address mode is automatic
    "dhcps":
    {
        "status":"enable",           # enable the DHCP server
        "startip":"192.168.1.2",     # IPv4 pool start with 192.168.1.2
        "endip":"192.168.1.100",     # IPv4 pool end with 192.168.1.100
        "mask":"255.255.255.0",      # netmask is 255.255.255.0
        "lease":"86400",             # lease is 86400
        "gw":"",                     # not configure the gateway, default assigns 192.168.1.1
        "dns":""                     # not configure the dns, default assigns 192.168.1.1
    }
}
```

#### Configuration settings example

Example, modify the first local network IP address
```shell
ifname@lan:static/ip=192.168.2.1
ttrue
```

Example, disable DHCP server on the first local network
```shell
ifname@lan:dhcps/status=disable
ttrue
```

Example, merge set DHCP pool of the first local network( include "startip" "endip" )
```shell
ifname@lan:dhcps|{"startip":"192.168.2.100","endip":"192.168.2.200"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the local network**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the local network**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get local network information**
    - failed return NULL
    - succeed return [ json ], local network status information
    ```json
    {
        "status":"Current state",        // [ "nodevice", "uping", "down", "up" ]
                                             // "nodevice" means the underlying device is not present
                                             // "uping" means connecting
                                             // "down" means interface is down
                                             // "up" means connection is established
        "mode":"IPV4 address mode",     // [ "dhcpc", "static" ]
        "netdev":"netdev name",         // [ string ]
        "ifdev":"ifdev name",           // [ string ], Optional
        "gw":"gateway ip address",      // [ ip address ], Optional
        "dns":"dns ip address",         // [ ip address ], Optional
        "dns2":"dns2 ip address",       // [ ip address ], Optional
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "ontime":"online uptime",       // [ string ], Optional, online system uptime
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"received bytes",    // [ number ]
        "rx_packets":"received packets",// [ number ]
        "tx_bytes":"sent bytes",        // [ number ]
        "tx_packets":"sent packets",    // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, present when IPv6 is enabled
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr3":"IPv6 address3"         // [ ipv6 address ], Optional, exist when IPV6 enable
    }
    ```

    Example, get the first local network information
    ```shell
    ifname@lan.status
    {
        "status":"up",                     # connect is succeed
        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"lan",                    # netdev is lan
        "ip":"192.168.1.1",                # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0"      # local IPv6 address is fe80::50:f4ff:fe00:0
    }
    ```

+ `netdev[]` **get the netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the first local network netdev
    ```shell
    ifname@lan.netdev
    lan
    ```

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - succeed return [ string ], the ifdev component name

    Example, get the first local network ifdev
    ```shell
    ifname@lan.ifdev
    vlan@lan
    ```
