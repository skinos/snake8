## Management of Access control access
Management ACL of client from local ifname   
Per logical **`ifname`** (e.g. `ifname@lan`), ACL controls whether matching traffic is **dropped**, **accepted**, or **left to later rules**, using **source (`src`)**, **destination**, **ports**, optional **time windows**, and so on. Configure under **`client@acl`**; when **`status`** is **enable**, the rules you define are enforced for that **ifname**.

### **Configuration( `client@acl` )**

```json
// Attributes introduction 
{
    "interface name":                                     // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        // client access control list from this ifname
        "status":"enable or disable the access control list",       // [ "disable", "enable" ]
        "rule":                                                     // access control list, valid when "status" be "enable"
        {
            "rule name":                                                 // [ string ], user can custom the rule name
            {
                "action":"drop or accept",                               // [ "drop", "accept", "return" ]
                                                                                  // "drop" for forbid
                                                                                  // "accept" for pass
                                                                                  // "return" for no more matching
                "proto":"protocol type",                                 // [ "domain", "tcp", "udp", "all", "layer7" ]
                                                                                  // "domain", for domain filtering, "dest" should be the domain name
                                                                                  // "tcp", for TCP protocol
                                                                                  // "udp", for UDP protocol
                                                                                  // "layer7", for layer7 application protocol
                                                                                  // "all", or space or none for all protocols

                "src":"local source address",                         // [ string ] who may send (LAN side); you may also use the older key "source" if "src" is omitted
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // single MAC: 00:23:43:13:34:40
                                                                                  // space or none for all ip address
                "dest":"internet destination address",                   // [ string ] packets destination
                                                                                  // single IP: 202.96.11.32, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1,72.32,192.1, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // range of IP: 202.96.132.11-202.96.132.20, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // domain: www.baidu.com, vaild when "proto" be "domain"
                                                                                  // layer7: Future expansion, vaild when "proto" be "layer7"
                                                                                  // space for all ip address, vaild when "proto" be "tcp" or "udp" or "all"
                "destport":"internet destination port",                  // [ number ] valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space or none for all port
                "key":"keyword",                                         // [ string ] matching of keyword in packets, valid when "proto" be "tcp" or "udp" or "all"

                "timer":"specifying an effective time",                  // [ "disable", "enable" ]
                "timer_cfg":                                                 // effective time, valid when "timer" be "enable"
                {
                    "datestart":"starting date",                             // [ string ], format is YYYY-MM-DD
                    "datestop":"ending date",                                // [ string ], format is YYYY-MM-DD
                    "timestart":"start time of day",                         // [ string ], format is hh:mm:ss
                    "timestop":"end time of day",                            // [ string ], format is hh:mm:ss
                    "monthdays":"designated month days",                     // [ string ], optional, limits rule to certain days of the month
                    "weekdays":"designated week number"                      // [ string ], format is 1,2,3,..., 0 for Sunday
                }
            }
            // ... more rule
        }
    }
    // ... more ifname
}
```   

Examples, show current all settings
```shell
client@acl
{
    "ifname@lan":
    {
        "status":"enable",                      # enable access control list
        "rule":
        {
            "dis163":                             # all client cannot access domain www.163.com at all time
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.163.com",
                "timer":"enable",
                "timer_cfg":
                {
                    "datestart":"",
                    "datestop":"",
                    "timestart":"00:00:00",
                    "timestop":"23:59:59",
                    "weekdays":"1,2,3,4,5,6,7"
                }
            },
            "dis164":                             # all client cannot access domain www.qq.com
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.qq.com",
            }
        }
    }
}        
```

Examples, add a acl rule named "disqq", all the client on ifname ifname@lan cannot access domain www.qq.com (omit **`src`** or leave it empty to match any LAN source on that **ifname**)
```shell
client@acl:ifname@lan/rule/disqq={"proto":"domain","dest":"www.qq.com","action":"drop"}
ttrue
```

Examples, delete a acl rule named "disqq"
```shell
client@acl:ifname@lan/rule/disqq=
ttrue
```

### **Joint handlers**
**Joint** wiring maps events to the handlers below (separate from everyday **Component API** methods):

+ `on[]` **refresh ACL when a LAN ifname is brought up**, *succeed return ttrue, failed return tfalse*
    - **`network/on`** → **`client@acl.on`**.
    - Parameter **2** is a JSON object with **`ifname`** (e.g. `ifname@lan`).
    - In **default** / **parasite** network modes, no ACL changes are applied.
    - Otherwise, ACL for that **ifname** is cleared and reapplied from saved configuration when **`status`** is **enable**.

+ `off[]` **tear down ACL when a LAN ifname is brought down**, *succeed return ttrue*
    - **`network/off`** → **`client@acl.off`**.
    - Parameter **2** is a JSON object with **`ifname`**. Removes ACL rules for that **ifname**; if **`ifname`** is missing, nothing is done and the call still succeeds.

### **Lifecycle API**

+ `setup[]` / `shut[]` — **when implemented** for **`client@acl`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.
+


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_client_acl(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@acl", "status") == NULL)
        return -1;
    return ssets_string("client@acl", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@acl", "list", NULL); talk_free if JSON */
```

