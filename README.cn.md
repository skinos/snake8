# Snake8 / landos — 嵌入式网关 SDK

本仓库是一套**嵌入式 Linux 网关与设备管理 SDK**。在 Linux 之上提供**组件模型**：每个功能以命名对象出现（如 `land@machine`、`forward@nat`），带有 **JSON 配置**与**可调用 API**。日常运维统一使用 **HE 语法**——交互终端、Shell 脚本、Web 管理与远程 Agent 共用同一套语言。

文档中常称这套栈为 **landos**；共享 C 库为 **skinos**。

English portal: [`README.md`](README.md).

---

## 为什么用它

- **统一控制面** — 读写配置与调用 API 共用 HE（`eline`、`he '…'`、Web、Agent）。
- **按 FPK 交付** — 每个 `project/<name>/` 打成可安装的 `.fpk`（类似设备上的应用包）。
- **内建生命周期** — 开机（`init`）、事件（`joint`）、关机（`uninit`）写在 `prj.json`。
- **多板型** — 一份源码树，经 `gBOARDID` → `config/<platform>/…` 适配产品。
- **可组合域** — 路由、Wi‑Fi、蜂窝、VPN、串口等注册进同一运行时，而不是零散脚本。
- **适配 AI 自动编程与运维** — 结构化工程（`prj.json`）、HE、FPK 与 Agent 技能（`AGENTS.md`、`.claude/skills/`）便于 AI 自动编写、编译、升级、调试与持续维护。

---

## 心智模型

| 层 | 作用 |
|----|------|
| **Linux** | 内核、驱动、常规用户态 |
| **组件** | `项目@名称` — JSON 配置 + 方法 |
| **HE** | 读/写配置与调用方法的统一语法 |
| **FPK** | 单个 Skinos 项目的交付单元 |
| **生命周期** | `init` / `joint` / `uninit` 执行已登记的 HE 调用 |

---

## 学习路径（简短）

1. 复制 [`gBOARDID.txt`](gBOARDID.txt) → `gBOARDID`，选定板型（主机调试可保留 `slave-x86-ubuntu2004`）。
2. `make preset`（Ubuntu 依赖）→ `make dep` → `make`（或按需 `make kernel` / `make app`）。
3. 运行或刷机；登录后默认界面为 **`eline`**（提示符 `$ `）。
4. 试一下 `land@machine`、`land@machine.status`（eline 里**不要**加 `he` 前缀）。
5. 可选：改某个工程 → `./mkdel` → `make obj=<name>` → 热部署 `.fpk`。

---

## 仓库地图

| 路径 | 用途 |
|------|------|
| [`Makefile`](Makefile) | 顶层构建：`dep`、`kernel`、`app`，输出到 `build/` |
| `gBOARDID` | 当前板型（gitignore）；模板见 [`gBOARDID.txt`](gBOARDID.txt) |
| `config/<platform>/` | 工具链、内核、rootfs 叠加、产品胶水（常为独立 git 仓） |
| `project/<name>/` | 功能域：源码 + `prj.json` → FPK |
| `build/` | 产物：`install/`、`rootfs/`、`store/*.fpk`、`*.zz` 固件 |
| `doc/com/` | 组件 API 文档（按域划分） |
| `tools/` | 宿主机工具（`fpk-install`、打包等） |
| `rice/` | 可选客户定制叠加 |

**根目录打包 / 项目文档：** [`project.md`](project.md) · [`projects.md`](projects.md) · [`fpk.md`](fpk.md) · [`project2fpk.md`](project2fpk.md) · [`fpk2rootfs.md`](fpk2rootfs.md)

许多 `project/*`、`config/*` 为**独立 git 仓库**。辅助脚本：`gitst`、`gitup`、`mkdel`。

---

## 编译

**板型 ID**（`平台-芯片-板型[-scope][-oem]`），例如 `swrt5-mt7981-r607`。未设置时默认：`slave-x86-ubuntu2004`。

```bash
# 查看 / 设置板型
cat gBOARDID.txt
# 编辑 gBOARDID，或：
make pid gBOARDID=slave-x86-ubuntu2004
make pidinfo

make preset               # 安装 Ubuntu 编译依赖
make dep                  # 准备 build/ 并把 FPK 装进 rootfs 树
make                      # 全量：kernel + app + 安装 → 固件
# 或
make kernel && make app
make obj=land             # 单工程 → build/store/<name>-<ver>-<hw>.fpk
```

**增量流程（重要）：** 优先 `./mkdel` 再编译；**不要用 `make clean`**（全量重编极慢）。

| 产物 | 含义 |
|------|------|
| `build/store/*.fpk` | 单工程包（可热部署） |
| `build/*.zz` | 完整固件（常见整包升级） |
| `build/rootfs/` | 组装好的 rootfs（`usr/share/skinos/…`） |
| `build/install/` | 供继续编译的头文件/库 |
| `doc/dev/include/` | 成功编译后同步的头文件 |

板级 / `config/swrt5` 定制详见 [`.claude/skills/skinos-sdk/SKILL.md`](.claude/skills/skinos-sdk/SKILL.md)。slave 主机说明见 [`config/slave/readme.md`](config/slave/readme.md)（若存在）。

其它目标：`make update`、`make rebuild`、`make menuconfig`、`make tftp` / `make ftp`（见 `misc.makefile` / 平台文档）。

---

## 开发

`project/` 下带 **`prj.json`** 的目录即 Skinos 项目（顶层 `name` 必须与目录名一致）。

1. 阅读 [`project.md`](project.md)（组成部分）与 [`project/land/prj.json.md`](project/land/prj.json.md)（字段）。
2. 以 [`project/tmptools/`](project/tmptools/) 为模板复制，改名并编辑 `prj.json`。
3. 编译：`./mkdel` → `make obj=<name>`。
4. C API：`#include "skin/skin.h"` — [`doc/com/land/skin.md`](doc/com/land/skin.md)。
5. 编写清单：[`.claude/skills/skinos-project/SKILL.md`](.claude/skills/skinos-project/SKILL.md)、[`project/AGENTS.md`](project/AGENTS.md)。

封包：[`project2fpk.md`](project2fpk.md) → 镜像安装 [`fpk2rootfs.md`](fpk2rootfs.md)。

---

## 在设备上管理

| 方式 | 场景 |
|------|------|
| **`eline`**（`$ `） | Telnet / SSH / 串口登录后的默认界面 — **直接**敲 HE，不加 `he` |
| **`he '…'`** | `ashy` 进入 BusyBox ash 之后，或脚本里 — **务必单引号** |
| **Web 管理** | `wui` 登记的页面，背后是同一套 JSON/API |
| **Agent** | 远程 HE；见 [`doc/com/agent/`](doc/com/agent/) |

```text
$ land@machine                 # 查看配置
$ land@machine:name=gw1        # 写一个字段
$ land@machine.status          # 调 API
$ ashy                         # 进入 shell
# he 'land@machine.status'
```

发现组件/API：`@`、`组件.` — 详见 [`eline.md`](doc/com/land/eline.md)。

热部署 FPK（一般无需重启）：`make obj=<name>` 后经 Web `arch@firmware` `api=fpk` 上传。远程升级全流程：[`.claude/skills/device-upgrade/SKILL.md`](.claude/skills/device-upgrade/SKILL.md)。

---

## 统一接口（HE）

几乎所有运维可归纳为三类：

| 目的 | 写法 |
|------|------|
| 读配置 | `项目@组件` · `项目@组件:路径` |
| 写配置 | `项目@组件:路径=值` · `项目@组件\|{…}` · `项目@组件={…}` |
| 调方法 | `项目@组件.方法` · `项目@组件.方法[a,b]` · `…:字段`（取 JSON 字段） |

完整语法与返回哨兵（`ttrue` / `tfalse` / …）：[`doc/com/land/he.md`](doc/com/land/he.md)。交互增强（`set`、`ashy`）：[`doc/com/land/eline.md`](doc/com/land/eline.md)。

---

## 打包（FPK）

**`.fpk`** 是单个项目的 gzip tar。概览：[`fpk.md`](fpk.md)。如何打进包：[`project2fpk.md`](project2fpk.md)。`fpk-install` 如何装进 rootfs：[`fpk2rootfs.md`](fpk2rootfs.md)。运行期 API：[`doc/com/land/fpk.md`](doc/com/land/fpk.md)（`land@fpk`）。

---

## 本树中的工程域

**land** 是管理底座；**network** / **ifname** / **wifi** / **modem** 提供连接；其上是 **forward** / **client**；**wui** / **tui** / **agent** 暴露同一套 HE。

分层、**该在哪个工程扩展**，以及 modem/uart 技能见 [`projects.md`](projects.md)。  
- 蜂窝 USB 模组驱动 → [`.claude/skills/skinos-modem/`](.claude/skills/skinos-modem/)  
- 串口协议 / DTU / NMEA 应用 → [`.claude/skills/skinos-uart/`](.claude/skills/skinos-uart/)

---

## 文档索引

| 类别 | 入口 |
|------|------|
| **本导读** | [`README.md`](README.md) · [`README.cn.md`](README.cn.md) |
| **landos 核心** | [`doc/com/land/README.md`](doc/com/land/README.md) — eline、he、init/joint/uninit、machine、auth、skin 等 |
| **工程编写** | [`project.md`](project.md) · [`project/land/prj.json.md`](project/land/prj.json.md) |
| **域目录** | [`projects.md`](projects.md) · 模组: [skinos-modem](.claude/skills/skinos-modem/) · 串口: [skinos-uart](.claude/skills/skinos-uart/) · `doc/com/<domain>/` |
| **打包** | [`fpk.md`](fpk.md) · [`project2fpk.md`](project2fpk.md) · [`fpk2rootfs.md`](fpk2rootfs.md) |
| **产品 / 用例** | [`doc/product/`](doc/product/) · [`doc/use/`](doc/use/) |
| **Agent / 技能** | [`AGENTS.md`](AGENTS.md) · [`.claude/skills/`](.claude/skills/) |

**经验法则：** 实现在 `project/<name>/` · API 文档在 `doc/com/<topic>/` · 清单在该工程的 `prj.json`。
