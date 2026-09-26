## wui@admin — Administrator WEB Server Management
Administration of equipment Management web page. The admin web stack is configured as **`wui@admin`**; the attributes below apply to that object after the service is bound to it.

### Configuration ( `wui@admin` )
```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ] — only disable skips starting the service on setup

    "port":"service port",                  // [ number ], 1-65535; omit or 0 = no plain HTTP listener
    "sslport":"https port",                 // [ number ], 1-65535; omit or 0 = no HTTPS listener
    "ttydport":"Terminal ttyd port",        // [ number ], 1-65535, default is 81
    "session_timeout":"session timeout",    // [ number ] seconds (evhttp idle); typical default 300
    "talk_timeout":"talk timeout",          // [ number ] seconds for /public, /he, /upload, /download; typical default 61
    "key_lifetime":"key life time",         // [ number ] seconds for session key validity; typical default 600
    "auth_object":"auth object",            // [ string ] — object used to verify login; platform default if omitted
    "auth_api":"auth api",                  // [ string ], default "match"

    "webpage_path":"document root",         // [ string ], optional; if unset, webpath uses project default misc path

    "publist":                   // /public allow-list: each command must hit at least one rule, else Auth Error; if omitted a built-in default is used
    {
        // Format: "pattern":"mode"
        // Mode must be one of (same as webs/httpd):
        //
        // equal — full-string equality
        //   String HE: entire command must equal the key
        //     e.g. "land@machine":"equal" allows only land@machine, not land@machine.status
        //   JSON HE: without op (or op is =/|) compare obj to key; with method compare "obj.op" to key
        //     e.g. "land@machine.status":"equal" allows {"obj":"land@machine","op":"status"}
        //
        // start — prefix match
        //   String HE: command must start with the key
        //     e.g. "land@machine":"start" allows land@machine, land@machine.status, land@machine:name=x
        //   JSON HE: without method, obj prefix; with method, "obj.op" prefix
        //     e.g. "ifname@":"start" allows ifname@lte / ifname@wan.status
        //
        // first2username — prefix match, and first argument must equal the logged-in username (/public has no user; this mode usually never matches there)
        //   String HE: command starts with key and [first] equals username
        //     e.g. "center@user.":"first2username" allows center@user.foo[admin,...] when username is admin
        //   JSON HE: obj prefix match and field "1" equals username
        //
        "land@machine":"equal",
        "land@machine.status":"start"
    },
    "helist":                   // /he allow-list: each command must hit at least one rule, else Auth Error; if omitted, /he has no allow-list filter
    {
        // Format: "pattern":"mode"
        // Mode must be one of (same as publist / webs/httpd):
        //
        // equal — full-string equality
        //   String HE: entire command must equal the key
        //     e.g. "land@machine":"equal" allows only land@machine, not land@machine.status
        //   JSON HE: without op (or op is =/|) compare obj to key; with method compare "obj.op" to key
        //     e.g. "land@machine.status":"equal" allows {"obj":"land@machine","op":"status"}
        //
        // start — prefix match
        //   String HE: command must start with the key
        //     e.g. "land@machine":"start" allows land@machine, land@machine.status, land@machine:name=x
        //   JSON HE: without method, obj prefix; with method, "obj.op" prefix
        //     e.g. "ifname@":"start" allows ifname@lte / ifname@wan.status
        //
        // first2username — prefix match, and first argument must equal the logged-in username (/he is authenticated, so this mode works)
        //   String HE: command starts with key and [first] equals username
        //     e.g. "center@user.":"first2username" allows center@user.foo[admin,...] when username is admin
        //   JSON HE: obj prefix match and field "1" equals username
        //
        "land@machine":"equal"
    },
    "banlist":                  // deny-list: checked before helist/publist; hit => Auth Error. If omitted, built-in default is {"service":"api","_service":"api_end"}
    {
        // Format: "pattern":"mode"
        // Mode must be one of:
        //
        // api — match method/API name only (exact), independent of object
        //   String HE: take api from obj.api / obj.api[args] / obj.api:path, compare equal to key
        //     e.g. "service":"api" bans webs@httpd.service, ifname@lte.service[x]
        //     does not ban land@service.list (method is list, not service)
        //   JSON HE: field "op" is a method (not =/|) and equals key
        //     e.g. {"obj":"webs@httpd","op":"service"} is banned
        //   Commands with no method (plain GET/SET/OR such as land@machine or land@machine={...}) are not hit by api
        //
        // api_start — match method-name prefix
        //   e.g. "lock_":"api_start" bans *.lock_imei, *.lock_imsi, ...
        //
        // api_end — match method-name suffix
        //   e.g. "_service":"api_end" bans methods ending with _service, such as foo_service, bar_service
        //   note: plain method name "service" does not end with _service; also set "service":"api"
        //
        // obj — match object name only (exact), independent of method
        //   String/JSON: if object equals key, ban any operation on that object (GET/SET/method)
        //     e.g. "center@ctrl":"obj" bans center@ctrl, center@ctrl.status, center@ctrl={...}
        //
        // obj_start — match object-name prefix
        //   e.g. "ifname@":"obj_start" bans all objects starting with ifname@ and their APIs
        //   e.g. "modem@":"obj_start" bans modem@lte, modem@lte2, ...
        //
        "service":"api",
        "_service":"api_end"
    },
    
    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        // "...":"..." You can configure multiple host who can access
        // (1) JSON object — values are IPv4 or MAC strings
        // (2) Single string — semicolon-separated list (WUI textarea)
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    },

    // custom the webpage frame
    "logo_file":"LOGO file path",                 // [ string ], file under project admin assets or device config path as deployed
    "logo_title":"Text in the middle of page",    // [ string ], or use "#NAME" / "#MODEL" for dynamic text (index.js)
    "logo_width":"LOGO width",                    // [ string ]
    "logo_height":"LOGO height",                  // [ string ]
    "logo_align":"center",                        // [ center, right ]
    "logo_model":"show or not",                   // [ enable, disable ]
    "nav_bar":"show or not",                      // [ enable, disable ]

    // custom the webpage show
    "bigversion":"show or not",                   // [ enable, disable ]
    "copyright":"show or not",                    // [ enable, disable ]
    "firmware_id":"show or not",                  // [ enable, disable ]
    "repo_online":"show or not",                  // [ disable, enable ]
    "upgrade_online":"show or not",               // [ disable, enable ]

    // custom the web menu — value "disable" hides the item; omit or other = show (ace/js)
    "menu":
    {
        "opmode":"show or not",                   // [ enable, disable ] — device page: run mode selector (device.js)
        "model":"show or not",                    // [ enable, disable ] — device page: model line (device.js)
        "development":"show or not"               // [ enable, disable ] — sidebar Development; non-std scope needs "enable" to show (index.js)
    }

}
```

Optional **`menu`** keys also used by the stock Ace UI (same `"disable"` rule): **`dashboard`**, **`utilization`**, **`interface`**, **`connection`**, **`wan`** … **`wan4`**, **`wisp`**, **`wisp2`**, **`lte`** … **`lte4`**, **`lwan`**, **`lan2`** … **`lan4`**, **`configure`**, **`software`**, **`terminal`**, **`download_log`** — see **`ace/js/index.js`**, **`ace/js/device.js`**, **`ace/js/syslog.js`**. FPK app entries may be hidden via **`menu[ "<fpk index>" ]`** matching **`land@fpk.wui_menu`**.

HTTPS uses certificate files named for the component, e.g. **`<component>.ca`**, **`<component>.crt`**, **`<component>.key`** in project configuration, when **`sslport`** is non-zero.

Example, show all the configure
```shell
wui@admin
{
    "status":"enable",             # start this service at system startup
    "login":"disable",             # you can access to webpage with no login
    "port":"80",                   # service port 80
    "sslport":"443",               # https port 443
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
Example, modify the port of web page server
```shell
wui@admin:port=2222
ttrue
```  
Example, disable the web page server
```shell
wui@admin:status=disable
ttrue
```  

Examples, change several attributes at once (**merge**)
```shell
wui@admin|{"status":"enable","port":"80","sslport":"443"}
ttrue
```

### Component API
+ `setup[]` **apply saved `wui@admin` configuration and start or skip the admin web service**, *succeed return ttrue*
    - If **`status`** is **`disable`**, the HTTP/HTTPS service is not started.
    - Otherwise starts the long-running **`service`** (static pages and `/auth`, `/he`, `/public`, `/upload`, `/download`, etc.).

    Example, run setup manually
    ```shell
    wui@admin.setup
    ttrue
    ```

+ `shut[]` **stop the admin web service**, *succeed return ttrue*
    - Stops the service instance registered for this object (same name as **`wui@admin`**).

    Example, shut down admin web
    ```shell
    wui@admin.shut
    ttrue
    ```

### Lifecycle API
+ `setup[]` — runs during **`init` → `app`** as **`wui@admin.setup`** in the default package.

+ `shut[]` — runs during **`uninit` → `app`** as **`wui@admin.shut`** in the default package.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_wui_admin(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wui@admin", "status") == NULL)
        return -1;
    ok = ssets_string("wui@admin", "value", "status");
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

/* Example: scall("wui@admin", "status", NULL); then talk_free if JSON */
```
