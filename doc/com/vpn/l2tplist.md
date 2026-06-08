## vpn@l2tplist — L2TP Client Infrastructure Management

### Overview

Manage the lifecycle of all L2TP VPN client instances. This component scans for configured instances (`vpn@l2tp`, `vpn@l2tp2`, … up to `vpn@l2tp10`) and registers each enabled one with `network@frame` as a VPN interface. It also provides instance creation and deletion APIs.

- registers/unregisters all configured L2TP instances with `network@frame` at boot/shutdown
- lists all L2TP client configurations and statuses
- creates new L2TP instances with sensible defaults
- deletes existing L2TP instances and unregisters them from the network frame



### Network Architecture

`vpn@l2tplist` is the **infrastructure manager** for L2TP VPN clients. At boot, it iterates all configured `vpn@l2tp*` instances and registers each one with `network@frame` as a VPN interface. Each instance then runs its own L2TP/PPP service independently via `openl2tpd`. `vpn@l2tplist` does not hold its own configuration — all per-instance settings live in `vpn@l2tp`, `vpn@l2tp2`, etc.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### API Reference

#### Management APIs

+ `setup[]` **initialize L2TP infrastructure**
    - succeed return ttrue
    - iterates vpn@l2tp through vpn@l2tp10, registers each configured instance with `network@frame` and schedules its setup

+ `shut[]` **shut down all L2TP clients**
    - succeed return ttrue
    - calls shut on each instance, then unregisters from `network@frame`


#### Query APIs

+ `list[]` **list all L2TP client configurations**
    - failed return NULL
    - succeed return [ json ], all configured instances with their full configuration
    ```json
    {
        "vpn@l2tp":                         // [ string ]: { json }, instance name
        {
            "status":"client status",       // [ string ]
            "server":"l2tp server",         // [ string ]
            "port":"server port",           // [ string ]
            // ... full instance config
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all L2TP clients
    ```shell
    vpn@l2tplist.list
    {
        "vpn@l2tp":
        {
            "status":"enable",
            "extern":"default",
            "server":"l2tp.example.com",
            "port":"1701",
            "authmode":"chap",
            "masq":"enable",
            "defaultroute":"enable"
        }
    }
    ```

+ `status[]` **list all L2TP client statuses**
    - failed return NULL
    - succeed return [ json ], all configured instances with their runtime status
    ```json
    {
        "vpn@l2tp":                         // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "uping", "down", "up" ]
            "serverip":"server IP",         // [ ip address ], resolved server IP
            // ... other status fields when up
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all L2TP client statuses
    ```shell
    vpn@l2tplist.status
    {
        "vpn@l2tp":
        {
            "status":"up",
            "netdev":"ppp0",
            "ip":"10.0.0.2",
            "dstip":"10.0.0.1",
            "mask":"255.255.255.255",
            "serverip":"198.51.100.1",
            "dns":"8.8.8.8",
            "livetime":"02:30:15:0",
            "rx_bytes":"123456",
            "rx_packets":"789",
            "tx_bytes":"654321",
            "tx_packets":"987"
        }
    }
    ```


#### Control APIs

+ `add[ server, port ]` **add a new L2TP client**
    - server ------------ [ string ], L2TP server address (IP or domain)
    - port -------------- [ number ], optional, L2TP server port, default 1701
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "vpn@l2tp4")
    - creates with defaults: status=disable, extern=default, authmode=disable, masq=enable, defaultroute=enable, mtu=1400
    - registers the new instance with `network@frame`

    Example, add an L2TP client
    ```shell
    vpn@l2tplist.add[ l2tp.example.com ]
    vpn@l2tp3
    ```

    Example, add an L2TP client with custom port
    ```shell
    vpn@l2tplist.add[ l2tp.example.com, 1702 ]
    vpn@l2tp3
    ```

+ `delete[ object ]` **delete an L2TP client**
    - object ------------ [ string ], the instance name to delete (e.g. "vpn@l2tp3")
    - failed return tfalse (object not found or not a valid L2TP instance)
    - succeed return ttrue
    - shuts down the instance, unregisters from `network@frame`, and removes configuration

    Example, delete an L2TP client
    ```shell
    vpn@l2tplist.delete[ vpn@l2tp3 ]
    ttrue
    ```
