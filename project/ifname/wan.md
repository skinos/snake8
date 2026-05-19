## ifname@wan — WAN Network Management

### Overview

Manage WAN networks. This component depends on WAN-facing interfaces (often **`arch`** `ethernet`) and the **network** project (`network@frame`, `skinnet`, multi-link **`connect`** — see [`../network/frame.md`](../network/frame.md)).
Usually `ifname@wan` is the first WAN network. If there are multiple WANs, `ifname@wan2` is the second WAN network, and numbering increases sequentially.

- manages WAN interface lifecycle: setup, shutdown, status query
- supports static, DHCP client, and PPPoE dial IPv4 addressing
- supports static, DHCPv6, and SLAAC IPv6 addressing
- provides keeplive mechanism with ICMP, DNS, and receive packet detection
- NAT masquerade for outgoing traffic



### Configuration reference ( ifname@wan )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ], enable means auto-setup after boot

    // MAC
    "mac":"set MAC address for interface",                       // [ mac address ]

    // IPv4
    "tid":"table identify number",                               // [ number ], policy route table ID, mainly used in multi-WAN
    "metric":"default route metric",                             // [ number ]
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

    // IPv6
    "method":"IPv6 address mode",             // [ "disable", "manual", "automatic", "slaac" ]
                                                    // "disable" means IPv6 is disabled
                                                    // "manual" means static IPv6 settings
                                                    // "automatic" means DHCPv6
                                                    // "slaac" means Stateless Address Autoconfiguration
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
        "mode":"mode for get the ipv6",                  // [ "try", "force", "disable" ]
        "prefix":"ipv6-prefix of length for request",    // [ "auto", "48", "52", "56", "60", "60", "disable" ]
        "custom_resolve":"Custom DNS",                   // [ "disable", "enable" ]
        "resolve":"Custom DNS1",                         // [ ipv6 address ], valid when "custom_resolve" is "enable"
        "resolve2":"Custom DNS2"                         // [ ipv6 address ], valid when "custom_resolve" is "enable"
    },
    "masquerade":"outgoing NAT for IPv6",                                                 // [ "disable", "enable" ]

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ "disable", "icmp", "dns", "recv", "auto" ]
                                      // "disable" for disable the keeplive
                                      // "icmp" for ping keeplive
                                      // "dns" for test the dns response
                                      // "recv" for count receive packet to keeplive
                                      // "auto" for count receive packet to keeplive when test the dns response failed
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
        "recv":                                                  // detail configuration for "type" is "recv"
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
    "method":"slaac",                                # IPv6 address mode is slaac
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
        "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, present when IPv6 is enabled
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr3":"IPv6 address3"         // [ ipv6 address ], Optional, exist when IPV6 enable
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
        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0"      # local IPv6 address is fe80::50:f4ff:fe00:0
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
