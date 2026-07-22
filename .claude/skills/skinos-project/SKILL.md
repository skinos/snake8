---
name: skinos-project
description: |
  Create and extend skinos projects under project/: new project from tmptools,
  components (com), executables (exe/cmd), libraries, Web UI pages, cn/en.json
  i18n, prj.json init/uninit/joint/wui registration, and libskin (land/skin) APIs
  (config_get, scall, cstart, MAIN2API, talk_t, …).
  Use when the user says "新建项目", "创建组件", "加一个exe", "加网页", "语言包",
  "开机启动", "关机", "注册joint", "prj.json", "照着 tmptools", "skin API",
  "libskin", "config_get", "scall", "comexe", "单实例", "MAIN2API", "_service",
  "unix_listen", or asks how to call land APIs without land source.
  After any new/changed com/exe API surface, also apply **skinos-component-doc**
  (English `<name>.md` per auth.md) — required, not optional.
  Do NOT use for board/sdk.config/rootfs customization (skinos-sdk) or device
  upgrade/deploy (device-upgrade).
---

# Skinos Project Authoring

Scaffold and register features under **`project/<name>/`**. Template tree: **`project/tmptools/`**.  
Deeper field semantics: `project.md`, `doc/com/land/prj.json.md`, `project/AGENTS.md`.

**libskin (from `land` FPK):** when implementing com/exe/cmd, follow  
[reference-skin-api.md](reference-skin-api.md) and the full book **`doc/com/land/skin.md`**.  
Include `#include "skin/skin.h"`, link `-lskin`. Headers live under `doc/dev/include/skin/` / `build/install/include/skin/` after build — **do not require `project/land` C sources**.

## Naming rules

| Item | Rule |
|------|------|
| Project dir | `project/<name>/` |
| `prj.json` → `name` | **Must equal** directory name |
| Component / exe / cmd / lib key | **Must equal** subdirectory name |
| Runtime object | `PROJECT_ID@KEY` (e.g. `ipsec@list`, `tmptools@testcom`) |
| HE method | `project@key.method` → C `_method` (e.g. `.setup` → `_setup`) |

## Template map (`project/tmptools/`)

| Path | Use as template for |
|------|---------------------|
| `prj.json` | Manifest skeleton (`com` / `init` / `joint` / `wui`) |
| `Makefile` | OpenWrt package wrapper — change `PROJECT_ID:=` |
| `component/component.c` | Minimal **com** (setup/shut/get/set) |
| `testcom/testcom.c` + `testcom.html` | Full **com** + service + WUI example |
| `comexe/comexe.c` | **exe** with `MAIN2API(exe_api_table)` |
| `testcmd/` | **cmd** (global shell command) |
| `library/` | **lib** (`lib<key>.so`) |
| `osc/` | Large/third-party **osc** tree |
| `kmodule/` | **ko** kernel module |
| `cn.json` / `en.json` | WUI i18n |
| `page.html` / `testcom.html` | WUI page markup + JS patterns |

`tmptools/prj.json` only **registers** some of these (e.g. `testcom`); extra dirs are **copy sources**.

## Create a new project

```bash
# 1) Copy template (or minimal empty dir + Makefile from tmptools)
cp -a project/tmptools project/myproj

# 2) Rename: keep only what you need; delete unused template dirs
# 3) Edit project/myproj/Makefile → PROJECT_ID:=myproj
# 4) Edit prj.json → "name":"myproj", intro/desc/version/author
# 5) Ensure every com/exe/cmd/lib key has a matching subdirectory
# 6) Build
make obj=myproj
# → build/store/myproj-<ver>-<hardware>.fpk
```

Constraints:

- Comments in new C code: **English** (repo convention)
- Do not invent APIs; follow `_setup` / `_shut` / `_get` / `_set` / `_service` patterns from templates
- Deploy: **device-upgrade** skill (`make obj=` → upload FPK)

## Create a component (`com`)

1. Copy `project/tmptools/component/` → `project/<proj>/<com_id>/`  
   (or `testcom/` if you need service + richer example)
2. Rename files/dirs to `<com_id>`; keep `mconfig` if needed
3. In `prj.json`:

```json
"com": {
  "mycom": "short description"
}
```

4. Implement callbacks (`#include "skin/skin.h"`):

| C symbol | HE | Typical role |
|----------|-----|--------------|
| `_setup` | `proj@mycom.setup` | Start / apply config (`cstart` service) |
| `_shut` | `proj@mycom.shut` | Stop (`sdelete`) |
| `_get` / `_set` | `proj@mycom` / `\|{…}` | Config |
| `_service` | via `cstart(..., "service", ...)` | Long-running (exiting process = exit OK for supervised service) |

5. Optional factory default: `project/<proj>/mycom.cfg` (JSON)
6. Register **init** / **uninit** / **joint** / **wui** as needed (below)
7. **Mandatory — interface doc:** follow **skinos-component-doc** skill; write/update
   English `<component-id>.md` matching `doc/com/land/auth.md` layout (APIs, config, joints).
   Do not finish the component task without this Markdown.
8. `make obj=<proj>`

## Create an executable (`exe` / comexe)

Standalone binary (`COM_FILE_EXECUTE`), not a `dlopen` com. Full single-instance
daemon patterns (libevent, Unix control socket, `cstart`/`flush`):  
**[reference-comexe.md](reference-comexe.md)**.

| | `com` | `exe` |
|--|-------|-------|
| Artifact | shared lib | own binary via `execl` |
| Entry | `_api` symbols | `MAIN2API(eapi_table)` → `main()` |
| Fit | light config/query | long-running `_service` |

1. Copy `project/tmptools/comexe/` → `project/<proj>/<exe_id>/`  
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

## Web page + language files (`wui`)

1. Add HTML at project root (e.g. `mycom.html`). Patterns: `testcom.html`  
   - Use `data-i18n="English Key"` on labels  
   - Talk to HE via existing WUI JS helpers (`he.exec`, form bind to `config` object)
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
- [ ] Every `com`/`exe`/`cmd`/`lib`/`osc`/`ko` key has a matching subdirectory  
- [ ] Every `init`/`uninit`/`joint` call target exists as `_method`  
- [ ] Every `wui.page` file exists; `lang` JSON keys cover `data-i18n`  
- [ ] `Makefile` `PROJECT_ID` matches  

## Build & verify

```bash
make obj=myproj
# Device: upload FPK (device-upgrade), then
# he 'myproj@mycom'
# he 'myproj@mycom.setup'
# fpk.list   # path may be /usr/share/skinos/myproj or /mnt/internal/skinos/myproj
```

## libskin API (land)

| Doc | Content |
|-----|---------|
| [reference-skin-api.md](reference-skin-api.md) | Handbook: types, config/service/scall, MAIN2API, paths |
| [reference-comexe.md](reference-comexe.md) | Single-instance `exe`: lifecycle, Unix socket, checklist |
| `doc/com/land/skin.md` | **Full** API + per-function samples (~3300 lines) |

When writing component code, **read `reference-skin-api.md` first**; for daemons open **`reference-comexe.md`**; use `skin.md` for exact signatures.

## Related skills

| Skill | Use for |
|-------|---------|
| **skinos-component-doc** | English `<name>.md` interface doc (required after com/exe) |
| **skinos-he** | Live device: eline / `he '…'` / `ashy` HE grammar |
| **skinos-sdk** | `gBOARDID`, `config/swrt5` board overlays |
| **device-upgrade** | Upload FPK / firmware, remote web `/he` |
