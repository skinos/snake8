## ddns@scripts — DDNS Client Management

### Overview

Manage Dynamic DNS client slots backed by OpenWrt **ddns-scripts**. Prefer the **Component API** below instead of editing raw configuration when possible.
- up to three fixed client slots: **client**, **client2**, **client3**
- each enabled slot runs an independent updater process
- provider list comes from bundled service definitions under the project install tree
- started on **network/online** via **setup[]** (no boot **init** level)


### Configuration reference ( ddns@scripts )

```json
// Attributes introduction 
{
    "client":                                    // [ json ], first DDNS client slot
    {
        "status":"client status",                // [ "disable", "enable" ], default "disable"
        "extern":"extern ifname object",         // [ string ], optional interface object (e.g. ifname@wan); empty uses default gateway netdev
        "isp":"DDNS service provider name",      // [ string ], must match a name from isplist[] (e.g. "oray.com")
        "domain":"domain name",                  // [ string ], hostname / FQDN to update
        "username":"username",                   // [ string ], provider username or API key id
        "password":"password",                   // [ string ], provider password or token
        "check_interval":"check interval",       // [ number ], optional, seconds between checks, default 600
        "retry_interval":"retry interval"        // [ number ], optional, seconds between retries, default 60
    },
    "client2":                                   // [ json ], second DDNS client slot (same fields as client)
    {
        "status":"client status",                // [ "disable", "enable" ]
        "extern":"extern ifname object",         // [ string ]
        "isp":"DDNS service provider name",      // [ string ]
        "domain":"domain name",                  // [ string ]
        "username":"username",                   // [ string ]
        "password":"password",                   // [ string ]
        "check_interval":"check interval",       // [ number ]
        "retry_interval":"retry interval"        // [ number ]
    },
    "client3":                                   // [ json ], third DDNS client slot (same fields as client)
    {
        "status":"client status",                // [ "disable", "enable" ]
        "extern":"extern ifname object",         // [ string ]
        "isp":"DDNS service provider name",      // [ string ]
        "domain":"domain name",                  // [ string ]
        "username":"username",                   // [ string ]
        "password":"password",                   // [ string ]
        "check_interval":"check interval",       // [ number ]
        "retry_interval":"retry interval"        // [ number ]
    }
}
```

#### Configuration example

Example, show all DDNS configure
```shell
ddns@scripts
{
    "client":
    {
        "status":"enable",
        "isp":"oray.com",
        "domain":"example.oray.net",
        "username":"user",
        "password":"secret"
    },
    "client2":
    {
        "status":"disable",
        "isp":"dyn.com",
        "domain":"",
        "username":"",
        "password":""
    }
}
```

#### Configuration settings example

Example, disable the first client
```shell
ddns@scripts:client/status=disable
ttrue
```

Example, change the password of client
```shell
ddns@scripts:client/password=12345678
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start enabled DDNS client slots**   
    - failed return tfalse
    - succeed return ttrue
    - Starts a supervised **service** for each slot whose **status** is **enable**
    - Called automatically on **network/online**; also runs after a successful configuration save

+ `shut[]` **stop all DDNS client slots**   
    - failed return tfalse
    - succeed return ttrue
    - Stops supervised processes and clears status files for **client** / **client2** / **client3**


#### Query APIs

+ `status[]` **get runtime status of all client slots**   
    - failed return NULL
    - succeed return [ json ], map of slot name to status object

    ```json
    {
        "client":                                // [ string ]: { json }, first slot status
        {
            "state":"update state",              // [ "register", "succeed", "already", "deny", "noneed" ]
            "domain_ip":"registered DNS IP",     // [ string ], optional
            "extern_ip":"local interface IP"     // [ string ], optional
        },
        "client2":{ },                           // same shape when present
        "client3":{ }                            // same shape when present
    }
    ```

    Example, query DDNS status
    ```shell
    ddns@scripts.status
    {
        "client":
        {
            "state":"succeed",
            "domain_ip":"1.2.3.4",
            "extern_ip":"1.2.3.4"
        }
    }
    ```

+ `isplist[]` **list supported DDNS service providers**   
    - failed return NULL
    - succeed return [ json ], map of provider name to definition filename

    ```json
    {
        "oray.com":"oray.com.json",              // [ string ]: [ string ], provider id → definition file
        "duckdns.org":"duckdns.org.json"
        // "...":"..."  How many providers show how many properties
    }
    ```

    Example, list providers
    ```shell
    ddns@scripts.isplist
    {
        "oray.com":"oray.com.json",
        "no-ip.com":"no-ip.com.json"
    }
    ```


#### Control APIs

+ `service[ id ]` **run the updater for one client slot** (supervised child)   
    - id ---------------- [ string ], slot name: **client**, **client2**, or **client3**
    - failed return tfalse / terror
    - succeed does not return (process is replaced by the updater)
    - Not intended for manual invocation; use **setup[]** after enabling the slot
