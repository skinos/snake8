## wifi@a — 5.8G Radio Management
Manage 5.8G Radio

### Configuration ( `wifi@a` )
```json
// Attribute introduction
{
    "mode":"radio mode",                 // [ "a", "anac", "ac" ]
                                                // "a" for 11A
                                                // "anac" for 11A/AC
                                                // "ac" for 11AC
    
    "bandwidth":"radio bandwidth",       // [ "40", "80", "160" ]
                                                // "40" for 40M
                                                // "80" for 80M
                                                // "160" for 160M

    "channel":"radio channel",           // [ 0, 36-165 ], "0" for auto select the channel
    "beacon":"beacon interval",          // [ number ]
    "country":"country code",            // [ "cn", "en", "jp", ... ]

    "dtim":"radio dtim",                 // [ number ]
    "ldpc":"use LDPC or not",            // [ "disable", "enable" ]
    "shortgi":"short GI",                // [ "disable", "enable" ]
    "stbc":"radio STBC"                  // [ "disable", "enable" ]
}
```

Example, show 5.8G Radio all configure
```shell
wifi@a
{
    "mode":"ac",               # 5.8G Radio is 11AC
    "bandwidth":"80",          # 5.8G bandwidth 80M
    "channel":"165",           # 5.8G channel is 165

    "beacon":"100",            # 5.8G beacon interval is 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```  

Example, modify the 5.8G Radio channel to auto
```shell
wifi@a:channel=0
ttrue
```

Example, modify the 5.8G Radio channel to 36
```shell
wifi@a:channel=36
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
wifi@a|{"mode":"ac","bandwidth":"80","channel":"0"}
ttrue
```

### Component API
**Directly callable** APIs: `wifi@a.method`, `wifi@a2.method`, … (HE / eline / HTTP `/he`).

+ `chlist[]` **get the 5.8G radio channel list**   
    - failed return NULL
    - error return terror    
    - succeed return json to describes this list   
    ```json
    // Attributes introduction of talk by the method return
    {
        "channel number":{}                // [ number ]:{}
        // more channel number
    }
    ```

    Example, get the 5.8G radio channel list
    ```shell
    wifi@a.chlist
    {
        "36":{},
        "40":{},
        "44":{},
        "48":{},
        "52":{},
        "56":{},
        "60":{},
        "64":{},
        "149":{},
        "153":{},
        "157":{},
        "161":{},
        "165":{}
    }
    ```

+ `stalist[]` **get list of clients on 5.8G radio**   
    - failed return NULL
    - error return terror    
    - succeed return json to describes this list   
    ```json
    // Attributes introduction of talk by the method return
    {
        "client MAC address":              // [ MAC address ]:{}
        {
            "livetime":"online time",               // [ hour:minute:second:day ]
            "rssi":"signal strength",               // [ number ], the unit maybe dBm or %
        }
        // ... more client
    }
    ```

    Example, get the 5.8G radio's client list
    ```shell
    wifi@a.stalist
    {
        "78:11:DC:92:D3:9E":                  // client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "88:C3:97:75:1B:C0":                 // client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "40:31:3C:4D:78:35":                 // client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@assid"
        }
    }
    ```

+ `stabeat[ MAC address ]` **disconnect the client**   
    - failed return tfalse
    - error return terror    
    - succeed return ttrue

    Example, disconnect the client 00:03:7F:13:BD:30 from 5.8G Radio
    ```shell
    wifi@a.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

### Lifecycle API
+ `setup[]` **start component services**, *succeed return ttrue, failed return tfalse*
    - Called from the platform **`init`** schedule when **`setup[]`** is wired for this component.

+ `shut[]` **stop component services**, *succeed return ttrue, failed return tfalse*
    - Called from the platform **`uninit`** schedule when **`shut[]`** is wired.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_wifi_a(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@a", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@a", "value", "status");
    return ok ? 0 : -1;
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

/* Example: scall("wifi@a", "status", NULL); then talk_free if JSON */
```
