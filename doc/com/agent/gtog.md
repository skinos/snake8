***
## Gateway to Gateway -- WireGuard Mesh Network
Create and manage WireGuard-based mesh VPN networks between multiple devices. Support endpoint discovery, automatic master election, branch/leaf topology, NAT traversal via ICMP/UDP raw packets, and dynamic peer management

#### Configuration( agent@gtog )
```json
{
    "net_max":"maximum number of gtog networks",           // [ number ], default GTOG_NET_MAX
    "port_start":"starting local port for networks"        // [ number ], default GTOG_LOCAL_PORT
                                                              // each network uses port_start + (index-1)
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



#### **Network Configuration( agent@net )**
Each registered network has its own configuration object (agent@net for the first, agent@net2 for the second, etc.)
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



#### **API( agent@gtog )**

+ `setup[]` **setup all gtog network infrastructure**
    setup will read the gtog configuration, then for each registered network:
    1. Ensure local port is assigned
    2. Register network interface with network@frame
    3. Register setup command with init system for delayed start
    4. Register setup on network online event
    - succeed return ttrue
    - failed return tfalse

    Example, setup the gtog infrastructure
    ```shell
    agent@gtog.setup
    ttrue
    ```

+ `shut[]` **shutdown all gtog network clients**
    shut will for each registered network:
    1. Stop the service process
    2. Unregister from init system
    3. Unregister from network online event
    4. Unregister network interface from network@frame
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
            "nattype":"device NAT type",                  // [ "1", "2" ]
                                                             // "1": full cone NAT, can be branch (relay node)
                                                             // "2": restricted NAT, can only be leaf
            "pref":"master preference value",             // [ number ], higher value = higher priority to become master

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
    add a branch node that this device should connect to as a relay point. Branch nodes have full cone NAT and can relay traffic
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
    add a leaf node as a WireGuard peer. Leaf nodes connect through branches and cannot relay traffic
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

+ `online[]` **internal: handle network online event**
    called when the network comes online, reads the gateway IP for NAT traversal
    - succeed return ttrue

+ `service[]` **internal background service for each network (not called directly)**
    this is the main service loop for each individual network (agent@net, agent@net2, etc.), it handles:
    1. Read network configuration (netid, network address, keepalive settings)
    2. Generate WireGuard key pair if not exists
    3. Create WireGuard interface and configure IP address
    4. Open raw UDP socket for NAT traversal probing
    5. Open ICMP socket for NAT type detection
    6. Main keepalive loop:
       - If role is ENDPOINT: send keepalive to server, receive endpoint/branch/leaf commands
       - If role is BRANCH: keepalive to both server and master, relay traffic
       - If role is LEAF: keepalive to master only
    7. Handle master election based on preference values and NAT types
    8. Manage WireGuard peers dynamically

    The service will exit and return different values based on the situation:
    - return terror: configuration error (missing netid, etc.), will not auto-restart
    - return tfalse: connection failed, will auto-retry
    - return ttrue: network not ready, will auto-retry

    **NAT traversal mechanism:**
    - Raw UDP packets are used to probe and maintain NAT mappings
    - ICMP echo requests are used to detect NAT type (full cone vs restricted)
    - Packet format: custom UDP payload containing network identifier for peer discovery

    **Master election:**
    - Each endpoint has a preference value (pref)
    - Endpoints with nattype="1" (full cone NAT) can become branch/master
    - The endpoint with the highest pref and nattype="1" becomes the master
    - Other endpoints connect as leaf nodes through the master or branches

    **WireGuard management:**
    - Key pair stored in registry: `<object>.pub` and `<object>.piv`
    - Interface created with: `ip link add <netdev> type wireguard`
    - Peers added/removed dynamically based on endpoint list updates
    - Allowed IPs configured based on point (VPN IP) and extend (routed network)
