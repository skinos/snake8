## vpn@l2tp — L2TP Client Instance Management

### Overview

Manage an individual L2TP VPN client connection. Each instance (`vpn@l2tp`, `vpn@l2tp2`, …) connects to an L2TP server using `openl2tpd` with PPP, and integrates with the network framework as a VPN-type extern interface. Instances are created and managed by [`vpn@l2tplist`](l2tplist.md).

- manages L2TP connection lifecycle: setup, shutdown, online/offline
- resolves server domain to IP and routes via the specified extern interface
- supports CHAP and simple authentication modes
- configures PPP options: MTU, MSS, LCP echo, DNS, custom IP, raw options
- handles NAT masquerade, default route, and custom route tables
- monitors connection state and flow statistics



### Network Architecture

`vpn@l2tp` is a **VPN extern interface** registered by `vpn@l2tplist` with `network@frame`. It uses a specific extern interface (e.g. `ifname@wan`) or the default gateway as its underlying transport. When the L2TP tunnel comes up, it notifies `network@frame.online`, which triggers VPN routing and multi-link scheduling updates.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).

### Configuration reference ( vpn@l2tp )

```json
// Attributes introduction 
{
    "status":"client status",                                  // [ "disable", "enable" ]
    "extern":"extern ifname dependency",                       // [ "disable", "default", "ifname@wan", "ifname@lte", ... ]
                                                                    // "disable" for no extern dependency
                                                                    // "default" to use the system default gateway
                                                                    // "ifname@wan", "ifname@lte", ... for a specific extern interface

    "server":"L2TP server address",                            // [ string ], IP address or domain name
    "port":"L2TP server port",                                 // [ number ], default 1701

    // Authentication
    "username":"PPP username",                                 // [ string ]
    "password":"PPP password",                                 // [ string ]
    "authmode":"authentication type",                          // [ "disable", "simple", "chap" ]
    "secret":"tunnel secret key",                              // [ string ], valid when authmode is "chap"

    // PPP options
    "ppp":                                 // PPP configuration
    {
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
        "pppopt":"raw PPP options"                 // [ string ], semicolon-separated PPP options
    },

    // Routing
    "masq":"NAT masquerade",                                   // [ "disable", "enable" ]
    "defaultroute":"set as default route",                     // [ "disable", "enable" ]
    "metric":"route metric",                                   // [ number ], optional
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

Example, show all L2TP client configuration
```shell
vpn@l2tp
{
    "status":"enable",
    "extern":"default",
    "server":"l2tp.example.com",
    "port":"1701",
    "username":"vpnuser",
    "password":"vpnpass",
    "authmode":"chap",
    "secret":"tunnelsecret",
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

Example, enable the L2TP client
```shell
vpn@l2tp:status=enable
ttrue
```

Example, change the L2TP server
```shell
vpn@l2tp:server=new-l2tp.example.com
ttrue
```

Example, merge set L2TP configure( include "server" "username" "password" )
```shell
vpn@l2tp|{"server":"l2tp.example.com","username":"user","password":"pass"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the L2TP client service**
    - succeed return ttrue
    - only starts if status is "enable"
    - launches the openl2tpd service subprocess

+ `shut[]` **shut down the L2TP client**
    - succeed return ttrue
    - notifies `network@frame.offline`, stops the service, clears connect_failed counter


#### Query APIs

+ `status[]` **get L2TP client status**
    - failed return NULL
    - succeed return [ json ], connection status and statistics
    ```json
    {
        "status":"Current state",        // [ "disable", "uping", "down", "up" ]
                                             // "disable" client is disabled
                                             // "uping" L2TP is connecting
                                             // "down" L2TP is down
                                             // "up" L2TP tunnel is established
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

    Example, get the first L2TP client status
    ```shell
    vpn@l2tp.status
    {
        "status":"up",
        "netdev":"ppp0",
        "ip":"10.0.0.2",
        "dstip":"10.0.0.1",
        "mask":"255.255.255.255",
        "serverip":"198.51.100.1",
        "dns":"8.8.8.8",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

+ `netdev[]` **get the L2TP netdev name**
    - failed return NULL
    - succeed return [ string ], the PPP netdev name (e.g. "ppp0")

    Example, get the first L2TP client netdev
    ```shell
    vpn@l2tp.netdev
    ppp0
    ```


#### Control APIs

+ `reset[]` **restart the L2TP client**
    - succeed return ttrue
    - behavior depends on the `extern` setting:
        - "default": restarts immediately
        - specific ifname: restarts only when the specified extern interface comes online
