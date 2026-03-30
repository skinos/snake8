## forward@ttl — TTL settings
Management of outgoing IP packet TTL per LAN **ifname**

### Configuration ( `forward@ttl` )
```json
// Attributes introduction 
{
    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], TTL policy for this ifname
    {
        "mode":"mode fo ttl settings",     // [ "disable", "fix", "inc", "dec" ], ttl modify mode; "disable" turns TTL off (WUI); legacy "none" is treated like disable
        "fix":"output packet TTL",         // [ number ], vaild when "mode" be "fix"
        "inc":"increase the TTL",          // [ number ], vaild when "mode" be "inc"
        "dec":"reduce the TTL"             // [ number ], vaild when "mode" be "dec"
    }
    // ... more ifname

}
```   

Example, show current all of ttl configure
```shell
forward@ttl
{
    "ifname@lan":
    {
        "mode":"fix",     // fix the output packet ttl
        "fix":"99"        // ttl number is 99
    }
}
```  

Example, modify the ttl 70
```shell
forward@ttl:ifname@lan/fix=70
ttrue
```  

Examples, merge TTL for one LAN
```shell
forward@ttl:ifname@lan|{"mode":"fix","fix":"64"}
ttrue
```

### Component API
Use standard **`forward@ttl`** get/set/merge for configuration.

+ `on[]` **re-apply TTL policy after a LAN ifname comes up**, *succeed return ttrue*
    - Parameter **2** carries **`ifname`**; TTL settings for that **ifname** are applied again from saved configuration.

+ `off[]` **remove TTL mangling for an ifname**

### Lifecycle API
+ `setup[]` / `shut[]` — **not** wired in the default **init** / **uninit** schedule for this component; use **`on[]`** / **`off[]`**.

### Joint Handlers
| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@ttl.on` |

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_ttl(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@ttl", "status") == NULL)
        return -1;
    return ssets_string("forward@ttl", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@ttl", "list", NULL); talk_free if JSON */
```
