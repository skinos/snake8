# `prj.json` — Skinos 项目清单

英文版本：[`prj.json.md`](./prj.json.md)。

本文档描述了**项目信息文件 `prj.json`**：字段、语义、打包布局和运行时行为。有关 **FPK 产品模型**和 **`land@fpk` API**，请参阅 [`fpk.cn.md`](./fpk.cn.md)。

---

## 1. 在系统中的角色

- 每个 **Skinos 项目目录**（通常是 SDK 中的 `project/<name>/`）必须包含一个 **`prj.json`**（`skin/project.h` 中的 `PROJECT_INFOFILE`）。
- 当项目构建为 **`.fpk`** 时，`prj.json` 会包含在包中。安装后，系统使用它来了解**项目名称、版本、库、可执行文件、组件、资源**，以及为**启动、关闭和 joint 事件**运行哪些**组件 API**。
- 将 `prj.json` 视为**机器可读的清单**：面向人类的元数据加上用于**构建/打包/注册/调度**的数据。

---

## 2. 如何快速阅读此文件

1. **顶级字符串**（`name`、`intro`、`desc`、`type`、`version`、`author`）标识项目和权限类别。
2. **`lib` / `exe` / `cmd` / `com` / `osc` / `ko` / `res`**：键通常是项目下的**源代码子目录**；值是**简短描述**。每个键**声明**该工件类别存在。
3. **`obj`**：将**公共对象名称**映射到 **`com` 目录名称**（别名、多实例）。
4. **`init` / `uninit` / `joint`**：**嵌套**映射 — 启动级别/关闭级别/事件名称 → 内部键是 **`project@component.method`**（HE 风格），值是描述（通常是 `""`）。
5. **`wui`**：可选的 Web UI 菜单 + 页面 + 语言注册。

以下的严格 **JSON** 示例**不包含注释**；叙述解释字段。

---

## 3. 顶级标识字段

| 字段 | 类型 | 含义 |
|------|------|---------|
| `name` | string | 项目（和 FPK）名称；与目录名称和已安装 id 一致。 |
| `intro` | string | 一行摘要。 |
| `desc` | string | 较长的描述。 |
| `version` | string | 版本（例如 `8.0.0`）；可能与 `PROJECT_DEFAULT_VERSION` 一致。 |
| `author` | string | 作者或维护者。 |
| `type` | string | 预期的权限层级。已记录的值包括 **`root`**、**`admin`**、**`user`**、**`app`**。**`root`** 在发货树中很常见；其他层级取决于工具链/策略 — 将 `type` 视为安装程序和 UI 的**声明意图**。 |

---

## 4. 工件部分（键 ≈ 子目录名）

每个都是一个对象：`"name": "简要描述"`。**键**是项目下该工件源代码所在的**目录名称**（打包工具使用 `prj.json` 来决定编译和复制什么）。

| 字段 | 含义 |
|------|---------|
| `lib` | 共享**库**；构建输出成为 FPK **lib** 区域下的 `.so`。为开发安装列出的头文件放到 **`install/include/<lib>/`**。 |
| `exe` | 仅在**此项目内部**使用的**可执行文件**（不一定在全局 `PATH` 上）。 |
| `cmd` | 用于从 Linux shell 运行的**命令** — 打包后出现在 FPK **`bin/`** 布局下（例如 `he`、`daemon`）。 |
| `com` | **组件**（例如 `.com`）；键 = 组件源目录；运行时对象通常是 **`name@key`**，其中 `name` 是 `prj.json` 的 `name`。 |
| `osc` | 捆绑的**第三方/开源**程序（每个键一个目录）。 |
| `ko` | 从指定目录构建的**内核模块**（`.ko`）。 |
| `res` | 作为一个单元复制到 FPK 中的**资源文件或目录树**（路径/名称由打包器声明）。 |

**提示：** 在 SDK 中，将 `lib` / `com` / `cmd` / `exe` 键与项目根目录下的**文件夹名称**匹配。

---

## 5. `obj`：组件实例和别名

形状：`"object-name": "component-directory-name"`。

- **值**始终引用 **`com`** 键（哪个目录实现逻辑）。
- **键**可以是：
  - **短服务名称**（经典的 `tui` 风格示例）：例如 `"telnetd": "telnet"` 意味着通过该别名调用 **`tui@telnet`**，或
  - **完整对象 id**，如 `land@joint` 映射到组件目录 `init`。

用于**多个逻辑对象共享一个组件**（例如 `ifname@lan` / `ifname@lan2`）或与目录名称不同的**稳定公共名称**。

---

## 6. `init` — 启动级别和 `.setup`

外部键 = **init 级别**（例如 `land`、`arch`、`app`）。内部对象：键是 **`project@component.method`**（通常是 `.setup`），值 = API 描述（通常为空）。

语义与 **`land@init`** 相关联；参见 [`init.cn.md`](./init.cn.md)。

---

## 7. `uninit` — 关闭级别和 `.shut`

与 `init` 相同的嵌套，但用于关闭阶段和 **`project@component.shut`**（等）。参见 [`uninit.cn.md`](./uninit.cn.md)。

---

## 8. `joint` — 事件 → 组件方法

外部键 = **joint 事件**（例如 `network/online`、`storage/insert`）。内部映射：**`project@component.method`** → 描述。参见 [`joint.cn.md`](./joint.cn.md)。

---

## 9. `wui` — Web UI 注册（可选）

`wui` 下的每个顶级键是一个**页面 id**。每个页面的常见字段：

| 子字段 | 含义 |
|----------|---------|
| `menu` | Web UI 中的顶级菜单组（例如 `System`）。 |
| `en` / `cn` | 英文/中文菜单标签。 |
| `page` | HTML 文件名（在项目下，按打包）。 |
| `lang` | 将语言环境键（`cn`、`en`、...）映射到该页面的 **JSON 语言文件**路径的对象。 |
| `config` | 如果设置，当此**组件配置对象**存在时显示菜单条目（例如 `tui@telnet`）。 |
| `object` | 当 `config` 为 null 时，可以通过此对象字符串将可见性绑定到**组件存在性**。 |
| `attr` | 当 `config` 非 null 时，用于更细粒度可见性规则的可选**属性路径**。 |

示例：`tmptools/prj.json`（`testcom`）和 **§15**（`tui` 摘录）。

---

## 10. 与 `he` / HE 的关系

`project@component.method` 字符串与 **`he`** 使用的 **HE** 匹配（[`he.cn.md`](./he.cn.md)）：可以理解为**"调度程序为你触发此调用"**，而不是交互式输入。

---

## 11. 验证和工具

- **`project_check`**（`skin/project.h`）可以验证项目 JSON。
- 辅助二进制文件可能位于 **`cmd`** 下（例如 `tmptools` → `prj`）。

---

## 12. 编译、打包和安装 — Skinos 资源流程

**符号说明：** **`prj.json:section`** 表示"该 JSON 对象下的每个**键**"：例如 `prj.json:cmd` 为每个键构建一个命令（`he`、`daemon`、...）。**`FPK:/`** 是 **`.fpk` 归档的根目录**。

**设备上的路径**使用与 **[`fpk.cn.md`](./fpk.cn.md)** 相同的**符号**（*运行时安装路径*）：**`⟨PRJ_ROOT⟩`**（[`skin/skinhead.h`](./skin/skinhead.h) 中的 C 宏 **`PROJECT_DIR`**）、**`⟨PRJ_NAME⟩`**（已安装项目 = `prj.json` → `name`）、**`⟨LIB_DIR⟩`** / **`⟨BIN_DIR⟩`**（**`PROJECT_LIB_DIR`** / **`PROJECT_BIN_DIR`**）、**`⟨SYS_ROOT⟩`**（合并树的运行系统根目录）。每个项目的安装前缀：**`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**。

**流程：** (1) 在由 `prj.json` 键命名的子目录中**编译**源代码 → (2) 将输出和松散文件**打包**成 **FPK** → (3) **安装**到 **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** 下，并将 `install/*` 复制到 **SDK `INSTALL/`** 树中。

确切的步骤遵循你的 **SDK Makefile/打包脚本**；下面的表格是 Skinos 工具目标的**布局契约**。

### 12.1 清单文件 → FPK

| 阶段 | 源（项目树） | FPK 中的目标 |
|-------|------------------------|-------------------|
| 复制 | `./prj.json` | `FPK:/prj.json` |

### 12.2 在 `prj.json` 中声明的构建输出 → FPK

| `prj.json` 部分 | 典型构建输出（该部分下的每个键） | FPK 中的目标 |
|--------------------|---------------------------------------------------|--------------------|
| `lib` | `*.so` 共享库 | `FPK:/lib/` |
| `com` | `*.com` 组件 | `FPK:/`（FPK 根目录） |
| `cmd` | 作为 shell 命令安装的可执行文件（基本名 = 键） | `FPK:/bin/` |
| `exe` | 项目本地可执行文件 | `FPK:/` |
| `osc` | 第三方/开源程序二进制文件 | `FPK:/` |
| `ko` | `*.ko` 内核模块 | `FPK:/` |
| `res` | 资源文件或目录树（由打包器定义） | `FPK:/`（按项目规则） |

### 12.3 项目下的预构建树 → FPK（仅复制）

| 源（项目树） | FPK 中的目标 |
|-----------------------|--------------------|
| `./lib/*.so*` | `FPK:/lib/` |
| `./bin/*` | `FPK:/bin/` |
| `./etc/*` | `FPK:/etc/` |
| `./internal/*` | `FPK:/internal/` |
| `./rootfs/*` | `FPK:/rootfs/` |

### 12.4 项目根目录的松散文件 → FPK

| 源（项目根目录） | FPK 中的目标 |
|-----------------------|--------------------|
| `*.cfg`, `*.sh`, `*.ash`, `*.png`, `*.jpg`, `*.json`, `*.html` | `FPK:/` |

### 12.5 开发者负载（`install/`）→ FPK

`<lib>` = `prj.json:lib` 中的**库键**（例如 `skin`）。

| 源 | FPK 中的目标 |
|--------|-------------------|
| 为该库发布的 `*.h` 头文件 | `FPK:/install/include/<lib>/` |
| 用于在 SDK 主机上链接的开发 `.so` 副本 | `FPK:/install/lib/` |

### 12.6 文档/COM-face 资源

| 源 | 目标 |
|--------|-------------|
| `*.md`, `*.png`（当被打包器路由用于文档/UI face 时） | `gCOMFACE_DIR/<project-name>/` |

*（某些 `*.png` 也可能按 §12.4 打包在 `FPK:/`；适用哪条规则取决于打包配方。）*

---

## 13. 在系统上安装 FPK

在 **`land@fpk.install`**（或等效操作）之后，FPK 内容被展开到 **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** 下（参见 **`fpk.cn.md`** — **运行时安装路径**）。

### 13.1 设备上的清单

| FPK 中 | 设备上 |
|--------|-----------|
| `prj.json` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/prj.json` |

### 13.2 从 FPK 构建/暂存的负载 → 每个项目前缀

| FPK 中 | 设备上（典型） | 备注 |
|--------|---------------------|-------|
| `lib/*` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/lib/` | 通常符号链接到 **`⟨LIB_DIR⟩`** |
| `*.com` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | 与其他根级负载一起 |
| `bin/*` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/bin/` | 通常符号链接到 **`⟨BIN_DIR⟩`** |
| 作为 **`exe`** 或 **`osc`** 条目打包的二进制文件 | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | 与 `.com` 相同的前缀 |
| `*.ko` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | |
| 打包的 **`res`** 内容 | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` | |

### 13.3 FPK 内的可选树 → 系统根目录

| FPK 中 | 设备上 | 状态 |
|--------|-----------|--------|
| `etc/*` | `⟨SYS_ROOT⟩/etc/` | 典型安装程序中**[未实现]** |
| `internal/*` | `⟨SYS_ROOT⟩/mnt/internal/` | 典型安装程序中**[未实现]**（路径可能因产品而异） |
| `rootfs/*` | `⟨SYS_ROOT⟩/` | 合并到**系统根目录** |

### 13.4 FPK 根目录的松散文件 → 每个项目前缀

| FPK 中（根目录） | 设备上 |
|-----------------|-----------|
| `*.cfg`, `*.sh`, `*.ash`, `*.png`, `*.jpg`, `*.json`, `*.html` | `⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/` |

### 13.5 开发者树：`install/` → SDK（不保留在 **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** 下）

| FPK 中 | 复制到（SDK/主机） | 备注 |
|--------|-------------------------|------|
| `install/include/<lib>/*` | `INSTALL/include/<lib>/` | 用于针对项目库进行编译 |
| `install/lib/*` | `INSTALL/lib/` | 开发链接库 |

这些路径通常在**安装时使用**，**不会**作为运行时子树保留在 **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** 下。

---

## 14. 示例摘录 — `land` 项目

来自此仓库的 `land/prj.json`：

```json
{
    "name": "land",
    "intro": "component infrastructure",
    "desc": "core for skin system, provides the basic components&library of the entire system",
    "type": "root",
    "version": "8.0.0",
    "author": "dimmalex@gmail.com",
    "lib": {
        "skin": "skinos core library"
    },
    "com": {
        "fpk": "fpk management",
        "init": "init/uninit/joint management"
    },
    "cmd": {
        "he": "tools for call all component"
    },
    "obj": {
        "land@uninit": "init",
        "land@joint": "init"
    },
    "init": {
        "land": {
            "land@auth.setup": "",
            "land@joint.setup": "",
            "land@init.setup": "",
            "land@uninit.setup": ""
        }
    },
    "joint": {
        "storage/insert": {
            "land@syslog.setup": ""
        }
    }
}
```

---

## 15. 示例摘录 — `tui` 风格清单

演示 **`com` + `cmd` + 短 `obj` 别名 + `init` + `wui`**（为了大小而裁剪为一个 `wui` 页面）：

```json
{
    "name": "tui",
    "intro": "Terminal user interface service",
    "type": "root",
    "version": "7.0.0",
    "author": "dimmalex@gmail.com",
    "com": {
        "telnet": "telnet server management",
        "ssh": "ssl shell server management"
    },
    "cmd": {
        "eline": "tools for terminal command line"
    },
    "obj": {
        "telnetd": "telnet",
        "sshd": "ssh"
    },
    "init": {
        "app": {
            "tui@telnet.setup": "",
            "tui@ssh.setup": ""
        }
    },
    "wui": {
        "telnet": {
            "menu": "System",
            "en": "Telnet Server",
            "cn": "Telnet服务器",
            "page": "telnet.html",
            "config": "tui@telnet",
            "lang": {
                "cn": "cn.json",
                "en": "en.json"
            }
        }
    }
}
```

---

## 16. 相关文档

| 文档 | 内容 |
|----------|---------|
| [`fpk.cn.md`](./fpk.cn.md) | FPK 生命周期，`land@fpk` |
| [`init.cn.md`](./init.cn.md)、[`uninit.cn.md`](./uninit.cn.md)、[`joint.cn.md`](./joint.cn.md) | 生命周期注册 |
| [`component.cn.md`](./component.cn.md) | 组件 |
| [`he.cn.md`](./he.cn.md) | HE 语法 |
| [`README.cn.md`](./README.cn.md) | land 文档索引 |

---

**总结：** **`prj.json`** = 项目标识 + **哪些目录变成 lib/com/cmd/exe/...** + **`obj` 别名** + **何时调用 `project@component.method`** + 可选的 **`wui`**。**§12–§13** 将 JSON 连接到 **FPK** 布局和**设备上路径** **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`**（`skinhead.h` 中的宏；参见 **`fpk.cn.md`**）。
