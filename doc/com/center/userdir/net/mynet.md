## <username>/net/<netid> — Mesh network topology file

### Overview

JSON file that stores one durable virtual mesh network for a username.

- Path: `{device_path}/<username>/net/<netid>` (`mynet` here is only an example netid)
- HE/dbs path: `center@heport/<username>/net/<netid>`
- Written by `center@api.network_add` / `endpoint_add` / `endpoint_delete`
- Loaded by `center@nport` on start and on knock
- Runtime hole fields (`ip`, `port`, `pubkey`, `nattype`) are **not** stored here; they live in nport memory and appear in dump APIs


### Configuration reference ( <username>/net/<netid> )

```json
// Attributes introduction 
{
    "status": "network switch",                     // [ "enable", "disable" ], optional; disable → UDP register gets d;netid;
    "seq": "topology version",                      // [ number ], optional on disk; nport owns live seq and may persist
    "network": "VPN CIDR",                          // [ string ], default "172.16.0.0/24"
    "keepintval": "device keeplive interval",       // [ string ], seconds; default "15"
    "keepfailed": "device keeplive fail count",     // [ string ], default "4"
    "keeptimeout": "device keeplive timeout",       // [ string ], seconds; default "15"

    "endpoint":                                     // [ json ], admin members keyed by macid
    {
        "00037f120000":                             // [ string ]: { json }, 12-char macid
        {
            "point": "VPN tunnel address",          // [ string ], required or auto-allocated inside network CIDR
            "extend": "LAN CIDRs behind gateway",   // [ string ], optional; e.g. 192.168.8.0/24,1.1.1.1/32
            "pref": "master preference",            // [ string ], optional; higher FREE peer wins master election
            "ip": "static public IP override",      // [ string ], optional; else learned from UDP hole
            "port": "static public port override",  // [ string ], optional; else learned from UDP hole
            "listen_port": "device WG/raw listen"   // [ string ], optional; pushed to device via register HE
        }
        // "...":{ ... }  How many endpoints show how many properties
    }
}
```

#### Configuration example

Example, show network file net/mynet for user ashyelf

```shell
center@heport/ashyelf/net/mynet
{                                               # return this
    "status": "enable",
    "seq": "3",
    "network": "172.16.0.0/24",
    "keepintval": "15",
    "keepfailed": "4",
    "keeptimeout": "15",
    "endpoint":
    {
        "00037f120000":
        {
            "point": "172.16.0.1",
            "extend": "192.168.8.0/24",
            "pref": "100",
            "listen_port": "10005"
        },
        "00037f120001":
        {
            "point": "172.16.0.2",
            "pref": "0"
        }
    }
}
```



### Other

- Reserved netids: `gtog`, `cmd`, `net`, `agent`, `local`, `portc`, `heclient`
- `center@api.endpoint_add` without `point` auto-allocates the next free host in `network`
- Hot-plug runtime: see `center@nport` / `agent@gtog` docs (`seq`, UDP `k;` / `s;`, HE `endpoint`/`branch`/`leaf`)
