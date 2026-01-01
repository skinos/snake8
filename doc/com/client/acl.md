***
## Management of Access control access
Management ACL of client from local interface   

#### Configuration( client@acl )
```json
// Attributes introduction 
{
    "interface name":                                     // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        // client access control list from this ifname
        "status":"enable or disable the access control list",       // [ "disable", "enable" ]
        "rule":                                                     // access control list, valid when "status" be "enable"
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

                "src":"local source address",                         // [ string ] packets from the "interface name"
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // signal MAC: 00:23:43:13:34:40
                                                                                  // space or none for all ip address
                "srcport":"local source port",                           // [ number ] valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space or none for all port
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
    // ... more interface
}
```   

Examples, show current all settings
```shell
client@acl
{
    "ifname@lan":
    {
        "status":"enable",                      # enable access control list
        "rule":
        {
            "dis163":                             # all client cannot access domain www.163.com at all time
            {
                "source":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.163.com",
                "timer":"enable",
                "timer_cfg":
                {
                    "destport":"",
                    "datestart":"",
                    "datestop":"",
                    "timestart":"00:00:00",
                    "timestop":"23:59:59",
                    "weekdays":"1,2,3,4,5,6,7"
                }
            },
            "dis164":                             # all client cannot access domain www.qq.com
            {
                "source":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.qq.com",
            }
        }
    }
}        
```

Examples, add a acl rule named "disqq", all the client on ifname ifname@lan cannot access domain www.qq.com
```shell
client@acl:ifname@lan/rule/disqq={"proto":"domain","dest":"www.qq.com","action":"drop"}
ttrue
```

Examples, delete a acl rule named "disqq"
```shell
client@acl:ifname@lan/rule/disqq=
ttrue
```


