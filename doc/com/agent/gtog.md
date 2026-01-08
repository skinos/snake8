***
## Gateway to gateway network frame


#### **Configuration( agent@gtog )**   

```json
{
    "net_max":"max gtog network",                       // [ number ]
    "port_start":"gtog network start port"              // [ number ]
}
``` 

Example, show all the configure
```shell
agent@gtog
{
    "net_max":"10",            #  max 10 network
    "port_start":"10004"       # network port start at 10004
}
```  



#### **API( agent@gtog )**

+ `setup[]` **setup all gtog network client**
    - succeed return ttrue
    - failed return tfalse

+ `shut[]` **shutdown all gtog network client**
    - succeed return ttrue
    - failed return tfalse


+ `register[ netid, {network configure} ]` **register a gtog network client**
    - netid -------------------- [ string ]   
    - {network configure} ------ [ json ]
    ```json
    // Attributes introduction of json of {network configure}
    {
        "port":"network server port",                         // [ number ]

        "netid":"network identify",                           // [ string ]
        "network":"network address",                          // [ string ]

        "keepintval":"keeplive to master/server interval",    // [ number ]
        "keepfailed":"keeplive to master/server failed",      // [ number ]
        "keeptimeout":"keeplive to master/server timeout"     // [ number ]
    }
    ```

+ `unregister[ netid ]` **unregister a gtog network client**
    - netid -------------------- [ string ]   
    - succeed return ttrue
    - failed return tfalse


+ `list[]` **list all gtog network client**
    - failed return NULL
    - succeed return json to describes infomation
    ```json
    // Attributes introduction of talk by the API return
    {
        "gtog network":
        {
            "netid":"network identify",
            "netdev":"network netdev", 
            "lport":"network local port",
            "master":"network current master",
            "pref":"network current master pref"
        }
        // ... more gtog network
    }
    ```


+ `endpoint[ netid, {endpoint list} ]` **update endpoint list to gtog network client**
    - netid ---------------- [ string ]   
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

+ `branch[ netid, {branch infomation} ]` **add branch to gtog network client**
    - netid -------------------- [ string ]   
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

+ `leaf[ netid, {leaf infomation} ]` **add leaf to gtog network client**
    - netid ------------------ [ string ]   
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

