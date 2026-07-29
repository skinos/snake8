## agent@local — Local Agent -- LAN Management Service

### Overview

Provide LAN management services: a JSON TCP command interface and a UDP broadcast discovery/command interface for device discovery, querying, and configuration on the local network.

- **JSON TCP**: accept management-tool connections, run HE commands, return JSON results
- **Broadcast UDP**: device discovery and LAN command / address assignment (bound to the LAN interface)
- Optional **manager** ACL via iptables (IP or MAC allow-list) when configured in **`setup`**

### Configuration reference ( `agent@local` )

```json
// Attributes introduction
{
    // JSON TCP service
    "json":"JSON TCP command service",                          // [ "disable", "enable" ]
    "json_port":"JSON TCP listen port",                         // [ number ], default 22220
    "json_command":"JSON TCP command permission",               // [ "query", "all" ]
                                                                   // omitted or empty: same as "query"
                                                                   // "query": GET and CALL whose method contains "stat", "list", or "info"
                                                                   //         (also applies to DBS get/call); SET/OR/DBS write blocked
                                                                   // "all": allow set/or/call (and DBS equivalents)
    "json_manager":"JSON TCP access control",                   // [ string or json ]
                                                                   // string: IP or MAC list separated by ";"
                                                                   //   e.g. "192.168.8.100;00:11:22:33:44:55"
                                                                   // json: { "host1":"192.168.8.100", "host2":"00:11:22:33:44:55" }
                                                                   // unset: no iptables ACL (allow all)
                                                                   // set: only listed IP/MAC accepted; others dropped (installed by setup)

    // Broadcast UDP service
    "broadcast":"broadcast UDP discovery service",              // [ "disable", "enable" ]
    "broadcast_port":"broadcast UDP listen port",               // [ number ], default 22222
    "broadcast_group":"broadcast group name for discovery",     // [ string ], default "default"
    "broadcast_command":"broadcast command permission",         // [ "query", "all", "disable" ]
                                                                   // omitted or empty: same as "query"
                                                                   // "query": GET and CALL with method containing "stat"/"list"/"info";
                                                                   //         for agent@local, factory, gpio, machine / land@machine: any CALL allowed
                                                                   // "all": allow all HE operations on the command path
                                                                   // "disable" (or any other non-empty value): no command execution; discovery only
                                                                   // note: MAC-targeted address assign is always accepted (not gated by this field)
    "broadcast_manager":"broadcast access control"              // [ string or json ], same format as json_manager
}
```

#### Configuration example

Example, show all the configure
```shell
agent@local
{
    "broadcast":"enable",
    "broadcast_port":"22222",
    "broadcast_group":"default",
    "broadcast_command":"query",
    "json":"disable",
    "json_port":"22220"
}
```

#### Configuration settings example

Example, enable JSON TCP with access control
```shell
agent@local={"json":"enable","json_port":"22220","json_command":"all","json_manager":"192.168.8.100;192.168.8.101"}
ttrue
```

Example, enable broadcast with full command permission
```shell
agent@local:broadcast=enable
ttrue
agent@local:broadcast_command=all
ttrue
```

Example, disable JSON TCP
```shell
agent@local:json=disable
ttrue
```

Example, set broadcast group name
```shell
agent@local:broadcast_group=mygroup
ttrue
```

### Concepts

**JSON TCP service**
* Listens on **`json_port`** (default **22220**) on **all interfaces** (`0.0.0.0`).
* Waits until the LAN netdev is up, then accepts TCP connections and runs batched HE commands from the request JSON.
* Permission follows **`json_command`** (see configuration). Response values are returned under the same request keys.

**Broadcast UDP service**
* Listens on **`broadcast_port`** (default **22222**) with **`SO_BINDTODEVICE`** on the LAN netdev.
* Supports device discovery, command execution (subject to **`broadcast_command`**), and MAC-targeted address assignment.
* Address assign always runs when a valid MAC-targeted assign packet is received; it may also start the JSON service if not already running.
* **`json_start`** / **`broadcast_start`** only start the process; they do **not** install manager iptables rules (rules are applied by **`setup`** when manager is set).

**Manager ACL**
* When **`json_manager`** / **`broadcast_manager`** is set, **`setup`** installs an iptables allow-list for the corresponding port; unset means no ACL.

### API Reference

#### Management API

+ `setup[]` **setup the local agent services**
    Read configure and start enabled services:
    1. If **`json`** is **`enable`**: install JSON iptables ACL when manager is set, then start JSON TCP service
    2. If **`broadcast`** is **`enable`**: install broadcast iptables ACL when manager is set, then start broadcast UDP service
    - succeed return ttrue
    - failed return tfalse

    Example
    ```shell
    agent@local.setup
    ttrue
    ```

+ `shut[]` **shutdown all local agent services**
    Remove json/broadcast iptables rules and stop both service processes
    - succeed return ttrue
    - failed return tfalse

    Example
    ```shell
    agent@local.shut
    ttrue
    ```

+ `json_start[]` **start the JSON TCP service only** (no iptables install)
    - succeed return ttrue

    Example
    ```shell
    agent@local.json_start
    ttrue
    ```

+ `json_stop[]` **stop the JSON TCP service only**
    - succeed return ttrue

    Example
    ```shell
    agent@local.json_stop
    ttrue
    ```

+ `broadcast_start[]` **start the broadcast UDP service only** (no iptables install)
    - succeed return ttrue

    Example
    ```shell
    agent@local.broadcast_start
    ttrue
    ```

+ `broadcast_stop[]` **stop the broadcast UDP service only**
    - succeed return ttrue

    Example
    ```shell
    agent@local.broadcast_stop
    ttrue
    ```
