## ifname@lte — LTE/NR Network Management

### Overview

Manage LTE/NR networks and 4G/NR baseband links. It pairs a **logical LTE interface** here with **modem management** (`modem@lte`, … — see [`../modem/lte.md`](../modem/lte.md)) and the **network framework** for uplink scheduling ([`../network/frame.md`](../network/frame.md)).
Usually `ifname@lte` is the first LTE/NR network instance. If there are multiple LTE/NR modems, `ifname@lte2` is the second instance, and numbering increases sequentially.

- manages LTE/NR interface lifecycle: setup, shutdown, status query
- supports PPP and DHCP client IPv4 addressing
- provides SIM card detection, PLMN registration, signal strength monitoring
- supports backup SIM card failover with configurable thresholds
- proxies modem-specific APIs: operator, reset, lock_imei, lock_imsi, custom_set, custom_watch
- provides unified configuration view: modem-side configs (sms, gnss, atport, lock_*, custom_*, watch_interval) are accessible through ifname@lte and automatically forwarded to modem@lte, allowing users to manage the entire LTE device from a single interface



### Network Architecture

`ifname@lte` is an **extern interface** registered by `network@frame` during boot. It uses `ifname@ltecon` as concom and `modem@lte` as ifdev. It provides a unified configuration view: modem-side configs (`sms`, `gnss`, `atport`, `lock_*`, `custom_*`, `watch_interval`) are stored in `modem@lte` but accessible and settable through `ifname@lte`. Modem-specific APIs (`operator`, `reset`, `lock_imei`, etc.) are also proxied. As an extern interface, it is subject to multi-uplink scheduling and publishes `network/onextern` / `network/offextern` joint events.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( ifname@lte )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ]

    // Profile attributes for LTE modem PDP
    "pin":"simcard pin",                                         // [ string ]
    "profile":"use custom PDP profile",                          // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile settings, used when "profile" is "enable"
    {
        "dial":"dial number",                     // [ number ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // [ "auto", "disable", "pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password"                  // [ string ]
    },

    // Backup simcard configure
    "bsim":"backup simcard function",                            // [ "disable", "enable" ]
    "bsim_cfg":                               // backup SIM settings, used when "bsim" is "enable"
    {
        "mode":"specify active SIM card",                           // [ "auto", "back", "main", "detect" ]
                                                                        // "auto" for automatic switching based on rules
                                                                        // "back" for backup simcard
                                                                        // "main" for main simcard
                                                                        // "detect" the IO for auto that need detect IO support
        "simcard_failed_threshold":"first failed time to switch",   // [ number ], default 60 seconds
        "simcard_failed_threshold2":"second failed time to switch", // [ number ], default 180 seconds
        "simcard_failed_threshold3":"third failed time to switch",  // [ number ], default 300 seconds
        "simcard_failed_everytime":"every failed time to switch",   // [ number ], default 1800 seconds
        "signal_failed_threshold":"first failed time to switch",    // [ number ], default 120 seconds
        "signal_failed_threshold2":"second failed time to switch",  // [ number ], default 300 seconds
        "signal_failed_threshold3":"third failed time to switch",   // [ number ], default 600 seconds
        "signal_failed_everytime":"every failed time to switch",    // [ number ], default 1800 seconds
        "attach_failed_threshold":"first failed time to switch",    // [ number ], default 60 seconds
        "attach_failed_threshold2":"second failed time to switch",  // [ number ], default 180 seconds
        "attach_failed_threshold3":"third failed time to switch",   // [ number ], default 600 seconds
        "attach_failed_everytime":"every failed time to switch",    // [ number ], default 1800 seconds
        "failed_threshold":"first failed time to switch",           // [ number ]
        "failed_threshold2":"second failed time to switch",         // [ number ]
        "failed_threshold3":"third failed time to switch",          // [ number ]
        "failed_everytime":"every failed time to switch",           // [ number ]
        "failover":"backup simcard usage duration",                 // [ number ], the unit is second
        "keeplive_switch":"keeplive failed to switch",              // [ "disable", "enable" ]
        "pin":"simcard pin",                                        // [ string ]
        "profile":"custom the profile",                             // [ "disable", "enable" ]
        "profile_cfg":                             // custom profile save here
        {
            "dial":"dial number",                     // [ number ]
            "cid":"dial CID",                         // [ number ], default is 1
            "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
            "apn":"APN name",                         // [ string ]
            "user":"user name",                       // [ string ]
            "passwd":"user password",                 // [ string ]
            "auth":"authentication method",           // [ "auto", "disable", "pap", "chap", "papchap" ]
        }
    },

    // SIM card detection attributes
    "need_simcard":"SIMcard must be detected",                   // [ "enable", "disable" ]
    "simcard_failed_threshold":"first failed to reset time",     // [ number ], default 60 seconds
    "simcard_failed_threshold2":"second failed to reset time",   // [ number ], default 180 seconds
    "simcard_failed_threshold3":"third failed to reset time",    // [ number ], default 300 seconds
    "simcard_failed_everytime":"every failed to reset time",     // [ number ], default 1800 seconds

    // Signal/PLMN detection attributes
    "need_plmn":"must register to plmn",                         // [ "enable", "disable" ]
    "need_signal":"signal must be valid",                        // [ "enable", "disable" ]
    "signal_failed_threshold":"first failed to reset time",      // [ number ], default 120 seconds
    "signal_failed_threshold2":"second failed to reset time",    // [ number ], default 300 seconds
    "signal_failed_threshold3":"third failed to reset time",     // [ number ], default 600 seconds
    "signal_failed_everytime":"every failed to reset time",      // [ number ], default 1800 seconds

    // Attach detection attributes
    "need_attach":"must attach succeed",                         // [ "enable", "disable" ]
    "attach_failed_threshold":"first failed to reset time",      // [ number ], default 60 seconds
    "attach_failed_threshold2":"second failed to reset time",    // [ number ], default 180 seconds
    "attach_failed_threshold3":"third failed to reset time",     // [ number ], default 600 seconds
    "attach_failed_everytime":"every failed to reset time",      // [ number ], default 1800 seconds

    // IPv4
    "tid":"table identify number",            // [ number ], exclusive route table ID, only for multiple WAN
    "metric":"default route metric",          // [ number ]
    "mode":"IPV4 address mode",               // [ "dhcpc", "static", "ppp" ]
                                                   // "dhcpc" for DHCP client
                                                   // "static" for manual setting
                                                   // "ppp" for PPP dial
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
    "ppp":                                    // detail configuration for "mode" is "ppp"
    {
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
    "automatic":                              // detail configuration for "method" is "automatic"
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
        "action":"action when keeplive fails",  // [ "reboot", "reset", "redial" ]
        "icmp":                                                             // detail configuration for "type" is "icmp"
        {
            "dest":                                                           // destination address for ICMP keeplive
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

    // Configure connect failed to action
    "failed_threshold":"first failed to reset time",                                   // [ number ]
    "failed_threshold2":"second failed to reset time",                                 // [ number ]
    "failed_threshold3":"third failed to reset time",                                  // [ number ]
    "failed_everytime":"every failed to reset time",                                   // [ number ]

    // Modem-side configuration (forwarded to modem@lte for unified management)
    // These fields belong to modem@lte but are accessible through ifname@lte
    // Setting these fields will automatically forward to modem@lte and reset the modem if changed
    "sms":"SMS function status",                                 // [ "disable", "enable" ]
    "gnss":"GNSS function status",                               // [ "disable", "enable" ]
    "atport":"AT port function status",                          // [ "disable", "enable" ]
    "lock_nettype":"preferred RAT lock policy",                  // [ "auto", "2g", "3g", "4g", "nsa", "sa" ]
    "lock_imei":"lock IMEI function",                            // [ "disable", "enable", "specific imei string" ]
    "lock_imsi":"lock IMSI function",                            // [ "disable", "enable", "specific imsi string" ]
    "custom_set":                             // custom AT commands to execute during modem setup
    {
        "custom name":"AT command"             // [ string ]: [ string ]
        // "...":"..."  How many commands show how many properties
    },
    "custom_watch":                           // custom AT commands to execute periodically during modem watch
    {
        "custom name":"AT command"             // [ string ]: [ string ]
        // "...":"..."  How many commands show how many properties
    },
    "watch_interval":"modem watch interval",                     // [ number ], the unit is second, default 8
}
```

#### Configuration example

Example, show all configuration of first LTE (includes modem-side configs)
```shell
ifname@lte
{
    "profile":"enable",                # custom the APN profile
    "profile_cfg":
    {
        "dial":"*99#",                     # dial number is *99#
        "type":"ipv4v6",                   # ip address type is ipv4 and ipv6
        "apn":"internet",                  # APN is internet
        "user":"card",                     # username is card
        "passwd":"card"                    # password is card
    },
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
    },
    "sms":"enable",                                  # enable SMS function
    "gnss":"enable",                                 # enable GNSS function
    "atport":"enable",                               # enable AT port function
    "watch_interval":"8",                            # watch interval is 8 seconds
    "custom_set":                                    # custom AT commands to execute during setup
    {
        "1":"AT+COPS=3,2",
        "2":"AT+CPIN=1234"
    }
}
```

#### Configuration settings example

Example, modify the keeplive to icmp for first LTE network
```shell
ifname@lte:keeplive/type=icmp
ttrue
```

Example, modify the mode to ppp for first LTE network
```shell
ifname@lte:mode=ppp
ttrue
```

Example, merge set the first LTE configure( include "profile" "profile_cfg" )
```shell
ifname@lte|{"profile":"enable","profile_cfg":{"apn":"NewAPN"}}
ttrue
```

Example, enable SMS function
```shell
ifname@lte:sms=enable
ttrue
```

Example, enable GNSS function
```shell
ifname@lte:gnss=enable
ttrue
```

Example, set watch interval for periodic modem monitoring
```shell
ifname@lte:watch_interval=10
ttrue
```

Example, set custom AT commands to execute on modem setup
```shell
ifname@lte:custom_set|{"1":"AT+COPS=3,2","2":"AT+CPIN=1234"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **setup the modem network**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the modem network**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get LTE network information**
    - failed return NULL
    - succeed return [ json ], LTE network status information
    ```json
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
                                             // "up" means ready for Internet access
                                             // "failed" for keeplive failed
                                             // "down" for the modem is down
        "mode":"IPV4 address mode",     // [ "dhcpc", "static", "ppp" ]
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
        "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable
        "imei":"IMEI number",           // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ string, "nosim", "pin", "puk" ]
                                            // string for iccid
                                            // "nosim" means no SIM card detected
                                            // "pin" for the simcard need PIN code
                                            // "puk" for the simcard pin error
        "plmn":"MCC and MNC",           // [ string, "noreg", "unreg", "dereg" ]
                                            // string for MCC and MNC
                                            // "noreg" means cannot register to operator
                                            // "unreg" means currently unregistered
                                            // "dereg" means registration rejected by operator
        "name":"modem name",            // [ string ], lte modem model or name
        "operator":"operator name",     // [ string ]
        "nettype":"network type",       // [ string ]
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], "0" no signal, "1" weakest, "4" strongest
        "rssi":"signal intensity",      // [ number ], the unit is dBm
        "csq":"CSQ number",             // [ number ], Optional
        "rsrp":"RSRP value",            // [ string ], Optional
        "rsrq":"RSRQ value",            // [ string ], Optional
        "sinr":"sinr value",            // [ string ], Optional
        "band":"current band",          // [ string ], Optional
        "ci":"cell identity",           // [ string ], Optional
        "lac":"location area code",     // [ string ], Optional
        "channel":"channel"             // [ string ], Optional
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

+ `netdev[]` **get the netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the first LTE network netdev
    ```shell
    ifname@lte.netdev
    usb0
    ```

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - succeed return [ string ], the ifdev component name

    Example, get the first LTE network ifdev
    ```shell
    ifname@lte.ifdev
    modem@lte
    ```

+ `operator[]` **get operator/profile information from ifdev**
    - failed return NULL
    - succeed return [ json ], operator-related info from modem component
    ```json
    {
        "operator":"operator name",      // [ string ]
        "apn":"APN name"                 // [ string ]
    }
    ```

    Example, get operator info of the first LTE network
    ```shell
    ifname@lte.operator
    {
        "operator":"China Unicom",
        "apn":"internet"
    }
    ```


#### Control APIs

+ `reset[]` **reset LTE modem through ifdev**
    - failed return tfalse
    - succeed return ttrue

    Example, reset the first LTE modem
    ```shell
    ifname@lte.reset
    ttrue
    ```

+ `lock_imei[ value ]` **set or query modem IMEI lock rule**
    - value ------------ [ json ], optional, lock configuration
    - failed return NULL
    - succeed return ttrue

    Example, lock IMEI for first LTE
    ```shell
    ifname@lte.lock_imei|{"value":"enable"}
    ttrue
    ```

+ `lock_imsi[ value ]` **set or query modem IMSI lock rule**
    - value ------------ [ json ], optional, lock configuration
    - failed return NULL
    - succeed return ttrue

    Example, lock IMSI for first LTE
    ```shell
    ifname@lte.lock_imsi|{"value":"enable"}
    ttrue
    ```

+ `custom_set[ cmd ]` **send custom AT/driver settings to modem**
    - cmd -------------- [ json ], custom command configuration
    - failed return NULL
    - succeed return [ string ], modem response

    Example, send custom AT command
    ```shell
    ifname@lte.custom_set|{"cmd":"AT+QCFG=\"nwscanmode\",3,1"}
    ```

+ `custom_watch[ cmd ]` **query custom watch values from modem**
    - cmd -------------- [ json ], custom command configuration
    - failed return NULL
    - succeed return [ string ], modem response

    Example, query custom AT command
    ```shell
    ifname@lte.custom_watch|{"cmd":"AT+QNWINFO"}
    ```


#### Other

+ `keepon[]` **clear the connect failed counter**
    - succeed return ttrue
    - called when network connection is confirmed alive
    - resets the internal connect_failed counter to prevent unnecessary modem reset

    Example, clear the connect failed counter for first LTE network
    ```shell
    ifname@lte.keepon
    ttrue
    ```

+ `keepoff[]` **handle keeplive check failure**
    - succeed return ttrue
    - performs configured action when keeplive check fails
    - action depends on keeplive/action configuration:
        - "reboot": reboot the system (if uptime > 180s)
        - "reset": reset the modem via ifdev
        - others: reset the connection

    Example, handle keeplive failure for first LTE network
    ```shell
    ifname@lte.keepoff
    ttrue
    ```
