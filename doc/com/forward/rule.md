***
## Policy based routing
Management of policy based routing

#### Configuration( forward@rule )
```json
// Attributes introduction 
{
    "rule name":                                               // [ string ], user can custom the rule name
    {
        "pref":"rule's priority",                                      // [ nubmer ], range of 0-4294967295, The smaller, the higher
                                                                                // 0 for local table priority
                                                                                // 32766 for main table priority
                                                                                // 50000 for default table priority
                                                                                // default is 40000

        "markid":"select the packet use markid",                       // [ nubmer ], range of 1-4294967295
                                                                                // Less than 100 is reserved for the system
                                                                                // You are advised to use more than 100 for user-defined rules
        "srcifname":"select the packet use source interface",          // [ "ifname@lan", "ifname@lan2", ... ], interface name
        "src":"select the packet use source ip address",               // [ ip address, network ]
        "srcmask":"select the packet use source mask of ip address",   // [ netmask ], necessary when "src" be network

        "tid":"which route table to go to"                             // [ number ], range of 0-255
                                                                                // 0 for local table
                                                                                // 253 for default table
                                                                                // 254 for main table
                                                                                // 255 for local table
                                                                                // Less than 100 is reserved for the system
                                                                                // You are advised to use more than 100 for user-defined rules(And avoid 253,254,255)
    }
    // ... more rule
}

```   
Example, show current all of policy rule
```shell
forward@rule
{
    "myCustom1":                       # rule name is myCustom1
    {                                  # packet from ifname@lan and source address is 1.1.1.1 route to route table 101, priority is 38000
        "pref":"38000",
        "srcifname":"ifname@lan",
        "src":"1.1.1.1",
        "srcmask":"255.255.255.255",
        "tid":"101"
    },
    "youCustom":                       # rule name is youCustom
    {                                  # packet from ifname@lan and markid is 300 route to route table 102, default priority is 40000
        "srcifname":"ifname@lan",
        "markid":"300",
        "tid":"102"
    }
}
```  

#### **Methods**

+ `status[]` **get the current policy rule**
    - failed reeturn NULL, error return terror  
    - succeed return json to describes infomation  
    ```json
    // Attributes introduction of json by the method return
    {
        "rule name",                                                 // [ string ], user custom the rule name
        {
            "pref":"rule's priority",                                      // [ nubmer ], range of 0-4294967295, The smaller, the higher
                                                                                    // 0 for local table priority
                                                                                    // 32766 for main table priority
                                                                                    // 50000 for default table priority
                                                                                    // default is 40000        
            "markid":"select the packet use markid",                       // [ nubmer ], range of 1-4294967295
                                                                                    // Less than 100 is reserved for the system
                                                                                    // You are advised to use more than 100 for user-defined rules
            "srcifname":"select the packet use source interface",          // [ "ifname@lan", "ifname@lan2", ... ], ifname
            "src":"select the packet use source ip address",               // [ ip address, network ]
            "srcmask":"select the packet use source mask of ip address",   // [ netmask ], necessary when "src" be network
            "tid":"which route table to go to"                             // [ number ], range of 0-255
                                                                                    // 0 for local table
                                                                                    // 253 for default table
                                                                                    // 254 for main table
                                                                                    // 255 for local table
                                                                                    // Less than 100 is reserved for the system
                                                                                    // You are advised to use more than 100 for user-defined rules(And avoid 253,254,255)

        }
        // ... more rule
    }
    ```   

    Example, get the current policy rule
    ```shell
    forward@rule.status
    {
        "myCustom1":                       # rule name is myCustom1
        {                                  # packet from ifname@lan and source address is 1.1.1.1 route to route table 101, priority is 38000
            "srcifname":"ifname@lan",
            "src":"1.1.1.1",
            "srcmask":"255.255.255.255",
            "tid":"101",
            "pref":"38000"
        },
        "youCustom":                       # rule name is youCustom
        {                                  # packet from ifname@lan and markid is 300 route to route table 102, default priority is 40000
            "srcifname":"ifname@lan",
            "markid":"300",
            "tid":"102"
        }
    }
    ```   

+ `add[ name, [src], [srcmask], [srcifname], [markid], tid, [pref] ]` **add policy rule**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   

    Example, add a rule named senser, make that source address 192.168.2.12 route to route table 1, priority is 33000
    ```shell
    forward@rule.add[ senser, 192.168.2.12, , , , 1, 33000 ]
    ttrue
    ```

    Example, add a rule named video, make all other acess route to route table 2, priority is 33300
    ```shell
    forward@rule.add[ video, , , , , 2, 33000 ]
    ttrue
    ```

+ `delete[ name ]` **delete policy rule**
    - succeed return ttrue
    - failed return tfalse
    - error return terror   

    Example, delete the policy rule named video
    ```shell
    forward@rule.delete[ video ]
    ttrue
    ```   

    Example, delete the policy rule named senser
    ```shell
    forward@rule.delete[ senser ]
    ttrue
    ```   



+ `tidlist[]` **list all the route table**
    - failed reeturn NULL, error return terror   
    - error return terror   
    - succeed return json to describes infomation  
    ```json
    // Attributes introduction of json by the method return
    {
        "table id":"table name"                             // [ number ]: [ string ]
        // ... more tid
    }
    ```   

    Example, get the current route table list
    ```shell
    forward@rule.tidlist
    {
        "1":"1",
        "2":"2",
        "253":"local",
        "254":"main",
        "255":"default",
    }
    ```   
