***
## System Route Table Management
Management of system route table, modifying this configuration directly is not recommended, It is recommended to manage through the method

#### Configuration( forward@main )   
```json
// Attributes introduction 
{
    "rule name":        // [ string ], user can custom the rule name
    {
        "target":"select the packet use source ip address",            // [ ip address, network ]
        "mask":"select the packet use source mask of ip address",      // [ netmask ], necessary when "target" be network
        "gw":"gateway ip address",                                     // [ ip address ]
        "metric":"route hop",                                          // [ number ]
        "ifname":"select the packet output interface",                 // [ "ifname@lan", "ifname@lan2", "ifname@wan", "ifname@lte", ... ], interface name
    }
    // ... more rule
}
```

Example, show current all rule settings of route
```shell
forward@main
{
    "myCustomRule1":    # first rule name is "myCustomRule1"
    {                                # make dest 192.168.1.0/255.255.255.0 to ifname@wan's 192.168.8.22, mark the metric be 2
        "target":"192.168.1.0",
        "mask":"255.255.255.0",
        "gw":"192.168.8.22",
        "metric":"2",
        "ifname":"ifname@wan"
    }
    "youCustomRule":    # second rule name is "youCustomRule"
    {                               # make all access to ifname@lan's 192.168.9.22
        "gw":"192.168.9.22",
        "ifname":"ifname@lan"
    }
}
```  



#### **Methods**

+ `status[]` **get the current system route table**
    - failed reeturn NULL, error return terror*   
    - error return terror*   
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of json by the method return
    {
        "rule name":        // [ string ], user custom the rule name, the system rule start with "~"
        {
            "target":"select the packet use source ip address",            // [ ip address, network ]
            "mask":"select the packet use source mask of ip address",      // [ netmask ], necessary when "target" be network
            "gw":"gateway ip address",                                     // [ ip address ]
            "ifname":"select the packet output interface",                 // [ "ifname@lan", "ifname@lan2", ... ], interface name
            "netdev":"network device",                                     // [ string ]
            "metric":"route hop",                                          // [ number ]
            "status":"rule state"                                          // [ "up", "down" ], "up" for enable, "down" for disable
        }
        // ... more rule
    }
    ```

    Example, get the current route rule
    ```shell
    forward@main.status
    {
        "myCustomRoute1":             // this is user add rule named "myCustomRoute1"
        {
            "target":"192.168.0.0",
            "mask":"255.255.255.0",
            "gw":"192.168.8.2",
            "metric":"2",
            "ifname":"ifname@wan",
            "device":"eth0.2",
            "flags":"1",
            "metric":"2",
            "ref":"0",
            "use":"0",
            "status":"up"
        },
        "~auto1":                    // this is system rule
        {
            "target":"127.0.0.1",
            "mask":"255.255.255.0",
            "gw":"0.0.0.0",
            "device":"lo",
            "flags":"1",
            "metric":"0",
            "ref":"0",
            "use":"0",
            "status":"up"
        }
    }
    ```   

+ `add[ name, [target], [mask], [gateway], [ifname], [metric] ]` **add route rule**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   

    Example, add a rule named office1, make that address 192.168.2.12 route to  192.168.9.40 of LAN
    ```shell
    forward@main.add[ office1, 192.168.2.12, 255.255.255.0, 192.168.9.40, ifname@lan, ]
    ttrue
    ```   

    Example, add a rule named office2, make that all ddress route to 192.168.9.41 of LAN
    ```shell
    forward@route.add[ office2, , , 192.168.9.41, ifname@lan, ]
    ttrue
    ```   

+ `delete[ name ]` **delete route rule**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   
    
    Example, delete the custom route named office2
    ```shell
    forward@route.delete[ office2 ]
    ttrue
    ```

    Example, delete the custom route named office1
    ```shell
    forward@route.delete[ office1 ]
    ttrue
    ```

