## ddns@scripts — DDNS Client Management

### Overview

Manage Dynamic DNS client slots backed by OpenWrt **ddns-scripts**. Prefer the **Component API** below instead of editing raw configuration when possible.
- up to three fixed client slots: **client**, **client2**, **client3**
- each enabled slot runs an independent updater process
- provider list comes from bundled service definitions under the project install tree
- **`extern`** follows **agent@heclient** (per enabled slot, aggregated like uart DTU): empty → `default`; `disable` skips joint; `default` → `network/online`; specific ifname → `network/onextern`
- boot **`init/app`** calls **setup[]**; network events call **reset[]** for matching slots


### Configuration reference ( ddns@scripts )

```json
// Attributes introduction 
{
    "client":                                    // [ json ], first DDNS client slot
    {
        "status":"client status",                // [ "disable", "enable" ], default "disable"
        "extern":"outbound / IP source interface", // [ "disable", "default", "<ifname>" ], default "default"
                                                      // empty string is treated as "default"
                                                      // "disable": no network joint for this slot; updater still uses default gateway netdev for IP
                                                      // "default": joint on network/online; wait gateway; IP from default gateway netdev
                                                      // "ifname@wan", ...: joint on network/onextern when ifname matches; wait that iface; IP from its netdev
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
        "extern":"outbound / IP source interface", // [ "disable", "default", "<ifname>" ]
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
        "extern":"outbound / IP source interface", // [ "disable", "default", "<ifname>" ]
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
        "extern":"default",
        "isp":"oray.com",
        "domain":"example.oray.net",
        "username":"user",
        "password":"secret"
    },
    "client2":
    {
        "status":"disable",
        "extern":"default",
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

Example, bind client to LTE
```shell
ddns@scripts:client/extern=ifname@lte
ttrue
```

Example, change the password of client
```shell
ddns@scripts:client/password=12345678
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **register network joints and start enabled DDNS client slots**   
    - failed return tfalse
    - succeed return ttrue
    - Aggregates **extern** from enabled **client\*** (status≠enable ignored); registers **network/online** and/or **network/onextern** to **ddns@scripts.reset**
    - Starts a supervised **service** for each enabled slot
    - Called from **init/app** and after a successful configuration save

+ `shut[]` **unregister joints and stop all DDNS client slots**   
    - failed return tfalse
    - succeed return ttrue
    - Stops supervised processes and clears status files for **client** / **client2** / **client3**

+ `reset[ event, event data ]` **restart matching client slot services on network event**   
    - event ------------ [ string ], e.g. `network/online` or `network/onextern`
    - event data ------- [ json ], object with `"ifname"`
    - For each enabled **client\***: empty → `default`; `disable` skips; `default` only on `network/online`; specific ifname must match — then **sreset** that slot service


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
    - Waits for gateway / interface IP when **extern** is not **disable** (same as heclient); returns **ttrue** to wait for **reset** if not ready
    - Not intended for manual invocation; use **setup[]** after enabling the slot
