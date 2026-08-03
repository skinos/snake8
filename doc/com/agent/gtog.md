## agent@gtog — Gateway-to-gateway WireGuard mesh manager

### Overview

Manage a pool of WireGuard mesh VPN channels (**`agent@net`**, **`agent@net2`**, …). **`agent@gtog`** owns pool limits, object↔`netid` mapping, and runtime **`register`** / **`unregister`**. Each channel object runs the same **`gtog`** binary and holds per-network configuration, peers, and the long-running **`service`**.

- Bring up persistent channels from on-disk config at boot (**`setup`** / **`shut`**)
- Create or destroy runtime channels by **`netid`** (**`register`** / **`unregister`**, config under **`=cache`**)
- Accept mesh topology from the coordinator via heclient: **`endpoint`** (full map), **`branch`** / **`leaf`** (incremental peer)
- Coordinate with **`center@nport`** over UDP (register / NAT / keeplive / sync); one network **`seq`** on pushes and `k;netid;seq;`
    > Per-channel options (`server`, `netid`, keepalive, DNS, routes, …) are documented in **`net.md`**


### Configuration reference ( agent@gtog )

```json
// Attributes introduction 
{
    "net_max":"maximum number of WireGuard channel slots",      // [ number ], default 10
    "port_start":"base local WireGuard listen_port"             // [ number ], default 10004
                                                                   // agent@net uses port_start; agent@netN uses port_start+N-1 when channel listen_port is unset
}
```

#### Configuration example

Example, show all the configure

```shell
agent@gtog
{
    "net_max":"10",                         # up to 10 channel slots
    "port_start":"10004"                    # first channel listen_port when unset
}
```

#### Configuration settings example

Example, set max networks to 5

```shell
agent@gtog:net_max=5
ttrue
```

Example, merge set pool limits( include "net_max" "port_start" )

```shell
agent@gtog|{"net_max":"8","port_start":"10004"}
ttrue
```


### Concepts

**Channel object pool**

All channels share names **`agent@net`**, **`agent@net2`**, … up to **`net_max`**. Callers work with **`netid`** ↔ **`agent@net*`**; they need not care whether the slot came from product config or runtime cache.

| Path | APIs | Config store | Lifetime |
|------|------|--------------|----------|
| Boot / product | **`setup[]`** / **`shut[]`** on **`agent@gtog`** | Normal project config | Survives reboot; brought up again by **`agent@gtog.setup`** |
| Runtime | **`register[]`** / **`unregister[]`** | **`register`** sets **`=cache`** (under `/tmp`); **`unregister`** uses **`=nocache`** and drops the cache | Gone after reboot until **`register`** runs again |

- **`setup`**: store **`net_max`** / **`port_start`** in register; for each slot that already has config, map object→`netid`, publish config **`port`** (coordinator) and **`listen_port`** (local; or **`port_start`** formula) into channel register, register with **`network@frame`**, schedule **`…setup`** on init delay.
- **`register`**: same **`netid`** reuses the same object; a new **`netid`** takes a free slot (skip map-occupied and slots that already have on-disk config), then start/reset **`service`**.

**Service phase, role, and net_state**

Each channel **`service`** uses three axes (also returned by **`list`** / **`state`**):

| Field | Meaning | Values |
|-------|---------|--------|
| **`phase`** | Service lifecycle | `0=init`, `1=server_dial`, `2=run` (includes waiting for topology), `3=exit` (legacy value `2=wait_mesh` may still appear until fully collapsed) |
| **`role`** | Mesh role of this device | `0=none`, `1=master`, `2=branch`, `3=leaf` |
| **`net_state`** | Whether a usable master exists | `0=unknown`, `1=no_master`, `2=has_master` |
| **`seq`** | Last applied topology version from coordinator | matches `seq` on `endpoint`/`branch`/`leaf` and on UDP `k;netid;seq;` |

Phases: **Init** (WireGuard iface) → **ServerDial** (UDP register → `u;` network JSON) → **Run** (UDP `b`/`l` until role+endpoint ready, then keeplive + **`network@frame.online`**) → **Exit**. Peers return only after **`endpoint`** / **`branch`** / **`leaf`**. Channel outbound IP / joint retry: **`agent@net*.reset`** in **`net.md`**.

**Keepalive (Run)**

| Target | Who | Purpose |
|--------|-----|---------|
| UDP `'k'` to **`center@nport`** | **every** online role (master / branch / leaf) | hole + receive `k;netid;seq;` |
| ICMP to master VPN IP | branch / leaf with master | tunnel quality |

If local **`seq`** stays behind coordinator **`seq`** for about **`keepfailed`** periods, device sends UDP **`s;netid;local_seq;`** so nport pushes a full **`endpoint`**. Timers clamp as before (`keepintval` ≥ 5, …).

**Topology APIs** (from coordinator over heclient)

- **`endpoint`**: full replace + optional **`seq`**; program WG; elect role; store **`%s.endpoint`**; unix **`reload`**.
- **`branch`** / **`leaf`**: incremental add/update + optional **`seq`** when **`role != none`**; full **`endpoint`** still required to drop peers.
- Dual call style: **`agent@gtog.<api>[ netid, … ]`** or **`agent@net*.<api>[ … ]`**.


### API Reference

#### Management APIs

+ `setup[]` **bring up the gtog pool or one channel service**
    - On **`agent@gtog`**: apply **`net_max`** / **`port_start`**, map slots that already have config, register them with **`network@frame`**, schedule each **`agent@net*.setup`** on init delay
    - On **`agent@net*`**: start that channel’s **`service`** when channel **`status`** is **`enable`**
    - failed return tfalse
    - succeed return ttrue
    - Scheduled by FPK init (**`manage`**: **`agent@gtog.setup`**)

    Example, setup the gtog infrastructure
    ```shell
    agent@gtog.setup
    ttrue
    ```

+ `shut[]` **tear down the gtog pool or one channel**
    - On **`agent@gtog`**: clear object→`netid` map; **`shut`** each present channel; unregister from **`network@frame`**
    - On **`agent@net*`**: unregister network joints, offline, stop **`service`**, bring the WireGuard interface down
    - failed return tfalse
    - succeed return ttrue

    Example, shutdown all gtog networks
    ```shell
    agent@gtog.shut
    ttrue
    ```


#### Query APIs

+ `list[]` **list mapped channels (on agent@gtog) or the endpoint file (on agent@net*)**
    - On **`agent@gtog`**: slots that currently have an object→`netid` entry
    - On **`agent@net*`**: JSON of **`%s.endpoint`** for that channel (see **`net.md`**)
    - failed return NULL
    - succeed return [ json ], channel map or endpoint list
    ```json
    {
        "network object name":                      // [ string ]: { json }, e.g. "agent@net"
        {
            "netid":"network identifier",           // [ string ]
            "netdev":"WireGuard interface name",    // [ string ]
            "port":"coordinator UDP port",          // [ number ]
            "listen_port":"local WireGuard listen", // [ number ]
            "role":"mesh role",                     // [ number ], 0=none, 1=master, 2=branch, 3=leaf
            "net_state":"master presence",          // [ number ], 0=unknown, 1=no_master, 2=has_master
            "phase":"service lifecycle phase",      // [ number ], 0=init, 1=server_dial, 2=run, 3=exit
            "pref":"self preference value"          // [ number ]
        }
        // "...":{ ... }  How many mapped channels show how many properties
    }
    ```

    Example, list all mapped networks
    ```shell
    agent@gtog.list
    {
        "agent@net":
        {
            "netid":"office-vpn",
            "netdev":"net",
            "port":"20002",
            "listen_port":"10004",
            "role":"2",
            "net_state":"2",
            "phase":"2",
            "pref":"50"
        }
    }
    ```

#### Control APIs

+ `register[ netid, configure ]` **bind a netid to a free or existing channel and start service**
    - netid -------------------- [ string ], network identifier
    - configure ---------------- [ json ], optional, merged into channel cache config (see **`net.md`**)
    - Keys in **`configure`** overlay the existing cache; omitted keys are kept. Center **`register`** HE may push only **`listen_port`**.
    - If the channel service is already running with the same **`port`** / **`listen_port`** and merged config is unchanged (or **`configure`** omitted), keep the mesh without restart; a changed config is applied and the service is reset
    ```json
    {
        "server":"coordinator address",             // [ string ], optional
        "port":"coordinator UDP port",              // [ number ], optional, default 20002
        "listen_port":"local WireGuard listen",     // [ number ], optional, default port_start formula
        "netid":"network identify",                 // [ string ], optional, overwritten by argument netid
        "network":"VPN CIDR",                       // [ string ], optional
        "keepintval":"keeplive interval",           // [ number ], optional
        "keepfailed":"keeplive fail count",         // [ number ], optional
        "keeptimeout":"keeplive timeout"            // [ number ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Sets channel **`=cache`**, maps object→`netid`, registers **`network@frame`**, **`sstart`** / **`sreset`** **`service`**

    Example, register a network with minimal options
    ```shell
    agent@gtog.register[ office-vpn, {"port":"20002","network":"10.0.1.0/24"} ]
    ttrue
    ```
    Example, center push of local listen port only
    ```shell
    agent@gtog.register[ office-vpn, {"listen_port":10005} ]
    ttrue
    ```

+ `unregister[ netid ]` **stop and unbind a runtime channel by netid**
    - netid ---- [ string ], network identifier to remove
    - failed return tfalse
    - succeed return ttrue
    - Offline/stop service, clear map entry, **`=nocache`**

    Example, unregister a network
    ```shell
    agent@gtog.unregister[ office-vpn ]
    ttrue
    ```

+ `endpoint[ netid, endpoint list ]` **replace the full endpoint map for a network**
    - netid ---------------- [ string ], network identifier
    - endpoint list -------- [ json ], map keyed by device macid
    ```json
    {
        "endpoint mac identify":                    // [ string ]: { json }
        {
            "ip":"public internet ip",              // [ ip address ]
            "port":"public internet port",          // [ number ]
            "pubkey":"WireGuard public key",        // [ string ]
            "nattype":"NAT class",                  // [ number ]: [ 1, 2 ], 1=FREE (relay-capable), 2=LIMIT (leaf)
            "pref":"master preference",             // [ number ], higher wins among FREE peers
            "point":"VPN tunnel ip",                // [ ip address ]
            "extend":"local networks via this peer" // [ string ], optional, e.g. "192.168.1.0/24"
        }
        // "...":{ ... }  How many endpoints show how many properties
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Self macid must exist in the map; programs WireGuard; elects **`role`** / **`net_state`**; unix **`reload`**

    Example, push a full endpoint list
    ```shell
    agent@gtog.endpoint[ office-vpn, {"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}} ]
    ttrue
    ```

+ `branch[ netid, branch information ]` **add or update one FREE (branch) peer**
    - netid -------------------- [ string ], network identifier
    - branch information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "ip":"public internet ip",                  // [ ip address ]
        "port":"public internet port",              // [ number ]
        "pubkey":"WireGuard public key",            // [ string ]
        "nattype":"NAT class",                      // [ number ], usually 1 (FREE)
        "pref":"master preference",                 // [ number ]
        "point":"VPN tunnel ip",                    // [ ip address ]
        "extend":"local networks via this peer"     // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Requires existing **`.endpoint`** and **`role != none`**; may promote peer to master when **`pref`** is higher; does not delete other peers

    Example, add a branch peer
    ```shell
    agent@gtog.branch[ office-vpn, {"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"} ]
    ttrue
    ```

+ `leaf[ netid, leaf information ]` **add or update one LIMIT (leaf) peer**
    - netid ------------------ [ string ], network identifier
    - leaf information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "ip":"public internet ip",                  // [ ip address ], optional for leaf peers
        "port":"public internet port",              // [ number ], optional
        "pubkey":"WireGuard public key",            // [ string ]
        "point":"VPN tunnel ip",                    // [ ip address ]
        "extend":"local networks via this peer"     // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Requires existing **`.endpoint`** and **`role != none`**; does not re-elect master

    Example, add a leaf peer
    ```shell
    agent@gtog.leaf[ office-vpn, {"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def...","point":"10.0.1.2","extend":"192.168.2.0/24"} ]
    ttrue
    ```

Channel **`online`** / **`offline`** / **`reset`** (and per-channel **`state`**) are documented in **`net.md`**.
