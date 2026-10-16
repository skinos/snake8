## ovpn@list — OpenVPN Client Infrastructure Management

### Overview

Manage the lifecycle of all OpenVPN client instances. This component scans for configured instances (`ovpn@client`, `ovpn@client2`, … up to `ovpn@client10`) and registers each one with `network@frame` as a VPN interface. It also provides instance creation and deletion APIs.

- registers/unregisters all configured OpenVPN instances with `network@frame` at boot/shutdown
- lists all OpenVPN client configurations and statuses
- creates new OpenVPN instances with sensible defaults
- deletes existing OpenVPN instances and unregisters them from the network frame



### Architecture

`ovpn@list` is the **infrastructure manager** for OpenVPN clients. At boot, it iterates all configured `ovpn@client*` instances and registers each one with `network@frame` as a VPN interface. Each instance then runs its own `openvpn` process. `ovpn@list` does not hold its own configuration — all per-instance settings live in `ovpn@client`, `ovpn@client2`, etc.



### API Reference

#### Management APIs

+ `setup[]` **initialize OpenVPN infrastructure**
    - succeed return ttrue
    - iterates ovpn@client through ovpn@client10, registers each configured instance with `network@frame` and schedules its setup

+ `shut[]` **shut down all OpenVPN clients**
    - succeed return ttrue
    - calls shut on each instance, then unregisters from `network@frame`


#### Query APIs

+ `list[]` **list all OpenVPN client configurations**
    - failed return NULL
    - succeed return [ json ], all configured instances with their full configuration
    ```json
    {
        "ovpn@client":                      // [ string ]: { json }, instance name
        {
            "status":"client status",       // [ string ]
            "server":"OpenVPN server",      // [ string ]
            "port":"server port"            // [ string ]
            // ... full instance config
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all OpenVPN clients
    ```shell
    ovpn@list.list
    {
        "ovpn@client":
        {
            "status":"enable",
            "extern":"default",
            "server":"vpn.example.com",
            "port":"1194",
            "proto":"udp",
            "dev":"tun",
            "auth":"certificate",
            "masq":"enable",
            "defaultroute":"enable"
        }
    }
    ```

+ `status[]` **list all OpenVPN client statuses**
    - failed return NULL
    - succeed return [ json ], all configured instances with their runtime status
    - for each `ovpn@client*` slot, calls that instance's `status[]`
    ```json
    {
        "ovpn@client":                      // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "uping", "down", "up" ]
            "serverip":"server IP",         // [ ip address ], resolved server IP
            "netdev":"netdev name"          // [ string ], e.g. "ovpn0"
            // ... other status fields when up
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all OpenVPN client statuses
    ```shell
    ovpn@list.status
    {
        "ovpn@client":
        {
            "status":"up",
            "netdev":"ovpn0",
            "ip":"10.8.0.2",
            "mask":"255.255.255.0",
            "gw":"10.8.0.1",
            "serverip":"198.51.100.1",
            "dns":"8.8.8.8",
            "livetime":"02:30:15:0",
            "rx_bytes":"123456",
            "tx_bytes":"654321"
        }
    }
    ```


#### Control APIs

+ `add[ server, port ]` **add a new OpenVPN client**
    - server ------------ [ string ], OpenVPN server address (IP or domain)
    - port -------------- [ number ], optional, server port, default 1194
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "ovpn@client3")
    - creates with defaults: status=disable, extern=default, proto=udp, dev=tun, topology=subnet, cipher=AES-256-GCM, auth=certificate, comp_lzo=disable, hmac=disable, interval=10, timeout=60, masq=enable, defaultroute=enable, log=file
    - registers the new instance with `network@frame`

    Example, add an OpenVPN client
    ```shell
    ovpn@list.add[ vpn.example.com ]
    ovpn@client
    ```

    Example, add an OpenVPN client with custom port
    ```shell
    ovpn@list.add[ vpn.example.com, 1195 ]
    ovpn@client3
    ```

+ `delete[ object ]` **delete an OpenVPN client**
    - object ------------ [ string ], the instance name to delete (e.g. "ovpn@client3")
    - failed return tfalse (object not found or not a valid OpenVPN instance)
    - succeed return ttrue
    - shuts down the instance, unregisters from `network@frame`, and removes configuration, certificate files, and the instance log

    Example, delete an OpenVPN client
    ```shell
    ovpn@list.delete[ ovpn@client3 ]
    ttrue
    ```
