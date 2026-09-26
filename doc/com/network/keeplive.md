## network@keeplive — Interface Keeplive Service

### Overview

Provide link liveliness checks for **ifname** uplinks (WAN / WISP / LTE, …). Configure lives under each ifname object as `keeplive{...}`; this component runs the check loops and optional one-shot ICMP probes.

- modes: `icmp`, `dns`, `recv`, `auto` (DNS then fall back to recv)
- records recent delay samples for multi-link delay rules (`network@frame` / `network@connect`)
- on sustained failure, calls the ifname `keepoff` action (`reboot` / `reset` / `redial`)

Detailed per-ifname attribute descriptions are in the ifname docs (e.g. [`../ifname/wisp.md`](../ifname/wisp.md), [`../ifname/lte.md`](../ifname/lte.md)).



### Network Architecture

`network@keeplive` is a helper used by extern/local ifname services after the link is up. It does not own interface configure. Multi-link scheduler (`network@connect`) may read delay samples via `lately`.

For the full network architecture, see [`frame.md`](frame.md).



### Configuration reference

Configure is stored on the **ifname** object, not on `network@keeplive`:

```json
// Attributes introduction ( under ifname@xxx:keeplive )
{
    "type":"keeplive mode",                 // [ "disable", "icmp", "dns", "recv", "auto" ]
    "action":"action when keeplive fails",  // [ "reboot", "reset", "redial" ]
    "icmp":
    {
        "dest": { "8.8.8.8":"", "1.1.1.1":"" },
        // interval / count / ... see ifname docs
    },
    "dns":  { /* ... */ },
    "recv": { /* ... */ },
    "auto": { /* ... */ }
}
```

Example, enable ICMP keeplive on WAN
```shell
ifname@wan:keeplive/type=icmp
```



### API Reference

#### Service APIs

+ `service[ ifname ]` **run keeplive loop for one ifname**
    - ifname ----------- [ string ], required, e.g. `ifname@wan`
    - failed return terror / tfalse
    - succeed return result of ifname `keepoff` after failure, or terror on bad params
    - requires ifname `netdev` and `keeplive` configure with a supported `type`
    - note: HTTP `/he` may block API name `service` via WUI banlist; use eline or remove ban for API tests

    Example
    ```shell
    network@keeplive.service[ ifname@wan ]
    ```

+ `lately[ ifname, count ]` **read recent delay samples (ms)**
    - ifname ----------- [ string ], required
    - count ------------ [ string/number ], optional, max samples to return
    - failed return NULL
    - succeed return [ json ], ordered delay samples as `"0":ms, "1":ms, ...`

    Example
    ```shell
    network@keeplive.lately[ ifname@wan, 10 ]
    ```

+ `clear[ ifname ]` **clear delay sample buffer**
    - ifname ----------- [ string ], required
    - failed return terror
    - succeed return ttrue

    Example
    ```shell
    network@keeplive.clear[ ifname@wan ]
    ttrue
    ```

#### Probe APIs

+ `icmp[ timeout, addr, ifname ]` **one-shot ICMP echo**
    - timeout ---------- [ string/number ], optional seconds (default 10)
    - addr ------------- [ string ], required, destination IP/host
    - ifname ----------- [ string ], optional; when set, bind to that ifname `netdev` / `tid`
    - failed return NULL
    - succeed return [ number ], RTT in ms

    Example
    ```shell
    network@keeplive.icmp[ 2, 8.8.8.8, ifname@wan ]
    ```

+ `micmp[ timeout, addr..., ifname ]` **multi-destination ICMP, first reply wins**
    - timeout ---------- [ string/number ], optional seconds (default 10)
    - addr... ---------- [ string ], one or more destinations
    - ifname ----------- [ string ], optional trailing ifname object (contains `@`)
    - failed return NULL
    - succeed return [ json ] `{ "ip":"...", "delay":ms }`

    Example
    ```shell
    network@keeplive.micmp[ 2, 8.8.8.8, 1.1.1.1, ifname@wan ]
    ```
