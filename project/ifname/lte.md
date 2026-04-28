## ifname@lte — LTE/NR Network Management
Manage LTE/NR networks and 4G/NR baseband links. It pairs a **logical LTE interface** here with **modem management** (`modem@lte`, … — see [`../modem/lte.md`](../modem/lte.md)) and the **network framework** for uplink scheduling ([`../network/frame.md`](../network/frame.md)).  
Usually `ifname@lte` is the first LTE/NR network instance. If there are multiple LTE/NR modems, `ifname@lte2` is the second instance, and numbering increases sequentially.


### Configuration ( `ifname@lte` )
**ifname@lte** is first LTE network   
**ifname@lte2** is second LTE network   

```json
// Attribute introduction
{
    "status":"start at system startup",    // [ "enable", "disable" ]

    // profile attributes for LTE modem PDP
    "pin":"simcard pin",                       // [ string ]
    "profile":"use custom PDP profile",        // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile settings, used when "profile" is "enable"
    {
        "dial":"dial number",                     // [ number ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // // [ "auto","disable","pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password"                  // [ string ]
    },

    // backup simcard configure
    // NOTE: The current `ifname@lte` implementation mainly manages link behavior.
    // These backup-SIM fields are modem-side capabilities and only take effect when the lower modem component supports them.
    "bsim":"backup simcard function",                         // [ "disable", "enable" ]
    "bsim_cfg":                                               // backup SIM settings, used when "bsim" is "enable"
    {
        "mode":"specify active SIM card",                           // [ "auto", "back", "main", "detect" ]
                                                                        // "auto" for automatic switching based on rules
                                                                        // "back" for backup simcard
                                                                        // "main" for main simcard
                                                                        // "detect" the IO for auto that need detect IO support
        "simcard_failed_threshold":"first failed time to switch",                                  // [ number ], default 60 seconds
        "simcard_failed_threshold2":"second failed time to switch",                                // [ number ], default 180 seconds
        "simcard_failed_threshold3":"third failed time to switch",                                 // [ number ], default 300 seconds
        "simcard_failed_everytime":"every failed time to switch",                                  // [ number ], default 1800 seconds

        "signal_failed_threshold":"first failed time to switch",                                    // [ number ], default 120 seconds
        "signal_failed_threshold2":"second failed time to switch",                                  // [ number ], default 300 seconds
        "signal_failed_threshold3":"third failed time to switch",                                   // [ number ], default 600 seconds
        "signal_failed_everytime":"every failed time to switch",                                    // [ number ], default 1800 seconds

        "attach_failed_threshold":"first failed time to switch",                                    // [ number ], default 60 seconds
        "attach_failed_threshold2":"second failed time to switch",                                  // [ number ], default 180 seconds
        "attach_failed_threshold3":"third failed time to switch",                                   // [ number ], default 600 seconds
        "attach_failed_everytime":"every failed time to switch",                                    // [ number ], default 1800 seconds

        "failed_threshold":"first failed time to switch",                                   // [ number ]
        "failed_threshold2":"second failed time to switch",                                 // [ number ]
        "failed_threshold3":"third failed time to switch",                                  // [ number ]
        "failed_everytime":"every failed time to switch",                                   // [ number ]

        "failover":"backup simcard usage duration",                                       // [ number ], the unit is second
        "keeplive_switch":"keeplive failed to switch",                                    // [ "disable", "enable" ]
        // backup profile attributes
        "pin":"simcard pin",                       // [ string ]
        "profile":"custom the profile",            // [ "disable", "enable" ]
        "profile_cfg":                             // custom profile save here, the json be used when "profile" value is enable
        {
            "dial":"dial number",                     // [ number ]
            "cid":"dial CID",                         // [ number ], default is 1
            "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
            "apn":"APN name",                         // [ string ]
            "user":"user name",                       // [ string ]
            "passwd":"user password",              // [ string ]
            "auth":"authentication method",           // [ "auto","disable","pap", "chap", "papchap" ]
        }
    },

    // simcard detection attributes
    "need_simcard":"SIMcard must be detected",                                                 // [ "enable", "disable" ]
                                                                                                    // "enable" requires SIM card detection, will reset modem if failed
                                                                                                    // "disable" allows operation without SIM card
    "simcard_failed_threshold":"first failed to reset time",                                   // [ number ], default 60 seconds
    "simcard_failed_threshold2":"second failed to reset time",                                 // [ number ], default 180 seconds
    "simcard_failed_threshold3":"third failed to reset time",                                  // [ number ], default 300 seconds
    "simcard_failed_everytime":"every failed to reset time",                                   // [ number ], default 1800 seconds

    // signal/plmn detection attributes
    "need_plmn":"must register to plmn",                                                       // [ "enable", "disable" ]
                                                                                                    // "enable" requires PLMN registration
                                                                                                    // "disable" skips PLMN registration check
    "need_signal":"signal must be valid",                                                      // [ "enable", "disable" ]
                                                                                                    // "enable" requires valid signal strength
                                                                                                    // "disable" skips signal check
    "signal_failed_threshold":"first failed to reset time",                                    // [ number ], default 120 seconds
    "signal_failed_threshold2":"second failed to reset time",                                  // [ number ], default 300 seconds
    "signal_failed_threshold3":"third failed to reset time",                                   // [ number ], default 600 seconds
    "signal_failed_everytime":"every failed to reset time",                                    // [ number ], default 1800 seconds

    // attach detection attributes
    "need_attach":"must attach succeed",                                                       // [ "enable", "disable" ]
                                                                                                    // "enable" requires successful network attachment
                                                                                                    // "disable" skips attach check
    "attach_failed_threshold":"first failed to reset time",                                    // [ number ], default 60 seconds
    "attach_failed_threshold2":"second failed to reset time",                                  // [ number ], default 180 seconds
    "attach_failed_threshold3":"third failed to reset time",                                   // [ number ], default 600 seconds
    "attach_failed_everytime":"every failed to reset time",                                    // [ number ], default 1800 seconds

    // IPv4
    "tid":"table identify number",            // [ number ] exclusive route table ID, only for multiple WAN
    "metric":"default route metric",          // [ number  ]
    "mode":"IPV4 address mode",               // [ "dhcpc" ] DHCP client mode, [ "static" ] manual setting, [ "ppp" ] PPP dial
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
        "dns":"Custom DNS1",                                       // [ ip address ], This is valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], This is valid when "custom_dns" is "enable"
    },
    "ppp":                                    // detail configuration for "mode" is "ppp"
    {
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
    "automatic":                              // detail configuration for "method" is "automatic"
    {
        "mode":"mode for get the ipv6",                  // [ "try", "force", "disable" ]
        "prefix":"ipv6-prefix of length for request",    // [ "auto", "48", "52", "56", "60", "60", "disable" ]
        "custom_resolve":"Custom DNS",                   // [ "disable", "enable" ]
        "resolve":"Custom DNS1",                         // [ ipv6 address ], This is valid when "custom_resolve" is "enable"
        "resolve2":"Custom DNS2"                         // [ ipv6 address ], This is valid when "custom_resolve" is "enable"
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

        "action":"action when keeplive fails",  // [ "reboot" ] reboot the system
                                                // [ "reset" ] reset the modem
                                                // [ others ] redial the connection
        "icmp":                                                             // detail configuration for "type" is "icmp"
        {
            "dest":                                                           // destination address for ICMP keeplive
            {
                "destination identify2":"destination address1",                        // [ string ]:[ IP address ]
                // "...":"..." You can configure multiple destination IP addresses. If only one PING echo packet is returned, the detection succeeds. If no PING echo packet is returned, the detection fails  
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

    // Configure connect failed to action
    "failed_threshold":"first failed to reset time",                                   // [ number ]
    "failed_threshold2":"second failed to reset time",                                 // [ number ]
    "failed_threshold3":"third failed to reset time",                                  // [ number ]
    "failed_everytime":"every failed to reset time"                                    // [ number ]
}
```   

Example, show all configuration of first LTE   
```shell
ifname@lte
{
    // PDP profile
    "profile":"enable",                # custom the APN profile
    "profile_cfg":
    {
        "dial":"*99#",                     # dial number is *99#
        "type":"ipv4v6",                   # ip address type is ipv4 and ipv6
        "apn":"internet",                  # APN is internet
        "user":"card",                     # username is card
        "passwd":"card"                    # password is card
    }

    "mode":"ppp",                      # PPP mode
    "ppp":                                 # ppp configure will be used when "mode" is ppp
    {
        "lcp_echo_interval":"10",          # LCP echo interval is 10 second
        "lcp_echo_failure":"12"            # LCP echo failure times is 12
    },
    "masq":"enable",                                 # out stream share the interface IPv4 address to access the Internet

    "method":"slaac",                                # IPv6 address mode is slaac

    "keeplive":                                      # keeplive mechanism configure save here
    {
        "type":"recv",                               # use count receive packet to keeplive
        "recv":                                      # If no 1 packet is received within 20X30 second(10 minutes), the link is considered unavailable
        {
            "timeout":"20",
            "failed":"30",
            "packets":"1"
        }
    }
}
```   

Example, show all configuration of second LTE/NR   
```shell
ifname@lte2
{
    // PDP profile
    "profile":"enable",                # custom the APN profile
    "profile_cfg":
    {
        "dial":"*99#",                     # dial number is *99#
        "type":"ipv4v6",                   # ip address type is ipv4 and ipv6
        "apn":"internet",                  # APN is internet
        "user":"card",                     # username is card
        "passwd":"card"                    # password is card
    }

    "mode":"ppp",                      # PPP mode
    "ppp":                                 # ppp configure will be used when "mode" is ppp
    {
        "lcp_echo_interval":"10",          # LCP echo interval is 10 second
        "lcp_echo_failure":"12"            # LCP echo failure times is 12
    },
    "masq":"enable",                                 # out stream share the interface IPv4 address to access the Internet

    "method":"slaac",                                # IPv6 address mode is slaac

    "keeplive":                                      # keeplive mechanism configure save here
    {
        "type":"dns",                               # use count dns to keeplive
        "dns":                                      # detect DNS timeout 8 seconds; after 4 failures report failure; sleep 5 seconds after detection succeeds
        {
            "timeout":"8",
            "failed":"4",
            "interval":"5"
        }
    }
}
```   

Example, modify the keeplive to icmp for first LTE network  
```shell
ifname@lte:keeplive/type=icmp
ttrue
```   

Example, modify the icmp keeplive destination address for first LTE network  
```shell
ifname@lte:keeplive/icmp/dest/test=8.8.8.8            # modify the icmp keeplive first destination address to 8.8.8.8
ttrue
ifname@lte:keeplive/icmp/dest/test2=8.8.4.4           # modify the icmp keeplive second destination address to 8.8.4.4 
ttrue
ifname@lte:keeplive/icmp/dest/test3=114.114.114.114   # modify the icmp keeplive third destination address to 114.114.114.114
ttrue
# You can also complete the above three commands with one JSON update
ifname@lte:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   

Example, modify the mode to ppp for first LTE network  
```shell
ifname@lte:mode=ppp
ttrue
```   

Example, enable custom profile and set APN
```shell
ifname@lte:profile=enable
ttrue
ifname@lte:profile_cfg/apn=NewAPN
ttrue
```   

You can also complete the above commands with one JSON update
```shell
ifname@lte|{"profile":"enable","profile_cfg":{"apn":"NewAPN"}}
ttrue
```   

Example, disable the first LTE network   
```shell
ifname@lte:status=disable
ttrue
```     

Example, disable the second LTE network
```shell
ifname@lte2:status=disable
ttrue
```


### Component API
**Directly callable** APIs: `ifname@lte.method`, `ifname@lte2.method`, …

**ifname@lte** is first LTE network  
**ifname@lte2** is second LTE/NR network

+ `status[]` **get LTE network information**   
    - failed: return `NULL`
    - error: return `terror`   
    - success: return JSON status information   
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"Current state",        // [ "nodevice", "reset", "setup", "register", "idle", "noimsi", "noimei", "uping", "block", "up", "failed", "down" ]
                                             // "nodevice" for the corresponding module could not be found
                                             // "reset" means modem is being reset
                                             // "setup" means modem is being initialized
                                             // "register" means modem is registering to network
                                             // "idle" means modem is alive but temporarily not ready for dialing
                                             // "noimsi" means IMSI lock check failed or unavailable
                                             // "noimei" means IMEI lock check failed or unavailable
                                             // "uping" for connecting
                                             // "block" means waiting for keeplive checks to recover
                                             // "up" means ready for Internet access (signal/network/SIM are OK)
                                             // "failed" for keeplive failed
                                             // "down" for the modem is down

        "mode":"IPV4 address mode",     // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting, [ "ppp" ] for PPP dial
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
        "rx_bytes":"received bytes",      // [ number ]
        "rx_packets":"received packets",  // [ number ]
        "tx_bytes":"sent bytes",          // [ number ]
        "tx_packets":"sent packets",      // [ number ]
        "mac":"MAC address",            // [ mac address ]

        "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], optional, present when IPv6 is enabled
                                            // "manual" for manual setting
                                            // "automatic" for DHCPv6
                                            // "slaac" for Stateless address autoconfiguration
        "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable

        // For LTE/NR baseband Status, the parameters are the same as modem@lte or modem@lte2
        "imei":"IMEI number",           // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                // number for iccid
                                                // "nosim" means no SIM card detected
                                                // "pin" for the simcard need PIN code
                                                // "puk" for the simcard pin error
        "plmn":"MCC and MNC",           // [ number, "noreg", "unreg", "dereg" ]
                                                // number for MCC and MNC
                                                // "noreg" means cannot register to operator
                                                // "unreg" means currently unregistered
                                                // "dereg" means registration rejected by operator
        "name":"modem name",             // [ string ], lte modem model or name
        "operator":"operator name",      // [ string ]
        "nettype":"network type",        // The format varies depending on the module
                                         // 2G usually shows GSM, GPRS, EDGE, CDMA
                                         // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G usually shows LTE, FDD, TDD
        "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" no signal, "1" weakest, "4" strongest
        "rssi":"signal intensity",       // [ number ], the unit is dBm
        "csq":"CSQ number",              // [ number ], Optional
        "rsrp":"RSRP value",             // [ string ], Optional, The format varies depending on the module
        "rsrq":"RSRQ value",             // [ string ], Optional, The format varies depending on the module
        "sinr":"sinr value",             // [ string ], Optional, The format varies depending on the module  
        "band":"current band",           // [ string ], Optional, The format varies depending on the module
        "ci":"cell identity",            // [ string ], Optional
        "lac":"location area code",      // [ string ], Optional
        "channel":"location area code"   // [ string ], Optional    
    }
    ```   

    Example, get the first LTE network information
    ```shell
    ifname@lte.status
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
    }
    ```   

    Example, get the second LTE network information
    ```shell
    ifname@lte2.status
    {
        "status":"up",                     # connect is succeed

        "mode":"dhcpc",                    # IPv4 connect mode is DHCP
        "netdev":"usb0",                   # netdev is usb0
        "gw":"10.137.89.154",              # gateway is 10.137.89.118
        "dns":"114.114.114.114",           # dns is 114.114.114.114
        "dns2":"8.8.8.8",                  # backup dns is 8.8.8.8
        "ip":"10.137.89.117",              # ip address is 10.137.89.117
        "mask":"255.255.255.252",          # network mask is 255.255.255.252
        "livetime":"00:15:50:0",           # already online 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00

        "method":"slaac",                  # IPv6 address mode is slaac
        "addr":"fe80::50:f4ff:fe00:0",     # local IPv6 address is fe80::50:f4ff:fe00:0

        "imei":"867160040494084",          # imei is 867160040494084
        "imsi":"460015356123463",          # imsi is 460015356123463
        "iccid":"89860121801097564807",    # iccid is 89860121801097564807
        "csq":"3",                         # CSQ number is 3
        "signal":"3",                      # signal level is 3
        "plmn":"46001",                    # plmn is 46001
        "nettype":"WCDMA",                 # nettype is WCDMA
        "rssi":"-107",                     # signal intensity is -107
        "operator":"China Unicom"          # operator name is China Unicom
    }
    ```   


+ `netdev[]` **get the netdev**  
    - failed: return `NULL`
    - error: return `terror`   
    - success: return netdev string  

    Example, get the first LTE network netdev
    ```shell
    ifname@lte.netdev
    usb0
    ```   

+ `ifdev[]` **get the ifdev**   
    - failed: return `NULL`
    - error: return `terror`   
    - success: return ifdev component name  

    Example, get the first LTE network ifdev
    ```shell
    ifname@lte.ifdev
    modem@lte
    ```   

+ `operator[]` **get operator/profile information from ifdev**
    - failed: return `NULL`
    - error: return `terror`
    - success: return operator-related info from modem component

    Example, get operator info of the first LTE network
    ```shell
    ifname@lte.operator
    {
        // modem-dependent fields, typically include operator/APN profile information
    }
    ```

+ `reset[]` **reset LTE modem through ifdev**
    - failed: return `tfalse`
    - error: return `terror`
    - success: return `ttrue`

    Example, reset the first LTE modem
    ```shell
    ifname@lte.reset
    ttrue
    ```

+ `lock_imei[]` **set or query modem IMEI lock rule**
    - failed: return `NULL`
    - error: return `terror`
    - success: return modem lock result (implementation-dependent)

    Example, lock IMEI for first LTE
    ```shell
    ifname@lte.lock_imei|{"value":"enable"}
    ttrue
    ```

+ `lock_imsi[]` **set or query modem IMSI lock rule**
    - failed: return `NULL`
    - error: return `terror`
    - success: return modem lock result (implementation-dependent)

    Example, lock IMSI for first LTE
    ```shell
    ifname@lte.lock_imsi|{"value":"enable"}
    ttrue
    ```

+ `custom_set[]` **send custom AT/driver settings to modem**
    - failed: return `NULL`
    - error: return `terror`
    - success: return modem response (implementation-dependent)

    Example
    ```shell
    ifname@lte.custom_set|{"cmd":"AT+QCFG=\"nwscanmode\",3,1"}
    ```

+ `custom_watch[]` **query custom watch values from modem**
    - failed: return `NULL`
    - error: return `terror`
    - success: return modem response (implementation-dependent)

    Example
    ```shell
    ifname@lte.custom_watch|{"cmd":"AT+QNWINFO"}
    ```

+ `shut[]` **shutdown the modem network**   
    - failed: return `tfalse`
    - error: return `terror`   
    - success: return `ttrue`

    Example, shutdown the first LTE network
    ```shell
    ifname@lte.shut
    ttrue
    ```   
    Example, shutdown the second LTE network
    ```shell
    ifname@lte2.shut
    ttrue
    ```   

+ `setup[]` **setup the modem network**   
    - failed: return `tfalse`
    - error: return `terror`   
    - success: return `ttrue`

    Example, setup the first LTE network
    ```shell
    ifname@lte.setup
    ttrue
    ```   
    Example, setup the second LTE network
    ```shell
    ifname@lte2.setup
    ttrue
    ```

### Lifecycle API
+ `setup[]` / `shut[]` — same entries as under **Component API**. The reference **ifname** package does not schedule **`init`/`uninit`** for **`ifname@lte`**; modem bring-up is driven by the **network** stack or product integration.

### Joint Handlers
+ `keepon[]` **clear the connect failed counter**   
    - success: return `ttrue`
    - called when network connection is confirmed alive
    - resets the internal `connect_failed` counter to prevent unnecessary modem reset

    Example, clear the connect failed counter for first LTE network
    ```shell
    ifname@lte.keepon
    ttrue
    ```   

+ `keepoff[]` **handle keeplive check failure**   
    - success: return `ttrue`
    - performs configured action when keeplive check fails
    - action depends on `keeplive/action` configuration:
      - `"reboot"`: reboot the system (if uptime > 180s)
      - `"reset"`: reset the modem via ifdev
      - others: reset the connection

    Example, handle keeplive failure for first LTE network
    ```shell
    ifname@lte.keepoff
    ttrue
    ```   


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_ifname_lte(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "ifname@lte", "status") == NULL)
        return -1;
    ok = ssets_string("ifname@lte", "value", "status");
    return ok ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* Example: scall("ifname@lte", "status", NULL); then talk_free if JSON */
```
