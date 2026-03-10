***
## WISP Network Management
Manage WISP network. This component must depend on wireless station interface and network Management Framework project  
Usually ifname@wisp is the first WISP(2.4G) network. If there are multiple WISP network in the system, ifname@wisp2 will be the second WISP(5.8G) network, and increase by degress

#### **configuration( ifname@wisp )**   
**ifname@wisp** is first WISP(2.4G) network   
**ifname@wisp2** is second WISP(5.8G) network   

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ "enable", "disable" ]

    // MAC
    "mac":"set MAC address for interface", // [ mac address ]

    // Wireless connect
    "peer":"SSID to connect",              // [ string ]
    "peermac":"BSSID to connect",          // [ mac address ]
    "peermode":"mode of connection",       // [ "hidden" ] Indicates that the peer end does not broadcast SSID. In hidden mode, channel must not be empty  
    "channel":"wireless channel",          // [ number ], 0-165, 0 for auto
    "secure":"mode of security",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] for no securiyt
                                                            // [ wpapsk ]  for WPAPSK
                                                            // [ wpa2psk ]  for WPA2PSK
                                                            // [ wpapskwpa2psk ] for WPA Mix
    "wpa_encrypt":"WAP encrypt",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] for AES
                                                            // [ tkip ] for TKIP
                                                            // [ tkipaes ] for auto
    "wpa_key":"WPA key",                   // [ string ], The value is a string of at least 8 characters. This parameter is mandatory if the "secure" is wpapsk/wpa2psk/wpapskwpa2psk
    "ssid_disable":"disable the ssid",     // [ "disable", "enable" ], disable the local ssid when connected

    // IPv4
    "tid":"table identify number",         // [ number ] exclusive route table ID, only for multiple WAN
    "metric":"default route metric",       // [ number  ]
    "mode":"IPV4 address mode",            // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "pppoe" ] for PPPOE dial
    "static":                                 // detial configure for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 netmask >
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // detial configure for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ]
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ]
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], This is valid when "custom_dns" is "enable"
    },
    "pppoe":                                    // detial configure for "mode" is "pppoe"
    {
        "username":"PPPOE username",                     // [ string ]
        "password":"PPPOE password",                     // [ string ]
        "service":"service name",                        // [ string ], default accept all service
        "mss":"TCP Maximum Segment Size",                // [ number ], The unit is in bytes
        "lcp_echo_interval":"LCP echo interval",         // [ number ], The unit is in seconds
        "lcp_echo_failure":"LCP echo failure times",     // [ number ]
        "pppopt":"PPP options",                          // [ string ], Multiple options are separated by colons
        "custom_dns":"Custom DNS",                       // [ "disable", "enable" ]
        "dns":"Custom DNS1",                             // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2",                            // [ ip address ], This is valid when "custom_dns" is "enable"
        "txqueuelen":"tx queue size"                     // [ number ]
    },
    "masq":"out stream share the interface IPv4 address to access the Internet",  // [ "disable", "enable" ]
    "mtu":"Maximum transmission unit",                                            // [ number ], The unit is in bytes

    // IPv6
    "method":"IPv6 address mode",             // [ "disable", "manual", "automatic", "slaac" ]
                                                    // "disable" is not use ipv6
                                                    // "manual" for manual setting
                                                    // "automatic" for DHCPv6
                                                    // "slaac" for Stateless address autoconfiguration
    "manual":                                 // detial configure for "method" is "manual"
    {
        "addr":"IPv6 address",                      // < ipv6 address >
        "prefix":"IPv6 prefix",                     // < number >, 1-128
        "hop":"IPv6 gateway",                       // [ ipv6 address ]
        "resolve":"IPv6 DNS",                       // [ ipv6 address ]
        "resolve2":"IPv6 DNS2"                      // [ ipv6 address ]
    },
    "automatic":                             // detial configure for "method" is "automatic"
    {
        "mode":"mode for get the ipv6",                  // [ "try", "force", "disable" ]
        "prefix":"ipv6-prefix of length for request",    // [ "auto", "48", "52", "56", "60", "60", "disable" ]
        "custom_resolve":"Custom DNS",                   // [ "disable", "enable" ]
        "resolve":"Custom DNS1",                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
        "resolve2":"Custom DNS2"                         // [ ipv6 address ], This is valid when "custom_dns" is "enable"
    },
    "masquerade":"out stream share the interface IPv6 address to access the Internet",   // [ "disable", "enable" ]

    // Configure for link detection mechanism, or call it keeplive mechanism
    "keeplive":
    {
        "type":"keeplive mode",   // [ "disable" ] for disable the keeplive
                                  // [ "icmp" ] for ping keeplive
                                  // [ "dns" ] for test the dns response
                                  // [ "recv" ] for count receive packet to keeplive
                                  // [ "auto" ] for count receive packet to keeplive when test the dns response failed

        "action":"failed to do",  // [ "reboot" ] reboot the system
                                  // [ "reset" ] reset the band
                                  // [ ] other redial the connetion
        "icmp":                                                   // detial configure for "type" is "icmp"
        {
            "dest":                                                         // destination address for ICMP keeplive
            {
                "destination identify2":"destination address1",                        // [ string ]:[ IP address ]
                // "...":"..." You can configure multiple destination IP addresses. If only one PING echo packet is returned, the detection succeeds. If no PING echo packet is returned, the detection fails  
            },
            "timeout":"Maximum time to wait for the return of a PING echo packet",     // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds
        },
        "dns":                                                   // detial configure for "type" is "icmp"
        {
            "timeout":"Maximum time to wait for the return of a dns resolve packet",   // [ number ], The unit is in seconds
            "failed":"Number of detection failures",                                   // [ number ], If the number of detection failures exceeds this threshold, the link is deactivated
            "interval":"Interval of each Successful detection"                         // [ number ], The unit is in seconds
        },
        "recv":                                                  // detial configure for "type" is "recv"
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

Example, show first WISP(2.4G) all configure
```shell
ifname@wisp
{
    "peer":"V520-D21D20",    # connect V520-D21D20
    "secure":"wpapsk",       # security is WPAPSK
    "wpa_encrypt":"aes",     # encrypt use AES
    "wpa_key":"87654321",    # password 87654321

    "mode":"dhcpc",                                  # mode is DHCP client
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

Example, modify the icmp keeplive destination address for first WISP(2.4G) network  
```shell
ifname@wisp:keeplive/icmp/dest/test=8.8.8.8            # modify the icmp keeplive first destination address to 8.8.8.8
ttrue
ifname@wisp:keeplive/icmp/dest/test2=8.8.4.4           # modify the icmp keeplive second destination address to 8.8.4.4 
ttrue
ifname@wisp:keeplive/icmp/dest/test3=114.114.114.114   # modify the icmp keeplive third destination address to 114.114.114.114
ttrue
# You can also use one command to complete the operation of the above three command
ifname@wisp:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   

Example, modify the SSID for first WISP(2.4G) connect
```shell
ifname@wisp:peer=Myhotpot
ttrue
ifname@wisp:secure=wpapsk
ttrue
ifname@wisp:wpa_key=88888888
ttrue
```   

You can also use one command to complete the operation of the above three command
```shell
ifname@wisp|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```   

Example, disable the first WISP(2.4G) keeplive function
```shell
ifname@wisp:keeplive=disable
ttrue
```   

Example, disable the first WISP(2.4G)
```shell
ifname@wisp:status=disable
ttrue
```   

Example, enable the first WISP(2.4G)
```shell
ifname@wisp:status=enable
ttrue
```     

Example, enable the second WISP(5.8G)
```shell
ifname@wisp2:status=enable
ttrue
```



#### **API**   
**ifname@wisp** is first WISP network   
**ifname@wisp2** is second WISP network   

+ `status[]` **get the WISP infomation**   
    - failed return NULL
    - error return terror   
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of talk by the method return
    {
        "status":"Current state",        // [ "uping", "scanning", "block", "up", "failed", "down" ]
                                             // "uping" for connecting
                                             // "scanning" for scanning the AP
                                             // "block" for wait keeplive succeed
                                             // "up" for the network is connect succeed
                                             // "failed" for keeplive failed
                                             // "down" for the ifname is down

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

        "peer":"Peer SSID",              // [ string ]
        "peermac":"Peer BSSID",          // [ MAC address ]
        "channel":"Peer channel",        // [ 1- 165 ]
        "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
        "rate":"connect rate",           // [ number ], Optional, the unit is M
        "rssi":"Peer RSSI",              // [ number ], Optional, the unit is dBm
        "rssp":"Peer signal percentage"  // [ number ], Optional, the unit is %
    }
    ```   

    Example, get the first WISP network infomation
    ```shell
    ifname@wisp.status
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
    }
    ```   

+ `netdev[]` **get the WISP netdev**   
    - failed return NULL
    - error return terror       
    - return string to describes this infomation  

    Example, get the first WISP netdev
    ```shell
    ifname@wisp.netdev
    ath11
    ```   

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - error return terror   
    - return string to describes this infomation  

    Example, get the first WISP network ifdev
    ```shell
    ifname@wan.ifdev
    wifi@nsta
    ```   

+ `chlist[]` **get the WISP channal list*   
    - failed return NULL
    - error return terror   
    - succeed return json to describes infomation   

    ```json
    // Attributes introduction of talk by the method return
    {
        "channel number":{}       // [ number ]:{}
        // ... more channel
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

+ `aplist[]` **use the WISP scan the surrounding AP**   
    - failed return NULL
    - error return terror   
    - succeed return json to describes infomation   

    ```json
    // Attributes introduction of talk by the method return
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID name",                           // [ string ]
            "channel":"channel number",                   // [ number ], 0-165, 0 for auto
            "secure":"mode of security",                  // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                 // "disable" for no securiyt
                                                                 // "wpapsk"  for WPAPSK
                                                                 // "wpa2psk"  for WPA2PSK
                                                                 // "wpapskwpa2psk" for WPA Mix
            "wpa_encrypt":"WAP encrypt",                  // [ "aes", "tkip", "tkipaes" ]
                                                                 // "aes" for AES
                                                                 // "tkip" for TKIP
                                                                 // "tkipaes" for auto
            "sig":"signal level(%)",                      // [ number ]
            "signal":"signal level[0-4]",                 // [ "0", "1", "2", "3", "4" ]
            "chext":"extern channel",                     // [ "none", "below", "above" ]
            "mode":"wireless system"                      // [ string ]
        }
        // ... more AP
    }
    ```   

    Example, get the surrounding AP from first WISP scan
    ```shell
    ifname@wisp.aplist
    {
        "80:EA:07:15:0E:E6":                    # first AP by scanning
        {
            "ssid":"1411",                                 # frist AP SSID
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
        },
        "8C:74:A0:D6:68:B0":                    # third AP by scanning
        {
            "ssid":"CMCC-ktfK",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11b/g/n"
        }
    }
    ```   

+ `shut[]` **shutdown the WISP network**   
    - failed return tfalse
    - error return terror   
    - succeed return ttrue

    Example, shutdown the frist WISP network
    ```shell
    ifname@wisp.shut
    ttrue
    ```   
    Example, shutdown the second WISP network
    ```shell
    ifname@wisp2.shut
    ttrue
    ```   

+ `setup[]` **setup the WISP network**   
    - failed return tfalse
    - error return terror   
    - succeed return ttrue

    Example, setup the frist WISP network
    ```shell
    ifname@wisp.setup
    ttrue
    ```   
    Example, setup the second WISP network
    ```shell
    ifname@wisp2.setup
    ttrue
    ```   



