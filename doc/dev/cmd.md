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

## Hello world

A `cmd` is not an HE object. After install, run the binary (often on `PATH` as `mycli`). From eline:

```shell
$ ashy
~ # mycli
myproj@mycli: running
```

If `mycli` is not on `PATH`, query the project root first, then run `bin/mycli`:

```shell
$ land@fpk.path[ myproj ]
```

Change `main()` to print a fixed greeting. The whole `mycli.c` looks like this (`#include` plus `main()` — a cmd has no API table):

```c
#include "skin/skin.h"

int main( int argc, const char **argv )
{
    (void)argc;
    (void)argv;

    app_info( "%s: hello world", EXE_IDPATH );
    printf( "%s: hello world\n", EXE_IDPATH );
    return 0;
}
```

Rebuild, reinstall the FPK, run `mycli` again — you should see `hello world`.

---

## Query and modify component configuration

`cmd` / `osc` have no `myproj@mycli` config of their own. They read and write **another** object's JSON (a comexe or shell component) through libskin. Pair this with [comexe.md](comexe.md) (`myproj@sensor` + `sensor.cfg`).

### Query (`config_get`)

Whole `mycli.c` that reads `myproj@sensor` and prints `name`:

```c
#include "skin/skin.h"

int main( int argc, const char **argv )
{
    obj_t objst;
    talk_t cfg;
    const char *ptr;

    (void)argc;
    (void)argv;

    objst = obj_create( "myproj@sensor" );
    cfg = config_get( objst, NULL );
    if ( cfg == NULL )
    {
        app_fault( "%s: no config for myproj@sensor", EXE_IDPATH );
        obj_free( objst );
        return -1;
    }
    ptr = json_string( cfg, "name" );
    if ( ptr == NULL )
    {
        ptr = "";
    }
    app_info( "%s: sensor name is %s", EXE_IDPATH, ptr );
    printf( "sensor name is %s\n", ptr );
    talk_free( cfg );
    obj_free( objst );
    return 0;
}
```

Check the same value in **eline** (no `he` prefix):

```shell
$ myproj@sensor
{
    "status": "enable",
    "name": "hello"
}

$ myproj@sensor:name
hello
```

`mycli` and eline must show the same `name`.

### Modify (`config_set`)

Whole `mycli.c` that writes `name` / `status`, then you confirm in eline:

```c
#include "skin/skin.h"

int main( int argc, const char **argv )
{
    obj_t objst;
    talk_t v;
    boole ret;

    (void)argc;
    (void)argv;

    objst = obj_create( "myproj@sensor" );
    v = json_create( NULL );
    json_set_string( v, "name", "fromcli" );
    json_set_string( v, "status", "enable" );
    ret = config_set( objst, v, NULL );
    talk_free( v );
    if ( ret != true )
    {
        app_fault( "%s: config_set failed", EXE_IDPATH );
        obj_free( objst );
        return -1;
    }
    obj_free( objst );
    return 0;
}
```

```shell
$ myproj@sensor:name
fromcli

$ myproj@sensor
{
    "status": "enable",
    "name": "fromcli"
}
```

You can also change config only in eline (`myproj@sensor:name=world`) and have `mycli` print the new value on the next run.

Compute `obj_create` / `config_get` / `config_set` into locals (do not nest them in one call). Free `talk_t` and `obj_t` you own before return.

---

## Implement common work

### A small CLI (`cmd`)

After FPK install, `fpk-install` links `cmd` keys into the global command directory. Discover the project root first (`land@fpk.path[ myproj ]` in eline); the binary also lives in that tree’s `bin/`.

Use `argc` / `argv` as a normal Linux program. Query and set another component as in the section above.

### Talk to a running service (testcmd pattern)

[`testcmd.c`](../../project/tmptools/testcmd/testcmd.c) shows a loop that:

1. Builds `obj_t` for another object (`PROJECT_ID"@testcom"`).
2. Reads a config key with `json_string`.
3. Publishes an mmap int with `reg_set_int` so the Web page / `_status` can read `reg_int`.

That is the usual split: **comexe** owns HE and `_service`; **cmd** is a helper process the service may `execl`.

### Find another binary in this FPK (`osc2path`)

From a component or from `main()`. Whole `mycli.c` that locates `mycli` in this FPK and replaces itself with it:

```c
#include "skin/skin.h"

int main( int argc, const char **argv )
{
    char path[PATH_MAX];
    const char *ptr;

    (void)argc;
    (void)argv;

    ptr = osc2path( path, sizeof(path), "mycli" );
    if ( ptr == NULL )
    {
        app_fault( "%s: cannot find mycli", EXE_IDPATH );
        return -1;
    }
    execl( path, "mycli", NULL );
    app_fault( "%s: execl failed", EXE_IDPATH );
    return -1;
}
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

```shell
$ land@fpk.path[ myproj ]
$ ashy
~ # mycli
```

HE from a cmd is optional; you can always invoke `he '…'` as a subprocess. Library details: [`../com/land/skin.md`](../com/land/skin.md).
