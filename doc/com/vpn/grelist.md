***
## Management of GRE tunnel
Management of GRE tunnel

#### Configuration( vpn@gre )
**vpn@gre** is first GRE tunnel
**vpn@gre2** is second GRE tunnel

```json
// Attributes introduction 
{
    // common attributes
    "status":"client status",                    // [ disable, enable ]
    "extern":"extern ifname",                      // [ "disable", "", "ifname@wan", "ifname@lte", ... ],
                                                                // "disable" or space for none
                                                                // "" for is defdault gateway
                                                                // "ifname@wan", "ifname@lte", ... for specified extern interface

    "peer":"gre tunnel peer address",            // [ string ]

    "localip":"gre tunnel local address",        // [ ip address ]
    "remoteip":"gre tunnel remote address",      // [ ip address ]
    "ttl":"gre tunnel ttl",                      // [ number ]
    "mtu":"gre tunnel MTU",                      // [ number ]

    // route attributes
    "masq":"share interface address to access",    // [ disable, enable ]
    "defaultroute":"set it default route",         // [ disable, enable ]
    "route_table":                                 // you can custom the route rule on this connect, vaild when "defaultroute" is "disable"
    {
        "route rule name":                         // [ string ]
        {
            "target":"destination address",           // [ string ], ip address or network
            "mask":"destination network mask"      // [ string ]
        }
        // ...more route rule
    },
    "custom_dns":"custom the dns server",        // [ disable, enable ]
    "dns":"dns address",                         // [ ip address ], vaild when "custom_dns" be "enable"
    "dns2":"backup dns address"                  // [ ip address ], vaild when "custom_dns" be "enable"
}

```


#### **Methods**

+ `setup[]` **setup the gre tunnel**, *succeed return ttrue, failed return tfalse, error return terror*

+ `shut[]` **shutdown the gre tunnel**, *succeed return ttrue, failed return tfalse, error return terror*

+ `status[]` **get the gre tunnel infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current status",        // [ uping, down, up ]
                                             // uping for connecting
                                             // down for the network is down
                                             // up for the network is connect succeed
        "netdev":"netdev name",         // [ string ]
        "gw":"gateway ip address",      // [ ip address ]
        "dns":"dns ip address",         // [ ip address ]
        "dns2":"dns2 ip address",       // [ ip address ]
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "livetime":"online time",       // hour:minute:second:day
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
    }
    ```
    ```shell
    # examples, get the first gre tunnel infomation
    vpn@gre.status
    {
        "status":"up",                     # connect is succeed
        "netdev":"gre0",                   # netdev is gre0
        "ip":"192.168.10.1",               # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "gw":"192.168.10.254",             # gateway is 192.168.10.254
        "dns":"114.114.114.114",           # dns is 114.114.114.114
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4"                   # send 4 packets
    }
    ```

+ `netdev[]` **get the gre tunnel netdev**, *succeed return netdev, failed return NULL, error return terror*
    ```shell
    # examples, get the first gre tunnel netdev
    vpn@gre.netdev
    gre0
    ```
