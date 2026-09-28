## <username>/net/<netid> — Mesh network topology file

### Overview

JSON file that stores one durable virtual mesh network for a username.

- Path: `{device_path}/<username>/net/<netid>` (`mynet` here is only an example netid)
- HE/dbs path: `center@heport/<username>/net/<netid>`
- Written by `center@api.network_add` / `network_modify` / `endpoint_add` / `endpoint_delete`
- Loaded by `center@nport` on start and on knock
- Runtime hole / relay fields (`ip`, `port`, `pubkey`, `nattype`) are **not** stored here; they live in nport memory and appear in dump APIs
- Optional **`relay`** makes a NAT member a hub. Ordinary spokes leave it `disable`. The allocated port is never written here.


### Configuration reference ( <username>/net/<netid> )

```json
// Attributes introduction 
{
    "status": "network switch",                     // [ "enable", "disable" ], optional; disable → UDP register gets d;netid;
    "seq": "disk alignment seq",                    // [ number ], optional on disk; api writes it (create=1, later +1); nport loads then may ++ live only; never written back by nport
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
            "pref": "hub preference",               // [ string ], optional; each device elects a live hub (pref, then macid)
            "ip": "static public IP override",      // [ string ], optional; else learned from UDP hole or center relay
            "port": "static public port override",  // [ string ], optional; else learned from UDP hole or center relay
            "listen_port": "device WG/raw listen",  // [ string ], optional; pushed to device via register HE
            "relay": "center UDP for a NAT hub"     // [ "auto", "enable", "disable" ], optional, default "disable"
                                                                        // FREE or relay_port → hub (branch); else leaf
                                                                        // "disable": never use a center UDP (omit / empty is disable)
                                                                        // "auto": if this member is behind NAT, open a center UDP after it is online
                                                                        // "enable": always use a center UDP; do not store the allocated port here
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
            "listen_port": "10005",
            "relay": "enable"
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
- **`relay` makes a NAT member a hub.** Ordinary spokes stay `disable`
- New members default to **`disable`** (omit / empty is the same)
- `auto`: if that member is behind NAT, the center opens a public UDP **only after it comes online**. You do not choose the port
- `enable` always uses that public UDP. `disable` never does
- The account file `<user>/config` **`relay_max`** caps how many of those public UDP listens this user may hold: omit = unlimited, `0` = none, `N` = at most N. Same mac + same listen reuses one pport slot. Over the cap the member stays a leaf. Admin sets this on `center@ctrl`, not on the net file
- Look at `center@api.endpoint_status` for the live hole `ip`/`port` and `relay_port`. Offline / hole reset **keep** the remembered port. Delete / disable / no longer want unmaps that port+mac. After nport restart the port is forgotten until WaitMesh
- This is not a Port Proxy `udpmap` rule (those start at `static_port` and stay on the port-proxy page)
- Hot-plug runtime: see `center@nport` / `agent@gtog` docs (`seq`, UDP `k;` / `s;` / `r;`, HE `register`/`unregister`/`endpoint`/`branch`/`leaf`)
