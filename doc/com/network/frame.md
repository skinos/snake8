## network@frame — Network Frame Management

### Overview

The **network frame** component (`network@frame`) is the hub for LAN/WAN/VPN registration, routing policy, and firewall hooks. When several **uplinks** exist, it works with the **multi-link scheduler service** to pick the active default route using numbered **priority slots** (`"1"`..`"10"`). Supported policies include **cold backup**, **hot backup**, and **lazy hot backup** (no automatic fail-back until the current uplink fails).

- manages network infrastructure lifecycle: iptables NAT/masq, flow offload, IP rules
- registers local/extern/VPN interfaces and coordinates VLAN and bridge sub-modules
- publishes joint events on IPv4/IPv6 interface state changes
- works with the **connect** service for multi-uplink scheduling



### Configuration reference ( network@frame )

```json
// Attributes introduction 
{
    "type":"Multiple link connect type",                           // [ "cold", "hot", "hot2", "hot3", "hot4", "hot5", "lazy", "lazy2", "lazy3", "lazy4", "lazy5" ]
                                                                   // "cold" for cold backup, only one uplink active at a time
                                                                   // "hot" for hot backup, multiple uplinks may stay up, default route prefers smallest-numbered online slot
                                                                   // "hotN" (N=2..5): only slots "1".."N" participate in scheduler decisions
                                                                   // "lazy" for lazy hot backup, does not move back to higher-priority uplink until current one fails
                                                                   // "lazyN" (N=2..5): only slots "1".."N" participate in scheduler decisions
    "concom":"Multiple link connection management components",     // [ string ], optional, custom scheduling component

    "1":"ifname object of extern",                                 // [ string ], priority slot 1, e.g. "ifname@wan", "ifname@lte", "ifname@wisp"
    "2":"ifname object of extern",                                 // [ string ], priority slot 2
    "3":"ifname object of extern",                                 // [ string ], priority slot 3
    "4":"ifname object of extern",                                 // [ string ], priority slot 4
    "5":"ifname object of extern",                                 // [ string ], priority slot 5
    "6":"ifname object of extern",                                 // [ string ], priority slot 6
    "7":"ifname object of extern",                                 // [ string ], priority slot 7
    "8":"ifname object of extern",                                 // [ string ], priority slot 8
    "9":"ifname object of extern",                                 // [ string ], priority slot 9
    "10":"ifname object of extern",                                // [ string ], priority slot 10

    "delay_count":"Statistical delay times of last",               // [ number ], reserved for delay-based scheduling
    "delay_divide":"delay divide line",                            // [ number ], the unit is ms
    "delay_diff":"delay differential",                             // [ number ], the unit is ms

    "custom_dns":"Custom DNS",                                     // [ "disable", "enable" ]
    "dns":"Custom DNS1",                                           // [ ip address ], valid when "custom_dns" is "enable"
    "dns2":"Custom DNS2",                                          // [ ip address ], valid when "custom_dns" is "enable"

    "offload":"flow offload mode"                                  // [ "disable", "enable", "hw" ]
                                                                        // "disable" for no offload
                                                                        // "enable" for software flow offload
                                                                        // "hw" for hardware flow offload
}
```

#### Configuration example

Example, show all the multi-link configure
```shell
network@frame
{
    "type":"hot",                              # multi-link scheduling mode: hot backup
    "1":"ifname@wan",                          # priority slot 1: WAN (Ethernet)
    "2":"ifname@lte",                          # priority slot 2: LTE modem
    "3":"ifname@lte2",                         # priority slot 3: second LTE modem
    "4":"ifname@wisp",                         # priority slot 4: WISP (wireless relay)
    "custom_dns":"disable"                     # use DNS from the active uplink (not custom)
}
```

#### Configuration settings example

Example, set multi-link scheduling mode to hot4
```shell
network@frame:type=hot4
ttrue
```

Example, set priority slot 1 to WAN and slot 2 to LTE
```shell
network@frame:1=ifname@wan
ttrue
```

Example, merge set multi-link configure( include "type" "1" "2" "3" "4" )
```shell
network@frame|{"type":"hot4","1":"ifname@lte","2":"ifname@lte2","3":"ifname@wan","4":"ifname@wisp"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the network infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Sets up iptables NAT/masq chain, flow offload, IP rules, registers local/extern interfaces, sets up VLAN and bridge, starts connect service

+ `shut[]` **shutdown the network infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - Stops the multi-link scheduler, unregisters interfaces, clears firewall/routing hooks


#### Query APIs

+ `status[]` **show external connections status**
    - failed return NULL
    - succeed return [ json ], status of all external connections
    ```json
    {
        "ifname object":           // [ string ], e.g. "ifname@wan", "ifname@lte", "ifname@lte2", "ifname@wisp"
        {
            "status":"Whether online",     // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
                                               // "nodevice" means the corresponding module could not be found
                                               // "reset" means modem is being reset
                                               // "setup" means modem is being initialized
                                               // "register" means modem is registering to network
                                               // "uping" for connecting
                                               // "scanning" for scanning the peer
                                               // "block" means waiting for keeplive checks to recover
                                               // "up" means ready for Internet access
                                               // "failed" for keeplive failed
                                               // "down" for the ifname is down
            "inuse":"Whether used"         // [ "disable", "enable" ], enable for in used, disable for not used
        }
        // "...":{}  How many extern show how many properties
    }
    ```

    Example, get all extern connection status
    ```shell
    network@frame.status
    {
        "ifname@lte":
        {
            "status":"up",
            "inuse":"enable"
        },
        "ifname@lte2":
        {
            "status":"down",
            "inuse":"disable"
        }
    }
    ```

+ `list[ type ]` **list all registered connections**
    - type ----------- [ string ], optional, filter by type: "local", "extern", "outer", "vpn", or omit for all
    - failed return NULL
    - succeed return [ json ], all registered connections with their ifdev
    ```json
    {
        "ifname object": "The corresponding ifdev object"    // [ string ]: [ string ]
        // "...":"..."  How many connections show how many properties
    }
    ```

    Example, list all connections
    ```shell
    network@frame.list
    {
        "ifname@lan":"bridge@lan",
        "ifname@lte":"modem@lte",
        "ifname@lte2":"modem@lte2"
    }
    ```

    Example, list only extern connections
    ```shell
    network@frame.list[ extern ]
    {
        "ifname@lte":"modem@lte",
        "ifname@lte2":"modem@lte2"
    }
    ```

+ `mode_list[]` **list available network modes**
    - failed return NULL
    - succeed return [ json ], available network mode names
    ```json
    {
        "mode name": ""     // [ string ]: [ string ]
        // "...":"..."  How many modes show how many properties
    }
    ```

    Example, list available network modes
    ```shell
    network@frame.mode_list
    {
        "default":"",
        "gateway":""
    }
    ```

+ `ifname[ device ]` **look up an ifname by device name**
    - device ----------- [ string ], the device name to look up
    - failed return NULL
    - succeed return [ string ], the ifname object name

    Example, look up ifname by device
    ```shell
    network@frame.ifname[ modem@lte ]
    ifname@lte
    ```

+ `local[ ifname ]` **list or check local interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ "uping", "down", "up" ]
            "mode":"IPV4 address mode",    // [ "dhcpc", "static" ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ], Optional
            "gw":"gateway ip address",     // [ ip address ], Optional
            "dns":"dns ip address",        // [ ip address ], Optional
            "dns2":"dns2 ip address",      // [ ip address ], Optional
            "ip":"ip address",             // [ ip address ]
            "mask":"network mask",         // [ ip address ]
            "mac":"MAC address",           // [ mac address ]
            "method":"IPv6 address mode",  // [ "manual", "automatic", "slaac" ], Optional
            "addr":"IPv6 address"          // [ ipv6 address ], Optional
        }
        // "...":{}  How many local interfaces show how many properties
    }
    ```

    Example, list all local interfaces
    ```shell
    network@frame.local
    {
        "ifname@lan":
        {
            "status":"up",
            "ip":"192.168.1.1",
            "mask":"255.255.255.0"
        }
    }
    ```

    Example, check if ifname@lan is a local interface
    ```shell
    network@frame.local[ ifname@lan ]
    ttrue
    ```

+ `extern[ ifname ]` **list or check extern interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
            "mode":"IPV4 address mode",    // [ "dhcpc", "static", "pppoec" ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ]
            "gw":"gateway ip address",     // [ ip address ]
            "dns":"dns ip address",        // [ ip address ]
            "dns2":"dns2 ip address",      // [ ip address ]
            "ip":"ip address",             // [ ip address ]
            "mask":"network mask",         // [ ip address ]
            "delay":"delay time",          // [ "failed", "block", number ], Optional
            "mac":"MAC address",           // [ mac address ]
            "method":"IPv6 address mode",  // [ "manual", "automatic", "slaac" ], Optional
            "addr":"IPv6 address"          // [ ipv6 address ], Optional
        }
        // "...":{}  How many extern interfaces show how many properties
    }
    ```

    Example, list all extern interfaces
    ```shell
    network@frame.extern
    {
        "ifname@wan":
        {
            "status":"up",
            "ip":"192.168.10.1",
            "gw":"192.168.10.254"
        }
    }
    ```

+ `vpn[ ifname ]` **list or check VPN interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "mode":"current mode",         // [ string ]
            "status":"current state",      // [ string ]
            "ifdev":"ifdev object",        // [ string ]
            "netdev":"netdev name",        // [ string ]
            "ip":"IP address",             // [ ip address ]
            "rx_bytes":"received bytes",   // [ number ]
            "rx_packets":"received packets",// [ number ]
            "tx_bytes":"sent bytes",       // [ number ]
            "tx_packets":"sent packets",   // [ number ]
            "mac":"MAC address"            // [ mac address ]
        }
        // "...":{}  How many VPN interfaces show how many properties
    }
    ```

+ `outer[ ifname ]` **list or check extern and VPN interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ string ]
            "mode":"IPV4 address mode",    // [ string ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ]
            "ip":"ip address",             // [ ip address ]
            "mac":"MAC address"            // [ mac address ]
        }
        // "...":{}  How many outer interfaces show how many properties
    }
    ```

+ `default[ ifname ]` **get or check the current default connection**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse, the interface that owns the default route
    ```json
    {
        "ifname":"interface name",     // [ string ]
        "ifdev":"device name",         // [ string ]
        "netdev":"network device"      // [ string ]
    }
    ```

    Example, get current default connection
    ```shell
    network@frame.default
    {
        "ifname":"ifname@wan",
        "ifdev":"ethernet@lan1",
        "netdev":"eth0"
    }
    ```

+ `gateway[ ifname ]` **get or check the current gateway connection**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse, the interface that owns the gateway route
    ```json
    {
        "ifname":"interface name",     // [ string ]
        "ifdev":"device name",         // [ string ]
        "netdev":"network device"      // [ string ]
    }
    ```

+ `extup[]` **get the first extern interface that is currently up**
    - failed return NULL
    - succeed return [ json ], status information of the first extern interface that is up
    ```json
    {
        "ifname":"interface name",           // [ string ]
        "ifdev":"device name",               // [ string ]
        "netdev":"network device",           // [ string ]
        "ip":"IP address",                   // [ ip address ]
        "mask":"network mask",               // [ ip address ]
        "gw":"gateway",                      // [ ip address ]
        "dns":"DNS server",                  // [ ip address ]
        "dns2":"backup DNS"                  // [ ip address ]
    }
    ```


#### Control APIs

+ `register[ ifname, concom, ifdev, type ]` **register a network interface with the frame**
    - ifname ----------- [ string ], interface name, e.g. "ifname@lte"
    - concom ----------- [ string ], connection component name, e.g. "ifname@ethcon"
    - ifdev ------------ [ string ], optional, interface device name, e.g. "modem@lte"
    - type ------------- [ string ], optional, interface type: "local", "extern", "vpn", defaults to "vpn"
    - failed return tfalse
    - succeed return ttrue

    Example, register an extern LTE interface
    ```shell
    network@frame.register[ ifname@lte, ifname@ltecon, modem@lte, extern ]
    ttrue
    ```

+ `unregister[ ifname, type ]` **unregister a network interface from the frame**
    - ifname ----------- [ string ], interface name
    - type ------------- [ string ], optional, interface type, defaults to "vpn"
    - failed return tfalse
    - succeed return ttrue

+ `add[ ifdev, netdev ]` **add a network device**
    - ifdev ------------ [ string ], interface device name
    - netdev ----------- [ string ], optional, network device name
    - failed return tfalse
    - succeed return ttrue
    - Tries VLAN add, then bridge add, then matches against local/extern interface lists and calls their setup

+ `delete[ ifdev ]` **delete a network device**
    - ifdev ------------ [ string ], interface device name
    - failed return tfalse
    - succeed return ttrue
    - Searches local then extern lists, calls shut on match; falls back to bridge/vlan delete

+ `online[ info ]` **IPv4 online notification**
    - info ------------- [ json ], connection information
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface comes online (IPv4)
    ```json
    {
        "ifname":"interface name",         // [ string ]
        "ifdev":"device name",             // [ string ]
        "netdev":"network device",          // [ string ]
        "ip":"IP address",                 // [ ip address ]
        "mask":"network mask",             // [ ip address ]
        "gw":"gateway",                    // [ ip address ]
        "dns":"DNS server",                // [ ip address ]
        "dns2":"backup DNS"                // [ ip address ]
    }
    ```

+ `offline[ ifname ]` **IPv4 offline notification**
    - ifname ----------- [ string ], interface name
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface goes offline (IPv4)

+ `upline[ info ]` **IPv6 online notification**
    - info ------------- [ json ], connection information
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface comes online (IPv6)

+ `downline[ ifname ]` **IPv6 offline notification**
    - ifname ----------- [ string ], interface name
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface goes offline (IPv6)



### Published Joint Events

The following joint events are published when network interface state changes. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `network/on` | Sent when a local interface (LAN) comes up with IPv4 connectivity. Triggered after the interface obtains an IP address and is ready for local network communication. |
| `network/off` | Sent when a local interface (LAN) goes down or loses IPv4 connectivity. Triggered when the interface is disabled or the connection is lost. |
| `network/up` | Sent when a local interface (LAN) comes up with IPv6 connectivity. Triggered after the interface obtains an IPv6 address. |
| `network/down` | Sent when a local interface (LAN) goes down or loses IPv6 connectivity. |
| `network/onextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv4 connectivity. Triggered after successful connection establishment to the internet service provider. This event precedes `network/online` for external connections. |
| `network/offextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv4 connectivity. Triggered when the ISP connection is lost. |
| `network/upextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv6 connectivity. |
| `network/downextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv6 connectivity. |
| `network/onvpn` | Sent when a VPN interface comes up with IPv4 connectivity. Triggered after the VPN tunnel is successfully established. |
| `network/offvpn` | Sent when a VPN interface goes down or loses IPv4 connectivity. Triggered when the VPN tunnel is closed or interrupted. |
| `network/upvpn` | Sent when a VPN interface comes up with IPv6 connectivity. |
| `network/downvpn` | Sent when a VPN interface goes down or loses IPv6 connectivity. |
| `network/online` | Sent when the system establishes a default route to the internet (IPv4). Triggered after an external interface comes up and the routing table is updated. This indicates the device has full internet access. |
| `network/offline` | Sent when the system loses its default route to the internet (IPv4). Triggered when all external interfaces are down or when the primary connection fails. |
| `network/upline` | Sent when the system establishes IPv6 internet connectivity via an external interface or VPN. |
| `network/downline` | Sent when the system loses IPv6 internet connectivity. |
