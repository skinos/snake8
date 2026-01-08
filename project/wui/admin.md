
***
## Administrator WEB Server Management
Administration of equipment Management web page

#### Configuration( wui@admin )

```json
// Attributes introduction 
{
    "status":"start at system startup",     // [ disable, enable ]

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

3. The example gets a json from call the API status of object land@machine in background, the he command is "land@machine.status"

```html
<script>

he.bkload( [ "land@machine.status" ] ).then( function(v){
    
    // the machines is a json that return by land@machine.status
    var machines = v[0];

});

</script>

```
