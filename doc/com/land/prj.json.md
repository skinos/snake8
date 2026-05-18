# `prj.json` — Skinos project manifest

This document describes the **project information file `prj.json`**: fields, semantics, packaging layout, and runtime behavior. For the **FPK product model** and **`land@fpk` APIs**, see [`fpk.md`](./fpk.md).

---

## 1. Role in the system

- Every **Skinos project directory** (often `project/<name>/` in the SDK) must contain a **`prj.json`** (the standard manifest filename, exposed to C code as **`PROJECT_INFOFILE`**).
- When the project is built into an **`.fpk`**, `prj.json` is shipped in the package. After installation, the system uses it to know the **project name, version, libraries, executables, components, resources**, and which **component APIs** to run for **boot, shutdown, and joint events**.
- Treat `prj.json` as a **machine-readable manifest**: human-facing metadata plus data for **build / pack / register / schedule**.

---

## 2. How to read this file quickly

1. **Top-level strings** (`name`, `intro`, `desc`, `type`, `version`, `author`) identify the project and permission class.
2. **`lib` / `exe` / `cmd` / `com` / `osc` / `ko` / `res`**: keys are usually **source subdirectories** under the project; values are **short descriptions**. Each key **declares** that artifact class exists. At runtime, **`land@fpk.register`** only **reads** the **`com`**, **`exe`**, **`obj`**, **`init`**, **`uninit`**, and **`joint`** objects from `prj.json` (plus required **`name`**). **`lib` / `cmd` / `osc` / `ko` / `res`** drive **build / pack / on-disk layout** (e.g. **`lib/`** and **`bin/`** trees); the register pass does **not** walk those sections as JSON keys.
3. **`obj`**: maps **public object names** to a **component key**. If the value contains **`@`**, it is passed straight to **`com_register(object, value, 0)`**; otherwise it is expanded to **`⟨prj.name⟩@⟨value⟩`** using the same **`@`** separator as HE object names.
4. **`init` / `uninit` / `joint`**: **nested** maps — boot level / shutdown level / event name → inner keys are **`project@component.method`** (HE-style), values are descriptions (often `""`).
5. **`wui`**: optional Web UI menu + page + language registration.

Strict **JSON** examples below contain **no comments**; narrative explains fields.

---

## 3. Top-level identity fields

| Field | Type | Meaning |
|------|------|---------|
| `name` | string | Project (and FPK) name; aligns with directory name and installed id. |
| `intro` | string | One-line summary. |
| `desc` | string | Longer description. |
| `version` | string | Version (e.g. `8.0.0`); may align with `PROJECT_DEFAULT_VERSION`. |
| `author` | string | Author or maintainer. |
| `type` | string | Intended permission tier. Documented values include **`root`**, **`admin`**, **`user`**, **`app`**. **`root`** is common in shipping trees; other tiers depend on toolchain / policy — treat `type` as **declared intent** for installers and UI. |

---

## 4. Artifact sections (key ≈ subdirectory name)

Each of these is an object: `"name": "brief description"`. The **key** is the **directory name** under the project where that artifact’s sources live (packaging tools use `prj.json` to decide what to compile and copy).

| Field | Meaning |
|------|---------|
| `lib` | Shared **libraries**; build output becomes `.so` under the FPK **lib** area. Headers listed for dev install go to **`install/include/<lib>/`**. |
| `exe` | **Executables** used **inside this project** only (not necessarily on global `PATH`). |
| `cmd` | **Commands** meant to be run from a Linux shell — packaged so they appear under the FPK **`bin/`** layout (e.g. `he`, `daemon`). |
| `com` | **Components** (e.g. `.com`); key = component source dir; runtime object is typically **`name@key`** where `name` is `prj.json`’s `name`. |
| `osc` | Bundled **third-party / open-source** programs (directory per key). |
| `ko` | **Kernel modules** (`.ko`) built from the named directory. |
| `res` | **Resource files or trees** copied as a unit into the FPK (paths / names as declared by the packager). |

**Tip:** In the SDK, match `lib` / `com` / `cmd` / `exe` keys to **folder names** at the project root.

---

## 5. `obj`: component instances and aliases

Shape: `"public-object-id": "component-subdir-or-origin-key"`.

- **Value** is either a **`com`** subdirectory name (expanded to **`project@subdir`** for **`com_register`**) or an **already-qualified** object path containing **`@`** (used as-is as the register **origin** key for **`type` 0**).
- **Key** is the **public object id** (e.g. **`tui@telnet`**, **`land@joint`**, or a short alias depending on product conventions).

Use this for **several logical objects sharing one component** (e.g. `ifname@lan` / `ifname@lan2`) or **stable public names** that differ from the directory name.

---

## 6. `init` — boot levels and `.setup`

Outer key = **init level** (e.g. `land`, `arch`, `app`). Inner object: keys are **`project@component.method`** (usually `.setup`), values = API description (often empty).

Semantics tie to **`land@init`**; see [`init.md`](./init.md).

---

## 7. `uninit` — shutdown levels and `.shut`

Same nesting as `init`, but for shutdown stages and **`project@component.shut`** (etc.). See [`uninit.md`](./uninit.md).

---

## 8. `joint` — events → component methods

Outer key = **joint event** (e.g. `network/online`, `storage/insert`). Inner map: **`project@component.method`** → description. See [`joint.md`](./joint.md).

---

## 9. `wui` — Web UI registration (optional)

Each top-level key under `wui` is a **page id**. Per page, common fields:

| Subfield | Meaning |
|----------|---------|
| `menu` | Top-level menu group in the Web UI (e.g. `System`). |
| `en` / `cn` | Menu label in English / Chinese. |
| `page` | HTML filename (under the project, as packaged). |
| `lang` | Object mapping locale keys (`cn`, `en`, …) to **JSON language file** paths for that page. |
| `config` | If set, the menu entry is shown when this **component config object** exists (e.g. `tui@telnet`). |
| `object` | With `config` unset, **`land@fpk.wui_menu`** may require this object (and optional **`api`**) to exist via **`com_have`** before the entry is listed. |
| `api` | Optional; paired with **`object`** for the **`com_have(object, api)`** check before **`land@fpk.wui_menu`** lists an entry. |
| `mode` | Optional object: **keys** are **work-mode** identifiers; they must match the runtime string in the default-object register **`network_mode`** (product code usually keeps this aligned with **`land@machine` → `mode`**, e.g. `ap`, `gateway`, `mix`). **Values** are strings; when the entry for the **current** mode is exactly **`"disable"`**, this **`wui`** page is **omitted** from **`land@fpk.wui_menu`**. Omit **`mode`**, omit the key for the current mode, or use any value other than **`"disable"`** to leave the page visible (still subject to **`config` / `object` / `attr`**). **Global prerequisite:** if register **`network_mode`** is missing or empty, **`wui_menu`** returns **NULL** for the whole menu. |
| `attr` | When `config` is non-null, optional **attribute path** for finer visibility rules. |

See **§15** for a composite example; match keys to real directories in your SDK tree.

---

## 10. Relation to `he` / HE

`project@component.method` strings match **HE** as used with **`he`** ([`he.md`](./he.md)): think **“scheduler fires this call for you”**, not interactive typing.

---

## 11. Validation and tooling

- **`project_check`**: verifies the project directory exists, **`prj.json`** parses as JSON, and **every** file in that directory whose name contains **`.json`** or **`.cfg`** also parses as JSON (invalid companion files fail the check).
- Helper binaries may live under **`cmd`** (named by each `cmd` key in `prj.json`); exact tools depend on your SDK.

---

## 12. Compile, pack, and install — Skinos resource flow

**Notation:** **`prj.json:section`** means “every **key** under that JSON object”: e.g. `prj.json:cmd` builds one command per key (`he`, `daemon`, …). **`FPK:/`** is the **root of the `.fpk` archive**.

**On-device paths** use the same **symbols** as **[`fpk.md`](./fpk.md)** (*Runtime install paths*): **`⟨PRJ_ROOT⟩`** (typically the installed-project root macro **`PROJECT_DIR`**), **`⟨PRJ_NAME⟩`** (installed project = `prj.json` → `name`), **`⟨LIB_DIR⟩`** / **`⟨BIN_DIR⟩`** (**`PROJECT_LIB_DIR`** / **`PROJECT_BIN_DIR`**), **`⟨SYS_ROOT⟩`** (running system root for merged trees). Per-project install prefix: **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**.

**Pipeline:** (1) **Compile** sources in subdirectories named by `prj.json` keys → (2) **Pack** outputs and loose files into an **FPK** → (3) **Install** under **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**, and copy `install/*` into the **SDK `INSTALL/`** tree.

Exact steps follow your **SDK Makefile / packaging scripts**; the tables below are the **layout contract** Skinos tooling targets.

### 12.1 Manifest file → FPK

| Stage | Source (project tree) | Destination in FPK |
|-------|------------------------|-------------------|
| Copy | `./prj.json` | `FPK:/prj.json` |

### 12.2 Build outputs declared in `prj.json` → FPK

| `prj.json` section | Typical build output (per key under that section) | Destination in FPK |
|--------------------|---------------------------------------------------|--------------------|
| `lib` | `*.so` shared library | `FPK:/lib/` |
| `com` | `*.com` component | `FPK:/` (FPK root) |
| `cmd` | Executable installed as a shell command (basename = key) | `FPK:/bin/` |
| `exe` | Project-local executable | `FPK:/` |
| `osc` | Third-party / open-source program binary | `FPK:/` |
| `ko` | `*.ko` kernel module | `FPK:/` |
| `res` | Resource files or trees (as defined by the packager) | `FPK:/` (per project rules) |

### 12.3 Pre-built trees under the project → FPK (copy-only)

| Source (project tree) | Destination in FPK |
|-----------------------|--------------------|
| `./lib/*.so*` | `FPK:/lib/` |
| `./bin/*` | `FPK:/bin/` |
| `./etc/*` | `FPK:/etc/` |
| `./internal/*` | `FPK:/internal/` |
| `./rootfs/*` | `FPK:/rootfs/` |

### 12.4 Loose files at project root → FPK

| Source (project root) | Destination in FPK |
|-----------------------|--------------------|
| `*.cfg`, `*.sh`, `*.ash`, `*.png`, `*.jpg`, `*.json`, `*.html` | `FPK:/` |

### 12.5 Developer payloads (`install/`) → FPK

`<lib>` = a **library key** from `prj.json:lib` (e.g. `skin`).

| Source | Destination in FPK |
|--------|-------------------|
| `*.h` headers shipped for that library | `FPK:/install/include/<lib>/` |
| Development `.so` copies for linking on the SDK host | `FPK:/install/lib/` |

### 12.6 Documentation / COM-face assets

| Source | Destination |
|--------|-------------|
| `*.md`, `*.png` (when routed by the packager for docs / UI face) | `gCOMFACE_DIR/<project-name>/` |

*(Some `*.png` may also be packed at `FPK:/` per §12.4; which rule applies depends on the packaging recipe.)*

---

## 13. Installing an FPK onto the system

After **`land@fpk.install`** (or equivalent), FPK contents are expanded under **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** (see **`fpk.md`** — **Runtime install paths**).

### 13.1 Manifest on device

| In FPK | On device |
|--------|-----------|
| `prj.json` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/prj.json` |

### 13.2 Built / staged payloads from FPK → per-project prefix

| In FPK | On device (typical) | Notes |
|--------|---------------------|-------|
| `lib/*` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/lib/` | Often symlinked into **`⟨LIB_DIR⟩`** |
| `*.com` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | Alongside other root-level payloads |
| `bin/*` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/bin/` | Often symlinked into **`⟨BIN_DIR⟩`** |
| Binaries that were packed as **`exe`** or **`osc`** entries | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | Same prefix as `.com` |
| `*.ko` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | |
| **`res`** content as packed | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | |

### 13.3 Optional trees inside the FPK → system paths (`land@fpk.register`)

When **`land@fpk.register`** runs against a project directory, it may **copy** payloads (only if the destination file does not already exist):

| In project / FPK tree | Typical destination (platform macros) | Notes |
|------------------------|-----------------------------------------------|--------|
| `etc/*` (under **`FPK_ETC_DIR`**, i.e. `etc/`) | **`PROJECT_ETC_DIR`** (often `/etc`) | Copy when the target path is not already present |
| `internal/*` (under **`FPK_INT_DIR`**, i.e. `internal/`) | **`PROJECT_INT_DIR`** (under **`PROJECT_MNT_DIR`**, product-defined) | Same rule |
| `rootfs/*` | **`⟨SYS_ROOT⟩/`** | **Not** applied by the **`land`** FPK register path; treat as **packaging / product installer** convention if your SDK emits this tree (see §12.3) |

### 13.4 Loose files at FPK root → per-project prefix

| In FPK (root) | On device |
|-----------------|-----------|
| `*.cfg`, `*.sh`, `*.ash`, `*.png`, `*.jpg`, `*.json`, `*.html` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` |

### 13.5 Developer tree: `install/` → SDK (not kept under **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**)

| In FPK | Copied to (SDK / host) | Note |
|--------|-------------------------|------|
| `install/include/<lib>/*` | `INSTALL/include/<lib>/` | For compiling against the project’s libraries |
| `install/lib/*` | `INSTALL/lib/` | Development link libraries |

These paths are usually **consumed at install time** and **not** left as a runtime subtree under **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**.

---

## 14. Example — `land` project (this repository)

Current `land/prj.json` (verbatim; formatting normalized):

```json
{
    "name": "land",
    "intro": "component infrastructure",
    "desc": "core for skin system, provides the basic components&library of the entire system",
    "type": "root",
    "version": "8.0.0",
    "author": "dimmalex@gmail.com",
    "lib": {
        "skin": "skinos core library"
    },
    "cmd": {
        "he": "tools for call all component",
        "daemon": "service daemon management",
        "eline": "tools for terminal line to execute the he command"
    },
    "com": {
        "fpk": "fpk management",
        "init": "init/uninit/joint management",
        "component": "component management",
        "register": "register variables",
        "syslog": "system log management",
        "service": "service management",
        "machine": "system basic information management",
        "auth": "authentication management"
    },
    "obj": {
        "land@uninit": "init",
        "land@joint": "init",
        "com": "component",
        "reg": "register",
        "log": "syslog",
        "serv": "service",
        "fpk": "fpk",
        "machine": "machine",
        "auth": "auth"
    },
    "init": {
        "arch": {
            "land@syslog.setup": ""
        },
        "land": {
            "land@auth.setup": "",
            "land@joint.setup": "",
            "land@init.setup": "",
            "land@uninit.setup": ""
        }
    },
    "joint": {
        "storage/insert": {
            "land@syslog.setup": ""
        },
        "storage/remove": {
            "land@syslog.setup": ""
        }
    }
}
```

---

## 15. Example excerpt — `tui`-style manifest

Illustrates **`com` + `cmd` + short `obj` aliases + `init` + `wui`** (trimmed to one `wui` page for size):

```json
{
    "name": "tui",
    "intro": "Terminal user interface service",
    "type": "root",
    "version": "7.0.0",
    "author": "dimmalex@gmail.com",
    "com": {
        "telnet": "telnet server management",
        "ssh": "ssl shell server management"
    },
    "cmd": {
        "eline": "tools for terminal command line"
    },
    "obj": {
        "telnetd": "telnet",
        "sshd": "ssh"
    },
    "init": {
        "app": {
            "tui@telnet.setup": "",
            "tui@ssh.setup": ""
        }
    },
    "wui": {
        "telnet": {
            "menu": "System",
            "en": "Telnet Server",
            "cn": "Telnet服务器",
            "page": "telnet.html",
            "config": "tui@telnet",
            "lang": {
                "cn": "cn.json",
                "en": "en.json"
            }
        }
    }
}
```

---

## 16. Related documents

| Document | Content |
|----------|---------|
| [`fpk.md`](./fpk.md) | FPK lifecycle, `land@fpk` |
| [`init.md`](./init.md), [`uninit.md`](./uninit.md), [`joint.md`](./joint.md) | Lifecycle registration |
| [`component.md`](./component.md) | Components |
| [`he.md`](./he.md) | HE grammar |

---

**TL;DR:** **`prj.json`** = project identity + **which directories become lib/com/cmd/exe/…** + **`obj` aliases** + **when to call `project@component.method`** + optional **`wui`**. **§12–§13** connect the JSON to **FPK** layout and **on-device paths** **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** (see **Runtime install paths** in **`fpk.md`**).
