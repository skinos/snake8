## <username>/udpmap — Persisted UDP port-forward rules

### Overview

JSON file that stores durable UDP port maps for one username.

- Path: `{device_path}/<username>/udpmap`
- HE/dbs path: `center@heport/<username>/udpmap`
- Written by `center@api.udpmap_add` / `udpmap_delete` after calling `center@pport.udp_map` / `udp_unmap`
- Reloaded by `center@pport` at service start for every user under `device_path`
- Schema matches `tcpmap`, with default `hand_proto` `"udp"`


### Configuration reference ( <username>/udpmap )

```json
// Attributes introduction 
{
    "25010":                                    // [ string ]: { json }, server UDP port number as key
    {
        "port": "server listen port",           // [ number ], same value as the top-level key
        "macid": "target gateway macid",        // [ string ], 12-char hex; `dev/<macid>/` must exist
        "hand_ip": "device local address",      // [ string ], IP or hostname inside the gateway LAN
        "hand_port": "device local port",       // [ string ], numeric port or local HE object name
        "hand_proto": "hand protocol",          // [ string ], default "udp"
        "lock": "client lock token"             // [ number ], optional; present only when > 0
    }
    // "...":{ ... }  How many UDP maps show how many properties
}
```

#### Configuration example

Example, show UDP maps for user ashyelf

```shell
center@heport/ashyelf/udpmap
{                                               # return this
    "25010":
    {
        "port": "25010",
        "macid": "00037f120000",
        "hand_ip": "192.168.8.22",
        "hand_port": "5000",
        "hand_proto": "udp"
    }
}
```



### Other

Prefer `center@api.udpmap_add` / `udpmap_list` / `udpmap_delete` over editing the file by hand. Runtime control object is `center@pport`.
