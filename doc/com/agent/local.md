***
## Local Agent -- LAN Management Service
Provide local area network management services including JSON TCP command interface and UDP broadcast discovery/command interface for device discovery, querying and configuration on the LAN

#### Configuration( agent@local )
```json
{
    // JSON TCP service
    "json":"JSON TCP command service",                          // [ "disable", "enable" ]
    "json_port":"JSON TCP service listen port",                 // [ number ], default 22220
    "json_command":"JSON TCP service command permission",       // [ "query", "all" ]
                                                                   // "query" only allow query commands (get, status, list, info)
                                                                   // "all" allow all commands including set and call
    "json_manager":"JSON TCP service access control",           // [ string or json ]
                                                                   // string format: IP or MAC addresses separated by semicolon "192.168.8.100;00:11:22:33:44:55"
                                                                   // json format: { "host1":"192.168.8.100", "host2":"00:11:22:33:44:55" }
                                                                   // when not set, no access control (allow all)
                                                                   // when set, only listed IP/MAC can access, others are dropped

    // Broadcast UDP service
    "broadcast":"broadcast UDP discovery service",              // [ "disable", "enable" ]
    "broadcast_port":"broadcast UDP service listen port",       // [ number ], default 22222
    "broadcast_group":"broadcast group name for discovery",     // [ string ], default "default"
    "broadcast_command":"broadcast service command permission",  // [ "query", "all", other ]
                                                                   // "query" allow query commands only (get, status, list, info)
                                                                   // "all" allow all commands including set and call
                                                                   // other value: disable all commands, only basic discovery
    "broadcast_manager":"broadcast service access control"      // [ string or json ], same format as json_manager
}
```

Example, show all the configure
```shell
agent@local
{
    "broadcast":"enable",                     # broadcast service is enabled
    "broadcast_port":"22222",                 # broadcast listen on port 22222
    "broadcast_group":"default",              # broadcast group name
    "broadcast_command":"query",              # only allow query commands
    "json":"disable",                         # JSON TCP service is disabled
    "json_port":"22220"                       # JSON TCP listen on port 22220
}
```

Example, enable the JSON TCP service with access control
```shell
agent@local={"json":"enable","json_port":"22220","json_command":"all","json_manager":"192.168.8.100;192.168.8.101"}
ttrue
```

Example, enable the broadcast service with full command permission
```shell
agent@local:broadcast=enable
ttrue
agent@local:broadcast_command=all
ttrue
```

Example, disable the JSON TCP service
```shell
agent@local:json=disable
ttrue
```

Example, set broadcast group name
```shell
agent@local:broadcast_group=mygroup
ttrue
```



#### **API**

+ `setup[]` **setup the local agent services**
    setup will read the configuration and start the enabled services:
    1. If json is "enable", setup iptables access control rules and start the JSON TCP service
    2. If broadcast is "enable", setup iptables access control rules and start the broadcast UDP service
    - succeed return ttrue
    - failed return tfalse

    Example, setup the local agent
    ```shell
    agent@local.setup
    ttrue
    ```

+ `shut[]` **shutdown all local agent services**
    shut will clean up iptables rules for both json and broadcast services, and stop both service processes
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the local agent
    ```shell
    agent@local.shut
    ttrue
    ```

+ `json_start[]` **start the JSON TCP service only**
    - succeed return ttrue

    Example, start json service
    ```shell
    agent@local.json_start
    ttrue
    ```

+ `json_stop[]` **stop the JSON TCP service only**
    - succeed return ttrue

    Example, stop json service
    ```shell
    agent@local.json_stop
    ttrue
    ```

+ `broadcast_start[]` **start the broadcast UDP service only**
    - succeed return ttrue

    Example, start broadcast service
    ```shell
    agent@local.broadcast_start
    ttrue
    ```

+ `broadcast_stop[]` **stop the broadcast UDP service only**
    - succeed return ttrue

    Example, stop broadcast service
    ```shell
    agent@local.broadcast_stop
    ttrue
    ```

+ `json_service[]` **internal JSON TCP service (not called directly)**
    this is the JSON TCP command service started by setup, it handles:
    1. Listen on TCP port (default 22220) on the local network interface
    2. Wait for system boot (at least 60 seconds uptime)
    3. Accept TCP connections from management tools
    4. Receive JSON command requests and execute them
    5. Return JSON response with results

    The service will exit and return different values based on the situation:
    - return terror: cannot get the local network device
    - return tfalse: socket error or accept error

    **Command permission (json_command):**
    - "query" mode: only allow GET operations and CALL operations that contain "stat", "list", or "info" in method name
    - "all" mode: allow all operations including SET, OR, and any CALL

    **JSON command protocol:**
    - Request format:
    ```json
    {
        "1":{"obj":"upnp@miniupnpd","op":"=","v":{"status":"enable"}},    // JSON command format
        "2":"land@syslog:size=100",                                         // string command format
        "3":"land@machine:mac=000371f12300"                                 // string command format
    }
    ```
    - Response format:
    ```json
    {
        "1":"ttrue",
        "2":"ttrue",
        "3":"tfalse"
    }
    ```

+ `broadcast_service[]` **internal broadcast UDP service (not called directly)**
    this is the UDP broadcast discovery/command service started by setup, it handles:
    1. Listen on UDP port (default 22222) bound to the local network interface
    2. Support both new protocol (ENQ/ACK) and old protocol (comma/pipe separated)
    3. Respond to device discovery requests with device info
    4. Execute commands received via broadcast and return results
    5. Support address assignment for device IP configuration

    The service will exit and return different values based on the situation:
    - return terror: cannot get the local network device
    - return tfalse: socket error or recv error

    **Broadcast protocols:**

    *New protocol (ENQ/ACK):*
    - Discovery request: `<group>$ENQ<commands separated by US char>`
    - Targeted request: `<macid>$ENQ<commands or JSON>`
    - Response: `<macid>$ACK<results separated by US char>`

    *Old protocol (comma/pipe):*
    - Basic discovery request: `<group>\0`
    - Basic discovery response: `<macid>|<lanip>|<port>`
    - Query request: `<group>,<command1>,<command2>,...`
    - Query response: `<macid>|<result1>|<result2>|...`
    - Address assign request: `<macid>>{"ip":"...","mask":"...","gw":"...","dns":"..."}`

    **Command permission (broadcast_command):**
    - "query" mode (mode=1): allow GET and limited CALL operations (stat, list, info). For agent@local, factory, gpio, machine objects only CALL is allowed
    - "all" mode (mode=2): allow all operations
    - other value (mode=0): disable all command execution, only basic discovery
