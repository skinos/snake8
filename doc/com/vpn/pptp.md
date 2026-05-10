***
## Management of PPTP Client
Management of PPTP client

#### Configuration( vpn@pptp )
**vpn@pptp** is first pptp client
**vpn@pptp2** is second pptp client

```json
// Attributes introduction 
{
    // common attributes
    "status":"client status",                      // [ disable, enable ]
    "extern":"extern ifname",                      // [ "disable", "", "ifname@wan", "ifname@lte", ... ],
                                                                // "disable" or space for none
                                                                // "" for is defdault gateway
                                                                // "ifname@wan", "ifname@lte", ... for specified extern interface

    "server":"pptp server address",                // [ string ]

    // seucre attributes
    "require_mppe":"ppp mppe",                     // [ disable, enable ]
    "mppe_stateful":"ppp mppe statefull",          // [ disable, enable ]

    // ppp attributes
    "ppp":
    {
        "username":"ppp username",                       // [ string ]
        "password":"ppp passowrd",                       // [ string ]
        "mtu":"Maximum transmission unit",               // [ number ], The unit is in bytes
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "custom_dns":"Custom DNS",                       // [ disable, enable ]
        "dns":"Custom DNS1",                             // [ IP address ], This is valid when custom_dns is [ enable ]
        "dns2":"Custom DNS2",                            // [ IP address ], This is valid when custom_dns is [ enable ]
        "txqueuelen":"tx queue len",
        "custom_ip":"custom the ppp interface ip",       // [ disable, enable ]
        "localip":"ppp interface local ip",              // [ ip address ], vaild when "custom_ip" is "enable"
        "remoteip":"ppp interface remote ip",            // [ ip address ], vaild when "custom_ip" is "enable"
        "pppopt":"ppp original options"                  // [ string ], multiple options are separated by semicolons
    },

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
    }
}

```


#### **Methods**

+ `setup[]` **setup the pptp client**, *succeed return ttrue, failed return tfalse, error return terror*

+ `shut[]` **shutdown the pptp client**, *succeed return ttrue, failed return tfalse, error return terror*

+ `status[]` **get the pptp client infomation**, *succeed return talk to describes infomation, failed return NULL, error return terror*
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
    # examples, get the first pptp client infomation
    vpn@pptp.status
    {
        "status":"up",                     # connect is succeed
        "netdev":"ppp0",                   # netdev is ppp0
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

+ `netdev[]` **get the pptp client netdev**, *succeed return netdev, failed return NULL, error return terror*
    ```shell
    # examples, get the first pptp client netdev
    vpn@pptp.netdev
    ppp0
    ```

