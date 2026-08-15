# Shell components

A **shell component** is an HE object whose methods are functions in a shell script instead of C. The runtime still uses `project@name.setup`, configuration objects, `init`, and `joint`. Use it for short glue (iptables, calling other HE lines, file copies) when a full comexe is more than you need.

There is **no** `prj add_shell`. Copy the template by hand from [`project/tmptools/comshell`](../../project/tmptools/comshell).

For a C executable component, see [comexe.md](comexe.md). For a `main()` command that is not an HE object, see [cmd.md](cmd.md).

---

## Copy the template

From the SDK root, after `prj create myproj` (or any existing project):

```bash
mkdir -p project/myproj/glue
cp project/tmptools/comshell project/myproj/glue/glue
chmod +x project/myproj/glue/glue
```

The packed artifact must be **`directory-name/directory-name`**. The SDK copies `PKG_BUILD_DIR/<key>/<key>` to the FPK root. If the script is named anything else, the pack step skips it.

Register it under **`exe`** (not `cmd`). `land@fpk.register` treats `exe` keys as HE objects and starts them with `execl`, so a `#!/bin/bash` script works. A `cmd` key is only a `bin/` command and is not registered as `project@key`.

Edit `project/myproj/prj.json`:

```json
"exe": {
    "glue": "shell glue for myproj"
}
```

### Local Makefile

If the subdirectory has no Makefile, the SDK compiles it with `exe.makefile` / `com.makefile` (expects `.c` files and a linked binary). Add a Makefile that only ensures the script is executable:

```make
BINS := $(notdir $(CURDIR))

all:
	chmod +x $(BINS)

install:

clean distclean:

.PHONY: all install clean distclean
```

Tree:

```text
project/myproj/glue/
├── Makefile
└── glue          # the script (copy of comshell)
```

---

## What the template does

`comshell` is a bash script. It must keep these two lines:

```bash
. $cheader
# ... function definitions ...
cend
```

`$cheader` loads the HE shell helpers (`creturn`, parameter variables, logging through `he`). `cend` dispatches the requested method. Do not remove them.

Environment (set by the framework):

| Name | Meaning |
|------|---------|
| `PROJECT` | Project id |
| `COM` | Component directory name |
| `PARAM1`, `PARAM2`, … | HE method arguments |

### Methods in the template

| Function | HE | Role |
|----------|-----|------|
| `setup` | `myproj@glue.setup` | Bring-up |
| `shut` | `myproj@glue.shut` | Shutdown |
| `service` | `myproj@glue.service` | Long-running loop (supervisor restarts on exit) |
| `list` | `myproj@glue.list` | Example that returns JSON |

Each function ends with `creturn` and a status or payload:

```bash
setup()
{
    echo "the ${PROJECT}@${COM} setup has be called"
    he log.info[ "the ${PROJECT}@${COM} setup has be called" ]
    creturn ttrue
}
```

`creturn ttrue` / `tfalse` match HE sentinels. For JSON, pass a string:

```bash
list()
{
    ret='{"'$PARAM1'":"'$PARAM2'"}'
    creturn $ret
}
```

Call other HE lines with `he` the same way as in `ashy` (quote when the shell would eat `|`, `{`, `[`).

---

## Common work

### Boot and shutdown

```bash
prj add_init myproj app myproj@glue.setup
prj add_uninit myproj app myproj@glue.shut
```

Implement `setup` / `shut` so those calls do real work (enable a feature, start `service`, reverse it on shut).

### Supervised loop

`service` in the template increments a counter and logs forever. If you register it with `cstart` from C, or call `myproj@glue.service` as a `land@service` target, exiting the loop means the process exits and the supervisor starts it again. That is normal; there is nothing to free after process exit.

To start the script’s `service` from HE:

```bash
he 'myproj@glue.service'
```

Prefer scheduling it from `setup` only when the feature should run as a daemon.

### Joint events

Add a function named after the method you register, for example `online()`, then:

```bash
prj add_joint myproj network/online myproj@glue.online
```

Read `PARAM1` (event) and keep the function short. Joint dispatch is synchronous.

### Configuration

Shell components can still have JSON config as `myproj@glue` if the object is registered. From the script, query with `he`:

```bash
he ${PROJECT}@${COM}:status
```

Prefer a comexe when you need `config_get` / `_set` restart logic in-process ([comexe.md](comexe.md), [`../com/land/skin.md`](../com/land/skin.md)).

---

## Build and check

```bash
./mkdel
make obj=myproj
he 'myproj@glue.setup'
```

Pack details: [`../../project2fpk.md`](../../project2fpk.md). HE: [`../com/land/he.md`](../com/land/he.md).
