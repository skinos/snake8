# FPK package format

An **FPK** is the standard delivery unit for a **Skinos project**: file extension **`.fpk`**, content is a **gzip-compressed tar**. After unpack, the tree root sits next to **`prj.json`**; layout matches the packaging staging directory (typically **`build/<name>/.fpk`**).

Project layout and `prj.json` parts: [`project.md`](project.md).  
Project tree → archive rules: [`project2fpk.md`](project2fpk.md).  
Field semantics: [`project/land/prj.json.md`](project/land/prj.json.md).  
Install into a target rootfs: [`fpk2rootfs.md`](fpk2rootfs.md).

> **Not the same as** [`doc/com/land/fpk.md`](doc/com/land/fpk.md) — that document is **`land@fpk`** (runtime install APIs). This file describes the **`.fpk` archive** and image-integration behavior.

---

## Artifact name

```text
<name>-<version>-<hardware>.fpk
```

**name** and **version** come from `prj.json` (`name`, `version`). **hardware** comes from the current board/product config (`gHARDWARE`). The archive is produced under the project build tree; copying into `build/store/` is optional (when `Build/Install/fpk` is given a store directory).

---

## What the archive contains

Logically: **`prj.json`**, compiled artifacts selected by `prj.json` sections, and project-tree resources copied by packaging rules.

| Kind | In the archive |
|------|----------------|
| `prj.json` | Required at package root (packed with other root `*.json`) |
| `com` keys | Unsuffixed module named like the key, at **package root** |
| `lib` keys | `lib/lib<key>.so` (+ `.so.0` link); headers/libs also under `install/` for the SDK |
| `cmd` keys | `bin/<key>` |
| `exe` / `osc` keys | Executable at **package root** |
| `ko` keys | `*.ko` at **package root** |
| Project `lib/` `bin/` `etc/` `internal/` `rootfs/` | Same-named dirs inside the package when present |
| Root `*.cfg` `*.html` `*.sh` … + `res` paths | Package root (or named paths from `res`) |

Step-by-step packaging: [`project2fpk.md`](project2fpk.md).

---

## Install and image integration

During image build, install is usually:

```text
fpk-install  <target-rootfs>  <build/install>  <.fpk>
```

That expands the payload under the configured project root (commonly `/usr/share/skinos/<name>/`), merges `etc` / `internal` / `rootfs` / `config`, and creates symlinks for `lib` / `bin`. Details: [`fpk2rootfs.md`](fpk2rootfs.md).

If a runtime path places the unpacked tree under an extension area (e.g. `PROJECT_APP_DIR` / `…/internal/skinos/<name>/`) **without** merging `etc` → `/etc`, layout semantics differ from image integration — keep those scenarios separate when debugging.

---

## Related docs

| Doc | Topic |
|-----|--------|
| [`project.md`](project.md) | On-disk project parts |
| [`project2fpk.md`](project2fpk.md) | How the archive is filled |
| [`fpk2rootfs.md`](fpk2rootfs.md) | `fpk-install` → rootfs mapping |
| [`project/land/prj.json.md`](project/land/prj.json.md) | Full `prj.json` field reference |
| [`doc/com/land/fpk.md`](doc/com/land/fpk.md) | `land@fpk` on a running device |
