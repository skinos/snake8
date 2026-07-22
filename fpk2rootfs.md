# `.fpk` → target rootfs layout

**Sole detail for** `fpk-install` → target rootfs (image / SDK install).  
Packing rules: [`project2fpk.md`](project2fpk.md). Runtime hot install: [`doc/com/land/fpk.md`](doc/com/land/fpk.md).

How **`fpk-install`** lays out an FPK (or an already unpacked FPK directory) onto a target root filesystem during image build / SDK install.

**Implementation:** `tools/fpktools/fpk-install.c`  
**Path macros:** `project/land/skin/skinhead.h` (`PROJECT_*`, `FPK_*`)  
**Makefile wrapper:** `Build/Install/fpk2rootfs` in platform `skin.mk`:

```make
fpk-install $(1) ${gINSTALL_DIR} $(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk
```

Packaging (what sits inside the archive): [`project2fpk.md`](project2fpk.md).  
`.fpk` format overview: [`fpk.md`](fpk.md).  
Project parts overview: [`project.md`](project.md).  
Runtime FPK component APIs: [`doc/com/land/fpk.md`](doc/com/land/fpk.md).

---

## Command line

```text
fpk-install  <rootfs>  <sdk-install-dir>  <.fpk-or-unpacked-dir> [...]
```

| Arg | Typical value | Role |
|-----|---------------|------|
| `rootfs` | `build/rootfs` (or image root) | Device-facing tree |
| `sdk-install-dir` | `build/install` (`gINSTALL_DIR`) | Host SDK headers/libs for further builds |
| sources | `.fpk` file(s) and/or directories | Each is installed in order |

If a source is a **file**, it is unpacked with `tar zxf` into `/tmp/fpk_install_<pid>`, installed from there, then that temp dir is removed. If it is a **directory**, that tree is used as-is (must contain `prj.json` for full project install).

---

## Target paths (`skinhead.h`)

Relative names inside the package:

| Macro | Value |
|-------|-------|
| `FPK_LIB_DIR` | `lib` |
| `FPK_BIN_DIR` | `bin` |
| `FPK_ETC_DIR` | `etc` |
| `FPK_INT_DIR` | `internal` |
| `FPK_ROOTFS_DIR` | `rootfs` |
| `FPK_CONFIG_DIR` | `config` |

Absolute destinations (prefixed with `<rootfs>`):

| Macro | `gPLATFORM__slave` | Device builds |
|-------|--------------------|---------------|
| `PROJECT_DIR` | `/usr/share/skinos` | same |
| `PROJECT_ETC_DIR` | `/etc` | same |
| `PROJECT_LIB_DIR` | `/usr/local/lib` | `/usr/lib` |
| `PROJECT_BIN_DIR` | `/usr/local/bin` | `/usr/bin` |
| `PROJECT_MNT_DIR` | `/mnt/skinos` | `/mnt` |
| `PROJECT_INT_DIR` | `$PROJECT_MNT_DIR/internal` | same pattern |
| `PROJECT_APP_DIR` | `$PROJECT_MNT_DIR/internal/skinos` | same pattern |

On device builds, `PROJECT_REGISTER_LIB_DIR` / `PROJECT_REGISTER_BIN_DIR` are `/var/lib` and `/var/bin`. **`fpk-install` does not use those**; it always links into `PROJECT_LIB_DIR` / `PROJECT_BIN_DIR`.

---

## Install steps (`fpk_install`)

`name` = `prj.json` → `"name"`.  
`cmd` = `prj.json` → `"cmd"` object (used when linking binaries).

### 1. SDK develop tree (`install` arg)

If `<sdk-install-dir>` exists and the unpacked package has content under `install/include` or `install/lib`:

```text
package install/include/*  →  <sdk-install-dir>/include/
package install/lib/*      →  <sdk-install-dir>/lib/
```

### 2. Project tree under `PROJECT_DIR/<name>/`

Runs only when `rootfs` exists **and** `name` is non-NULL:

1. Ensure `PROJECT_DIR`, `PROJECT_LIB_DIR`, `PROJECT_BIN_DIR` exist under rootfs.
2. `rm -fr` then `cp -fdRp` the **entire** unpack tree to  
   `<rootfs>/usr/share/skinos/<name>/`.
3. Inside that project directory, remove staged overlays that will be merged elsewhere:  
   `install/`, `etc/`, `internal/`, `rootfs/`.
4. For each entry under project `lib/` whose name contains the substring `so`:  
   symlink  
   `<rootfs>$PROJECT_LIB_DIR/<file>` → `/usr/share/skinos/<name>/lib/<file>`  
   (link target path is absolute under the device, not relative to the host rootfs prefix used in `ln`).
5. For each entry under project `bin/`: create a symlink into `$PROJECT_BIN_DIR` **only if** that basename is a key in `prj.json` `"cmd"`.  
   Empty `lib/` / `bin/` dirs are removed from the project tree.

If `name` is missing, this whole block is skipped (comment in code: weird packages may ship only `rootfs` / `install`).

### 3. Overlay merge (always when `rootfs` exists)

Taken from the **original unpack directory** (`builddir`), not from the cleaned project tree:

| Package path | Destination under rootfs |
|--------------|--------------------------|
| `etc/*` | `$PROJECT_ETC_DIR` (`/etc`) |
| `internal/*` | `$PROJECT_INT_DIR` |
| `rootfs/*` | `/` (root of rootfs) |
| `config/*` | `$PROJECT_DIR` (`/usr/share/skinos/`), **not** `/usr/share/skinos/<name>/config/` |

---

## Mapping summary (example: `name` = `demo`, slave paths)

| Inside FPK | On target rootfs |
|------------|------------------|
| Whole tree (then strip `install`/`etc`/`internal`/`rootfs`) | `/usr/share/skinos/demo/` |
| `lib/*so*` | symlink in `/usr/local/lib/` → `/usr/share/skinos/demo/lib/…` |
| `bin/<cmd>` (key in `cmd`) | symlink in `/usr/local/bin/` → `/usr/share/skinos/demo/bin/…` |
| `etc/*` | merged into `/etc/` |
| `internal/*` | merged into `/mnt/skinos/internal/` |
| `rootfs/*` | merged into `/` |
| `config/*` | merged into `/usr/share/skinos/` |
| `install/*` | host `build/install/` only (not kept under the project dir) |

Device builds use `/usr/lib` and `/usr/bin` instead of `/usr/local/...`, and `/mnt/internal` instead of `/mnt/skinos/internal`.

---

## Relation to runtime / hot install

`fpk-install` is only the **image-integration** path (`Build/Install/fpk2rootfs`).  
Runtime / hot deploy (`land@fpk`, web `api=fpk`, `PROJECT_APP_DIR`) is documented in [`doc/com/land/fpk.md`](doc/com/land/fpk.md) — do not assume the same `etc` → `/etc` merge as this tool.

---

## See also

- [`project2fpk.md`](project2fpk.md) — how the archive is filled  
- [`fpk.md`](fpk.md) — `.fpk` archive overview  
- [`project.md`](project.md) — Skinos project tree and parts  
- [`project/land/prj.json.md`](project/land/prj.json.md) — manifest fields (`name`, `cmd`, …)  
- [`doc/com/land/fpk.md`](doc/com/land/fpk.md) — `land@fpk` behavior on a running device  
