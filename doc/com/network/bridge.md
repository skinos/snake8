## network@bridge — Bridge Device Management

### Overview

Manage Linux bridge devices for combining multiple network interfaces into a single broadcast domain. This component reads bridge definitions from **`arch@net`** (the `bridge` subtree of the current network mode), creates Linux bridges using `brctl`, adds member interfaces, and registers them with **`network@frame`**. Bridges are typically used on the LAN side to combine ethernet ports and wireless APs into a single network.

- creates and manages bridge netdevs using `brctl`
- supports STP (Spanning Tree Protocol) with configurable timing parameters
- adds/removes member interfaces dynamically
- registers bridge devices with `network@frame` for use by ifname components
- delegates up/connect/down operations to all member interfaces



### Network Architecture

`network@bridge` is an **infrastructure layer** component coordinated by `network@frame` during boot. It reads bridge definitions from `arch@net.bridge`, creates bridge netdevs, adds member interfaces (e.g. `ethernet@lan2`, `wifi@nssid`), and registers them with `network@frame`. The bridge netdev is then used by `ifname@lan` as its `ifdev`.

For the full network architecture, see [`frame.md`](frame.md).

### Configuration reference ( network@bridge )

Bridge configuration is defined in **`arch@net`** under the `bridge` subtree of each network mode. `network@bridge` reads this configuration at setup time.

```json
// Attributes introduction 
{
    "bridge":                                   // Bridge definitions
    {
        "bridge key":                           // [ string ], bridge instance name (e.g. "lan")
        {
            "member":                           // map of member device components
            {
                "device component name":"",     // [ string ]: [ string ], e.g. "ethernet@lan2":""
                // "...":"..."  How many members show how many properties
            },
            "stp":"STP status",                 // [ "enable", "disable" ]
            "hellotime":"STP hello time",       // [ string ], optional, default "2" seconds
            "maxage":"STP max age",             // [ string ], optional, default "20" seconds
            "forwarddelay":"STP forward delay", // [ string ], optional, default "4" seconds
            "prio":"STP bridge priority"        // [ string ], optional, default "32768"
        }
        // "...":{}  How many bridges show how many properties
    }
}
```

#### Configuration example

Example, show bridge configuration from arch@net
```shell
arch@net
{
    "bridge":
    {
        "lan":
        {
            "member":
            {
                "ethernet@lan2":"",
                "wifi@nssid":""
            },
            "stp":"disable"
        }
    }
}
```

Example, show bridge with STP enabled
```shell
arch@net
{
    "bridge":
    {
        "lan":
        {
            "member":
            {
                "ethernet@lan":"",
                "ethernet@lan2":"",
                "wifi@nssid":""
            },
            "stp":"enable",
            "hellotime":"2",
            "maxage":"20",
            "forwarddelay":"4",
            "prio":"32768"
        }
    }
}
```



### API Reference

#### Management APIs

+ `setup[]` **initialize bridge infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - when called on the root object (`network@bridge`): registers all bridge instances from `arch@net.bridge`
    - when called on an instance (`bridge@lan`): creates the bridge netdev and registers with `network@frame`

+ `shut[]` **shut down bridge infrastructure**
    - succeed return ttrue
    - when called on the root object: unregisters and frees all bridge instances
    - when called on an instance: deletes from `network@frame` and removes the bridge netdev


#### Query APIs

+ `netdev[]` **get the bridge netdev name**
    - failed return NULL
    - succeed return [ string ], the Linux netdev name (e.g. "br-lan")

    Example, get netdev of bridge@lan
    ```shell
    bridge@lan.netdev
    br-lan
    ```

+ `list[]` **list all bridge instances or get one instance members**
    - failed return NULL
    - succeed return [ json ], bridge list or single instance members
    ```json
    {
        "bridge@lan":                       // [ string ]: { json }, bridge instance name
        {
            "member device":"",             // [ string ]: [ string ]
            // "...":"..."  How many members show how many properties
        }
        // "...":{}  How many bridges show how many properties
    }
    ```

    Example, list all bridge instances
    ```shell
    network@bridge.list
    {
        "bridge@lan":
        {
            "ethernet@lan2":"",
            "wifi@nssid":""
        }
    }
    ```

+ `bridge[ ifdev ]` **find which bridge an ifdev belongs to**
    - ifdev ------------ [ string ], the device component name
    - failed return NULL
    - succeed return [ string ], the bridge instance name (e.g. "bridge@lan")

    Example, find which bridge ethernet@lan2 belongs to
    ```shell
    network@bridge.bridge[ ethernet@lan2 ]
    bridge@lan
    ```

+ `status[]` **get bridge connection status**
    - failed return tfalse
    - succeed return [ json ], bridge status
    ```json
    {
        "status":"current status"           // [ "up", "down" ]
    }
    ```

    Example, get status of bridge@lan
    ```shell
    bridge@lan.status
    {
        "status":"up"
    }
    ```


#### Control APIs

+ `add[ ifdev ]` **add an interface to its bridge**
    - ifdev ------------ [ string ], the device component name to add
    - failed return tfalse
    - succeed return ttrue
    - finds the bridge that lists `ifdev` as a member, creates the bridge if needed, brings up the `ifdev`, connects it, and adds its netdev to the bridge

    Example, add ethernet@lan2 to its bridge
    ```shell
    network@bridge.add[ ethernet@lan2 ]
    ttrue
    ```

+ `delete[ ifdev ]` **remove an interface from its bridge**
    - ifdev ------------ [ string ], the device component name to remove
    - failed return tfalse
    - succeed return ttrue
    - finds the bridge that lists `ifdev` as a member, removes its netdev from the bridge, and brings down the `ifdev`

    Example, remove ethernet@lan2 from its bridge
    ```shell
    network@bridge.delete[ ethernet@lan2 ]
    ttrue
    ```

+ `setmac[ mac ]` **set MAC address on the bridge netdev**
    - mac -------------- [ string ], MAC address
    - failed return tfalse
    - succeed return ttrue

    Example, set MAC address on bridge@lan
    ```shell
    bridge@lan.setmac[ 02:50:F4:00:00:01 ]
    ttrue
    ```
