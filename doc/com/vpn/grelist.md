## vpn@grelist — GRE Tunnel Infrastructure Management

### Overview

Manage the lifecycle of all GRE tunnel instances. This component scans for configured instances (`vpn@gre`, `vpn@gre2`, … up to `vpn@gre10`) and registers each enabled one with `network@frame` as a VPN interface. It also provides instance creation and deletion APIs.

- registers/unregisters all configured GRE instances with `network@frame` at boot/shutdown
- lists all GRE tunnel configurations and statuses
- creates new GRE instances with optional immediate setup
- deletes existing GRE instances and unregisters them from the network frame



### Network Architecture

`vpn@grelist` is the **infrastructure manager** for GRE tunnels. At boot, it iterates all configured `vpn@gre*` instances and registers each one with `network@frame` as a VPN interface. Each instance then manages its own GRE tunnel independently. `vpn@grelist` does not hold its own configuration — all per-instance settings live in `vpn@gre`, `vpn@gre2`, etc.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### API Reference

#### Management APIs

+ `setup[]` **initialize GRE infrastructure**
    - succeed return ttrue
    - iterates vpn@gre through vpn@gre10, registers each configured instance with `network@frame` and schedules its setup

+ `shut[]` **shut down all GRE tunnels**
    - succeed return ttrue
    - calls shut on each instance (which tears down the ip tunnel), then unregisters from `network@frame`


#### Query APIs

+ `list[]` **list all GRE tunnel configurations**
    - failed return NULL
    - succeed return [ json ], all configured instances with their full configuration
    ```json
    {
        "vpn@gre":                          // [ string ]: { json }, instance name
        {
            "status":"client status",       // [ string ]
            "peer":"tunnel peer",           // [ string ]
            "localip":"local IP",           // [ ip address ]
            "remoteip":"remote IP",         // [ ip address ]
            // ... full instance config
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all GRE tunnels
    ```shell
    vpn@grelist.list
    {
        "vpn@gre":
        {
            "status":"enable",
            "extern":"default",
            "peer":"203.0.113.1",
            "localip":"10.0.0.1",
            "remoteip":"10.0.0.2",
            "masq":"enable",
            "defaultroute":"enable"
        }
    }
    ```

+ `status[]` **list all GRE tunnel statuses**
    - failed return NULL
    - succeed return [ json ], all configured instances with their runtime status
    ```json
    {
        "vpn@gre":                          // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "uping", "down", "up" ]
            "serverip":"peer IP",           // [ ip address ], resolved peer IP
            // ... other status fields when up
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all GRE tunnel statuses
    ```shell
    vpn@grelist.status
    {
        "vpn@gre":
        {
            "status":"up",
            "netdev":"gre",
            "ip":"10.0.0.1",
            "dstip":"10.0.0.2",
            "mask":"255.255.255.255",
            "serverip":"203.0.113.1",
            "livetime":"02:30:15:0",
            "rx_bytes":"123456",
            "rx_packets":"789",
            "tx_bytes":"654321",
            "tx_packets":"987"
        }
    }
    ```


#### Control APIs

+ `add[ peer, localip, remoteip ]` **add a new GRE tunnel**
    - peer ------------- [ string ], GRE peer address (remote endpoint IP or domain)
    - localip ---------- [ ip address ], optional, local tunnel IP
    - remoteip --------- [ ip address ], optional, remote tunnel IP
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "vpn@gre3")
    - if all three parameters are provided, status defaults to enable and setup is called immediately
    - if parameters are missing, status is not set (remains disabled)
    - registers the new instance with `network@frame`

    Example, add a GRE tunnel with full parameters
    ```shell
    vpn@grelist.add[ 203.0.113.1, 10.0.0.1, 10.0.0.2 ]
    vpn@gre3
    ```

    Example, add a GRE tunnel with peer only (disabled)
    ```shell
    vpn@grelist.add[ 203.0.113.1 ]
    vpn@gre3
    ```

+ `delete[ object ]` **delete a GRE tunnel**
    - object ------------ [ string ], the instance name to delete (e.g. "vpn@gre3")
    - failed return tfalse (object not found or not a valid GRE instance)
    - succeed return ttrue
    - shuts down the tunnel (ip tunnel del), unregisters from `network@frame`, and removes configuration

    Example, delete a GRE tunnel
    ```shell
    vpn@grelist.delete[ vpn@gre3 ]
    ttrue
    ```
