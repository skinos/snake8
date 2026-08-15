# Commands and bundled programs (`cmd` / `osc`)

A **command** (`cmd`) and a **bundled program** (`osc`) are ordinary C programs with `main()`. They are **not** HE objects. The framework does not call `_setup` on them. You start them from a shell, from another component (`execl` / `osc2path`), or from an init line that names a program path.

| | `cmd` | `osc` |
|--|-------|-------|
| `prj` | `add_cmd` | `add_osc` |
| `prj.json` | `"cmd"` | `"osc"` |
| FPK location | `bin/<key>` | package root `<key>` |
| Typical use | Operator / script CLI on `PATH` after install | Helper or vendored tree kept in the same FPK |

Both commands copy the same template: [`project/tmptools/osc/`](../../project/tmptools/osc/) (`main.c` renamed to `<key>.c`). A richer example that talks to another component is [`project/tmptools/testcmd/testcmd.c`](../../project/tmptools/testcmd/testcmd.c).

For an HE object with methods, use [comexe.md](comexe.md) (`prj add_com` / `add_exe`).

---

## libskin (land)

Even a `main()` program can use **libskin**, the shared library from the **land** project (`libskin.so`, `-lskin`). Include the umbrella header and call the same APIs a component uses: read another object’s JSON, log, look up files in this FPK, share mmap registers.

```c
#include "skin/skin.h"
```

After a host build, headers are also under `doc/dev/include/skin/` and `build/install/include/skin/`. You do not need `project/land` C sources.

| Area | Symbols useful from `main()` |
|------|------------------------------|
| Object | `obj_create`, `obj_free`, `obj_name` |
| Config | `config_get`, `config_set`, `json_string`, `json_number` |
| Call HE methods | `scall` / `scalls` |
| Log | `app_info`, `app_warn`, `app_fault` |
| Paths in this project | `osc2path`, `var2path` |
| Register | `reg_set_int`, `reg_int` |

Full signatures and samples: **[`../com/land/skin.md`](../com/land/skin.md)**.

Build macros still apply (`PROJECT_ID`, `EXE_ID`, `EXE_IDPATH`, `gPLATFORM`, …). The osc template logs with `EXE_IDPATH`.

---

## Generate with `prj`

From the SDK root (see [project.md](project.md)):

```bash
prj add_cmd myproj mycli
# command program mycli introduction: <one line>

prj add_osc myproj helper
# bundled program helper introduction: <one line>
```

Missing `project/myproj` is created first.

### What appears on disk

```text
project/myproj/mycli/
├── mycli.c       # from osc/main.c
└── mconfig

project/myproj/helper/
├── helper.c
└── mconfig
```

`prj.json`:

```json
"cmd": {
    "mycli": "<introduction>"
},
"osc": {
    "helper": "<introduction>"
}
```

`mconfig` is the same shape as comexe (`CFLAGS`, `LDFLAGS`, `LIBA_ADDIN`, `LIBSO_ADDIN`). Extra `-I` / `-l` go there.

The generated `main()` only logs and returns 0. Replace the body; keep `#include "skin/skin.h"` if you use libskin.

---

## Implement common work

### A small CLI (`cmd`)

After FPK install, `fpk-install` links `cmd` keys into the global command directory. Discover the project root first (`he 'land@fpk.path[ myproj ]'`); the binary also lives in that tree’s `bin/`.

Use `argc` / `argv` as a normal Linux program. Call into components when you need device state:

```c
objst = obj_create( "myproj@sensor" );
cfg = config_get( objst, NULL );
if ( cfg == NULL )
{
    app_fault( "%s: no config for myproj@sensor", EXE_IDPATH );
    obj_free( objst );
    return -1;
}
ptr = json_string( cfg, "name" );
talk_free( cfg );
obj_free( objst );
```

Compute `obj_create` / `config_get` into locals (do not nest them in one call). Free `talk_t` and `obj_t` you own before return.

### Talk to a running service (testcmd pattern)

[`testcmd.c`](../../project/tmptools/testcmd/testcmd.c) shows a loop that:

1. Builds `obj_t` for another object (`PROJECT_ID"@testcom"`).
2. Reads a config key with `json_string`.
3. Publishes an mmap int with `reg_set_int` so the Web page / `_status` can read `reg_int`.

That is the usual split: **comexe** owns HE and `_service`; **cmd** is a helper process the service may `execl`.

### Find another binary in this FPK (`osc2path`)

From a component or from `main()`:

```c
char path[PATH_MAX];
const char *ptr;

ptr = osc2path( path, sizeof(path), "mycli" );
if ( ptr == NULL )
{
    app_fault( "%s: cannot find mycli", EXE_IDPATH );
    return -1;
}
execl( path, "mycli", NULL );
```

`osc2path` resolves a file inside the current project install (cmd, osc, or other packed name). Do not hardcode `/usr/share/skinos/…`.

### When to choose `osc`

Use `"osc"` when the program should sit next to components at the FPK root (same class as `exe` for packing) but does **not** need HE methods. Large upstream trees belong here: keep their own Makefile in the subdirectory; `prj add_osc` only drops a one-file `main()` plus `mconfig`.

Init can still run a program by path if you put a program string in `init` instead of `project@object.method`. Prefer a comexe `.setup` that `execl`s the osc binary so boot stays in HE.

---

## Build and check

```bash
./mkdel
make obj=myproj
```

On the device, after install:

```bash
he 'land@fpk.path[ myproj ]'
# then run bin/mycli or the root helper from that directory
```

HE from a cmd is optional; you can always invoke `he '…'` as a subprocess. Library details: [`../com/land/skin.md`](../com/land/skin.md).
