## Network client — one GTOG VPN instance
Each **`agent@net`**, **`agent@net2`**, … is one mesh VPN membership, usually managed by **`agent@gtog`**. Configuration covers server reachability, VPN addressing, keepalives, optional DNS/routing, and peer updates via **`endpoint` / `branch` / `leaf`**.

### **Configuration( `agent@net` )**

**agent@net** is first gtog network
**agent@net2** is second gtog network

```json
{
    // server connection
    "server":"gtog network server address",                    // [ string ], domain name or ip address
                                                                  // if not set, use heclient's server
    "port":"gtog network server port",                         // [ number ], default 20002
    "key":"shared key for traffic with the mesh coordinator", // [ string ], product default if omitted

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



### **Component API**

**Directly callable** APIs from HE / eline / HTTP `/he`.
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
                                                  // "uping" starting; "down" stopped; "up" healthy; "failed"/"block" keepalive problems
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
        "mode":"current node mode",            // [ number ] 1=no coordinator yet, 2=leaf, 3=branch, 4=this device is coordinator
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
            "nattype":"NAT type",                      // [ "1", "2" ], influences relay vs leaf-only role
            "pref":"coordinator preference",           // [ number ], higher = stronger candidate
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
            "nattype":"device NAT type",                  // [ "1", "2" ]
            "pref":"coordinator preference",              // [ number ]

            "point":"endpoint VPN ip address",            // [ ip address ]
            "extend":"endpoint local network"             // [ network address ]
        }
        // ... more endpoints
    }
    ```

    After the list changes, each device picks **master / branch / leaf** from **`nattype`**, **`pref`**, and peer visibility (same rules as **`agent@gtog.endpoint`**).

    Example, update endpoint list
    ```shell
    agent@net.endpoint[{"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}}]
    ttrue
    ```

+ `branch[ {branch information} ]` **add or update a branch (relay) node**
    Adds or refreshes a relay-capable peer; coordinator role may move to the stronger **`pref`** when topology allows.

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
    Adds or refreshes a non-relay peer; routing follows the current coordinator path.

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

+ `online[ {network information} ]` **internal — link came up**
    Applies DNS, policy routes, masquerade, and MTU from configuration and the supplied snapshot.
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

+ `offline[]` **internal — link went down**
    Reverts **`online[]`** side effects (DNS, NAT/MSS tweaks, etc.).
    - succeed return ttrue

+ `service[]` **internal (not called via HE)**
    Background worker for this **`agent@net*`** object: brings up WireGuard, registers with the mesh coordinator, syncs tunables (**`network`**, keepalive fields, …), maintains reachability for the current role (**master / branch / leaf**), and drives **`online[]` / `offline[]`** when the tunnel state changes. **Hard config / disable** errors stop without auto-restart; **link / socket** issues and **extern not ready** are retried.

### **Lifecycle API**

+ `setup[]` / `shut[]` — **when implemented** for **`agent@net`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.

### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_agent_net(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@net", "status") == NULL)
        return -1;
    return ssets_string("agent@net", "enable", "status") ? 0 : -1;
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

/* e.g. scall("agent@net", "list", NULL); talk_free if JSON */
```

