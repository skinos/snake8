***
## Connect to port proxy
connect to port proxy to proxy the tcp/udp/dev connection

#### Configuration( agent@portc )
```json
{
    "status":"connect to port proxy",               // [ "disable", "enable" ]
    "port":"port proxy port",                       // [ number ]
    "pond":"pond client for proxy ",                // [ nubmer ]

    "connect_timeout":"connect timeout",                            // [ number ], the unit is second
    "idle_keeplive_interval":"idle connection keeplive interval",   // [ number ], the unit is second
    "idle_keeplive_timeout":"idle connection timeout time",         // [ number ], the unit is second
    "use_keeplive_timeout":"use connection timeout time"            // [ number ], the unit is second
}
```
Examples, show all the configure
```shell
agent@portc
{
    "status":"enable",            # port proxy enable
    "port":"20005",
    "pond":"3"
    "keeplive":"8",               # keeplive to server in 8sec 
    "idle_imeout":"46",           # 46sec timeout to close the idle connection
    "uset_imeout":"360"           # 360sec timeout to close the use connection
}
```  



#### **Methods**

+ `setup[]` **setup the portc client**
    - succeed return ttrue   
    - failed return tfalse   
    - error return terror   

+ `shut[]` **shutdown the portc client**
    - succeed return ttrue   
    - failed return tfalse   
    - error return terror   

+ `status[]` **get the portc status**
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

+ `list[]` **get the portc all connection**
    - failed return NULL
    - error return terror*
    - succeed return json to describes infomation
    ```json
    // Attributes introduction of json by the method return
    {
        "file descriptor number":                               // [ number ]
        {
            "local_ip":"use this ip connect to server",             // [ ip address ]
            "local_port":"use this port connect to server",         // [ port ]
            "tx":"send byte to server",                             // [ number ]
            "rx":"recv byte form server",                           // [ number ]
            "last":"last recv uptime mark",                         // [ number ]
            "port":"server port at the server",                     // [ number ]
            "hand_ip":"proxy to the ip",                            // [ ip address ]
            "hand_port":"proxy to the port",                        // [ number ]
            "hand_proto":"proxy use the protocol",                  // [ 't', 'u', 'd' ]
            "hand_tx":"send byte to server",                        // [ number ]
            "hand_rx":"recv byte form server"                       // [ number ]
        }
        // ... more file descriptor number
    }
    ```

