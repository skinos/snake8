## network@connect — Extern Uplink Scheduler

### Overview

Long-running service that schedules **extern** uplinks according to `network@frame` configure (`type`, slots `"1"`..`"N"`, delay rules, DNS/sticky options). It waits for the local LAN ifname to be up, then monitors link status and installs default route / DNS (and ECMP/shunts for `dbdc*`).

- object alias: `connect` → `network@connect` (see `prj.json` `obj`)
- scheduling types: `cold` / `hot` / `lazy` / `dbdc` (+ numbered variants); see [`frame.md`](frame.md)
- balance path caps at 6 nexthops (`BALANCE_MAX`)



### Network Architecture

`network@connect` is the runtime for multi-link policy configured on `network@frame`. WUI page `mix` edits `network@frame` and relies on this service to apply routing. It uses `network@frame.status` / extern status and may use `network@keeplive.lately` delay samples for dbdc/hot/lazy delay filtering.

For the full network architecture, see [`frame.md`](frame.md).



### Configuration reference

Scheduler policy is configured on **`network@frame`**, not on `network@connect`. See [`frame.md`](frame.md) configuration reference (`type`, `"1"`..`"6"`, `delay_*`, `dns`, `sticky`, …).



### API Reference

#### Management APIs

+ `setup[]` **start the connect service**
    - failed return tfalse
    - succeed return ttrue
    - may run optional `connect-setup.sh`, then `cstart(... service ...)`

+ `shut[]` **stop the connect service**
    - succeed return ttrue
    - stops the service process and shuts registered extern ifnames from frame extern list
    - **risk**: stops multi-uplink scheduling; WAN routing may drop until restarted

+ `reset[]` **restart the connect service**
    - failed return tfalse
    - succeed return ttrue

+ `flush[]` **signal running service (SIGHUP)**
    - failed return tfalse (service not running)
    - succeed return ttrue
    - used to reload policy without full stop

+ `service[]` **service main loop** (internal)
    - waits for local ifname up, then runs event loop for scheduling
    - note: HTTP `/he` may block API name `service` via WUI banlist

#### Query APIs

+ `status[]` **query scheduler status from running service**
    - failed return NULL (service not running)
    - succeed return [ json ], status payload from the live connect process

    Example
    ```shell
    network@connect.status
    ```
