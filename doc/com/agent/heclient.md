***
## Accept heport management   
connect to heport server and accept administrative control from that   

#### Configuration( agent@heclient )   
```json
{
    "status":"connect to heport server for he command",    // [ "disable", "enable" ]

    "extern":"bind extern ifname to work",                 // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ]

    "server":"http server address",                        // [ string ]
    "port":"heport server port",                           // [ number ]

    "user":"username for device",                          // [ string ]
    "vcode":"vcode for device",                            // [ string ]

    "connect_timeout":"timeout for connection"          // [ number ]
    "keeplive_interval":"keeplive interval"             // [ number ]
    "keeplive_timeout":"timeout for keeplive"           // [ number ]
}
```   
Example, show all the configure   
```shell
agent@heclient
{
    "status":"enable",                                      # connect to heport server is enable

    "server":"devport.ashyelf.com",                         # heport server is devport.ashyelf.com
    "port":"20002",

    "user":"dimmalex@gmail.com",
    "vcode":"123456"
}
```  
Example, disable the he client   
```shell
agent@heclient:status=disable
ttrue
```  
Example, modify the heport server to heport.ashyelf.com   
```shell
agent@heclient:server=heport.ashyelf.com
ttrue
```  



#### **Methods**   

+ `setup[]` **setup the he client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   

+ `shut[]` **shutdown the he client**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   

+ `status[]` **get the he client infomation**
    - failed return NULL   
    - error return terror   
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status",        // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                             // "uping" for connecting
                                             // "down" for the network is down
                                             // "online" for connected
                                             // "usererror" for username wrong
                                             // "vcodeerror" for vcode wrong
        "server":"hepoprt server ip"      // [ ip address ]
    }
    ```   
    Example, get the he client infomation   
    ```shell
    agent@heclient.status
    {
        "status":"online",                    // connect succeed
        "server":"114.132.219.158"            // heport server ip address
    }
    ```   

+ `adjust[ {adjust infomation} ]` **adjust the other component of gateway**
    - {adjust infomation} ------ json   
    ```json
    // Attributes introduction of json pass to API
    {
        "component name":                           // [ string ], the component name
        {
            // configure content
        }
        // ...more other configure
    }
    ```  
    - succeed return ttrue
    - failed return tfalse
    - error return terror   
 
    Example, adjust to run the port client, disable the network client   
    ```shell
    agent@heclient.adjust[ { "agent@portc":{ "status":"enable"}, "agent@gtog":{ "status":"disable" } } ]
    ttrue
    ```   

