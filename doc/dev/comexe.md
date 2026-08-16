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

### Default configuration (`sensor.cfg`)

Factory JSON lives in the **project directory** (same folder as `prj.json`), named like the component. For `exe` key `sensor`:

```text
project/myproj/sensor.cfg
```

```json
{
    "status": "enable",
    "name": "hello"
}
```

The pack step copies root `*.cfg` into the FPK. `config_get` for `myproj@sensor` starts from this file when the runtime store is still empty. Later HE / WUI writes go to the persistent store — do not edit that store by hand.

Do **not** put the file only under `sensor/`; a subdirectory `.cfg` is not collected as a project-root resource.

---

## Hello world

Build and install the FPK, then log in to **eline** (prompt `$ `). Type HE **directly** — no `he` prefix. After `ashy`, wrap the same line: `he 'myproj@sensor'`. Grammar: [`../com/land/he.md`](../com/land/he.md), [`../com/land/eline.md`](../com/land/eline.md).

### Call `setup`

The generated `_setup` already logs the object name. In eline:

```shell
$ myproj@sensor.setup
ttrue
```

To greet using the default `name` from `sensor.cfg`, edit **`sensor.c` as a whole file**. Keep the include, every API the table lists, `_get` / `_set`, and `MAIN2COM` — that is the file layout. Only `_setup` needs the hello-world body; the other functions can stay as short stubs.

```c
#include "skin/skin.h"

boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *name;
    const char *object;
    const char *shown;

    (void)param;
    object = obj_name( this );
    name = NULL;
    shown = "(no name)";
    cfg = config_get( this, NULL );
    if ( cfg != NULL )
    {
        name = json_string( cfg, "name" );
    }
    if ( name != NULL )
    {
        shown = name;
    }
    app_info( "%s: hello, %s", object, shown );
    printf( "%s: hello, %s\n", object, shown );
    if ( cfg != NULL )
    {
        talk_free( cfg );
    }
    return ttrue;
}

boole_t _shut( obj_t this, param_t param )
{
    const char *object;

    (void)param;
    object = obj_name( this );
    app_info( "%s: _shut", object );
    return ttrue;
}

boole_t _service( obj_t this, param_t param )
{
    const char *object;

    (void)param;
    object = obj_name( this );
    app_info( "%s: _service", object );
    pause();
    return tfalse;
}

boole_t _online( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    return ttrue;
}

talk_t _get( obj_t this, attr_t path )
{
    talk_t cfg;

    cfg = config_get( this, path );
    return cfg;
}

boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

    ret = config_set( this, v, path );
    return ret;
}

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "online",  (comapi_t)_online },
    { "service", (comapi_t)_service },
};

MAIN2COM( exe_api_table, _get, _set );
```

Call `setup` again; the log / console line should contain `hello, hello` (the `name` from `sensor.cfg`).

`MAIN2COM` supplies `main()`. Method names in the table are the HE suffixes. **Do not** put `"get"` / `"set"` in the table — they are the last two arguments of `MAIN2COM`. To add a method later: implement `_myapi` and add one table row.

### Query configuration

```shell
$ myproj@sensor
{
    "status": "enable",
    "name": "hello"
}

$ myproj@sensor:name
hello

$ myproj@sensor:status
enable
```

Empty return means there is no store yet and no packed `sensor.cfg` — add the `.cfg` and reinstall the FPK.

### Modify configuration

One attribute:

```shell
$ myproj@sensor:name=world
ttrue

$ myproj@sensor:name
world
```

Merge several fields (other keys stay):

```shell
$ myproj@sensor|{"name":"sensor1","status":"enable"}
ttrue

$ myproj@sensor
{
    "status": "enable",
    "name": "sensor1"
}
```

Replace the whole object:

```shell
$ myproj@sensor={"status":"disable","name":"off"}
ttrue
```

The template `_set` saves with `config_set` and then calls `_shut` / `_setup`. After a set, `setup` runs again with the new JSON — that is the hello-world round trip: **eline changes config, `_setup` reads it**.

---

## Implement common work

The hello-world `sensor.c` above is the file you keep editing. Change the body of one API at a time; leave the include, table, and `MAIN2COM` in place.

### Configuration (`_get` / `_set`)

HE without a method goes through `_get` / `_set`. To save and then restart the service, fill `_set` like this (rest of the file unchanged):

```c
#include "skin/skin.h"

boole_t _setup( obj_t this, param_t param )
{
    /* hello-world body, or start a service — see below */
    (void)this;
    (void)param;
    return ttrue;
}

boole_t _shut( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    return ttrue;
}

boole_t _service( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    pause();
    return tfalse;
}

boole_t _online( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    return ttrue;
}

talk_t _get( obj_t this, attr_t path )
{
    talk_t cfg;

    cfg = config_get( this, path );
    return cfg;
}

boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;
    const char *object;

    object = obj_name( this );
    ret = config_set( this, v, path );
    if ( ret == true )
    {
        app_info( "%s: _set saved, restarting", object );
        _shut( this, NULL );
        _setup( this, NULL );
    }
    return ret;
}

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "online",  (comapi_t)_online },
    { "service", (comapi_t)_service },
};

MAIN2COM( exe_api_table, _get, _set );
```

Drop the `_shut` / `_setup` pair inside `_set` if a save should not restart the service. Factory JSON is `sensor.cfg` next to `prj.json` (see above).

### Bring-up and shutdown (`_setup` / `_shut`)

Register them on boot and shutdown ([init.md](init.md)):

```bash
prj add_init myproj app myproj@sensor.setup
prj add_uninit myproj app myproj@sensor.shut
```

A usual `_setup`: read config; if `status` is `"enable"`, start a supervised service. `_shut` stops it. Same file layout; only those two bodies change:

```c
#include "skin/skin.h"

boole_t _setup( obj_t this, param_t param )
{
    talk_t cfg;
    const char *object;
    const char *status;

    (void)param;
    object = obj_name( this );
    cfg = config_get( this, NULL );
    if ( cfg == NULL )
    {
        return tfalse;
    }
    status = json_string( cfg, "status" );
    if ( status != NULL && strcmp( status, "enable" ) == 0 )
    {
        cstart( this, "service", NULL, object );
    }
    talk_free( cfg );
    return ttrue;
}

boole_t _shut( obj_t this, param_t param )
{
    const char *object;

    (void)param;
    object = obj_name( this );
    sdelete( object );
    return ttrue;
}

boole_t _service( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    pause();
    return tfalse;
}

boole_t _online( obj_t this, param_t param )
{
    (void)this;
    (void)param;
    return ttrue;
}

talk_t _get( obj_t this, attr_t path )
{
    talk_t cfg;

    cfg = config_get( this, path );
    return cfg;
}

boole _set( obj_t this, talk_t v, attr_t path )
{
    boole ret;

    ret = config_set( this, v, path );
    return ret;
}

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "online",  (comapi_t)_online },
    { "service", (comapi_t)_service },
};

MAIN2COM( exe_api_table, _get, _set );
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
```

On the device (eline):

```shell
$ myproj@sensor
$ myproj@sensor.setup
```

On-device project path: `land@fpk.path[ myproj ]`.
