***
## IO Agent
manage IO

#### **Configuration( agent@io )** 

```json
// Attributes introduction 
{
    // IO Initial Configuration, set the IO state at the system boot
    "init":
    {
        "io number":"IO state"        // [ "g1", "g2", "g3", ... ]: [ "0f", "0r", "0b", "10", "11", "2-x-y" ]
                                        // io number can be "g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8", ...
                                        // IO state can be: "0f" for input mode and the tigger by falling
                                        //                  "0r" for input mode and the tigger by rising
                                        //                  "0b" for input mode and the tigger by rising and falling
                                        //                  "10" for output low
                                        //                  "11" for output high
                                        //                  "2-x-y" for output cyclic timer, x for high, y for low, 
                                        //                          ex. "2-100-200" will outputs a cyclic timer of 100ms high and 200ms low
        // ... more io state init
    },

    // IO trigger Configuration
    "trigger":
    {
        "io number":                    // [ "g1", "g2", "g3", ... ]:{}
        {
            "io state":                        // [ "falling", "rising", "both" ]:{}
            {
                "cmd name":                          // [ string ]:{}
                {
                    "obj":"object name",                 // [ string ]
                    "op":"object API",                   // [ string ]
                    "1":"first parameter",               // [ string ]
                    "2":"second parameter"               // [ string ]
                    // ... more parameter
                }
                // ... more operation
            }
            // ... more io state
        }
    },

    // first client, default is TCP/UDP client configure
    "client":
    {
        "status":"client status",               // [ "disable", "enable" ]
        
        "server":"server address",              // [ string ]
        "proto":"client protocol",              // [ "tcp", "udp" ]
        "port":"server port",                   // [ number ]
        "interval":"connect failed interval",   // [ number ], the unit is second

        "id":"custom the identify",             // [ string ]
        "user":"custom the username",           // [ string ]
        "vcode":"custom the vocde"              // [ string ]
    },

    // second client, default is MQTT Client configure
    "mqtt":
    {
        "status":"client status",               // [ "disable", "enable" ]
        "server":"server address",              // [ string ]
        "port":"server port",                   // [ number ]

        "mqtt_id":"device identify",            // [ string ]
        "mqtt_username":"mqtt username",        // [ string ]
        "mqtt_password":"mqtt password",        // [ string ]
        "mqtt_interval":"mqtt connect interval",// [ nubmer ], the unit is second
        "mqtt_keepalive":"mqtt keepalive",      // [ number ], the unit is second
        "mqtt_publish":"mqtt publish topic",    // [ string ]
        "mqtt_publish_qos":"mqtt publish qos",  // [ number ]
        "mqtt_subscribe":
        {
            "subscribe topic":"topic qos",
            "subscribe topic2":"topic2 qos",
            "subscribe topic3":"topic3 qos",
            // "subscribe topic":"topic qos"     How many subscribe topic need setting save how many properties
        }
    },

    // server,  TCP/UDP server configure
    "server":
    {
        "status":"client status",               // [ "disable", "enable" ]
        "proto":"client protocol",              // [ "tcp", "udp" ]
        "port":"server port",                   // [ number ]
        "interval":"connect failed interval",   // [ number ], the unit is second
        "timeout":"connect idle timeout"        // [ number ], the unit is second
    }

}
```

Examples, show all the io agnet configure
```shell
agent@io
{
    "init":
    {
        "g1":"10",               # init the g1 output low
        "g2":"11",               # init the g2 output high
        "g3":"0b",               # init the g3 input mode, and trigger by falling and rising
        "g4":"2-200-300"         # init the g4 output a cyclic timer, 200ms high, 300ms low
    },
    "trigger":
    {
        "g3":
        {
            "falling":
            {
                "sms send":                 # send sms "G3 Low" to 8717688704240 when g3 falling(high to low)
                {
                    "obj":"modem@lte",
                    "op":"sms_send",
                    "1":"8717688704240",
                    "2":"G3 Low"
                },
                "io2 op":                   # out low to g2 when g3 falling(high to low)
                {
                    "obj":"agent@io",
                    "op":"modify",
                    "1":"g2",
                    "2":"10"
                }
            },
            "rising":                       # send sms "G3 High" to 8717688704240 when g3 rising(low to high)
            {
                "sms send":
                {
                    "obj":"modem@lte",
                    "op":"sms_send",
                    "1":"8717688704240",
                    "2":"G3 High"
                }            
            },
            "both":
            {
                "io3 op":                   # out high to g1 when g3 falling or rising
                {
                    "obj":"agent@io",
                    "op":"modify",
                    "1":"g1",
                    "2":"11"
                }            
            }
        }
    },    
    "client":
    {
        "status":"enable",           # enable the tcp client
        "proto":"tcp",               # protocol is tcp
        "server":"io.wmdevice.com",  # sever is io.wmdevice.com
        "port":"9000",               # port is 9000
        "id":"B3223",                # id is B3223, when no settings default is device MAC
        "user":"dimmalex",           # mqtt username is dimmalex
        "vcode":"0815"               # mqtt pasword is 0815
    },
    "mqtt":
    {
        "status":"enable",            # enable the mqtt client
        "server":"mqtt.wmdevice.com", # server is mqtt.wmdevice.com
        "port":"1883",                # port is 1883
        "mqtt_id":"",                 # default is device MAC
        "mqtt_username":"dimmalex",   # mqtt username is dimmalex
        "mqtt_password":"0815",       # mqtt pasword is 0815
        "mqtt_keepalive":"10",        # mqtt keeplive is 10 sec
        "mqtt_interval":"5",          # mqtt connect interval is 5 sec
        "mqtt_publish":"d218/io",     # mqtt publish topic is d218/io
        "mqtt_publish_qos":"1",       # mqtt publish qos is 1
        "mqtt_subscribe":
        {
            "d218/order":"2",         # mqtt subscribe d218/order topic, qos is 2
            "route/order":"2"         # mqtt also subscribe route/order topic, qos is 2
        }
    }
}
```  

Examples, disable mqtt client
```shell
agent@io:mqtt/status=disable
ttrue
```  

Examples, modify the tcp/udp client server
```shell
agent@io:client/server=new.wmdevice.com
ttrue
```  



#### **API(agent@io)**

+ `status[]` **get the io agent status**   
    - failed return NULL   
    - succeed return json to describes   
    ```json
    // Attributes introduction of talk by the method return
    {
        "io name":"IO state", // [ "g1", "g2", "g3", ... ]: [ string ]
                                            // IO state, can be 00 for input low
                                                     //  can be 01 for input high
                                                     //  can be 10 for output low
                                                     //  can be 11 for output high
                                                     //  can be 12 for output a cyclic timer
        // ... more the io name
    }
    ```

    ```shell
    # examples, get the io agent status
    agent@io.status
    {
        "g1":"10",      # g1 output low
        "g2":"11",      # g2 output high
        "g3":"01",      # g3 input high
        "g4":"00"       # g4 input low
        "g5":"12"       # g5 output a cyclic timer
    }
    ```

+ `current[]` **get the io agent line status**   
    - failed return NULL   
    - succeed return string to describes   
    ```json
    // Attributes introduction of string by the method return
    current all IO state // [ string ]
                         // format is "g1=xx;g2=xx;g3=xx;..."
                         // "g1", "g2", "g3" is the io name 
                         // "xx" is the state, can be "00" for input low
                                           //  can be "01" for input high
                                           //  can be "10" for output low
                                           //  can be "11" for output high
                                           //  can be "12" for output a cyclic timer
    ```    

    ```shell
    # examples, get the io agent line status
    agent@io.current
    g1=10;g2=11;g3=01;g4=00;g5=12     # g1 output low, g2 output high, g3 input high, g4 input low, g5 output a cyclic timer
    ```

+ `list[]` **get the client/mqtt/serverstatus**   
    - failed return NULL   
    - succeed return json to describes   
    ```json
    // Attributes introduction of talk by the method return
    {
        "client":                    // Client status, have this when client enable
        {
            "connect":"connect state",    // [ "ok" ], have this when connect succeed 
            "tx":"send bytes",            // [ number ]
            "rx":"recv bytes"             // [ number ]
        },
        "mqtt":                      // MQTT status, have this when MQTT enable
        {
            "connect":"connect state",    // [ "ok" ], have this when connect succeed 
            "tx":"send bytes",            // [ number ]
            "rx":"recv bytes"             // [ number ]
        },
        "server:ip:port":            // Server status, have this when the client of other device connected in
        {
            "connect":"connect state",    // [ "ok" ], have this when connect succeed 
            "tx":"send bytes",            // [ number ]
            "rx":"recv bytes"             // [ number ]
        }
        // ... more the client of server
    }
    ```


+ `modify[ io, state ]` **modify the io state**   
    - io ----------- [ "g1", "g2", "g3", ... ] the IO name
    - state -------- [ "0f", "0r", "0b", "10", "11", "2-x-y" ]
        - "0f" for input mode and the tigger by falling
        - "0r" for input mode and the tigger by frising
        - "0b" for input mode and the tigger by frising and falling
        - "10" for output low
        - "11" for output high
        - "2-x-y" for output timer, x for high, y for low, ex. "2-100-200" will outputs a cyclic timer of 100ms high and 200ms low
    - failed return tfalse   
    - succeed return ttrue

+ `report[]` **report io state to client/mqtt/client right now**   
    - failed return tfalse   
    - succeed return ttrue

