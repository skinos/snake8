## network@vlan — VLAN Device Management

### Overview

Manage 802.1Q VLAN devices on top of physical or bridge interfaces. This component reads VLAN definitions from **`arch@net`** (the `vlan` subtree of the current network mode), creates Linux VLAN netdevs (e.g. `eth0.1`), and registers them with **`network@frame`**. Each VLAN instance is bound to a root device (`ifdev`) and a VLAN ID (`vid`).

- creates and manages VLAN netdevs using `vconfig`
- registers VLAN devices with `network@frame` for use by ifname components
- delegates connection status to the root device via `vconnected` / `vonline` / `voffline`
- supports dynamic add/delete of VLAN devices when root devices appear or disappear



### Network Architecture

`network@vlan` is an **infrastructure layer** component coordinated by `network@frame` during boot. It reads VLAN definitions from `arch@net.vlan`, creates VLAN netdevs on top of root devices (e.g. `ethernet@lan`), and registers them with `network@frame`. The VLAN netdevs are then used by ifname components (e.g. `ifname@lan`) as their `ifdev`.

For the full network architecture, see [`frame.md`](frame.md).

### Configuration reference ( network@vlan )

VLAN configuration is defined in **`arch@net`** under the `vlan` subtree of each network mode. `network@vlan` reads this configuration at setup time.

```json
// Attributes introduction 
{
    "vlan":                                     // VLAN definitions
    {
        "identify or name":                     // [ string ], VLAN instance name (e.g. "lan", "wan")
        {
            "ifdev":"root device",              // [ string ], root device component (e.g. "ethernet@lan")
            "vid":"802.1Q VLAN ID"              // [ string ], decimal VLAN ID (e.g. "1", "100")
        }
        // "...":{}  How many VLANs show how many properties
    }
}
```

#### Configuration example

Example, show VLAN configuration from arch@net
```shell
arch@net
{
    "vlan":
    {
        "lan":
        {
            "ifdev":"ethernet@lan",
            "vid":"1"
        },
        "wan":
        {
            "ifdev":"ethernet@wan",
            "vid":"100"
        }
    }
}
```



### API Reference

#### Management APIs

+ `setup[]` **initialize VLAN infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - when called on the root object (`network@vlan`): registers all VLAN instances from `arch@net.vlan`
    - when called on an instance (`vlan@lan`): creates the VLAN netdev and registers with `network@frame`

+ `shut[]` **shut down VLAN infrastructure**
    - succeed return ttrue
    - when called on the root object: unregisters and frees all VLAN instances
    - when called on an instance: deletes from `network@frame` and removes the VLAN netdev


#### Query APIs

+ `netdev[]` **get the VLAN netdev name**
    - failed return NULL
    - succeed return [ string ], the Linux netdev name (e.g. "eth0.1")

    Example, get netdev of vlan@lan
    ```shell
    vlan@lan.netdev
    eth0.1
    ```

+ `list[]` **list all VLAN instances or get one instance config**
    - failed return NULL
    - succeed return [ json ], VLAN list or single instance config
    ```json
    {
        "vlan@lan":                         // [ string ]: { json }, VLAN instance name
        {
            "ifdev":"root device",          // [ string ]
            "vid":"VLAN ID"                 // [ string ]
        }
        // "...":{}  How many VLANs show how many properties
    }
    ```

    Example, list all VLAN instances
    ```shell
    network@vlan.list
    {
        "vlan@lan":
        {
            "ifdev":"ethernet@lan",
            "vid":"1"
        },
        "vlan@wan":
        {
            "ifdev":"ethernet@wan",
            "vid":"100"
        }
    }
    ```

+ `status[]` **get VLAN connection status**
    - failed return tfalse
    - succeed return [ json ], VLAN status
    ```json
    {
        "status":"current status"           // [ "up", "down" ]
    }
    ```

    Example, get status of vlan@lan
    ```shell
    vlan@lan.status
    {
        "status":"up"
    }
    ```


#### Control APIs

+ `add[ ifdev ]` **add VLAN devices for a root device**
    - ifdev ------------ [ string ], the root device component name
    - failed return tfalse
    - succeed return ttrue
    - creates all VLAN netdevs that depend on the specified `ifdev`, brings up the root device, and registers with `network@frame`

    Example, add VLANs for ethernet@lan
    ```shell
    network@vlan.add[ ethernet@lan ]
    ttrue
    ```

+ `delete[ ifdev ]` **delete VLAN devices for a root device**
    - ifdev ------------ [ string ], the root device component name
    - failed return tfalse
    - succeed return ttrue
    - removes all VLAN netdevs that depend on the specified `ifdev` from `network@frame` and deletes the VLAN devices

    Example, delete VLANs for ethernet@lan
    ```shell
    network@vlan.delete[ ethernet@lan ]
    ttrue
    ```

+ `setmac[ mac ]` **set MAC address on the VLAN netdev**
    - mac -------------- [ string ], MAC address
    - failed return tfalse
    - succeed return ttrue

    Example, set MAC address on vlan@lan
    ```shell
    vlan@lan.setmac[ 02:50:F4:00:00:01 ]
    ttrue
    ```
