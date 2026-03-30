## wui@admin — Administrator WEB Server Management
Administration of equipment Management web page. The admin web stack is configured as **`wui@admin`**; the attributes below apply to that object after the service is bound to it.

### Configuration ( `wui@admin` )
```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ] — only disable skips starting the service on setup

    "port":"service port",                  // [ number ], 1-65535; omit or 0 = no plain HTTP listener
    "sslport":"https port",                 // [ number ], 1-65535; omit or 0 = no HTTPS listener
    "termport":"Terminal port",             // [ number ], 1-65535, default is 81
    "session_timeout":"session timeout",    // [ number ] seconds (evhttp idle); typical default 300
    "talk_timeout":"talk timeout",          // [ number ] seconds for /public, /he, /upload, /download; typical default 61
    "key_lifetime":"key life time",         // [ number ] seconds for session key validity; typical default 600
    "auth_object":"auth object",            // [ string ] — object used to verify login; platform default if omitted
    "auth_api":"auth api",                  // [ string ], default "match"

    "webpage_path":"document root",         // [ string ], optional; if unset, webpath uses project default misc path

    "publist":                   // valid public command list; if omitted, a built-in default allow list may apply
    {
        "command match":"compare type"     // [ string ]: "sub" = substring, "equal" = full match, any other value = prefix match
    },
    "helist":                   // valid he command list (same compare rules as publist)
    {
        "command match":"compare type"
        // ... more the command match
    },

    "manager":                              // Only the specified IP address or MAC address is allowed for access
    {
        // "...":"..." You can configure multiple host who can access
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    },

    // custom the webpage html
    "css_file":"CSS file path",                   // [ string ], Fill in the file name must be in located on /PRJ/wui/admin/assets/css/ or /mnt/config/wui/, via <%csspath(); %> show
    "logo_file":"LOGO file path",                 // [ string ], Fill in the file name must be in located on /PRJ/wui/admin/assets/css/ or /mnt/config/wui/
    "login_file":"file path",                     // [ string ], read only, Fill in the file name must be in located on /PRJ/wui/admin/ or /mnt/config/wui/
    "index_file":"file path",                     // [ string ], read only, Fill in the file name must be in located on /PRJ/wui/admin/ or /mnt/config/wui/

    // custom the webpage frame
    "logo_title":"Text in the middle of page",    // [ string ]
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

    // custom the web menu
    "menu":
    {
        "wan":"show or not",                      // [ enable, disable  ]
        "wan2":"show or not",                     // [ enable, disable  ]
        "wisp":"show or not",                     // [ enable, disable  ]
        "wisp2":"show or not",                    // [ enable, disable  ]
        "lte":"show or not",                      // [ enable, disable  ]
        "lte2":"show or not",                     // [ enable, disable  ]
        "lan":"show or not",                      // [ enable, disable  ]
        "sta":"show or not",                      // [ enable, disable  ]
        "connection":"show or not",               // [ enable, disable  ]
        "opmode":"show or not",                   // [ enable, disable  ]
        "language":"show or not",                 // [ enable, disable  ]

        "terminal":"show or not",                 // [ enable, disable  ]
        "development":"show or not"               // [ enable, disable  ]
    }

}
```


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
