## vpn@pptplist — PPTP Client Infrastructure Management

### Overview

Manage the lifecycle of all PPTP VPN client instances. This component scans for configured instances (`vpn@pptp`, `vpn@pptp2`, … up to `vpn@pptp10`) and registers each enabled one with `network@frame` as a VPN interface. It also provides instance creation and deletion APIs.

- registers/unregisters all configured PPTP instances with `network@frame` at boot/shutdown
- lists all PPTP client configurations and statuses
- creates new PPTP instances with sensible defaults
- deletes existing PPTP instances and unregisters them from the network frame



### Network Architecture

`vpn@pptplist` is the **infrastructure manager** for PPTP VPN clients. At boot, it iterates all configured `vpn@pptp*` instances and registers each one with `network@frame` as a VPN interface. Each instance then runs its own PPP service independently. `vpn@pptplist` does not hold its own configuration — all per-instance settings live in `vpn@pptp`, `vpn@pptp2`, etc.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### API Reference

#### Management APIs

+ `setup[]` **initialize PPTP infrastructure**
    - succeed return ttrue
    - iterates vpn@pptp through vpn@pptp10, registers each configured instance with `network@frame` and schedules its setup

+ `shut[]` **shut down all PPTP clients**
    - succeed return ttrue
    - calls shut on each instance, then unregisters from `network@frame`


#### Query APIs

+ `list[]` **list all PPTP client configurations**
    - failed return NULL
    - succeed return [ json ], all configured instances with their full configuration
    ```json
    {
        "vpn@pptp":                         // [ string ]: { json }, instance name
        {
            "status":"client status",       // [ string ]
            "server":"pptp server",         // [ string ]
            // ... full instance config
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all PPTP clients
    ```shell
    vpn@pptplist.list
    {
        "vpn@pptp":
        {
            "status":"enable",
            "extern":"default",
            "server":"pptp.example.com",
            "port":"1723",
            "require_mppe":"disable",
            "masq":"enable",
            "defaultroute":"enable"
        }
    }
    ```

+ `status[]` **list all PPTP client statuses**
    - failed return NULL
    - succeed return [ json ], all configured instances with their runtime status
    ```json
    {
        "vpn@pptp":                         // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "uping", "down", "up" ]
            "serverip":"server IP",         // [ ip address ], resolved server IP
            // ... other status fields when up
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all PPTP client statuses
    ```shell
    vpn@pptplist.status
    {
        "vpn@pptp":
        {
            "status":"up",
            "netdev":"ppp0",
            "ip":"10.0.0.2",
            "dstip":"10.0.0.1",
            "mask":"255.255.255.255",
            "serverip":"203.0.113.1",
            "dns":"8.8.8.8",
            "livetime":"02:30:15:0",
            "rx_bytes":"123456",
            "rx_packets":"789",
            "tx_bytes":"654321",
            "tx_packets":"987"
        },
        "vpn@pptp2":
        {
            "status":"down",
            "serverip":"198.51.100.1"
        }
    }
    ```


#### Control APIs

+ `add[ server ]` **add a new PPTP client**
    - server ------------ [ string ], PPTP server address (IP or domain)
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "vpn@pptp4")
    - creates with defaults: status=disable, extern=default, port=1723, require_mppe=disable, masq=enable, defaultroute=enable, mtu=1400
    - registers the new instance with `network@frame`

    Example, add a PPTP client
    ```shell
    vpn@pptplist.add[ pptp.example.com ]
    vpn@pptp3
    ```

+ `delete[ object ]` **delete a PPTP client**
    - object ------------ [ string ], the instance name to delete (e.g. "vpn@pptp3")
    - failed return tfalse (object not found or not a valid pptp instance)
    - succeed return ttrue
    - shuts down the instance, unregisters from `network@frame`, and removes configuration

    Example, delete a PPTP client
    ```shell
    vpn@pptplist.delete[ vpn@pptp3 ]
    ttrue
    ```
