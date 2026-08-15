# Executable components (comexe)

An **executable component** is a standalone binary that the framework starts with `execl`. At runtime it is still a HE object: `PROJECT_ID@EXE_ID`. Configuration, methods, boot hooks, and joint handlers use the same grammar as any other component.

`prj add_com` and `prj add_exe` both generate this kind of program and register it under `"exe"` in `prj.json`. This page does not cover loadable `com` modules (`dlopen`).

Compared with a `cmd` / `osc` binary ([cmd.md](cmd.md)), a comexe exposes HE methods (`setup`, `shut`, `service`, …) through `MAIN2COM`. Compared with a shell component ([shell.md](shell.md)), the implementation is C.

Long-running single-instance daemons (Unix control socket, libevent) are described in [`single_object_comexe.md`](single_object_comexe.md). Service supervision: [`../com/land/service.md`](../com/land/service.md).

---

## libskin (land)

C components and programs link **libskin**, the shared library built from the **land** project (`libskin.so`, `-lskin`). It is the C side of HE: JSON values, configuration, calling other objects, logging, services, and project paths.

```c
#include "skin/skin.h"
```

The umbrella header pulls in talk/JSON (`talk_t`), objects (`obj_t`), parameters (`param_t`), config, register, service, and logging. After a host build, public headers are also under `doc/dev/include/skin/` and `build/install/include/skin/`. You do not need `project/land` C sources.

Typical calls in a comexe:

| Area | Symbols |
|------|---------|
| Config | `config_get`, `config_set` |
| Call another object | `scall` / `scalls` |
| Log | `app_info`, `app_warn`, `app_fault` |
| Service | `cstart`, `creset`, `sdelete`, `spid` |
| Paths | `osc2path`, `var2path` |
| Register (mmap) | `reg_set_int`, `reg_int`, `reg_set_string` |

Signatures, return sentinels (`ttrue` / `tfalse` / `terror` / `tpanic`), and samples: **[`../com/land/skin.md`](../com/land/skin.md)**.

---

## Generate with `prj`

From the SDK root (see [project.md](project.md)):

```bash
prj add_com myproj sensor
# or: prj add_exe myproj sensor
# component sensor introduction: <one-line description>
```

If `project/myproj` does not exist, `prj` creates it first (same as `prj create`).

Template: [`project/tmptools/comexe/`](../../project/tmptools/comexe/).

### What appears on disk

```text
project/myproj/
├── Makefile
├── prj.json
└── sensor/
    ├── sensor.c      # copied from comexe.c, renamed
    └── mconfig       # CFLAGS / LDFLAGS / LIBA_ADDIN / LIBSO_ADDIN
```

`prj.json` gains:

```json
"exe": {
    "sensor": "<the introduction you typed>"
}
```

Runtime object: `myproj@sensor`. Build macros in `sensor.c` (do not `#define` them yourself):

| Macro | Value |
|-------|--------|
| `PROJECT_ID` | `myproj` |
| `EXE_ID` / `COM_ID` | `sensor` (directory name) |
| `EXE_IDPATH` / `COM_IDPATH` | `myproj@sensor` |
| `gPLATFORM`, `HARDWARE`, `gCUSTOM`, … | Board from `gBOARDID` |

### `mconfig`

```make
CFLAGS += -I../
LDFLAGS +=
LIBA_ADDIN :=
LIBSO_ADDIN :=
```

Add what you need, for example `LDFLAGS += -levent -lskinnet`. The SDK already links libskin for exe targets.

---

## Implement common work

The generated `sensor.c` already has `_setup`, `_shut`, `_service`, `_online`, `_get`, `_set`, an `eapi_table_t`, and `MAIN2COM`. Keep the callbacks you need; stub or delete the rest. Method names in the table are the HE suffixes. **Do not** put `"get"` / `"set"` in the table — `MAIN2COM` wires those.

```c
static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "online",  (comapi_t)_online },
    { "service", (comapi_t)_service },
};

MAIN2COM( exe_api_table, _get, _set );
```

To add a method: implement `_myapi(obj_t this, param_t param)` and add one table row `{ "myapi", (comapi_t)_myapi }`.

### Configuration (`_get` / `_set`)

HE without a method reads and writes JSON through `_get` / `_set`:

```text
he 'myproj@sensor'
he 'myproj@sensor:status'
he 'myproj@sensor:status=enable'
he 'myproj@sensor|{"status":"enable","name":"s1"}'
```

The template saves with `config_set`, then restarts:

```c
ret = config_set( this, v, path );
if ( ret == true )
{
    _shut( this, NULL );
    _setup( this, NULL );
}
```

Drop the `_shut` / `_setup` pair if a save should not restart the service. Optional factory default: `project/myproj/sensor.cfg` at the project root.

### Bring-up and shutdown (`_setup` / `_shut`)

Register them on boot and shutdown ([init.md](init.md)):

```bash
prj add_init myproj app myproj@sensor.setup
prj add_uninit myproj app myproj@sensor.shut
```

A usual `_setup`: read config; if `status` is `"enable"`, start a supervised service. `_shut` stops it.

```c
cstart( this, "service", NULL, object );
/* ... */
sdelete( object );
```

`cstart` runs the `"service"` method (your `_service`) under `land@service`. If that process exits, the supervisor starts it again. `_service` should loop (or `pause()`) until killed; returning from `_service` is treated as process exit. That is expected for a supervised service — do not treat “cleanup after return” as a leak when the process is gone.

### Long-running work (`_service`)

Replace `pause()` with the real loop. Use `config_get` at start; use `reg_set_int` if other processes should watch a counter. Keep JSON handles you own and `talk_free` them before a long loop or before return.

### Joint handler (`_online`)

Register with `prj add_joint` ([joint.md](joint.md)). Finish quickly. Arguments:

- `param_string( param, 1 )` — event name (`network/online`, …)
- `param_talk( param, 2 )` — payload JSON (often `ifname`)

To restart the service on WAN up: `creset( this, "service", NULL, object )`.

### Call from HE and from C

```bash
he 'myproj@sensor.setup'
he 'myproj@sensor.shut'
he 'myproj@sensor.service'
he 'myproj@sensor.online'
```

From another program: `scall( "myproj@sensor", "setup", param )`. Details in [`../com/land/skin.md`](../com/land/skin.md) (component communication) and [`../com/land/he.md`](../com/land/he.md).

### Interface document

After the public HE surface exists, write English `<component>.md` next to the code. Layout: [`component_doc_guide.md`](component_doc_guide.md) (ground truth [`../com/land/auth.md`](../com/land/auth.md)). Do not document `get` / `set` as user APIs.

---

## Build and check

```bash
./mkdel
make obj=myproj
he 'myproj@sensor'
he 'myproj@sensor.setup'
```

On-device project path: `he 'land@fpk.path[ myproj ]'`.
