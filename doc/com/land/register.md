## Management of register

Each object can have registers used to hold component data or communicate with other components. Values are **volatile** across reboot unless your product persists them elsewhere.

### **Configuration( `land@register` )**

The **saved configuration object** for `land@register` (query/set via `land@register`, `land@register:path`, merge `|{json}`, etc.).



`land@register` has **no** persisted JSON configuration; use the **Component API** to read and write register slots per object.

### **Component API**

+ `list[ [object] ]` **list all register**, show specified object register list
    - object ----------- [ string ], when omitted, list registers for the default object
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

+ `int[ [object], register name ]` **show register value in integer**  
    - object ----------- [ string ], omit to use default object (same as one-argument form)
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the tid of ifname@wan object
    ```shell
    land@register.int[ifname@wan, tid]
    5
    ```

+ `boole[ [object], register name ]` **show register value in boole**
    - object ----------- [ string ], omit to use default object
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the keeplive of ifname@wan object
    ```shell
    land@register.boole[ifname@wan, keeplive]
    true
    ```

+ `string[ [object], register name ]` **show register value in string**
    - object ----------- [ string ], omit to use default object
    - register name ----------- [ string ]
    - none or failed return NULL     
    - return string to describes  

    Example, show the mode register of ifname@wan object 
    ```shell
    land@register.string[ifname@wan, mode]
    dhcpc
    ```

+ `dump10[ [object], register name ]` **dump register bytes as hex, newline every 10 bytes**
    - object --------------- [ string ], omit to use default object
    - register name -------- [ string ]
    - failed return tfalse; on success prints to stdout and returns NULL

+ `dump100[ [object], register name ]` **dump register bytes as hex, newline every 100 bytes**
    - object --------------- [ string ], omit to use default object
    - register name -------- [ string ]
    - failed return tfalse; on success prints to stdout and returns NULL

+ `set_int[ [object], register name, value ]` **set a integer value to register**
    - object --------------- [ string ], omit to set the global register namespace
    - register name -------- [ string ]
    - value ---------------- [ number ]
    - failed return tfalse
    - succeed return ttrue

    Example, set 5 to tid of ifname@wan object
    ```shell
    land@register.set_int[ifname@wan, tid, 5]
    ttrue
    ```

+ `set_boole[ [object], register name, value ]` **set a boolean value to register**
    - object --------------- [ string ], omit for global register
    - register name -------- [ string ]
    - value ---------------- [ "true", "false" ]
    - failed return tfalse
    - succeed return ttrue

    Example, set true to keeplive of ifname@wan object
    ```shell
    land@register.set_boole[ifname@wan, keeplive, true]
    ttrue
    ```

+ `set_string[ [object], register name, [value] ]` **set string value to register**
    - object --------------- [ string ], omit for global register
    - register name -------- [ string ]
    - value ---------------- [ string ], omit or empty string clears to empty
    - failed return tfalse
    - succeed return ttrue

    Example, set myreg3 of ifname@wan object value
    ```shell
    land@register.set_string[ifname@wan, myreg3, myreg3valueisnull ]
    ttrue
    ```

### **C Code Example**

**Call component methods**

```c
#include "skin/skin.h"

static void print_register_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `list[ [object] ]`

```c
talk_t ret = scalls("land@register", "list", "ifname@wan");
if (ret > tpanic)
{
    printf("register list json ready\n");
    talk_free(ret);
}
else print_register_call_error("list", ret);
```

##### `int[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "int", "ifname@wan,tid");
if (ret > tpanic)
{
    printf("tid=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("int", ret);
```

##### `boole[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "boole", "ifname@wan,keeplive");
if (ret > tpanic)
{
    printf("keeplive=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("boole", ret);
```

##### `string[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "string", "ifname@wan,mode");
if (ret > tpanic)
{
    printf("mode=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("string", ret);
```

##### `dump10[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "dump10", "ifname@wan,mac");
if (ret == tfalse || ret == terror || ret == tpanic) print_register_call_error("dump10", ret);
```

##### `dump100[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "dump100", "ifname@wan,mac");
if (ret == tfalse || ret == terror || ret == tpanic) print_register_call_error("dump100", ret);
```

##### `set_int[ [object], register name, value ]`

```c
talk_t ret = scalls("land@register", "set_int", "ifname@wan,tid,7");
if (ret != ttrue) print_register_call_error("set_int", ret);
```

##### `set_boole[ [object], register name, value ]`

```c
talk_t ret = scalls("land@register", "set_boole", "ifname@wan,keeplive,true");
if (ret != ttrue) print_register_call_error("set_boole", ret);
```

##### `set_string[ [object], register name, [value] ]`

```c
talk_t ret = scalls("land@register", "set_string", "ifname@wan,myreg3,myvalue");
if (ret != ttrue) print_register_call_error("set_string", ret);
```

### **Lifecycle API**

+ `setup[]` / `shut[]` — **when implemented** for **`land@register`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.
+


### **Joint handlers**

**None** by default for this object (product builds may add more).


### **Published joint events**

**None** beyond what is documented above in the reference package.

