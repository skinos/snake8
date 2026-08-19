## forward@ttl — TTL Modification Management

### Overview

Manage outgoing IP packet TTL modification per LAN **ifname**. Prefer the **Component API** below instead of editing raw configuration when possible.

- supports fix, increment, and decrement TTL modes
- uses iptables mangle POSTROUTING



### Configuration reference ( forward@ttl )

```json
// Attributes introduction 
{
    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], TTL policy for this ifname
    {
        "mode":"TTL modify mode",          // [ "disable", "fix", "inc", "dec" ], "disable" turns TTL off; legacy "none" is treated as disable
        "fix":"output packet TTL",         // [ number ], valid when mode is "fix"
        "inc":"increase the TTL",          // [ number ], valid when mode is "inc"
        "dec":"reduce the TTL"             // [ number ], valid when mode is "dec"
    }
    // "...":{ ... }  How many ifname show how many properties
}
```

#### Configuration example

Example, show current all of ttl configure

```shell
forward@ttl
{
    "ifname@lan":
    {
        "mode":"fix",
        "fix":"99"
    }
}
```

#### Configuration settings example

Example, modify the TTL to 70

```shell
forward@ttl:ifname@lan/fix=70
ttrue
```

Example, merge TTL settings for one LAN

```shell
forward@ttl:ifname@lan|{"mode":"fix","fix":"64"}
ttrue
```



### API Reference

#### Management APIs

+ `on[]` **re-apply TTL policy after a LAN ifname comes up**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**; TTL settings for that ifname are applied again from saved configuration

+ `off[]` **remove TTL mangling for an ifname**
    - succeed return ttrue
    - parameter **2** carries **`ifname`**


### Published Joint Events

| Joint key | Method |
|-----------|--------|
| `network/on` | `forward@ttl.on` |
