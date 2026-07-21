# `prj.json` — Skinos project manifest

This document is the **field reference** for the project information file `prj.json`: shapes, semantics, validation, and what **`land@fpk.register`** reads at runtime.

| Doc | Owns |
|-----|------|
| **This file** | `prj.json` fields, format reference, register/runtime notes |
| [`project.md`](../../project.md) | On-disk project parts and when to use which |
| [`project2fpk.md`](../../project2fpk.md) | Project tree → `.fpk` pack pipeline |
| [`fpk2rootfs.md`](../../fpk2rootfs.md) | Image install via `fpk-install` |
| [`fpk.md`](./fpk.md) | `land@fpk` APIs and runtime path symbols (`⟨PRJ_ROOT⟩`, …) |

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
| `com` | **Loadable components**; key = component source dir. Default SDK pack places an **unsuffixed** binary named like the key at the FPK root (see [`project2fpk.md`](../../project2fpk.md)). Runtime object is typically **`name@key`** where `name` is `prj.json`’s `name`. |
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

See **§14** for a composite example; match keys to real directories in your SDK tree.

---

## 10. Relation to `he` / HE

`project@component.method` strings match **HE** as used with **`he`** ([`he.md`](./he.md)): think **“scheduler fires this call for you”**, not interactive typing.

---

## 11. Validation and tooling

- **`project_check`**: verifies the project directory exists, **`prj.json`** parses as JSON, and **every** file in that directory whose name contains **`.json`** or **`.cfg`** also parses as JSON (invalid companion files fail the check).
- Helper binaries may live under **`cmd`** (named by each `cmd` key in `prj.json`); exact tools depend on your SDK.

---

## 12. Build / pack / install pointers

This file does **not** own pack or image-install layout tables. Use:

| Concern | Document |
|---------|----------|
| Project tree → `.fpk` (prepare / compile / stage) | [`project2fpk.md`](../../project2fpk.md) |
| `.fpk` → target rootfs (`fpk-install`) | [`fpk2rootfs.md`](../../fpk2rootfs.md) |
| Runtime install / hot deploy / path symbols (`⟨PRJ_ROOT⟩`, …) | [`fpk.md`](./fpk.md) |
| On-disk parts overview | [`project.md`](../../project.md) |

**Register vs image install:** `land@fpk.register` reads `com` / `exe` / `obj` / `init` / `uninit` / `joint` (plus `name`) as described in §2. Overlay behavior for `etc` / `internal` under register (often copy-if-missing) is part of **`land@fpk`**, not `fpk-install` — see [`fpk.md`](./fpk.md). Image builds that call `fpk-install` merge overlays as documented in [`fpk2rootfs.md`](../../fpk2rootfs.md).

---

## 13. `prj.json` format reference

The following JSON shows the complete `prj.json` structure with type annotations. All fields are described by their type and purpose.

```json
// Attributes introduction 
{
    "name": "project name",                                  // [ string ], project name, also used as directory name and FPK identifier
    "intro": "short description",                            // [ string ], one-line summary of the project
    "desc": "detailed description",                          // [ string ], longer description of the project purpose
    "type": "permission tier",                               // [ "root", "admin", "user", "app" ], intended permission tier for installers and UI
    "version": "version string",                             // [ string ], semantic version (e.g. "8.0.0")
    "author": "author or maintainer",                        // [ string ], author name or email address

    "lib":                                                   // [ json ], shared libraries built from subdirectories
    {
        "library directory": "description"                   // [ string ]: [ string ], library source directory name and its description
        // "...":"..."  How many libraries show how many properties
    },
    "exe":                                                   // [ json ], project-local executables
    {
        "executable directory": "description"                // [ string ]: [ string ], executable source directory name and its description
        // "...":"..."  How many executables show how many properties
    },
    "cmd":                                                   // [ json ], shell commands installed to global bin path
    {
        "command directory": "description"                   // [ string ]: [ string ], command source directory name and its description
        // "...":"..."  How many commands show how many properties
    },
    "com":                                                   // [ json ], loadable components (pack artifact = unsuffixed key name)
    {
        "component directory": "description"                 // [ string ]: [ string ], component source directory name and its description
        // "...":"..."  How many components show how many properties
    },
    "osc":                                                   // [ json ], bundled third-party / open-source programs
    {
        "program directory": "description"                   // [ string ]: [ string ], open-source program directory name and its description
        // "...":"..."  How many programs show how many properties
    },
    "ko":                                                    // [ json ], kernel modules (.ko)
    {
        "module directory": "description"                    // [ string ]: [ string ], kernel module source directory name and its description
        // "...":"..."  How many modules show how many properties
    },
    "res":                                                   // [ json ], resource files or trees
    {
        "resource path": "description"                       // [ string ]: [ string ], resource file or directory name and its description
        // "...":"..."  How many resources show how many properties
    },

    "obj":                                                   // [ json ], public object name to component mappings
    {
        "public object name": "backing component"            // [ string ]: [ string ], object name registered in the system and its backing component
                                                               // if value contains "@", used as-is for com_register
                                                               // if value has no "@", expanded to "project@value"
        // "...":"..."  How many objects show how many properties
    },
    "init":                                                  // [ json ], boot startup tasks
    {
        "boot level":                                        // [ string ]: { json }, boot level name (arch, land, bus, device, network, manage, local, extern, app, app2, general, delay, delay2, delay3, delay4, delay5)
        {
            "component.method": "description"                // [ string ]: [ string ], HE-style component method call and its description
            // "...":"..."  How many tasks at this level show how many properties
        }
        // "...":{...}  How many boot levels show how many properties
    },
    "uninit":                                                // [ json ], shutdown tasks (same structure as init)
    {
        "shutdown level":                                    // [ string ]: { json }, shutdown level name (reverse order of init levels)
        {
            "component.method": "description"                // [ string ]: [ string ], HE-style component method call and its description
            // "...":"..."  How many tasks at this level show how many properties
        }
        // "...":{...}  How many shutdown levels show how many properties
    },
    "joint":                                                 // [ json ], joint event subscriptions
    {
        "joint event name":                                  // [ string ]: { json }, joint event name (e.g. network/online, storage/insert)
        {
            "component.method": "description"                // [ string ]: [ string ], HE-style component method call and its description
            // "...":"..."  How many handlers at this event show how many properties
        }
        // "...":{...}  How many events show how many properties
    },
    "wui":                                                   // [ json ], web UI page registrations (optional)
    {
        "page identifier":                                   // [ string ]: { json }, page identifier, used as key under wui
        {
            "menu": "menu group",                            // [ string ], top-level menu group in the web UI (e.g. "System", "Network")
            "en": "English menu label",                      // [ string ], English display name for the menu entry
            "cn": "Chinese menu label",                      // [ string ], Chinese display name for the menu entry
            "page": "html filename",                         // [ string ], HTML page filename under the project directory
            "config": "component config object",             // [ string ], optional, show entry only when this component config exists
            "object": "component object name",               // [ string ], optional, show entry only when this object exists (com_have check)
            "api": "method name",                            // [ string ], optional, paired with object for com_have(object, api) check
            "attr": "attribute path",                        // [ string ], optional, when config is set, additional attribute path for visibility
            "mode":                                          // [ json ], optional, work-mode visibility filter
            {
                "mode name": "enable or disable"             // [ string ]: [ "enable", "disable" ], when "disable", hide entry for this mode
                // "...":"..."  How many modes show how many properties
            },
            "lang":                                          // [ json ], language file paths
            {
                "locale": "language json file path"          // [ string ]: [ string ], locale key (cn, en, etc.) and its language file path
                // "...":"..."  How many locales show how many properties
            }
        }
        // "...":{...}  How many pages show how many properties
    }
}
```

---

## 14. Example — `land` project (this repository)

The `land` project provides core infrastructure: authentication, component management, init/uninit/joint scheduling, register variables, syslog, service supervision, machine info, and FPK packaging.

```json
{
    "name": "land",                                      # the land project, core infrastructure
    "intro": "component infrastructure",                 # short description
    "desc": "core for skin system, provides the basic components&library of the entire system",
    "type": "root",                                      # requires root permission
    "version": "8.0.0",                                  # current version
    "author": "dimmalex@gmail.com",                      # author

    "lib":                                               # has 1 library
    {
        "skin": "skinos core library"                    # the skinos core library
    },
    "cmd":                                               # has 3 commands
    {
        "he": "tools for call all component",            # he command tool
        "daemon": "service daemon management",           # daemon executable
        "eline": "tools for terminal line to execute the he command"
    },
    "com":                                               # has 8 components
    {
        "fpk": "fpk management",                         # fpk package management
        "init": "init/uninit/joint management",          # init/uninit/joint shared implementation
        "component": "component management",             # component registration
        "register": "register variables",                # register variable management
        "syslog": "system log management",               # syslog service
        "service": "service management",                 # daemon service management
        "machine": "system basic information management", # machine info and control
        "auth": "authentication management"              # user authentication
    },
    "obj":                                               # has 9 object mappings
    {
        "land@uninit": "init",                           # land@uninit uses the init component
        "land@joint": "init",                            # land@joint uses the init component
        "com": "component",                              # com alias for component
        "reg": "register",                               # reg alias for register
        "log": "syslog",                                 # log alias for syslog
        "serv": "service",                               # serv alias for service
        "fpk": "fpk",                                    # fpk alias for fpk
        "machine": "machine",                            # machine alias for machine
        "auth": "auth"                                   # auth alias for auth
    },
    "init":                                              # has 2 boot levels
    {
        "arch":                                          # arch level: platform layer setup
        {
            "land@syslog.setup": ""                      # start syslog at arch level
        },
        "land":                                          # land level: core infrastructure setup
        {
            "land@auth.setup": "",                       # setup authentication
            "land@joint.setup": "",                      # setup joint event system
            "land@init.setup": "",                       # setup init system
            "land@uninit.setup": ""                      # setup uninit system
        }
    },
    "joint":                                             # has 2 joint event subscriptions
    {
        "storage/insert":                                # when external storage is inserted
        {
            "land@syslog.setup": ""                      # re-setup syslog to use new storage
        },
        "storage/remove":                                # when external storage is removed
        {
            "land@syslog.setup": ""                      # re-setup syslog after storage removal
        }
    }
}
```

---

## 16. Example — `tui`-style manifest

The `tui` project provides terminal user interface services: telnet and SSH servers, with a web UI page for telnet configuration.

```json
{
    "name": "tui",                                       # the tui project, terminal user interface
    "intro": "Terminal user interface service",          # short description
    "type": "root",                                      # requires root permission
    "version": "7.0.0",                                  # current version
    "author": "dimmalex@gmail.com",                      # author

    "com":                                               # has 2 components
    {
        "telnet": "telnet server management",            # telnet server component
        "ssh": "ssl shell server management"             # SSH server component
    },
    "cmd":                                               # has 1 command
    {
        "eline": "tools for terminal command line"       # terminal line tool
    },
    "obj":                                               # has 2 object mappings
    {
        "telnetd": "telnet",                             # telnetd object uses telnet component
        "sshd": "ssh"                                    # sshd object uses ssh component
    },
    "init":                                              # has 1 boot level
    {
        "app":                                           # app level: application setup
        {
            "tui@telnet.setup": "",                      # setup telnet server
            "tui@ssh.setup": ""                          # setup SSH server
        }
    },
    "wui":                                               # has 1 web UI page
    {
        "telnet":                                        # telnet configuration page
        {
            "menu": "System",                            # under System menu group
            "en": "Telnet Server",                       # English menu label
            "cn": "Telnet服务器",                          # Chinese menu label
            "page": "telnet.html",                       # HTML page file
            "config": "tui@telnet",                      # show when tui@telnet config exists
            "lang":                                      # language files
            {
                "cn": "cn.json",                         # Chinese language file
                "en": "en.json"                          # English language file
            }
        }
    }
}
```

---

## 17. Related documents

| Document | Content |
|----------|---------|
| [`fpk.md`](./fpk.md) | FPK lifecycle, `land@fpk` |
| [`init.md`](./init.md), [`uninit.md`](./uninit.md), [`joint.md`](./joint.md) | Lifecycle registration |
| [`component.md`](./component.md) | Components |
| [`he.md`](./he.md) | HE grammar |

---

**TL;DR:** **`prj.json`** = project identity + **which directories become lib/com/cmd/exe/…** + **`obj` aliases** + **when to call `project@component.method`** + optional **`wui`**. Pack layout → [`project2fpk.md`](../../project2fpk.md); image rootfs → [`fpk2rootfs.md`](../../fpk2rootfs.md); runtime paths → [`fpk.md`](./fpk.md).
