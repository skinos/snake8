## agent@gtog — Gateway-to-gateway WireGuard mesh manager

### Overview

Manage a pool of WireGuard mesh VPN channels (**`agent@net`**, **`agent@net2`**, …). **`agent@gtog`** owns pool limits, object↔`netid` mapping, and runtime **`register`** / **`unregister`**. Each channel object runs the same **`gtog`** binary and holds per-network configuration, peers, and the long-running **`service`**.

- On **`setup`**, Query **`center@nport`** for networks of this device and **`register`** each **`netid`** so the channel **`service`** starts
- Create or destroy runtime channels by **`netid`** (**`register`** / **`unregister`**, config under **`=cache`**)
- Accept mesh topology from the coordinator via heclient: **`endpoint`** (full map), **`branch`** / **`leaf`** (incremental add), **`leave`** (incremental drop)
- Coordinate with **`center@nport`** over UDP (Query / Connect / NAT / keeplive / sync); one network **`seq`** on pushes and `k;netid;seq;`
    > Per-channel options (`server`, `netid`, keepalive, DNS, routes, …) are documented in **`net.md`**


### Configuration reference ( agent@gtog )

```json
// Attributes introduction 
{
    "status":"pool switch",                                     // [ "disable","enable" ], setup starts query only when enable
    "net_max":"maximum number of WireGuard channel slots",      // [ number ], default 10
    "listen_port":"base local WireGuard listen_port",           // [ number ], default 10004
                                                                   // agent@net uses listen_port; agent@netN uses listen_port+N-1 when channel listen_port is unset
    "port":"coordinator UDP port",                              // [ number ], default 20002
    "key":"shared key with the coordinator",                    // [ string ], default "NPORT-UDP@ashyelf.com"
    "server":"coordinator address",                             // [ string ], optional; empty → agent@heclient.server
    "keepintval":"query retry interval in seconds",             // [ number ], default 15 (≥5); also used while Query has no reply
    "keeptimeout":"query UDP wait in seconds"                   // [ number ], default 15; clamped to < keepintval
}
```

#### Configuration example

Example, show all the configure

```shell
agent@gtog
{
    "net_max":"10",                         # up to 10 channel slots
    "listen_port":"10004",                  # first channel listen_port when unset
    "port":"20002",
    "key":"NPORT-UDP@ashyelf.com"
}
```

#### Configuration settings example

Example, set max networks to 5

```shell
agent@gtog:net_max=5
ttrue
```

Example, merge set pool limits( include "net_max" "listen_port" )

```shell
agent@gtog|{"net_max":"8","listen_port":"10004"}
ttrue
```


### Concepts

**Channel object pool**

All channels share names **`agent@net`**, **`agent@net2`**, … up to **`net_max`**. Callers work with **`netid`** ↔ **`agent@net*`**; they need not care whether the slot came from product config or runtime cache.

| Path | APIs | Config store | Lifetime |
|------|------|--------------|----------|
| Boot | **`setup[]`** / **`shut[]`** / **`query`** | Query then **`register`** (**`=cache`**) | Listed nets come up after Query; gone when dropped from the list |
| Runtime | **`register[]`** / **`unregister[]`** | **`register`** sets **`=cache`** (under `/tmp`); **`unregister`** uses **`=nocache`** and drops the cache | Gone after reboot until Query or **`register`** runs again |

- **`setup`**: store **`net_max`** / **`listen_port`** / **`port`** / **`key`** in register; start **`query`**. Query sends UDP `mac;*;q;` to **`center@nport`** (pool **`server`** or **`agent@heclient.server`**, port default **20002**). Reply `q;*;{ "<netid>": {}, ... }`. Extra **`netid`s** are **`register`**ed (channel **`service`** starts); mapped nets missing from the list are **`unregister`**ed; `{}` stops all. Query repeats on **`keepintval`** until a list arrives.
- **`register`**: same **`netid`** reuses the mapped object; a new **`netid`** takes a free slot. Incoming configure is compared to the channel cache; unchanged → **`sstart`**; different → save cache and **`sreset`** **`service`**.
- Pool **`_set`** (heclient **`adjust`** of **`agent@gtog`**) zeros every channel register **`seq`** so the next keep **`s;`** pulls a full table.

**Service phase, role, and net_state**

Each channel **`service`** uses three axes (also returned by **`list`** / **`state`**):

| Field | Meaning | Values |
|-------|---------|--------|
| **`phase`** | Service lifecycle | `init`, `dial`, `run` (includes waiting for topology), `exit` |
| **`role`** | Mesh role of this device | `none`, `master`, `branch`, `leaf` |
| **`net_state`** | Whether a usable master exists | `unknown`, `no_master`, `has_master` |
| **`seq`** | Last applied topology version | channel register; from `endpoint` / `branch` / `leaf` / `leave` |
| **`coord_seq`** | Last seq seen on UDP `k;netid;seq;` | coordinator keep |
| **`delay`** | ICMP RTT to master (ms) | omitted until ICMP delay > 0 |
| **`coord_delay`** | nport keep RTT (ms) | omitted until coord delay > 0 |
| **`fails`** | Consecutive ICMP master keep fails | live **`status`** |
| **`coord_fails`** | Consecutive nport keep fails | live **`status`** |
| **`nattype`** | Self NAT class from endpoint | `free`, `limit`, `unknown` (unix sends `1`/`2`/`0`; **`_state`** maps to words) |

**`agent@net*.status`** / **`state`** return the words above. **`agent@gtog.list`** uses unix **`state`** per live channel (same words); falls back to register numbers if unix is down.

Phases: **Init** (WireGuard iface; leftover **`.endpoint`** is unlinked) → **ServerDial** (UDP Connect → `u;` / `t;`) → **Run** (WaitMesh UDP `b`/`l` up to four times at 10 / 15 / 20 / 30 seconds, 75 second deadline; HE **`endpoint`** sets role, then keeplive + **`network@frame.online`**) → **Exit**. Peers return only after **`endpoint`** / **`branch`** / **`leaf`** / **`leave`**. Outbound **`listen_ip`**: channel **`extern`**, else **`agent@heclient.extern`**, then gateway / iface status; WAN retry via **`agent@heclient.reset`** → **`agent@gtog.reset`**.

**Keepalive (Run)**

| Target | Who | Purpose |
|--------|-----|---------|
| UDP `'k'` to **`center@nport`** | **every** online role (master / branch / leaf) | hole + receive `k;netid;seq;`; RTT is **`coord_delay`**, fails are **`coord_fails`** |
| ICMP to every hub VPN IP | master / branch / leaf | keep spoke tunnels; RTT to the local master is **`delay`**; master fail re-elects among live hubs |

If local register **`seq`** is not equal to **`coord_seq`** on a keep reply, device sends UDP **`s;netid;local_seq;`** so nport pushes a full **`endpoint`**. Timers clamp as before (`keepintval` ≥ 5, …). Pool **`_set`** zeros every channel **`seq`** so the next keep will **`s;`**.

**Topology APIs** (from coordinator over heclient)

- **`endpoint`**: HE writes **`%s.endpoint`** (topology only) and unix **`reload`**. Service only reads the file, copies live ICMP **`fails`/`delay`** onto matching macs, then **`gtog_wg_set`**. Prefer **`relay_port`** for the WG endpoint (with **`relay_ip`**, or `agent@portc` / `agent@heclient` `server` when `relay_ip` is omitted). Hub = FREE or **`relay_port`**. Each device elects a local **master** among reachable hubs (**`pref`**, then smaller **`macid`**; ICMP book is in memory). A leaf puts the mesh CIDR and other spokes' **`extend`** on that master. A hub keeps spoke prefixes on the direct leaf peers.
- **`branch`**: HE merges one hub (`FREE` or **`relay_port`**) into **`.endpoint`** + optional **`seq`**, then **`reload`**. Requires an existing file
- **`leaf`**: HE merges one spoke into **`.endpoint`** + optional **`seq`**, then **`reload`**
- **`leave`**: HE deletes **`macid`** from **`.endpoint`** + optional **`seq`**, then **`reload`**. Full **`endpoint`** remains the lag / `s;` fallback. Service start unlinks leftover **`.endpoint`** so WaitMesh always runs after Connect; **`unregister`** also deletes the file
- Dual call style: **`agent@gtog.<api>[ netid, … ]`** or **`agent@net*.<api>[ … ]`**.


### API Reference

#### Management APIs

+ `setup[]` **bring up the gtog pool or one channel service**
    - On **`agent@gtog`**: apply **`net_max`** / **`listen_port`** / **`port`** / **`key`**, start **`query`**. Query lists nets of this mac from **`center@nport`**, then **`register`** each **`netid`** so the channel **`service`** starts
    - On **`agent@net*`**: start that channel’s **`service`** unless channel **`status`** is **`disable`**
    - failed return tfalse
    - succeed return ttrue
    - Scheduled by FPK init (**`manage`**: **`agent@gtog.setup`**)

    Example, setup the gtog infrastructure
    ```shell
    agent@gtog.setup
    ttrue
    ```

+ `shut[]` **tear down the gtog pool or one channel**
    - On **`agent@gtog`**: stop **`query`**; clear object→`netid` map; **`shut`** each present channel; unregister from **`network@frame`**
    - On **`agent@net*`**: offline, stop **`service`**, bring the WireGuard interface down
    - failed return tfalse
    - succeed return ttrue

    Example, shutdown all gtog networks
    ```shell
    agent@gtog.shut
    ttrue
    ```

+ `query[]` **ask the coordinator which netids this device belongs to**
    - Only valid on **`agent@gtog`**
    - UDP `mac;*;q;` until a list arrives; extra **`netid`s** are **`register`**ed; mapped nets missing from the list are **`unregister`**ed
    - Long-running **`query`** service started by **`setup`**
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    agent@gtog.query
    ttrue
    ```


#### Query APIs

+ `list[]` **list mapped channels (on agent@gtog) or the live peer map (on agent@net*)**
    - On **`agent@gtog`**: slots that currently have an object→`netid` entry
    - On **`agent@net*`**: unix live **`rt->endpoint`** (includes ICMP **`delay`** / **`fails`**); file if the service is not in Run
    - failed return NULL
    - succeed return [ json ], channel map or endpoint list
    ```json
    {
        "network object name":                      // [ string ]: { json }, e.g. "agent@net"
        {
            "netid":"network identifier",           // [ string ]
            "netdev":"WireGuard interface name",    // [ string ]
            "listen_port":"local WireGuard listen", // [ number ]
            "role":"mesh role",                     // [ string ]: [ "none","master","branch","leaf" ], words when unix state is used
            "net_state":"master presence",          // [ string ]: [ "unknown","no_master","has_master" ]
            "phase":"service lifecycle phase",      // [ string ]: [ "init","dial","run","exit" ]
            "pref":"self preference value"          // [ number ]
                                                       // unix down: role / net_state / phase fall back to register numbers
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
            "role":"branch",
            "net_state":"has_master",
            "phase":"run",
            "pref":"50"
        }
    }
    ```

#### Control APIs

+ `register[ netid, configure ]` **bind a netid to a free or existing channel and start service**
    - netid -------------------- [ string ], network identifier
    - configure ---------------- [ json ], optional, merged into channel cache config (see **`net.md`**)
    - Keys in **`configure`** overlay the existing cache; omitted keys are kept. Center **`register`** HE typically pushes **`network`**, **`keep*`**, and optional **`listen_port`**.
    - Incoming configure is compared to the channel cache; same (or **`configure`** omitted) → **`sstart`**; different → save and **`sreset`** **`service`**.
    ```json
    {
        "server":"coordinator address",             // [ string ], optional
        "port":"coordinator UDP port",              // [ number ], optional, default 20002
        "listen_port":"local WireGuard listen",     // [ number ], optional, default listen_port formula
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

+ `reset[]` **restart all mapped channel services**
    - Only valid on **`agent@gtog`** (pool); channel objects return error
    - **`sreset`** each slot that has an object→`netid` map entry (**`agent@net`**, **`agent@net2`**, …)
    - Called by **`agent@heclient.reset`** when the heclient bound **`extern`** path changes
    - failed return terror / tfalse
    - succeed return ttrue

    Example
    ```shell
    agent@gtog.reset
    ttrue
    ```

+ `endpoint[ netid, endpoint list ]` **replace the full endpoint map, or read the on-disk file**
    - netid ---------------- [ string ], network identifier
    - endpoint list -------- [ json ], omit to return **`%s.endpoint`** (HE topology; no ICMP book)
    - with a map: replace; without a map: return the file (empty object if missing)
    ```json
    {
        "seq":"coordinator topology version",       // [ number ], optional; applied to channel register when > 0
        "endpoint mac identify":                    // [ string ]: { json }
        {
            "ip":"public internet ip",              // [ ip address ], live hole
            "port":"public internet port",          // [ number ], live hole
            "relay_port":"center UDP relay",        // [ number ], optional; prefer this for WG endpoint
            "relay_ip":"center public IP",          // [ ip address ], optional reserved; omit → portc/heclient server
            "pubkey":"WireGuard public key",        // [ string ]
            "nattype":"NAT class",                  // [ number ]: [ 1, 2 ], 1=FREE, 2=LIMIT
            "pref":"master preference",             // [ number ], higher wins among FREE or relay_port peers
            "point":"VPN tunnel ip",                // [ ip address ]
            "extend":"local networks via this peer" // [ string ], optional, e.g. "192.168.1.0/24"
        }
        // "...":{ ... }  How many endpoints show how many properties
    }
    ```
    - get (no map): return the on-disk file, or `{}` if missing
    - set failed return tfalse
    - set succeed return ttrue
    - Self macid must exist in the map; HE writes **`.endpoint`** (source of truth); service **`reload`** reads it, programs WireGuard, elects **`role`** / **`net_state`**. Unix down still succeeds if the file was written

    Example, push a full endpoint list
    ```shell
    agent@gtog.endpoint[ office-vpn, {"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}} ]
    ttrue
    ```

+ `branch[ netid, branch information ]` **add or update one hub (FREE or relay_port)**
    - netid -------------------- [ string ], network identifier
    - branch information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "ip":"public internet ip",                  // [ ip address ], live hole
        "port":"public internet port",              // [ number ], live hole
        "relay_port":"center UDP relay",            // [ number ], optional; prefer for WG endpoint
        "relay_ip":"center public IP",              // [ ip address ], optional reserved; omit → portc/heclient server
        "pubkey":"WireGuard public key",            // [ string ]
        "nattype":"NAT class",                      // [ number ], 1=FREE; 2=LIMIT (hub because relay_port)
        "pref":"master preference",                 // [ number ]
        "point":"VPN tunnel ip",                    // [ ip address ]
        "extend":"local networks via this peer"     // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Requires existing **`.endpoint`**; HE merges the hub then **`reload`**; does not delete other peers

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
    - Requires existing **`.endpoint`**; HE merges the spoke then **`reload`**; a spoke without **`relay_port`** is not a hub

    Example, add a leaf peer
    ```shell
    agent@gtog.leaf[ office-vpn, {"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def...","point":"10.0.1.2","extend":"192.168.2.0/24"} ]
    ttrue
    ```

+ `leave[ netid, leave information ]` **remove one peer from the mesh**
    - netid ------------------- [ string ], network identifier
    - leave information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "seq":"coordinator topology version"        // [ number ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - HE deletes **`macid`** from **`.endpoint`** then **`reload`**; missing file or self-leave is success. Lag still uses full **`endpoint`**

    Example, drop a peer
    ```shell
    agent@gtog.leave[ office-vpn, {"macid":"aabbccddeeff","seq":"12"} ]
    ttrue
    ```

Channel **`online`** / **`offline`** (and per-channel **`state`**) are documented in **`net.md`**. Pool **`reset`** is above; WAN retry is driven by **`agent@heclient.reset`**.
