# AGENTS.md — Snake8 / landos Embedded Gateway SDK

## What this is

Embedded Linux gateway SDK with a **component model**. Each feature is a named component (e.g. `land@machine`, `forward@nat`) with JSON config and callable APIs. The stack is called **landos**; shared libraries are **skinos**.

## Build system

**Platform selection** via `gBOARDID` file (format: `platform-chip-board[scope][oem]`):
- Default: `slave-x86-ubuntu2004` (debug/host build)
- Active board: check `gBOARDID` file; template in `gBOARDID.txt`
- Drives `config/<platform>/` resolution

**Key commands**:
```bash
make dep          # Prepare build dirs, install fpk to rootfs
make kernel       # Build kernel/SDK
make app          # Build all projects
make obj=<name>   # Build single project (e.g. make obj=land)
make clean        # Clean everything
make clean obj=<name>  # Clean single project
make preset       # Install Ubuntu build dependencies
make clangd       # Generate compile_commands.json for IDE
```

**Build workflow (IMPORTANT — do NOT use `make clean`)**:
- **修改单个项目代码** (如 ipsec): `./mkdel` → `make obj=ipsec` → 生成 `build/store/<name>-<ver>.fpk`
- **修改 sdk.config**: `./mkdel` → `make` → 生成完整固件 `build/*.zz`
- **禁止使用 `make clean`** — 会导致全量重编，耗时极长。`./mkdel` 足够清理增量状态

**FPK 热部署**: FPK 文件可通过 web API 安装到运行中的设备，无需重启
```bash
# 上传 FPK（与固件升级相同的接口，api=fpk）
curl -X POST "http://<device>/upload?username=admin&key=<key>&object=arch@firmware&api=fpk&p=[]" -F "filename=@build/store/<name>.fpk"
```

**Build order**: `dep → kernel → app → kernel_install → app_install`

**Outputs**:
- `build/install/` — staged headers/libs
- `build/rootfs/` — assembled rootfs (projects at `usr/share/skinos/`)
- `build/store/` — FPK packages (per-project `.fpk` files)
- `build/*.zz` — full firmware images

**FPK package build** (single project, hot-deployable):
```bash
make obj=<name>              # Build project → build/store/<name>-<ver>-<hw>.fpk
```
FPK files can be uploaded to a running device without restart. Use `arch@firmware.fpk` API.

## Device upgrade & remote testing

See **`.claude/skills/device-upgrade/SKILL.md`** for the full workflow.

Quick summary:
1. **Authenticate**: `POST /public` → get `rand` → compute `MD5(pass:user:rand)` → base64 → `POST /auth` → get `key`
2. **Full firmware (.zz)**: `POST /upload?object=arch@firmware&api=zz` → restart → wait 120s
3. **FPK package**: `make obj=<name>` → `POST /upload?object=arch@firmware&api=fpk` → no restart
4. **Test APIs**: `POST /he` with `{"username":"admin","key":"...","he":"<project>@<instance>.<api>"}`

## Certificate handling convention

Components with TLS certificates (UART, IPsec, etc.) follow this pattern:

| Item | Convention |
|------|-----------|
| Extensions | `.ca` (CA cert), `.crt` (client cert), `.key` (private key) |
| Naming | `config_path(PROJECT_ID, "%s.crt", object)` |
| JSON key in `key[]` | `"ca"`, `"crt"`, `"key"` |
| HE APIs | `import_ca[file]`, `import_cert[file]`, `import_key[file]`, `clear_ca[]`, `clear_cert[]`, `clear_key[]`, `key[]` |
| HTML element IDs | `<prefix>_ca`, `<prefix>_crt`, `<prefix>_key` with `_down`/`_remove` |
| Upload URL | `/upload?...&object=<obj>&api=import_ca&p=[]` |
| Delete command | `<obj>.clear_ca` |

## Project structure

Each `project/<name>/` contains:
- `prj.json` — manifest (required): defines components, libs, executables, boot lifecycle
- Source code (C components, libraries, daemons)
- Makefile

**prj.json key fields**:
- `com` — subdirectories that become components (`project@subdir`)
- `init` — boot level → API calls (levels: `arch`, `land`, `app`, `general`, `manage`, `delay`)
- `joint` — event → API calls (e.g. `network/on`, `storage/insert`)
- `obj` — aliases (e.g. `land@joint` → `init`)

## Device interaction

**`eline`** — interactive terminal UI (prompt `$ `), type HE lines directly
**`he`** — run one HE line from shell: **always single-quote**: `he 'land@machine.info'`

**Getting a shell on the device**: telnet 进入 eline 后，输入 `ashy` 回车即可进入 BusyBox ash shell
```bash
telnet <ip> <port>
# 输入用户名密码后进入 eline ($ 提示符)
$ ashy
# 进入 shell (# 提示符)
/ # ls /usr/lib/ipsec/charon
/ # exit  # 回到 eline
```

HE grammar docs: `doc/com/land/he.md`, `doc/com/land/eline.md`

## Directory map

| Path | Purpose |
|------|---------|
| `config/` | Per-platform toolchains, kernel, rootfs overlays |
| `project/` | All implementations and packages |
| `doc/com/land/` | Core docs (component model, lifecycle, HE grammar) |
| `tools/` | Host-side build helpers (fpk packaging) |
| `rice/` | Customer-specific overlays (optional) |
| `build/` | Build outputs (gitignored) |

## Config platforms

| Platform | Description |
|----------|-------------|
| `slave` | Host/debug build (x86 Ubuntu) |
| `smtk2` | MediaTek MT76xx SDK |
| `smtk3` | MediaTek MT79xx SDK |
| `swrt5` | OpenWrt-based builds |
| `srock` | Rockchip RK3568 |

## Documentation

Core docs under `doc/com/land/`:
- `README.md` — overview and index
- `eline.md` — interactive terminal usage
- `he.md` — HE grammar (for scripts/automation)
- `fpk.md` — FPK package layout and prj.json fields
- `init.md`, `joint.md`, `uninit.md` — lifecycle events
- `component.md`, `machine.md`, `auth.md` — core components

## Conventions

- Component names: `project@directory` (e.g. `land@machine`)
- `type: "root"` in prj.json = root-privileged/system-level package
- Boot levels run in order: `arch → land → app → general → manage → delay`
- `obj` aliases create short names (e.g. `com` → `component`, `log` → `syslog`)
- Chinese docs available as `*.cn.md` alongside English versions

## Gotchas

- `gBOARDID` is gitignored — copy from `gBOARDID.txt` template
- Platform configs (`config/smtk2/`, `config/swrt5/`, etc.) are separate git repos
- `build/` directory is gitignored and recreated by `make dep`
- Many projects in `.gitignore` (land, forward, network, etc.) — may be separate repos
