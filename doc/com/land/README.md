# landos Project Overview (Documentation Entry Point)

SDK-wide portal (build, develop, device management, packaging): **[`README.md`](../../README.md)** · domain catalog **[`projects.md`](../../projects.md)**.

`landos` is a component-based gateway/device management foundation. It breaks device capabilities into components (for example `land@machine`, `land@syslog`, `land@auth`), and manages them through unified **configuration** (JSON) and **method interfaces** (APIs). During system startup, the registered tasks from your project are executed for initialization/coordination/shutdown. In day-to-day use, users interact with components mainly via the `he` tool: query/modify configurations and call component methods to perform real business actions.

This document is the entry point for **land** core docs: overview, architecture notes, shared terms, how to start, and an index of English component docs under this directory.

---

## 1. What this project does

From the structure of `prj.json`, the project is organized as:
- A set of libraries/executables/drivers/components
- A set of system-level registration and boot tasks (`init` / `uninit` / `joint`)
- A management interface accessible via `he` (the `land@*` components + the `he` command)

The main abstraction is the **component**:
- Runtime id: `project@component` (e.g. `land@machine`)
- **JSON configuration** — query/update via `he`
- **Methods** — `project@com.method[args]`, returning `talk_t` (JSON / string / number, or sentinels `ttrue` / `tfalse` / `terror` / `tpanic` / `NULL`)

---

## 2. Architecture notes

### Config vs runtime cache

- **Configuration** (`config_*` / HE get-set) is the durable JSON for a component.
- **Register** (`land@register` / `reg_*`) holds shared mmap variables across processes.
- **init / joint / uninit** also keep a **runtime cache** of registered tasks (from `prj.json` + `register` / `add`). `call` executes the cache; reboot drops runtime-only `register` entries unless persisted with `add`.

### Boot and event lifecycle

| Object | Role |
|--------|------|
| `land@init` | Startup tasks by **boot level** (`arch` … `delay5`) |
| `land@joint` | Handlers for **joint events** (`network/online`, `storage/insert`, …) |
| `land@uninit` | Shutdown tasks by **shutdown level** |

Shared implementation: `project/land/init/init.c` (`obj` aliases in `prj.json`).

System-driven dispatch (see `init.md` / `joint.md` / `uninit.md`):

- `call[ level|event ]` — run all cached handlers for that level/event (`land.init` / publishers).
- `knock[ project ]` — run one project's `prj.json` init/uninit/joint section (`land@fpk` install/uninstall).

Boot level order (init):  
`arch → land → bus → device → network → manage → local → extern → app → app2 → general → delay … delay5`

### Packaging and daily entry points

- Projects ship as **FPK**; `land@fpk` installs into the project root and may `knock` init/uninit without reboot.
- Application code links **libskin** (`#include "skin/skin.h"`). Full C API: [`skin.md`](./skin.md).
- CLI: [`he.md`](./he.md) / interactive [`eline.md`](./eline.md) (then `ashy` for BusyBox shell).
- Process supervisor: `land@service` + `cstart` / `sdelete` from C.
- Gateway info / reboot: `land@machine`.

---

## 3. Terminology (quick)

| Term | Meaning |
|------|---------|
| **Component** | Named object `project@name` with config + callable methods |
| **HE** | Command language for get/set/call (tool: `he`, UI: `eline`) |
| **talk_t** | libskin JSON/value handle; `talk_free` when heap-owned |
| **ttrue / tfalse / terror / tpanic / tnull** | Sentinel returns (not freeable JSON); see `skin.md` §1.1 |
| **param_t** | Method argument list (`param_string` 1-based, `-1` = last) |
| **obj_t / attr_t** | Parsed object path and nested attribute path |
| **Configuration** | Per-component JSON (`config_get` / HE without method) |
| **Register** | Shared mmap variables (`land@register`, `reg_*`) |
| **FPK** | Installable project package (`land@fpk`) |
| **prj.json** | Project manifest: `com` / `exe` / `init` / `joint` / `uninit` / `wui` / … |
| **Boot level** | Stage name for `land@init.call` (e.g. `app`, `manage`) |
| **Joint event** | System-wide event string for `land@joint.call` |
| **com vs exe** | Shared-library component vs standalone `MAIN2API` binary |
| **scall / ccall** | Call another component from C |
| **cstart / sdelete** | Register/start or remove a supervised service |

**HE shapes:**

```text
land@machine                 # get full config
land@machine:name=gw1        # set one attribute
land@machine|{ "name":"gw1" }  # merge set
land@machine.status          # call method
land@machine.status[ eth0 ]  # call with params
```

**Return-code habit:** treat only pointers **greater than `tpanic`** as heap JSON that may need `talk_free`. Check `tfalse` / `terror` / `tpanic` before parsing.

---

## 4. How to use the project (from 0 to 1)

### 4.1 Installation / Deployment (FPK)

Package as `.fpk` following [`fpk.md`](./fpk.md), then install so the device gains the components.

### 4.2 Automatic execution after boot (init / joint / uninit)

At startup the system runs registered tasks by boot level and joint events:
- `init` — attach tasks at a boot stage
- `joint` — event-triggered (storage/hotplug/network, …)
- `uninit` — cleanup on exit

### 4.3 Manage the gateway with `he`

See Quick start in [`he.md`](./he.md), for example:
- `land@machine` (query)
- `land@machine:name=...` (modify)
- `land@machine.status` (call a method)

---

## 5. Documentation index

Recommended read order:

1. [`he.md`](./he.md) — `he` command format, quick start, config/method examples
2. [`eline.md`](./eline.md) — interactive terminal (`eline`)
3. [`daemon.md`](./daemon.md) — system daemon
4. [`fpk.md`](./fpk.md) — FPK packaging/install and `prj.json` overview
5. [`prj.json.md`](./prj.json.md) — project manifest fields in depth
6. [`skin.md`](./skin.md) — libskin C API (`-lskin`)
7. [`component.md`](./component.md) — component registration/management
8. [`machine.md`](./machine.md) — `land@machine`
9. [`auth.md`](./auth.md) — authentication and permissions
10. [`syslog.md`](./syslog.md) — system log
11. [`service.md`](./service.md) — service supervisor
12. [`register.md`](./register.md) — register variables
13. [`init.md`](./init.md) — startup tasks (`register` / `list` / `call` / `knock`)
14. [`joint.md`](./joint.md) — joint-event handlers
15. [`uninit.md`](./uninit.md) — shutdown tasks

SDK packaging (repo root): [`project.md`](../../../project.md), [`fpk.md`](../../../fpk.md), [`project2fpk.md`](../../../project2fpk.md), [`fpk2rootfs.md`](../../../fpk2rootfs.md). Field reference: [`prj.json.md`](./prj.json.md).

---

## 6. Suggested reading path

1. This README (architecture + terms above)
2. [`he.md`](./he.md) for command format and return types
3. Component docs for your feature (`machine` / `auth` / `syslog` / …)
4. [`init.md`](./init.md) / [`joint.md`](./joint.md) / [`uninit.md`](./uninit.md) for boot and events
5. [`skin.md`](./skin.md) when writing C com/exe/cmd
