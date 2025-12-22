***
## Accept local network management
Accept administrative command from the local network( defualt is ifname@lan )

#### Configuration( agent@local )
```json
{
    "broadcast":"disable or enable listen for broadcast command from the local",  // [ "disable", "enable" ]
    "broadcast_port":"broadcast port",                                            // [ number ], default is 22222
    "broadcast_group":"group name",                                               // [ string ], default is default
    "broadcast_command":"broadcast command mode",                                 // [ "disable", "query", "all" ], default is disable
                                                                                        // "disable" for no broadcast command suport
                                                                                        // "query" for query broadcast command suport
                                                                                        // "all" for all broadcast command suport
    "broadcast_manager":                                                          // Only the specified IP address or MAC address is allowed for access
    {
        // "...":"..." You can configure multiple host who can access
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    },

    "json":"disable or enable listen for JSON command from the local",            // [ "disable", "enable" ]
    "json_port":"JSON port",                                                      // [ number ], defaultis 22220
    "json_command":"json command mode",                                           // [ "query", "all" ], default is query
    "json_manager":                                                               // Only the specified IP address or MAC address is allowed for access
    {
        // "...":"..." You can configure multiple host who can access
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    }
}
```

Example, show all the configure
```shell
agent@local
{
    "broadcast":"enable",                 # listen for broadcast command from the local network
    "broadcast_port":"22222",             # broadcast port is UDP 22222
    "broadcast_group":"default",          # broadcast group is default
    "json":"disable",                     # JSON command accept is disable
    "json_port":"22220"                   # JSON command listen TCP port is 22220
}
```  
Example, enable the JSON command accept
```shell
agent@local:json=enable
ttrue
```  


