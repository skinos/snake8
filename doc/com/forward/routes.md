## forward@routes — Advanced Route Table Management

### Overview

One component manages advanced Linux routing tables in two layers. Prefer the **Component API** below instead of editing raw configuration when possible.

- **`forward@routes`**: infrastructure — create/list/delete table instances, broadcast `setup` / `on`
- **`routes@<table>`** (for example `routes@123`, `routes@default`): instance — manage static rules inside that one table (same idea as `forward@main`, but with `ip route … table <table>`)
    > Web UI **Custom Route** uses `forward@routes.list`, then operates each table through `routes@<table>`



### Concepts

| Object | Role | Typical calls |
|--------|------|----------------|
| `forward@routes` | infrastructure manager | `.list` `.add[tid]` `.delete[tid]` `.setup` `.on` |
| `routes@123` | rules for Linux table `123` | `.status` `.add[name,…]` `.delete[name]` `.setup` `.on` |
| `routes@default` | rules for Linux table `default` | same as above |

`obj_com(this)` is the table id when the call target is an instance (`123`, `default`, …). When the call target is `forward@routes`, `obj_com` is `routes` and the API runs in infrastructure mode.

How an instance appears:

1. `prj.json` **`obj`** maps it (for example `"routes@default":"routes"`), or
2. **`forward@routes.add[ tid ]`** writes empty config and `com_register`, or
3. config already exists under `routes/` (for example `routes/123.cfg`) and **`forward@routes.setup`** does `config_list` then `com_register`

`com_register` runs only in **`forward@routes.add`** and **`forward@routes.setup`**. **`list`** / **`on`** only walk already registered `routes@*` objects.



### Configuration reference ( routes@* )

Infrastructure `forward@routes` has no rule configuration. Each instance stores its own rules.

Configure with logical **`ifname`** only (same as `forward@main`). Do **not** put `netdev` in config; the component resolves `ifname` → kernel `dev` on apply.

```json
// Attributes introduction 
{
    "rule name":        // [ string ]: { json }, user can custom the rule name
    {
        "target":"destination network or host",            // [ ip address, network ], 0.0.0.0 for default
        "mask":"destination netmask",                      // [ netmask ], host default 255.255.255.255
        "gw":"gateway ip address",                         // [ ip address ], optional
        "metric":"route hop",                              // [ number ], optional
        "ifname":"output logical ifname",                  // [ "ifname@lan", "ifname@wan", ... ], optional
        "src":"preferred source address",                  // [ ip address ], optional, ip route src
        "type":"route type",                               // [ "unicast","blackhole","unreachable","prohibit","throw" ], default unicast
        "onlink":"force gateway on link",                  // [ "enable" ], optional
        "mtu":"path mtu",                                  // [ number ], optional
        "tos":"type of service",                           // [ number/hex ], optional
        "scope":"route scope"                              // [ "global","link","host" ], optional
    }
    // "...":{ ... }  How many rule show how many properties
}
```

#### Configuration example

Example, show current all rule settings of routes@123

```shell
routes@123
{
    "myCustomRule1":
    {
        "target":"192.168.1.0",
        "mask":"255.255.255.0",
        "gw":"192.168.8.22",
        "metric":"2",
        "ifname":"ifname@wan",
        "src":"10.1.100.1",
        "type":"unicast"
    },
    "dropBad":
    {
        "target":"198.51.100.0",
        "mask":"255.255.255.0",
        "type":"blackhole",
        "metric":"10"
    }
}
```

#### Configuration settings example

Example, merge set several route rules

```shell
routes@123|{"office1":{"target":"192.168.2.0","mask":"255.255.255.0","gw":"192.168.9.40","ifname":"ifname@lan"}}
ttrue
```



### API Reference

#### Management APIs

Infrastructure — call **`forward@routes`**:

+ `setup[]` **register configs under `routes/` and apply every instance**
    - succeed return ttrue
    - failed return tfalse
    - `config_list("routes")` then `com_register` each `routes@<tid>`, then `com_list` and call each instance `setup`
    - normally scheduled as **`init` → `app` → `forward@routes.setup`**

+ `add[ tid ]` **create an empty table instance**
    - tid ------------ [ string ], table id or name → object `routes@tid`
    - succeed return ttrue
    - failed return tfalse when tid is empty, reserved (`routes`), or config already exists
    - writes empty JSON config and `com_register` to `forward@routes`

    Example, create table 123

    ```shell
    forward@routes.add[ 123 ]
    ttrue
    ```

+ `delete[ tid ]` **delete a table instance**
    - tid ------------ [ string ], table id or name of `routes@tid`
    - succeed return ttrue
    - failed return tfalse
    - removes that table’s configured kernel routes, deletes config, `com_unregister`

    Example, delete table 123

    ```shell
    forward@routes.delete[ 123 ]
    ttrue
    ```

+ `on[]` **re-apply every registered instance after a network event**
    - succeed return ttrue
    - failed return tfalse
    - `com_list` only (no register); call each instance `on` with the same parameters
    - subscribed from **`network/on`**, **`network/onextern`**, **`network/onvpn`**

Instance — call **`routes@123`** / **`routes@default`** / …:

+ `setup[]` **apply saved static routes into this one table**
    - succeed return ttrue
    - failed return tfalse
    - also reached when **`forward@routes.setup`** dispatches here

+ `add[ name, [target], [mask], [gateway], [ifname], [metric], [src], [type], [onlink], [mtu] ]` **add one route rule**
    - name ----------- [ string ], route rule name
    - target --------- [ network, ip address ]
    - mask ----------- [ network mask ]
    - gateway -------- [ ip address ]
    - ifname --------- [ "ifname@lan", "ifname@lte", ... ], logical ifname (not netdev)
    - metric --------- [ number ]
    - src/type/onlink/mtu — optional flat args (HE allows **at most 10** parameters total)
    - **or** pass one JSON object as parameter **7** for extended fields (needed for `tos`/`scope`, and used by the Web UI):
      `add[ name, target, mask, gw, ifname, metric, {"src":"...","type":"...","onlink":"enable","mtu":"...","tos":"...","scope":"..."} ]`
    - succeed return ttrue
    - failed return tfalse
    - applied with `ip route add table <tid> … proto static`

    Example, add a rule named office1 on table 123

    ```shell
    routes@123.add[ office1, 192.168.2.12, 255.255.255.0, 192.168.9.40, ifname@lan ]
    ttrue
    ```

    Example, add with preferred source, mtu, tos and scope (JSON blob)

    ```shell
    routes@123.add[ office2, 198.51.100.0, 255.255.255.0, 10.0.0.1, ifname@wan, 20, {"src":"10.5.107.32","type":"unicast","mtu":"1400","tos":"16","scope":"link"} ]
    ttrue
    ```

    Example, add a rule named office3, make all address route to 192.168.9.41 of LAN

    ```shell
    routes@123.add[ office3, , , 192.168.9.41, ifname@lan ]
    ttrue
    ```

+ `delete[ name ]` **delete one route rule by name**
    - name ----------- [ string ], route rule name
    - succeed return ttrue
    - failed return tfalse

    Example, delete the route named office2

    ```shell
    routes@123.delete[ office2 ]
    ttrue
    ```

+ `on[]` **re-apply matching rules after a network event**
    - succeed return ttrue
    - failed return tfalse
    - parameter **2** is a JSON object; when **`ifname`** is present, only rules tied to that logical ifname are reconsidered
    - also reached when **`forward@routes.on`** dispatches here


#### Query APIs

Infrastructure — call **`forward@routes`**:

+ `list[]` **list registered table instances**
    - failed return NULL
    - succeed return [ json ], object name to table id map
    ```json
    {
        "routes@table": "table id or name"     // [ string ]: [ string ], registered instance and its table
        // "...":"..."  How many instance show how many properties
    }
    ```

    Example, list all registered advanced route tables

    ```shell
    forward@routes.list
    {
        "routes@default":"default",
        "routes@123":"123"
    }
    ```

Instance — call **`routes@123`** / **`routes@default`** / …:

+ `status[]` **get current routes of this table**
    - failed return NULL
    - succeed return [ json ], current route rules from `ip route show table <tid>`
    - names match configure when possible; otherwise `~autoN`
    - **`ifname`** is mapped from kernel `dev` (netdev→logical ifname), same as `forward@main`
    - **`netdev`** / **`proto`** / **`status`** are runtime-only (not stored in configure)
    ```json
    {
        "rule name":        // [ string ]: { json }, user custom name; system rules start with "~"
        {
            "target":"destination network or host",            // [ ip address, network ]
            "mask":"destination netmask",                      // [ netmask ]
            "gw":"gateway ip address",                         // [ ip address ]
            "ifname":"output logical ifname",                  // [ "ifname@lan", ... ], user-facing
            "netdev":"kernel device",                          // [ string ], readonly
            "metric":"route hop",                              // [ number ]
            "src":"preferred source",                          // [ ip ], when present
            "type":"route type",                               // [ string ]
            "onlink":"enable",                                 // when present
            "mtu":"path mtu",                                  // when present
            "tos":"tos",                                       // when present
            "scope":"scope",                                   // when present
            "proto":"route protocol",                          // [ "kernel","static","dhcp", ... ], readonly
            "status":"rule state"                              // [ "up", "down" ]
        }
        // "...":{ ... }  How many rule show how many properties
    }
    ```

    Example, get the current route rules of table 123

    ```shell
    routes@123.status
    {
        "myCustomRoute1":
        {
            "target":"192.168.0.0",
            "mask":"255.255.255.0",
            "gw":"192.168.8.2",
            "ifname":"ifname@wan",
            "netdev":"eth0.2",
            "metric":2,
            "type":"unicast",
            "proto":"static",
            "status":"up"
        }
    }
    ```
