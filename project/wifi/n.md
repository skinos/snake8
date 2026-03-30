## wifi@n — 2.4G Radio Management
Manage 2.4G Radio. Which driver and host stack apply is **product-specific**; the **`arch`** area supplies board integration, while configuration flows through the same **`land`** / `he` model as other components.

### Configuration ( `wifi@n` )
```json
// Attribute introduction
{
    "mode":"radio mode",                 // [ "n", "b", "bg", "x" ]
                                                // "n" for 11N
                                                // "b" for 11B
                                                // "bg" for 11BG
                                                // "x" for 11AX
    
    "bandwidth":"radio bandwidth",       // [ "20", "2040", "40" ]
                                                // "20" for 20M
                                                // "2040" for 20M and 40M
                                                // "40" for 40M

    "channel":"radio channel",           // [ 0-14 ], "0" for auto select the channel
    "beacon":"beacon interval",          // [ number ]
    "country":"country code",            // [ "cn", "en", "jp", ... ]

    "dtim":"radio dtim",                 // [ number ]
    "ldpc":"use LDPC or not",            // [ "disable", "enable" ]
    "shortgi":"short GI",                // [ "disable", "enable" ]
    "stbc":"radio STBC"                  // [ "disable", "enable" ]
}
```

Example, show 2.4G SSID all configure
```shell
wifi@n
{
    "mode":"n",                # 2.4G Radio is 11N
    "bandwidth":"40",          # 2.4G bandwidth 40M
    "channel":"11",            # 2.4G channel is 11

    "beacon":"100",            # 2.4G beacon interval is 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # enable the LDPC
    "shortgi":"enable",        # enable the short GI
    "stbc":"enable"            # enable the STBC
}
```  

Example, modify the 2.4G radio channel to auto
```shell
wifi@n:channel=0
ttrue
```

Example, modify the 2.4G radio channel to 11
```shell
wifi@n:channel=11
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
wifi@n|{"mode":"an","bandwidth":"40","channel":"0"}
ttrue
```

### Component API
**Directly callable** APIs: `wifi@n.method`, `wifi@n2.method`, … (HE / eline / HTTP `/he`).

+ `chlist[]` **get the 2.4G radio channel list**   
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

    Example, get the 2.4G radio channel list
    ```shell
    wifi@n.chlist
    {
        "1":{},
        "2":{},
        "3":{},
        "4":{},
        "5":{},
        "6":{},
        "7":{},
        "8":{},
        "9":{},
        "10":{},
        "11":{},
        "12":{},
        "13":{}
    }
    ```

+ `stalist[]` **get list of clients on 2.4G radio**   
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

    Example, get the 2.4G radio's client list
    ```shell
    wifi@n.stalist
    {
        "78:11:DC:92:D3:9E":                  // client 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "88:C3:97:75:1B:C0":                 // client 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "40:31:3C:4D:78:35":                 // client 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@nssid"
        }
    }
    ```

+ `stabeat[ MAC address ]` **disconnect the client**  
    - failed return tfalse
    - error return terror    
    - succeed return ttrue

    Example, disconnect the client 00:03:7F:13:BD:30 from 2.4G Radio
    ```shell
    wifi@n.stabeat[ 00:03:7F:13:BD:30 ]
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

static int example_config_wifi_n(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@n", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@n", "value", "status");
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

/* Example: scall("wifi@n", "status", NULL); then talk_free if JSON */
```
