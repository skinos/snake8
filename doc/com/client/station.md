***
## Management of all local client   
Management of all local client

#### Configuration( client@station )   
```json
// Attributes introduction 
{
    "client MAC address":                                // [ MAC address ]
    {
        "ifname":"specify on interface name",                 // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], default the "ifname@lan"
        "name":"specify hostname",                            // [ string ]
        "bindip":"specify ip address on dhcp assignment",     // [ ip address ]
        "arpbind":"bind ip set the arp table",                // [ "disable", "enable" ]
        "lease":"specify lease on dhcp assignment"            // [ number ] the unit is second

        "acl":"acl state",                                          // [ "disable", "enable" ]
        "acl_rule"                                                  // acl rule in here
        {
            "rule name":                                                 // [ string ], user can custom the rule name
            {
                "action":"drop or accept",                               // [ "drop", "accept", "return" ]
                                                                                  // "drop" for forbid
                                                                                  // "accept" for pass
                                                                                  // "return" for no more matching
                "proto":"protocol type",                                 // [ "domain", "key", "tcp", "udp", "all", "layer7" ]
                                                                                  // "domain", for domain, "dest" well be domain
                                                                                  // "tcp", for tcp protocol
                                                                                  // "udp", for udp protocol
                                                                                  // "layer7, for layer7 application
                                                                                  // "all", or space or none for all protocol
                "dest":"internet destination address",                   // [ string ] packets destination
                                                                                  // single IP: 202.96.11.32, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1,72.32,192.1, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // range of IP: 202.96.132.11-202.96.132.20, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // domain: www.baidu.com, vaild when "proto" be "domain"
                                                                                  // layer7: Future expansion, vaild when "proto" be "layer7"
                                                                                  // space for all ip address, vaild when "proto" be "tcp" or "udp" or "all"
                "destport":"internet destination port",                  // [ number ] valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space or none for all port
                "key":"keyword",                                         // [ string ] matching of keyword in packets, valid when "proto" be "tcp" or "udp" or "all"

                "timer":"specifying an effective time",                  // [ "disable", "enable" ]
                "timer_cfg":                                                 // effective time, valid when "status" be "enable"
                {
                    "datestart":"starting date",                             // [ string ], format is YYYY-MM-DD
                    "datestop":"ending date",                                // [ string ], format is YYYY-MM-DD
                    "timestart":"start time of day",                         // [ string ], format is hh-mm-ss
                    "timestop":"end time of day",                            // [ string ], format is hh-mm-ss
                    "weekdays":"designated week number"                      // [ string ], format is 1,2,3,..., 0 for Sunday
                }
            }
            // ... more rule
        }

    }
    // more client MAC address rule
}
```   

Examples, bind ip 192.168.31.222 for 00:51:45:CB:78:80
```shell
client@station:00:51:45:CB:78:80/bindip=192.168.31.222
ttrue
```

Examples, clear the bind ip for 00:51:45:CB:78:89
```shell
client@station:00:51:45:CB:78:89/bindip=
ttrue
```

#### **Methods**

+ `list[ [local ifname] ]` **list current all client infomation**   
    - [local ifname] ------ [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], Specifying the ifname
    - return NULL when failed
    - return terror when error
    - return json to describes all client infomation when succeed
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "name":"client name",                 // [ string ]
            "ip":"ip address",                    // [ IP address ]
            "ifname":"connected ifname",          // [ "ifname@lan", "ifname@lan2", ... ]
            "livetime":"connected time"           // [ string ], format is hour:minute:second:day, offline when no such property
            // ... more client attribute
        }
        // ... more client
    }
    ```   

    Example, list current all client
    ```shell
    client@station.list
    {
        "04:CF:8C:39:91:7A":            # first client
        {
            "name":"xiaomi-aircondition-ma2_mibt917A"     # hostname is xiaomi-aircondition-ma2_mibt917A
            "ip":"192.168.31.140",                        # ip is 192.168.31.140
            "ifname":"ifname@lan"                         # the client from the ifname@lan
        },
        "40:31:3C:B5:6D:4C":            # second client
        {
            "ip":"192.168.31.61",
            "ifname":"ifname@lan",                        # the client from the ifname@lan
            "name":"minij-washer-v5_mibt6D4C",
            "livetime":"14:39:34:1"                       # livetime is 1 day 14 hour 39 minute 34 second
        },
        "14:13:46:C9:97:C7":            # third client
        {
            "ip":"192.168.31.9",
            "ifname":"ifname@lan",                        # the client from the ifname@lan
            "livetime":"14:39:26:1"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "ifname":"ifname@lan2",                       # the client from the ifname@lan2
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "ifname":"ifname@lan2",                        # the client from the ifname@lan2
            "name":"Xiaomi-13-Ultra",
            "livetime":"00:00:36:0"
        }
    }
    ```   

    Example, list all client of ifname@lan2
    ```shell
    client@station.list[ ifname@lan2 ]
    {
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "name":"Xiaomi-13-Ultra",
            "livetime":"00:00:36:0"
        }
    }
    ```


