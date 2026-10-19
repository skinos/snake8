# Shell components (bash HE objects)

A **shell component** is an HE object whose methods are **bash functions** in a script
(not C). Runtime shape matches com/exe: `project@name.method`, optional `.cfg`,
`init` / `joint` / `uninit`. Prefer for short glue (iptables, tcpdump, calling other
HE lines). Prefer a **comexe** when you need in-process `config_get` / restart logic.

Ground truth for pack: `config/<platform>/skin.mk` (`Build/Prepare/Default`,
`Build/Install/fpk`). Template: `project/tmptools/comshell`. Example:
`project/ifname/netcap` → `ifname@netcap`.

Human doc (may lag): `doc/dev/shell.md`.

---

## Preferred layout (flat file)

**Do not create a subdirectory** unless the user asks. Put the script at the
**project root**, same name as the `exe` key:

```text
project/myproj/
├── prj.json
├── Makefile
├── netcap          # #!/bin/bash script, chmod +x, Unix LF only
└── netcap.md       # English API doc (skinos-component-doc)
```

`prj.json`:

```json
"exe": {
  "netcap": "short description"
}
```

**Why this works** (`skin.mk`):

1. **Prepare** — for each `exe` key, if path `./<key>` exists (file **or** dir), copy into `PKG_BUILD_DIR`.
2. **Compile** — only runs when `PKG_BUILD_DIR/<key>` is a **directory**. A flat script is skipped (no `.c` build).
3. **Install/fpk** — if `PKG_BUILD_DIR/<key>` is a file → `INSTALL_BIN` to FPK root as `<key>`; if directory → install `<key>/<key>`.

`land@fpk.register` treats **`exe`** keys as HE objects (`COM_FILE_EXECUTE`, started with `execl`). **`cmd`** only installs under `bin/` and is **not** `project@key`.

Object name: **`PROJECT_ID@KEY`** (e.g. `ifname@netcap`).

---

## Alternate layout (subdirectory)

Only when you need a local Makefile or extra sources:

```text
project/myproj/glue/
├── Makefile    # chmod +x only — see below
└── glue        # script name MUST equal directory name
```

Without a local Makefile, the SDK applies `exe.makefile` (expects `.c` → link failure). Use:

```make
BINS := $(notdir $(CURDIR))

all:
	chmod +x $(BINS)

install:

clean distclean:

.PHONY: all install clean distclean
```

Packed artifact is still FPK root file `glue` (from `glue/glue`).

---

## Script skeleton (mandatory)

Copy from `project/tmptools/comshell`. **Keep** shebang, `. $cheader`, and `cend`.

```bash
#!/bin/bash
. $cheader

setup()
{
    creturn ttrue
}

shut()
{
    creturn ttrue
}

mymethod()
{
    creturn ttrue
}
```

| Piece | Role |
|-------|------|
| `. $cheader` | Loads header: HE one-shot → `land/api.sh`; `land@service` daemon → `land/serv.sh`. Both install EXIT trap + `creturn`/`cend`. |
| *(no trailing `cend` needed)* | After the script body, EXIT trap runs `cend`: `$API` → function, or panic/`EXIT_EFUNC`. Explicit `cend` still OK (once-only). |
| `creturn …` | HE (`api.sh`): reply on `$cpipe` then `exit`. Daemon (`serv.sh`): `exit` with EXIT_* only. |

**Method naming:** HE `project@key.method` → script function **`method()`** (same name, no extra prefix). Example: `land@service.start[..., service]` sets `API=service` and calls `service()`.

**Internal helpers:** name them with a leading `_` (e.g. `_cap`) so `api_list` / leading `.` do **not** list them (scanner only accepts names starting `a-z`).

**`get` / `set`:** optional. If absent, `cget`/`cset` get `tpanic` from `cend` and fall back to file config. `api_list` / `.` do **not** list them; `api_exist` / `?` **can** see them (`get()`/`set()` or ELF `_get`/`_set`).

Unix **LF** line endings only. CRLF breaks `. $cheader` and `name()`.

### Name collisions (`type $API` in `cend`)

Ash resolves **function → builtin → external**. Define the method function when you need that API.

| Name | Host risk | Notes |
|------|-----------|--------|
| `setup` / `shut` / `get` | Usually none | Missing → `tpanic` (config get/set then file fallback) |
| `set` | Shell **builtin** | `type set` succeeds even without `set()`; missing `set()` still ends without `creturn` → parent `tpanic` → file config |
| `service` | Often `/usr/sbin/service` | **Must** define `service()` if `land@service.start[..., service]` is used; function wins over the external binary |

**`land@service` short HE alias:** do **not** register bare `service` in `prj.json` `obj` (keeps `/tmp/he.shell` from claiming the word `service`). Call `land@service.…` explicitly.

**API discovery:** `.obj` / `api_list` omit `get`/`set`. `?obj.method` / `com_have` / `com_exist` use `api_exist` (scan file for `method()` / `_method`, **including** `get`/`set`). Optional filepath preferred over object. Helpers named `_…` are not listed by `api_list`.
---

## Environment (set by framework)

| Name | Meaning |
|------|---------|
| `PROJECT` | Project id |
| `COM` | Component / exe key (script basename) |
| `OBJECT` | Full object name |
| `API` | Method being invoked |
| `PARAM1`…`PARAMn` | HE arguments |
| `PARAM_SIZE` | Argument count |
| `cpipe` | Reply pipe fd (do not write casually) |
| `cheader` | Path to `api.sh` |

---

## Nested `he`

Call nested HE **directly** (no `env -u cpipe`):

```bash
netdev=$(he ${ifobj}.netdev 2>/dev/null)
he log.info[ "message here" ]
```

Then **one** `creturn …` for this method.

Treat HE failure strings as errors, e.g. empty / `tfalse*` / `terror*` / `tpanic*`.

---

## Optional factory config

`project/myproj/<key>.cfg` (JSON next to `prj.json`) packs with root `*.cfg`.
Query from script: `he ${PROJECT}@${COM}:name`.

---

## init / joint / uninit

Same as C components. Register method names that exist as bash functions:

```json
"init": { "app": { "myproj@netcap.setup": "" } }
```

---

## Docs and checklist

- Write English `<key>.md` (**skinos-component-doc**).
- `chmod +x` the script; verify `bash -n` and `file` shows no CRLF.
- Build: `./mkdel` → `make obj=<proj>` → FPK hot-deploy (**device-upgrade**), unless land/arch.

---

## Not the same as `wui@script`

`wui@script` copies user scripts under the project **internal** dir and registers
them as **`script@<filename>`** at runtime. That is a dynamic manager, **not** a
packaged `exe` shell component. Packaged shells use **`exe` + project root script**.
