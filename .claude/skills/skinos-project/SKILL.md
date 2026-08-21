---
name: skinos-project
description: |
  Create and extend skinos projects under project/, rice/, or config/<platform>/
  (incl. per-platform arch): new project from tmptools, components (com),
  executables (exe/cmd), shell components (bash HE via exe + flat script),
  libraries, Web UI pages, cn/en.json i18n, prj.json init/uninit/joint/wui
  registration, and libskin (land/skin) APIs (config_get, scall, cstart,
  MAIN2API, talk_t, …).
  Use when the user says "新建项目", "建一个项目", "在rice下建", "xxx项目",
  "在项目X下建组件", "给smtk2下的arch建组件", "创建组件", "加一个exe",
  "shell组件", "bash组件", "comshell", "加网页", "语言包", "开机启动",
  "关机", "注册joint", "prj.json", "照着 tmptools", "skin API", "libskin",
  "config_get", "scall", "comexe", "单实例", "MAIN2API", "_service",
  "unix_listen", or asks how to call land APIs without land source.
  Resolve user speech with Rule 0 (project homes + arch-per-platform) before
  scaffolding — default bare "建项目" → project/; "rice下" → rice/; platform
  + arch → config/<platform>/arch/.
  For HTML page JS / he.load / he.exec / prj.json wui details, also apply
  **skinos-wui** (mandatory before inventing he.* helpers).
  After any new/changed com/exe/shell API surface, also apply
  **skinos-component-doc** (English `<name>.md` per auth.md) — required.
  Do NOT use for board/sdk.config/rootfs customization (skinos-sdk) or device
  upgrade/deploy (device-upgrade).
---

# Skinos Project Authoring

Scaffold and register features in a **project home** (see Rule 0). Template tree: **`project/tmptools/`**.  
Deeper field semantics: `project.md`, `doc/com/land/prj.json.md`, `project/AGENTS.md`.

**libskin (from `land` FPK):** when implementing com/exe/cmd, follow  
[reference-skin-api.md](reference-skin-api.md) and the full book **`doc/com/land/skin.md`**.  
Include `#include "skin/skin.h"`, link `-lskin`. Headers live under `doc/dev/include/skin/` / `build/install/include/skin/` after build — **do not require `project/land` C sources**.

## Rule 0 — User speech → project path (mandatory)

When the user says **「项目」** / **「xxx 项目」**, they mean a Skinos package tree that
lives in **one** of these homes (not only `project/`):

| Home | Path | Typical contents |
|------|------|------------------|
| **project** | `~/snake8/project/<name>/` | Main product domains (`land`, `modem`, `uart`, …) |
| **rice** | `~/snake8/rice/<name>/` | Customer / overlay packages (`test`, OEM trees, …) |
| **config platform** | `~/snake8/config/<platform>/<name>/` | Platform-bound packages (`arch`, `pdriver`, `center`, …) |

`<platform>` is an SDK platform dir under `config/` (`slave`, `smtk2`, `smtk3`, `swrt5`, `srock`, …).

### `arch` is special

**`arch` is not a single tree.** Each platform has its own:

```text
config/slave/arch/
config/smtk2/arch/
config/smtk3/arch/
config/swrt5/arch/
config/srock/arch/
…
```

- 「**smtk2 下的 arch**」/「**给 smtk2 的 arch 建组件**」→ `config/smtk2/arch/`
- 「**arch 项目**」without a platform → use **active** `gPLATFORM` from `gBOARDID` / `make pidinfo`; if unclear, **ask** — do not pick a random platform’s `arch`

`arch` (and `land`) are **core base firmware**: device verify needs full **`.zz`**, not FPK hot-deploy (**device-upgrade**).

### Phrase → action map

| User says (examples) | Resolve to | Action |
|----------------------|------------|--------|
| 「**建一个项目 b**」/「新建项目 b」（无前缀） | `project/b/` | Create under **`project/`** (default) |
| 「**在 rice 下建一个 a**」/「rice 里加项目 a」 | `rice/a/` | Create under **`rice/`** |
| 「**在 smtk2 下建项目 x**」/「config/smtk2 下…」 | `config/smtk2/x/` | Create under that **platform** |
| 「**xxx 项目**」（指已有包） | search homes below | Locate existing `<name>`; do not invent path |
| 「**在项目 test 下建一个组件**」 | find project named `test` | Create **com** (or exe) **inside** that project |
| 「**给 smtk2 下的 arch 建组件**」 | `config/smtk2/arch/` | Create component under that **arch** |

### Locate an existing named project

When the user names a project **without** a home prefix (e.g. 「项目 test」「在 test 下…」):

1. Search for a directory named `<name>` that is a project (has `prj.json` / is a known package root) under, in order:
   - `project/<name>/`
   - `rice/<name>/`
   - `config/<platform>/<name>/` for **each** platform (especially **`arch`**)
2. **One hit** → use it.
3. **Several hits** (e.g. both `project/test` and `rice/test`) → **ask which home**; do not silently pick.
4. **Explicit prefix** in the sentence (`rice` / `project` / `smtk2` / `swrt5` …) → search **only** that home; do not wander.

Scaffolding steps below still apply; only the **target root** changes (`project/<n>`, `rice/<n>`, or `config/<platform>/<n>`). Template copy source remains **`project/tmptools/`**. Build with `make obj=<name>` (same package name). On **slave** host builds, `config/slave/dir.makefile` resolves `COMPILE_PROJECT`/`OBJ` under `project/`, then **`rice/`**, then platform. For host-only compile while the tree stays on another board, pass `gBOARDID=slave-…` on the make line (**skinos-board** Rule 0) — do not rewrite the `gBOARDID` file for a quick test.

## Naming rules

| Item | Rule |
|------|------|
| Project dir | `project/<name>/`, `rice/<name>/`, or `config/<platform>/<name>/` |
| `prj.json` → `name` | **Must equal** directory name |
| Component / exe / cmd / lib key | **Must equal** subdirectory name **or** (shell `exe` only) flat script filename at project root |
| Runtime object | `PROJECT_ID@KEY` (e.g. `ipsec@list`, `tmptools@testcom`, `ifname@netcap`) |
| HE method | `project@key.method` → C `_method` (e.g. `.setup` → `_setup`) |

## Template map (`project/tmptools/`)

| Path | Use as template for |
|------|---------------------|
| `prj.json` | Manifest skeleton (`com` / `init` / `joint` / `wui`) |
| `Makefile` | OpenWrt package wrapper — change `PROJECT_ID:=` |
| `component/component.c` | Minimal **com** (setup/shut/get/set) |
| `testcom/testcom.c` + `testcom.html` | Full **com** + service + WUI example |
| `comexe/comexe.c` | **exe** with `MAIN2API(exe_api_table)` |
| `comshell` | **shell component** template (bash HE; register under `exe`) |
| `testcmd/` | **cmd** (global shell command) |
| `library/` | **lib** (`lib<key>.so`) |
| `osc/` | Large/third-party **osc** tree |
| `kmodule/` | **ko** kernel module |
| `cn.json` / `en.json` | WUI i18n |
| `page.html` / `testcom.html` | WUI page markup + JS patterns |

`tmptools/prj.json` only **registers** some of these (e.g. `testcom`); extra dirs are **copy sources**.

## Create a new project

Resolve the target home with **Rule 0** first (`project/` default; `rice/` or `config/<platform>/` when named).

```bash
# 1) Copy template (or minimal empty dir + Makefile from tmptools)
cp -a project/tmptools project/myproj          # default: 「建一个项目 myproj」
# cp -a project/tmptools rice/myproj           # 「在 rice 下建 myproj」
# mkdir -p config/smtk2/myproj && cp -a …     # 「在 smtk2 下建…」— rare; usually extend arch/pdriver

# 2) Rename: keep only what you need; delete unused template dirs
# 3) Edit <home>/myproj/Makefile → PROJECT_ID:=myproj
# 4) Edit prj.json → "name":"myproj", intro/desc/version/author
# 5) Ensure every com/exe/cmd/lib key has a matching subdir (or flat shell script for exe)
# 6) Build
make obj=myproj
# → build/store/myproj-<ver>-<hardware>.fpk
```

Constraints:

- Comments in new C code: **English** (repo convention)
- Do not invent APIs; follow `_setup` / `_shut` / `_get` / `_set` / `_service` patterns from templates
- Deploy: **device-upgrade** skill — most apps: `make obj=` → upload FPK; **`land` / `arch` are core base firmware → full `.zz` only** (never FPK-hot-test those two)

## Create a component (`com`)

1. Resolve the parent project with **Rule 0** (e.g. 「项目 test」→ find `test`; 「smtk2 下的 arch」→ `config/smtk2/arch/`).
2. Copy `project/tmptools/component/` → `<project_root>/<com_id>/`  
   (or `testcom/` if you need service + richer example)
3. Rename files/dirs to `<com_id>`; keep `mconfig` if needed
4. In that project’s `prj.json`:

```json
"com": {
  "mycom": "short description"
}
```

5. Implement callbacks (`#include "skin/skin.h"`):

| C symbol | HE | Typical role |
|----------|-----|--------------|
| `_setup` | `proj@mycom.setup` | Start / apply config (`cstart` service) |
| `_shut` | `proj@mycom.shut` | Stop (`sdelete`) |
| `_get` / `_set` | `proj@mycom` / `\|{…}` | Config |
| `_service` | via `cstart(..., "service", ...)` | Long-running (exiting process = exit OK for supervised service) |

6. Optional factory default: `<project_root>/mycom.cfg` (JSON)
7. Register **init** / **uninit** / **joint** / **wui** as needed (below)
8. **Mandatory — interface doc:** follow **skinos-component-doc** skill; write/update
   English `<component-id>.md` matching `doc/com/land/auth.md` layout (APIs, config, joints).
   Do not finish the component task without this Markdown.
9. `make obj=<proj>`

## Create an executable (`exe` / comexe)

Standalone binary (`COM_FILE_EXECUTE`), not a `dlopen` com. Full single-instance
daemon patterns (libevent, Unix control socket, `cstart`/`flush`):  
**[reference-comexe.md](reference-comexe.md)**.

| | `com` | `exe` |
|--|-------|-------|
| Artifact | shared lib | own binary via `execl` |
| Entry | `_api` symbols | `MAIN2API(eapi_table)` → `main()` |
| Fit | light config/query | long-running `_service` |

1. Resolve parent project (**Rule 0**); copy `project/tmptools/comexe/` → `<project_root>/<exe_id>/`  
   (for a production-style daemon, also study `project/network/connect/`)
2. Register under **`exe`** (not `com`):

```json
"exe": {
  "myexe": "description"
}
```

3. Implement `_setup` / `_shut` / `_service` (+ optional `_flush` / `_status` client).  
   Typical daemon: `_setup` → `cstart(this, "service", NULL, obj_name(this))`;  
   single-instance also via `unix_listen` on `var2path(..., "%s.unix", COM_ID)`.
4. Keep `MAIN2API(exe_api_table)`; first table row is the default method.  
   Build macros: `EXE_ID`, `EXE_IDPATH` (`PROJECT_ID@EXE_ID`), often `COM_ID` / `COM_IDPATH`.
5. Optional `mconfig` for `LDFLAGS` (`-levent`, `-lskinnet`, …).
6. Same HE shape as com: `he 'proj@myexe.setup'`
7. **Mandatory — interface doc:** **skinos-component-doc** → English `<exe-id>.md`
8. `make obj=<proj>`

**`cmd`** (global CLI): copy `testcmd/`, register under `"cmd": { "mycli": "…" }` → installed to FPK `bin/`.

**`lib`**: copy `library/`, register `"lib": { "mylib": "…" }` → `libmylib.so`.

## Create a shell component (bash HE)

Bash script as an HE object (`COM_FILE_EXECUTE`). Full detail:
**[reference-shell.md](reference-shell.md)**.

**Default (preferred):** flat script at project root + `exe` in `prj.json`.  
**Do not** invent `<key>/<key>` + Makefile unless the user wants a subdirectory.

```bash
cp project/tmptools/comshell project/myproj/myshell
chmod +x project/myproj/myshell
# edit: keep #!/bin/bash, . $cheader, cend; Unix LF only
```

```json
"exe": {
  "myshell": "shell glue description"
}
```

| | C `exe` (comexe) | Shell `exe` |
|--|------------------|---------------|
| Source | `<key>/<key>.c` + `MAIN2API` | Flat `<key>` script (or `<key>/<key>`) |
| Register | `prj.json` → `exe` | same |
| Object | `proj@key` | same |
| Reply | C `talk2fd` | `creturn` / `cend` via `$cheader` (`land/api.sh`) |

Rules:

1. Register under **`exe`**, never **`cmd`**.
2. Nested `he` must use `env -u cpipe he …` before a final `creturn` (avoids reply-pipe pollution).
3. Optional factory: `<project_root>/<key>.cfg`.
4. **Mandatory — interface doc:** **skinos-component-doc** → English `<key>.md`.
5. Build: `./mkdel` → `make obj=<proj>`.

Not the same as **`wui@script`** (`script@filename` under internal dir).

## Web page + language files (`wui`)

**Full rules: skill `skinos-wui`** (read it before writing page JS).  
Templates: `project/tmptools/page.html`, `testcom.html`. Allowed APIs:
`project/wui/ace/api/he.js` — save/write is **`he.exec`**, never `he.save`.

1. Add HTML at project root (e.g. `mycom.html`). Copy `tmptools/page.html`.  
   - `data-i18n="English Key"` on labels  
   - Load: `he.load([obj])`; save: `he.exec([obj+"="+JSON.stringify(cfg)])`
2. Register in `prj.json`:

```json
"wui": {
  "mycom": {
    "en": "My Feature",
    "cn": "我的功能",
    "menu": "VPN",
    "page": "mycom.html",
    "config": "myproj@mycom",
    "lang": {
      "cn": "cn.json",
      "en": "en.json"
    }
  }
}
```

| Field | Meaning |
|-------|---------|
| `en` / `cn` | Menu title |
| `menu` | Optional parent menu group (see other projects) |
| `page` | HTML file in project root |
| `config` | Bound HE object `project@key` |
| `lang` | i18n JSON files |

3. **Language files** (`cn.json` / `en.json`): map **English UI string → translation**

```json
{
  "Service State": "服务状态",
  "Apply": "应用"
}
```

Keys must match `data-i18n="…"` (and shared strings like Refresh/Apply if used).  
`en.json` can be a partial override; `cn.json` holds Chinese.

## Register boot (`init`)

Levels run in order: **`arch → land → app → general → manage → delay`**.

```json
"init": {
  "app": {
    "myproj@mycom.setup": ""
  },
  "manage": {
    "myproj@list.setup": ""
  }
}
```

- Value is usually `""` (no extra args) or a small arg string per product docs
- Pick level like peer projects (e.g. ipsec uses `manage` for `ipsec@list.setup`)
- Method must exist on the component (`_setup`)

## Register shutdown (`uninit`)

Shutdown levels differ from init (see `doc/com/land/uninit.md`):  
`delay5`…`delay` → `general` → `app2` → `app` → … → `land` → `arch`.

Common project style (nested by level):

```json
"uninit": {
  "manage": {
    "myproj@mycom.shut": ""
  }
}
```

Implement `_shut` to stop services cleanly.

## Register events (`joint`)

```json
"joint": {
  "network/on": {
    "myproj@mycom.on": ""
  },
  "network/onextern": {
    "myproj@mycom.onextern": ""
  },
  "network/online": {
    "myproj@mycom.online": ""
  },
  "storage/insert": {
    "myproj@mycom.storage": ""
  }
}
```

- Event names are system-wide strings (`network/online`, …)
- Handler should finish quickly; read `param_string(param,1)` (event) and `param_talk(param,2)` (payload)
- See `doc/com/land/joint.md`

## Minimal `prj.json` checklist

```json
{
  "name": "myproj",
  "intro": "...",
  "desc": "...",
  "type": "root",
  "version": "1.0.0",
  "author": "...",
  "com": { "mycom": "..." },
  "init": { "app": { "myproj@mycom.setup": "" } },
  "uninit": { "app": { "myproj@mycom.shut": "" } },
  "joint": { },
  "wui": { }
}
```

Verify before build:

- [ ] `name` == directory name  
- [ ] Every `com`/`cmd`/`lib`/`osc`/`ko` key has a matching subdirectory  
- [ ] Every `exe` key has a matching subdirectory **or** a flat executable script at project root (shell component)  
- [ ] Every `init`/`uninit`/`joint` call target exists as `_method` (C) or bash function (shell)  
- [ ] Every `wui.page` file exists; `lang` JSON keys cover `data-i18n`  
- [ ] `Makefile` `PROJECT_ID` matches  
- [ ] Shell scripts: Unix LF, `chmod +x`, `. $cheader` + `cend` kept 

## Build & verify

```bash
make obj=myproj
# Device: upload FPK (device-upgrade), then
# he 'land@fpk.path[ myproj ]'   # ONLY way to get on-device project root (path is dynamic)
# he 'myproj@mycom'
# he 'myproj@mycom.setup'
```

### On-device project location (mandatory)

Installed project paths are **dynamic** (`gBOARDID` / `PROJECT_DIR`, image vs FPK overlay). Agents and docs must **not** hardcode `/usr/share/skinos/<name>`, `/mnt/internal/skinos/<name>`, or similar.

| Goal | Use |
|------|-----|
| Current project root on device | `land@fpk.path[ <prj.json name> ]` |
| Map of all installs | `land@fpk.list` (each `"path"`) |

`cmd` binaries live under that root’s `bin/` (and may be symlinked into a PATH dir — still discover via `path` first). See `doc/com/land/fpk.md`; deploy skill: **device-upgrade**.

## libskin API (land)

| Doc | Content |
|-----|---------|
| [reference-skin-api.md](reference-skin-api.md) | Handbook: types, config/service/scall, MAIN2API, paths |
| [reference-comexe.md](reference-comexe.md) | Single-instance `exe`: lifecycle, Unix socket, checklist |
| [reference-shell.md](reference-shell.md) | Bash shell components: flat `exe` script, `creturn`/`cpipe`, pack rules |
| `doc/com/land/skin.md` | **Full** API + per-function samples (~3300 lines) |

When writing component code, **read `reference-skin-api.md` first**; for daemons open **`reference-comexe.md`**; for bash HE objects open **`reference-shell.md`**; use `skin.md` for exact signatures.

## Related skills

| Skill | Use for |
|-------|---------|
| **skinos-wui** | Project HTML pages, `prj.json` wui, `he.load` / `he.exec` |
| **skinos-component-doc** | English `<name>.md` interface doc (required after com/exe/shell) |
| **skinos-he** | Live device: eline / `he '…'` / `ashy` HE grammar |
| **skinos-sdk** | `gBOARDID`, `config/swrt5` board overlays |
| **device-upgrade** | Upload FPK / firmware, remote web `/he` |
