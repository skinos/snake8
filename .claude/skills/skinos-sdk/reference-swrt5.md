---
# swrt5 platform config reference

Detailed product customization for `gPLATFORM=swrt5` (OpenWrt-based).  
Loaded from [SKILL.md](SKILL.md) when changing board configs, rootfs, sdk.config, DTS, or default component `.cfg`.

## Hierarchy (maps from gBOARDID)

Example: `gBOARDID=swrt5-mt7981-r607` → `gSCOPE` defaults to **`std`**.

```text
config/swrt5/                          gPLATFORM_DIR
├── *.makefile, skin.mk, mkrootfs.sh, tools/, dl/, adjust/, arch/, pdriver/, rootfs/
├── mt7981/                            gHARDWARE_DIR
│   ├── kernel.config, rootfs/, config/, kernel/
│   └── r607/                          gCUSTOM_DIR
│       ├── makefile.config            ← usually here for products
│       ├── sdk.config
│       ├── dts → symlink to adjust/patch/...dts
│       ├── config/                    product default .cfg
│       └── wrt/                       gSCOPE_DIR when board ends with -wrt
│           ├── sdk.config, kernel.config, rootfs/
```

Example: `swrt5-mt7981-r607-wrt` → `gSCOPE=wrt` → use `.../r607/wrt/` when present.

**Fallthrough rule (always deepest existing file wins):**  
`gSCOPE_DIR` → `gCUSTOM_DIR` → `gHARDWARE_DIR` → `gPLATFORM_DIR`

If `.../r607/std/` does **not** exist (common), `gBOARDID=swrt5-mt7981-r607` still works: tools resolve `makefile.config` / `sdk.config` at **custom** (`r607/`) level.

OEM optional 5th field: `gOEM_DIR = .../${gCUSTOM}/${gOEM}/` with `${gOEM}.sh` + optional `config/` packed into `.zz` via `firmware-encode`.

## Platform top-level files (`config/swrt5/`)

| File / dir | Role |
|------------|------|
| `sdk.makefile` | `kernel`/`app`/`sdk_*`: sync configs into OpenWrt tree `swrt5/`, build, pack `.zz` |
| `lay.makefile` | `rootfs_prepare` / `rootfs_install`: stage `build/rootfs` |
| `com.makefile` / `lib.makefile` / `exe.makefile` | Compile one com/lib/exe with `gPLATFORM`/`gHARDWARE`/… macros |
| `skin.mk` | OpenWrt package helpers: prepare/compile/install FPK layout |
| `mkrootfs.sh` | Called first in `rootfs_prepare` (device nodes etc.) |
| `adjust/patch/` | Patches applied by `make sdk_adjust` / `sdk_menu` (`patch.sh`) — DTS, target, busybox, packages |
| `arch/` | Platform **arch** skinos project (GPIO, firmware, wifi drivers, …) — merges board `config/` |
| `pdriver/` | Portable drivers as skinos packages |
| `rootfs/` | Platform-wide rootfs overlay (e.g. banner, sysctl) |
| `kernel/` | **Kernel source overlays** + `custom.sh` (`src2kernel` → `${LINUX_DIR}`); e.g. `option.c` for modem USB serial IDs. See SKILL.md § Kernel source overlays |
| `dl/` | Cached SDK tarballs / prebuilt `.fpk` |
| `tools/` | Platform host tools |

OpenWrt tree itself is **`${gTOP_DIR}/swrt5/`** (`gSDK_DIR`), not under `config/swrt5/`.

## `makefile.config` (per product)

Included by top `target.makefile` via `gMAKEFILE_CFGFILE` (scope→custom→hardware→platform).

Typical contents (see `mt7981/r607/makefile.config`):

1. **`gSDK_CFGFILE`** — OpenWrt `.config` source (`sdk.config` fallthrough)
2. **`gpKERNEL_CFGFILE`** — Linux kconfig fragment (`kernel.config` fallthrough)
3. **`gpKERNEL_SDK_CFGFILE`** — destination inside OpenWrt tree  
   (r607: `swrt5/target/linux/mediatek/filogic/config-6.12`)
4. **`gpBOOT_CFGFILE`** — bootloader config if used
5. **`gSKIN_MKFILE`** — which `skin.mk` to use
6. **`gpUPGRADE_IMAGE`** — OpenWrt factory/sysupgrade bin that becomes `.upgrade` then `.zz`  
   (r607: `.../openwrt-mediatek-filogic-ashyelf_r607-squashfs-factory.bin`)
7. **`gLIB_CFLAGS` / `gCOM_CFLAGS` / `gEXE_*`** — include/lib paths for skinos compiles

**When to edit:** new board / change OpenWrt target image path / kernel config path / cross flags.  
After changing `sdk.config` or `kernel.config`: `./mkdel` → full `make` (not only `make obj=`).

### How `sdk.makefile` uses these

- `kernel_dep`: copy `gSDK_CFGFILE` → `swrt5/.config` if different; copy `gpKERNEL_CFGFILE` → `gpKERNEL_SDK_CFGFILE`
- `kernel`: `cd swrt5; make V=s` then `firmware-encode` → `build/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz`
- `kernel_menuconfig` / `sdk_menuconfig`: edit then **copy back** into the config repo files
- `app` with `COMPILE_PROJECT`: `package/feeds/project/<name>/compile` (or `rice`)

## Two ways to put files on the running system

### A. Rootfs overlay → image root `/`

Dirs named **`rootfs/`** at platform / hardware / custom / scope:

```text
config/swrt5/rootfs/
config/swrt5/mt7981/rootfs/
config/swrt5/mt7981/r607/rootfs/          # if present
config/swrt5/mt7981/r607/wrt/rootfs/      # scope=wrt
```

**Merge order** (`lay.makefile` `rootfs_install`): platform → hardware → custom → scope (later **overwrites** earlier).

Copied into `build/rootfs/` then into the OpenWrt rootfs. Examples:

- `etc/banner`, `etc/banner.he`, `etc/inittab`, `etc/shells`
- `etc/init.d/preland`, `etc/rc.d/S20preland`
- `etc/fw_env.config`

**`needless.sh`**: if present under a `rootfs/` that ends up in the image, OpenWrt `image.mk` **runs it inside the rootfs** before squashfs (strip unwanted `init.d`, symlink `/mnt`, then deletes itself). Customize cleanup per hardware/scope here.

Also: optional **`mkrootfs.sh`** at each layer runs during `rootfs_prepare` (before FPK install into staging rootfs).

### B. Component default config → `/usr/share/skinos/`

Dirs named **`config/`** (not `rootfs`):

```text
config/swrt5/mt7981/config/wifi/*.cfg
config/swrt5/mt7981/config/uart/*.cfg
config/swrt5/mt7981/r607/config/ifname/lte.cfg
```

**Collected by `config/swrt5/arch/Makefile`** into the **arch** FPK `config/` (platform → hardware → custom → scope, copy overlay).

`fpk-install` copies FPK `config/*` to **`PROJECT_DIR`** = **`/usr/share/skinos/`** on the rootfs.

So `config/.../config/wifi/n.cfg` → device **`/usr/share/skinos/wifi/n.cfg`** (factory default for that object). Layout is **`/usr/share/skinos/<project>/<file>`** matching project/component naming.

Runtime mutable config is still via HE/`config_set` (often under `/etc/config/...` depending on component); these `.cfg` files are the **shipped defaults** baked via arch.

### C. Files inside a skinos project

Under `project/<name>/`: `rootfs/`, `etc/`, `*.cfg` in project root — packed into that project’s FPK (`skin.mk`), then installed to `/usr/share/skinos/<name>/` or FPK `rootfs/` → `/`. Prefer board overlays in `config/swrt5/...` for **product** differences; use project tree for **feature** defaults shared by all boards.

## DTS / kernel / adjust

- Product DTS often: `gCUSTOM_DIR/dts` → symlink into `adjust/patch/<chip>/dts/*.dts`
- Kernel fragments: `kernel.config` at hardware or scope (wrt)
- `adjust/patch/<chip>/target/` — OpenWrt target/board patches
- Apply with `make sdk_adjust` (or as part of `sdk_menu`)

## `arch/` and `pdriver/`

- Built as skinos packages into `build/store/`
- `arch` merges board `config/` (above) and board-specific `odm/${gCUSTOM}/` trees
- Hotplug scripts example: `arch/usb_modem.sh` → `/etc/hotplug.d/usb/` inside arch FPK rootfs

## Customization cookbook

| Goal | Where to change | Rebuild |
|------|-----------------|---------|
| Switch product | `gBOARDID` | full `make` |
| OpenWrt package set / busybox | `.../sdk.config` (or `make sdk_menuconfig` then save back) | `./mkdel` + `make` |
| Kernel options | `kernel.config` (+ path in `makefile.config`) | `./mkdel` + `make` |
| Device tree | `dts` / `adjust/patch/.../dts` | `./mkdel` + `make` |
| `/etc/*`, init, banners | `.../rootfs/` (+ `needless.sh`) | full image (`make`) |
| Default wifi/uart/ifname JSON cfg | `.../config/<project>/*.cfg` | rebuild **arch** (full `make` or arch package) |
| Cross compile flags / image bin path | `makefile.config` | full `make` |
| OEM branding in zz | `gOEM` dir + `*.sh` + `config/` | full `make` |
| One app feature only | `project/<name>/` | `make obj=<name>` → FPK |

## Agent procedure for a given gBOARDID

1. Read `gBOARDID` → resolve `gPLATFORM_DIR` … `gSCOPE_DIR` / `gOEM_DIR`
2. Open **`makefile.config`** at deepest level — note `gSDK_CFGFILE`, `gpKERNEL_*`, `gpUPGRADE_IMAGE`
3. List overlays that exist:
   - `rootfs/` at platform/hardware/custom/scope
   - `config/` at those levels
   - `sdk.config` / `kernel.config` / `dts`
4. For “change file X on device”: decide **rootfs path** vs **`/usr/share/skinos/...` default cfg** vs **project FPK**
5. After sdk/rootfs/makefile.config changes: **`./mkdel` && `make`**, not only `make obj=`
6. Confirm on device: path under `/etc` or `/usr/share/skinos`; `fpk.list` for package location

## Note on other platforms

`smtk2` / `smtk3` / `srock` / `slave` follow the **same gBOARDID layering and fallthrough**, but makefile names, `gpUPGRADE_IMAGE`, and kernel paths differ — always read that platform’s `makefile.config` and `sdk.makefile` instead of assuming swrt5 paths.
