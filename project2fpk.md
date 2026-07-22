# Project tree → `.fpk` archive

**Sole detail for** project sources → `.fpk` (`Build/Install/fpk` / platform `skin.mk`).  
Device install paths belong in [`fpk2rootfs.md`](fpk2rootfs.md); field grammar in [`project/land/prj.json.md`](project/land/prj.json.md).

How a Skinos project under `project/<name>/` is built into a gzip-compressed tarball
`<name>-<version>-<hardware>.fpk`.

**Implementation (SDK):**

| Piece | Role |
|-------|------|
| Project `Makefile` / `fpk.makefile` | Includes `$(gSKIN_MKFILE)` (platform `skin.mk`) |
| `Package/Define` | Reads `prj.json` keys via `prj-read` |
| `Build/Prepare/Default` | Copy sources into `$(PKG_BUILD_DIR)` |
| `Build/Compile/Default` | Build lib/com/exe/ko/osc/cmd |
| `Build/Install/fpk` | Fill `$(PKG_BUILD_DIR)/.fpk` and `tar zcf` the archive |

Typical OpenWrt-style project install (see `project/tmptools/Makefile`):

```make
$(call Build/Install/fpk,$(gSTORE_DIR))
$(call Build/Install/fpk2rootfs,$(1))
```

Project parts overview: [`project.md`](project.md).  
`.fpk` format overview: [`fpk.md`](fpk.md).  
Field semantics of `prj.json`: [`project/land/prj.json.md`](project/land/prj.json.md).  
Where files land on a rootfs after install: [`fpk2rootfs.md`](fpk2rootfs.md).  
Runtime FPK APIs: [`doc/com/land/fpk.md`](doc/com/land/fpk.md).

---

## Names, version, staging dirs

From `prj.json` / `prj-read`:

| Value | Used as |
|-------|---------|
| `name` | `PROJECT_ID` / `PKG_NAME` (must match `project/<name>/`) |
| `version` | `VERSION_ID` / `PKG_VERSION` |
| Keys under `lib` `com` `exe` `osc` `cmd` `ko` | Which subdirs to copy/build/pack |
| Paths under `res` | Extra files/dirs copied to the FPK root if they exist |

Staging layout created in prepare:

```text
$(gBUILD_DIR)/$(PKG_NAME)/          # PKG_BUILD_DIR (OpenWrt: often build_dir/.../skinos_<name>)
└── .fpk/                           # FPK_BUILD_DIR
    ├── lib/
    ├── bin/
    ├── etc/
    ├── internal/
    └── rootfs/
```

Output archive name:

```text
$(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk
```

Optionally copied to `$(gSTORE_DIR)` (e.g. `build/store/`) when `Build/Install/fpk` is called with that directory.

---

## Step 1 — Prepare (`Build/Prepare/Default`)

1. Require a non-empty `VERSION_ID` (from `prj.json` `version`).
2. `mkdir -p $(PKG_BUILD_DIR)`.
3. If project root has `./lib/`, copy the whole tree into `PKG_BUILD_DIR`.
4. For each key in `lib` `com` `exe` `osc` `cmd` `ko`, if that path exists under the project root, copy it into `PKG_BUILD_DIR`.
5. Create `.fpk/{lib,bin,etc,internal,rootfs}` under `PKG_BUILD_DIR`.
6. (swrt5 `skin.mk`) remove accidental `compile_commands.json` from the build tree.

`prj.json` itself is **not** copied in this loop; it is packed later as a root `*.json` (see below).

---

## Step 2 — Compile (`Build/Compile/Default`)

Order in `skin.mk`:

1. Strip stale `*.o` / `*.a` under `PKG_BUILD_DIR`.
2. Build each **`lib`** key (`gLIB_MAKEFILE` if no local Makefile), then **`make install`** for libs.
3. Build each **`com`** (`gCOM_MAKEFILE`).
4. Build each **`exe`** (`gEXE_MAKEFILE`).
5. Build each **`ko`** (kernel `modules` against `LINUX_DIR`).
6. Build **`osc`** and **`cmd`** (same exe makefile pattern) unless a custom list is passed.

Only **directories** under `PKG_BUILD_DIR/<key>` are compiled (non-directory placeholders are skipped).

---

## Step 3 — Stage into `.fpk` and pack (`Build/Install/fpk`)

`./` below means the **project source root** (`project/<name>/`).  
Built artifacts come from `$(PKG_BUILD_DIR)`.

| Source | Destination inside `.fpk/` |
|--------|----------------------------|
| `./lib/*.so*` (if `./lib` exists) | `lib/` |
| `./bin/*` | `bin/` |
| `./etc/*` | `etc/` |
| `./internal/*` | `internal/` |
| `./rootfs/*` | `rootfs/` |
| Root `*.png` `*.jpg`, `*.json` `*.cfg` `*.html`, plus existing `res` paths | **FPK root** (includes `prj.json`) |
| Root `*.sh` `*.ash` `*.lua` | **FPK root**, `chmod a+x` |
| Each **`com`** key: `PKG_BUILD_DIR/<key>/<key>` | **FPK root** as `<key>` |
| Each **`lib`** key: `lib<key>.so` | `lib/`, plus symlink `lib<key>.so.0` → `lib<key>.so` |
| Each **`exe`** / **`osc`** key: binary `<key>` | **FPK root** |
| Each **`cmd`** key: binary `<key>` | `bin/<key>` |
| Each **`ko`** key: `*.ko` | **FPK root** |
| Each **`lib`** headers `*.h` + `lib*.so` | `install/include/<key>/`, `install/lib/` (+ `.so.0` link) |

Then (swrt5): `$(RSTRIP)` on `.fpk`.

Finally:

```sh
cd ${FPK_BUILD_DIR}
tar zcf $(PKG_BUILD_DIR)/${PROJECT_ID}-${VERSION_ID}-${gHARDWARE}.fpk *
```

Whatever sits under `.fpk/` is what `tar` extracts later.

---

## Minimal unpacked FPK example

`prj.json` with `"name":"demo"`, `com.sensor`, `cmd.cli`:

```text
prj.json
sensor                 # com binary
bin/cli                # cmd binary
lib/…                  # if any libs
install/include/…      # SDK headers (optional)
…
```

---

## Next step: install into a rootfs

Image / `make` install calls:

```make
$(call Build/Install/fpk2rootfs,$(target_rootfs))
# → fpk-install <rootfs> ${gINSTALL_DIR} <path-to>.fpk
```

See [`fpk2rootfs.md`](fpk2rootfs.md) and `tools/fpktools/fpk-install.c`.
