# Boot and shutdown (`init` / `uninit`)

`init` and `uninit` in `prj.json` are **tables of HE calls**. At boot, `land@init` runs every project’s tasks level by level. At shutdown, `land@uninit` runs them in the reverse-oriented shutdown order. You do not start processes from the Makefile; you register `project@object.method` and implement that method.

This page is how to **register** those calls from a project. Runtime APIs (`register` / `list` / `call` / `knock`): [`../com/land/init.md`](../com/land/init.md) and [`../com/land/uninit.md`](../com/land/uninit.md). Implement the methods on a comexe ([comexe.md](comexe.md)) or a shell component ([shell.md](shell.md)).

---

## Generate with `prj`

From the SDK root:

```bash
prj add_init myproj app myproj@sensor.setup
prj add_uninit myproj app myproj@sensor.shut
```

Arguments:

| Argument | Meaning |
|----------|---------|
| project | Directory under `project/` |
| level | Boot or shutdown **level** name (not a number) |
| API | Full HE call, usually `project@object.setup` or `.shut` |

The command writes nested JSON. Values are descriptions; `prj` stores an empty string.

```json
"init": {
    "app": {
        "myproj@sensor.setup": ""
    }
},
"uninit": {
    "app": {
        "myproj@sensor.shut": ""
    }
}
```

The method must exist on the object (`_setup` / `_shut` in C, or `setup` / `shut` in a shell component). Registering a missing method fails at boot with a bad call.

---

## Boot levels (`init`)

`land@init.call` runs levels in this order:

`arch` → `land` → `bus` → `device` → `network` → `manage` → `local` → `extern` → `app` → `app2` → `general` → `delay` … `delay5`

| Level | Typical content |
|-------|-----------------|
| `arch` / `land` | Platform and core (not ordinary apps) |
| `network` / `manage` | Interfaces, VPN / management daemons |
| `app` | Application `.setup` (default for a new project) |
| `app2` / `general` | Work that must wait until apps are up |
| `delay` … `delay5` | Deferred start (about 1–5 seconds) |

Pick the same band as similar projects. A sensor or protocol app almost always uses `app`. Something that needs WAN addresses may use `extern` or a [joint](joint.md) instead of a late init level.

---

## Shutdown levels (`uninit`)

Shutdown order is the other way around:

`delay5` … `delay` → `general` → `app2` → `app` → `extern` → `local` → `manage` → `network` → `device` → `bus` → `land` → `arch`

Register `.shut` at the **same conceptual layer** as `.setup` (for example both `app`) so the service stops with its peers.

---

## Implement `.setup` and `.shut`

C (comexe template):

- `_setup` — read `config_get`; if enabled, `cstart( this, "service", NULL, object )`.
- `_shut` — `sdelete( object )` and return immediately.

Shell: `setup()` / `shut()` with `creturn ttrue`.

You can still call the methods by hand:

```bash
he 'myproj@sensor.setup'
he 'myproj@sensor.shut'
```

`init` is “the system calls this for you at this stage”. It is not a different API.

### Several calls at one level

```json
"init": {
    "app": {
        "myproj@sensor.setup": "",
        "myproj@glue.setup": ""
    }
}
```

Order among keys at the **same** level is not a documented sort; do not rely on JSON key order for sequencing. If B must run after A, put B on a later level (`app2`, `delay`) or start B from A’s `_setup`.

### Runtime-only tasks

`land@init.register` adds a task that dies on reboot. `land@init.add` persists in configuration. Project authors normally use **`prj.json` only** so the FPK always reinstalls the same table.

---

## Check

After install:

```bash
he 'land@init.list'
he 'land@uninit.list'
```

Confirm your `myproj@sensor.setup` line appears under the level you chose. Then reboot or `land@init.call[ app ]` on a debug image only if you know that re-running app setup is safe.

Related: [`../com/land/service.md`](../com/land/service.md) for `cstart` / `sdelete`.
