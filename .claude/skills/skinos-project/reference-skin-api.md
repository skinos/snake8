# libskin API — developer handbook (com / exe / cmd)

**Canonical full API book (every function + samples):**  
[`doc/com/land/skin.md`](../../../doc/com/land/skin.md)

**Public headers (source of prototypes):** installed as `skin/*.h`  
→ after build: `build/install/include/skin/`, `doc/dev/include/skin/`  
→ on device: linked via `-lskin` (`/usr/lib/libskin.so` typical)

When **`project/land` is FPK-only (no `.c`)**, develop against **headers + `skin.md`**. Do not require `project/land/skin/*.c`.

Scaffolding skill: [SKILL.md](SKILL.md).

---

## 1. What `land` FPK provides

| Deliverable | Role for app authors |
|-------------|----------------------|
| **`libskin.so`** (`lib` key `skin`) | Link all com/exe/cmd with `-lskin` (+ crypto/ssl/event as in board `makefile.config`) |
| **Headers** `skin.h`, `talk.h`, `com.h`, … | Compile-time API |
| **Components** `land@machine`, `land@init`, `land@joint`, `land@uninit`, `land@service`, `land@fpk`, `land@auth`, `land@component`, `land@register`, `land@syslog`, … | Call via `scall` / HE |
| **Commands** `he`, `daemon`, `eline` | Shell / HE entry |
| **Docs** `doc/com/land/*.md` | HE + component + **this API book** |

Always: `#include "skin/skin.h"`.

---

## 2. Include chain

```text
skin.h
├── stdhead.h          C/POSIX, sockets, pthread, libevent
├── skinhead.h         limits, *_COM names, paths, then:
│   ├── link.h talk.h param.h path.h utility.h
│   ├── config.h dbs.h project.h com.h register.h
│   ├── log.h he2com.h serv.h
└── skinapi.h          macros: joint_*, machine_*, fpk_*, …
```

---

## 3. Core types & return values

| Type | Meaning |
|------|---------|
| `talk_t` | JSON graph node (`json_t*`) — config, API results |
| `boole_t` | Same pointer-sized sentinels as `talk_t` for many APIs |
| `boole` | C int bool (`true`/`false`) for config_set etc. |
| `obj_t` | `project@component` object |
| `attr_t` | Slash attribute path `a/b/c` |
| `param_t` | Method arguments (HE `[a,b,{…}]`) |
| `com_t` | Open component handle |

**Sentinels** (cast pointers — not heap JSON):

| Symbol | Meaning (typical) |
|--------|-------------------|
| `tnull` / `NULL` | No payload / idle |
| `ttrue` | Success |
| `tfalse` | Failure |
| `terror` | Error |
| `tpanic` | Bad object/API/dispatch |

**Rule:** if return `> tpanic`, treat as JSON/`talk_t` and **`talk_free(result)`**.  
`json2string()` returns **`char*` → `free()`**, not `talk_free`.

---

## 4. Build macros (com / exe / cmd)

Provided by platform `com.makefile` / `exe.makefile` (do not `#define` yourself):

| Macro | com | exe/cmd | Value |
|-------|-----|---------|-------|
| `PROJECT_ID` | ✓ | ✓ | `prj.json` name |
| `COM_ID` / `COM_IDPATH` | ✓ | often | dir name / `proj@dir` |
| `EXE_ID` / `EXE_IDPATH` | | ✓ | dir name / `proj@dir` |
| `gPLATFORM`, `gHARDWARE`, `gCUSTOM`, `gSCOPE` | ✓ | ✓ | board |
| `gPLATFORM__*`, `gHARDWARE__*`, `gCUSTOM__*` | ✓ | ✓ | `#ifdef` switches |

---

## 5. Component (`com`) — required patterns

### Export naming

HE `myproj@mycom.setup` → C symbol **`_setup`**.

```c
#include "skin/skin.h"

boole_t _setup(obj_t this, param_t param);
boole_t _shut(obj_t this, param_t param);
talk_t  _get(obj_t this, attr_t path);
boole   _set(obj_t this, talk_t v, attr_t path);
boole_t _service(obj_t this, param_t param);  /* long-running */
```

Framework loads the `.so` and resolves `_` + method name (`COM_API_PREFIX`).

### Config in `_setup` / `_set`

```c
talk_t cfg = config_get(this, NULL);
const char *st = json_string(cfg, "status");
if (st && !strcmp(st, "enable"))
    cstart(this, "service", NULL, obj_name(this));
talk_free(cfg);
```

- File store: **`PROJECT_CFG_DIR`** = `/etc/config` (device) under `<project>/…`
- Path helper: `config_path(buf, sizeof buf, PROJECT_ID, "%s.crt", COM_ID);`
- Factory defaults may also live under `/usr/share/skinos/<project>/` (FPK `config/`)

### Service lifecycle

| Call | Meaning |
|------|---------|
| `cstart(this, "service", NULL, name)` | Start `_service` if not running |
| `creset(...)` / `sreset(...)` | Restart |
| `sdelete(name)` | Unregister/stop |
| `spid(name)` | PID |

`s*` APIs take `"project@com"` **format string**; `c*` take `obj_t`.

If `_service` exits, supervisor may restart it — that is normal for daemon-style services.

### Call other components

```c
talk_t r = scalls("land@machine", "status", "");
if (r > tpanic) { /* use json_* */ talk_free(r); }

talk_t ip = sgets("network@eth0", "ip");
boole ok = ssets("network@eth0", string2x("192.168.1.1"), "ip");
```

Families: `scall`/`scalls`/`scallt`/… and `ccall`/`ccalls`/… (obj_t).  
`sget`/`sset` use peer `_get`/`_set` or fall back to `config_*`.

### Logging

Prefer **`app_info` / `app_warn` / `app_fault`** (and `_debug` / `_verbose`) in application projects.  
Infrastructure macros: `land_*`, `network_*`, … (see `log.h` / skin.md §9).

### Registers (cross-process)

```c
reg_set_int(this, "loop", 1);
int n = reg_int(this, "loop");  /* check errno: 0 vs error */
reg_set_string(this, "name", "x");
```

---

## 6. Executable (`exe`) — `MAIN2API`

```c
#include "skin/skin.h"

boole_t _setup(obj_t this, param_t param) { … }
boole_t _shut(obj_t this, param_t param) { … }
boole_t _service(obj_t this, param_t param) { pause(); return tfalse; }

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
};
MAIN2API(exe_api_table);
```

- Framework starts the binary and passes context via `execute_object()` / `execute_param()` / `execute_api()` / `execute_pipe()`.
- Table **must be an array** (not a pointer) so `MAIN2API` can use `sizeof`.
- HE: `myproj@myexe.setup` same as com.
- Shell: `he 'myproj@myexe.setup'`

**`cmd`:** same C style; installed to global `bin/`; may also be invoked as CLI with argv→HE (`argv2he` path inside `MAIN2API`).

---

## 7. `talk_t` / JSON cheat sheet

| Task | API |
|------|-----|
| Parse | `string2json(s)` → `talk_free` |
| Emit string | `json2string(t)` → **`free`** |
| Get field | `json_string(t,"key")`, `json_number`, `json_json` |
| Set field | `json_set_string/number/json` |
| Scalar string value | `string2x("…")` |
| Dup / compare / print | `talk_dup`, `talk_equal`, `talk_print` |
| Nested path | `attr_get` / `attr_set` with `attr_t` or `attr_gets(t,"a/b")` |

Validate with `json_check` before `json_sync` / `json_patch` (invalid input may assert).

---

## 8. `param_t` cheat sheet

| Task | API |
|------|-----|
| Size | `param_size(p)` |
| Nth string (1-based) | `param_string(p, n)` |
| Nth JSON | `param_talk(p, n)` |
| Build | `param_create` / `param_adds` / `param_addt` |
| Free | `param_free` |

Joint handlers often: `param_string(param, 1)` = event name, `param_talk(param, 2)` = payload.

---

## 9. Important paths (`skinhead.h`)

| Macro | Typical device value |
|-------|----------------------|
| `PROJECT_DIR` | `/usr/share/skinos` |
| `PROJECT_CFG_DIR` | `/etc/config` |
| `PROJECT_DBS_DIR` | `/mnt/dbs` |
| `PROJECT_INT_DIR` / `PROJECT_APP_DIR` | `/mnt/internal`, `…/skinos` (hot FPK) |
| `PROJECT_TMP_DIR` | `/tmp` |
| `CONFIG_FILE_POSTFIX` | `.cfg` |

Well-known objects: `MACHINE_COM` (`land@machine`), `INIT_COM`, `JOINT_COM`, `UNINIT_COM`, `FPK_COM`, `SERVICE_COM`, … (full list in skin.md §16 / `skinhead.h`).

---

## 10. `skinapi.h` shortcuts

```c
machine_status("");           /* → land@machine.status */
machine_restart(3, "upgrade");
joint_register(event, call);
joint_calls(event, …);
fpk_list();
init_register(item, call);
```

---

## 11. Header index (what each file is for)

| Header | Use when you need |
|--------|-------------------|
| `talk.h` / `json.h` | JSON values |
| `param.h` | Method args |
| `path.h` | `obj_t` / `attr_t` |
| `config.h` | Persistent component config files |
| `dbs.h` | Component DB layer (`dbs_fetch`/`dbs_save`) |
| `com.h` | `scall`/`cget`/`MAIN2API`/open component |
| `serv.h` | `cstart`/`sdelete`/service list |
| `register.h` | Shared mmap registers |
| `log.h` | `app_*` / `landlog` |
| `he2com.h` | Parse/run HE from C |
| `project.h` | Project paths, prj.json helpers, i18n |
| `utility.h` | shell, files, net, base64, UART, … |
| `link.h` | Intrusive lists |
| `skinapi.h` | System shortcuts |

---

## 12. Memory & safety checklist

1. `talk_free` for talk/json from libskin getters/calls (when `> tpanic`).
2. `free` for `json2string` and many `utility` allocators.
3. Do not `talk_free` pointers that are **views** into a parent JSON (`attr_get` notes).
4. `param_import` transfers ownership — do not double-free.
5. Assume **not thread-safe** unless documented; use `register_lock*` for registers.
6. After `scall*`, handle `tpanic`/`terror`/`tfalse` before treating as JSON.
7. Comments in new code: **English**.

---

## 13. Where to read next

| Need | Document |
|------|----------|
| Full signatures + sample per function | `doc/com/land/skin.md` |
| HE line grammar | `doc/com/land/he.md` |
| Component model | `doc/com/land/component.md` |
| New project/com/wui | [SKILL.md](SKILL.md) |
| Single-instance exe / comexe | [reference-comexe.md](reference-comexe.md) |
| Board / rootfs | `skinos-sdk` skill |

Keep **`skin.md` + public headers** in any SDK release that omits `land` sources.
