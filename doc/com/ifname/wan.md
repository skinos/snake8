## ifname@wan — WAN Network Management

### Overview

Manage WAN networks. This component depends on WAN-facing interfaces (often **`arch`** `ethernet`) and the **network** project (`network@frame`, `skinnet`, multi-link **`connect`** — see [`../network/frame.md`](../network/frame.md)).
Usually `ifname@wan` is the first WAN network. If there are multiple WANs, `ifname@wan2` is the second WAN network, and numbering increases sequentially.

- manages WAN interface lifecycle: setup, shutdown, status query
- supports static, DHCP client, and PPPoE dial IPv4 addressing
- supports static, DHCPv6, and SLAAC IPv6 addressing (`mode6`); `6in4`/`6rd` keys are reserved (not implemented in this release)
- when the kernel has no IPv6 support, reading this ifname’s config omits IPv6 keys (`mode6`, `static6`, `dhcpc6`, `slaac`, `masq6`, `mtu6`, …); IPv6 runtime stays off
- provides keeplive mechanism with ICMP, DNS, and receive packet detection
- NAT masquerade for outgoing traffic



### Network Architecture

`ifname@wan` is an **extern interface** registered by `network@frame` during boot. It uses `ifname@ethcon` as concom and `ethernet@lan1` as ifdev. As an extern interface, it is subject to multi-uplink scheduling by `network@connect` and applies NAT masquerade for LAN clients. It monitors link health via keeplive (ICMP/DNS/recv) and publishes `network/onextern` / `network/offextern` joint events on state changes.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( ifname@wan )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ], enable means auto-setup after boot

    // MAC
    "mac":"set MAC address for interface",                       // [ mac address ]

    // IPv4
    "tid":"table identify number",                               // [ number ], policy route table ID, mainly used in multi-WAN
    "mode":"IPV4 address mode",                                  // [ "dhcpc", "static", "pppoec" ]
                                                                      // "dhcpc" for DHCP client
                                                                      // "static" for manual setting
                                                                      // "pppoec" for PPPoE dial
    "static":                                 // detail configuration for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 netmask >
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // detail configuration for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ]
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ]
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], valid when "custom_dns" is "enable"
    },
    "pppoec":                                   // detail configuration for "mode" is "pppoec"
    {
        "username":"PPPOE username",                     // [ string ]
        "password":"PPPOE password",                     // [ string ]
        "service":"service name",                        // [ string ], default accept all service
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ "disable", "enable" ]
        "dns":"Custom DNS1",                             // [ ip address ], valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2",                            // [ ip address ], valid when "custom_dns" is "enable"
        "txqueuelen":"tx queue size"                     // [ number ]
    },
    "masq":"outgoing NAT for IPv4",                                               // [ "disable", "enable" ]
    "mtu":"Maximum transmission unit",                                            // [ number ], The unit is in bytes

    // IPv6 keys: addr = this ifname address; mask = this ifname subnet length;
    //            prefix = CIDR delegated to LAN; pdlen = IA_PD length to request (auto/56/disable)
    "mode6":"IPv6 address mode",              // [ "disable", "auto", "static6", "dhcpc6", "slaac", "6in4", "6rd" ]
                                                    // default "auto"; omitted from config read when the kernel has no IPv6
                                                    // "disable" — no IPv6
                                                    // "auto" — run DHCPv6 client (odhcp6c) with RA merge; succeed if IA_PD or a GUA on this ifname
                                                    //          status/upline may report mode6 as "slaac" when only RA GUA was obtained (not a second ethcon slaac stage)
                                                    // "static6" — use static6{}
                                                    // "dhcpc6" — use dhcpc6{}
                                                    // "slaac" — use slaac{}; always accept RA (kernel accept_ra path)
                                                    // "6in4" / "6rd" — reserved; not implemented in this release
    "static6":                                // detail when "mode6" is "static6"
    {
        "addr":"IPv6 address",                      // < ipv6 address >
        "mask":"IPv6 subnet length of addr",        // [ number ], default 64; this ifname only (WAN on-link length)
        "gw":"IPv6 gateway",                        // [ ipv6 address ]
        "dns":"IPv6 DNS",                           // [ ipv6 address ]
        "dns2":"IPv6 DNS2"                          // [ ipv6 address ]
                                                    // no static LAN PD here; give LAN a prefix via ifname@lan static6 / ula, or real IA_PD from dhcpc6
    },
    "dhcpc6":                                 // detail when "mode6" is "dhcpc6", and the first try of "auto"
    {
        "request":"DHCPv6 address request",          // [ "try", "force", "none" ], default "try"
        "pdlen":"IA_PD length to request",           // [ "auto", "48", "52", "56", "60", "64", "disable" ], default "auto"
                                                         // requested length only; the CIDR obtained is status/upline "prefix"
        "ra":"accept Router Advertisement",          // [ "disable", "enable" ], default "enable"
        "ra_holdoff":"min seconds between RA updates", // [ number string ], default "3" (odhcp6c -m; RFC4861)
        "release":"send release on stop",            // [ "disable", "enable" ], default "disable"
        "need_pd":"require IA_PD to succeed",        // [ "disable", "enable" ], default "disable"
        "clientid":"DHCPv6 client DUID",             // [ string ], empty default
        "reqopts":"extra DHCPv6 request options",    // [ string ], empty default
        "custom_dns":"Custom IPv6 DNS",              // [ "disable", "enable" ], default "disable"
        "dns":"Custom IPv6 DNS1",                    // [ ipv6 address ], valid when "custom_dns" is "enable"
        "dns2":"Custom IPv6 DNS2"                    // [ ipv6 address ], valid when "custom_dns" is "enable"
    },
    "slaac":                                  // detail when "mode6" is "slaac"
    {
        "custom_dns":"Custom IPv6 DNS",              // [ "disable", "enable" ], default "disable"
        "dns":"Custom IPv6 DNS1",                    // [ ipv6 address ], valid when "custom_dns" is "enable"
        "dns2":"Custom IPv6 DNS2"                    // [ ipv6 address ], valid when "custom_dns" is "enable"
    },
    "6in4":                                   // reserved (not implemented); detail when "mode6" is "6in4"
    {
        "peer":"tunnel remote IPv4",                 // [ ipv4 address ]
        "addr":"local IPv6 endpoint",                // < ipv6 address >
        "prefix":"delegated prefix",                 // [ string ], e.g. 2001:db8::/48, PD source for LAN
        "local":"local IPv4 endpoint",               // [ ipv4 address ], empty means this ifname IPv4
        "mtu":"tunnel MTU",                          // [ number ], default 1480
        "ttl":"tunnel TTL",                          // [ number ], default 64
        "tunnel_id":"broker tunnel id",              // [ string ], empty default
        "username":"broker username",                // [ string ], empty default
        "password":"broker password",                // [ string ], empty default
        "update_key":"broker update key"             // [ string ], empty default
    },
    "6rd":                                    // reserved (not implemented); detail when "mode6" is "6rd"
    {
        "peer":"6rd border relay IPv4",              // [ ipv4 address ]
        "prefix":"6rd IPv6 prefix",                  // < ipv6 prefix >
        "prefix_len":"6rd prefix length",            // [ number ], default 32
        "ip4_mask":"IPv4 mask bits in 6rd",          // [ number ], default 0
        "local":"local IPv4 endpoint",               // [ ipv4 address ], empty means this ifname IPv4
        "mtu":"tunnel MTU",                          // [ number ], default 1480
        "ttl":"tunnel TTL"                           // [ number ], default 64
    },
    "masq6":"outgoing NAT66",                 // [ "auto", "enable", "disable" ], default "auto"
                                                    // "auto" — NAT66 only when no prefix is delegated to LAN; with PD do not NAT
                                                    // "enable" / "disable" — force; relay needs "disable"
    "mtu6":"IPv6 MTU",                        // [ number ], empty default means do not set IPv6 MTU separately; 6in4/6rd use the tunnel "mtu"

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ "disable", "icmp", "dns", "recv", "auto" ]
                                      // "disable" — no keeplive service
                                      // "icmp" — ping dest[]; after failed consecutive fails, run action (keepoff)
                                      // "dns" — resolve via ifname DNS (reg dns/dns2); after failed consecutive
                                      //         fails (whether or not a probe ever succeeded), run action (keepoff)
                                      // "recv" — count RX packets; interface up with traffic counts as success;
                                      //          after failed consecutive quiet periods, run action (keepoff)
                                      // "auto" — try DNS first (same probe as "dns", params under "auto");
                                      //          if DNS never succeeds once, fall back to "recv" for this run;
                                      //          if DNS succeeded before then fails, run action (keepoff), no recv fallback;
                                      //          if dns/dns2 missing, keeplive exits (cannot run)
        "action":"action when keeplive fails",  // [ "reboot", "reset", "redial" ]
                                                    // "reboot" for reboot the system
                                                    // "reset" for reset the interface device
                                                    // "redial" for redial the connection
        "icmp":                                                   // detail configuration for "type" is "icmp"
        {
            "dest":                                                         // destination address for ICMP keeplive
            {
                "destination identify":"destination address",                     // [ string ]: [ IP address ]
                // "...":"..."  How many destinations show how many properties
            },
            "dest6":                                                        // IPv6 destination address for ICMP keeplive
            {
                "destination identify":"destination address",                     // [ string ]: [ ipv6 address ]
                // "...":"..."  How many destinations show how many properties
            },
            "timeout":"Maximum time to wait for the return of a PING echo packet",     // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds
        },
        "dns":                                                   // detail configuration for "type" is "dns"
        {
            "timeout":"Maximum time to wait for the return of a dns resolve packet",   // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds
        },
        "auto":                                                  // DNS-phase params when "type" is "auto" (probe uses ifname dns/dns2)
        {
            "timeout":"Maximum time to wait for the return of a dns resolve packet",   // [ number ], The unit is in seconds
            "failed":"Number of detection failures before DNS phase ends",             // [ number ]
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds
        },
        "recv":                                                  // detail for "type" is "recv", and for "auto" after DNS never succeeded
        {
            "timeout":"How many seconds did not receive a packet considered a failure",// [ number ], The unit is in seconds
            "packets":"How many packets",                                              // [ number ]
            "failed":"failed times"                                                    // [ number ]
        }
    },

    // Configure connect detection and failed action
    "need_connect":"must connect succeed",                                             // [ "enable", "disable" ]
    "connect_failed_threshold":"first failed to reset time",                           // [ number ], default 60 seconds
    "connect_failed_threshold2":"second failed to reset time",                         // [ number ], default 180 seconds
    "connect_failed_threshold3":"third failed to reset time",                          // [ number ], default 600 seconds
    "connect_failed_everytime":"every failed to reset time",                           // [ number ], default 1800 seconds

    // Configure general failed action (for keeplive/online failures)
    "failed_threshold":"first failed to reset time",                                   // [ number ], default 3
    "failed_threshold2":"second failed to reset time",                                 // [ number ], default 7
    "failed_threshold3":"third failed to reset time",                                  // [ number ], default 15
    "failed_everytime":"every failed to reset time"                                    // [ number ], default 37
}
```

#### Configuration example

Example, show all configuration of the first WAN
```shell
ifname@wan
{
    "mac":"88:12:4E:23:43:12",                       # clone the MAC
    "mode":"pppoec",                                 # mode is PPPoE client
    "pppoec":
    {
        "username":"1923221@gd.com",                # PPPOE username is 1923221@gd.com
        "password":"FDAED13E"                       # PPPOE password is FDAED13E
    },
    "masq":"enable",                                 # out stream share the interface IPv4 address to access the Internet
    "mode6":"auto",                                  # IPv6: try DHCPv6 then SLAAC
    "masq6":"auto",                                  # NAT66 only when no PD to LAN
    "keeplive":                                      # keeplive mechanism configure save here
    {
        "type":"icmp",                               # use ICMP to keeplive
        "icmp":
        {
            "dest":                                             # ping the 8.8.8.8 and 114.114.114.114
            {
                "test":"8.8.8.8",
                "test2":"114.114.114.114"
            },
            "timeout":"10",                                     # The timeout exceeded 10 seconds for 5 consecutive times, the link is considered unavailable
            "failed":"5",
            "interval":"5"
        }
    }
}
```

Example, IPv4 DHCP client
```shell
ifname@wan
{
    "mode":"dhcpc",
    "masq":"enable"
}
```

Example, IPv4 DHCP client with custom DNS
```shell
ifname@wan
{
    "mode":"dhcpc",
    "dhcpc":
    {
        "custom_dns":"enable",
        "dns":"8.8.8.8",
        "dns2":"1.1.1.1"
    },
    "masq":"enable"
}
```

Example, IPv4 DHCP client with a static address before lease
```shell
ifname@wan
{
    "mode":"dhcpc",
    "dhcpc":
    {
        "static":"enable",
        "routeopt":"enable"
    },
    "masq":"enable"
}
```

Example, IPv4 static
```shell
ifname@wan
{
    "mode":"static",
    "static":
    {
        "ip":"192.168.10.2",
        "mask":"255.255.255.0",
        "gw":"192.168.10.1",
        "dns":"8.8.8.8",
        "dns2":"1.1.1.1"
    },
    "masq":"enable"
}
```

Example, IPv4 PPPoE
```shell
ifname@wan
{
    "mode":"pppoec",
    "pppoec":
    {
        "username":"1923221@gd.com",
        "password":"FDAED13E"
    },
    "masq":"enable"
}
```

Example, IPv4 PPPoE with service name and custom DNS
```shell
ifname@wan
{
    "mode":"pppoec",
    "pppoec":
    {
        "username":"1923221@gd.com",
        "password":"FDAED13E",
        "service":"isp",
        "custom_dns":"enable",
        "dns":"8.8.8.8",
        "dns2":"1.1.1.1"
    },
    "masq":"enable",
    "mtu":"1492"
}
```

Example, IPv4 without NAT masquerade
```shell
ifname@wan
{
    "mode":"dhcpc",
    "masq":"disable"
}
```

Example, IPv6 disable
```shell
ifname@wan
{
    "mode6":"disable"
}
```

Example, IPv6 factory path (try DHCPv6 then SLAAC, NAT66 only without PD)
```shell
ifname@wan
{
    "mode6":"auto",
    "masq6":"auto"
}
```

Example, IPv6 auto with a dedicated IPv6 MTU
```shell
ifname@wan
{
    "mode6":"auto",
    "masq6":"auto",
    "mtu6":"1280"
}
```

Example, IPv6 DHCPv6 with prefix delegation
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",                             # try IA_NA, do not fail if none
        "pdlen":"auto",                              # request IA_PD
        "ra":"enable"
    },
    "masq6":"auto"                                   # no NAT66 when PD is delegated to LAN
}
```

Example, IPv6 DHCPv6 request a /56 PD
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",
        "pdlen":"56",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 request a /64 PD only
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",
        "pdlen":"64",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 address only (no IA_PD)
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",
        "pdlen":"disable",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 PD only (no IA_NA)
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"none",
        "pdlen":"auto",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 must get IA_PD to succeed
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"force",
        "pdlen":"auto",
        "need_pd":"enable",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 with custom DNS
```shell
ifname@wan
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",
        "pdlen":"auto",
        "custom_dns":"enable",
        "dns":"2001:4860:4860::8888",
        "dns2":"2606:4700:4700::1111"
    },
    "masq6":"auto"
}
```

Example, IPv6 SLAAC only with NAT66
```shell
ifname@wan
{
    "mode6":"slaac",
    "masq6":"enable"
}
```

Example, IPv6 SLAAC share the on-link /64 with LAN (relay, no NAT66)
```shell
ifname@wan
{
    "mode6":"slaac",
    "masq6":"disable"
}
```

Example, IPv6 SLAAC with custom DNS
```shell
ifname@wan
{
    "mode6":"slaac",
    "slaac":
    {
        "custom_dns":"enable",
        "dns":"2001:4860:4860::8888",
        "dns2":"2606:4700:4700::1111"
    },
    "masq6":"auto"
}
```

Example, IPv6 static WAN address
```shell
ifname@wan
{
    "mode6":"static6",
    "static6":
    {
        "addr":"2001:db8:1::2",
        "mask":"64",
        "gw":"2001:db8:1::1",
        "dns":"2001:4860:4860::8888",
        "dns2":"2606:4700:4700::1111"
    },
    "masq6":"enable"
}
```

Example, IPv6 6in4 tunnel
```shell
ifname@wan
{
    "mode6":"6in4",
    "6in4":
    {
        "peer":"216.66.80.90",
        "addr":"2001:470:1f0a:1::2",
        "prefix":"2001:470:1f0b:1::/64",
        "mtu":"1480"
    },
    "masq6":"auto"
}
```

Example, IPv6 6in4 tunnel with broker update
```shell
ifname@wan
{
    "mode6":"6in4",
    "6in4":
    {
        "peer":"216.66.80.90",
        "addr":"2001:470:1f0a:1::2",
        "prefix":"2001:470:1f0b::/48",
        "mtu":"1480",
        "tunnel_id":"123456",
        "username":"user",
        "update_key":"secret"
    },
    "masq6":"auto"
}
```

Example, IPv6 6rd
```shell
ifname@wan
{
    "mode6":"6rd",
    "6rd":
    {
        "peer":"192.0.2.1",
        "prefix":"2001:db8::",
        "prefix_len":"32",
        "ip4_mask":"0",
        "mtu":"1480"
    },
    "masq6":"auto"
}
```

Example, IPv6 force NAT66 on this WAN (backup uplink)
```shell
ifname@wan
{
    "mode6":"auto",
    "masq6":"enable"
}
```

Example, IPv6 ICMP keeplive destinations
```shell
ifname@wan
{
    "keeplive":
    {
        "type":"icmp",
        "icmp":
        {
            "dest":
            {
                "test":"8.8.8.8"
            },
            "dest6":
            {
                "test":"2001:4860:4860::8888",
                "test2":"2606:4700:4700::1111"
            }
        }
    }
}
```

#### Configuration settings example

Example, modify the keeplive to icmp for first WAN network
```shell
ifname@wan:keeplive/type=icmp
ttrue
```

Example, modify the first WAN dial mode to DHCP
```shell
ifname@wan:mode=dhcpc
ttrue
```

Example, modify the first WAN pppoec username and password
```shell
ifname@wan:pppoec/username=dimmalex@ashyelf.com
ttrue
```

Example, merge set the first WAN configure( include "mode" "masq" )
```shell
ifname@wan|{"mode":"dhcpc","masq":"enable"}
ttrue
```

Example, merge set IPv6 mode and NAT66 for the first WAN
```shell
ifname@wan|{"mode6":"auto","masq6":"auto"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the WAN network**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the WAN network**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get WAN network information**
    - failed return NULL
    - succeed return [ json ], WAN network status information
    ```json
    {
        "status":"Current state",        // [ "nodevice", "uping", "block", "up", "failed", "down" ]
                                             // "nodevice" means the underlying device is not present
                                             // "uping" for connecting
                                             // "block" means waiting for keeplive checks to recover
                                             // "up" means network is connected
                                             // "failed" for keeplive failed
                                             // "down" for the ifname is down
        "mode":"IPV4 address mode",     // [ "dhcpc", "static", "pppoec" ]
        "netdev":"netdev name",         // [ string ]
        "ifdev":"ifdev name",           // [ string ], Optional
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "delay":"delay time",           // [ "failed", "block", number ], Optional, "failed" for network test failed, "block" for testing
        "ontime":"online uptime",       // [ string ], Optional, online system uptime
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"received bytes",    // [ number ]
        "rx_packets":"received packets",// [ number ]
        "tx_bytes":"sent bytes",        // [ number ]
        "tx_packets":"sent packets",    // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "mode6":"IPv6 address mode",    // [ "disable", "auto", "static6", "dhcpc6", "slaac", "6in4", "6rd" ], Optional, present when IPv6 is enabled
        "addr":"IPv6 address/prefixlen",  // [ string ], Optional, CIDR on netdev, e.g. "2001:db8::1/64"
        "addr2":"IPv6 address2/prefixlen", // [ string ], Optional
        "addr3":"IPv6 address3/prefixlen", // [ string ], Optional
        "gw6":"IPv6 gateway",           // [ ipv6 address ], Optional
        "dns6":"IPv6 DNS",              // [ ipv6 address ], Optional
        "dns62":"IPv6 DNS2",            // [ ipv6 address ], Optional
        "prefix":"delegated IPv6 prefix" // [ string ], Optional, LAN CIDR from IA_PD / 6in4 / 6rd; omitted when empty (not from static6)
    }
    ```

    Example, get the first WAN network information
    ```shell
    ifname@wan.status
    {
        "status":"up",                     # connect is succeed
        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"wan",                    # netdev is wan
        "gw":"192.168.10.254",             # gateway is 192.168.10.254
        "dns":"114.114.114.114",           # dns is 114.114.114.114
        "dns2":"221.5.88.88",              # backup dns is 221.5.88.88
        "ip":"192.168.10.1",               # ip address is 192.168.10.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "mode6":"auto",                    # IPv6 address mode is auto
        "addr":"fe80::50:f4ff:fe00:0/64"   # local IPv6 address is fe80::50:f4ff:fe00:0/64
    }
    ```

+ `netdev[]` **get the WAN netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the first WAN network netdev
    ```shell
    ifname@wan.netdev
    wan
    ```

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - succeed return [ string ], the ifdev component name

    Example, get the first WAN network ifdev
    ```shell
    ifname@wan.ifdev
    vlan@wan
    ```


#### Other

+ `keepon[]` **clear the connect failed counter**
    - succeed return ttrue
    - called when network connection is confirmed alive
    - resets the internal connect_failed counter to prevent unnecessary device reset

    Example, clear the connect failed counter for first WAN network
    ```shell
    ifname@wan.keepon
    ttrue
    ```

+ `keepoff[]` **handle keeplive check failure**
    - succeed return ttrue
    - performs configured action when keeplive check fails
    - action depends on keeplive/action configuration:
        - "reboot": reboot the system (if uptime > 180s)
        - "reset": reset the interface device
        - others: reset the connection

    Example, handle keeplive failure for first WAN network
    ```shell
    ifname@wan.keepoff
    ttrue
    ```
