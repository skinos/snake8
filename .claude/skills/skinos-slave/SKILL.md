---
name: skinos-slave
description: |
  Compile, install, run, stop, and uninstall slave-* boards on the SDK host itself.
  Slave models run locally on the build server (not a remote gateway flash).
  Use when gBOARDID starts with slave-, or the user says 测试slave, 编译slave,
  本机运行, 安装到本机, sdk_install, sdk_start, sdk_stop, sdk_uninstall,
  make rebuild, or asks to build/install/run/stop/uninstall after compiling slave.
  Do NOT use for swrt5/smtk2/smtk3/srock remote devices (device-upgrade)
  or for general Makefile / board-switch docs (skinos-sdk / skinos-board).
---

# Slave Host — Compile / Install / Run / Stop / Uninstall

`gBOARDID` starting with **`slave-`** is a **host/debug** model. It runs on the
**same Ubuntu server that holds the SDK**. Do not upload `.zz` / FPK to a
remote box to “test slave”.

Human first-time install (clone / `preset` / `update`): `config/slave/install.md`.  
This skill is the **agent lifecycle** after the tree is already on the server.

CWD is always the **SDK root** (directory with top-level `Makefile` and `gBOARDID`).

## Command map

| Command | Meaning |
|---------|---------|
| `make dep; make;` | 编译 |
| `make sdk_install` | 安装，编译后要安装到本机 |
| `make sdk_start` | 运行，运行后就可以测试了 |
| `make sdk_stop` | 停止 |
| `make sdk_uninstall` | 卸载 |
| `make rebuild` | 编译安装运行 |

`make rebuild` is `make sdk_stop; make sdk_uninstall; make; make sdk_install; make sdk_start` (`misc.makefile`).
`sdk_uninstall` removes the hot-installed FPK tree `/mnt/skinos/internal/skinos` only. `/mnt/skinos/dbs` and the rest of `/mnt/skinos` stay.
Bare `make` already begins with `make dep` when `obj` is empty.

Aliases: `make install` → `sdk_install`, `make start` → `sdk_start`, `make stop` → `sdk_stop`.

**Do not use `make clean`.**

## When this applies

1. Read `gBOARDID`.
2. **`slave-*`** → this skill. Typical ids: `slave-x86-ubuntu2004`, `slave-x86-ubuntu2404`.
3. Any other platform (`swrt5-`, `smtk2-`, `smtk3-`, `srock-`, …) → **device-upgrade** (remote `.zz` / FPK).

If the user asked to test **slave** but the file is a device board: pass
`gBOARDID=slave-x86-ubuntu2404` (or `ubuntu2004` for that host) on **every**
`make` / `rebuild`. Do **not** `make pid` or edit the `gBOARDID` file unless
the user wants a persistent switch (**skinos-board**).

## Agent workflow

When the user asks to compile and/or test slave:

```
Task progress:
- [ ] 1. Confirm slave-* gBOARDID (or pass it on the make line)
- [ ] 2. Compile (or use make rebuild for compile+install+run)
- [ ] 3. Install to this host if compile was separate
- [ ] 4. Start if install was separate
- [ ] 5. Test on this host (he / local WUI)
```

### Compile

```bash
make dep; make;
```

`make dep` wipes `build/` (including `build/install` headers). The matching
compile is a **full** `make`, not `make obj=<name>`.

`make obj=<name>` only builds a store FPK. It does **not** stage `build/rootfs`
and does **not** update the host. Do not use it as the slave install/run path.

### Install (required after compile)

```bash
make sdk_install
```

Needs **sudo**. Copies staged `build/rootfs` onto this machine, then `ldconfig`.

What it does (`config/slave/sdk.makefile`):

1. Run `/usr/share/skinos/shut.sh` if present (stops a previous instance).
2. `sudo rm -fr /tmp/skinos /var/skinos /usr/share/skinos`
3. Copy `build/rootfs/usr/share/skinos` → `/usr/share/skinos`
4. Copy `build/rootfs/usr/local/lib/*` → `/usr/local/lib`
5. Copy `build/rootfs/usr/local/bin/*` → `/usr/local/bin`  
   **Skipped** when `/usr/prj` exists (tiger7). Then use
   `/usr/share/skinos/land/bin/{he,daemon}`.
6. `chmod u+s` on `/usr/share/skinos/agent/gtog` if that file exists.

Does **not** touch `/mnt/skinos`. Hot-installed FPKs under `/mnt/skinos/internal/skinos` are removed by **`sdk_uninstall`** (and thus by **`make rebuild`**). `/mnt/skinos/dbs` stays.

### Run — then test

```bash
make sdk_start
```

Runs `/usr/share/skinos/setup.sh`. After it returns, the instance is live —
**start debugging immediately**. Do not reboot the SDK server.
Do not `land@machine.restart` unless a later step truly needs it.

### One shot (compile + install + run)

```bash
make rebuild
```

Use this when the user wants 编译安装运行 in one go.

### Stop

```bash
make sdk_stop
```

Runs `shut.sh`, then removes `/tmp/skinos` and `/var/skinos`.
Keeps `/usr/share/skinos` and `/mnt/skinos`.

### Uninstall

```bash
make sdk_uninstall
```

Stop, then `sudo rm -fr /tmp/skinos /var/skinos /usr/share/skinos /mnt/skinos/internal/skinos`, then `ldconfig`. `/mnt/skinos/dbs`, `config`, `.oem`, and the rest of `internal` stay.

## Test on this host

After `sdk_start` (or `rebuild`), test **locally**. Do not invent a remote
device URL and do not use **device-upgrade** upload.

```bash
he 'land@machine.status'
```

If `he` is not on `PATH` (tiger7 skip of `/usr/local/bin`):

```bash
/usr/share/skinos/land/bin/he 'land@machine.status'
```

Success: valid JSON and `version` matches `^v[0-9]`. Slave often runs
`mode=parasite`. HE grammar: **skinos-he**.

Admin WUI default on this platform: **`http://127.0.0.1:20001`**
(`config/slave/config/wui/admin.cfg`). Confirm live ports from HE
(`wui@admin`, `center@userwui`, …) if the page does not match.

Credentials: use what the user gave this session. Default `admin` / `admin`
only if they said so.

HTTP `/public` `/auth` `/he` against **localhost** is fine after start.
Do **not** treat that as a remote firmware/FPK upgrade.

## Do not confuse

| Situation | Skill |
|-----------|--------|
| `slave-*` compile / install / run / stop / uninstall on this server | **this skill** |
| Remote gateway `.zz` / FPK (`swrt5` / `smtk*` / `srock` / user gave `http://IP:port`) | **device-upgrade** |
| `gBOARDID` / `make pid` / temporary `gBOARDID=slave-…` on the make line | **skinos-board** |
| SDK layout, `mkdel`, `make obj=`, `make ftp` / `sdk_repo` | **skinos-sdk** |

## Gotchas

- Always run from SDK root, not `config/slave/center` or another project dir.
- `make dep` then `make obj=center` **fails** (`skin/skin.h` missing). After `dep`, only a full `make`.
- `sdk_install` already stops the old instance. After a new compile: install, then start (or `rebuild`).
- `sdk_install` does not touch `/mnt/skinos`. `sdk_uninstall` (and `rebuild`) removes only `/mnt/skinos/internal/skinos`. `/mnt/skinos/dbs` stays.
- Do not `make clean`. Do not flash `.zz` to a field unit to validate slave.
