## vpn@pptp — PPTP Client Instance Management

### Overview

Manage an individual PPTP VPN client connection. Each instance (`vpn@pptp`, `vpn@pptp2`, …) connects to a PPTP server using PPP, supports MPPE encryption, and integrates with the network framework as a VPN-type extern interface. Instances are created and managed by [`vpn@pptplist`](pptplist.md).

- manages PPTP connection lifecycle: setup, shutdown, online/offline
- resolves server domain to IP and routes via the specified extern interface
- supports MPPE encryption (required or optional)
- configures PPP options: MTU, MSS, LCP echo, DNS, custom IP, raw options
- handles NAT masquerade, default route, and custom route tables
- monitors connection state and flow statistics



### Network Architecture

`vpn@pptp` is a **VPN extern interface** registered by `vpn@pptplist` with `network@frame`. It uses a specific extern interface (e.g. `ifname@wan`) or the default gateway as its underlying transport. When the PPTP tunnel comes up, it notifies `network@frame.online`, which triggers VPN routing and multi-link scheduling updates.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).

### Configuration reference ( vpn@pptp )

```json
// Attributes introduction 
{
    "status":"client status",                                  // [ "disable", "enable" ]
    "extern":"extern ifname dependency",                       // [ "disable", "default", "ifname@wan", "ifname@lte", ... ]
                                                                    // "disable" for no extern dependency
                                                                    // "default" to use the system default gateway
                                                                    // "ifname@wan", "ifname@lte", ... for a specific extern interface

    "server":"PPTP server address",                            // [ string ], IP address or domain name
    "port":"PPTP server port",                                 // [ number ], default 1723

    // Security
    "require_mppe":"require MPPE encryption",                  // [ "disable", "enable" ]
    "mppe_stateful":"MPPE stateful mode",                      // [ "disable", "enable" ]

    // PPP credentials (can be placed here or inside ppp sub-object; ppp takes priority)
    "username":"PPP username",                                 // [ string ]
    "password":"PPP password",                                 // [ string ]

    // PPP options
    "ppp":                                 // PPP configuration
    {
        "username":"PPP username",                 // [ string ], optional, overrides top-level username
        "password":"PPP password",                 // [ string ], optional, overrides top-level password
        "mtu":"Maximum transmission unit",         // [ number ], the unit is bytes, default 1400
        "mss":"TCP Maximum Segment Size",          // [ number ], the unit is bytes
        "lcp_echo_interval":"LCP echo interval",   // [ number ], the unit is seconds
        "lcp_echo_failure":"LCP echo failure times",// [ number ]
        "custom_dns":"Custom DNS",                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                       // [ ip address ], valid when custom_dns is "enable"
        "dns2":"Custom DNS2",                      // [ ip address ], valid when custom_dns is "enable"
        "txqueuelen":"TX queue length",            // [ number ]
        "custom_ip":"custom PPP interface IP",     // [ "disable", "enable" ]
        "localip":"PPP local IP",                  // [ ip address ], valid when custom_ip is "enable"
        "remoteip":"PPP remote IP",                // [ ip address ], valid when custom_ip is "enable"
        "domain":"DNS search domain",              // [ string ], optional, written to resolv.conf
        "pppopt":"raw PPP options"                 // [ string ], semicolon-separated PPP options
    },

    // Routing
    "masq":"NAT masquerade",                                   // [ "disable", "enable" ]
    "defaultroute":"set as default route",                     // [ "disable", "enable" ]
    "mtu":"MTU override",                                      // [ number ], optional, overrides ppp.mtu for route clamping
    "metric":"route metric",                                   // [ number ], optional, route metric for this interface
    "route_table":                             // custom route rules, valid when defaultroute is "disable"
    {
        "route rule name":                     // [ string ]
        {
            "target":"destination address",        // [ string ], IP address or network
            "mask":"destination network mask"      // [ string ]
        }
        // "...":{}  How many routes show how many properties
    }
}
```

#### Configuration example

Example, show all PPTP client configuration
```shell
vpn@pptp
{
    "status":"enable",
    "extern":"default",
    "server":"pptp.example.com",
    "port":"1723",
    "require_mppe":"disable",
    "username":"vpnuser",
    "password":"vpnpass",
    "masq":"enable",
    "defaultroute":"enable",
    "ppp":
    {
        "mtu":"1400",
        "lcp_echo_interval":"10",
        "lcp_echo_failure":"6"
    }
}
```

#### Configuration settings example

Example, enable the PPTP client
```shell
vpn@pptp:status=enable
ttrue
```

Example, change the PPTP server
```shell
vpn@pptp:server=new-pptp.example.com
ttrue
```

Example, merge set PPTP configure( include "server" "username" "password" )
```shell
vpn@pptp|{"server":"pptp.example.com","username":"user","password":"pass"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the PPTP client service**
    - succeed return ttrue
    - only starts if status is "enable"
    - launches the pppd service subprocess

+ `shut[]` **shut down the PPTP client**
    - succeed return ttrue
    - notifies `network@frame.offline`, stops the service, clears connect_failed counter


#### Query APIs

+ `status[]` **get PPTP client status**
    - failed return NULL
    - succeed return [ json ], connection status and statistics
    ```json
    {
        "status":"Current state",        // [ "disable", "uping", "down", "up" ]
                                             // "disable" client is disabled
                                             // "uping" PPP is connecting
                                             // "down" PPP is down
                                             // "up" PPP tunnel is established
        "netdev":"netdev name",          // [ string ], e.g. "ppp0"
        "serverip":"server IP",          // [ ip address ], resolved server IP
        "ip":"IP address",               // [ ip address ], local tunnel IP
        "dstip":"destination IP",        // [ ip address ], remote tunnel IP
        "mask":"network mask",           // [ ip address ]
        "gw":"gateway IP",               // [ ip address ], Optional
        "dns":"DNS server",              // [ ip address ], Optional, present when connected
        "dns2":"backup DNS",             // [ ip address ], Optional, present when connected
        "livetime":"online time",        // [ string ], format hour:minute:second:day
        "rx_bytes":"received bytes",     // [ string ]
        "rx_packets":"received packets", // [ string ]
        "tx_bytes":"sent bytes",         // [ string ]
        "tx_packets":"sent packets"      // [ string ]
    }
    ```

    Example, get the first PPTP client status
    ```shell
    vpn@pptp.status
    {
        "status":"up",
        "netdev":"ppp0",
        "ip":"10.0.0.2",
        "dstip":"10.0.0.1",
        "mask":"255.255.255.255",
        "serverip":"203.0.113.1",
        "dns":"8.8.8.8",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

+ `netdev[]` **get the PPTP netdev name**
    - failed return NULL
    - succeed return [ string ], the PPP netdev name (e.g. "ppp0")

    Example, get the first PPTP client netdev
    ```shell
    vpn@pptp.netdev
    ppp0
    ```


#### Control APIs

+ `reset[]` **restart the PPTP client**
    - succeed return ttrue
    - behavior depends on the `extern` setting:
        - "default": restarts immediately
        - specific ifname: restarts only when the specified extern interface comes online
