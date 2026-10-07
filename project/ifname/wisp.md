## ifname@wisp — WISP Network Management

### Overview

Manage WISP (Wireless ISP) networks. This component depends on a wireless station interface (**`wifi`** `sta` / **`arch`** radio BSP) and the **network** project ([`../network/frame.md`](../network/frame.md)).
Usually ifname@wisp is the first WISP (2.4G) network. If there are multiple WISP networks in the system, ifname@wisp2 is the second WISP (5.8G) network, and numbering increases sequentially.

- manages WISP interface lifecycle: setup, shutdown, status query
- supports static, DHCP client, and PPPoE dial IPv4 addressing
- supports static, DHCPv6, and SLAAC IPv6 addressing (`mode6`); `6in4`/`6rd` keys are reserved (not implemented in this release)
- when the kernel has no IPv6 support, reading this ifname’s config omits IPv6 keys (`mode6`, `static6`, `dhcpc6`, `slaac`, `masq6`, `mtu6`, …); IPv6 runtime stays off
- provides wireless AP scanning, channel listing, and security mode listing
- keeplive mechanism with ICMP, DNS, and receive packet detection
- NAT masquerade for outgoing traffic



### Network Architecture

`ifname@wisp` is an **extern interface** registered by `network@frame` during boot. It uses `ifname@ethcon` as concom and `wifi@nsta` (or `wifi@asta` for 5.8G) as ifdev — a wireless station connecting to a remote AP. As an extern interface, it is subject to multi-uplink scheduling and applies NAT masquerade. It publishes `network/onextern` / `network/offextern` joint events on state changes.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( ifname@wisp )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ]

    // MAC
    "mac":"set MAC address for interface",                       // [ mac address ]

    // Wireless connect
    "peer":"SSID to connect",                                    // [ string ]
    "peermac":"BSSID to connect",                                // [ mac address ]
    "peermode":"mode of connection",                             // [ "hidden" ], peer AP does not broadcast SSID; channel must be set in hidden mode
    "channel":"wireless channel",                                // [ number ], 0-165, 0 for auto
    "nossid":"disable the ssid",                                 // [ "disable", "enable" ], disable the local ssid when connected
    "secure":"mode of security",                                 // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                      // "disable" for no security
                                                                      // "wpapsk" for WPAPSK
                                                                      // "wpa2psk" for WPA2PSK
                                                                      // "wpapskwpa2psk" for WPA Mix
    "wpa_encrypt":"WAP encrypt",                                 // [ "aes", "tkip", "tkipaes" ]
                                                                      // "aes" for AES
                                                                      // "tkip" for TKIP
                                                                      // "tkipaes" for auto
    "wpa_key":"WPA key",                                         // [ string ], minimum 8 characters, mandatory if secure is wpapsk/wpa2psk/wpapskwpa2psk

    // IPv4
    "tid":"table identify number",                               // [ number ], exclusive route table ID, only for multiple WAN
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
        "mask":"IPv6 subnet length of addr",        // [ number ], default 64; this ifname only (WISP on-link length)
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

    // Configure connect failed to action
    "failed_threshold":"first failed to reset time",                                   // [ number ]
    "failed_threshold2":"second failed to reset time",                                 // [ number ]
    "failed_threshold3":"third failed to reset time",                                  // [ number ]
    "failed_everytime":"every failed to reset time"                                    // [ number ]
}
```

#### Configuration example

Example, show all configuration of first WISP (2.4G)
```shell
ifname@wisp
{
    "peer":"V520-D21D20",    # connect V520-D21D20
    "secure":"wpapsk",       # security is WPAPSK
    "wpa_encrypt":"aes",     # encrypt use AES
    "wpa_key":"87654321",    # password 87654321
    "mode":"dhcpc",                                  # mode is DHCP client
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
ifname@wisp
{
    "mode":"dhcpc",
    "masq":"enable"
}
```

Example, IPv4 static
```shell
ifname@wisp
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
ifname@wisp
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

Example, IPv6 factory path (try DHCPv6 then SLAAC, NAT66 only without PD)
```shell
ifname@wisp
{
    "mode6":"auto",
    "masq6":"auto"
}
```

Example, IPv6 DHCPv6 with prefix delegation
```shell
ifname@wisp
{
    "mode6":"dhcpc6",
    "dhcpc6":
    {
        "request":"try",
        "pdlen":"auto",
        "ra":"enable"
    },
    "masq6":"auto"
}
```

Example, IPv6 SLAAC only with NAT66
```shell
ifname@wisp
{
    "mode6":"slaac",
    "masq6":"enable"
}
```

Example, IPv6 SLAAC share the on-link /64 with LAN (relay, no NAT66)
```shell
ifname@wisp
{
    "mode6":"slaac",
    "masq6":"disable"
}
```

Example, IPv6 static address
```shell
ifname@wisp
{
    "mode6":"static6",
    "static6":
    {
        "addr":"2001:db8:1::2",
        "mask":"64",
        "gw":"2001:db8:1::1",
        "dns":"2001:4860:4860::8888"
    },
    "masq6":"disable"
}
```

#### Configuration settings example

Example, modify the keeplive to icmp for WISP(2.4G) network
```shell
ifname@wisp:keeplive/type=icmp
ttrue
```

Example, modify the WISP(2.4G) network dial mode to DHCP
```shell
ifname@wisp:mode=dhcpc
ttrue
```

Example, modify SSID and security for the first WISP (2.4G) connection
```shell
ifname@wisp:peer=Myhotpot
ttrue
```

Example, merge set the first WISP configure( include "peer" "secure" "wpa_key" )
```shell
ifname@wisp|{"peer":"Myhotpot","secure":"wpapsk","wpa_key":"88888888"}
ttrue
```

Example, merge set IPv6 mode and NAT66 for the first WISP
```shell
ifname@wisp|{"mode6":"auto","masq6":"auto"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the WISP network**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the WISP network**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get WISP information**
    - failed return NULL
    - succeed return [ json ], WISP network status information
    ```json
    {
        "status":"Current state",        // [ "uping", "scanning", "block", "up", "failed", "down" ]
                                             // "uping" for connecting
                                             // "scanning" for scanning the AP
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
        "prefix":"delegated IPv6 prefix", // [ string ], Optional, LAN CIDR from IA_PD / 6in4 / 6rd; omitted when empty (not from static6)
        "peer":"Peer SSID",             // [ string ]
        "peermac":"Peer BSSID",         // [ mac address ]
        "channel":"Peer channel",       // [ number ], 0-165
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], 0 is no signal, 1 is weakest, 4 is strongest
        "rate":"connect rate",          // [ number ], Optional, the unit is M
        "rssi":"Peer RSSI",             // [ number ], Optional, the unit is dBm
        "rssp":"Peer signal percentage" // [ number ], Optional, the unit is %
    }
    ```

    Example, get the first WISP network information
    ```shell
    ifname@wisp.status
    {
        "status":"up",                     # connect is succeed
        "mode":"dhcpc",                    # IPv4 connect mode is DHCP
        "netdev":"ath11",                  # netdev is ath11
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
        "addr":"fe80::50:f4ff:fe00:0/64",  # local IPv6 address is fe80::50:f4ff:fe00:0/64
        "peer":"TP-link-2231",            # peer is TP-link-2231
        "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
        "channel":"10",                   # channel is 10
        "rate":"270",                     # rate is 270M
        "rssi":"-41",                     # rssi is -41dBm
        "signal":"3"                      # signal level is 3
    }
    ```

+ `netdev[]` **get the WISP netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the first WISP netdev
    ```shell
    ifname@wisp.netdev
    ath11
    ```

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - succeed return [ string ], the ifdev component name

    Example, get the first WISP network ifdev
    ```shell
    ifname@wisp.ifdev
    wifi@nsta
    ```

+ `chlist[]` **get the WISP channel list**
    - failed return NULL
    - succeed return [ json ], available wireless channels
    ```json
    {
        "channel number":{}       // [ number ]:{}
        // "...":{}  How many channels show how many properties
    }
    ```

    Example, get the first WISP channel list
    ```shell
    ifname@wisp.chlist
    {
        "1":{},        # channel 1
        "2":{},        # channel 2
        "3":{},        # channel 3
        "4":{},        # channel 4
        "5":{},        # channel 5
        "6":{},        # channel 6
        "7":{},        # channel 7
        "8":{},        # channel 8
        "9":{},        # channel 9
        "10":{},       # channel 10
        "11":{}        # channel 11
    }
    ```

+ `securelist[]` **get supported security modes**
    - failed return NULL
    - succeed return [ json ], supported security modes
    ```json
    {
        "secure mode":{}       // [ string ]:{}
        // "...":{}  How many modes show how many properties
    }
    ```

    Example, get supported security modes of first WISP
    ```shell
    ifname@wisp.securelist
    {
        "disable":{},
        "wpapsk":{},
        "wpa2psk":{},
        "wpapskwpa2psk":{}
    }
    ```

+ `aplist[]` **scan surrounding APs with WISP**
    - failed return NULL
    - succeed return [ json ], scanned AP information
    ```json
    {
        "AP BSSID":                       // [ mac address ]
        {
            "ssid":"SSID name",               // [ string ]
            "channel":"channel number",       // [ number ], 0-165, 0 for auto
            "secure":"mode of security",      // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
            "wpa_encrypt":"WAP encrypt",      // [ "aes", "tkip", "tkipaes" ]
            "sig":"signal level(%)",          // [ number ]
            "signal":"signal level[0-4]",     // [ "0", "1", "2", "3", "4" ]
            "chext":"extern channel",         // [ "none", "below", "above" ]
            "mode":"wireless system"          // [ string ]
        }
        // "...":{}  How many APs show how many properties
    }
    ```

    Example, get the surrounding AP from first WISP scan
    ```shell
    ifname@wisp.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # first AP SSID
            "channel":"6",                                 # first AP channel
            "secure":"wpapskwpa2psk",                      # secure mode is WPA mix
            "wpa_encrypt":"aes",                           # encrypt type is AES
            "sig":"70",                                    # signal is 70%
            "signal":"3",                                  # signal level is 3, range is 0-4
            "chext":"below",                               # extern channel is below
            "mode":"11b/g/n"
        },
        "B4:82:C5:80:22:41":                    # second AP by scanning
        {
            "ssid":"dimmalex-work",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11b/g/n"
        }
    }
    ```


#### Other

+ `keepon[]` **clear the connect failed counter**
    - succeed return ttrue
    - called when network connection is confirmed alive
    - resets the internal connect_failed counter to prevent unnecessary device reset

    Example, clear the connect failed counter for first WISP network
    ```shell
    ifname@wisp.keepon
    ttrue
    ```

+ `keepoff[]` **handle keeplive check failure**
    - succeed return ttrue
    - performs configured action when keeplive check fails
    - action depends on keeplive/action configuration:
        - "reboot": reboot the system (if uptime > 180s)
        - "reset": reset the interface device
        - others: reset the connection

    Example, handle keeplive failure for first WISP network
    ```shell
    ifname@wisp.keepoff
    ttrue
    ```
