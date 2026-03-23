***
## Network Client -- GTOG WireGuard Network Instance
Individual WireGuard mesh network client instance. Each network (agent@net, agent@net2, ...) represents one gtog mesh VPN network, managed by the gtog infrastructure (agent@gtog). Handles WireGuard interface lifecycle, server registration, endpoint/branch/leaf peer management, keepalive, routing, DNS, and network online/offline events

#### Configuration( agent@net )
**agent@net** is first gtog network
**agent@net2** is second gtog network

```json
{
    // server connection
    "server":"gtog network server address",                    // [ string ], domain name or ip address
                                                                  // if not set, use heclient's server
    "port":"gtog network server port",                         // [ number ], default 20002
    "key":"encryption key for server communication",           // [ string ], default "NPORT-UDP@ashyelf.com"

    // bindding extern network
    "extern":"bindding extern ifname to connect server",       // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], optional
                                                                  // when not set, use default gateway to connect server

    // network identify
    "netid":"network identify string",                         // [ string ], unique network identifier assigned by server
    "network":"network address (CIDR format)",                 // [ string ], e.g. "10.0.0.0/24", assigned by server or config
    "lport":"local WireGuard listen port",                     // [ number ], auto assigned by gtog based on port_start

    // keeplive
    "keepintval":"keeplive interval in seconds",               // [ number ], default 15
    "keepfailed":"keeplive max retry count",                   // [ number ], default 4
    "keeptimeout":"keeplive timeout in seconds",               // [ number ], default 15

    // interface
    "mtu":"WireGuard interface MTU",                           // [ number ], optional

    // DNS (only effective when custom_dns is "enable")
    "custom_dns":"use custom DNS settings",                    // [ "disable", "enable" ]
    "dns":"primary DNS server",                                // [ ip address ], optional
    "dns2":"secondary DNS server",                             // [ ip address ], optional
    "domain":"DNS search domain",                              // [ string ], optional

    // routing
    "metric":"route metric value",                             // [ number ], optional
    "defaultroute":"set as default route",                     // [ "disable", "enable" ]
    "route_table":                                             // custom route table, only when defaultroute is not "enable"
    {
        "route name":
        {
            "target":"destination network or host",            // [ ip address or network ]
            "mask":"subnet mask"                               // [ mask ], default "255.255.255.255" (host route)
        }
    }
}
```

Example, show all the configure
```shell
agent@net
{
    "port":"20002",                            # server port
    "netid":"office-vpn",                      # network identifier
    "network":"10.0.1.0/24",                   # VPN network address
    "keepintval":"10",                         # keeplive interval 10 seconds
    "keepfailed":"3",                          # max 3 keeplive failures
    "keeptimeout":"35"                         # keeplive timeout 35 seconds
}
```

Example, configure with custom DNS and default route
```shell
agent@net={"port":"20002","netid":"office-vpn","network":"10.0.1.0/24","custom_dns":"enable","dns":"8.8.8.8","dns2":"8.8.4.4","defaultroute":"enable","mtu":"1420"}
ttrue
```

Example, configure with custom route table
```shell
agent@net={"port":"20002","netid":"office-vpn","network":"10.0.1.0/24","route_table":{"r1":{"target":"192.168.10.0","mask":"255.255.255.0"},"r2":{"target":"172.16.0.0","mask":"255.255.0.0"}}}
ttrue
```

Example, set the extern interface
```shell
agent@net:extern=ifname@lte
ttrue
```



#### **API**
**agent@net** is first gtog network
**agent@net2** is second gtog network

+ `setup[]` **setup this network client, start the service**
    when called on the network object (agent@net), start the background service process for this network
    - succeed return ttrue
    - failed return tfalse

    Example, setup the first network
    ```shell
    agent@net.setup
    ttrue
    ```

+ `shut[]` **shutdown this network client**
    call network offline, stop the background service process, and bring down the WireGuard interface
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the first network
    ```shell
    agent@net.shut
    ttrue
    ```

+ `status[]` **get the network client current status and details**
    - failed return NULL
    - succeed return json to describes detailed status information
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status",             // [ "uping", "down", "up", "failed", "block" ]
                                                  // "uping" for service running but not connected
                                                  // "down" for service is not running
                                                  // "up" for connected and keeplive ok
                                                  // "failed" for keeplive failed (master_delay < 0)
                                                  // "block" for keeplive blocked (master_delay == 0)
        "delay":"keeplive round-trip delay",   // [ number ], in milliseconds, only when status is "up"
        "ip":"local VPN ip address",           // [ ip address ], only when interface is up
        "mask":"VPN netmask",                  // [ mask ], only when interface is up
        "dstip":"destination ip address",      // [ ip address ], only when point-to-point
        "livetime":"connection live time",      // [ string ], human readable, e.g. "1 hours 30 minutes"
        "rx_bytes":"total received bytes",     // [ number ]
        "rx_packets":"total received packets", // [ number ]
        "tx_bytes":"total sent bytes",         // [ number ]
        "tx_packets":"total sent packets",     // [ number ]
        "server":"gtog server address",        // [ string ]
        "pref":"self preference value",        // [ number ]
        "mode":"current node mode",            // [ number ]
                                                  // 1: NOMASTER (no master available)
                                                  // 2: LEAF (connected as leaf node)
                                                  // 3: BRANCH (connected as branch/relay node)
                                                  // 4: MASTER (self is master)
        "mip":"master VPN ip address",         // [ ip address ], current master's VPN ip
        "mmacid":"master mac identify",        // [ string ], current master's macid
        "mpref":"master preference value",     // [ number ], current master's pref
        "tid":"route table id"                 // [ number ], policy routing table id
    }
    ```

    Example, get status when connected as leaf node
    ```shell
    agent@net.status
    {
        "status":"up",                            # connected and keeplive ok
        "delay":"45",                             # 45ms round-trip to master
        "ip":"10.0.1.3",                          # local VPN ip
        "mask":"255.255.255.0",                   # VPN netmask
        "livetime":"2 hours 15 minutes",          # connection live time
        "rx_bytes":"1048576",                     # 1MB received
        "rx_packets":"1024",
        "tx_bytes":"524288",                      # 512KB sent
        "tx_packets":"512",
        "server":"cls.ashyelf.com",               # gtog server
        "pref":"50",                              # self preference
        "mode":"2",                               # leaf mode
        "mip":"10.0.1.1",                         # master VPN ip
        "mmacid":"001122334455",                  # master macid
        "mpref":"100"                             # master preference
    }
    ```

    Example, get status when connecting
    ```shell
    agent@net.status
    {
        "status":"uping"                          # service running, connecting
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@net.status
    {
        "status":"down"                           # service is not running
    }
    ```

    Example, get status as master node
    ```shell
    agent@net.status
    {
        "status":"up",
        "delay":"30",
        "ip":"10.0.1.1",
        "mask":"255.255.255.0",
        "livetime":"5 hours 10 minutes",
        "rx_bytes":"10485760",
        "rx_packets":"10240",
        "tx_bytes":"5242880",
        "tx_packets":"5120",
        "server":"cls.ashyelf.com",
        "pref":"100",
        "mode":"4"                                # master mode, no master info (self is master)
    }
    ```

+ `list[]` **list all endpoints of this network**
    return the full endpoint list stored in the endpoint file for this network
    - failed return NULL
    - succeed return json with all endpoint information
    ```json
    // Attributes introduction of json that the API return
    {
        "endpoint mac identify":
        {
            "point":"endpoint VPN ip address",         // [ ip address ]
            "extend":"endpoint local network",         // [ network address ]
            "pubkey":"WireGuard public key",           // [ string ]
            "nattype":"NAT type",                      // [ "1", "2" ], "1" for full cone (can be branch), "2" for restricted (leaf only)
            "pref":"master preference value",          // [ number ], higher = higher priority to become master
            "ip":"public internet ip address",         // [ ip address ]
            "port":"public internet port",             // [ number ]
            "macid":"device mac identify"              // [ string ]
        }
        // ... more endpoints
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
            "port":"10004",
            "macid":"001122334455"
        },
        "aabbccddeeff":
        {
            "point":"10.0.1.2",
            "extend":"192.168.2.0/24",
            "pubkey":"ghi789jkl012...",
            "nattype":"2",
            "pref":"50",
            "ip":"5.6.7.8",
            "port":"10004",
            "macid":"aabbccddeeff"
        }
    }
    ```

+ `endpoint[ {endpoint list} ]` **update the full endpoint list for this network**
    replace the entire endpoint list, reconfigure WireGuard peers, determine node role (master/branch/leaf), and update master information
    - {endpoint list} ------ json
    ```json
    // {endpoint list} attributes introduction
    {
        "endpoint mac identify":
        {
            "ip":"device public ip address",              // [ ip address ]
            "port":"device public port",                  // [ number ]
            "pubkey":"device WireGuard public key",       // [ string ]
            "nattype":"device NAT type",                  // [ "1", "2" ], "1" for full cone (can be branch), "2" for restricted (leaf only)
            "pref":"device master preference",            // [ number ]

            "point":"endpoint VPN ip address",            // [ ip address ]
            "extend":"endpoint local network"             // [ network address ]
        }
        // ... more endpoints
    }
    ```

    **Role determination after endpoint update:**
    - If self nattype is "1" (full cone) and no higher-pref master exists: become **MASTER**
    - If self nattype is "1" and higher-pref master exists: become **BRANCH**
    - If self nattype is "2" (restricted) and master exists: become **LEAF**
    - If self nattype is "2" and no master exists: become **NOMASTER**

    Example, update endpoint list
    ```shell
    agent@net.endpoint[{"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}}]
    ttrue
    ```

+ `branch[ {branch information} ]` **add or update a branch (relay) node**
    add a branch node as a WireGuard peer. If the branch has higher pref than current master, it becomes the new master, and the old master is demoted to branch

    **Behavior depends on current node mode:**
    - **MASTER mode**: if branch pref > self pref, branch becomes master and self becomes BRANCH. Otherwise add as branch peer
    - **NOMASTER mode**: branch becomes master, self becomes LEAF
    - **BRANCH/LEAF mode**: if branch pref > current master pref, branch becomes new master, old master demoted to branch. Otherwise add as branch peer

    - {branch information} ------ json
    ```json
    // {branch information} attributes introduction
    {
        "macid":"device mac identify",                    // [ string ]
        "ip":"device public ip address",                  // [ ip address ]
        "port":"device public port",                      // [ number ]
        "pubkey":"device WireGuard public key",           // [ string ]
        "nattype":"device NAT type",                      // [ "1", "2" ]
        "pref":"device master preference",                // [ number ]

        "point":"endpoint VPN ip address",                // [ ip address ]
        "extend":"endpoint local network"                 // [ network address ]
    }
    ```

    Example, add a branch node
    ```shell
    agent@net.branch[{"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"}]
    ttrue
    ```

+ `leaf[ {leaf information} ]` **add or update a leaf node**
    add a leaf node as a WireGuard peer

    **Behavior depends on current node mode:**
    - **MASTER/BRANCH mode**: add leaf as WireGuard peer with allowed IPs
    - **LEAF mode**: extend routing to the new leaf via master (since leaf cannot directly relay, route through master)
    - **NOMASTER mode**: record the leaf only (no WireGuard peer added, no route to master)

    - {leaf information} ------ json
    ```json
    // {leaf information} attributes introduction
    {
        "macid":"device mac identify",                    // [ string ]
        "ip":"device public ip address",                  // [ ip address ]
        "port":"device public port",                      // [ number ]
        "pubkey":"device WireGuard public key",           // [ string ]

        "point":"endpoint VPN ip address",                // [ ip address ]
        "extend":"endpoint local network"                 // [ network address ]
    }
    ```

    Example, add a leaf node
    ```shell
    agent@net.leaf[{"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def...","point":"10.0.1.2","extend":"192.168.2.0/24"}]
    ttrue
    ```

+ `online[ {network information} ]` **handle network online event (internal)**
    called when the network becomes online, configure DNS, routing, NAT masquerade, and MTU
    - {network information} ------ json
    ```json
    {
        "ifname":"network object name",        // [ string ], e.g. "agent@net"
        "netdev":"WireGuard device name",      // [ string ], e.g. "wg0"
        "gw":"gateway ip address",             // [ ip address ], master's VPN ip
        "dns":"primary DNS server",            // [ ip address ], optional, only when custom_dns is "enable"
        "dns2":"secondary DNS server",         // [ ip address ], optional
        "domain":"DNS search domain",          // [ string ], optional
        "masq":"enable NAT masquerade"         // [ "enable" ], set when no extend network configured
    }
    ```
    - succeed return ttrue

+ `offline[]` **handle network offline event (internal)**
    called when the network goes offline, clean up DNS file, NAT masquerade rules, and TCP MSS clamping
    - succeed return ttrue

+ `service[]` **internal background service (not called directly)**
    this is the main service loop for each network instance, it handles:
    1. Load WireGuard kernel module if not loaded
    2. Read configuration (server, netid, network, keepalive settings)
    3. Determine local IP address (from extern interface or default gateway)
    4. Create WireGuard interface and generate key pair
    5. Create raw UDP socket for server communication
    6. Register with server (send macid, netid, pubkey)
    7. Receive network configuration from server (network address, keepalive tuning)
    8. Request endpoint list by sending trigger to server
    9. Enter keepalive loop based on assigned role:
       - **MASTER/NOMASTER**: keepalive to server via raw UDP
       - **BRANCH**: keepalive to both server (raw UDP) and master (ICMP)
       - **LEAF**: keepalive to master via ICMP only

    The service will exit and return different values based on the situation:
    - return terror: configuration error (missing netid, server disable order), will not auto-restart
    - return tfalse: connection or socket error, will auto-retry
    - return ttrue: extern network interface is not ready, will auto-retry

    **Server registration protocol:**
    - Send: `<macid>;<netid>;<pubkey>;<uptime>;` (encrypted)
    - Response types:
      - `t;<netid>;` - registration acknowledged, branch capability confirmed
      - `u;<netid>;{json}` - network configuration update (network, keepintval, etc.)
      - `r;<netid>;` - reset order, service will restart
      - `d;<netid>;` - disable order, service will exit with error

    **Endpoint trigger protocol:**
    - Send: `<macid>;<netid>;<b|l>;<uptime>;` (encrypted, b=branch capable, l=leaf only)
    - Server responds by calling endpoint/branch/leaf APIs on this device

    **Server keepalive protocol:**
    - Send: `<macid>;<netid>;k;<uptime>;` (encrypted)
    - Response: `k;<netid>;` for success, `r;` for reset order
    - Returns round-trip delay in milliseconds

    **Master keepalive protocol:**
    - Send ICMP echo request to all endpoint VPN IPs (except self)
    - Wait for ICMP echo reply from master
    - Returns round-trip delay in milliseconds
