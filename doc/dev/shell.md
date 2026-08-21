# Shell components

A **shell component** is an HE object whose methods are functions in a shell script instead of C. The runtime still uses `project@name.setup`, configuration objects, `init`, and `joint`. Use it for short glue (iptables, calling other HE lines, file copies) when a full comexe is more than you need.

There is **no** `prj add_shell`. Copy the template by hand from [`project/tmptools/comshell`](../../project/tmptools/comshell).

Agent / pack rules (authoritative for agents): **`.claude/skills/skinos-project/reference-shell.md`**.

For a C executable component, see [comexe.md](comexe.md). For a `main()` command that is not an HE object, see [cmd.md](cmd.md).

---

## Preferred: flat script at project root

Put the script **next to** `prj.json`, same name as the `exe` key. Do **not** create a subdirectory by default.

```bash
cp project/tmptools/comshell project/myproj/glue
chmod +x project/myproj/glue
```

Register under **`exe`** (not `cmd`):

```json
"exe": {
    "glue": "shell glue for myproj"
}
```

Tree:

```text
project/myproj/
├── glue              # the script (copy of comshell), chmod +x, Unix LF
├── glue.cfg          # optional factory config
├── glue.md           # English API doc
├── Makefile
└── prj.json
```

`skin.mk` copies a file `exe` key into the FPK root without compiling. A subdirectory is only needed when you want a local Makefile or extra sources (see below).

`land@fpk.register` treats `exe` keys as HE objects and starts them with `execl`, so a `#!/bin/bash` script works. A `cmd` key is only a `bin/` command and is not registered as `project@key`.

---

## Alternate: subdirectory

```bash
mkdir -p project/myproj/glue
cp project/tmptools/comshell project/myproj/glue/glue
chmod +x project/myproj/glue/glue
```

The packed artifact must be **`directory-name/directory-name`**. The SDK installs `PKG_BUILD_DIR/<key>/<key>` to the FPK root.

If the subdirectory has no Makefile, the SDK applies `exe.makefile` (expects `.c`). Add a Makefile that only ensures the script is executable:

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
project/myproj/
├── glue.cfg
├── Makefile
├── prj.json
└── glue/
    ├── Makefile
    └── glue
```

### Default configuration (`glue.cfg`)

Same rule as a comexe: in the **project directory** (next to `prj.json`), create JSON named like the component.

```text
project/myproj/glue.cfg
```

```json
{
    "status": "enable",
    "name": "hello"
}
```

Root `*.cfg` is packed into the FPK. `myproj@glue` then has a config object you can query and set from eline, WUI, or from the script with `he`.

---

## Hello world

Install the FPK and open **eline** (`$ `). Type HE without `he`. After `ashy`, use `he 'myproj@glue.setup'`.

### Call `setup`

The template `setup` already echoes and logs. In eline:

```shell
$ myproj@glue.setup
ttrue
```

To greet from `glue.cfg`, edit the **whole script**. Keep `#!/bin/bash`, `. $cheader`, every method, and `cend`. Only `setup` needs the hello-world body.

```bash
#!/bin/bash
. $cheader

setup()
{
    name=$(env -u cpipe he ${PROJECT}@${COM}:name)
    echo "hello, ${name}"
    env -u cpipe he log.info[ "hello, ${name}" ]
    creturn ttrue
}

shut()
{
    echo "the ${PROJECT}@${COM} shut has be called"
    env -u cpipe he log.info[ "the ${PROJECT}@${COM} shut has be called" ]
    creturn ttrue
}

service()
{
    number=1
    while :
    do
        number=$[number+1]
        env -u cpipe he log.info[ "hello world ( $number ) times" ]
        sleep 1
    done
    creturn tfalse
}

list()
{
    ret='{"'$PARAM1'":"'$PARAM2'"}'
    creturn $ret
}

cend
```

**Nested `he`:** always `env -u cpipe he …` so nested calls do not write this component’s reply pipe; then one `creturn`.

Call `setup` again; you should see `hello, hello`.

### Query configuration

```shell
$ myproj@glue
{
    "status": "enable",
    "name": "hello"
}

$ myproj@glue:name
hello

$ myproj@glue:status
enable
```

If the query is empty, the FPK has no `glue.cfg` yet and nothing has been saved at runtime.

### Modify configuration

```shell
$ myproj@glue:name=world
ttrue

$ myproj@glue:name
world

$ myproj@glue|{"name":"glue1","status":"enable"}
ttrue

$ myproj@glue
{
    "status": "enable",
    "name": "glue1"
}
```

Then `myproj@glue.setup` should print `hello, glue1` if `setup` reads `:name` as above. The Web page for this object ([wui.md](wui.md)) uses the same JSON.

---

## What the template does

`comshell` is a bash script. The file always has this shape: shebang, `. $cheader`, one function per HE method, then `cend`. Do not remove `. $cheader` or `cend`. Use **Unix LF** line endings (CRLF breaks the script).

The hello-world listing above is that full file. `$cheader` loads the HE shell helpers (`creturn`, parameter variables, logging through `he`). `cend` dispatches the requested method.

Environment (set by the framework):

| Name | Meaning |
|------|---------|
| `PROJECT` | Project id |
| `COM` | Component directory / script name |
| `PARAM1`, `PARAM2`, … | HE method arguments |
| `API` | Method name being invoked |
| `cpipe` | Reply pipe fd |

| Function | HE | Role |
|----------|-----|------|
| `setup` | `myproj@glue.setup` | Bring-up |
| `shut` | `myproj@glue.shut` | Shutdown |
| `service` | `myproj@glue.service` | Long-running loop (supervisor restarts on exit) |
| `list` | `myproj@glue.list` | Example that returns JSON |

`creturn ttrue` / `tfalse` match HE sentinels. `list` already returns a JSON string from `PARAM1` / `PARAM2`. Call other HE lines with `env -u cpipe he …` (quote when the shell would eat `|`, `{`, `[`).

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

Prefer a comexe when you need `config_get` / `_set` restart logic in-process ([comexe.md](comexe.md), [`../com/land/skin.md`](../com/land/skin.md)).

---

## Build and check

```bash
./mkdel
make obj=myproj
```

On the device (eline):

```shell
$ myproj@glue
$ myproj@glue.setup
```

Pack details: [`../../project2fpk.md`](../../project2fpk.md). HE: [`../com/land/he.md`](../com/land/he.md). Agent skill: [`.claude/skills/skinos-project/reference-shell.md`](../../.claude/skills/skinos-project/reference-shell.md).
