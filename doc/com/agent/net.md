## agent@net — One GTOG WireGuard mesh channel

### Overview

Each **`agent@net`**, **`agent@net2`**, … is one WireGuard mesh channel implemented by the **`gtog`** executable (same binary as **`agent@gtog`**, different object name). There is no separate **`net`** component directory: these objects are slots in the **`agent@gtog`** pool.

- Hold per-channel configuration, WireGuard interface, peers, and the long-running **`service`**
- Accept topology on this slot via **`endpoint`** / **`branch`** / **`leaf`** (also callable as **`agent@gtog.<api>[ netid, … ]`**)
- Expose runtime **`state`** / **`status`** and the local endpoint file via **`list`**
    > Pool limits, object↔`netid` mapping, and **`register`** / **`unregister`** are documented in **`gtog.md`**


### Configuration reference ( agent@net )

**`agent@net`** is the first slot; **`agent@net2`** is the second; up to **`agent@gtog:net_max`**.  
**`register`** writes under **`=cache`** (runtime). **`setup`** uses the normal project config path when no cache file is present.

```json
// Attributes introduction 
{
    "status":"channel service switch",                          // [ "disable","enable" ], checked by agent@net.setup
                                                                   // "enable": start service
                                                                   // "disable" or unset: setup returns without starting

    "server":"coordinator address for UDP register/keep",       // [ string ], IPv4 text for inet_pton (no DNS here)
                                                                   // empty: fall back to agent@heclient.server
    "port":"coordinator UDP port",                              // [ number ], default 20002
                                                                   // destination port of central nport/heport hole
    "listen_port":"local WireGuard and raw UDP listen port",    // [ number ]
                                                                   // omit → agent@gtog port_start formula (agent@net → port_start, agent@netN → port_start+N-1)
    "key":"shared key with the coordinator",                    // [ string ], default "NPORT-UDP@ashyelf.com"
    "extern":"outbound interface for raw UDP listen_ip",        // [ string ]: [ "disable","default","ifname@wan",... ]
                                                                   // omit → agent@heclient.extern; empty/disable → default
                                                                   // "default": network@frame.gateway; "ifname@…": iface status
                                                                   // no joints (WAN retry via heclient.reset)

    "netid":"network identify string",                          // [ string ], unique network id
    "network":"VPN address CIDR",                               // [ string ], e.g. "10.0.1.0/24"
                                                                   // may also arrive from coordinator UDP 'u' reply

    "keepintval":"keeplive interval in seconds",                // [ number ], default 15 (≥5; may be overridden by 'u' reply
    "keepfailed":"coordinator/ICMP keep max fail count",        // [ number ], default 4 (≥1)
    "keeptimeout":"keeplive timeout in seconds",                // [ number ], default 15; clamped to < keepintval

    "mtu":"WireGuard interface MTU",                            // [ number ], optional

    "custom_dns":"use custom DNS on online",                    // [ "disable","enable" ]
    "dns":"primary DNS server",                                 // [ ip address ], optional, only when custom_dns is enable
    "dns2":"secondary DNS server",                              // [ ip address ], optional
    "domain":"DNS search domain",                               // [ string ], optional

    "metric":"route metric",                                    // [ number ], optional
    "defaultroute":"install default route via mesh",            // [ "disable","enable" ]
    "route_table":                                              // [ string ]: { json }, used when defaultroute is not enable
    {
        "route name":                                           // [ string ]: { json }
        {
            "target":"destination host or network",             // [ string ]
            "mask":"subnet mask"                                // [ string ], default "255.255.255.255"
        }
        // "...":{ ... }  How many routes show how many properties
    }
}
```

#### Configuration example

Example, show a typical channel configure

```shell
agent@net
{
    "status":"enable",                          # allow setup to start service
    "server":"203.0.113.10",                    # coordinator IPv4
    "port":"20002",                             # coordinator UDP port
    "listen_port":"10004",                      # local WireGuard listen port
    "netid":"office-vpn",                       # network identifier
    "network":"10.0.1.0/24",                    # VPN CIDR
    "keepintval":"10",                          # keep every 10 seconds
    "keepfailed":"3",                           # exit after 3 keep failures
    "keeptimeout":"8",                          # keep timeout (< keepintval)
    "mtu":"1420"                                # WireGuard MTU
}
```

#### Configuration settings example

Example, set the coordinator UDP port

```shell
agent@net:port=20002
ttrue
```

Example, set the local listen port

```shell
agent@net:listen_port=10004
ttrue
```

Example, merge set ports and network id( include "port" "listen_port" "netid" )

```shell
agent@net|{"port":"20002","listen_port":"10004","netid":"office-vpn"}
ttrue
```

Example, merge set custom DNS and default route( include "custom_dns" "dns" "dns2" "defaultroute" )

```shell
agent@net|{"custom_dns":"enable","dns":"8.8.8.8","dns2":"8.8.4.4","defaultroute":"enable"}
ttrue
```


### Concepts

**Port fields**

| Name | Scope | Meaning |
|------|-------|---------|
| **`port`** | channel config / reg | Coordinator (server) UDP port; default **20002** |
| **`listen_port`** | channel config / reg | Local WireGuard listen and raw UDP source port; default **`port_start`** formula |
| **`port`** in endpoint / branch / leaf JSON | peer map | That peer’s public internet UDP port (not the channel config **`port`**) |
| **`listen_port`** in center endpoint / HE | heport network DB | Same meaning as channel **`listen_port`**; pushed via **`register[ netid, {listen_port} ]`** when set |

**Lifecycle axes** (register on the channel; see **`gtog.md`** for full keep and topology rules):

| Field | Values |
|-------|--------|
| **`phase`** | `0=init`, `1=server_dial`, `2=run` (includes waiting for topology; legacy alias `wait_mesh`), `3=exit` |
| **`role`** | `0=none`, `1=master`, `2=branch`, `3=leaf` |
| **`net_state`** | `0=unknown`, `1=no_master`, `2=has_master` |

**NAT / peer class in endpoint JSON**

| **`nattype`** | Meaning |
|---------------|---------|
| `1` (FREE) | Relay-capable; may become master/branch |
| `2` or omitted on leaf push | LIMIT leaf; connects to FREE peers; extend routes via master |

LIMIT peers are not wired to each other; traffic between leaves goes through the master (or other programmed FREE peers).


### API Reference

#### Management APIs

+ `setup[]` **start this channel’s service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - Persistent slots are usually scheduled by **`agent@gtog.setup`**; runtime slots by **`agent@gtog.register`**

    Example, setup the first network
    ```shell
    agent@net.setup
    ttrue
    ```

+ `shut[]` **stop this channel**
    - Offline, stop **`service`**, bring the WireGuard interface down
    - failed return tfalse
    - succeed return ttrue

    Example, shutdown the first network
    ```shell
    agent@net.shut
    ttrue
    ```

+ `online[ network information ]` **apply online DNS, routes, MASQUERADE, and MTU**
    - Invoked when **`network@frame.online`** publishes this channel
    - network information ----- [ json ]
    ```json
    {
        "ifname":"network object name",             // [ string ], e.g. "agent@net"
        "netdev":"WireGuard device name",           // [ string ]
        "gw":"gateway VPN ip",                      // [ ip address ], optional
        "dns":"primary DNS",                        // [ ip address ], optional
        "dns2":"secondary DNS",                     // [ ip address ], optional
        "domain":"DNS search domain",               // [ string ], optional
        "masq":"enable NAT masquerade"              // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue

+ `offline[]` **clear online side effects**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `state[]` **get this channel runtime status and details**
    - Alias: **`status[]`**
    - When **`phase >= run`** (legacy name **`wait_mesh`**) and the service is running, prefer live unix query; otherwise register / **`.ol`** snapshot
    - failed return NULL
    - succeed return [ json ], detailed status
    ```json
    {
        "status":"link status",                     // [ string ]: [ "uping", "down", "up", "failed", "block" ]
                                                       // "uping": service up, not fully online (or master_delay < 0 while service still running)
                                                       // "down": service stopped / interface down
                                                       // "up": interface up and master_delay > 0
                                                       // "failed": master_delay < 0 and service not running
                                                       // "block": master_delay == 0
        "delay":"keeplive RTT in milliseconds",     // [ number ], when status is up
        "ip":"local VPN ip",                        // [ ip address ], when interface is up
        "mask":"VPN netmask",                       // [ string ], when interface is up
        "dstip":"point-to-point peer ip",           // [ ip address ], optional
        "livetime":"human-readable uptime",         // [ string ], optional
        "rx_bytes":"received bytes",                // [ string ]
        "rx_packets":"received packets",            // [ string ]
        "tx_bytes":"sent bytes",                    // [ string ]
        "tx_packets":"sent packets",                // [ string ]
        "server":"coordinator address in use",      // [ string ], optional
        "pref":"self preference",                   // [ number ], optional
        "role":"mesh role",                         // [ number ], 0=none, 1=master, 2=branch, 3=leaf
        "net_state":"master presence",              // [ number ], 0=unknown, 1=no_master, 2=has_master
        "phase":"service lifecycle phase",          // [ number ], 0=init, 1=server_dial, 2=run, 3=exit
        "mip":"master VPN ip",                      // [ ip address ], optional
        "mmacid":"master mac identify",             // [ string ], optional
        "mpref":"master preference",                // [ number ], optional
        "tid":"policy route table id"               // [ number ], optional
    }
    ```

    Example, get status when connected as leaf
    ```shell
    agent@net.status
    {
        "status":"up",                              # keeplive ok
        "delay":"45",                               # 45 ms to master
        "ip":"10.0.1.3",
        "mask":"255.255.255.0",
        "livetime":"2 hours 15 minutes",
        "rx_bytes":"1048576",
        "rx_packets":"1024",
        "tx_bytes":"524288",
        "tx_packets":"512",
        "server":"203.0.113.10",
        "pref":"50",
        "role":"3",                                 # leaf
        "net_state":"2",                            # has_master
        "phase":"2",                                # run
        "mip":"10.0.1.1",
        "mmacid":"001122334455",
        "mpref":"100"
    }
    ```

    Example, get status when connecting
    ```shell
    agent@net.status
    {
        "status":"uping"                            # service running, not fully online
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@net.status
    {
        "status":"down"                             # service not running
    }
    ```

+ `list[]` **list endpoints stored for this channel**
    - Returns **`%s.endpoint`** JSON for this object
    - On **`agent@gtog`**, **`list[]`** lists mapped channels instead (see **`gtog.md`**)
    - failed return NULL
    - succeed return [ json ], endpoint map keyed by macid
    ```json
    {
        "endpoint mac identify":                    // [ string ]: { json }
        {
            "point":"VPN tunnel ip",                // [ ip address ]
            "extend":"local networks via peer",     // [ string ], optional
            "pubkey":"WireGuard public key",        // [ string ]
            "nattype":"NAT class",                  // [ number ]: [ 1, 2 ], optional on some leaf entries
            "pref":"master preference",             // [ number ], optional
            "ip":"public internet ip",              // [ ip address ], optional
            "port":"public internet port"           // [ number ], optional
        }
        // "...":{ ... }  How many endpoints show how many properties
    }
    ```

    Example, list all endpoints
    ```shell
    agent@net.list
    {
        "001122334455":
        {
            "point":"10.0.1.1",
            "extend":"192.168.1.0/24",
            "pubkey":"abc123def456...",
            "nattype":"1",
            "pref":"100",
            "ip":"1.2.3.4",
            "port":"10004"
        },
        "aabbccddeeff":
        {
            "point":"10.0.1.2",
            "extend":"192.168.2.0/24",
            "pubkey":"ghi789jkl012...",
            "nattype":"2",
            "ip":"5.6.7.8",
            "port":"10004"
        }
    }
    ```


#### Control APIs

+ `endpoint[ endpoint list ]` **replace the full endpoint map for this channel**
    - endpoint list -------- [ json ], map keyed by device macid
    ```json
    {
        "endpoint mac identify":                    // [ string ]: { json }
        {
            "ip":"public internet ip",              // [ ip address ]
            "port":"public internet port",          // [ number ]
            "pubkey":"WireGuard public key",        // [ string ]
            "nattype":"NAT class",                  // [ number ]: [ 1, 2 ], 1=FREE, 2=LIMIT
            "pref":"master preference",             // [ number ]
            "point":"VPN tunnel ip",                // [ ip address ]
            "extend":"local networks via this peer" // [ string ], optional
        }
        // "...":{ ... }  How many endpoints show how many properties
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Self macid must be present; removes WireGuard peers absent from the map; sets **`role`** / **`net_state`** / **`master_*`**; unix **`reload`**

    Example, update endpoint list
    ```shell
    agent@net.endpoint[ {"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}} ]
    ttrue
    ```

+ `branch[ branch information ]` **add or update one FREE peer on this channel**
    - branch information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "ip":"public internet ip",                  // [ ip address ]
        "port":"public internet port",              // [ number ]
        "pubkey":"WireGuard public key",            // [ string ]
        "nattype":"NAT class",                      // [ number ], usually 1
        "pref":"master preference",                 // [ number ]
        "point":"VPN tunnel ip",                    // [ ip address ]
        "extend":"local networks via this peer"     // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Requires **`.endpoint`** and **`role != none`**; may move master when **`pref`** is higher; does not remove other peers

    Example, add a branch peer
    ```shell
    agent@net.branch[ {"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"} ]
    ttrue
    ```

+ `leaf[ leaf information ]` **add or update one LIMIT peer on this channel**
    - leaf information ------- [ json ]
    ```json
    {
        "macid":"device mac identify",              // [ string ]
        "ip":"public internet ip",                  // [ ip address ], optional
        "port":"public internet port",              // [ number ], optional
        "pubkey":"WireGuard public key",            // [ string ]
        "point":"VPN tunnel ip",                    // [ ip address ]
        "extend":"local networks via this peer"     // [ string ], optional
    }
    ```
    - failed return tfalse
    - succeed return ttrue
    - Requires **`.endpoint`** and **`role != none`**; does not re-elect master

    Example, add a leaf peer
    ```shell
    agent@net.leaf[ {"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def...","point":"10.0.1.2","extend":"192.168.2.0/24"} ]
    ttrue
    ```
