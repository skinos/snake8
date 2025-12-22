***

## Management of register
Each object can have a number of registers, can be used to hold information about the component itself or to communicate with other components, all register value will be lost when system restart


#### **API( land@register )**

+ `list[ [object] ]` **list all register**, show specified object register list
    - object ----------- [ string ], default list register for land@machine
    - none or failed return NULL 
    - return json to describes the list  

    ```json
    // Attributes introduction of json by the method return
    {
        "register name":"register value size in byte"
        // ... more register list
    }    
    ```
    Example, get object ifname@wan register list
    ```shell
    land@register.list[ifname@wan]
    {
        "ifdev":"20",                    // named ifdev register size is 20 byte
        "tid":"4",                       // named tid register size is 4 byte
        "mode":"20",                     // named mode register size is 20 byte
        "method":"20",
        "connect_failed":"4",
        "netdev":"20",
        "keeplive":"20",
        "metric":"20",
        "custom_dns":"20",
        "dns":"20",
        "dns2":"20",
        "delay_buf":"240",
        "delay_pos":"4",
        "delay":"4"
    }
    ```

+ `int[ object, register name ]` **show register value in integer**  
    - object ----------- [ string ]
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the tid of ifname@wan object
    ```shell
    land@register.int[ifname@wan, tid]
    5
    ```

+ `boole[ object, register name ]` **show register value in boole**
    - object ----------- [ string ]
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the keeplive of ifname@wan object
    ```shell
    register.boole[ifname@wan, keeplive]
    true
    ```

+ `string[ object, register name ]` **show register value in string**
    - object ----------- [ string ]
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the mode register of ifname@wan object 
    ```shell
    register.string[ifname@wan, mode]
    dhcpc
    ```


+ `set_int[ object, register name, value ]` **set a integer value to register**
    - object --------------- [ string ]
    - register name -------- [ string ]
    - value ---------------- [ number ]
    - failed return tfalse
    - succeed return ttrue

    Example, set 5 to tid of ifname@wan object
    ```shell
    register.set_int[ifname@wan, tid, 5]
    ttrue
    ```

+ `set_boole[ object, register name, value ]` **set a boolean value to register**
    - object --------------- [ string ]
    - register name -------- [ string ]
    - value ---------------- [ "true", "false" ]
    - failed return tfalse
    - succeed return ttrue

    Example, set true to keeplive of ifname@wan object
    ```shell
    register.set_boole[ifname@wan, keeplive, true]
    ttrue
    ```

+ `set_string[ object, register name, value ]` **set string value to register**
    - object --------------- [ string ]
    - register name -------- [ string ]
    - value ---------------- [ string ]
    - failed return tfalse
    - succeed return ttrue

    Example, set myreg3 of ifname@wan object value
    ```shell
    register.set_string[ifname@wan, myreg3, myreg3valueisnull ]
    ttrue
    ```


