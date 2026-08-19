## <username>/tcpmap — Persisted TCP port-forward rules

### Overview

JSON file that stores durable TCP port maps for one username.

- Path: `{device_path}/<username>/tcpmap`
- HE/dbs path: `center@heport/<username>/tcpmap`
- Written by `center@api.tcpmap_add` / `tcpmap_delete` after calling `center@pport.tcp_map` / `tcp_unmap`
- Reloaded by `center@pport` at service start for every user under `device_path`
- Top-level keys are **server listen port numbers** as strings (static pool, typically ≥ 25000)
- Ephemeral gateway helpers (`gateway_http` / `gateway_telnet` / `gateway_ssh`) use runtime maps only and do **not** update this file


### Configuration reference ( <username>/tcpmap )

```json
// Attributes introduction 
{
    "25002":                                    // [ string ]: { json }, server TCP port number as key
    {
        "port": "server listen port",           // [ number ], same value as the top-level key
        "macid": "target gateway macid",        // [ string ], 12-char hex; `dev/<macid>/` must exist
        "hand_ip": "device local address",      // [ string ], IP or hostname inside the gateway LAN
        "hand_port": "device local port",       // [ string ], numeric port or local HE object name (e.g. uart@serial)
        "hand_proto": "hand protocol",          // [ string ], default "tcp"
        "lock": "client lock token"             // [ number ], optional; present only when > 0
    }
    // "...":{ ... }  How many TCP maps show how many properties
}
```

#### Configuration example

Example, show TCP maps for user ashyelf

```shell
center@heport/ashyelf/tcpmap
{                                               # return this
    "25002":
    {
        "port": "25002",
        "macid": "00037f120000",
        "hand_ip": "192.168.8.1",
        "hand_port": "80",
        "hand_proto": "tcp"
    },
    "25003":
    {
        "port": "25003",
        "macid": "00037f120000",
        "hand_ip": "127.0.0.1",
        "hand_port": "uart@serial",
        "hand_proto": "tcp",
        "lock": "1"
    }
}
```



### Other

Prefer `center@api.tcpmap_add` / `tcpmap_list` / `tcpmap_delete` over editing the file by hand. Runtime control object is `center@pport`.
