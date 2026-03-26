# 项目开发工具 (prj)

## 概述

`prj` 命令行工具是一个用于创建和管理 Skinos 项目的在线编程工具。它提供了用于开发组件、Web UI 页面以及与 Skinos 组件化架构集成的项目配置的模板和脚手架。

## 安装

`prj` 工具是 `tmptools` 软件包的一部分。安装 `tmptools` 后，`prj` 命令将在系统 PATH 中可用。

## 用法

```shell
prj <命令> <项目名称> [参数...]
```

## 命令

### 1. 创建新项目

创建一个新的项目模板，包含基本的 `prj.json` 配置文件。

```shell
prj create <项目名称>
```

**参数：**
- `项目名称` - 要创建的项目名称（必需）

**交互式提示：**
- 项目简介 - 项目的简要描述

**示例：**
```shell
prj create myproject
# 输出：
# project myproject introduction: 我的设备管理自定义项目
# The project directory /app/myproject
```

**生成的文件：**
- `prj.json` - 项目配置文件，包含默认字段：
  - `name`: 项目名称
  - `version`: 默认版本（例如 "8.0.0"）
  - `author`: 设置为 "tmptools"
  - `intro`: 用户提供的简介

---

### 2. 删除项目

删除现有项目及其所有内容。

```shell
prj delete <项目名称>
```

**参数：**
- `项目名称` - 要删除的项目名称（必需）

**警告：** 此操作将永久删除项目目录，无法撤销。

**示例：**
```shell
prj delete myproject
```

---

### 3. 添加 Shell 组件

为项目创建一个基于 shell 的组件模板。

```shell
prj add_com <项目名称> <组件名称>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `组件名称` - 要创建的组件名称（必需）

**行为：**
- 如果项目不存在，将自动创建
- 从 `component.ash` 模板创建 `.ash` 文件
- 向系统注册组件

**示例：**
```shell
prj add_com myproject sensor
# 输出：
# The component file /app/myproject/sensor.ash
```

**生成的组件结构：**
模板包含以下方法：
- `setup()` - 在系统初始化期间调用
- `shut()` - 在系统关闭期间调用
- `service()` - 后台服务循环
- `list()` - 带参数的示例方法

---

### 4. 添加 Web UI 页面

为项目创建 Web UI (WUI) 页面模板。

```shell
prj add_wui <项目名称> <wui名称>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `wui名称` - WUI 页面的标识符（必需）

**交互式提示：**
- Web 菜单标题（中文）- 中文显示名称
- Web 菜单名称（英文）- 英文显示名称

**行为：**
- 如果项目不存在，将自动创建
- 从 `page.html` 模板创建 HTML 页面
- 生成语言文件（`cn.json`、`en.json`）
- 使用 WUI 配置更新 `prj.json`

**示例：**
```shell
prj add_wui myproject settings
# 输出：
# web menu title(Chinese): 设置
# web menu name(English): Settings
# The web page file /app/myproject/settings.html
```

**生成的文件：**
- `<wui名称>.html` - 包含表单控件和 JavaScript 的网页
- `<wui名称>-cn.json` - 中文字符串
- `<wui名称>-en.json` - 英文字符串

---

### 5. 注册启动任务

注册一个在系统启动期间调用的组件 API。

```shell
prj add_init <项目名称> <启动级别> <组件api>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `启动级别` - 启动级别（例如 `app`、`general`、`network`）（必需）
- `组件api` - 要调用的组件 API（例如 `myproject@sensor.setup`）（必需）

**启动级别：**
- `arch` - 硬件设置
- `land` - 平台设置
- `bus` - 总线设置
- `device` - 设备设置
- `network` - 网络设置
- `manage` - 管理框架设置
- `local` - 本地接口设置
- `extern` - 外部连接设置
- `app` - 应用程序设置
- `app2` - 二级应用设置
- `general` - 常规应用设置
- `delay` 到 `delay5` - 延迟启动

**示例：**
```shell
prj add_init myproject app myproject@sensor.setup
```

---

### 6. 注册关机任务

注册一个在系统关闭期间调用的组件 API。

```shell
prj add_uninit <项目名称> <关机级别> <组件api>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `关机级别` - 关机级别（必需）
- `组件api` - 要调用的组件 API（必需）

**关机级别：**（与启动顺序相反）
- `delay5` 到 `delay` - 延迟关机
- `general` - 常规关机
- `app2`、`app` - 应用程序关机
- `extern` - 外部连接关机
- `local` - 本地接口关机
- `manage` - 管理框架关机
- `network` - 网络关机
- `device` - 设备关机
- `bus` - 总线关机
- `land` - 平台关机
- `arch` - 硬件关机

**示例：**
```shell
prj add_uninit myproject app myproject@sensor.shut
```

---

### 7. 注册联合事件处理器

注册一个在系统事件发生时调用的组件 API。

```shell
prj add_joint <项目名称> <联合事件> <组件api>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `联合事件` - 事件名称（必需）
- `组件api` - 要调用的组件 API（必需）

**常用联合事件：**

| 事件 | 描述 |
|------|------|
| `machine/status` | 系统状态变化 |
| `date/modify` | 系统日期修改 |
| `auth/modify` | 认证配置更改 |
| `network/on` | 本地接口已连接 (IPv4) |
| `network/off` | 本地接口已断开 (IPv4) |
| `network/up` | 本地接口已连接 (IPv6) |
| `network/down` | 本地接口已断开 (IPv6) |
| `network/onextern` | 外部接口已连接 (IPv4) |
| `network/offextern` | 外部接口已断开 (IPv4) |
| `network/online` | 默认连接已建立 |
| `network/offline` | 默认连接已丢失 |
| `station/appear` | 客户端已连接 |
| `station/disappear` | 客户端已断开 |

**示例：**
```shell
prj add_joint myproject network/online myproject@sensor.online
```

---

### 8. 添加动态对象

创建一个依赖于现有组件的动态组件。

```shell
prj add_object <项目名称> <对象名称> <组件名称>
```

**参数：**
- `项目名称` - 目标项目名称（必需）
- `对象名称` - 对象标识符（必需）
- `组件名称` - 要使用的底层组件（必需）

**示例：**
```shell
prj add_object myproject mysensor sensor
```

---

### 9. 检查项目格式

验证项目的 `prj.json` 文件格式。

```shell
prj check <项目名称>
```

**参数：**
- `项目名称` - 要验证的项目（必需）

**示例：**
```shell
prj check myproject
```

---

### 10. 打包项目

将项目打包成 FPK（固件包）文件以进行分发。

```shell
prj pack <项目名称>
```

**参数：**
- `项目名称` - 要打包的项目（必需）

**输出：**
- 在临时目录中创建 `<名称>-<版本>-<硬件>.fpk`

**限制：**
- 无法打包位于系统项目目录 (`PROJECT_DIR`) 下的项目
- 只能打包位于应用程序目录 (`PROJECT_APP_DIR`) 下的项目

**示例：**
```shell
prj pack myproject
# 输出：
# The packaging is located in this /tmp/myproject-8.0.0-mt7621.fpk
```

---

## 项目目录结构

使用 `prj` 创建的典型项目具有以下结构：

```
/app/<项目名称>/
├── prj.json              # 项目配置
├── <组件>.ash            # Shell 组件（如果已创建）
├── <wui名称>.html        # Web UI 页面（如果已创建）
├── <wui名称>-cn.json     # 中文语言文件
└── <wui名称>-en.json     # 英文语言文件
```

## prj.json 模式

项目配置文件遵循以下结构：

```json
{
    "name": "项目名称",
    "intro": "项目简介",
    "desc": "详细描述",
    "type": "root",
    "version": "8.0.0",
    "author": "作者名称",
    
    "com": {
        "组件名称": "组件描述"
    },
    
    "obj": {
        "对象名称": "底层组件"
    },
    
    "init": {
        "启动级别": {
            "组件api": ""
        }
    },
    
    "uninit": {
        "关机级别": {
            "组件api": ""
        }
    },
    
    "joint": {
        "事件名称": {
            "组件api": ""
        }
    },
    
    "wui": {
        "页面id": {
            "menu": "菜单类别",
            "cn": "中文标题",
            "en": "英文标题",
            "page": "page.html",
            "config": "项目@组件",
            "lang": {
                "cn": "cn.json",
                "en": "en.json"
            }
        }
    }
}
```

## 组件模板 (component.ash)

Shell 组件模板提供基本结构：

```bash
#!/bin/bash
. $cheader

setup()
{
    # 初始化代码
    creturn ttrue
}

shut()
{
    # 清理代码
    creturn ttrue
}

service()
{
    # 后台服务循环
    while :
    do
        # 服务逻辑
        sleep 1
    done
    creturn tfalse
}

list()
{
    ret='{"key":"value"}'
    creturn $ret
}

cend
```

## Web UI 模板 (page.html)

HTML 模板包括：
- 基于 Bootstrap 的响应式布局
- 支持 i18n 的表单控件
- 与 HE 命令集成的 JavaScript
- 自动配置加载/保存

关键 JavaScript 变量：
- `comname` - 完整组件名称（例如 "myproject@sensor"）
- `comcfg` - 组件配置对象
- `langjson` - 语言文件路径

## 示例

### 完整的项目创建工作流程

```shell
# 1. 创建新项目
prj create myapp
# 输入：我的应用程序

# 2. 添加 shell 组件
prj add_com myapp controller

# 3. 添加 Web UI 页面
prj add_wui myapp dashboard
# 输入中文：仪表盘
# 输入英文：Dashboard

# 4. 注册启动任务
prj add_init myapp app myapp@controller.setup

# 5. 注册关机任务
prj add_uninit myapp app myapp@controller.shut

# 6. 注册网络事件处理器
prj add_joint myapp network/online myapp@controller.online

# 7. 验证项目
prj check myapp

# 8. 打包分发
prj pack myapp
```

## 故障排除

| 问题 | 解决方案 |
|------|----------|
| "cannot mkdir" | 检查目录权限 |
| "json format error" | 验证 prj.json 中的 JSON 语法 |
| "no exist" | 项目不存在；先使用 `create` |
| "path is under PROJECT_DIR" | 打包前将项目移动到 PROJECT_APP_DIR |
| 组件未注册 | 确保组件文件具有可执行权限 |

## 另请参阅

- [README.cn.md](../README.cn.md) - 统一架构概述和项目指南
- [land/fpk.md](../land/fpk.md) - FPK 打包格式
- [land/component.md](../land/component.md) - 组件开发指南
