***
## Management of destination NAT
Management of destination NAT to proxy internet port on gateway for local client access

#### Configuration( forward@dnat )
```json
// Attributes introduction 
{

    "local interface name":       // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        "rule name":                    // [ string ], user can custom the rule name
        {
            "destip":"internet server ip address",    // [ ip address ]
            "destport":"internet server port",        // [ number ]
                                                                // signal port: 80
                                                                // range of port: 80-100
                                                                // multiple port: 80,8080,8000
            "protocol":"protocol type",               // [ "tcp", "udp", "tcpudp" ], "tcpudp" for TCP and UDP
            "targetport":"local proxy port"           // [ number ]
        },
        // ... more rule
    }
    // ... more local ifname
}
```

Example, show current all of dnat rule
```shell
forward@dnat
{
    "ifname@lan":                       # for the LAN
    {
        "proxy1":                          # rule name is proxy1
        {
            "destip":"29.23.11.35",        # internet server ip is 29.23.11.35
            "destport":"28-90",            # internet server port is start 28, end at 90 
            "protocol":"tcpudp",           # proxy tcp and udp
            "targetport":"100"             # proxy the server on port 100 at local interface of LAN
        }
    },
    "ifname@lan2":                      # for the LAN2
    {
        "forweb":                          # rule name is proxy1
        {
            "destip":"129.232.91.5",       # internet server ip is 129.232.91.5
            "destport":"80",               # internet server port is 80
            "protocol":"tcp",              # proxy tcp
            "targetport":"8000"            # proxy the server on port 8000 at local interface of LAN2
        }
    }    
}
```

Example, add a rule named proxy2 to ifname@lan
```shell
forward@dnat:ifname@lan/proxy2={"destip":"202.96.134.144","destport":"53","protocol":"tcpudp","targetport":"500"}
ttrue
```   

Example, modify destport of rule named proxy2 to 55
```shell
forward@dnat:ifname@lan/proxy2/destport=55
ttrue
```   

Example, delete a rule named proxy2 from ifname@lan
```shell
forward@dnat:ifname@lan/proxy2=
ttrue
```   
