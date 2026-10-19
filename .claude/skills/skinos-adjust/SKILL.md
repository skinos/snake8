---
name: skinos-adjust
description: |
  Persist OpenWrt/SDK package changes (busybox, hostapd, feeds, include/*.mk,
  DTS copies, whole package trees) as files under config/<platform>/adjust,
  then copy them into the generated SDK tree via patch.sh. Use when editing
  packages inside swrt5/, wrt5/, smtk2/, mtk2/, smtk3/, srock/; when the user
  says 改 busybox, 改 openwrt 包, adjust, patch.sh, "这个打印是 busybox",
  or asks to patch a file found under those SDK directories. Those trees are
  generated at build time with no git — edits there are lost when building
  from another machine or after a fresh unpack. Do NOT use for skinos
  project/ code (skinos-project), kernel option.c overlays (skinos-sdk),
  or device upgrade (device-upgrade).
---

# Skinos SDK Adjust (OpenWrt package overlays)

OpenWrt trees at repo top (`swrt5/`, `wrt5/`, `smtk2/`, `mtk2/`, `smtk3/`, `srock/`) are **generated at build time and have no git**. A change made only there disappears on the next unpack or when compiling on another machine.

**Lasting edits go under `config/<platform>/adjust/`, then `patch.sh` copies them into the SDK.**

Kernel `.c` overlays (`option.c` + `custom.sh`) are **skinos-sdk**, not this skill.

## Hard rules

1. **Never** leave the only copy of a change in `${gSDK_DIR}` (`swrt5/package/…`, `build_dir/…`, …).
2. Read **`gBOARDID`** first → `gPLATFORM` → `config/${gPLATFORM}/adjust/`.
3. Register every new overlay in that platform’s **`adjust/patch/patch.sh`**. A file sitting in `adjust/patch/` but not copied by `patch.sh` is never applied.
4. Patches are **per platform / BusyBox version**. Do not copy a swrt5 (BusyBox 1.37) hunk onto smtk2 (1.23) without retargeting. Apply only to the platform(s) the user is building, unless they named several SDK trees.

## Layout

| Path | Role |
|------|------|
| `config/<platform>/adjust/patch/` | Canonical overlays (git-managed, often a nested repo) |
| `config/<platform>/adjust/patch/patch.sh` | Copies overlays into `${gSDK_DIR}` (`make sdk_adjust`) |
| `<platform>/` e.g. `swrt5/` | Generated OpenWrt tree (`gSDK_DIR`) — read to locate files, do not keep edits here |

`patch.sh` already has `gPLATFORM_DIR` / `gSDK_DIR`. Match its existing helper: swrt5/wrt5 use `copy_if_diff`; smtk2/smtk3/srock often use `cp`.

## Workflow

1. **Locate** the upstream file (read-only) under `${gSDK_DIR}` or `build_dir/…` so the patch context is correct.
2. **Write the overlay** under `config/${gPLATFORM}/adjust/patch/<area>/`:
   - Quilt patch for a package that already has `patches/` (BusyBox, mt76, ppp, …).
   - Whole-file replace for Makefile / `feeds.conf` / DTS / `include/*.mk` (same pattern as existing `copy_if_diff` lines).
   - Whole directory via `copy_dir_if_diff` / `cp -r` when `patch.sh` already does that (e.g. proftpd).
3. **Add a `patch.sh` line** next to similar copies. Destination must be the OpenWrt path that package actually uses, e.g.:

```bash
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/busybox/chpasswd-no-changed-syslog.patch" \
    "${gSDK_DIR}/package/utils/busybox/patches/901-chpasswd-no-changed-syslog.patch"
```

4. Custom BusyBox patches in this repo use **`900+`** names so they apply after OpenWrt’s `000–600` series.
5. New C/hunks: attribution comment in repo style (`/* add by dimmalex for … */` / `/* comment by dimmalex for no log interference */`).
6. Apply: `make sdk_adjust` (or any target that runs `patch.sh`). Firmware: **`./mkdel` → `make` → `.zz`**. Not `make obj=`.

## Quilt patch form (BusyBox example)

Paths are relative to the **unpacked package source** (not `package/utils/busybox/`):

```diff
--- a/loginutils/chpasswd.c
+++ b/loginutils/chpasswd.c
@@ -110,8 +110,11 @@
 		logmode = LOGMODE_BOTH;
 		if (rc < 0)
 			bb_error_msg_and_die("an error occurred updating password for %s", name);
-		if (rc)
-			bb_info_msg("password for '%s' changed", name);
+		/* comment by dimmalex for no log interference */
+		//if (rc)
+		//	bb_info_msg("password for '%s' changed", name);
 		logmode = LOGMODE_STDIO;
```

Older trees (smtk2) may use `--- busybox-1.23.2/loginutils/chpasswd.c` headers. Copy the style of that platform’s existing `adjust/patch/busybox/` patches.

## Do not confuse with

| Change | Where |
|--------|--------|
| `project/<name>/` skinos code | **skinos-project** (`make obj=`) |
| Linux `option.c` / kernel source overlay | **skinos-sdk** `config/…/kernel/` + `custom.sh` |
| Product `rootfs/` / `sdk.config` / `kernel.config` | **skinos-sdk** |
| Live device upgrade | **device-upgrade** |
