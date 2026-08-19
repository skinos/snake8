## forward@main — Default Route Table Management

### Overview

Manage the system default route table (`ip route` in table `main`). Prefer the **Component API** below instead of editing raw configuration when possible.

- manages static routes in the main routing table (table 254)
- empty / absent `target` is skipped in `setup` and `on` (unlike `routes@*` which treats it as default)
    > for default route management, use `routes@default` instead



### Configuration reference ( forward@main )

```json
// Attributes introduction 
{
    "rule name":        // [ string ]: { json }, user can custom the rule name
    {
        "target":"destination network or host",            // [ ip address, network ]
        "mask":"destination netmask",                      // [ netmask ], optional; host default 255.255.255.255
        "gw":"gateway ip address",                         // [ ip address ], optional
        "metric":"route hop",                              // [ number ], optional
        "ifname":"output logical ifname"                   // [ "ifname@lan", "ifname@wan", "ifname@lte", ... ], optional
    }
    // "...":{ ... }  How many rule show how many properties
}
```

#### Configuration example

Example, show current all rule settings of the default route table

```shell
forward@main
{
    "myCustomRule1":
    {
        "target":"192.168.1.0",
        "mask":"255.255.255.0",
        "gw":"192.168.8.22",
        "metric":"2",
        "ifname":"ifname@wan"
    },
    "youCustomRule":
    {
        "gw":"192.168.9.22",
        "ifname":"ifname@lan"
    }
}
```

#### Configuration settings example

Example, merge set one route rule

```shell
forward@main|{"office1":{"target":"192.168.2.0","mask":"255.255.255.0","gw":"192.168.9.40","ifname":"ifname@lan"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **apply saved static routes into the main table**
    - succeed return ttrue
    - skips rules with empty / absent `target`
    - normally scheduled as **`init` → `app` → `forward@main.setup`**

+ `add[ name, [target], [mask], [gateway], [ifname], [metric] ]` **add route rule**
    - name ----------- [ string ], route rule name
    - target --------- [ network, ip address ], optional
    - mask ----------- [ network mask ], optional
    - gateway -------- [ ip address ], optional
    - ifname --------- [ "ifname@lan", "ifname@lte", ... ], optional
    - metric --------- [ number ], optional
    - succeed return ttrue
    - failed return tfalse

    Example, add a rule named office1, route 192.168.2.12 via 192.168.9.40 of LAN

    ```shell
    forward@main.add[ office1, 192.168.2.12, 255.255.255.0, 192.168.9.40, ifname@lan ]
    ttrue
    ```

    Example, add a rule named office2, route all traffic via 192.168.9.41 of LAN

    ```shell
    forward@main.add[ office2, , , 192.168.9.41, ifname@lan ]
    ttrue
    ```

+ `delete[ name ]` **delete route rule**
    - name ----------- [ string ], route rule name
    - succeed return ttrue
    - failed return tfalse

    Example, delete the route named office2

    ```shell
    forward@main.delete[ office2 ]
    ttrue
    ```

+ `on[]` **re-apply matching static routes after a network event**
    - succeed return ttrue
    - parameter **2** is a JSON object; when **`ifname`** is present, only rules tied to that logical ifname are reconsidered
    - skips rules with empty / absent `target`


#### Query APIs

+ `status[]` **get the current default route table**
    - failed return NULL
    - succeed return [ json ], current route rules from `/proc/net/route`
    - names match configure when possible; otherwise `~autoN`
    - **`ifname`** is mapped from kernel `dev` (netdev → logical ifname)
    - **`netdev`** / **`flags`** / **`ref`** / **`use`** / **`status`** are runtime-only
    ```json
    {
        "rule name":        // [ string ]: { json }, user custom name; system rules start with "~"
        {
            "target":"destination network or host",            // [ ip address, network ]
            "mask":"destination netmask",                      // [ netmask ]
            "gw":"gateway ip address",                         // [ ip address ]
            "ifname":"output logical ifname",                  // [ "ifname@lan", ... ], user-facing
            "netdev":"kernel device",                          // [ string ], readonly
            "flags":"route flags",                             // [ number ], readonly
            "metric":"route hop",                              // [ number ]
            "ref":"reference count",                           // [ number ], readonly
            "use":"use count",                                 // [ number ], readonly
            "status":"rule state"                              // [ "up", "down" ], readonly
        }
        // "...":{ ... }  How many rule show how many properties
    }
    ```

    Example, get the current route rules

    ```shell
    forward@main.status
    {
        "myCustomRoute1":
        {
            "target":"192.168.0.0",
            "mask":"255.255.255.0",
            "gw":"192.168.8.2",
            "ifname":"ifname@wan",
            "netdev":"eth0.2",
            "flags":1,
            "metric":2,
            "ref":0,
            "use":0,
            "status":"up"
        }
    }
    ```


### Published Joint Events

| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@main.on` |
| `network/onextern` | `forward@main.on` |
| `network/onvpn` | `forward@main.on` |
