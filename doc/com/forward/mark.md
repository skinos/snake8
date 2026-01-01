***
## Mark the packet（forward@mark） 
mark the packet with a mark id

#### Configuration( forward@mark )

```json
// Attributes introduction
{
    "rule name":                    // [ string ], user can custom the rule name
    {
        "markid":"mark the packet with this id",                       // [ number ], range of 1-4294967295
                                                                            // Less than 100 is reserved for the system
                                                                            // You are advised to use more than 100 for user-defined rules
        "src":"select the packet use source ip address",               // [ ip address ]
        "dest":"select the packet use destination ip address",         // [ ip address ]
        "protocol":"protocol type",                                    // [ "all", "tcp", "udp", "tcpudp" ], all for all protocol, tcpudp for tcp and udp
        "srcport":"source port",                                       // [ number ], default is all source port
        "destport":"destination port"                                  // [ number ], default is all destination port
    }
    // ... more rule
}
```

Example, show current all of mark rule
```shell
{
    
    "myCustom1":                       # rule name is myCustom1
    {                                  # mark that source address is 192.168.8.250 and destination address is 202.94.22.38 packet 300
        "markid":"300",
        "src":"192.168.8.250",
        "dest":"202.94.22.38"
    },
    "youCustom":                       # rule name is youCustom
    {                                  # mark that source address is 192.168.8.251 and tcp protocol source port is 1000-2000 and destination port is 80/8080 packet 301
        "markid":"301",
        "src":"192.168.8.251",
        "protocol":"tcp",
        "srcport":"1000-2000",
        "destport":"80,8080"
    }
}
```  
