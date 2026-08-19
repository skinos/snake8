## center@nport — Mesh UDP coordinator

### Overview

UDP coordinator for gateway-to-gateway mesh (register, NAT probe, keeplive, topology version).

- Device client is **`agent@gtog`** / **`agent@net*`**
- Durable topology lives under heport `device_path` as `<user>/net/<netid>` (see `userdir/net/mynet.md`); those keys are **not** part of `center@nport` config
- Neighbor tables are pushed over **`center@heport`** TLS via `talk_hh_execute` → `agent@gtog.endpoint|branch|leaf`
- One monotonic **`seq`** per network: carried on HE pushes and on UDP keeplive reply `k;netid;seq;`


### Architecture

- **UDP**: hole listen + NAT test socket; device uses `simple_encode` outbound; server replies in plain text
- **TLS push**: nport → unix `heport.unix` → forward tid HE → device ACK
- **Hot-plug**: new joiner gets full `endpoint`; online peers get small `branch`/`leaf`; lagging peer may `s;netid;seq;` for full resync


### Dependencies

- Requires `center@heport` so `device_path` is published and unix control / hh forward work
- Device must run `agent@heclient` + `agent@gtog`


### Configuration reference ( center@nport )

```json
// Attributes introduction 
{
    "status": "enable the mesh listen service",                 // [ "disable", "enable" ], default be "enable" when unset
    "port": "UDP listen port for register / keeplive",          // [ number ], default be 20002
                                                                    // may share the number with heport TCP 20002 (different protocol)
    "nettest_port": "UDP listen port for NAT type test",        // [ number ], default be 20003
                                                                    // may share the number with heport api TCP 20003
    "key": "shared key for UDP simple encode",                  // [ string ], default be "NPORT-UDP@ashyelf.com"
                                                                    // must match agent@net*.key
    "timeout": "endpoint idle timeout on server"                // [ number ], default be 60, the unit is second
}
```

#### Configuration example

Example, show all the configure

```shell
center@nport
{
    "status": "enable",
    "port": "20002",
    "nettest_port": "20003",
    "key": "NPORT-UDP@ashyelf.com",
    "timeout": "60"
}
```

#### Configuration settings example

Example, disable the mesh listen service

```shell
center@nport:status=disable
ttrue
```

Example, change UDP listen port and shared key

```shell
center@nport:port=20012
ttrue
center@nport:key=MyMeshKey
ttrue
```



### Concepts

**UDP device → server** (decoded plaintext): `{macid};{netid};{field};{uptime};`

| field | Meaning |
|-------|---------|
| pubkey | register |
| `b` / `l` | WaitMesh trigger (FREE / LIMIT) |
| `k` | keeplive |
| `s` | request full endpoint sync (client lagging on `seq`) |

**UDP server → device** (plain): `t;netid;`, `u;netid;{json}`, `d;netid;`, `k;netid;seq;`, `r;netid;`

**Push policy**

- Online / knock: `agent@gtog.register[ netid, {listen_port} ]` when the endpoint file has **`listen_port`** (else register with no configure)
- New joiner after `b`/`l`: full `agent@gtog.endpoint` (includes `seq`)
- Each online peer: `branch` or `leaf` for the joiner only (same `seq`)
- Fail / lag: full `endpoint` to that mac only; rate-limited queue over heport unix

**All online mesh members** send raw UDP keeplive to nport (including LEAF that already has a master) so hole timeout and `seq` delivery work.



### API Reference

#### Management APIs

+ `setup[]` **start nport service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - Lifecycle method scheduled by package init

+ `shut[]` **stop nport service**
    - failed return tfalse
    - succeed return ttrue


### Other

`center@nport` exposes **config** (`status` / `port` / `nettest_port` / `key` / `timeout`) and lifecycle **`setup` / `shut`** over HE. Mesh knock/dump are **unix control** inside the nport process (`nport_call`); operators and WUI must use the HE wrappers on **`center@api`**:

| Need | HE command |
|------|------------|
| Reload one network + sync online members | `center@api.network_knock[ user, netid ]` |
| Reload one endpoint + register/sync | `center@api.endpoint_knock[ user, netid, macid ]` |
| Durable + live hole dump (whole net) | `center@api.network_dump[ user, netid ]` |
| Durable + live hole dump (one peer) | `center@api.endpoint_dump[ user, netid, macid ]` |
| Topology CRUD | `center@api.network_*` / `center@api.endpoint_*` (they knock nport after save) |

Example
```shell
center@api.network_knock[ ashyelf, mynet ]
ttrue
center@api.endpoint_dump[ ashyelf, mynet, 00037f120000 ]
```

See **`api.md`** (Mesh network) for full parameter lists and dump JSON shape.
