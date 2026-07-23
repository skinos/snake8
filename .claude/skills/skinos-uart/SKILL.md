---
name: skinos-uart
description: |
  Add or extend UART protocol applications and drivers under project/uart/:
  uartdrv@ exe drivers (dtu/gnss/tui patterns), uart@frame / uart@tty instances,
  serial line settings, WUI uart.html, board tty*.cfg, make obj=uart.
  Use when the user says "加串口协议", "UART驱动", "DTU", "NMEA", "uartdrv@",
  "uart@tty", "串口对接设备", "复制 dtu/gnss", or asks how serial apps bind to ports.
  Do NOT use for cellular USB modems (skinos-modem), Telnet/SSH access (tui
  project), or generic project scaffolding alone (skinos-project).
---

# Skinos uart — serial ports and protocol drivers

Ground truth: **`project/uart/`**. Portal map: [`projects.md`](../../projects.md).  
Docs: [`doc/com/uart/`](../../doc/com/uart/) (`frame.md`, `tty.md`, `dtu.md`, `tui.md`).  
Generic authoring: **skinos-project**. Deploy: **device-upgrade**.

## When to use this project

| Task | Here? |
|------|-------|
| App that talks a **protocol on `/dev/tty*`** (DTU, NMEA, custom binary/ASCII) | **Yes** — new `exe` + `uartdrv@` |
| Bind which app runs on UART#N, baud, flow | **Yes** — `uart@tty*` config + board `tty*.cfg` |
| Cellular **USB modem** AT/netdev | **skinos-modem**, not uart |
| Telnet/SSH into eline over network | **`project/tui`**, not uart |
| GNSS as system objects beyond serial | Also see **`project/gnss`**; serial side still often `uartdrv@gnss` |

## Architecture

```text
uart@frame.setup
    → for each uart@tty … uart@tty8
         resolve ttydev / devcom / drvcom
         sstarts(drvcom, "service", instance, ttydev[, devcom])
              ↓
         uartdrv@dtu | @tui | @gnss | @myproto
              ↓
         /dev/ttyS*  or  usb@tty-* (hotplug via frame.add/delete)
```

| Piece | Type | Role |
|-------|------|------|
| **`uart@frame`** | `com` | Lifecycle: register instances, start/stop drivers, USB add/delete, joints for network reset |
| **`uart@tty`…`tty8`** | config instances | Per-port JSON (line + `drvcom` + driver subtree) |
| **`uartdrv@*`** | `exe` via `obj` | Protocol apps; must implement `_service` |
| **`skinuart`** | `lib` | `serial_open`, logging macros, DTR helpers |

## Existing drivers

| Alias | Dir | Use when |
|-------|-----|----------|
| `uartdrv@dtu` | `dtu/` | Serial ↔ TCP/UDP/MQTT, framing, TLS, multi-peer |
| `uartdrv@tui` | `tui/` | Canonical serial → `eline` |
| `uartdrv@gnss` | `gnss/` | NMEA parse, status IPC, optional sock broadcast |

`prj.json` pattern:

```json
"exe": { "dtu": "…", "tui": "…", "gnss": "…" },
"obj": {
  "uartdrv@dtu": "dtu",
  "uartdrv@tui": "tui",
  "uartdrv@gnss": "gnss"
}
```

## Add a new serial protocol driver

### 1. Choose template

| Goal | Copy |
|------|------|
| Protocol **parser** / device control over UART (lighter) | **`project/uart/gnss/`** |
| Bridge to **network**, timers, TLS, peers | **`project/uart/dtu/`** |
| Interactive **eline** on serial | **`tui/`** (or reuse as-is) |

```bash
cp -a project/uart/gnss project/uart/myproto
# rename myproto.c / symbols; keep mconfig (-levent -lskinuart)
```

### 2. Implement `_service` (+ optional IPC APIs)

Frame always starts:

```text
drvcom.service[ uart_object, ttydev, devcom? ]
```

Pattern:

```c
uart_object = param_string(param, 1);   /* e.g. uart@tty2 */
ttydev      = param_string(param, 2);
cfg = config_sget(uart_object, NULL);
/* driver subtree key == alias suffix: uartdrv@myproto → json key "myproto" */
driver_cfg = json_value(cfg, "myproto");
fd = serial_open(ttydev, cfg, …);
/* event loop / protocol … */
```

Export with **`MAIN2API`**: at least `{ "service", _service }`; add `status` / `reset` / … if needed (see dtu/gnss).

### 3. Register in `prj.json`

```json
"exe": { "myproto": "custom UART protocol" },
"obj": { "uartdrv@myproto": "myproto" }
```

### 4. Bind a port (board / runtime)

Board overlay example: `config/<platform>/…/config/uart/tty2.cfg`:

```json
{
  "speed": "115200",
  "drvcom": "uartdrv@myproto",
  "myproto": { "…protocol options…": "" }
}
```

Or set at runtime:

```text
he 'uart@tty2:drvcom=uartdrv@myproto'
he 'uart@tty2|{"myproto":{…}}'
he 'uart@tty2.setup'
```

Hotplug USB serial: `uart@frame.register[…]` / `add[devcom]` / `delete[devcom]` (see `frame.md`).

### 5. WUI (optional)

Shared page **`uart.html`**: menus `uart`…`uart8` pass `object=uart@ttyN`.  
Modes come from `@uartdrv`. Unknown drivers can use **custom key/value** panel; known ones may need a dedicated HTML section + `en.json`/`cn.json` labels.

### 6. Build & deploy

```bash
./mkdel
make obj=uart
# → build/store/uart-<ver>-<hw>.fpk
```

Deps include `libevent2` (and for dtu: mosquitto/openssl as needed).

### 7. Docs

Add/update English component doc under `doc/com/uart/` (or topic dir) via **skinos-component-doc**.

## Frame / tty APIs (short)

| Object | Useful APIs |
|--------|-------------|
| `uart@frame` | `setup` / `shut`, `register`, `unregister`, `add`, `delete`, `list` |
| `uart@ttyN` | line settings, `drvcom`, `ttydev`, `status`, `reset`, TLS `import_*` / `key` (for DTU TLS), `power` |
| `uartdrv@dtu` | `service`, `status`, `reset` |
| `uartdrv@gnss` | `service`, `status`, `tty_cmd` |
| `uartdrv@tui` | `service` only |

Boot: `prj.json` `init.general` → `uart@frame.setup`.  
Network-triggered reconnect: frame registers **joint** on `network/online` or `network/onextern` per instance `extern` field (runtime, not always in prj.json).

## Gotchas

- Config **subtree name** must match `uartdrv@` **suffix** (`uartdrv@gnss` → `"gnss"`).
- Do not invent a separate `uart@myproto` com for protocol apps — use **`uartdrv@` + tty binding**.
- `_service` must not return if it should keep running (daemon-style); process is supervised via `sstarts`.
- `project/uart` may be a **nested git repo** — edits live there.
- Docs may mention `uartdrv@sixents` (RTK); in-tree GNSS driver is **`uartdrv@gnss`** — align UI/docs when adding RTK.
- On-device package files: `land@fpk.path[ uart ]` (never hardcode `/usr/share/skinos/uart`).
