## network@hosts — Hosts File Management

### Overview

Manage static hostname entries written into `/etc/hosts`. Configuration is a flat map of **hostname → IP**. At setup, the component also injects the local LAN IP paired with the system nodename when `local_ifname` is known, then merges configured entries into `/etc/hosts`.

- reads/writes component configure via standard `get` / `set`
- applies configure by rewriting `/etc/hosts` through `setup`
- disabled on `wrt` scope and `slave` platform (setup returns ttrue / set returns false)



### Network Architecture

`network@hosts` is initialized early via `land` init (`network@hosts.setup`). It does not participate in `network@frame` uplink scheduling. WUI page `hosts` edits this configure and calls apply/`setup`.

For the full network architecture, see [`frame.md`](frame.md).



### Configuration reference ( network@hosts )

Configuration is the hosts map itself (no nested subtree).

```json
// Attributes introduction
{
    "hostname or alias list":"ip address"   // [ string ]: [ ip address ]
                                            // key is one hostname, or space-separated aliases
                                            // value is IPv4 or IPv6 address
    // "...":"..."  How many hosts show how many properties
}
```

#### Configuration example

Default configure:
```shell
network@hosts
{
    "localhost":"127.0.0.1",
    "ip6-localhost ip6-loopback":"::1"
}
```

Example, add a LAN name
```shell
network@hosts|{"localhost":"127.0.0.1","ip6-localhost ip6-loopback":"::1","router":"192.168.8.1"}
```

Example, set one entry
```shell
network@hosts:router=192.168.8.1
```



### API Reference

#### Management APIs

+ `setup[]` **apply hosts configure to `/etc/hosts`**
    - failed return tfalse
    - succeed return ttrue
    - rebuilds hosts from configure (plus local nodename line when available) and merges into `/etc/hosts`
    - on `wrt` / `slave`, returns ttrue without changing hosts

    Example
    ```shell
    network@hosts.setup
    ttrue
    ```

Notes:
- `get` / `set` are the standard configure callbacks (not listed under `.network@hosts` API map). Changing configure via `set` automatically calls `setup`.
