# Skinos project development

A **Skinos project** is a directory with a `prj.json` manifest. The SDK packs it into an **FPK** (`.fpk`). On the device the runtime id is `name@key` (project name, then the part key). You query configuration and call methods with **HE** (`eline`, `he '…'`, or a Web page).

This page is the entry for **writing** a project. It shows how to create one with `prj`, what that command writes, and where to go next for each kind of program.

| Next | What it covers |
|------|----------------|
| [Executable component (comexe)](comexe.md) | HE object with `_setup` / `_shut` / `_service` — `prj add_com` / `add_exe` |
| [Shell component](shell.md) | Same HE shape, implemented as a script — copy `tmptools/comshell` |
| [Commands and bundled programs](cmd.md) | `main()` binaries — `prj add_cmd` / `add_osc` |
| [Web UI and language files](wui.md) | HTML page + `cn.json` / `en.json` — `prj add_wui` |
| [Boot and shutdown](init.md) | Register `.setup` / `.shut` — `prj add_init` / `add_uninit` |
| [Joint events](joint.md) | Network / storage handlers — `prj add_joint` |

Field grammar: [`../com/land/prj.json.md`](../com/land/prj.json.md). On-disk parts: [`../../project.md`](../../project.md). HE: [`../com/land/he.md`](../com/land/he.md). C API: [`../com/land/skin.md`](../com/land/skin.md). FPK runtime: [`../com/land/fpk.md`](../com/land/fpk.md).

---

## What a project contains

`prj.json` has two kinds of keys.

**Identity** — `name`, `intro`, `desc`, `version`, `author`, `type`. `name` must match the directory name.

**Artifacts** — each key is a subdirectory that the build compiles and packs:

| `prj.json` key | Result in the FPK |
|----------------|-------------------|
| `exe` | Binary at package root; registered as HE object `name@key` |
| `cmd` | Binary under `bin/` (shell command) |
| `osc` | Binary at package root (bundled / third-party program) |
| `lib` | `lib<key>.so` |
| `ko` | `*.ko` |
| `com` | Loadable module at package root (not covered in this tutorial set) |

**Orchestration** lives only in JSON (no matching directory):

| Key | Role |
|-----|------|
| `init` | Boot-stage HE calls (usually `.setup`) |
| `uninit` | Shutdown-stage HE calls (usually `.shut`) |
| `joint` | Event name → HE calls |
| `wui` | Web menu, HTML page, language files |
| `obj` | Extra public object names that reuse an existing component |

`land@fpk.register` reads `com`, `exe`, `obj`, `init`, `uninit`, and `joint` (plus `name`). `lib` / `cmd` / `osc` / `ko` drive pack layout.

---

## The `prj` command

Source: `tools/fpktools/prj.c`. After the SDK tools build, the binary is installed into the platform tools directory (slave host: `config/slave/x86/tools/prj`).

**Run it from the SDK root.** Templates are read from `project/tmptools/` with a relative path. `prj` only creates trees under `project/<name>/`. Customer overlays (`rice/`) and platform packages (`config/<platform>/`) are copied by hand.

```text
prj create     <project>
prj delete     <project>
prj check      <project>
prj add_com    <project> <name>          executable component (comexe)
prj add_exe    <project> <name>          same as add_com
prj add_cmd    <project> <name>          command (FPK bin/)
prj add_osc    <project> <name>          bundled program (FPK root)
prj add_lib    <project> <name>
prj add_ko     <project> <name>
prj add_wui    <project> <webpage>
prj add_init   <project> <level> <API>
prj add_uninit <project> <level> <API>
prj add_joint  <project> <event> <API>
prj add_object <project> <object> <component>
```

Several subcommands prompt on stdin (introduction, menu titles). `add_com` / `add_cmd` / `add_wui` create the project first if it does not exist.

---

## Create a project

```bash
cd /path/to/snake8
prj create myproj
# project myproj introduction: <type a one-line intro, then Enter>
```

Printed path: `project/myproj`.

### Generated `prj.json`

```json
{
    "name": "myproj",
    "version": "8.0.0",
    "author": "fpktools",
    "intro": "<the line you typed>"
}
```

| Field | Source |
|-------|--------|
| `name` | Command argument; must stay equal to the folder name |
| `version` | Default `8.0.0` (`PROJECT_DEFAULT_VERSION`) |
| `author` | `fpktools` — change it |
| `intro` | Stdin prompt |

Add `desc` and `type` when you know them (`type` is declared intent: `root` / `admin` / `user` / `app`). Artifact and orchestration objects appear when you run the `add_*` commands.

### Generated `Makefile`

Copied from [`project/tmptools/Makefile`](../../project/tmptools/Makefile) with one substitution:

```make
PROJECT_ID:=myproj
```

The rest is the OpenWrt-style package wrapper: `prj-read` for version/intro/desc, `Package/Define`, `Build/Install/fpk` and `Build/Install/fpk2rootfs`. Edit `DEPENDS` / `PKG_BUILD_DEPENDS` if the project needs extra libraries. Do not rename `PROJECT_ID` independently of the directory and `prj.json` `name`.

### Tree after `create`

```text
project/myproj/
├── Makefile
└── prj.json
```

Typical next steps:

1. `prj add_com myproj sensor` — HE object `myproj@sensor` ([comexe.md](comexe.md))
2. `prj add_init myproj app myproj@sensor.setup` and `prj add_uninit myproj app myproj@sensor.shut` ([init.md](init.md))
3. `prj add_joint myproj network/online myproj@sensor.online` ([joint.md](joint.md))
4. `prj add_wui myproj sensor` ([wui.md](wui.md))
5. Optional: `prj add_cmd` / `add_osc` ([cmd.md](cmd.md)), or copy a shell component ([shell.md](shell.md))

`prj add_lib`, `prj add_ko`, and `prj add_object` exist for a shared library, a kernel module, and an extra object alias. They have no dedicated page here; the templates are `project/tmptools/library/` and `project/tmptools/kmodule/`, and `obj` is described in [`prj.json.md`](../com/land/prj.json.md).

---

## Build and install

```bash
./mkdel
make obj=myproj
# → build/store/myproj-<version>-<hardware>.fpk
```

Prefer `./mkdel` over `make clean` (a full clean rebuilds the whole SDK).

On the device, the install root is **not** a fixed path. Query it:

```bash
he 'land@fpk.path[ myproj ]'
```

List every install with `land@fpk.list`. Do not hardcode `/usr/share/skinos/…`.

Most application FPKs can be uploaded without reboot. Changes to **land** or **arch** need a full firmware image (`.zz`).

---

## Related documents

| Document | Role |
|----------|------|
| [`../../project.md`](../../project.md) | Which on-disk part to choose |
| [`../com/land/prj.json.md`](../com/land/prj.json.md) | Manifest field reference |
| [`../../project2fpk.md`](../../project2fpk.md) | Source tree → `.fpk` |
| [`../com/land/fpk.md`](../com/land/fpk.md) | `land@fpk` APIs and path symbols |
| [`../com/land/skin.md`](../com/land/skin.md) | libskin C API |
| [`../com/land/he.md`](../com/land/he.md) | HE grammar |
| [`../com/land/README.md`](../com/land/README.md) | land runtime overview |
| [`component_doc_guide.md`](component_doc_guide.md) | How to write a component interface `.md` |
| [`single_object_comexe.md`](single_object_comexe.md) | Single-instance exe (Unix socket, libevent) |
