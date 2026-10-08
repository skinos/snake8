# Firmware Build — `wrt5-mt7628-r600` (std)

Download the Snake8 SDK and build a **Skinos / landos gateway** **`.zz` firmware image** for the
**R600** product on **MT7628**, **standard (std)** scope.

This walkthrough uses board identity:

```text
gBOARDID=wrt5-mt7628-r600
```

Parsed fields:

| Field | Value | Resolves to |
|-------|-------|-------------|
| platform | `wrt5` | OpenWrt-based Skinos build; SDK tree `wrt5/` |
| chip | `mt7628` | RAMIPS / MT76x8 target |
| product | `r600` | Product layer under `config/wrt5/mt7628/r600/` |
| scope | `std` | Standard edition — **no** 4th segment; Makefile defaults `gSCOPE` to `std` |

> **Do not** write `-std` into `gBOARDID`. A physical `std/` directory under the product is
> **not** required; config fallthrough uses the product dir (and chip/platform files).

### What this firmware is

| Topic | Detail |
|-------|--------|
| **Image type** | **Skinos / landos gateway** firmware (component model, HE APIs, Skinos Web UI). |
| **Upgrade file** | The **`build/*.zz`** file is the deliverable used to upgrade an **R600** device. |
| **After upgrade** | Default LAN IP is **`192.168.8.1`** (`project/ifname/lan.cfg`). Open the Skinos Web UI at `http://192.168.8.1/`. |
| **Inside `.zz`** | `.zz` is an encoded wrapper around the OpenWrt **sysupgrade** image (`.upgrade`). Extracting / decoding it yields the sysupgrade binary — also present before encoding as `build/mt7628_r600_std_<version>.upgrade` and under `wrt5/bin/targets/ramips/mt76x8/*.bin`. |

The **`-wrt`** scope builds **stock OpenWrt (LuCI)** instead of Skinos — see
[`r600-openwrt-build.md`](r600-openwrt-build.md). List all IDs with `make pidlist`.

---

## 1. Requirements

| Item | Notes |
|------|-------|
| **Host OS** | Ubuntu 20.04 or 24.04 (64-bit) recommended |
| **Tools** | `git`, `make`, `sudo` |
| **Disk** | Allow **≥ 50 GB** free under the SDK root (OpenWrt tree + `build_dir` grow large) |
| **Network** | `make update` downloads OpenWrt tarballs and FPK packages via internal `repo-update` |
| **Time** | First full `make` on wrt5 often takes **1–3 hours** depending on CPU and cache |

Cross-compilers and most dependencies are pulled by `make preset` and the OpenWrt SDK
unpack step — no separate toolchain install is required for this board.

---

## 2. Download the SDK

### 2.1 Main repository

```bash
# Gitee
git clone https://gitee.com/snake8/snake8.git
cd snake8

# GitHub
git clone https://github.com/skinos/snake8.git
cd snake8
```

The **wrt5** platform configuration ships in the main tree under `config/wrt5/` — no
separate platform repo clone is required.

---

## 3. Install build tools

Run once per host:

```bash
make preset
```

This installs compilers, libraries, and utilities listed in `misc.makefile` (requires
`sudo`).

---

## 4. Select board identity

Persist the R600 **std** product model (writes the `gBOARDID` file):

```bash
make pid gBOARDID=wrt5-mt7628-r600
```

Verify:

```bash
make pidinfo
cat gBOARDID
```

Expected:

```text
gBOARDID=wrt5-mt7628-r600
```

> **Note:** `gBOARDID` is gitignored and local to your tree. Always check it before
> building — do not assume the board from another machine or an old session.

---

## 5. Update OpenWrt SDK and packages

On first setup, or after switching wrt5 hardware/product, refresh the platform SDK:

```bash
make update
```

What this step does:

1. **`git pull`** on the top-level snake8 repo (includes `config/wrt5/`)
2. **`sdk_update`** — if `wrt5/` is missing, download and unpack OpenWrt SDK, download
   cache, and feeds from the firmware repository (`repo-update wrt5 mt7628 r600 …`)
3. **`sdk_adjust`** — apply persistent patches from `config/wrt5/adjust/patch/`
4. **`sdk_menu`** — update/install OpenWrt feeds for `project` and `rice`

After a successful run you should have:

| Path | Content |
|------|---------|
| `wrt5/` | **OpenWrt-based SDK tree** used by Skinos (`gSDK_DIR`) |
| `config/wrt5/dl/` | Downloaded tarballs and FPK cache |
| `config/wrt5/mt7628/r600/` | Product layer (`makefile.config`, `rootfs/`, `config/`, …) |
| `config/wrt5/mt7628/sdk.config` | Chip-level OpenWrt config used when the product has no `sdk.config` (R600 std) |

The **`wrt5/`** directory at the SDK root is unpacked by `make update`. Product-specific
settings for R600 **std** live under `config/wrt5/…` and are applied when you run `make`.

Sync platform config with the main repo when needed:

```bash
git pull
```

---

## 6. Build firmware

For a normal full firmware image:

```bash
./mkdel
make
```

**Do not use `make clean`** for routine work — it forces a very slow full wipe.
`./mkdel` clears incremental OpenWrt/skinos build state under `wrt5/build_dir/` and is
enough before most rebuilds.

Internal build order:

```text
dep → kernel → app → kernel_install → app_install
```

- **`kernel`** — cross-compile the OpenWrt image, write `.upgrade`, encode **`.zz`**
- **`app`** — build Skinos projects into FPKs and stage into the rootfs

For **std**, the result is a Skinos gateway image (HE / WUI), not stock OpenWrt LuCI.

---

## 7. Build outputs

After a successful build:

| Artifact | Location | Purpose |
|----------|----------|---------|
| **Firmware image (upgrade)** | `build/mt7628_r600_std_<version>.zz` | Upload to **R600** for firmware upgrade |
| OpenWrt sysupgrade (plain) | `build/mt7628_r600_std_<version>.upgrade` | Raw image **before** `.zz` encoding |
| OpenWrt sysupgrade (build tree) | `wrt5/bin/targets/ramips/mt76x8/openwrt-ramips-mt76x8-ashyelf_d218-squashfs-sysupgrade.bin` | Same image inside the OpenWrt build output |
| FPK packages | `build/store/*.fpk` | Per-project packages (hot-deploy eligible when not land/arch) |
| Build log | `build/mt7628_r600_std.txt` | Version / image metadata |

`<version>` comes from `gPUBLISH` / `gVERSION` in the top `Makefile` (for example
`v8.6.0920`), so a typical filename is:

```text
build/mt7628_r600_std_v8.6.0920.zz
build/mt7628_r600_std_v8.6.0920.upgrade
```

The **`.upgrade`** file is the OpenWrt sysupgrade firmware. The **`.zz`** file wraps that
binary for distribution and device upgrade.

List results:

```bash
ls -lh build/*.zz build/*.upgrade build/store/*.fpk
```

### Upgrade an R600 device

Flash or OTA-upload the **`.zz`** file built for `wrt5-mt7628-r600`. After the device
reboots:

- Default IP: **`192.168.8.1`**
- Web UI: **`http://192.168.8.1/`** (Skinos WUI)
- Default login is typically **`admin`** (confirm on device / product docs)

Connect your PC to the device LAN and set a static address on the same subnet (for
example `192.168.8.100/24`) if DHCP is not yet available.

For upload via the gateway Web API or recovery workflow, see
[`../../../.claude/skills/device-upgrade/SKILL.md`](../../../.claude/skills/device-upgrade/SKILL.md).

---

## 8. Rebuild after changes

| What you changed | Command | Deploy on R600 |
|------------------|---------|----------------|
| `config/wrt5/…` (sdk/kernel/rootfs/DTS), `project/land/`, `config/*/arch/` | `./mkdel && make` | Upload new **`.zz`** only (no FPK for land/arch) |
| Single non-core project (e.g. `modem`, `uart`, `ifname`) | `./mkdel` → `make obj=<name>` | Upload `build/store/<name>-….fpk` via `arch@firmware.fpk` |
| OpenWrt package selection / full image | `./mkdel && make` | Upload new **`.zz`** |

```bash
./mkdel && make
```

> **Note:** Stock OpenWrt for this product uses `wrt5-mt7628-r600-wrt` — see
> [`r600-openwrt-build.md`](r600-openwrt-build.md).

---

## 9. Quick reference

```bash
# One-time / refresh
git clone https://gitee.com/snake8/snake8.git && cd snake8
make preset
make pid gBOARDID=wrt5-mt7628-r600
make update

# Firmware
./mkdel && make
ls build/*.zz build/*.upgrade

# Single app FPK (not land/arch)
./mkdel && make obj=<name>
ls build/store/<name>-*.fpk

# Board helpers
make pidlist
make pidinfo
```

---

## 10. Troubleshooting

| Symptom | Check |
|---------|-------|
| `make pid` fails / empty `pidlist` | `config/wrt5/` missing — re-clone or `git pull` snake8 |
| `make update` / `repo-update` fails | Network access to firmware repo; proxy/firewall |
| Wrong product flashed | `cat gBOARDID` must be `wrt5-mt7628-r600` before `make` |
| Got LuCI instead of Skinos WUI | You built a **`-wrt`** scope — use this std `gBOARDID`, not `…-r600-wrt` |
| Stale kernel/rootfs after config edit | Run `./mkdel` before `make`, not `make obj=` alone |
| land/arch change not visible after FPK | land/arch require full **`.zz`** — FPK hot-deploy will not update them |

---

## 11. References

| Topic | Doc |
|-------|-----|
| Stock OpenWrt (LuCI) for R600 | [`r600-openwrt-build.md`](r600-openwrt-build.md) |
| SDK layout, `mkdel`, config layers | [`../../../.claude/skills/skinos-sdk/SKILL.md`](../../../.claude/skills/skinos-sdk/SKILL.md) |
| wrt5 platform config | [`../../../config/wrt5/`](../../../config/wrt5/) |
| Switch product / scope | [`../../../.claude/skills/skinos-board/SKILL.md`](../../../.claude/skills/skinos-board/SKILL.md) |
| Flash `.zz` / FPK to a live gateway | [`../../../.claude/skills/device-upgrade/SKILL.md`](../../../.claude/skills/device-upgrade/SKILL.md) |
| Project development | [`../project.md`](../project.md) |
| HE / runtime | [`../../com/land/he.md`](../../com/land/he.md) |
| README (build overview) | [`../../../README.md`](../../../README.md) |
