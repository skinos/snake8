# landos 项目概述（文档入口点）

`landos` 是一个基于组件的网关/设备管理基础。它将设备能力分解为组件（例如 `land@machine`、`land@syslog`、`land@auth`），并通过统一的**配置**（JSON）和**方法接口**（API）来管理它们。在系统启动期间，项目中注册的任务会被执行以完成初始化/协调/关闭。在日常使用中，用户主要通过 `he` 工具与组件交互：查询/修改配置并调用组件方法来执行实际业务操作。

本文档提供：
1. 项目功能和整体工作流程的高层概述
2. 如何使用项目（从零开始的新用户）
3. 现有 Markdown 文档的索引（一行简介 + 可点击链接）

---

## 1. 本项目的功能

从 `prj.json` 的结构来看，项目组织为：
- 一组库/可执行文件/驱动程序/组件
- 一组系统级注册和启动任务（`init` / `uninit` / `joint`）
- 通过 `he` 访问的管理接口（`land@*` 组件 + `he` 命令）

系统暴露的主要抽象是**组件**：
- 组件名称（例如 `land@machine`）作为统一的管理对象
- 用 JSON 描述的组件配置，可以查询和更新
- 通过命令调用的组件方法，返回 `talk_t` 结果（字符串/JSON，或返回码如 `ttrue/tfalse/terror/tpanic`）

---

## 2. 核心概念（快速心智模型）

1. **组件配置**
   - 每个组件都有一个 JSON 配置模型
   - 你可以通过 `he` 查询或修改这些配置字段

2. **缓存/寄存器变量**
   - 一些组件（如 `init/uninit/joint` 和寄存器相关功能）将配置物化为缓存文件/寄存器变量
   - 这在"系统读取的内容"和"配置中写入的内容"之间创建了时序差异（特别是跨系统启动时）

3. **启动生命周期**
   - `land@init`：注册在指定启动级别触发的启动任务
   - `land@joint`：在 joint 事件发生时执行任务
   - `land@uninit`：在关闭/退出阶段执行任务

4. **`he` 命令**
   - 查询配置、修改配置、调用组件方法并解析方法 JSON 结果
   - 支持多种格式（完整配置、单个字段、合并更新、方法调用和返回选定的 JSON 字段）
   - 可在本地（终端）和通过支持的通信协议工作

---

## 3. 如何使用项目（从 0 到 1）

### 3.1 安装/部署（FPK）

当你想将项目（或组件功能）部署到设备上时，通常按照 `fpk.cn.md` 将其打包为 `.fpk`，然后安装它，使系统获得相应的组件和功能。

### 3.2 启动后自动执行（init / joint / uninit）

在设备启动时，系统读取项目注册信息，并根据启动级别和 joint 事件执行初始化/协调/关闭任务。

你可以将其视为组件就绪的调度器：
- `init`：在指定阶段将任务附加到系统调度器
- `joint`：事件触发的执行（例如存储/热插拔/网络变化）
- `uninit`：退出时的清理/停止任务

### 3.3 用 `he` 管理网关（最常见的日常入口）

从 `he.cn.md` 中的 `快速入门（30 秒）` 部分开始，例如：
- 查询完整组件配置
- 查询和设置单个字段
- 调用方法然后从返回的 JSON 中提取子字段

你可以在 `he.cn.md` 中查找的具体示例：
- `land@machine`（查询）
- `land@machine:name=...`（修改）
- `land@machine.status`（调用方法）

---

## 4. 文档索引（现有 Markdown）

这些文档是学习和日常使用的主要入口点（推荐阅读顺序）：

1. [`ARCHITECTURE.cn.md`](./ARCHITECTURE.cn.md)：系统概述（组件、配置与运行时缓存、启动生命周期）。
2. [`TERMINOLOGY.cn.md`](./TERMINOLOGY.cn.md)：共享术语表（talk_t、属性路径、启动级别、返回码）。
3. [`he.cn.md`](./he.cn.md)：`he` 命令格式、快速入门和配置/方法使用示例。
4. [`fpk.cn.md`](./fpk.cn.md)：FPK 打包和安装概念，以及 `prj.json` 结构。
5. [`component.cn.md`](./component.cn.md)：组件注册/管理接口和用法。
6. [`machine.cn.md`](./machine.cn.md)：网关基本信息和 `land@machine` 配置/方法接口。
7. [`auth.cn.md`](./auth.cn.md)：认证和权限配置/方法调用示例。
8. [`syslog.cn.md`](./syslog.cn.md)：系统日志配置、日志文件位置策略和方法描述（查询/清除/调用）。
9. [`service.cn.md`](./service.cn.md)：服务管理组件接口（start/stop/exit/status）。
10. [`register.cn.md`](./register.cn.md)：寄存器变量读/写和相关方法描述。
11. [`init.cn.md`](./init.cn.md)：启动任务管理（注册/注销/列表）和 `land@init` API。
12. [`joint.cn.md`](./joint.cn.md)：Joint 事件触发的任务管理（`land@joint`）。
13. [`uninit.cn.md`](./uninit.cn.md)：关闭/退出阶段任务管理（`land@uninit`）。

---

## 5. 建议阅读顺序（最快路径）

1. 首先阅读 [`ARCHITECTURE.cn.md`](./ARCHITECTURE.cn.md) 获取全局心智模型
2. 接下来阅读 [`TERMINOLOGY.cn.md`](./TERMINOLOGY.cn.md) 对齐文档中使用的共享术语
3. 阅读 [`he.cn.md`](./he.cn.md) 掌握命令格式和返回类型
4. 阅读与你目标业务相关的组件文档（例如 `machine/auth/syslog/service/register`）
5. 最后阅读 `init/joint/uninit` 以准确了解系统启动阶段发生的事情
