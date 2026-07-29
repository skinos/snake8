## agent@gtog — Gateway to gateway (GTOG) — mesh VPN
Manages multiple **WireGuard**-based mesh VPNs (**`agent@net`**, **`agent@net2`**, …): register networks, push endpoint lists, and add **branch** (relay-capable) or **leaf** peers. Topology and reachability follow each device’s NAT characteristics and configured preference.

### Configuration ( `agent@gtog` )
```json
{
    "net_max":"maximum number of gtog networks",           // [ number ], product default
    "port_start":"starting local UDP port for networks"    // [ number ], product default; successive nets use incrementing ports
}
```

Example, show all the configure
```shell
agent@gtog
{
    "net_max":"10",                            # support up to 10 networks
    "port_start":"10004"                       # first network uses port 10004
}
```

Example, set max networks to 5
```shell
agent@gtog:net_max=5
ttrue
```

#### Network Configuration( agent@net )
Each registered network has its own configuration object (agent@net for the first, agent@net2 for the second, etc.)

> **Full field list:** `server`, `extern`, `key`, `lport`, DNS, routing, `mtu`, etc. are documented in **`net.md`** (Network Client). The table below is a minimal summary; use **`net.md`** as the authoritative reference for per-network options.

```json
{
    "port":"network server port",                              // [ number ]
    "netid":"network identify string",                         // [ string ], unique network identifier
    "network":"network address (CIDR format)",                 // [ string ], e.g. "10.0.0.0/24"
    "keepintval":"keeplive interval to master/server",         // [ number ], the unit is second
    "keepfailed":"keeplive max failed count",                  // [ number ]
    "keeptimeout":"keeplive timeout to master/server"          // [ number ], the unit is second
}
```

Example, show network configure
```shell
agent@net
{
    "port":"10000",
    "netid":"office-vpn",
    "network":"10.0.1.0/24",
    "keepintval":"10",
    "keepfailed":"3",
    "keeptimeout":"35"
}
```

### Component API
+ `setup[]` **setup all gtog network infrastructure**
    Reads **`agent@gtog`** limits, wires each registered **`agent@net*`** into **`network@frame`**, and hooks **init** / **`network/online`** so instances can start in order.
    - succeed return ttrue
    - failed return tfalse

    Example, setup the gtog infrastructure
    ```shell
    agent@gtog.setup
    ttrue
    ```

+ `shut[]` **shutdown all gtog network clients**
    Stops each **`agent@net*`** service and removes **`network@frame`** / event hooks installed by **`setup[]`**.
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown all gtog networks
    ```shell
    agent@gtog.shut
    ttrue
    ```

+ `register[ netid, {network configure} ]` **register a new gtog network**
    register a new network, assign it to the next available network slot, save configuration, and register with the system
    - netid -------------------- [ string ], network identifier
    - {network configure} ------ json
    ```json
    // Attributes introduction of json of {network configure}
    {
        "port":"network server port",                         // [ number ]
        "netid":"network identify",                           // [ string ]
        "network":"network address",                          // [ string ], e.g. "10.0.1.0/24"
        "keepintval":"keeplive to master/server interval",    // [ number ]
        "keepfailed":"keeplive to master/server failed count",// [ number ]
        "keeptimeout":"keeplive to master/server timeout"     // [ number ]
    }
    ```
    - succeed return ttrue
    - failed return tfalse

    Example, register a new network
    ```shell
    agent@gtog.register[office-vpn,{"port":"10000","netid":"office-vpn","network":"10.0.1.0/24","keepintval":"10","keepfailed":"3","keeptimeout":"35"}]
    ttrue
    ```

+ `unregister[ netid ]` **unregister a gtog network**
    find and remove the network with the given netid, stop its service, bring down the interface, and clean up configuration
    - netid ---- [ string ], network identifier to remove
    - succeed return ttrue
    - failed return tfalse

    Example, unregister a network
    ```shell
    agent@gtog.unregister[office-vpn]
    ttrue
    ```

+ `list[]` **list all registered gtog networks and their status**
    - failed return NULL
    - succeed return json with all network information
    ```json
    // Attributes introduction of talk by the API return
    {
        "network object name":
        {
            "netid":"network identifier",              // [ string ]
            "netdev":"network device name",            // [ string ], WireGuard interface name
            "lport":"local listen port",               // [ number ]
            "master":"current master endpoint",        // [ string ], "ip:port" of current master
            "pref":"current master preference value"   // [ number ]
        }
        // ... more networks
    }
    ```

    Example, list all networks
    ```shell
    agent@gtog.list
    {
        "agent@net":
        {
            "netid":"office-vpn",
            "netdev":"wg0",
            "lport":"10004",
            "master":"1.2.3.4:10000",
            "pref":"100"
        },
        "agent@net2":
        {
            "netid":"home-vpn",
            "netdev":"wg1",
            "lport":"10005",
            "master":"5.6.7.8:10000",
            "pref":"50"
        }
    }
    ```

+ `endpoint[ netid, {endpoint list} ]` **update the full endpoint list for a network**
    replace the entire endpoint list for the specified network. The local device uses this to know all peers in the network, elect a master, and establish connections
    - netid ---------------- [ string ], network identifier
    - {endpoint list} ------ json
    ```json
    // {endpoint list} attributes introduction
    {
        "endpoint mac identify":
        {
            "ip":"device public ip address",              // [ ip address ]
            "port":"device public port",                  // [ number ]
            "pubkey":"device WireGuard public key",       // [ string ]
            "nattype":"device NAT type",                  // [ "1", "2" ], affects whether the node may act as relay vs leaf-only
            "pref":"master preference value",             // [ number ], higher = stronger candidate to coordinate the mesh

            "point":"endpoint VPN ip address",            // [ ip address ], assigned IP within the VPN network
            "extend":"endpoint local network"             // [ string ], local network to route through this endpoint
        }
        // ... more endpoints
    }
    ```

    Example, update endpoint list
    ```shell
    agent@gtog.endpoint[office-vpn,{"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc123...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def456...","nattype":"2","pref":"50","point":"10.0.1.2","extend":"192.168.2.0/24"}}]
    ttrue
    ```

+ `branch[ netid, {branch information} ]` **add a branch (relay) node to the network**
    Registers a relay-capable peer this device should use when **`nattype`** allows branch topology.
    - netid -------------------- [ string ], network identifier
    - {branch information} ----- json
    ```json
    // {branch information} attributes introduction
    {
        "macid":"device mac identify",                    // [ string ]
        "ip":"device public ip address",                  // [ ip address ]
        "port":"device public port",                      // [ number ]
        "pubkey":"device WireGuard public key",           // [ string ]
        "nattype":"device NAT type",                      // [ "1", "2" ]
        "pref":"master preference value",                 // [ number ]
        "point":"endpoint VPN ip address",                // [ ip address ]
        "extend":"endpoint local network"                 // [ string ]
    }
    ```

    Example, add a branch node
    ```shell
    agent@gtog.branch[office-vpn,{"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc123...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"}]
    ttrue
    ```

+ `leaf[ netid, {leaf information} ]` **add a leaf node to the network**
    Registers a peer that participates as **leaf** (no relay role for that endpoint).
    - netid ------------------ [ string ], network identifier
    - {leaf information} ----- json
    ```json
    // {leaf information} attributes introduction
    {
        "macid":"device mac identify",                    // [ string ]
        "ip":"device public ip address",                  // [ ip address ]
        "port":"device public port",                      // [ number ]
        "pubkey":"device WireGuard public key",           // [ string ]
        "point":"endpoint VPN ip address",                // [ ip address ]
        "extend":"endpoint local network"                 // [ string ]
    }
    ```

    Example, add a leaf node
    ```shell
    agent@gtog.leaf[office-vpn,{"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def456...","point":"10.0.1.2","extend":"192.168.2.0/24"}]
    ttrue
    ```

+ `state[]` **get the VPN instance runtime state** (alias: `status[]`)
    Only meaningful on per-network objects (`agent@net`, `agent@net2`, …).
    When called on the main **`agent@gtog`** object, returns NULL.
    - failed return NULL
    - succeed return JSON describing the instance state:
    ```json
    {
        "status":"current status",      // "up", "down", "uping", "failed", "block"
        "ip":"tunnel local IP",
        "mask":"tunnel mask",
        "dstip":"tunnel peer IP",
        "netdev":"WireGuard interface name",
        "delay":"master keepalive delay (ms)",
        "livetime":"human-readable uptime",
        "rx_bytes":"...", "rx_packets":"...",
        "tx_bytes":"...", "tx_packets":"...",
        "server":"master server address",
        "pref":"preference value",
        "mode":"current mode",
        "mip":"master IP",
        "mmacid":"master MAC ID",
        "mpref":"master preference",
        "tid":"configuration transaction ID"
    }
    ```

+ `online[]` **internal**
    Invoked when **`network/online`** fires so the instance can refresh reachability context (e.g. gateway) before continuing bring-up.

+ `offline[]` **internal**
    Invoked when the VPN link goes down. Cleans up DNS resolver entries, removes iptables MASQUERADE rules, and clears TCP MSS clamping for the interface.

### Lifecycle API
+ `setup[]` / `shut[]` — **when implemented** for **`agent@gtog`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_agent_gtog(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@gtog", "status") == NULL)
        return -1;
    return ssets_string("agent@gtog", "enable", "status") ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* e.g. scall("agent@gtog", "list", NULL); talk_free if JSON */
```
