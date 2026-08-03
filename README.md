# Snake8 / landos — Embedded Gateway SDK

This repository is an **embedded Linux gateway and device-management SDK**. On top of Linux it adds a **component model**: each feature is a named object (`land@machine`, `forward@nat`, …) with **JSON configuration** and **callable APIs**. Day-to-day control uses one **HE grammar** everywhere — interactive terminal, shell scripts, Web admin, and remote agents.

In docs, this stack is often called **landos**; the shared C library is **skinos**.

Chinese portal: [`README.cn.md`](README.cn.md).

---

## Why use it

- **One control surface** — view/set config and call APIs with the same HE syntax (`eline`, `he '…'`, Web, agents).
- **Ship features as FPK** — each `project/<name>/` builds to an installable `.fpk` (like an app package for the device).
- **Lifecycle built in** — boot stages (`init`), events (`joint`), shutdown (`uninit`) declared in `prj.json`.
- **Multi-board** — one tree, many products via `gBOARDID` → `config/<platform>/…`.
- **Composable domains** — routing, Wi‑Fi, modem, VPN, UART, … register into one runtime instead of one-off scripts.
- **AI-ready workflow** — structured projects (`prj.json`), HE, FPK, and agent skills (`AGENTS.md`, `.claude/skills/`) so AI can author, build, upgrade, debug, and maintain the stack with less ad-hoc scripting.

---

## Mental model

| Layer | Role |
|-------|------|
| **Linux** | Kernel, drivers, usual userspace |
| **Component** | `project@name` — JSON config + methods |
| **HE** | Text grammar to get/set config and call methods |
| **FPK** | Delivery unit for one Skinos project |
| **Lifecycle** | `init` / `joint` / `uninit` run registered HE calls |

---

## Learning path (short)

1. Pick a board with `make pidlist`, then `make pid gBOARDID=…` (or keep default `slave-x86-ubuntu2004` for host debug).
2. `make preset` (Ubuntu deps) → `make dep` → `make` (or `make kernel` / `make app` as needed).
3. Run / flash the image; log in — default UI is **`eline`** (prompt `$ `).
4. Try `land@machine` and `land@machine.status` (no `he` prefix in eline).
5. Optional: change a project → `./mkdel` → `make obj=<name>` → hot-deploy the `.fpk`.

---

## Repository map

| Path | Purpose |
|------|---------|
| [`Makefile`](Makefile) | Top build: `dep`, `kernel`, `app`, install into `build/` |
| `gBOARDID` | Active board id (gitignored); list with `make pidlist` |
| `config/<platform>/` | Toolchain, kernel, rootfs overlays, product glue (often separate git repos) |
| `project/<name>/` | Feature domains: sources + `prj.json` → FPK |
| `build/` | Outputs: `install/`, `rootfs/`, `store/*.fpk`, `*.zz` images |
| `doc/com/` | Component API docs (by domain) |
| `tools/` | Host helpers (`fpk-install`, packaging, …) |
| `rice/` | Optional customer overlays |

**Root packaging / project docs:** [`project.md`](project.md) · [`projects.md`](projects.md) · [`fpk.md`](fpk.md) · [`project2fpk.md`](project2fpk.md) · [`fpk2rootfs.md`](fpk2rootfs.md)

Many `project/*` and `config/*` trees are **separate git repos**. Helpers: `gitst`, `gitup`, `mkdel`.

---

## Build

**Board id** (`platform-chip-board[-scope][-oem]`), e.g. `swrt5-mt7981-r607`. Default if unset: `slave-x86-ubuntu2004`.

```bash
# Inspect / set board
make pidlist              # list switchable gBOARDID (scanned from config/)
make pid gBOARDID=slave-x86-ubuntu2004
make pidinfo

make preset               # Ubuntu host packages
make dep                  # prepare build/ + stage FPKs into rootfs tree
make                      # full: kernel + app + install → firmware
# or
make kernel && make app
make obj=land             # one project → build/store/<name>-<ver>-<hw>.fpk
```

**Incremental workflow (important):** prefer `./mkdel` then rebuild; **avoid `make clean`** (full rebuild is very slow).

| Output | Meaning |
|--------|---------|
| `build/store/*.fpk` | Per-project packages (hot-deployable) |
| `build/*.zz` | Full firmware image (typical device upgrade) |
| `build/rootfs/` | Assembled rootfs (`usr/share/skinos/…`) |
| `build/install/` | Staged headers/libs for further builds |
| `doc/dev/include/` | Headers refreshed after a successful build |

Deeper board / `config/swrt5` customization: [`.claude/skills/skinos-sdk/SKILL.md`](.claude/skills/skinos-sdk/SKILL.md). Slave host walkthrough: [`config/slave/readme.md`](config/slave/readme.md) (if present).

Useful extras: `make update`, `make rebuild`, `make menuconfig`, `make tftp` / `make ftp` (see `misc.makefile` / platform docs).

---

## Develop

A directory under `project/` with **`prj.json`** is a Skinos project (`name` must match the folder).

1. Read [`project.md`](project.md) (parts) and [`project/land/prj.json.md`](project/land/prj.json.md) (fields).
2. Copy [`project/tmptools/`](project/tmptools/) as a template; rename and edit `prj.json`.
3. Build: `./mkdel` → `make obj=<name>`.
4. C APIs: `#include "skin/skin.h"` — [`doc/com/land/skin.md`](doc/com/land/skin.md).
5. Authoring checklist: [`.claude/skills/skinos-project/SKILL.md`](.claude/skills/skinos-project/SKILL.md), [`project/AGENTS.md`](project/AGENTS.md).

Pack pipeline: [`project2fpk.md`](project2fpk.md) → image install [`fpk2rootfs.md`](fpk2rootfs.md).

---

## Manage on the device

| Path | When |
|------|------|
| **`eline`** (`$ `) | Default after Telnet / SSH / serial — type HE **without** `he` |
| **`he '…'`** | After `ashy` (BusyBox ash) or in scripts — **always single-quote** |
| **Web admin** | Same JSON/APIs via pages registered in `wui` |
| **Agent** | Remote HE; see [`doc/com/agent/`](doc/com/agent/) |

```text
$ land@machine                 # view config
$ land@machine:name=gw1        # set one field
$ land@machine.status          # call API
$ ashy                         # drop to shell
# he 'land@machine.status'
```

Discover: `@` (components), `component.` (APIs) — details in [`eline.md`](doc/com/land/eline.md).

Hot-deploy an FPK (no reboot): build `make obj=<name>`, upload via Web `arch@firmware` `api=fpk`. Full remote upgrade flow: [`.claude/skills/device-upgrade/SKILL.md`](.claude/skills/device-upgrade/SKILL.md).

---

## Unified interface (HE)

Three operations cover almost all management:

| Goal | Form |
|------|------|
| Read config | `project@component` · `project@component:path` |
| Write config | `project@component:path=value` · `project@component\|{…}` · `project@component={…}` |
| Call method | `project@component.method` · `project@component.method[a,b]` · `…:field` (pick JSON field) |

Full grammar and return sentinels (`ttrue` / `tfalse` / …): [`doc/com/land/he.md`](doc/com/land/he.md). Interactive extras (`set`, `ashy`): [`doc/com/land/eline.md`](doc/com/land/eline.md).

---

## Packaging (FPK)

An **`.fpk`** is a gzip tar of one project. Overview: [`fpk.md`](fpk.md). How the archive is filled: [`project2fpk.md`](project2fpk.md). How `fpk-install` lays it into a rootfs: [`fpk2rootfs.md`](fpk2rootfs.md). Runtime APIs: [`doc/com/land/fpk.md`](doc/com/land/fpk.md) (`land@fpk`).

---

## Projects in this tree

**land** is the management base; **network** / **ifname** / **wifi** / **modem** provide connectivity; **forward** / **client** sit on top; **wui** / **tui** / **agent** expose the same HE surface.

Full layering, **where-to-add** table, and modem/uart skills: [`projects.md`](projects.md).  
- Cellular USB module driver → [`.claude/skills/skinos-modem/`](.claude/skills/skinos-modem/)  
- Serial protocol / DTU / NMEA app → [`.claude/skills/skinos-uart/`](.claude/skills/skinos-uart/)

---

## Documentation index

| Area | Start here |
|------|------------|
| **This portal** | [`README.md`](README.md) · [`README.cn.md`](README.cn.md) |
| **landos core** | [`doc/com/land/README.md`](doc/com/land/README.md) — eline, he, init/joint/uninit, machine, auth, skin, … |
| **Project authoring** | [`project.md`](project.md) · [`project/land/prj.json.md`](project/land/prj.json.md) |
| **Domain catalog** | [`projects.md`](projects.md) · modem: [skinos-modem](.claude/skills/skinos-modem/) · uart: [skinos-uart](.claude/skills/skinos-uart/) · `doc/com/<domain>/` |
| **Packaging** | [`fpk.md`](fpk.md) · [`project2fpk.md`](project2fpk.md) · [`fpk2rootfs.md`](fpk2rootfs.md) |
| **Product / how-to** | [`doc/product/`](doc/product/) · [`doc/use/`](doc/use/) |
| **Agent / CI helpers** | [`AGENTS.md`](AGENTS.md) · [`.claude/skills/`](.claude/skills/) |

**Rule of thumb:** code in `project/<name>/` · API docs in `doc/com/<topic>/` · manifest in that project’s `prj.json`.
