***
## Connect to network
connect to network for gateway to gatway

#### **Configuration( agent@net )**   
**agent@net** is first gtog network   
**agent@net2** is second gtog network   

```json
{
    "status":"connect to network proxy",                  // [ "disable", "enable" ]
    "port":"network server port",                         // [ number ]

    "netid":"network identify",                           // [ string ]
    "network":"network address",                          // [ string ]

    "keepintval":"keeplive to master/server interval",    // [ number ]
    "keepfailed":"keeplive to master/server failed",      // [ number ]
    "keeptimeout":"keeplive to master/server timeout"     // [ number ]
}
```   
Examples, show all the configure
```shell
agent@gtog
{
    "status":"enable",            # network enable
    "port":"20002"
}
```  



#### **Methods**

+ `setup[]` **setup the network client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror*

+ `shut[]` **shutdown the network client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror

+ `status[]` **get the network client status**
    - failed return NULL
    - error return terror
    - succeed return json to describes infomation
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status"         // [ "uping", "down", "online" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "online" for the cloud is connect succeed
    }
    ```

+ `branch[ {branch infomation} ]` **update a branch to this client**
    - {branch infomation} ------ json   
    ```json
    // Attributes introduction of json pass to API
    {
        "macid":"branch mac identify",             // [ string ]
        "type":"gateway type",                     // [ "auto", "master", "branch" ], default is "auto"
        "ip":"device endpoint ip address",         // [ ip address ]
        "pubkey":"public key",                     // [ string ], exist when device attach

        "net":"device endpoint network address",   // [ network address ]
        "mask":"device endpoint netmask address",  // [ netmask address ]

        "nattype":"device nat type",               // [ "8", "4", "2" ], exist when device attach
        "peer":"device ip address",                // [ ip address ], exist when device attach
        "port":"device port"                       // [ port ], exist when device attach
    }
    ```

+ `leaf[ {leaf infomation} ]` **update a leaf to this client**
    - {leaf infomation} ------ json   
    ```json
    // Attributes introduction of json pass to API
    {
        "macid":"branch mac identify",             // [ string ]
        "type":"gateway type",                     // [ "auto", "master", "branch" ], default is "auto"
        "ip":"device endpoint ip address",         // [ ip address ]
        "pubkey":"public key",                     // [ string ], exist when device attach

        "net":"device endpoint network address",   // [ network address ]
        "mask":"device endpoint netmask address",  // [ netmask address ]

        "nattype":"device nat type",               // [ "8", "4", "2" ], exist when device attach
        "peer":"device ip address",                // [ ip address ], exist when device attach
        "port":"device port"                       // [ port ], exist when device attach
    }
    ```

+ `endpoint[]` **list all the endpoint of network**
    ```json
    // Attributes introduction of json that the API return
    {
        "endpoint mac identify":          [ string ]:{}
        {
            "point":"endpoint ip address",       // [ ip address ]
            "extend":"endpoint local network",   // [ network ]
            "pubkey":"public key",               // [ string ]
            "nattype":"nat type",                // [ "1", "2" ], "1" for branch, "2" for leaf
            "perf":"branch priority",            // [ number ], the larger the number, the higher the priority
            "ip":"internet ip for endpoint",     // [ ip address ]
            "port":"internet port for endpoint", // [ port ]
            "macid":"mac identify"               // [ string ]
        }
        // ... more endpoint
    }
    ```


