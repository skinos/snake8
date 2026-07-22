# Single-instance executable (`exe` / comexe)

Patterns from the former `doc/dev/single_object_comexe.md`.  
Minimal scaffold: `project/tmptools/comexe/comexe.c`.  
Production-style example: `project/network/connect/` (libevent + Unix control socket).

After APIs exist, write English `<exe-id>.md` via **skinos-component-doc**.

---

## com vs exe

| | `com` (shared lib) | `exe` (comexe) |
|--|--------------------|----------------|
| Artifact | `.so` / `.com`, `dlopen` by daemon | Standalone binary, `execl` |
| Entry | Exported `_api` symbols | `MAIN2API(table)` → `main()` |
| Process | In-process / short call | Own process |
| Fit | Config/query, light logic | Long-running loop, complex state |

Runtime id: **`PROJECT_ID@EXE_ID`** (same HE shape as com).

---

## Layout & build

```
project/<proj>/
├── Makefile
├── prj.json          # "exe": { "<exe_id>": "…" }
└── <exe_id>/
    ├── <exe_id>.c    # APIs + eapi_table_t + MAIN2API
    └── mconfig       # optional CFLAGS/LDFLAGS
```

Build (e.g. `config/swrt5/exe.makefile`) injects:

| Macro | Meaning |
|-------|---------|
| `PROJECT_ID` | From parent Makefile |
| `EXE_ID` / `COM_ID` | Directory basename |
| `EXE_IDPATH` / `COM_IDPATH` | `"PROJECT_ID@dirname"` |
| `gPLATFORM`, `gHARDWARE`, … | Board macros |

`mconfig` example:

```makefile
CFLAGS +=
LDFLAGS += -levent -lskinnet
```

Common libs: `-levent`, `-lskinnet`, `-lcrypto -lssl`, …

---

## MAIN2API dispatch

```c
#include "skin/skin.h"

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
    { "status",  (comapi_t)_status },
    { "flush",   (comapi_t)_flush },
};
MAIN2API(exe_api_table);   /* table must be an array, not a pointer */
```

- First row is the **default** when no method name is given.
- Signature: `boole_t` / `talk_t` `_api(obj_t this, param_t param)`.
- Shell-spawned: `execute_object` / `execute_param` / `execute_api` / `execute_pipe`.
- CLI: `argv2he` → HE form `project@exe.method[params]`.

HE (always single-quote in shell):

```bash
he 'myproj@myexe.setup'
he 'myproj@myexe.status'
he 'myproj@myexe.flush'
```

---

## Lifecycle pattern (daemon)

| API | Role |
|-----|------|
| `_setup` | Optional misc script; **`cstart(this, "service", NULL, obj_name(this))`** — service manager will not start a second instance |
| `_shut` | **`sstop` / `sdelete`**; cleanup |
| `_service` | Long-running body (libevent / poll / …). Process exit ends the service — cleanup before return is enough |
| `_flush` | Optional: `spid(object)` + `kill(pid, SIGHUP)` to reload without a second process |
| `_status` | Often a **client** that talks to the running daemon (Unix socket), not a second daemon |

### Single-instance layers

1. **Service manager** — `cstart` registers one named service (PID tracked).
2. **Unix socket** — `var2path(..., "%s.unix", COM_ID)` + `unix_listen(..., SOCK_DGRAM)`; second bind → `EADDRINUSE`.

Control socket paths (typical):

```c
var2path(path, sizeof path, "%s.unix", COM_ID);
/* or */
project_var_path(path, sizeof path, PROJECT_ID, "%s.unix", EXE_ID);
```

### `_service` sketch (libevent)

1. Load config (`config_get`).
2. `event_base` + SIGINT/SIGTERM → `event_base_loopbreak`; ignore SIGPIPE.
3. Optional timer (`EV_TIMEOUT|EV_PERSIST`).
4. `unix_listen` + `EV_READ` handler (`recvfrom` → `string2json` → reply `talk2udp`).
5. `event_base_dispatch`; free events/base on exit; return `tfalse` (process exits).

Keep the full loop in the project’s own `.c` (see `network/connect` or the old guide’s template). Do not invent new helper types/functions unless the task asks for them — copy from `tmptools/comexe` or `connect`.

### Client call sketch

Other code / `_status`:

1. `stat` the `.unix` path — missing → daemon down (`tpanic` / `ENOENT`).
2. Build JSON `{ "cmd": "...", "v": ... }`.
3. `unix_connect` + `talk2udp` / `udp2talk` with timeout; `unlink` ephemeral local socket.

---

## Cross-component helpers (exe body)

```c
talk_t r = scall("other@com", "method", param);
scalls("arch@gpio", "action", "network/online,%s", COM_IDPATH);

talk_t cfg = config_get(this, NULL);
config_set(this, val, "attr");

const char *mode = reg_string(NULL, "network_mode");
reg_set_int("my_counter", 42);

app_info("%s: started", object);
network_warn("unix_listen failed");   /* if network log family fits */

project_var_path(path, sizeof path, PROJECT_ID, "myfile");
misc2path(path, sizeof path, "my-script.sh");
```

Use real log macros from `log.h` (`app_info` / `app_warn` / `app_fault`, not invented `app_error`).

---

## prj.json

```json
"exe": {
  "myexe": "description"
},
"init": {
  "app": {
    "myproj@myexe.setup": ""
  }
},
"joint": {
  "network/online": {
    "myproj@myexe.online": ""
  }
}
```

- Register under **`exe`**, not `com`.
- Joint handlers must stay **fast**; heavy work stays in `_service`.

---

## Checklist

1. Dir `project/<proj>/<exe_id>/` + `<exe_id>.c` + optional `mconfig`
2. `_setup` / `_shut` / `_service` (+ `_flush` / Unix client as needed)
3. `exe_api_table` + `MAIN2API`
4. `prj.json` → `exe` (+ `init` / `joint`)
5. **skinos-component-doc** → English interface `.md`
6. `make obj=<proj>` → test with `he 'proj@exe.setup'` / `.status`

## References

| Path | Role |
|------|------|
| `project/tmptools/comexe/` | Minimal MAIN2API template |
| `project/network/connect/` | Full single-instance daemon |
| [reference-skin-api.md](reference-skin-api.md) | `cstart`, `scall`, paths |
| `doc/com/land/skin.md` §5.6 | `MAIN2API` / `execute_*` |
