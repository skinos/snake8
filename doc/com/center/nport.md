## center@nport — Mesh UDP coordinator

### Overview

UDP coordinator for gateway-to-gateway mesh (register, NAT probe, keeplive, topology version).

- Device client is **`agent@gtog`** / **`agent@net*`**
- Durable topology lives under heport `device_path` as `<user>/net/<netid>` (see `userdir/net/mynet.md`); those keys are **not** part of `center@nport` config
- Neighbor tables are pushed over **`center@heport`** TLS via `talk_hh_submit` → `agent@gtog.register` / `unregister` / `endpoint` / `branch` / `leaf`
    > there is no HE `leave`; drop a member with `unregister` plus `seq` so peers `s;` when their applied `seq` does not match
- One network **temporarily supports at most 341 endpoints**. `agent@gtog.endpoint` must fit one unix datagram (`JSON_LINE_MAX` 65535). Raising that needs a redesign of the unix-domain path (nport → `heport.unix`)
- One **`seq`** per network: alignment token for the neighbor table (not greater/less). See **Concepts**. Carried on HE pushes and on `k;netid;seq;`
    > WaitMesh join does not bump `seq`


### Architecture

- **UDP**: hole listen + NAT test socket; device uses `simple_encode` outbound; server replies in plain text
- **TLS push**: nport → unix `heport.unix` → forward tid HE → device; submit is fire-and-forget
    > `talk_hh_submit` timeout is heport register `talk_timeout` (one try); pport unix submit stays 3s / one try
- **Hot-plug**: WaitMesh joiner gets full `endpoint`; already-online peers get `branch` / `leaf` for that joiner; a peer whose applied `seq` ≠ live `seq` sends `s;` for full resync
- **Relay**: `center@pport.relay_map` / `relay_unmap` over unix (`talk_pport_submit`); at service start `pport_call` `relay_clear` once (pport has no mesh netid)


### Dependencies

- Requires `center@heport` so `device_path` and `talk_timeout` are published and unix control / hh forward work
- Mesh relay listen needs `center@pport`
- Device must run `agent@heclient` + `agent@gtog`


### Configuration reference ( center@nport )

```json
// Attributes introduction 
{
    "status": "enable the mesh listen service",                 // [ "disable", "enable" ], default be "enable" when unset
    "port": "UDP listen port for Query / Connect / keeplive",   // [ number ], default be 20002
                                                                    // may share the number with heport TCP 20002 (different protocol)
    "nettest_port": "UDP listen port for NAT type test",        // [ number ], default be 20003
                                                                    // may share the number with heport api TCP 20003
    "key": "shared key for UDP simple encode",                  // [ string ], default be "NPORT-UDP@ashyelf.com"
                                                                    // built-in; not shown on nport.html / not for customer edit
                                                                    // must match agent@net*.key
    "timeout": "stale keeplive / skip peer in neighbor push"    // [ number ], default be 60, the unit is second
                                                                    // last_keeplive older than this: keeplive replies r; and push skips the peer
                                                                    // does not unmap a borrowed relay
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

Example, change UDP listen port

```shell
center@nport:port=20012
ttrue
```



### Concepts

**UDP device → server** (decoded plaintext): `{macid};{netid};{op};{arg};`

| op | Meaning | arg |
|----|---------|-----|
| `q` | Query nets for this mac | uptime (ignored) |
| pubkey | Connect / register dial | uptime (ignored) |
| `b` / `l` | WaitMesh (FREE / LIMIT) | **listen_port** (1..65535; overwrites center memory; used for relay_map) |
| `k` | keeplive | uptime |
| `s` | request full endpoint sync | local seq |

**UDP server → device** (plain): `q;*;{json}`, `t;netid;`, `u;netid;{json}`, `d;netid;`, `k;netid;seq;`, `r;netid;`

| reply | When |
|-------|------|
| `q;*;{ "netid":{}, ... }` | Query: enabled nets this mac belongs to |
| `t;` then `u;` | Connect ok (test socket then hole socket) |
| `d;` | no net / disable / not a member |
| `k;netid;seq;` | keeplive ok (already online, hole unchanged, not stale) |
| `r;` | must Connect again: not online, hole lost/changed, stale keeplive, or admin CIDR/keepalive change |

Connect stores hole and pubkey only. Online starts at `b` / `l`.

**Push policy**

- `endpoint_add` / member change / net just enabled / CIDR·keepalive change: `agent@gtog.register[ netid, { network, keep*, listen_port? } ]`. `listen_port` is sent only when memory has it (`> 0`)
- WaitMesh `b`/`l` reports the actual local listen; center stores it then `join_push`
- Joiner: full `agent@gtog.endpoint` (online peers + `seq`)
- Each other online peer (keeplive not stale): joiner is a hub (`FREE` or `relay_port`) → `branch`; else `leaf`. A leaf with no `extend` is sent only to hubs
- Delete member / delete net / net just disabled: HE `unregister` (and unmap the **remembered** relay port). No `leave`. Caller bumps `seq`; online peers get `k;seq` and `s;` when it does not match
- Fail / lag: full `endpoint` to that mac only (`s;`)

**Endpoint count (unix datagram)**

`talk_hh_submit` is one unix datagram: `xxxxxxxxxxxx-` + HE JSON, `JSON_LINE_MAX` **65535** (including NUL). A full `agent@gtog.endpoint` (`seq` plus every online peer from `nport_peer_json`) must fit in that packet. Extra peers are truncated and never leave nport.

With `point` / `pubkey` / `nattype` / `ip` / `port` / `pref` plus `extend` and `relay_port` (the largest usual peer), about **341** neighbors fit. **Each network temporarily supports at most 341 endpoints.** To support more, redesign the unix-domain communication (nport → `heport.unix`); do not only raise a counter. Peers without `extend` / `relay_port` can squeeze more into 65535 (~430–450), but 341 is the current supported max.

**seq**

`seq` is an alignment token. Equal means the device already has this table; unequal means `s;` for a full `endpoint`. It is not a greater/less version.

| Copy | Who writes | When it changes |
|------|------------|-----------------|
| Disk `<user>/net/<netid>` | `center@api` only | create = `1`; `network_modify` / `endpoint_add` / `endpoint_delete` = `+1` |
| Live nport memory | load from disk; `++` on knock | membership / CIDR / keepalive / disable change; never written back to the file |
| Device register | HE payload when `seq>0` | copied as-is; gtog reset/setup sets `0` |

`k;netid;seq;` and HE `seq` are the **live** value. WaitMesh / join `branch`/`leaf` / hole `r;` do not change `seq`. After nport restart, live is the disk number again; devices `s;` if theirs differs.

**All online mesh members** send raw UDP keeplive to nport so hole timeout and live `seq` delivery work.

**Center UDP relay**

**`relay` makes a NAT member a hub.** FREE (`b`) or a member with **`relay_port`** is a branch; others are leaves. A NAT hub cannot receive WireGuard UDP on its hole, so peers use the center UDP. New members default to **`disable`**. When the wish is `auto` + LIMIT, or `enable`, and `listen_port > 0`, nport calls **`center@pport.relay_map[ ,mac,127.0.0.1,listen_port,udp,0,0 ]`** after WaitMesh and fills **`pref`** with **`ENDPOINT_PREF_RELAY` (50)** if disk pref is empty (FREE fills **100**). Push JSON keeps the real hole in `ip`/`port` and adds **`relay_port`**. `relay_ip` is reserved and omitted; the device uses `agent@portc` / `agent@heclient` `server` as the center address.

- Adding a member does not reserve a port. Borrow happens on WaitMesh (or knock returns a port the member should no longer hold)
- Same mac + same `127.0.0.1` + same listen + `udp` reuses the existing pport slot (`map` with port 0)
- Duplicate `listen_port` for the same device across meshes is illegal
- Offline / hole reset / stale keeplive **keep** the remembered port (no unmap)
- Delete / disable / no longer want: `relay_unmap` **port + mac** of the remembered listen only. Unmap-by-mac-only is not used (would steal the other net's slot)
- After nport restart memory has no `relay_port`. Service start waits for pport `status=enable` then `pport_call` `relay_clear` so leftover `timeout=0` slots are gone before WaitMesh. Next keeplive is `r;` → Connect + WaitMesh, then a new `relay_map`
- Not a user `udpmap` (those start at `static_port` and persist under `<user>/udpmap`)
- Do not use `center@pport.dynamic_port[]` (TCP counter only)
- `timeout=0` on `relay_map` so idle does not unmap
- `nattype` stays FREE / LIMIT. Center and device both treat `FREE` or `relay_port` as a hub (`branch`). ICMP still pings the tunnel `point`
- Per-user cap: `{device_path}/<user>/config` **`relay_max`**, counted in nport `nport_relay_limit` (`max` / `current`, one object per user). Unset = unlimited. `0` = do not `relay_map`. `N` = at most N remembered borrows across that user's meshes. Over the cap the member stays a leaf. Lowering the cap does not unmap listens already up. `current` is process memory (starts at 0 after nport restart)



### API Reference

#### Management APIs

+ `setup[]` **start nport service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - Lifecycle method scheduled by package init

+ `shut[]` **stop nport service**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[ netid ]` **operator view of live network status**
    - netid -------------- [ string ], optional, network id
    - failed return NULL
    - succeed return [ json ]
    - No netid: every loaded network, `netid` → `{ user, seq, status }`
    - With netid: that network’s `{ user, seq, status }`. Missing netid returns NULL (`ENOENT`)
    - Member run state is **`center@nport.endpoint_status`**. Full memory fields are **`center@nport.network_dump`** / **`center@nport.endpoint_dump`** (debug)

    Example, all networks
    ```shell
    center@nport.status
    {
        "mynet":
        {
            "user":"ashyelf",
            "seq":"3",
            "status":"enable"
        }
    }
    ```

    Example, one network
    ```shell
    center@nport.status[ mynet ]
    {
        "user":"ashyelf",
        "seq":"3",
        "status":"enable"
    }
    ```

+ `endpoint_status[ netid, [macid] ]` **operator view of live endpoint status**
    - netid -------------- [ string ], network id
    - macid -------------- [ string ], optional, 12-hex
    - failed return NULL
    - succeed return [ json ]
    - No macid: every member, `macid` → slim run state (`online` / `stale` / hole or static `ip`·`port` / `listen_port` / `nattype` 1=FREE 2=LIMIT / `relay`·`relay_port`)
    - With macid: that member’s slim object. Missing net or mac returns NULL (`ENOENT`)
    - Slim on purpose (unix datagram ~64KB). Full memory fields are **`center@nport.endpoint_dump`**
    - The neighbor table actually pushed to devices is **`center@nport.network_endpoint`**

+ `network_endpoint[ netid ]` **same JSON as `agent@gtog.endpoint` cmd.2**
    - netid -------------- [ string ], required, network id
    - failed return NULL
    - succeed return [ json ]
    - Snapshot of what every recipient gets on HE `endpoint` / `s;` / WaitMesh joiner table: `seq` plus each **online and not stale** peer via `nport_peer_json` (`point` / `extend` / `pubkey` / `nattype` / hole or static `ip`·`port` / `pref` / `relay_port`)
    - All recipients get this same object. Offline and stale peers are omitted (they are not pushed)
    - Must fit one unix datagram (65535). One network **temporarily supports at most 341 endpoints**; more needs a redesign of the unix-domain path
    - Missing netid returns NULL (`ENOENT`)
    - Not `endpoint_status` (that is the operator slim view of every member)

    Example
    ```shell
    center@nport.network_endpoint[ mynet ]
    {
        "seq":"3",
        "00037f120000":
        {
            "point":"172.16.32.2",
            "extend":"192.168.8.0/24",
            "pubkey":"...",
            "nattype":"2",
            "ip":"1.2.3.4",
            "port":"54321",
            "pref":"100",
            "relay_port":"20006"
        }
    }
    ```

    Example, all members of one network
    ```shell
    center@nport.endpoint_status[ mynet ]
    {
        "00037f120000":
        {
            "point":"172.16.32.2",
            "online":"true",
            "ip":"1.2.3.4",
            "port":"54321",
            "listen_port":"10005",
            "nattype":"2",
            "relay":"auto",
            "relay_port":"20006"
        }
    }
    ```

    Example, one member
    ```shell
    center@nport.endpoint_status[ mynet, 00037f120000 ]
    {
        "point":"172.16.32.2",
        "online":"true",
        "ip":"1.2.3.4",
        "port":"54321",
        "listen_port":"10005",
        "nattype":"2",
        "relay":"auto",
        "relay_port":"20006"
    }
    ```

+ `network_dump[ netid ]` **debug dump of one network’s full memory**
    - netid -------------- [ string ], required
    - failed return NULL
    - succeed return [ json ]
    - All network fields plus every endpoint via `nport_endpoint_dump` (static `ip`/`port`, `hole_*`, `last_*`, `pubkey`, …)

    Example
    ```shell
    center@nport.network_dump[ mynet ]
    ```

+ `endpoint_dump[ netid, macid ]` **debug dump of one endpoint’s full memory**
    - netid -------------- [ string ], required
    - macid -------------- [ string ], required, 12-hex
    - failed return NULL
    - succeed return [ json ]

    Example
    ```shell
    center@nport.endpoint_dump[ mynet, 00037f120000 ]
    ```


### Other

`center@nport` exposes **config** (`status` / `port` / `nettest_port` / `key` / `timeout`), lifecycle **`setup` / `shut`**, and live **`status`** / **`endpoint_status`** / **`network_endpoint`** / **`network_dump`** / **`endpoint_dump`** over HE. Mesh knock is **unix control** (`nport_call`); operators and WUI also use **`center@api`**:

| Need | HE command |
|------|------------|
| Live status of all nets / one net | `center@nport.status` / `center@nport.status[ netid ]` |
| Live status of one net’s members / one peer | `center@nport.endpoint_status[ netid ]` / `center@nport.endpoint_status[ netid, macid ]` |
| Neighbor table pushed to devices (`gtog.endpoint` cmd.2) | `center@nport.network_endpoint[ netid ]` |
| Debug dump one net / one peer (all fields) | `center@nport.network_dump[ netid ]` / `center@nport.endpoint_dump[ netid, macid ]` |
| Reload one network + sync online members | `center@api.network_knock[ user, netid ]` |
| Reload one endpoint + register (or unregister if gone) | `center@api.endpoint_knock[ user, netid, macid ]` |
| Live status via api (user scoped) | `center@api.network_status` / `center@api.endpoint_status` |
| Neighbor table via api (user scoped) | `center@api.network_endpoint[ user, netid ]` |
| Topology CRUD | `center@api.network_*` / `center@api.endpoint_*` (they knock nport after save) |

Example
```shell
center@api.network_knock[ ashyelf, mynet ]
ttrue
center@api.endpoint_status[ ashyelf, mynet, 00037f120000 ]
```

See **`api.md`** (Mesh network) for user-scoped list / status. Dump stays on **`center@nport`**.
