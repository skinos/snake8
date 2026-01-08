***
## Connect to network
connect to network for gateway to gatway

#### **Configuration( agent@net )**   
**agent@net** is first gtog network   
**agent@net2** is second gtog network   

```json
{
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



#### **API**
**agent@net** is first gtog network   
**agent@net2** is second gtog network   

+ `setup[]` **setup the this client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror

+ `shut[]` **shutdown the this client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror

+ `status[]` **get the this client status**
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

+ `list[]` **list all the endpoint of this client**
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


+ `endpoint[ {endpoint list} ]` **update endpoint list to this client**
    - {endpoint list} ------ json   
    ```json
    // {endpoint list} attributes introduction
    {
        "endpoint mac identify":          [ string ]:{}
        {    
            "ip":"device connect ip address",         // [ ip address ]
            "port":"device connect port",             // [ port ]
            "pubkey":"device public key",             // [ string ]
            "nattype":"device nat type",              // [ "1", "2" ], "1" for free that can be branch, "2" for limit only be leaf
            "pref":"device master pref",              // [ number ]

            "point":"endpoint ip address",            // [ ip address ]
            "extend":"endpoint local network"         // [ network address ]
        }
        // ... more endpoint
    }
    ```

+ `branch[ {branch infomation} ]` **add a branch to this client**
    - {branch infomation} ------ json   
    ```json
    // {branch infomation} attributes introduction
    {
        "macid":"device mac identify",            // [ string ]
        "ip":"device connect ip address",         // [ ip address ]
        "port":"device connect port",             // [ port ]
        "pubkey":"device public key",             // [ string ]
        "nattype":"device nat type",              // [ "1", "2" ], "1" for free that can be branch, "2" for limit only be leaf
        "pref":"device master pref",              // [ number ]

        "point":"endpoint ip address",            // [ ip address ]
        "extend":"endpoint local network"         // [ network address ]
    }
    ```

+ `leaf[ {leaf infomation} ]` **add a leaf to this client**
    - {leaf infomation} ------ json   
    ```json
    // {leaf infomation} attributes introduction
    {
        "macid":"device mac identify",            // [ string ]
        "ip":"device connect ip address",         // [ ip address ]
        "port":"device connect port",             // [ port ]
        "pubkey":"device public key",             // [ string ]

        "point":"endpoint ip address",            // [ ip address ]
        "extend":"endpoint local network"         // [ network address ]
    }
    ```

