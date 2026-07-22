# AGENTS.md — 项目与组件开发指南

本指南专注于在 `project/` 目录下开发 landos 项目和组件。

## 核心概念

**组件命名**: `PROJECT_ID@COM_ID` (如 `land@machine`, `forward@nat`)
**项目结构**: 每个项目是一个独立目录，包含 `prj.json` 清单和源代码

## 快速开始：从模板创建新项目

完整步骤（组件 / exe / 网页 / 语言包 / init / uninit / joint）见仓库根目录：

**`../.claude/skills/skinos-project/SKILL.md`**

1. 复制 `project/tmptools/` 作为模板
2. 修改 `prj.json` 中的 `name`, `intro`, `desc`
3. 重命名组件目录
4. 构建: `make obj=<项目名>`

## 项目目录结构

```
project/<name>/
├── prj.json          # 项目清单 (必需)
├── Makefile          # OpenWrt风格构建脚本
├── <component>/      # 组件目录 (com字段注册)
│   └── <component>.c
├── <executable>/     # 可执行程序 (exe字段注册)
│   └── <name>.c
├── <library>/        # 共享库 (lib字段注册)
│   ├── <name>.c
│   └── <name>.h
├── osc/              # 第三方开源代码
│   ├── main.c
│   └── mconfig
├── mconfig           # 本地编译配置 (可选)
├── cn.json           # 中文语言包
├── en.json           # 英文语言包
└── *.html            # Web UI页面
```

## prj.json 清单详解

```json
{
    "name": "myproject",           // 项目ID
    "intro": "short description",  // 简短描述
    "desc": "detailed description", // 详细描述
    "type": "root",                // root=系统级包, 空=普通包
    "version": "8.0.0",
    "author": "email@example.com",

    "com": {                       // 组件目录
        "mycom": "组件描述"
    },
    "exe": {                       // 可执行程序
        "myexe": "程序描述"
    },
    "lib": {                       // 共享库
        "mylib": "库描述"
    },
    "osc": {                       // 第三方代码
        "mythirdparty": "第三方描述"
    },

    "init": {                      // 启动时调用
        "arch": { "project@component.setup": "" },      // 最早启动
        "land": { "project@component.setup": "" },       // 基础层启动
        "app": { "project@component.setup": "" },        // 应用层启动
        "general": { "project@component.setup": "" },    // 通用启动
        "manage": { "project@component.setup": "" },     // 管理层启动
        "delay": { "project@component.setup": "" }       // 延迟启动
    },
    "joint": {                     // 事件触发
        "network/on": { "project@component.on": "" },
        "network/online": { "project@component.online": "" },
        "storage/insert": { "project@component.storage": "" }
    },
    "uninit": {                    // 关闭时调用
        "project@component.shut": ""
    },

    "wui": {                       // Web UI注册
        "mycom": {
            "en": "English Title",
            "cn": "中文标题",
            "page": "mycom.html",
            "config": "project@mycom",
            "lang": { "cn": "cn.json", "en": "en.json" }
        }
    }
}
```

## 组件开发模板

### 标准组件 (`component/<name>.c`)

```c
#include "skin/skin.h"

// 启动函数
boole_t _setup(obj_t this, param_t param)
{
    const char *object = obj_name(this);
    talk_t cfg = config_get(this, NULL);
    if (cfg == NULL) {
        app_warn("%s: setup failed - no config", object);
        return tfalse;
    }
    
    const char *status = json_string(cfg, "status");
    if (status != NULL && strcmp(status, "enable") == 0) {
        app_info("%s: starting service", object);
        cstart(this, "service", NULL, object);  // 启动服务
    }
    
    talk_free(cfg);
    return ttrue;
}

// 关闭函数
boole_t _shut(obj_t this, param_t param)
{
    const char *object = obj_name(this);
    app_info("%s: shutting down", object);
    sdelete(object);  // 停止服务
    return ttrue;
}

// 服务函数 (长期运行)
boole_t _service(obj_t this, param_t param)
{
    const char *object = obj_name(this);
    app_info("%s: service started", object);
    
    while (1) {
        // 你的业务逻辑
        sleep(1);
    }
    return tfalse;
}

// 获取配置
talk_t _get(obj_t this, attr_t path)
{
    return config_get(this, path);
}

// 设置配置
boole _set(obj_t this, talk_t v, attr_t path)
{
    boole ret = config_set(this, v, path);
    if (ret == true) {
        _shut(this, NULL);
        _setup(this, NULL);  // 重启服务
    }
    return ret;
}

// 网络事件处理
talk_t _online(obj_t this, param_t param)
{
    const char *event = param_string(param, 1);
    talk_t v = param_talk(param, 2);
    const char *ifname = (v != NULL) ? json_string(v, "ifname") : NULL;
    
    app_info("%s: network event %s on %s", obj_name(this), 
             event != NULL ? event : "", ifname != NULL ? ifname : "");
    return ttrue;
}
```

### 可执行程序 (`exe/<name>.c`)

```c
#include "skin/skin.h"

boole_t _setup(obj_t this, param_t param) { return ttrue; }
boole_t _shut(obj_t this, param_t param) { return ttrue; }
boole_t _service(obj_t this, param_t param) { pause(); return tfalse; }

static const eapi_table_t exe_api_table[] = {
    { "setup",   (comapi_t)_setup },
    { "shut",    (comapi_t)_shut },
    { "service", (comapi_t)_service },
};

MAIN2API(exe_api_table);
```

### 共享库 (`lib/<name>.c`)

```c
#include "skin/skin.h"
#include "mylib.h"

int mylib_function(int param)
{
    // 库函数实现
    return 0;
}
```

## 编译宏定义

构建系统自动提供以下宏：

| 宏 | 类型 | 说明 |
|---|------|------|
| `PROJECT_ID` | String | 项目名称 (来自prj.json) |
| `COM_ID` | String | 组件目录名 |
| `COM_IDPATH` | String | 完整组件名 `PROJECT_ID@COM_ID` |
| `EXE_ID` | String | 可执行程序目录名 |
| `EXE_IDPATH` | String | 完整程序名 `PROJECT_ID@EXE_ID` |
| `gPLATFORM` | String | 平台名 (如 "MTK", "OPENWRT") |
| `HARDWARE` | String | 芯片名 (如 "MT7628", "MT7981") |
| `gCUSTOM` | String | 产品型号 (如 "D218", "R607") |
| `gPLATFORM__*` | Define | 平台宏 (如 `gPLATFORM__MTK`) |
| `gHARDWARE__*` | Define | 芯片宏 (如 `gHARDWARE__mt7628`) |
| `gCUSTOM__*` | Define | 产品宏 (如 `gCUSTOM__D218`) |

## mconfig 编译配置

每个组件/程序目录可包含 `mconfig` 文件：

```makefile
# 添加头文件路径
CFLAGS += -I../ -I../../other_project

# 添加链接库
LDFLAGS += -lmylib

# 静态库依赖
LIBA_ADDIN := ../lib/libmylib.a

# 动态库依赖
LIBSO_ADDIN := ../lib/libmylib.so
```

## 常用 skin API

### 头文件包含
```c
#include "skin/skin.h"  // 包含所有常用头文件
```

### 核心数据类型
- `talk_t` - JSON通信数据类型 (使用后需 `talk_free`)
- `obj_t` - 组件对象 (使用后需 `obj_free`)
- `attr_t` - 属性路径 (使用后需 `attr_free`)
- `param_t` - 参数结构 (使用后需 `param_free`)
- `boole_t` - 布尔类型 (`ttrue`, `tfalse`, `terror`, `tpanic`, `tnull`)

### 日志宏
```c
app_info("format", ...);   // 信息日志
app_warn("format", ...);   // 警告日志
app_fault("format", ...);  // 错误日志
```

### 配置操作
```c
talk_t cfg = config_get(this, NULL);           // 获取完整配置
const char *val = json_string(cfg, "key");     // 读取字符串
int num = json_number(cfg, "key");             // 读取整数
config_set(this, value, attr);                 // 保存配置
```

### 组件调用
```c
void *ret = scall("project@component", "api", param);  // 调用其他组件
talk_t json = string2json("{\"key\":\"value\"}");       // 创建JSON
char *str = json2string(json);                          // JSON转字符串 (需free)
```

### 服务管理
```c
cstart(this, "service", NULL, name);   // 启动服务
cstop(this, "service", NULL, name);    // 停止服务
creset(this, "service", NULL, name);   // 重启服务
sdelete(name);                         // 删除服务
int pid = spid(name);                  // 获取服务PID
```

### 注册变量 (跨进程共享)
```c
int *p = reg_set_int(this, "name", value);  // 设置整数注册变量
int val = reg_int(this, "name");            // 获取整数注册变量
reg_set_string(this, "name", "value");      // 设置字符串注册变量
const char *s = reg_string(this, "name");   // 获取字符串注册变量
```

## 构建命令

```bash
# 构建单个项目
make obj=<项目名>

# 清理单个项目
make clean obj=<项目名>

# 构建所有项目
make app

# 准备构建环境
make dep
```

## 事件系统 (joint)

常用事件：
- `network/on` - 本地网络接口启动
- `network/onextern` - 外部网络接口启动
- `network/online` - 互联网连接建立
- `storage/insert` - 存储设备插入
- `storage/remove` - 存储设备移除

事件处理函数签名：
```c
talk_t _on(obj_t this, param_t param)
{
    const char *event = param_string(param, 1);   // 事件名
    talk_t v = param_talk(param, 2);               // 事件数据
    const char *ifname = json_string(v, "ifname"); // 网络接口名
    return ttrue;
}
```

## Web UI 集成

1. 在 `prj.json` 的 `wui` 字段注册页面
2. 创建 HTML 页面文件
3. 创建语言包文件 (`cn.json`, `en.json`)
4. 使用 `config` 字段绑定组件配置

## 调试技巧

```bash
# 在设备上查看组件配置
he 'project@component'

# 设置组件配置
he 'project@component:key=value'

# 调用组件API
he 'project@component.api'

# 列出组件（HE 元命令；不是 land@component.list）
he '@'
# 或按工程过滤: he '*land'

# 查看日志文件列表
he 'land@syslog.list'
```

## 参考资源

- 模板项目: `project/tmptools/`
- 核心文档: `doc/com/land/`（入口 `README.md`，含架构与术语）
- skin API文档: `doc/com/land/skin.md`（`project/land/skin.md` 为同步副本）
- HE语法: `doc/com/land/he.md`
- 组件模型: `doc/com/land/component.md`
