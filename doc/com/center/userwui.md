## user access service   
all user access this server to management device

### **Configuration( `center@userwui` )**


```json
// attribute introduction
{
    "status":"enable the function",                       // [ disable, enable ]

    "port":"service port",                  // [ number ], 1-65535, default is 80
    "sslport":"https port",                 // [ number ], 1-65535, default is 443
    "termport":"Terminal port",             // [ number ], 1-65535, default is 81
    "session_timeout":"session timeout",    // [ number ]
    "talk_timeout":"talk timeout",          // [ number ]
    "key_lifetime":"key life time",         // [ number ]
    "auth_object":"auth object",            // [ string ]
    "auth_api":"auth api",                  // [ string ]

    "publist":                   // vaild public command list
    {
        "command match":"compare type"     // [ string ]: [ "start", "sub", "equal" ]
    },
    "helist":                   // vaild he command list
    {
        "command match":"compare type"     // [ string ]: [ "start", "sub", "equal" ]
        // ... more the command match
    }

}
```
Examples, show all the configure
```shell
he center@userwui
{
    "status":"enable",

    "port":"20000",

    "talk_timeout":"61",
    "key_lifetime":"300"
}
```  
Examples, modify the he port to 1000
```shell
he center@userwui:port=10000
ttrue
```  



#### **Javascript API**

1. include the js for he API

```html
<script src="/api/he.js"></script>
```

2. /api/he.js API

```html
<script>

he.cmd( he command array, loading object for show, func call when return );

he.load( he command array, loading object for show );
he.exec( he command array, loading object for show );

// exec the he command in background
he.bkload( he command array );
he.bkexec( he command array );

</script>
```

3. The example gets a list of gateways for username "alice" in background, the he command is "center@api.list[ alice ]"

```html
<script>

he.bkload( [ "center@api.list[ alice ]" ] ).then( function(v){
    
    // the gwlist is a json that return by center@api.list[ alice ]
    var gwlist = v[0];

});

</script>

4. The example gets configure of alice from center@heport database

```html
<script>

he.bkload( [ "center@heport/alice/config ]" ] ).then( function(v){
    
    // the config is a json that return by center@heport/alice/config
    var config = v[0];

});

</script>



```

### **Lifecycle API**

+ `setup[]` / `shut[]` — in the default **center** package, wired for **`init` / `uninit` → `app`** together with **`center@pport`**, **`center@heport`**, **`center@userwui`**.
+ Adjust object name for the component this file describes.



### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_center_userwui(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "center@userwui", "status") == NULL)
        return -1;
    return ssets_string("center@userwui", "enable", "status") ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* e.g. scall("center@userwui", "list", NULL); talk_free if JSON */
```

