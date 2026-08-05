---
name: skinos-sdk
description: |
  Explain and use the Snake8/landos/skinos SDK layout, gBOARDID board selection,
  Makefile build system, and per-board product customization under config/<platform>/
  (especially config/swrt5/: makefile.config, sdk.config, rootfs/, config/*.cfg, DTS,
  and kernel/ + custom.sh overlays that copy sources like option.c into the SDK kernel).
  Use when the user asks about SDK structure, gBOARDID, Makefile, mkdel, make obj=,
  how to customize a product, rootfs overlay, default component cfg, sdk.config,
  kernel.config, kernel source override, custom.sh, option.c, "换板子", "定制配置",
  "运行系统上的文件", or swrt5 board files.
  Also use when the user says "更新仓库固件", "上传固件到仓库", "make ftp",
  "固件仓库", or asks to push the built .zz into the OTA/firmware repository
  (not a live device) — that is `make ftp` after a successful full build.
  Also use when the user says "更新fpk仓库", "上传fpk到仓库", "make sdk_repo",
  "fpk仓库" — that is `make sdk_repo` (uploads build/store FPKs via repo-upload).
  Do NOT use for device upload/upgrade/remote test — that is device-upgrade.
---

# Skinos SDK Structure & Build System

Landos/skinos embedded gateway SDK: select a board with **`gBOARDID`**, build via the **top-level Makefile**, sources under **`project/`**, platform glue under **`config/<platform>/`**.

Deeper project/`prj.json` detail: `project.md`, `fpk.md`, `doc/com/land/fpk.md`.  
Device deploy/test: `.claude/skills/device-upgrade/SKILL.md`.

## Repository layout

| Path | Role |
|------|------|
| `Makefile` | Top entry: parses `gBOARDID`, defines globals, `all` / `dep` / `clean` / forwards |
| `gBOARDID` | **Active board** (gitignored). List with `make pidlist` |
| `target.makefile` | Forwards `kernel` / `app` / `boot` into platform SDK |
| `mkdel` | Incremental clean of OpenWrt/`build_dir` skinos trees (prefer over `make clean`) |
| `config/<platform>/` | Platform toolchain, `*.makefile`, overlays (often separate git repos) |
| `project/<name>/` | Skinos projects (`prj.json` + sources) → FPK |
| `build/` | Outputs (gitignored): `install/`, `rootfs/`, `store/*.fpk`, `*.zz` |
| `tools/` | Host helpers (fpk pack, etc.) |
| `doc/com/` | Component API docs generated/installed per project |
| `rice/` | Optional customer overlays |
| `<platform>/` e.g. `swrt5/` | Checked-out platform SDK / OpenWrt tree used by `config/<platform>/` |

## gBOARDID

### Format

```text
platform-chip-board[-scope][-oem]
```

Parsed in `Makefile` into:

| Field | Makefile var | Example (`swrt5-mt7981-r607`) |
|-------|--------------|------------------------------|
| platform | `gPLATFORM` | `swrt5` |
| chip | `gHARDWARE` | `mt7981` |
| PCB/product | `gCUSTOM` | `r607` |
| scope (optional) | `gSCOPE` | default **`std`** if omitted |
| OEM (optional) | `gOEM` | default **`default`** if omitted |

Default if no `gBOARDID` file: `slave-x86-ubuntu2004` (host debug).

### How to switch board

```bash
# Inspect available / current board
make pidlist
make pidinfo

# Switch board
make pid gBOARDID=swrt5-mt7981-r607
make pidinfo
```

`gBOARDID` is **local/gitignored** — do not assume a board; **read the file** before building.

### Config path resolution

From board fields:

```text
config/${gPLATFORM}/                          # gPLATFORM_DIR
config/${gPLATFORM}/${gHARDWARE}/             # gHARDWARE_DIR
config/${gPLATFORM}/${gHARDWARE}/${gCUSTOM}/  # gCUSTOM_DIR
.../${gCUSTOM}/${gSCOPE}/                     # gSCOPE_DIR
.../${gCUSTOM}/${gOEM}/                       # gOEM_DIR
```

Platform makefiles / tools / `makefile.config` are picked from the **deepest existing** of scope → custom → hardware → platform.

If scope dir is missing (e.g. no `.../r607/std/` while `gSCOPE=std`), fall through to **custom** — still valid.

### Common platforms

| `gPLATFORM` | Typical use |
|-------------|-------------|
| `slave` | Host/debug (x86 Ubuntu) |
| `smtk2` | MediaTek MT76xx |
| `smtk3` | MediaTek MT79xx |
| `swrt5` | OpenWrt-based (e.g. mt7981-r607) |
| `srock` | Rockchip RK3568 |
| `wrt5` | Related OpenWrt lineage |

## Product customization (`config/swrt5/` model)

`config/swrt5/` is a **separate git repo** of platform glue. OpenWrt sources live in top-level **`swrt5/`** (`gSDK_DIR`). Full detail: [reference-swrt5.md](reference-swrt5.md).

### Layer map (example `swrt5-mt7981-r607` / `...-r607-wrt`)

```text
config/swrt5/                    # platform: sdk.makefile, lay.makefile, skin.mk, arch/, adjust/, rootfs/
└── mt7981/                      # hardware: kernel.config, rootfs/, config/
    └── r607/                    # custom: makefile.config, sdk.config, dts, config/
        └── wrt/                 # scope when gBOARDID ends with -wrt
            sdk.config, rootfs/, …
```

### Key files

| Artifact | Purpose |
|----------|---------|
| `makefile.config` | Resolves `gSDK_CFGFILE`, `gpKERNEL_CFGFILE`, `gpUPGRADE_IMAGE`, compile `CFLAGS`/`LDFLAGS` |
| `sdk.config` | OpenWrt `.config` → copied to `swrt5/.config` on `kernel_dep` |
| `kernel.config` | Linux fragment → path in `makefile.config` (`gpKERNEL_SDK_CFGFILE`) |
| `dts` | Often symlink into `adjust/patch/<chip>/dts/` |
| `**/rootfs/**` | Overlay onto **image root `/`** (banner, inittab, init.d, …) |
| `**/config/<project>/*.cfg` | Factory defaults → via **arch** FPK → **`/usr/share/skinos/<project>/`** |
| `needless.sh` (in rootfs) | Run inside rootfs at image build to strip/fix paths |
| `adjust/patch/` | SDK patches (`make sdk_adjust`) |
| **`kernel/` + `custom.sh`** | **Override Linux source files** into the SDK tree before kernel compile (see below) |

### Kernel source overlays (`config/…/kernel/` — depends on gBOARDID)

Do **not** edit files under the OpenWrt/SDK tree (e.g. `swrt5/build_dir/.../linux-*/drivers/...`) for lasting product changes. Instead:

1. **Read `gBOARDID` first** — different boards resolve to **different** `config/…/kernel/` directories.
2. Place the replacement source under the correct layer (or rely on fallthrough — see below).
3. Ensure that platform’s **`kernel/custom.sh`** copies it into `${LINUX_DIR}/…` via **`src2kernel`**.
4. Rebuild **full firmware** (`./mkdel` → `make`) — this is a **kernel** change, not `make obj=`.

**gBOARDID → which `kernel/` can apply** (same fallthrough as other config artifacts):

```text
gBOARDID = platform-chip-board[-scope][-oem]

${gSCOPE_DIR}/kernel/       e.g. config/swrt5/mt7981/r607/wrt/kernel/   (if -wrt)
${gCUSTOM_DIR}/kernel/      e.g. config/swrt5/mt7981/r607/kernel/
${gHARDWARE_DIR}/kernel/    e.g. config/swrt5/mt7981/kernel/
${gPLATFORM_DIR}/kernel/    e.g. config/swrt5/kernel/     ← often where option.c lives
```

Examples:

| `gBOARDID` | Typical dirs that may hold overlays |
|------------|-------------------------------------|
| `swrt5-mt7981-r607` | `config/swrt5/kernel/`, optionally `…/mt7981/kernel/`, `…/mt7981/r607/kernel/` |
| `swrt5-mt7621-v525w` | `config/swrt5/kernel/`, `…/mt7621/kernel/`, `…/mt7621/v525w/kernel/` |
| `smtk2-mt7621-d228` | `config/smtk2/kernel/`, `…/mt7621/d228/kernel/` |

**Always** open the active board’s layers before editing: a board-specific `option.c` under `…/<chip>/<board>/kernel/` **wins over** the platform file.

**How it is invoked:** `adjust/patch/` patches OpenWrt’s `include/kernel-defaults.mk` so that, before each kernel build, it runs:

```bash
export LINUX_DIR=$(LINUX_DIR); ${gPLATFORM_DIR}/kernel/custom.sh
```

(`custom.sh` itself usually lives at **platform** `config/<platform>/kernel/custom.sh`, but **`src2kernel` still prefers deeper board files**.)

**`src2kernel <basename> <dest-dir-or-file>`** copies the **first existing** file among:

```text
${gSCOPE_DIR}/kernel/<basename>
${gCUSTOM_DIR}/kernel/<basename>
${gHARDWARE_DIR}/kernel/<basename>
${gPLATFORM_DIR}/kernel/<basename>
```

into the second argument (typically under `${LINUX_DIR}/…`).

**Example (USB modem serial IDs)** — platform `custom.sh` line:

```bash
src2kernel option.c ${LINUX_DIR}/drivers/usb/serial
```

For `gBOARDID=swrt5-mt7981-r607` with no board-local override, edit **`config/swrt5/kernel/option.c`**.  
If `config/swrt5/mt7981/r607/kernel/option.c` exists, **edit that one** instead (it shadows the platform file).  
Never edit the copy inside `swrt5/build_dir/…`.

Cellular module workflow: **skinos-modem** (kernel support first, then `project/modem` AT driver).

**Signing required:** every change you add to overlays such as `option.c` **must** carry an attribution comment next to the new code, using the repo style:

```c
/* add by Auto for <module / reason> */
{ USB_DEVICE_INTERFACE_CLASS(0x3466, 0x3301, 0xff) },
```

Do not leave anonymous kernel diffs. Existing examples: `/* add by dimmalex for fg132 */`, `//add by qingcheng`.

**Adding a new overlay file:**

1. Put the file in the layer that matches the **target `gBOARDID`** (platform-wide vs one product only).
2. Add a `src2kernel …` line in that platform’s `kernel/custom.sh` if not already present.
3. `./mkdel` → `make` → upgrade `.zz`.

### Where runtime files come from

1. **`rootfs/` overlays** (`lay.makefile` `rootfs_install`): platform → hardware → custom → scope (later wins) → files on device under `/etc/...`, etc.
2. **`config/` defaults**: merged in `config/swrt5/arch/Makefile` into arch FPK; `fpk-install` copies to `/usr/share/skinos/` (`PROJECT_DIR`).
3. **`project/<name>/`**: feature packaging (shared across boards); board-specific tweaks prefer `config/swrt5/...`.

### What to edit for common goals

| Goal | Edit | Rebuild |
|------|------|---------|
| OpenWrt packages | `sdk.config` | `./mkdel` + `make` |
| Kernel **kconfig** | `kernel.config` | `./mkdel` + `make` |
| Kernel **source** (e.g. `option.c`) | `config/…/kernel/` + `custom.sh` | `./mkdel` + `make` |
| `/etc` files, init | `.../rootfs/` | full `make` |
| Default wifi/uart/lte cfg | `.../config/<proj>/*.cfg` | full `make` (arch) |
| Image bin / flags | `makefile.config` | full `make` |
| One app only | `project/<name>/` | `make obj=<name>` |

**Always read the active board’s `makefile.config` and existing `rootfs/`/`config/` before changing another product’s files.**

## Makefile macro conventions

| Prefix/suffix | Meaning |
|---------------|---------|
| `g*` | Global SDK |
| `gp*` | Platform-specific |
| `gh*` / `gc*` / `gs*` | Chip / PCB / scope (as used in tree) |
| `gos*` | Running rootfs staging (`gosROOT_DIR`, `gosPRJ_DIR`) |
| `*_DIR` | Directory |
| `*_CFGFILE` / `*_SH` / `*_CMD` | Config / shell / command |

Important dirs after parse:

- `gBUILD_DIR` = `build/`
- `gINSTALL_DIR` = `build/install/` (headers/libs staging)
- `gSTORE_DIR` = `build/store/` (`.fpk`)
- `gosROOT_DIR` = `build/rootfs/`
- `gosPRJ_DIR` = `build/rootfs/usr/share/skinos/`
- `gPROJECT_DIR` = `project/`
- `gSDK_DIR` = `${gTOP_DIR}/${gPLATFORM}` (e.g. `swrt5/`)

Platform makefile hooks (under `config/<platform>/`):

- `sdk.makefile` — kernel/SDK
- `lay.makefile` — rootfs layout
- `fpk.makefile` / `dir.makefile` / `com.makefile` / `lib.makefile` / `exe.makefile` / `pack.makefile`

Top `Makefile` forwards: `kernel`, `kernel_%`, `app`, `app_%`, `boot`, `sdk_%` → `target.makefile`.

## Build commands

### Full firmware (`.zz`)

```bash
cat gBOARDID                    # must match target device
./mkdel                         # or: make dep (dep itself runs ./mkdel then recreates build/)
make                            # all: dep → kernel → app → kernel_install → app_install
# Output: build/<hardware>_<custom>_<scope>_<version>.zz
```

**Do not use `make clean`** for normal work — full rebuild is very slow.

### Upload firmware to the repository (`make ftp`)

When the user (or a task prompt) says to **update the firmware repository** / **upload firmware to the repo** — meaning the OTA/firmware store, **not** a live device — build first, then run:

```bash
./mkdel
make              # produce build/*.zz for current gBOARDID
make ftp          # → sdk_ftp: upload the current built .zz into the firmware repository
```

### Upload FPK packages to the repository (`make sdk_repo`)

When the user says to **update the FPK repository** / **upload FPK to the repo** — again the package store, **not** a live device — ensure FPKs exist under `build/store/`, then run:

```bash
make obj=<name>   # or a full make that produces build/store/*.fpk
make sdk_repo     # → sdk_repo: upload FPKs from build/store into the FPK repository
```

| Intent | Command | What gets uploaded |
|--------|---------|-------------------|
| Update **firmware repository** (OTA `.zz` store) | **`make ftp`** → `sdk_ftp` | Current built `build/*.zz` (via `repo-upload … <file>`) |
| Update **FPK repository** (package store) | **`make sdk_repo`** | All FPKs under `build/store/` (via `repo-upload … fpk`) |
| Upgrade a **running device** | Web API `.zz` / FPK | **device-upgrade** skill |

Notes:

- `make ftp` is a short alias in `misc.makefile` → **`make sdk_ftp`**. There is **no** short `make repo` alias — use **`make sdk_repo`** (top Makefile `sdk_%` → platform `sdk.makefile`).
- Both are implemented with host tool **`repo-upload`** (`tools/fpktools/repo-upload.c`). Platform examples: `config/wrt5/sdk.makefile`, `config/slave/sdk.makefile`.
- `make ftp` / `sdk_ftp` needs a successful full firmware build for the **active `gBOARDID`** first.
- `make sdk_repo` needs `build/store/` populated (e.g. after `make obj=<name>` or a full build that stages FPKs).
- Related: `make tftp` → local TFTP put of the `.zz` (lab/debug); do not confuse with repo upload.
- Do **not** confuse either repo upload with pushing packages to a live gateway (**device-upgrade**).

### Single project FPK

```bash
make obj=<project>              # runs ./mkdel <project> then builds that project only
# Output: build/store/<project>-<ver>-<hardware>.fpk
```

`make obj=ipsec` cleans only `skinos_ipsec*` under platform `build_dir` (via `mkdel`), **not** `root*`.

### `mkdel` behavior

| Invocation | Effect |
|------------|--------|
| `./mkdel` | Remove all `skinos_*` and `root*` under each `config` platform's `build_dir` |
| `./mkdel ipsec vpn` | Remove only `skinos_ipsec*`, `skinos_vpn*` (no `root*`) |

### Other targets

```bash
make dep              # mkdel + wipe build/install staging + rootfs_prepare
make kernel / make app
make ftp              # upload current build/*.zz to firmware repository (sdk_ftp)
make sdk_repo         # upload build/store/*.fpk to FPK repository
make tftp             # put .zz to local TFTP server (sdk_tftp; not the repo)
make clean            # AVOID unless intentional full wipe
make clean obj=<name> # clean one project via target.makefile
make preset           # host build deps (Ubuntu)
make clangd           # compile_commands.json
make pidinfo          # print active gBOARDID
```

### Build order (full image)

`dep → kernel_dep/app_dep → kernel → app → kernel_install → app_install` → pack `.zz` into `build/`.

### Outputs

| Artifact | Path |
|----------|------|
| Staged headers/libs | `build/install/` |
| Assembled rootfs | `build/rootfs/` (staging layout often under `usr/share/skinos/` — **host build only**) |
| Per-project FPK | `build/store/*.fpk` |
| Full firmware | `build/*.zz` |

### On-device project path (do not hardcode)

**Host** `build/rootfs/usr/share/skinos/<name>/` is only the image staging layout. **On a running device**, the live project directory depends on `gBOARDID` / `PROJECT_DIR`, factory bake vs FPK hot-install, and may be under overlay paths — it is **not** always `/usr/share/skinos/<name>`.

**Always** resolve with:

```text
land@fpk.path[ <project> ]     # absolute install dir for one project
land@fpk.list                  # all projects; each has "path"
```

Never `find /usr/share/skinos` or assume `/mnt/internal/skinos/…`. Same rule in **device-upgrade** / **skinos-project** / **skinos-he**. API: `doc/com/land/fpk.md`.

## Skinos project (`project/<name>/`)

- Must have **`prj.json`**; top-level **`name` must equal directory name**
- Components: `project@subdir` (e.g. `ipsec@client`)
- Typical `prj.json` keys: `com`, `exe`, `lib`, `cmd`, `osc`, `ko`, `res`, `init`, `joint`, `uninit`, `wui`, `obj`
- Boot `init` levels (order): `arch → land → app → general → manage → delay`
- `type: "root"` = system-level package
- All projects use shared skin APIs from **`project/land/skin`**

More: `project.md`, `fpk.md`, `project/AGENTS.md`, `doc/com/land/fpk.md`.

## Agent checklist

When building or explaining the SDK:

1. **Read `gBOARDID`** (or ask which board) before `make` / `make obj=`
2. Resolve `config/${gPLATFORM}/...` layers; open that board’s **`makefile.config`**
3. For on-device file questions: distinguish **`rootfs/` → `/`** vs **`config/*.cfg` → `/usr/share/skinos/`** (see [reference-swrt5.md](reference-swrt5.md))
4. Prefer **`./mkdel`** / **`make obj=`** over **`make clean`**
5. sdk / rootfs / makefile.config / DTS / **`kernel/` source overlays** → **`./mkdel` && `make`** → deploy **`.zz`** (**device-upgrade**); project-only → **`make obj=`** → **FPK**
6. For kernel USB modem IDs (`option.c`): edit winning `config/…/kernel/` for this `gBOARDID`; dial AT driver is **skinos-modem**
7. Point **device** deploy/upgrade to **device-upgrade**. Repo uploads (not a live device): **firmware repository** → **`make ftp`**; **FPK repository** → **`make sdk_repo`**
8. Do not invent board IDs — use `make pidlist` or the user’s active `gBOARDID`
