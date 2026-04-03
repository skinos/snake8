# Snake8 / landos — 嵌入式网关 SDK

## 概述

本仓库提供面向**嵌入式 Linux 网关及行业终端**的软件开发套件（SDK）。系统在 Linux 之上引入**组件化**管理模型：各功能域以**具名组件**形式呈现，具备 **JSON 配置模型**及对外 **API**。无论通过串口、SSH、Telnet、Web 管理端或远程代理访问，均统一采用 **HE（命令语法）**；差异仅在于入口程序（如 `eline`、`he`）及会话环境。

文档中常将该体系称为 **landos**（land 层管理与调度模型）；公共基础库一般称为 **skinos**。

---

## 体系结构要点

| 层次 | 说明 |
|--------|------|
| **Linux** | 内核、驱动及用户态守护进程，与标准嵌入式 Linux 一致 |
| **组件** | 以 `land@machine`、`forward@nat`、`client@dhcps` 等形式寻址；各组件包含 JSON 配置及可调用的 API |
| **`eline`** | 设备侧**默认交互环境**（提示符通常为 **`$ `**）：基于 readline，支持行编辑与历史；在此环境下应**直接输入 HE 命令**，**不得**再加 `he` 前缀。说明见 [`doc/com/land/eline.cn.md`](doc/com/land/eline.cn.md) |
| **`he`** | 在 **`ash` 或常规 shell** 中执行**单行 HE** 的程序；应使用 **`he '完整命令'`**（单引号包裹），以免 shell 误解析 `=`、管道符 `|`、JSON 等字符。语法规范见 [`doc/com/land/he.cn.md`](doc/com/land/he.cn.md) |
| **工程与 FPK** | `project/` 下含 `prj.json` 的目录可构建为 **FPK** 安装包，部署至设备后扩展系统能力 |
| **生命周期** | 系统按启动阶段执行 `init` 中登记的组件 API；网络链路变化等事件触发 **joint**（联动）任务；关机或退出阶段执行 `uninit`。具体映射关系由各工程 `prj.json` 定义 |

### 核心文档入口（[`doc/com/land/`](doc/com/land/)）

- [`README.cn.md`](doc/com/land/README.cn.md) — 总体说明与索引  
- [`eline.cn.md`](doc/com/land/eline.cn.md) — 基于串口 / Telnet / SSH 的**常规运维与会话约定**  
- [`he.cn.md`](doc/com/land/he.cn.md) — **Shell 环境及脚本**中使用 `he` 的语法与注意事项  
- [`fpk.cn.md`](doc/com/land/fpk.cn.md) — FPK 安装布局及 `prj.json` 字段说明  
- [`init.cn.md`](doc/com/land/init.cn.md)、[`joint.cn.md`](doc/com/land/joint.cn.md)、[`uninit.cn.md`](doc/com/land/uninit.cn.md) — 启动阶段、联动事件与收尾阶段的任务注册  
- [`component.cn.md`](doc/com/land/component.cn.md)、[`machine.cn.md`](doc/com/land/machine.cn.md)、[`auth.cn.md`](doc/com/land/auth.cn.md)、[`syslog.cn.md`](doc/com/land/syslog.cn.md)、[`service.cn.md`](doc/com/land/service.cn.md)、[`register.cn.md`](doc/com/land/register.cn.md)、[`daemon.cn.md`](doc/com/land/daemon.cn.md)、[`skin.cn.md`](doc/com/land/skin.cn.md)、[`prj.json.cn.md`](doc/com/land/prj.json.cn.md) — 核心 `land@*` 组件及相关工具  

路由、二层/三层、无线等主题的说明位于 [`doc/com/`](doc/com/) 下对应子目录。

---

## 仓库顶层目录

| 路径 | 用途 |
|------|------|
| **`Makefile`** | 顶层构建入口：`dep`、`kernel` / `app`，输出至 `build/`；头文件同步至 `doc/dev/include` |
| **`gBOARDID`** | 指定**平台、SoC、板型、scope、OEM**（书写规则见文件内注释），并驱动 `config/<platform>/…` 的解析。该文件被 `.gitignore` 排除；**`gBOARDID.txt`** 为纳入版本管理的参考模板 |
| **`config/`** | 按平台/方案划分的配置树（如 **`smtk2`**、**`smtk3`**、**`srock`**、**`sopen`**、**`slave`** 等），包含工具链、内核、rootfs 叠加、`sdk.makefile`、`fpk.makefile` 等 |
| **`project/`** | **实现代码与可交付工程**：C 语言组件、库、守护进程、Web 资源及 **`prj.json`**（声明组件集合、`init` / `joint` / `uninit` 挂钩及 Web 菜单元数据）；目录命名通常与 `doc/com` 划分一致 |
| **`doc/`** | 用户文档及构建生成的头文件说明 |
| **`tools/`** | 宿主机侧构建辅助工具（含指向 **`project/fpktools`** 的符号链接：`firmware-encode`、`fpk-install`、`prj` 等 FPK 打包工具） |
| **`var/`** | 本地 IDE（如 Si4）工程文件，与固件产物无直接关系 |
| **`rice/`** | 可选的客户定制叠加目录（对应 `Makefile` 中 `gRICE_DIR`） |

典型**构建输出**包括：

- **`build/install/`** — 编译过程中的头文件、库等中间产物  
- **`build/rootfs/`** — 组装的 rootfs；已安装工程通常位于 **`usr/share/skinos/`** 等路径（参见 `Makefile` 中 `gosPRJ_DIR`）  

符号化安装路径（如 **`⟨PRJ_ROOT⟩`**）的定义见 [`doc/com/land/fpk.cn.md`](doc/com/land/fpk.cn.md)，并与 `PROJECT_DIR` 等 C 语言宏对应。

---

## `project/` 目录组织

**`project/`** 下每个子目录代表一个**功能域**，同时包含**实现源码**（C 组件、库、守护进程）和**项目描述文件**（**`prj.json`**）。

示例：`project/land`、`project/forward`、`project/network`、`project/webs`、`project/wifi`、`project/modem` 等。

每个工程须至少包含：

1. **`prj.json`** — **项目描述文件**（字段说明见下文）  
2. **Makefile** — 与目标平台构建体系一致（本 SDK 中常见形态接近 OpenWrt）

FPK 打包规则及安装路径约定见 [`doc/com/land/fpk.cn.md`](doc/com/land/fpk.cn.md)。

---

## `prj.json` 字段说明

`prj.json` 同时服务于构建说明、运行时注册及文档对照。

| 字段 | 含义 |
|-----|------|
| **`name`**、**`intro`**、**`desc`**、**`version`**、**`author`**、**`type`** | 工程标识与描述信息；`type: "root"` 通常表示系统级、需特权安装的包 |
| **`com`** | 子目录一般对应一个组件，对外名称为 `工程名@子目录` |
| **`lib`** | 本工程构建的库 |
| **`exe`** | 独立可执行文件 |
| **`osc`** | 随工程打包的第三方或开源源码树 |
| **`cmd`** | 随工程安装至系统的命令（例如 **land** 工程中的 `he`、`eline`、`daemon`） |
| **`obj`** | **别名映射**：对外组件名可指向另一实际实现 |
| **`init`** | **启动阶段**与待调用 **`工程@组件.接口`** 的对应关系。已知阶段包括 **`arch`**、**`land`**、**`app`**、**`general`**、**`manage`**、**`delay`**（详见 [`init.cn.md`](doc/com/land/init.cn.md)）。每个阶段的值可为**字符串**（单个 API，如 `"wui@admin.setup"`）或**对象**（多个 API，如 `{"land@auth.setup":"", "land@init.setup":""}`) |
| **`joint`** | **事件**（如 `network/on`、`network/online`）与待调用接口的对应关系 |
| **`uninit`** | 关机或退出阶段待执行的接口 |
| **`wui`** | Web 管理端注册信息：菜单分组、多语言标题、HTML 页面，以及可选的 `config` / `object` 绑定 |

启动阶段与联动事件的命名及语义以 [`init.cn.md`](doc/com/land/init.cn.md)、[`joint.cn.md`](doc/com/land/joint.cn.md)、[`uninit.cn.md`](doc/com/land/uninit.cn.md) 为准。

---

## `project/` 工程与运行时关系

各工程并非相互独立的应用程序，而是向**同一运行时**注册组件。组件名通常为 **`工程@目录`**（由 **`com`** 导出）；若配置 **`obj`**，则对外名称可能映射至其他实现（例如 `land@joint` 由 `init` 相关实现承载）。`init`、`joint`、`uninit` 中可**调用其他工程所提供组件的 API**（例如在外部网络事件下调用 `forward@main.on`），从而形成协同行为链。

### 依赖关系示意（ASCII，不依赖 Mermaid）

图示自上而下可理解为自**人机入口**至**基础设施**的依赖方向；框外英文注释表示横向协作关系。

```text
       +---------------------------+       +---------------------------+
       | wui (admin -> webs@httpd) |       | tui (telnet / ssh)        |
       +-------------+-------------+       +-------------+-------------+
                     \                               /
                      \   operators use the same HE  /
                       v   grammar via land tools   v
                 +-----------------------------------------+
                 | land: he, eline, daemon                 |
                 +--------------------+--------------------+
                                      |
                                      v
  +---------------------------------------------------------------------+
  | land:      skin, machine, auth, syslog, service, register, fpk,    |
  |            init / joint / uninit, component registry               |
  +---------------------------------------------------------------------+
                                      |
            +-------------------------+-------------------------+
            v                         v                         v
  +-------------------+     +-------------------+     +-------------------+
  | network           |     | ifname            |     | wifi              |
  | hosts, frame,     |     | ethcon, ltecon    |     | ap, sta, skinwifi |
  | vlan, bridge, …   |     |                   |     |                   |
  +---------+---------+     +-------------------+     +-------------------+
            |                         ^                         ^
            |              +----------+-----------+              |
            |              | modem (atd, smsd,   |              |
            |              | ec2x, rm500u, …)   |              |
            |              +----------+-----------+              |
            |                         |                         |
            +-------------------------+-------------------------+
                                      |
                      +---------------+---------------+
                      v                               v
              +---------------+               +---------------+
              | forward       |               | client        |
              | main, nat, fw |               | dhcps, station|
              | dnat, ttl, alg|               | acl           |
              +---------------+               +---------------+
                      \                               /
                       `---- joint on network/* -----'
                         (on/off, online/offline
                          refreshes rules and DHCP)

  clock ..........> uses network/online (and friends) for NTP / time
  storage ........> land joint on storage insert/remove (e.g. syslog paths)
  agent ..........> follows network + machine status; drives HE remotely
```

**图示说明**

- **纵向依赖：** 假定 **land** 及网络相关工程已就绪，**forward**、**client** 等策略层方可稳定工作。  
- **forward 与 client：** 大量逻辑通过 **`network/*`** 类 **joint** 事件与链路状态联动，既依赖当前接口状态，亦在状态变化时刷新规则，并非简单的单向调用关系。  
- **wifi 与 modem：** 常与 **ifname**、**network** 协同部署；具体固件未必包含全部模块。  
- **图底部英文注释：** **clock** 依赖 WAN 在线等事件进行 NTP 校时；**storage** 在存储插拔时触发 **land** 侧 **joint**（如调整 syslog 路径）；**agent** 根据网络与设备状态在远端执行 HE 控制。

### 各工程职责概要

- **`land`** — **管理与运行时基础**：`skin` 库；`land@init`、`land@joint`、`land@uninit` 负责任务调度；`land@fpk` 负责安装包；`land@machine`、`land@auth`、`land@syslog`、`land@service` 与 **`daemon`**、`land@register`、`land@component` 分别负责设备信息、认证、日志、服务、寄存数据与组件登记；**`he`**、**`eline`** 为命令行入口。其余工程默认上述组件名可用。  
- **`network`** — **二层/三层公共能力**：`network@hosts`、`frame`、`vlan`、`bridge`、`keeplive` 等；**`osc`** 中可包含 PPP 相关源码；**`connect`** 等可执行程序常与 **ifname** 所管理的广域接口配合。`network@hosts.setup` 在 **`land`** 阶段执行，以尽早提供主机名解析能力。  
- **`ifname`** — **按接口维度的连接与寻址逻辑**（`ifname@ethcon`、`ifname@ltecon`）。文档中的 WAN、LTE、WISP 等场景多映射至此类组件。  
- **`wifi`** — **射频及 AP/STA 策略**（`wifi@ap`、`wifi@sta`），配合 **`skinwifi`**。具体驱动与内核配置见 **`config/`**，并与 **network**、**ifname** 共同决定桥接或路由拓扑。  
- **`modem`** — **蜂窝通信**：`modem@atd`、`modem@smsd`，模组驱动 **`ec2x`**、**`rm500u`**，库 **`skinmodem`**，调试工具 **`tip`**。LTE 作为上行时通常与 **`ifname@ltecon`** 等配置联合使用。  
- **`forward`** — **路由、NAT、防火墙、端口映射等**：`forward@main`、`nat`、`firewall`、`dnat`、`ttl`、`alg`。在 `network/on`、`network/onextern`、`network/onvpn` 等 **joint** 事件下刷新规则，逻辑上位于**接口管理稳定之后**。  
- **`client`** — **局域网侧**：DHCP（`client@dhcps`）、终端信息（`client@station`）、访问控制（`client@acl`）。与 **`network/on|off|online|offline`** 等事件紧密关联。  
- **`clock`** — 时间、NTP、定时重启（`clock@ntps`、`date`、`restart`）；常结合 **`network/online`** 等事件在广域网可用后校时。  
- **`storage`** — 如 **`storage@ftp`**；由 `init` 拉起服务；存储插拔时 **land** 的 **joint** 可调整 syslog 等路径（参见 **land** 工程 `prj.json`）。  
- **`tui`** — Telnet、SSH（`tui@telnet`、`tui@ssh`，Dropbear），用于远程接入 **`eline`** 或 **`he`**。  
- **`wui`** — Web 管理端：**`wui@admin`** 通过 **`obj`** 绑定至 **`webs@httpd`**（实现位于 **`project/webs`**）。**`app`** 阶段 **`init` / `uninit`** 负责管理站点的启停。  
- **`agent`** — **远程与云端管控**：`agent@io`、`local`、`heclient`、`portc` 及可执行文件 **`gtog`**。**`init` / `joint`** 将 **`heclient`** 与 **`network/online`**、`machine/status` 等状态关联。  
- **`webs`** — **HTTP 服务**：`webs@httpd`（Web 管理端的后端引擎）。`wui@admin` 通过 **`obj`** 绑定至此组件。  
- **`uart`** — 串口相关应用：`uart@frame`、`dtu`、`hetui`，库 **`skinuart`**；Web 配置页常对应 **`uart@tty`**、`tty2`、`tty3`。  
- **`tmptools`** — **示例与模板工程**：`tmptools@testcom`、`testexe`、**`prj`** 命令，用于学习组件与工程组织方式。

---

## 工程与组件文档索引

下表将 **`prj.json` 中的组件名** 映射至**文档路径**（相对于仓库根目录）。若无独立 Markdown，则指向最接近的说明或 **`project/<领域>/`** 源码。

| 工程 | 组件文档 | 备注 |
|---------|-----------------------------------|----------------|
| **land** | [`land@fpk`](doc/com/land/fpk.cn.md) · [`land@init`](doc/com/land/init.cn.md) · [`land@joint`](doc/com/land/joint.cn.md) · [`land@uninit`](doc/com/land/uninit.cn.md) · [`land@component`](doc/com/land/component.cn.md) · [`land@register`](doc/com/land/register.cn.md) · [`land@syslog`](doc/com/land/syslog.cn.md) · [`land@service`](doc/com/land/service.cn.md) · [`land@machine`](doc/com/land/machine.cn.md) · [`land@auth`](doc/com/land/auth.cn.md) | **`cmd`**: [`he`](doc/com/land/he.cn.md)、[`eline`](doc/com/land/eline.cn.md)、[`daemon`](doc/com/land/daemon.cn.md) · **`lib`**: [`skin`](doc/com/land/skin.cn.md) |
| **forward** | [`forward@alg`](doc/com/forward/alg.cn.md) · [`forward@ttl`](doc/com/forward/ttl.cn.md) · [`forward@firewall`](doc/com/forward/firewall.cn.md) · [`forward@nat`](doc/com/forward/nat.cn.md) · [`forward@dnat`](doc/com/forward/dnat.cn.md) · [`forward@main`](doc/com/forward/main.cn.md) · 另见 [`rule.cn.md`](doc/com/forward/rule.cn.md)、[`mark.cn.md`](doc/com/forward/mark.cn.md) | **`obj`** 示例：`forward@254` → `main` |
| **network** | [`network@frame`](doc/com/network/frame.cn.md)；`hosts` / `vlan` / `bridge` / `keeplive` *暂无独立文档，参见 [`frame.cn.md`](doc/com/network/frame.cn.md) 及 `project/network/`* | **`lib`**: `skinnet`；**`exe`**: `connect`；**`osc`**: `ppp-2.4.5` |
| **client** | [`client@acl`](doc/com/client/acl.cn.md) · [`client@dhcps`](doc/com/client/dhcps.cn.md) · [`client@station`](doc/com/client/station.cn.md) | 大量 **joint** 与 LAN/WAN 事件绑定 |
| **modem** | [`modem@atd`](doc/com/modem/lte.cn.md)（蜂窝 / AT，与 **`atd`** 同属一类能力）· [`modem@smsd`](doc/com/modem/sms.cn.md)；**`ec2x` / `rm500u`** *参见 [`lte.cn.md`](doc/com/modem/lte.cn.md) 与 `project/modem/`* | **`lib`**: `skinmodem`；**`cmd`**: `tip`；**`obj`**: `usbdrv@ec2x` / `rm500u` |
| **wifi** | AP：[`wifi@n`（2.4G）](doc/com/wifi/n.cn.md)、[`wifi@a`（5.8G）](doc/com/wifi/a.cn.md)；STA：[`wifi@nsta`](doc/com/wifi/nsta.cn.md)、[`wifi@asta`](doc/com/wifi/asta.cn.md)；多 SSID：[`assid`](doc/com/wifi/assid.cn.md)、[`nssid`](doc/com/wifi/nssid.cn.md) — *与 `prj.json` 中 **`ap` / `sta`** 目录对应* | **`lib`**: `skinwifi` |
| **ifname** | [`ifname@lan`](doc/com/ifname/lan.cn.md) · [`wan`](doc/com/ifname/wan.cn.md) · [`lte`](doc/com/ifname/lte.cn.md) · [`wisp`](doc/com/ifname/wisp.cn.md) — *与 **`ethcon` / `ltecon`** 等用法对应* | 有线 / 蜂窝 / WISP 上行相关能力 |
| **clock** | [`clock@ntps`](doc/com/clock/ntps.cn.md) · [`clock@date`](doc/com/clock/date.cn.md) · [`clock@restart`](doc/com/clock/restart.cn.md) | **`osc`**: `ntpclient` |
| **storage** | [`storage@ftp`](doc/com/storage/ftp.cn.md) | 文件服务类 |
| **tui** | [`tui@telnet`](doc/com/tui/telnet.cn.md) · [`tui@ssh`](doc/com/tui/ssh.cn.md) | **`obj`**: `telnetd`→`telnet`，`sshd`→`ssh` |
| **wui** | [`wui@admin`](doc/com/wui/admin.cn.md) · [ACE 皮肤](doc/com/wui/ace.cn.md) · [网页编写指南](doc/com/wui/webpage.cn.md)；**`webs@httpd`** *实现位于 `project/webs/`* | Web 管理框架 |
| **agent** | [`agent@io`](doc/com/agent/io.cn.md) · [`local`](doc/com/agent/local.cn.md) · [`heclient`](doc/com/agent/heclient.cn.md) · [`portc`](doc/com/agent/portc.cn.md) · [`gtog`](doc/com/agent/gtog.cn.md) · [`net`](doc/com/agent/net.cn.md) | **`exe`**: `gtog` |
| **webs** | *参见 `project/webs/` 源码* — `webs@httpd` | HTTP 后端引擎 |
| **uart** | *当前无 `doc/com/uart/`* — 参见 `project/uart/` | **`lib`**: `skinuart` |
| **tmptools** | [`prj` 工具说明](project/tmptools/prj.cn.md) · [`prj.json` 规范](doc/com/land/prj.json.cn.md)；**`testcom`** *示例见 [`prj.json.cn.md`](doc/com/land/prj.json.cn.md) 第 15 节及 `project/tmptools/`* | **`cmd`**: `prj`；**`exe`**: `testexe` |

部分目录可能**未**包含独立 `prj.json`，仍可由平台 Makefile 或其他工程作为依赖引入。

---

## 文档目录结构

| 路径 | 内容 |
|------|-----------|
| **`doc/com/<领域>/`** | 按功能划分的**组件说明**：一般包含 JSON 配置项与 API 描述，命名与 `工程@组件` 一致 |
| **`doc/com/land/`** | **命令行与会话**：[`eline.cn.md`](doc/com/land/eline.cn.md)、[`he.cn.md`](doc/com/land/he.cn.md)、[`fpk.cn.md`](doc/com/land/fpk.cn.md)，以及 [`init`](doc/com/land/init.cn.md) / [`joint`](doc/com/land/joint.cn.md) / [`uninit`](doc/com/land/uninit.cn.md)；核心 `land@*` 说明亦集中于此 |
| **`doc/dev/include/`** | 构建完成后由 `build/install/include` 同步的 **C 头文件**，供二次开发参考 |
| **`doc/product/`**、**`doc/use/`** | 产品或场景相关补充材料（可选） |

**对应关系归纳：** 实现位于 **`project/`**，接口与配置约定见 **`doc/com/<主题>/`**，打包与生命周期挂钩见 **`project/<名称>/prj.json`**。

---

## 运行时管理

系统管理统一基于 **HE 语法**（[`eline.cn.md`](doc/com/land/eline.cn.md)、[`he.cn.md`](doc/com/land/he.cn.md)）。操作可分为三类：

| 操作类型 | 说明 |
|-----------|------|
| **查询配置** | 读取组件持久化 JSON：完整对象为 `组件名`，子路径为 `组件名:属性/路径`（层级以 `/` 分隔） |
| **修改配置** | 单字段赋值、整体替换 `组件={...}`、或采用管道形式合并局部字段（详见 [`he.cn.md`](doc/com/land/he.cn.md)） |
| **调用 API** | 形式为 `组件.接口` 或 `组件.接口[参数]`；返回值多为 JSON 或 **`ttrue` / `tfalse`** 等哨兵。若仅需 JSON 子字段，可在末尾追加 **`:属性路径`** |

**会话约定：** 提示符 **`$ `** 表示 **eline** 环境，应**原样输入** HE 行；提示符 **`~ #`** 表示已通过 **`ashy` 进入 shell**，须将同一 HE 行写作 **`he '…'`**。

### 1. 查询配置

- **`组件名`**：返回完整配置对象。  
- **`组件名:属性/路径`**：返回该路径下取值（字符串或 JSON 片段）。

**eline（`$ `）示例：**

```text
$ land@machine
$ land@machine:name
$ ifname@lan:static
```

**Shell（`~ #`）示例：** 上述命令分别写为 `he 'land@machine'` 等形式。

| 命令 | 说明 | 典型输出 |
|---------|----------------|----------------|
| **`land@machine`** | 读取 **`land@machine`** 全部持久化配置（设备标识、语言、MAC 相关等） | **JSON 对象**；随后返回 **`$ `** 提示符。若组件或路径无效，可能返回错误信息或哨兵值 |
| **`land@machine:name`** | 读取 **`name` 字段** | **纯文本行**；未配置时可能为空；通常不含 JSON 引号 |
| **`ifname@lan:static`** | 读取 LAN 静态地址相关配置（视固件是否提供该模型） | **JSON 片段**（常见字段如 `ip`、`mask`）；未配置时输出极少 |

在 **`$ `** 下输入 **`@`** 可列出当前固件中的组件；非 eline 环境的等价操作见 [`he.cn.md`](doc/com/land/he.cn.md)。JSON 字段语义以 **`doc/com/`** 各组件文档为准，不同产品可能存在差异。

### 2. 修改配置

写入前经组件校验。常见形式包括：单字段赋值、**`路径=`** 清空、**`组件={...}`** 整体替换、**`组件|{...}`** 等合并语法（完整规则见 [`he.cn.md`](doc/com/land/he.cn.md)）。

**eline（`$ `）示例：**

```text
$ land@machine:name=MyGateway
$ land@machine|{"language":"en"}
$ gnss@nmea:client=
```

**Shell（`~ #`）示例：**

```sh
he 'land@machine:name=MyGateway'
he 'land@machine|{"language":"en"}'
he 'gnss@nmea:client='
```

当命令含 **`|`**、**`=`** 或复杂 JSON 时，在 shell 中**须**使用**单引号**将完整参数传给 `he`。

| 命令 | 说明 | 典型输出 |
|---------|----------------|----------------|
| **`land@machine:name=MyGateway`** | 将 **`name`** 设为 **`MyGateway`**（由 **`land@machine`** 校验） | 常见 **`ttrue` / `tfalse`** 或简短状态信息；失败时返回错误提示。脚本中 **`he`** 的退出码与结果相关（见 [`he.cn.md`](doc/com/land/he.cn.md)） |
| **`land@machine` + 管道合并**（示例 `{"language":"en"}`，语法见 [`he.cn.md`](doc/com/land/he.cn.md)） | 仅更新所列键，其余保持不变 | 与单字段写入类似；非法 JSON 或禁止的键将导致失败 |
| **`gnss@nmea:client=`** | 清除 **`client`** 子配置（示例：删除客户端配置块） | 成功/失败类提示；成功时通常无大段 JSON |

*注：**`gnss@nmea`** 仅当目标固件包含该组件时适用。*

### 3. 调用组件 API

与**查询配置**不同：使用 **`组件.方法`**、**`组件.方法[参数]`**；若仅需返回 JSON 中的字段，使用 **`组件.方法:字段/路径`**。

**eline（`$ `）示例：**

```text
$ land@machine.status
$ land@machine.status:version
$ clock@date.ntpsync[ntp1.aliyun.com]
$ client@station.list
```

**Shell（`~ #`）示例：**

```sh
he 'land@machine.status'
he 'land@machine.status:version'
he 'clock@date.ntpsync[ntp1.aliyun.com]'
he 'client@station.list'
```

| 命令 | 说明 | 典型输出 |
|---------|----------------|----------------|
| **`land@machine.status`** | 调用 **`land@machine`** 的 **`status`**（运行状态、版本、能力等，字段由实现定义） | **JSON**；或无 JSON 体时的 **`ttrue`/`tfalse`/`terror`/`tpanic`** 等（见 [`he.cn.md`](doc/com/land/he.cn.md)） |
| **`land@machine.status:version`** | 同上，但仅输出 **`version` 字段** | 短字符串；字段不存在时为空或报错 |
| **`clock@date.ntpsync[ntp1.aliyun.com]`** | 向指定 NTP 服务器发起同步 | 接受请求时常为 **`ttrue`**，失败为 **`tfalse`** 或错误文本；部分版本可能返回额外 JSON — 详见 **`doc/com/clock/date.cn.md`** |
| **`client@station.list`** | 列出已学习的 LAN 终端（语义因产品而异，类似 ARP 表） | **JSON** 数组或对象；服务未就绪时可能无输出或报错 |

列举某组件可用 API 可使用 **`<组件名>.`**（参见 [`eline.cn.md`](doc/com/land/eline.cn.md)、[`he.cn.md`](doc/com/land/he.cn.md)）。

---

### 4. 默认环境：`eline`（`$ `）

多数固件将 **`eline`** 设为登录 shell。通过串口、Telnet 或 SSH 登录后，提示符为 **`$ `**，具备命令历史与行编辑能力。此时应：

- **直接输入 HE 命令**，**不得**添加 **`he`** 前缀。  
- 语法与 [`he.cn.md`](doc/com/land/he.cn.md) 一致；[`eline.cn.md`](doc/com/land/eline.cn.md) 另说明 **eline 扩展功能**：  
  - **`set <组件>`** — 单组件多字段交互式编辑  
  - **`ashy`** — 进入 BusyBox **`ash`**，此后须按前三节使用 **`he '…'`**  
  - **`exit`** 或 Ctrl+D — 在不进入 shell 的情况下退出 eline  
  - 少量经 **eline** 转发的系统命令（实现见 `eline` 源码）

### 5. Shell 环境：`he '…'`

在 **`ashy`** 之后，或由脚本、cron、独立 **`/bin/ash`** 会话中，前台不再为 HE 解释循环，**每条** HE 命令须通过 **`he '…'`** 调用，格式同前三节。

[`he.cn.md`](doc/com/land/he.cn.md) 说明了 **`he` 的参数拼接规则**及在含空格或 shell 元字符时**必须使用单引号**的原因。

部分旧版固件仍采用经典 **HE 循环**（提示符 **`# `**），此时同样**不得**使用 `he` 前缀；请以实际提示符为准。

### 6. 组件与 API 发现

- 在 **`$ `** 下：**`@`** 列出组件；**`<组件名>.`** 列出 API — 详见 [`eline.cn.md`](doc/com/land/eline.cn.md)、[`he.cn.md`](doc/com/land/he.cn.md)。  
- 返回值可能为 **JSON**、纯文本、空，或 **`ttrue` / `tfalse` / `terror` / `tpanic`** 等（参见 [`he.cn.md`](doc/com/land/he.cn.md)、[`eline.cn.md`](doc/com/land/eline.cn.md)）。

### 7. Web 管理界面

各工程 **`wui`** 中注册的页面通过浏览器访问，其底层仍操作**同一套 JSON 配置**并调用**相同组件 API**，由前端脚本发起请求。

### 8. 远程与云端

**`agent`** 工程将 **`agent@heclient`** 等与 **WAN 在线状态**、**设备状态**等事件绑定，以保证远程控制与会话与链路状态一致。详见 [`doc/com/agent/`](doc/com/agent/) 及 `project/agent/prj.json`。

---

## 构建流程（概要）

1. 在根目录 **`gBOARDID`** 中配置与目标硬件一致的标识（平台 + SoC + 板型 + 可选 scope/OEM）。  
2. 执行 **`make preset`**（首次构建时），安装 Ubuntu 宿主机侧编译依赖。  
3. 执行 **`make update`**，拉取最新的 SDK 及平台仓库代码。  
4. 执行 **`make dep`**，初始化 `build/` 及 rootfs 暂存目录。  
5. 执行 **`make`**（或按需要分别执行 **`make kernel`** / **`make app`**，参见 **`target.makefile`** 及对应平台 **`sdk.makefile`**）。  
6. 构建完成后，公共头文件更新至 **`doc/dev/include/`**。  

其他常用目标（定义于 **`misc.makefile`**）：

| 目标 | 用途 |
|------|------|
| `make preset` | 安装 Ubuntu 构建依赖 |
| `make update` | 拉取最新 SDK + 平台仓库 |
| `make rebuild` | 重新编译、安装并启动（宿主机 / slave 模式） |
| `make menu` / `make menuconfig` | 进入 SDK 菜单配置（smtk2/smtk3 等） |
| `make install` / `make start` / `make stop` | 安装 / 启动 / 停止系统（slave / 宿主模式） |
| `make tftp` / `make ftp` / `make sz` | 通过不同传输方式部署固件至设备 |

具体工具链与镜像生成步骤取决于所选 **`config/<平台>`** 目录中的说明与 Makefile。在 **slave**（Ubuntu 宿主机）平台上的完整快速上手流程，请参阅 [`config/slave/readme.md`](config/slave/readme.md)。

---

## 仓库结构 — 多仓库布局

主仓库（`snake8`）本身**不直接包含**平台配置树的内容，而是：

- **`project/`** 下每个子目录（如 `project/land`、`project/forward`、`project/agent` 等）为**独立的 Git 仓库**，需单独克隆或拉取。
- 每个 **`config/<平台>`** 目录（如 `config/smtk2`、`config/srock`）同样是独立 Git 仓库。
- 可选的 **`rice/`** 目录（客户定制叠加）也可能是独立仓库。

顶层 **`.gitignore`** 已排除上述路径，不被主仓库跟踪。

多仓库操作辅助脚本：

| 脚本 | 用途 |
|------|------|
| **`gitst`** | 在主仓库、所有 `project/` 子仓库、`rice/` 及各 `config/<平台>` 中执行 `git status` |
| **`gitup`** | 在上述同一组仓库中执行 `git pull` |
| **`mkdel`** | 清理所有平台 SDK 构建树中的构建产物（`skinos_*` 临时目录） |

---

## 版本与杂项

- 顶层 **`Makefile`** 中 **`gPUBLISH` / `gVERSION`** 标识本 SDK 版本线。  
- 根目录 **`TPD.txt`** 为内部备忘，**不**作为用户文档。

---

## 总结

**Snake8** 将能力划分为 `project/` 下多个**协同工程**，于运行时注册至**统一组件体系**。**`land`** 提供 **`he`**、**`eline`**、**`daemon`** 及 **`land@*`** 基础服务；**`network`**、**`ifname`**、**`wifi`**、**`modem`** 负责网络连通性；**`forward`**、**`client`** 实现路由与局域网策略；**`webs`** 提供 HTTP 服务能力；**`wui`**、**`tui`**、**`agent`**、**`uart`**、**`clock`**、**`storage`**、**`tmptools`** 等工程在此基础上扩展管理与业务功能。

**运维建议：** 在设备上优先采用 [`eline.cn.md`](doc/com/land/eline.cn.md) 所述 **`$ `** 环境直接输入 HE；仅在常规 shell 或自动化脚本中使用 [`he.cn.md`](doc/com/land/he.cn.md) 规定的 **`he '…'`** 形式。
