***
## Management of System Firewall
Management of firewall to limit access from extern inteface( internet )

#### Configuration( forward@firewall )   
```json
// Attributes introduction 
{
    "extern interface name":       // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], above rules set at this interface name
    {
        "status":"disable or enable the firewall",          // [ "enable", "disable" ]
        "default":"action for default access",              // [ "drop", "accept" ]

        "input_icmp_through":"ICMP protocol access",        // [ "disable", "enable" ]
        "input_espah_through":"ESP/AH protocol access",     // [ "disable", "enable" ]
        "input_telnet_through":"TELNET Server access",      // [ "disable", "enable" ]
        "input_ssh_through":"SSH Server access",            // [ "disable", "enable" ]
        "input_wui_through":"WEB Server access",            // [ "disable", "enable" ]

        "nat_through":"NAT rule settings at forward@nat  passthrough auto",    // [ "disable", "enable" ]
        "forward_icmp_through":"ICMP protocol passthrough",                    // [ "disable", "enable" ]
        "forward_espah_through":"ESP/AH protocol passthrough",                 // [ "disable", "enable" ]

        "rule":                              // firewall rule settings
        {
            "rule name":                                   // [ string ], user can custom the rule name
            {
                "action":"drop or accept or return",                     // [ "drop", "accept", "return" ], "drop" for forbid, "accept" for pass, "return" for don't match it with after rule
                "src":"source address",                                  // [ string ]:
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // signal MAC: 00:23:43:13:34:40
                                                                                  // space for all ip address
                "protocol":"protocol type",                              // [ "tcp", "udp", "all" ]
                "dest":"destination address",                            // [ string ]:
                                                                                  // single IP: 202.96.11.32
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1,72.32,192.1
                                                                                  // range of IP: 202.96.132.11-202.96.132.20
                                                                                  // space for all ip address

                "destport":"destination port"                           // [ number ]: valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space for all port
            }
            // ... more rule
        }
    }
    // ... more extern interface
}
```  

Example, show current all of firewall settings
```shell
forward@firewall
{
    "ifname@lte":                 // first LTE network firewall settings
    {
        "status":"enable",             // enable the firewall
        "default":"drop",              // default action is drop
        "rule":
        {
            "pcweb":                        // rule named "pcweb", accept dest 192.168.8.222 and destport TCP 80 access
            {
                "action":"accept",
                "dest":"192.168.8.222",
                "protocol":"tcp",
                "destport":"80"
            }
        }
    },
    "ifname@lte2":               // second NR/LTE network firewall settings
    {
        "status":"disable",           // disable the firewall
        "default":"drop",
        "rule":
        {
        }
    }
}
```

Example, add the rule named webwork for ifname@lte, that make 113.23.64.28 can access the gateway web port 80 from ifname@lte
```shell
forward@firewall:ifname@lte/rule/webwork={"action":"accept","src":"113.23.64.28","protocol":"tcp","destport":"80"}
ttrue
```

Example, add the rule named webpass for ifname@lte, that make 113.23.64.28 can access the web port 80 of 192.168.8.250 from ifname@lte
```shell
forward@firewall:ifname@lte/rule/webpass={"action":"accept","src":"113.23.64.28","protocol":"tcp","dest":"192.168.8.250","destport":"80"}
ttrue
```

Example, enable the first LTE network firewall
```shell
forward@firewall:ifname@lte/status=enable
ttrue
```

Example, modify default action from the first LTE network to drop
```shell
forward@firewall:ifname@lte/default=drop
ttrue
```

Example, delete the rule named webwork for ifname@lte
```shell
forward@firewall:ifname@lte/rule/webwork=
ttrue
```

Example, delete the rule named webpass for ifname@lte
```shell
forward@firewall:ifname@lte/rule/webpass=
ttrue
```
