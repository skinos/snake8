# Skinos project — on-disk parts

What a **Skinos project** is: the tree under `project/<name>/`, the **parts** that live as files/directories, and when to choose each kind.

| Doc | Owns |
|-----|------|
| **This file** | On-disk parts, when to use which, grow-a-project walkthrough |
| [`project/land/prj.json.md`](project/land/prj.json.md) | `prj.json` field shapes, semantics, format reference |
| [`project2fpk.md`](project2fpk.md) | Project tree → `.fpk` (`skin.mk` pack pipeline) |
| [`fpk2rootfs.md`](fpk2rootfs.md) | `.fpk` → rootfs (`fpk-install`) |
| [`fpk.md`](fpk.md) | `.fpk` archive overview |
| [`doc/com/land/fpk.md`](doc/com/land/fpk.md) | `land@fpk` runtime APIs |

---

## Terms

- **Skinos project**: a tree under `project/<name>/` whose root has a project manifest (`prj.json`).
- **Parts**: on-disk resources — `prj.json`, key-named subdirs (`lib` / `com` / `exe` / `cmd` / `ko` / `osc`), optional root `lib/`, convention dirs `bin` / `etc` / `internal` / `rootfs`, paths named by `res`, loose root files collected by globs, and component config (`<key>.cfg` plus runtime `project@key` data).

**Constraint:** top-level `name` in `prj.json` **must** match the directory name `project/<name>/`.

---

## Scope

One product feature often ships several user-space / kernel artifacts in **one** Skinos project directory. The manifest holds identity, part keys, and orchestration fields. Delivery is an **FPK**; at runtime you use **HE** (`project@component`, where **project** is `name`).

**Pros:** Clear part boundaries; keys and scheduling in one file; uniform HE; FPK install registers components and boot tasks; `obj` aliases; `wui` binds pages to config objects.

**Cons:** Learning curve; `init` stage order is fixed by `land@init`; every `project@component.method` must exist and be registered.

---

## Parts

Rows below are **on-disk** resources. The rightmost column is only a **cross-check**: which top-level `prj.json` key usually registers that part.  
`init` / `uninit` / `joint` / `wui` / `obj` are **not** file/directory kinds — they are JSON sections only (see [prj.json.md](project/land/prj.json.md)).

`lib` / `com` / `exe` / `osc` / `cmd` / `ko` usually each map to a **subdir named like the key**.

### Summary table

| Part | Meaning | Role in build/pack | Usual path | Typical `prj.json` key |
|------|---------|--------------------|------------|------------------------|
| Project manifest | Root `prj.json` | Input to pack and install | `project/<name>/prj.json` | The file itself |
| Shared library (per-key dir) | Subdir → `lib<key>.so` | Compile → package `lib/` | `project/<name>/<key>/` | `lib` |
| Root `lib/` | Optional whole library tree | Prepare copies tree; `*.so*` into package `lib/` | `project/<name>/lib/` | No top-level key |
| Loadable component | Key-named module; **artifact at package root = key name** (no suffix) | Com compile rules | `project/<name>/<key>/` | `com` |
| Component config | Persistent data for `project@key`; optional `<key>.cfg` | `<key>.cfg` packed as loose root file | Runtime store; default `.cfg` at project root | No top-level key; tied to `com` / `exe` |
| Executable component | Standalone binary | Package root | `project/<name>/<key>/` | `exe` |
| Bundled program | Large / third-party tree → executable | Same class as `exe` | `project/<name>/<key>/` | `osc` |
| External command | Global CLI binary | Package `bin/<key>`; `fpk-install` links `cmd` keys | `project/<name>/<key>/` | `cmd` |
| Kernel module | `.ko` | `*.ko` at package root | `project/<name>/<key>/` | `ko` |
| Named resources | Paths listed under `res` | Packed if they exist | Relative to project root | `res` |
| `bin` / `etc` / `internal` / `rootfs` | Convention subtrees | Same-named package dirs | Under `project/<name>/` | No top-level key |
| Root globs | Scripts, images, `*.json`/`*.cfg`/`*.html`, … | Package root | Project root | No separate key |

Pack details: [`project2fpk.md`](project2fpk.md). Field grammar: [`prj.json.md`](project/land/prj.json.md).

---

## Project manifest (identity only)

Path: `project/<name>/prj.json`.

```json
{
  "name": "myproj",
  "version": "1.0.0",
  "intro": "one-line intro",
  "desc": "longer description",
  "author": "maintainer",
  "type": "app"
}
```

- **`name`**: folder name; the “project” side of `project@component`.
- **`version` / `intro` / `desc`**: build and UI display.
- **`type`**: e.g. `root` / `admin` / `user` / `app` — product-defined intent.

Part keys and orchestration fields: [`prj.json.md`](project/land/prj.json.md).

---

## `lib` (shared libraries)

```json
"lib": { "util": "shared helpers for this project" }
```

**Directory:** `project/myproj/util/` (key = dir name).

**Use for:** code linked by `com` / `cmd` / `exe` / `osc` or other projects; expose headers and `lib<key>.so`.

**Vs other parts:** no HE object `project@util` unless wrapped by a `com` / `exe`; libraries are linked, not scheduled as `@` components.

---

## `com` (loadable components)

```json
"com": {
  "sensor": "sensor logic",
  "gateway": "gateway logic"
}
```

**Directories:** `project/myproj/sensor/`, `gateway/`.

**Build result:** SDK pack places an unsuffixed binary named like the key at the FPK root (see [`project2fpk.md`](project2fpk.md)).

**Use for:** in-process methods (`setup` / `shut` / `list` / business APIs) for `init` / `joint`. Follow the SDK loadable-component template.

**Vs `lib`:** `com` is loaded/scheduled as `@`; `lib` is only linked.

**Vs `cmd` / `exe`:** `com` is in-process; `cmd`/`exe` are separate processes. `exe` can also own `project@key` config.

---

## Component config (bound to a component key)

Not another binary type: **persistent JSON** for a component key (usually under `"com"` / `"exe"`), plus optional factory default `project-root/<key>.cfg`. HE object: `project@key`.

**In `prj.json`:** there is **no** top-level `"config"` section. Declaring `"com": { "sensor": … }` implies config surface `project@sensor`. `<key>.cfg` is a loose resource.

**Factory default:** put `<key>.cfg` at project root (e.g. `sensor.cfg` for `com.sensor`). Packed as root `*.cfg` ([`project2fpk.md`](project2fpk.md)) or via `res`.

**Minimal:** `com` dir `sensor/` + optional `sensor.cfg` → `he 'myproj@sensor'`. Same pattern for `exe` + `helper.cfg`.

**Vs code parts:** config is the **data plane**; `com` modules / `exe` binaries are the **code plane**.

**Vs `wui`:** `wui` `config` / `object` point at the **config object**, not the module/binary file. Field details: [`prj.json.md` §9](project/land/prj.json.md).

HE syntax for config and methods: [`doc/com/land/he.md`](doc/com/land/he.md).

---

## `cmd` (external commands)

```json
"cmd": { "mycli": "CLI tool" }
```

**Directory:** `project/myproj/mycli/`.

**Use for:** operator/script CLIs; after image install, typically a global command named like the key (`fpk-install` only links keys listed under `cmd` — [`fpk2rootfs.md`](fpk2rootfs.md)).

**Vs `exe`:** `cmd` is meant as a system command + `bin/` link policy; `exe` is more often an in-package helper invoked by path.

---

## `exe` (executable components)

```json
"exe": { "helper": "install/maintenance helper" }
```

**Directory:** `project/myproj/helper/`.

**Use for:** standalone executables at FPK root. Still a **component**: may have `project@helper` config and optional `helper.cfg`. Runs as its own process (unlike `com`).

**Vs `osc`:** `exe` is usually small first-party tools; `osc` is often third-party / large trees.

---

## `osc` (bundled programs)

```json
"osc": { "ntpclient": "vendored NTP client tree" }
```

**Directory:** `project/myproj/ntpclient/` (often a full subtree with its own build rules).

**Use for:** large or upstream trees kept in-tree and in the same FPK. Same install class as `exe` (executable at package root).

---

## `ko` (kernel modules)

```json
"ko": { "demo": "sample kernel module" }
```

**Directory:** `project/myproj/demo/`.

**Use for:** produce `.ko` for `insmod` / product scripts. Needs matching kernel headers/build env. Pure app projects often omit `ko`.

---

## `res` (named resources)

`res` **names paths** in `prj.json` that must enter the `.fpk`.

```json
"res": { "defaults": "default data directory" }
```

**Use for:** explicitly packing non-built resources when globs are not enough. Root `*.cfg` / `*.html` / `*.json` are often collected automatically — see [`project2fpk.md`](project2fpk.md).

---

## Orchestration in `prj.json` (not on-disk parts)

These sections live **only** in the manifest — no same-named subdirs required. Field shapes and semantics: [`prj.json.md`](project/land/prj.json.md).

| Key | Purpose (short) | Details |
|-----|-----------------|---------|
| `obj` | Public aliases / forwards onto a `com` (or other `@` object) | [prj.json.md §5](project/land/prj.json.md) |
| `init` | Boot-stage `project@component.method` table | [§6](project/land/prj.json.md) + [`init.md`](doc/com/land/init.md) |
| `uninit` | Shutdown-stage methods (usually `.shut`) | [§7](project/land/prj.json.md) + [`uninit.md`](doc/com/land/uninit.md) |
| `joint` | Event name → methods | [§8](project/land/prj.json.md) + [`joint.md`](doc/com/land/joint.md) |
| `wui` | Web menu / page / visibility against a config object | [§9](project/land/prj.json.md) |

Tiny example (orchestration only):

```json
"obj": { "myproj@wan": "eth" },
"init": { "app": { "myproj@sensor.setup": "" } },
"joint": { "network/on": { "myproj@gateway.on": "" } }
```

---

## Walkthrough: build a full sample project

Fictional project **`fullsample`**: grow the tree and `prj.json` part by part. Use SDK templates for real source; omit `ko` if you have no kernel build.

### 1. Directory and minimal `prj.json`

Create `project/fullsample/`:

```json
{
  "name": "fullsample",
  "version": "1.0.0",
  "intro": "full parts sample",
  "desc": "shows how parts are registered in prj.json",
  "author": "you",
  "type": "app"
}
```

Add `fullsample` to the product’s build project list (product-specific).

### 2–9. Add disk parts

| Step | `prj.json` | Directory / file |
|------|------------|------------------|
| 2 | `"lib": { "util": "…" }` | `project/fullsample/util/` |
| 3 | `"com": { "main": "…" }` | `project/fullsample/main/` (artifact name `main`) |
| 4 | *(no key)* | optional root `main.cfg` for `fullsample@main` |
| 5 | `"cmd": { "cli": "…" }` | `project/fullsample/cli/` |
| 6 | `"exe": { "helper": "…" }` | `project/fullsample/helper/` (+ optional `helper.cfg`) |
| 7 | `"osc": { "bundle": "…" }` | `project/fullsample/bundle/` |
| 8 | `"ko": { "kmod": "…" }` | `project/fullsample/kmod/` (optional) |
| 9 | `"res": { "data": "…" }` | path required by toolchain |

### 10–12. Add orchestration

See field rules in [`prj.json.md`](project/land/prj.json.md). Minimal:

```json
"obj": { "fullsample@alias": "main" },
"init": { "app": { "fullsample@main.setup": "" } },
"uninit": { "app": { "fullsample@main.shut": "" } },
"joint": { "network/on": { "fullsample@main.on": "" } },
"wui": {
  "panel": {
    "menu": "Demo",
    "en": "Panel",
    "cn": "示例面板",
    "page": "panel.html",
    "config": "fullsample@main"
  }
}
```

Implement `setup` / `shut` / `on` in `main`; add `panel.html` as needed.

### 13. Combined `prj.json` (reference)

```json
{
  "name": "fullsample",
  "version": "1.0.0",
  "intro": "full parts sample",
  "desc": "shows how parts are registered in prj.json",
  "author": "you",
  "type": "app",
  "lib": { "util": "sample shared library" },
  "com": { "main": "main business component" },
  "cmd": { "cli": "sample command" },
  "exe": { "helper": "in-package helper" },
  "osc": { "bundle": "bundled program tree" },
  "ko": { "kmod": "sample kernel module" },
  "res": { "data": "resource note" },
  "obj": { "fullsample@alias": "main" },
  "init": { "app": { "fullsample@main.setup": "" } },
  "uninit": { "app": { "fullsample@main.shut": "" } },
  "joint": { "network/on": { "fullsample@main.on": "" } },
  "wui": {
    "panel": {
      "menu": "Demo",
      "cn": "示例面板",
      "en": "Panel",
      "page": "panel.html",
      "config": "fullsample@main"
    }
  }
}
```

### 14. Build, install, verify

Build/pack: [`project2fpk.md`](project2fpk.md). Image install: [`fpk2rootfs.md`](fpk2rootfs.md). Smoke: `he 'fullsample@main'`, `he 'fullsample@main.setup'`; check `land@init.list` for stage `app`.

---

## Checklist

1. Root has `prj.json`; `name` matches directory name.  
2. Every `lib` / `com` / `cmd` / `exe` / `osc` / `ko` key has a same-named subdir; `com` pack artifact name matches that key (unsuffixed).  
3. Strings in `init` / `joint` are real `project@component.method` values ([`prj.json.md`](project/land/prj.json.md)).  
4. Factory defaults: root `<key>.cfg` for matching `com` / `exe` keys when needed.  
5. Prefer SDK templates for new parts.
