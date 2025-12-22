***
## Network frame management 
Network management framework, define local connections and external connections and data scheduling when multiple external connections coexist
The network framework defines a three-tier concept to manage network interfaces: 
- netdev, network inetface of linux, such as eth1, eth2, ath0, usb0
- ifdev, Abstract network devices, such as wifi@nssid, modem@lte, wifi@asta, can be divided by VLAN and combined by bridge, 
- ifname, Abstract network connections


#### **configuration( network@frame )**

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


#### **Methods**

+ `status[]` **show external connections status when multiple external connections coexist**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":
        {
            "status":"Whether online",    // [ down, up ], up for online, down for offline
            "inuse":"Whether used"       // [ disable, enable ], enable for in used, disable for not used
        },
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```
    ```shell
    # examples, get external connections status
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

+ `list[]` **list all connections**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":"The corresponding ifdev object",
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```
    ```shell
    # examples, get all connections
    network@frame.list
    {
        "ifname@lan":"bridge@lan",              # local connection named ifname@lan
        "ifname@lte":"modem@lte",               # extern connection named ifname@lte
        "ifname@lte2":"modem@lte2"              # extern connection named ifname@lte2
    }
    ```

+ `local[]` **list all local connections and infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":
        {
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
            // ... many other properties show
        }
        //"ifname object":{ ... }     How many local connections how many properties show
    }
    ```
    ```shell
    # examples, get all the local connections infomation
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

+ `extern[]` **list all extern connections and infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":
        {
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
        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```
    ```shell
    # examples, get all the extern connections infomation
    network@frame.local
    {
        "ifname@lte":
        {
            "ifdev":"modem@lte",
            "netdev":"usb1",
            "mac":"02:50:F4:00:00:00",
            "status":"uping",
            "tid":"1",
            "method":"slaac",
            "addr":"fe80::50:f4ff:fe00:0",
            "vid":"2c7c",
            "pid":"0125",
            "name":"Quectel-EC2X",
            "devcom":"modem@device",
            "imei":"867160040494084",
            "state":"connect",
            "imsi":"460115372165490",
            "iccid":"89861120147330291660",
            "plmn":"noreg",
            "csq":"21",
            "rssi":"-71",
            "signal":"4"
        },
        "ifname@lte2":
        {
            "mode":"dhcpc",
            "ifname":"ifname@lte2",
            "netdev":"usb0",
            "gw":"10.232.185.157",
            "dns":"120.80.80.80",
            "dns2":"221.5.88.88",
            "ontime":"02:22:47:0",
            "status":"up",
            "ip":"10.232.185.158",
            "mask":"255.255.255.252",
            "delay":"205",
            "livetime":"03:02:35:0",
            "rx_bytes":"137450",
            "rx_packets":"1609",
            "tx_bytes":"162336",
            "tx_packets":"1739",
            "mac":"02:50:F4:00:00:00",
            "tid":"2",
            "method":"slaac",
            "addr":"fe80::50:f4ff:fe00:0",
            "vid":"2c7c",
            "pid":"0800",
            "name":"Quectel-RG500Q",
            "devcom":"modem@device",
            "imei":"869710030002905",
            "imsi":"460015356123463",
            "iccid":"89860121801097564807",
            "state":"connect",
            "rsrp":"-104",
            "rssi":"-72",
            "signal":"4",
            "nettype":"FDD LTE",
            "rsrq":"-15",
            "sinr":"-18.0",
            "band":"LTE BAND 8",
            "csq":"19"
        }
    }
    ```

+ `vpn[]` **list all vpn connections and infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":
        {
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
        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```
    ```shell
    # examples, get all the vpn connections infomation
    network@frame.vpn
    ```

+ `outer[]` **list all extern and vpn ifname and its infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname object":
        {
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
        }
        //"ifname object":{ ... }     How many extern connections how many properties show
    }
    ```
    ```shell
    # examples, get all the extern and vpn connections infomation
    network@frame.outer
    ```

+ `default[]` **get current default connection and infomation**, *succeed return talk to describes infomation, failed reeturn NULL, error return terror*
    ```json
    // Attributes introduction of talk by the method return
    {
        "ifname":"ifname object",
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
        // ... many other properties show
    }
    ```
    ```shell
    # examples, get current default connection infomation
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



+ `main[]` **new session immediately uses the main connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples
    network@frame.main
    ttrue
    ```

+ `back[]` **new session immediately uses the backup connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples
    network@frame.back
    ttrue
    ```

+ `back[]` **new session immediately load balance on main and back connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples
    network@frame.back
    ttrue
    ```

+ `auto[]` **enable automatic load balance, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples
    network@frame.auto
    ttrue
    ```



+ `src_main[ source ip address ]` **set new session of source ip address immediately uses the main connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the source ip 192.168.8.222 new session route to main connection
    network@frame.src_main[ 192.168.8.222 ]
    ttrue
    ```
+ `dest_main[ dest ip address ]` **set new session of dest ip address immediately uses the main connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the dest ip 114.114.114.114 new session route to main connection
    network@frame.dest_main[ 114.114.114.114 ]
    ttrue
    ```

+ `src_back[ source ip address ]` **set new session of source ip address immediately uses the back connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the source ip 192.168.8.111 new session route to back connection
    network@frame.src_back[ 192.168.8.111 ]
    ttrue
    ```
+ `dest_main[ dest ip address ]` **set new session of dest ip address immediately uses the main connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the dest ip 114.114.115.115 new session route to back connection
    network@frame.dest_back[ 114.114.115.115 ]
    ttrue
    ```

+ `src_both[ source ip address ]` **set new session of source ip address load balance on main and back connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the source ip 192.168.8.123 new session load balance on main and back connection
    network@frame.src_both[ 192.168.8.123 ]
    ttrue
+ `dest_both[ dest ip address ]` **set new session of dest ip address load balance on main and back connection, only vaild in dbdc**, *succeed return ttrue, failed return tfalse, error return terror*
    ```shell
    # examples, the dest ip 8.8.8.8 new session load balance on main and back connection
    network@frame.dest_both[ 8.8.8.8 ]
    ttrue


