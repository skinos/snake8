## center@pport — Device port proxy service

### Overview

Listen for gateway **portc** reverse-proxy connections, and map public TCP/UDP ports onto a gateway’s local TCP, UDP, or serial (`dev`) target.

- Gateways keep a pond of standby TCP links to this service (default port **20005**)
- Standby keeplive: client pings with `k`, server echoes `k`; idle timeout is `nomate_timeout`
- Client ping interval is `(nomate_timeout-1)/3`; heport pushes to **`agent@portc`**: `mode`, `active_pond`, `idle_pond`, `nomate_timeout`, `connect_timeout=(mating_timeout-1)`, `mate_timeout`
- Those values are published by `center@pport`: `status` in `_setup`, `mode`/`active_pond`/`idle_pond`/`nomate_timeout`/`mating_timeout`/`mate_timeout` in `_service` (defaults apply when unset); `heport_pport_config` reads them via `reg_sintv` / `reg_sstring`
- **`mode`**: `pond` (default, one proxy TCP per session) or `mux` (future multiplex; not implemented — service refuses to start). Hard cutover later; center does not run both data planes.
- **UDP map framing** on the proxy TCP (after mate): each datagram is `[u16be length][payload]` (`length` 0..65535). TCP/serial maps stay raw byte streams. Requires matching **`agent@portc`** version.
- Map rules can be persistent (`timeout` 0) or idle-expired; optional source-IP lock
- Persistent maps reload from heport user `tcpmap` / `udpmap` files at service start
- Pair with device-side **`agent@portc`** for the client pond



### Configuration reference ( center@pport )

```json
// Attributes introduction 
{
    "status":"enable the port proxy service",                         // [ "disable", "enable" ], default be "enable" when unset

    "port":"TCP listen port for gateway portc connections",           // [ number ], default be 20005
    "dynamic_port":"start of dynamic TCP port pool",                  // [ number ], default be 20006, used by dynamic_port[]
    "static_port":"base of mapped public TCP/UDP ports",              // [ number ], default be 25000, map index = port - static_port
    "mode":"proxy transport mode",                                    // [ "pond", "mux" ], default be "pond", pushed to agent@portc
                                                                          // "pond": current one-TCP-per-session; "mux": future, not implemented (service exits)
    "active_pond":"active standby pond size on gateway (maps present)", // [ number ], default be 6, pushed to agent@portc
    "idle_pond":"idle standby pond size on gateway (no maps)",          // [ number ], default be 1, pushed to agent@portc

    "unreg_timeout":"timeout before register completes",              // [ number ], default be 10, the unit is second
    "nomate_timeout":"idle timeout for standby (unmated) links",      // [ number ], default be 46, minimum 10, the unit is second
                                                                          // also pushed to agent@portc as nomate_timeout via heport
                                                                          // ping interval = (nomate_timeout-1)/3 on both ends
    "mating_timeout":"timeout waiting for mate ACK from gateway",     // [ number ], default be 15, the unit is second
                                                                          // heport pushes connect_timeout = mating_timeout - 1 to agent@portc
                                                                          // (portc uses it only for register connect / TCP hand connect)
    "mate_timeout":"idle timeout for mated sessions"              // [ number ], default be 180, the unit is second
                                                                          // public client idle on map ports; pushed to agent@portc as mate_timeout
}
```

#### Configuration example

Example, show all the configure

```shell
center@pport
{
    "status":"enable",                    # port proxy enabled
    "port":"20005",                       # gateway portc connect port
    "dynamic_port":"20006",               # dynamic port pool start
    "static_port":"25000",                # mapped public port base
    "mode":"pond",                        # pond now; mux reserved
    "active_pond":"6",                    # gateway active standby pool
    "idle_pond":"1",                      # gateway idle standby pool
    "unreg_timeout":"10",
    "nomate_timeout":"46",                # standby idle timeout (client interval=15)
    "mating_timeout":"15",
    "mate_timeout":"180"
}
```

#### Configuration settings example

Example, disable the port proxy

```shell
center@pport:status=disable
ttrue
```

Example, change the gateway listen port

```shell
center@pport:port=20005
ttrue
```

Example, merge set timeouts( include "nomate_timeout" "mating_timeout" "mate_timeout" )

```shell
center@pport|{"nomate_timeout":"46","mating_timeout":"15","mate_timeout":"180"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the port proxy service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - When `status` is `disable`, returns ttrue without starting the service
    - Lifecycle method; also used after configuration changes


+ `shut[]` **stop the port proxy service**
    - failed return tfalse
    - succeed return ttrue



#### Query APIs

+ `list[]` **list registered gateways and standby (unmated) proxy counts**
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "device mac identify": "standby proxy count"  // [ string ]: [ number ], 12-hex macid → pond size
        // "...":0  How many online gateways show how many properties
    }
    ```

    Example, list all devices with standby links
    ```shell
    center@pport.list
    {
        "00037f122340":"3"
    }
    ```

+ `tcp_list[]` **list all active TCP map rules**
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "tcp port":                         // [ string ]: { json }, public map port as key
        {
            "fd":"listen file descriptor",       // [ number ]
            "macid":"gateway mac identify",      // [ string ]
            "hand_ip":"local target IP",         // [ string ]
            "hand_port":"local target port or uart object", // [ string ]
            "hand_proto":"local protocol",       // [ "tcp", "udp", "dev" ]
            "total_read":"bytes from public clients",   // [ number ]
            "total_write":"bytes to public clients",    // [ number ]
            "total_accept":"accepted sessions",  // [ number ]
            "timeout":"idle unmap timeout",      // [ number ], 0 means never unmap by idle
            "lock":"source IP lock",             // [ number ], optional, present when lock > 0
            "last_time":"last activity uptime"   // [ number ], the unit is second
        }
        // "...":{}  How many TCP maps show how many properties
    }
    ```

    Example, list TCP maps
    ```shell
    center@pport.tcp_list
    {
        "25000":
        {
            "fd":"12",
            "macid":"00037f122340",
            "hand_ip":"127.0.0.1",
            "hand_port":"80",
            "hand_proto":"tcp",
            "total_read":"0",
            "total_write":"0",
            "total_accept":"0",
            "timeout":"0",
            "last_time":"12345"
        }
    }
    ```

+ `udp_list[]` **list all active UDP map rules**
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "udp port":                         // [ string ]: { json }, public map port as key
        {
            "fd":"listen file descriptor",       // [ number ]
            "macid":"gateway mac identify",      // [ string ]
            "hand_ip":"local target IP",         // [ string ]
            "hand_port":"local target port or uart object", // [ string ]
            "hand_proto":"local protocol",       // [ "tcp", "udp", "dev" ]
            "total_read":"bytes from public clients",   // [ number ]
            "total_write":"bytes to public clients",    // [ number ]
            "total_accept":"accepted sessions",  // [ number ]
            "timeout":"idle unmap timeout",      // [ number ], 0 means never unmap by idle
            "lock":"source IP lock",             // [ number ], optional, present when lock > 0
            "last_time":"last activity uptime"   // [ number ], the unit is second
        }
        // "...":{}  How many UDP maps show how many properties
    }
    ```

    Example, list UDP maps
    ```shell
    center@pport.udp_list
    {
        "25001":
        {
            "fd":"13",
            "macid":"00037f122340",
            "hand_ip":"192.168.8.1",
            "hand_port":"5000",
            "hand_proto":"udp",
            "total_read":"0",
            "total_write":"0",
            "total_accept":"0",
            "timeout":"0",
            "last_time":"12345"
        }
    }
    ```

+ `dynamic_port[]` **allocate next dynamic TCP port from the dynamic pool**
    - failed return NULL
    - succeed return [ number ], next port in `[dynamic_port, static_port)`
    - Counter wraps back to `dynamic_port` when it reaches `static_port`

    Example, get a dynamic port
    ```shell
    center@pport.dynamic_port
    20006
    ```

+ `device_dump[ macid ]` **dump standby proxies and waiting clients for one gateway**
    - macid -------------- [ string ], 12-hex mac identify
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "proxy":                           // [ json ], standby (unmated) proxy links
        {
            "ip:port":                     // [ string ]: { json }, peer address
            {
                "fd":"socket fd",               // [ number ]
                "last_time":"last activity"     // [ number ]
            }
            // "...":{}  How many standby proxies show how many properties
        },
        "client":                          // [ json ], public clients waiting for a proxy
        {
            "ip:port":                     // [ string ]: { json }
            {
                "fd":"socket fd",               // [ number ]
                "last_time":"last activity",    // [ number ]
                "create_time":"create uptime",  // [ number ]
                "total_read":"bytes read"       // [ number ]
            }
            // "...":{}  How many waiting clients show how many properties
        }
    }
    ```

    Example, dump one device
    ```shell
    center@pport.device_dump[ 00037f122340 ]
    {
        "proxy":
        {
            "1.2.3.4:54321":
            {
                "fd":"20",
                "last_time":"12340"
            }
        },
        "client":
        {
        }
    }
    ```

+ `tcp_dump[ port ]` **dump one TCP map and its active sessions**
    - port --------------- [ number ], public map port
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "fd":"listen file descriptor",            // [ number ]
        "port":"public map port",                 // [ number ]
        "macid":"gateway mac identify",           // [ string ]
        "hand_ip":"local target IP",              // [ string ]
        "hand_port":"local target",               // [ string ]
        "hand_proto":"local protocol",            // [ "tcp", "udp", "dev" ]
        "total_read":"bytes from clients",        // [ number ]
        "total_write":"bytes to clients",         // [ number ]
        "total_accept":"accepted sessions",       // [ number ]
        "timeout":"idle unmap timeout",           // [ number ]
        "client_lock":"source IP lock flag",      // [ number ]
        "client_addr":"locked client IP",         // [ string ], optional
        "last_time":"last activity uptime",       // [ number ]
        "ip:port":                                // [ string ]: { json }, active client session
        {
            "fd":"client socket",                      // [ number ]
            "last_time":"last activity",               // [ number ]
            "create_time":"create uptime",             // [ number ]
            "total_read":"bytes read",                 // [ number ]
            "total_write":"bytes written",             // [ number ]
            "proxy_addr":"mated gateway peer",         // [ string ], optional
            "proxy_fd":"proxy socket",                 // [ number ], optional
            "proxy_last_time":"proxy last activity"    // [ number ], optional
        }
        // "...":{}  How many sessions show how many properties
    }
    ```

    Example, dump TCP map 25000
    ```shell
    center@pport.tcp_dump[ 25000 ]
    {
        "fd":"12",
        "port":"25000",
        "macid":"00037f122340",
        "hand_ip":"127.0.0.1",
        "hand_port":"80",
        "hand_proto":"tcp",
        "total_read":"100",
        "total_write":"200",
        "total_accept":"1",
        "timeout":"0",
        "client_lock":"0",
        "last_time":"12345"
    }
    ```

+ `udp_dump[ port ]` **dump one UDP map and its active sessions**
    - port --------------- [ number ], public map port
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "fd":"listen file descriptor",            // [ number ]
        "port":"public map port",                 // [ number ]
        "macid":"gateway mac identify",           // [ string ]
        "hand_ip":"local target IP",              // [ string ]
        "hand_port":"local target",               // [ string ]
        "hand_proto":"local protocol",            // [ "tcp", "udp", "dev" ]
        "total_read":"bytes from clients",        // [ number ]
        "total_write":"bytes to clients",         // [ number ]
        "total_accept":"accepted sessions",       // [ number ]
        "timeout":"idle unmap timeout",           // [ number ]
        "client_lock":"source IP lock flag",      // [ number ]
        "client_addr":"locked client IP",         // [ string ], optional
        "last_time":"last activity uptime",       // [ number ]
        "ip:port":                                // [ string ]: { json }, active client session
        {
            "fd":"client socket",                      // [ number ]
            "last_time":"last activity",               // [ number ]
            "create_time":"create uptime",             // [ number ]
            "total_read":"bytes read",                 // [ number ]
            "total_write":"bytes written",             // [ number ]
            "proxy_addr":"mated gateway peer",         // [ string ], optional
            "proxy_fd":"proxy socket",                 // [ number ], optional
            "proxy_last_time":"proxy last activity"    // [ number ], optional
        }
        // "...":{}  How many sessions show how many properties
    }
    ```

    Example, dump UDP map 25001
    ```shell
    center@pport.udp_dump[ 25001 ]
    {
        "fd":"13",
        "port":"25001",
        "macid":"00037f122340",
        "hand_ip":"192.168.8.1",
        "hand_port":"5000",
        "hand_proto":"udp",
        "total_read":"0",
        "total_write":"0",
        "total_accept":"0",
        "timeout":"0",
        "client_lock":"0",
        "last_time":"12345"
    }
    ```



#### Control APIs

+ `tcp_map[ port, macid, hand_ip, hand_port, hand_proto, timeout, lock ]` **add a TCP public map to a gateway local target**
    - port --------------- [ number ], optional, public port; 0 or empty allocates the next free port from `static_port`
    - macid -------------- [ string ], 12-hex gateway mac identify
    - hand_ip ------------ [ string ], local IP on the gateway (e.g. `127.0.0.1`)
    - hand_port ---------- [ string ], local TCP/UDP port, or uart object id when `hand_proto` is `dev` (e.g. `uart@tty`)
    - hand_proto --------- [ "tcp", "udp", "dev" ], optional, default be `tcp`
    - timeout ------------ [ number ], optional, idle seconds then unmap; 0 or empty means persistent; when > 0 the gateway must already be online
    - lock --------------- [ number ], optional, > 0 locks the first public client source IP
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "port":"public map port",                  // [ number ]
        "macid":"gateway mac identify",            // [ string ]
        "hand_ip":"local target IP",               // [ string ]
        "hand_port":"local target",                // [ string ]
        "hand_proto":"local protocol",             // [ "tcp", "udp", "dev" ]
        "lock":"source IP lock"                    // [ number ], optional, present when lock > 0
    }
    ```

    Example, map TCP 80 on gateway to a free public port
    ```shell
    center@pport.tcp_map[ ,00037f122340,127.0.0.1,80,tcp,0,0 ]
    {
        "port":"25000",
        "macid":"00037f122340",
        "hand_ip":"127.0.0.1",
        "hand_port":"80",
        "hand_proto":"tcp"
    }
    ```

    Example, map serial uart@tty with idle timeout and source lock
    ```shell
    center@pport.tcp_map[ ,00037f122340,127.0.0.1,uart@tty,dev,180,1 ]
    {
        "port":"25002",
        "macid":"00037f122340",
        "hand_ip":"127.0.0.1",
        "hand_port":"uart@tty",
        "hand_proto":"dev",
        "lock":"1"
    }
    ```

+ `tcp_unmap[ port, macid ]` **remove TCP map rule(s)**
    - port --------------- [ number ], optional, public port to unmap
    - macid -------------- [ string ], optional, 12-hex mac; with port empty, unmap all TCP maps for this macid
    - At least one of port or macid is required
    - failed return tfalse
    - succeed return ttrue

    Example, unmap one TCP port
    ```shell
    center@pport.tcp_unmap[ 25000, ]
    ttrue
    ```

    Example, unmap all TCP maps for a gateway
    ```shell
    center@pport.tcp_unmap[ ,00037f122340 ]
    ttrue
    ```

+ `udp_map[ port, macid, hand_ip, hand_port, hand_proto, timeout, lock ]` **add a UDP public map to a gateway local target**
    - port --------------- [ number ], optional, public port; 0 or empty allocates the next free port from `static_port`
    - macid -------------- [ string ], 12-hex gateway mac identify
    - hand_ip ------------ [ string ], local IP on the gateway
    - hand_port ---------- [ string ], local port, or uart object id when `hand_proto` is `dev`
    - hand_proto --------- [ "tcp", "udp", "dev" ], optional, default be `udp`
    - timeout ------------ [ number ], optional, idle seconds then unmap; 0 or empty means persistent; when > 0 the gateway must already be online
    - lock --------------- [ number ], optional, > 0 locks the first public client source IP
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "port":"public map port",                  // [ number ]
        "macid":"gateway mac identify",            // [ string ]
        "hand_ip":"local target IP",               // [ string ]
        "hand_port":"local target",                // [ string ]
        "hand_proto":"local protocol",             // [ "tcp", "udp", "dev" ]
        "lock":"source IP lock"                    // [ number ], optional, present when lock > 0
    }
    ```

    Example, map UDP 5000 on gateway
    ```shell
    center@pport.udp_map[ ,00037f122340,192.168.8.1,5000,udp,0,0 ]
    {
        "port":"25001",
        "macid":"00037f122340",
        "hand_ip":"192.168.8.1",
        "hand_port":"5000",
        "hand_proto":"udp"
    }
    ```

+ `udp_unmap[ port, macid ]` **remove UDP map rule(s)**
    - port --------------- [ number ], optional, public port to unmap
    - macid -------------- [ string ], optional, 12-hex mac; with port empty, unmap all UDP maps for this macid
    - At least one of port or macid is required
    - failed return tfalse
    - succeed return ttrue

    Example, unmap one UDP port
    ```shell
    center@pport.udp_unmap[ 25001, ]
    ttrue
    ```
