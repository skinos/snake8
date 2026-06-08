## vpn@gre — GRE Tunnel Instance Management

### Overview

Manage an individual GRE (Generic Routing Encapsulation) tunnel. Each instance (`vpn@gre`, `vpn@gre2`, …) creates an IP-over-IP tunnel to a remote peer, and integrates with the network framework as a VPN-type extern interface. Instances are created and managed by [`vpn@grelist`](grelist.md).

- manages GRE tunnel lifecycle: setup (ip tunnel add), shutdown (ip tunnel del), online/offline
- resolves peer domain to IP and routes via the specified extern interface
- configures tunnel parameters: TTL, MTU, local/remote tunnel IPs
- handles NAT masquerade, default route, and custom route tables
- monitors connection state and flow statistics



### Network Architecture

`vpn@gre` is a **VPN extern interface** registered by `vpn@grelist` with `network@frame`. It uses a specific extern interface (e.g. `ifname@wan`) or the default gateway as its underlying transport. When the GRE tunnel comes up, it notifies `network@frame.online`, which triggers VPN routing and multi-link scheduling updates.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).

### Configuration reference ( vpn@gre )

```json
// Attributes introduction 
{
    "status":"tunnel status",                                  // [ "disable", "enable" ]
    "extern":"extern ifname dependency",                       // [ "disable", "default", "ifname@wan", "ifname@lte", ... ]
                                                                    // "disable" for no extern dependency
                                                                    // "default" to use the system default gateway
                                                                    // "ifname@wan", "ifname@lte", ... for a specific extern interface

    "peer":"GRE tunnel peer address",                          // [ string ], IP address or domain name
    "localip":"GRE tunnel local address",                      // [ ip address ], local tunnel endpoint IP
    "remoteip":"GRE tunnel remote address",                    // [ ip address ], remote tunnel endpoint IP
    "ttl":"GRE tunnel TTL",                                    // [ number ], default 255
    "mtu":"GRE tunnel MTU",                                    // [ number ], optional

    // DNS
    "custom_dns":"custom DNS",                                 // [ "disable", "enable" ]
    "dns":"DNS server",                                        // [ ip address ], valid when custom_dns is "enable"
    "dns2":"backup DNS server",                                // [ ip address ], valid when custom_dns is "enable"
    "domain":"DNS search domain",                              // [ string ], optional

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

Example, show all GRE tunnel configuration
```shell
vpn@gre
{
    "status":"enable",
    "extern":"default",
    "peer":"203.0.113.1",
    "localip":"10.0.0.1",
    "remoteip":"10.0.0.2",
    "ttl":"255",
    "masq":"enable",
    "defaultroute":"enable"
}
```

#### Configuration settings example

Example, enable the GRE tunnel
```shell
vpn@gre:status=enable
ttrue
```

Example, change the GRE peer
```shell
vpn@gre:peer=new-peer.example.com
ttrue
```

Example, merge set GRE configure( include "peer" "localip" "remoteip" )
```shell
vpn@gre|{"peer":"203.0.113.1","localip":"10.0.0.1","remoteip":"10.0.0.2"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the GRE tunnel**
    - succeed return ttrue
    - only starts if status is "enable"
    - resolves peer to IP, creates ip tunnel, sets up routing, notifies network@frame.online

+ `shut[]` **shut down the GRE tunnel**
    - succeed return ttrue
    - notifies `network@frame.offline`, tears down the ip tunnel (ip tunnel del)


#### Query APIs

+ `status[]` **get GRE tunnel status**
    - failed return NULL
    - succeed return [ json ], connection status and statistics
    ```json
    {
        "status":"Current state",        // [ "disable", "uping", "down", "up" ]
                                             // "disable" tunnel is disabled
                                             // "uping" tunnel is connecting
                                             // "down" tunnel is down
                                             // "up" tunnel is established
        "netdev":"netdev name",          // [ string ], e.g. "gre", "gre2"
        "serverip":"peer IP",            // [ ip address ], resolved peer IP
        "ip":"IP address",               // [ ip address ], local tunnel IP
        "dstip":"destination IP",        // [ ip address ], remote tunnel IP
        "mask":"network mask",           // [ ip address ]
        "gw":"gateway IP",               // [ ip address ], Optional
        "dns":"DNS server",              // [ ip address ], Optional
        "dns2":"backup DNS",             // [ ip address ], Optional
        "livetime":"online time",        // [ string ], format hour:minute:second:day
        "rx_bytes":"received bytes",     // [ string ]
        "rx_packets":"received packets", // [ string ]
        "tx_bytes":"sent bytes",         // [ string ]
        "tx_packets":"sent packets"      // [ string ]
    }
    ```

    Example, get the first GRE tunnel status
    ```shell
    vpn@gre.status
    {
        "status":"up",
        "netdev":"gre",
        "ip":"10.0.0.1",
        "dstip":"10.0.0.2",
        "mask":"255.255.255.255",
        "serverip":"203.0.113.1",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

+ `netdev[]` **get the GRE netdev name**
    - failed return NULL
    - succeed return [ string ], the GRE netdev name (e.g. "gre", "gre2")

    Example, get the first GRE tunnel netdev
    ```shell
    vpn@gre.netdev
    gre
    ```


#### Control APIs

+ `reset[]` **restart the GRE tunnel**
    - succeed return ttrue
    - behavior depends on the `extern` setting:
        - "default": restarts immediately
        - specific ifname: restarts only when the specified extern interface comes online
