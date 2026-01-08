***
## Network frame management 
Network management framework, define external connections and data scheduling when multiple external connections coexist

#### **Configuration( network@frame )**

```json
// Attribute introduction
{
    "type":"Multiple link connect type",                          // [ "cold", "hot", "dhdc", "auto" ]
                                                                                // "clod" for clod backup at the main and back
                                                                                // "hot" for hot backup at the main and back
                                                                                // "spare" for hot backup at the main and back, king
                                                                                // "backup" for hot backup at the main and back, king, reserve
                                                                                // "dhdc" for load balancing at the main and back 
                                                                                // "auto" for load balancing at the main and back, king, reserve
    "concom":"Multiple link connection management components",    // [ string ], You can customize the data scheduling component to implement more personalized requirements

    // When type is hot, it is the main connection. When type is dbdc, it is one of the external network connections 
    "main":"ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ] 

    // When type is hot, it is the backup connection. When type is dbdc, it is one of the external network connections 
    "back":"ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]

    // When type is hot, the value is ignored. If the type is dbdc, all the default route data is forwarded by the connection as soon as the connection goes online
    "king":"ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]

    // When type is hot, the value is ignored. If the type is dbdc, the connections only connect but on route data via it
    "reserve":"ifname object of extern",       // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]

    // Configure parameters of the delay switchover function, only in dbdc mode, the function can control the data via low delay connection
    "delay_count":"Statistical delay times of last",   // [ number ]
    "delay_divide":"delay divide line",                // [ number ], the unit is ms
    "delay_diff":"Delay differential",                 // [ number ], the unit is ms

    // DNS custom when Multiple DNS
    "custom_dns":"Custom DNS",                       // [ "disable", "enable", "ifname@lte", "ifname@lte2", ... ]
    "dns":"Custom DNS1",                             // [ ip address ], This is valid when "custom_dns" is "enable"
    "dns2":"Custom DNS2"                             // [ ip address ], This is valid when "custom_dns" is "enable"
}
// Examples
{
    "type":"dbdc",                  // dhdc for load balancing 
    "main":"ifname@lte",            // load balancing at ifname@lte and ifname@lte2
    "back":"ifname@lte2",
    "king":"ifname@wan",            // all data switch to ifname@wan when ifname@wan is online
    "reserve":"ifname@wisp",        // keep the ifname@wisp online, but don't switch the data via it
    "delay_count":"10",             // Collect statistics on the latest 10 delays and schedule traffic based on the delay
    "delay_divide":"150", 
    "delay_diff":"100"
}
```


#### **API( network@frame )**

+ `status[]` **show external connections status when multiple external connections coexist**
    - failed return NULL
    - error return terror, Like in the wrong mode of operation, only work in the muliti-connection mode
    - return json to describes the network infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":            // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]:{}
        {
            "status":"Whether online",    // [ down, up ], up for online, down for offline
            "inuse":"Whether used"        // [ disable, enable ], enable for in used, disable for not used
        },
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```

    Example, get external connections status
    ```shell
    network@frame.status
    {
        "ifname@lte":                   # ifname@lte status is online and inuse current
        {
            "status":"up",
            "inuse":"enable"
        },
        "ifname@lte2":                  # ifname@lte2 status is offline and not use current
        {
            "status":"down",
            "inuse":"disable"
        }
    }
    ```

+ `list[]` **list all connections**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":"The corresponding ifdev object",    // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]: [ string ]
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ``` 

    Example, get all connections
    ```shell
    network@frame.list
    {
        "ifname@lan":"bridge@lan",              # local connection named ifname@lan
        "ifname@lte":"modem@lte",               # extern connection named ifname@lte
        "ifname@lte2":"modem@lte2"              # extern connection named ifname@lte2
    }
    ```

+ `local[]` **list all local connections and infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":            // [ "ifname@lan", "ifname@lan2", "ifname@lan3", "ifname@lan4" ]:{}
        {                           // return by API "status" of ifname object
            "status":"Current state",        // [ "uping", "down", "up" ]
                                                // "uping" for connecting
                                                // "down" for the ifname is down
                                                // "up" for the network is connect succeed

            "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting
            "netdev":"netdev name",         // [ string ]
            "ifdev":"ifdev name",           // [ string ], Optional
            "gw":"gateway ip address",      // [ ip address ], Optional
            "dns":"dns ip address",         // [ ip address ], Optional
            "dns2":"dns2 ip address",       // [ ip address ], Optional
            "ip":"ip address",              // [ ip address ]
            "mask":"network mask",          // [ ip address ]
            "ontime":"online uptime",       // [ string ], Optional, online system uptime
            "livetime":"online time",       // [ string ], format is hour:minute:second:day
            "rx_bytes":"send bytes",        // [ number ]
            "rx_packets":"send packets",    // [ number ]
            "tx_bytes":"receive bytes",     // [ number ]
            "tx_packets":"receive packets", // [ number ]
            "mac":"MAC address",            // [ mac address ]

            "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, exist when IPV6 enable
                                                // "manual" for manual setting
                                                // "automatic" for DHCPv6
                                                // "slaac" for Stateless address autoconfiguration
            "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr3":"IPv6 address3"         // [ ipv6 address ], Optional, exist when IPV6 enable
        }
        //"ifname object":{ ... }     How many local connections how many properties show
    }
    ```

    Example, get all the local connections infomation
    ```shell
    network@frame.local
    {
        "ifname@lan":                           # local connection named ifname@lan
        {
            "mode":"static",
            "ifname":"ifname@lan",
            "ifdev":"bridge@lan",
            "netdev":"lan",
            "ontime":"00:00:23:0",
            "status":"up",
            "ip":"192.168.8.1",
            "mask":"255.255.255.0",
            "livetime":"02:52:37:0",
            "rx_bytes":"348892",
            "rx_packets":"5923",
            "tx_bytes":"280817",
            "tx_packets":"335",
            "mac":"00:03:7F:12:72:06"
        }
    }
    ```

+ `extern[]` **list all extern connections and infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":          // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]:{}
        {                         // return by API "status" of ifname object
            "status":"Current state",        // [ "uping", "down", "up" ]
                                                // "uping" for connecting
                                                // "down" for the ifname is down
                                                // "block" for wait keeplive return
                                                // "failed" for keeplive failed
                                                // "up" for the network is connect succeed

            "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
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
            "rx_bytes":"send bytes",        // [ number ]
            "rx_packets":"send packets",    // [ number ]
            "tx_bytes":"receive bytes",     // [ number ]
            "tx_packets":"receive packets", // [ number ]
            "mac":"MAC address",            // [ mac address ]

            "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, exist when IPV6 enable
                                                // "manual" for manual setting
                                                // "automatic" for DHCPv6
                                                // "slaac" for Stateless address autoconfiguration
            "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable
        
                ////////////////////////////////////////////////////////////////////////////////////////////
                // show this attr when "ifname object" be "ifname@lte" "ifname@lte2" "ifname@lte3" "ifname@lte4" // 
                ////////////////////////////////////////////////////////////////////////////////////////////
                "imei":"IMEI numer",            // [ string ]
                "imsi":"IMSI number",           // [ string ]
                "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                        // number for iccid
                                                        // "nosim" for cannot found the simcard
                                                        // "pin" for the simcard need PIN code
                                                        // "puk" for the simcard pin error
                "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                        // number for MCC and MNC
                                                        // "noreg" for cannot register to opeartor
                                                        // "unreg" for cannot register to opeartor
                                                        // "dereg" for register to operator be refused
                "name":"modem name",             // [ string ], lte modem model or name
                "operator":"operator name",      // [ string ]
                "nettype":"network type",        // The format varies depending on the module
                                                // 2G usually shows GSM, GPRS, EDGE, CDMA
                                                // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                                // 4G usually shows LTE, FDD, TDD
                "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
                "rssi":"signal intensity",       // [ number ], the unit is dBm
                "csq":"CSQ number",              // [ number ], Optional
                "rsrp":"RSRP value",             // [ string ], Optional, The format varies depending on the module
                "rsrq":"RSRQ value",             // [ string ], Optional, The format varies depending on the module
                "sinr":"sinr value",             // [ string ], Optional, The format varies depending on the module  
                "band":"current band",           // [ string ], Optional, The format varies depending on the module
                "ci":"cell identity",            // [ string ], Optional
                "lac":"location area code",      // [ string ], Optional
                "channel":"location area code",  // [ string ], Optional    

                //////////////////////////////////////////////////////////////////
                // show this attr when "ifname object" be "ifname@wisp" "ifname@wisp2" //
                //////////////////////////////////////////////////////////////////
                "peer":"Peer SSID",              // [ string ]
                "peermac":"Peer BSSID",          // [ MAC address ]
                "channel":"Peer channel",        // [ 1- 165 ]
                "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
                "rate":"connect rate",           // [ number ], Optional, the unit is M
                "rssi":"Peer RSSI",              // [ number ], Optional, the unit is dBm
                "rssp":"Peer signal percentage"  // [ number ], Optional, the unit is %    

        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```

    Example, get all the extern connections infomation
    ```shell
    network@frame.extern
    {
        "ifname@lte":
        {
            "mode":"dhcpc",                    # IPv4 connect mode is DHCP
            "netdev":"usb1",                   # netdev is usb1
            "gw":"10.84.136.246",
            "dns":"120.80.80.80",
            "dns2":"221.5.88.88",
            "ifdev":"modem@lte",
            "ontime":"28826",
            "status":"up",                     # connect is succeed
            "delay":"26",
            "ip":"10.84.136.245",
            "mask":"255.255.255.252",
            "livetime":"00:31:58:0",
            "rx_bytes":"4407784",
            "rx_packets":"34234",
            "tx_bytes":"4440236",
            "tx_packets":"47893",
            "mac":"02:50:F4:00:00:00",
            "imei":"868186042111714",
            "ci":"4A37D91",
            "lac":"25E3",
            "plmn":"46001",
            "csq":"23",
            "nettype":"FDD LTE",
            "rsrp":"-97",
            "rssi":"-66",
            "rsrq":"-9",
            "sinr":"-18",
            "band":"LTE BAND 1",
            "channel":"100",
            "signal":"4",
            "operator":"China Unicom",
            "imsi":"460018708133639",
            "iccid":"8986012580155265717",
            "name":"Quectel-EC2X"
        },
        "ifname@wisp":
        {
            "status":"up",                     # connect is succeed
            "mode":"dhcpc",                    # IPv4 connect mode is DHCP
            "netdev":"ath11",                  # netdev is ath11
            "gw":"192.168.10.254",             # gateway is 192.168.10.254
            "dns":"114.114.114.114",           # dns is 114.114.114.114
            "dns2":"221.5.88.88",              # backup dns is 221.5.88.88
            "ip":"192.168.10.1",               # ip address is 192.168.1.1
            "mask":"255.255.255.0",            # network mask is 255.255.255.0
            "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
            "rx_bytes":"1256",                 # receive 1256 bytes
            "rx_packets":"4",                  # receive 4 packets
            "tx_bytes":"1320",                 # send 1320 bytes
            "tx_packets":"4",                  # send 4 packets
            "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
            "method":"slaac",                  # IPv6 address mode is slaac
            "addr":"fe80::50:f4ff:fe00:0",     # local IPv6 address is fe80::50:f4ff:fe00:0
            "peer":"TP-link-2231",            # peer is TP-link-2231
            "peermac":"70:3A:D8:54:BC:90",    # peer BSSID is 70:3A:D8:54:BC:90
            "channel":"10",                   # channel is 10
            "rate":"270",                     # rate is 270M
            "rssi":"-41",                     # rssi is -41dBm
            "signal":"3"                      # signal level is 3
        },
        "ifname@wan":
        {
            "status":"up",                     # connect is succeed
            "mode":"static",                   # IPv4 connect mode is static
            "netdev":"wan",                    # netdev is lan
            "gw":"192.168.10.254",             # gateway is 192.168.10.254
            "dns":"114.114.114.114",           # dns is 114.114.114.114
            "dns2":"221.5.88.88",              # backup dns is 221.5.88.88
            "ip":"192.168.10.1",               # ip address is 192.168.1.1
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
    }
    ```

+ `vpn[]` **list all vpn connections and infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":              // [ string ]:{}, VPN ifname object name
        {                             // return by API "status" of ifname object
            "mode":"current mode",
            "status":"current state",
            "ifdev":"corresponding ifdev object",
            "netdev":"netdev name of linux",
            "ip":"IP address",
            "rx_bytes":"receive bytes",
            "rx_packets":"receive packets",
            "tx_bytes":"tx bytes",
            "tx_packets":"tx packets",
            "mac":"MAC address"            
            //... more the attr return by API "status" of ifname object
        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```

    Example, get all the vpn connections infomation
    ```shell
    network@frame.vpn
    {
        "ovpn@client":
        {
            ...
        },
        "vpnc@pptp":
        {
            ...
        }
    }    
    ```

+ `outer[]` **list all extern and vpn ifname and its infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":              // [ string ]:{}, VPN and extern ifname object name
        {                             // return by API "status" of ifname object
            "mode":"current mode",
            "status":"current state",
            "ifdev":"corresponding ifdev object",
            "netdev":"netdev name of linux",
            "ip":"IP address",
            "rx_bytes":"receive bytes",
            "rx_packets":"receive packets",
            "tx_bytes":"tx bytes",
            "tx_packets":"tx packets",
            "mac":"MAC address"            
            //... more the attr return by API "status" of ifname object
        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```

    Example, get all the extern and vpn connections infomation
    ```shell
    network@frame.outer
    {
        "ifname@wan":
        {
            ...
        },    
        "ifname@wisp":
        {
            ...
        },          
        "ovpn@client":
        {
            ...
        },
        "vpnc@pptp":
        {
            ...
        }
    }        
    ```

+ `default[]` **get current default connection and infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {                                // return by API "status" of ifname object
        "ifname":"ifname object",       // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
        "status":"Current state",       // [ "uping", "down", "up" ]
                                            // "uping" for connecting
                                            // "down" for the ifname is down
                                            // "block" for wait keeplive return
                                            // "failed" for keeplive failed
                                            // "up" for the network is connect succeed

        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
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
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]

        "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, exist when IPV6 enable
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable
    
            ////////////////////////////////////////////////////////////////////////////////////////////
            // show this attr when "ifname" be "ifname@lte" "ifname@lte2" "ifname@lte3" "ifname@lte4" // 
            ////////////////////////////////////////////////////////////////////////////////////////////
            "imei":"IMEI numer",            // [ string ]
            "imsi":"IMSI number",           // [ string ]
            "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                    // number for iccid
                                                    // "nosim" for cannot found the simcard
                                                    // "pin" for the simcard need PIN code
                                                    // "puk" for the simcard pin error
            "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                    // number for MCC and MNC
                                                    // "noreg" for cannot register to opeartor
                                                    // "unreg" for cannot register to opeartor
                                                    // "dereg" for register to operator be refused
            "name":"modem name",             // [ string ], lte modem model or name
            "operator":"operator name",      // [ string ]
            "nettype":"network type",        // The format varies depending on the module
                                            // 2G usually shows GSM, GPRS, EDGE, CDMA
                                            // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                            // 4G usually shows LTE, FDD, TDD
            "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
            "rssi":"signal intensity",       // [ number ], the unit is dBm
            "csq":"CSQ number",              // [ number ], Optional
            "rsrp":"RSRP value",             // [ string ], Optional, The format varies depending on the module
            "rsrq":"RSRQ value",             // [ string ], Optional, The format varies depending on the module
            "sinr":"sinr value",             // [ string ], Optional, The format varies depending on the module  
            "band":"current band",           // [ string ], Optional, The format varies depending on the module
            "ci":"cell identity",            // [ string ], Optional
            "lac":"location area code",      // [ string ], Optional
            "channel":"location area code",  // [ string ], Optional    

            //////////////////////////////////////////////////////////////////
            // show this attr when "ifname" be "ifname@wisp" "ifname@wisp2" //
            //////////////////////////////////////////////////////////////////
            "peer":"Peer SSID",              // [ string ]
            "peermac":"Peer BSSID",          // [ MAC address ]
            "channel":"Peer channel",        // [ 1- 165 ]
            "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
            "rate":"connect rate",           // [ number ], Optional, the unit is M
            "rssi":"Peer RSSI",              // [ number ], Optional, the unit is dBm
            "rssp":"Peer signal percentage"  // [ number ], Optional, the unit is %    
    }
    ```

    Example, get current default connection infomation
    ```shell
    network@frame.default
    {
        "ifname":"ifname@lte2",              # current default connetion is ifname@lte2, and the above is the infomation
        "mode":"dhcpc",
        "netdev":"usb0",
        "gw":"10.232.185.157",
        "dns":"120.80.80.80",
        "dns2":"221.5.88.88",
        "ontime":"02:22:47:0",
        "status":"up",
        "ip":"10.232.185.158",
        "mask":"255.255.255.252",
        "delay":"186",
        "livetime":"03:08:57:0",
        "rx_bytes":"142658",
        "rx_packets":"1671",
        "tx_bytes":"168684",
        "tx_packets":"1808",
        "mac":"02:50:F4:00:00:00",
        "name":"Quectel-RG500Q",
        "imei":"869710030002905",
        "imsi":"460015356123463",
        "iccid":"89860121801097564807",
        "state":"connect",
        "rssi":"-75",
        "signal":"4"
    }    
    ```

+ `gateway[]` **get current gateway connection and infomation**
    - failed return NULL
    - return json to describes the infomation  
    ```json
    // Attributes introduction of talk by the method return
    {                                // return by API "status" of ifname object
        "ifname":"ifname object",       // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
        "status":"Current state",       // [ "uping", "down", "up" ]
                                            // "uping" for connecting
                                            // "down" for the ifname is down
                                            // "block" for wait keeplive return
                                            // "failed" for keeplive failed
                                            // "up" for the network is connect succeed

        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
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
        "rx_bytes":"send bytes",        // [ number ]
        "rx_packets":"send packets",    // [ number ]
        "tx_bytes":"receive bytes",     // [ number ]
        "tx_packets":"receive packets", // [ number ]
        "mac":"MAC address",            // [ mac address ]

        "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, exist when IPV6 enable
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable
    
            ////////////////////////////////////////////////////////////////////////////////////////////
            // show this attr when "ifname" be "ifname@lte" "ifname@lte2" "ifname@lte3" "ifname@lte4" // 
            ////////////////////////////////////////////////////////////////////////////////////////////
            "imei":"IMEI numer",            // [ string ]
            "imsi":"IMSI number",           // [ string ]
            "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                    // number for iccid
                                                    // "nosim" for cannot found the simcard
                                                    // "pin" for the simcard need PIN code
                                                    // "puk" for the simcard pin error
            "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                    // number for MCC and MNC
                                                    // "noreg" for cannot register to opeartor
                                                    // "unreg" for cannot register to opeartor
                                                    // "dereg" for register to operator be refused
            "name":"modem name",             // [ string ], lte modem model or name
            "operator":"operator name",      // [ string ]
            "nettype":"network type",        // The format varies depending on the module
                                            // 2G usually shows GSM, GPRS, EDGE, CDMA
                                            // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                            // 4G usually shows LTE, FDD, TDD
            "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
            "rssi":"signal intensity",       // [ number ], the unit is dBm
            "csq":"CSQ number",              // [ number ], Optional
            "rsrp":"RSRP value",             // [ string ], Optional, The format varies depending on the module
            "rsrq":"RSRQ value",             // [ string ], Optional, The format varies depending on the module
            "sinr":"sinr value",             // [ string ], Optional, The format varies depending on the module  
            "band":"current band",           // [ string ], Optional, The format varies depending on the module
            "ci":"cell identity",            // [ string ], Optional
            "lac":"location area code",      // [ string ], Optional
            "channel":"location area code",  // [ string ], Optional    

            //////////////////////////////////////////////////////////////////
            // show this attr when "ifname" be "ifname@wisp" "ifname@wisp2" //
            //////////////////////////////////////////////////////////////////
            "peer":"Peer SSID",              // [ string ]
            "peermac":"Peer BSSID",          // [ MAC address ]
            "channel":"Peer channel",        // [ 1- 165 ]
            "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
            "rate":"connect rate",           // [ number ], Optional, the unit is M
            "rssi":"Peer RSSI",              // [ number ], Optional, the unit is dBm
            "rssp":"Peer signal percentage"  // [ number ], Optional, the unit is %    
    }
    ```

    Example, get current gateway connection infomation
    ```shell
    network@frame.gateway
    {
        "ifname":"ifname@lte2",              # current default connetion is ifname@lte2, and the above is the infomation
        "mode":"dhcpc",
        "netdev":"usb0",
        "gw":"10.232.185.157",
        "dns":"120.80.80.80",
        "dns2":"221.5.88.88",
        "ontime":"02:22:47:0",
        "status":"up",
        "ip":"10.232.185.158",
        "mask":"255.255.255.252",
        "delay":"186",
        "livetime":"03:08:57:0",
        "rx_bytes":"142658",
        "rx_packets":"1671",
        "tx_bytes":"168684",
        "tx_packets":"1808",
        "mac":"02:50:F4:00:00:00",
        "name":"Quectel-RG500Q",
        "imei":"869710030002905",
        "imsi":"460015356123463",
        "iccid":"89860121801097564807",
        "state":"connect",
        "rssi":"-75",
        "signal":"4"
    }    
    ```
