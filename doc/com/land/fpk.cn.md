## land@fpk — FPK 包管理

管理系统 FPK 项目。

### 概念

#### 项目概念
* 项目是为满足特定需求而开发的程序集合，类似于 Windows 或 Android 上的应用程序。
* 每个项目在 SDK 的 `./project` 目录下有一个项目目录。源代码和资源存储在该目录中。
* 所有开发工作都应在项目内组织。
* 每个项目目录必须包含：
    - 项目信息文件，命名为 **prj.json**
    - 项目构建 Makefile。例如，使用 OpenWrt 构建系统时，Makefile 遵循 OpenWrt 格式。
* 每个项目目录可以包含：
    - 可执行文件或可执行源代码（SDK 包含源代码；设备安装仅包含二进制文件）
    - 库或库源代码（SDK 包含源代码；设备安装仅包含二进制文件）
    - 内核驱动或驱动源代码（SDK 包含源代码；设备安装仅包含二进制文件）
    - 组件源代码（SDK 包含源代码；设备安装仅包含组件二进制文件）
    - 组件或项目的默认配置文件
    - 用于用户管理的网页文件
    - 网页界面的语言文件
    - 脚本文件和其他资源文件

#### FPK 概念
* 开发完成后，项目被打包为 FPK（安装包）并安装到系统中。
* FPK 类似于 Windows 上的安装包或 Android 上的 APK。
* FPK 文件名以 `.fpk` 结尾
* FPK 可以通过网页或命令行安装。
* 每个 FPK 必须包含：
	- 名为 **prj.json** 的项目信息文件
* 每个 FPK 可以包含：
	- 库
	- 可执行文件
	- 驱动文件
	- 以 `.com` / `.ash` 结尾的组件文件
	- 以 `.cfg` 结尾的配置文件
	- 以 `.html` 结尾的网页文件
	- 以 `.json` 结尾的语言文件
	- 以 `.sh` 结尾的 Shell 脚本文件
	- 其他资源文件
	- `install/include` 下用于开发的库头文件（用于 SDK 编译）
	- `install/lib` 下用于开发的库（用于 SDK 编译）

#### 运行时安装路径（符号）

文档使用**尖括号占位符**代替固定路径。它们对应**平台构建宏**（值因平台/产品而异）；下表列出常用宏名：

| 符号 | 典型含义 | C 宏（参考） |
|------|---------|-------------|
| **`⟨PRJ_ROOT⟩`** | **已安装**项目（FPK 载荷）的根目录 | **`PROJECT_DIR`** |
| **`⟨PRJ_NAME⟩`** | 一个项目的子目录（与 `prj.json` -> `name` 相同） | -- |
| **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** | 该项目在设备上的安装前缀 | -- |
| **`⟨LIB_DIR⟩`** | 用于符号链接的全局共享库目录 | **`PROJECT_LIB_DIR`** |
| **`⟨BIN_DIR⟩`** | 用于符号链接的全局命令目录 | **`PROJECT_BIN_DIR`** |
| **`⟨SYS_ROOT⟩`** | 运行系统根目录（用于合并的 `rootfs/` 树、`/etc` 等） | -- |

例如 **`land@fpk.list`** JSON 字段中使用 **`⟨PRJ_ROOT⟩/…`**，这样在构建时更改 **`PROJECT_DIR`** 后仍然有效。

#### prj.json：项目信息文件
此文件在创建项目时自动生成。了解其格式有助于验证项目开发输出。
```json
// 属性
{
    "name":"project (FPK) name",                   // [ string ]
    "intro":"project (FPK) introduction",          // [ string ]
    "desc":"detailed description of the project",  // [ string ]
    "type":"project (FPK) type",                   // [ "root" ]，"root" 表示需要 root 权限
    "version":"project (FPK) version",             // [ string ]
    "author":"project (FPK) author",               // [ string ]

    "osc":                           // 项目中包含的所有开源程序显示在这些属性中
    {
        "open-source program directory":"description"
        // "...":"..." 每个开源程序显示一条属性
    },
    "lib":                           // 项目中包含的所有库显示在这些属性中
    {
        "library directory":"description"
        // "...":"..." 每个库显示一条属性
    },
    "exe":                           // 项目中包含的所有可执行程序显示在这些属性中
    {
        "executable program directory":"description"
        // "...":"..." 每个可执行程序显示一条属性
    },
    "com":                           // 项目中包含的所有组件显示在这些属性中
    {
        "component directory":"description"
        // "...":"..." 每个组件显示一条属性
    },
    "res":                           // 项目中包含的所有资源文件或目录显示在这些属性中
    {
        "resource file or directory":"description"
        // "...":"..." 每个资源文件或目录显示一条属性
    },
    "obj":                           // 项目中包含的所有对象（动态组件）显示在这些属性中
    {
        "object name":"actual components"
        // "...":"..." 每个对象（动态组件）显示一条属性
    },
    "init":                          // 项目中包含的所有启动项显示在这些属性中
    {
        "initialize level":"components method"
        // "...":"..." 每个启动项显示一条属性
    },
    "uninit":                        // 项目中包含的所有关机项显示在这些属性中
    {
        "shutdown level":"components method"
        // "...":"..." 每个关机项显示一条属性
    },
    "joint":                         // 项目中包含的所有 Joint 处理项显示在这些属性中
    {
        "joint event":"components method"
        // "...":"..." 每个 Joint 处理项显示一条属性
    }
}
// 示例
{
    "name":"arch",                              // arch 项目
    "intro":"mtk platform layer for farm os",   // 项目介绍
    "desc":"This project for MTK chips provides a unified management and usage interface for upper-layer projects",
                                                // 项目描述
    "type":"root",                              // 表示需要 root 权限
    "version":"6.0.0",                          // 版本为 6.0.0
    "author":"dimmalex@gmail.com",              // 作者为 dimmalex@gmail.com
    "osc":                                      // 有 1 个开源程序
    {
        "ntpclient":"ntp client"                      // ntpclient
    },
    "lib":                                      // 有 1 个库
    {
        "land":"core library"                         // land 核心库
    },
    "exe":                                      // 有 2 个可执行程序
    {
        "daemon":"service daemon",                    // daemon，服务实现
        "he":"tools for calling all components"       // he，skinos 的命令工具
    },
    "com":                                      // 有 7 个组件
    {
        "device":"device information",                // device，管理所有设备
        "data":"data management",                     // data，管理配置和 EEPROM
        "firmware":"firmware management",             // firmware，固件管理
        "gpio":"register and gpio management",        // gpio，寄存器和 GPIO 管理
        "test":"test device management",              // test，管理工厂测试
        "ethernet":"ethernet switch management",      // ethernet，以太网交换管理
        "mt7628":"802.11n wireless management"        // mt7628，无线管理
    },
    "res":                                      // 有 1 个资源文件
    {
        "testpage.py":"test only"                    // 工厂测试工具
    },
    "obj":                                      // 有 2 个对象（动态组件）
    {
        "wifi@nradio":"mt7628",                      // 对象为 wifi@nradio，实际组件为 mt7628
        "test":"test"                                // 对象为 test，实际组件为 test
    },
    "init":                                      // 有 3 个启动项
    {
        "ethernet":"arch@ethernet.setup",            // 在初始化级别 ethernet 调用
        "nradio":"wifi@nradio.setup",                // 在初始化级别 nradio 调用
        "aradio":"wifi@aradio.setup"                 // 在初始化级别 aradio 调用
    },
    "uninit":                                      // 有 2 个关机项
    {
        "nradio":"wifi@nradio.shut",                 // 在关机级别 nradio 调用
        "aradio":"wifi@aradio.shut"                  // 在关机级别 aradio 调用
    },
    "joint":                                      // 有 14 个 Joint 处理项
    {
        "firmware/upgrading":"arch@gpio.event",  // firmware/upgrading 发生时调用
        "firmware/upgraded":"arch@gpio.event",   // firmware/upgraded 发生时调用 arch@gpio.event
        "network/arise":"arch@gpio.event",       // network/arise 发生时调用 arch@gpio.event
        "network/ready":"arch@gpio.event",
        "network/lining":"arch@gpio.event",
        "network/online":"arch@gpio.event",
        "network/offline":"arch@gpio.event",
        "modem/poweron":"arch@gpio.event",
        "modem/poweroff":"arch@gpio.event",
        "modem/msim":"arch@gpio.event",
        "modem/bsim":"arch@gpio.event",
        "signal/flash":"arch@gpio.event",
        "nssid/up":"arch@gpio.event",
        "nssid/down":"arch@gpio.event"
    }    
}
```

### 配置 ( `land@fpk` )

`land@fpk` 的**持久化配置对象**（通过 `land@fpk`、`land@fpk:path` 查询/设置，合并 `|{json}` 等）。


`land@fpk` **不**像功能组件那样使用独立的 JSON 配置文档。每个已安装的项目在 **`⟨PRJ_ROOT⟩/⟨PRJ_NAME⟩/`** 下携带自己的 **`prj.json`**。下面的方法在运行时注册、取消注册或检查这些项目。

### 组件 API

+ `register[ project directory [, ...] ]` **将项目注册到系统**。此 API 在启动时调用以注册所有项目。
    - project directory ----------- [ string ]，项目目录
    - ... ------------------------- [ string ]，注册多个项目目录
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，注册一个项目
    ```shell
    land@fpk.register[ ⟨PRJ_ROOT⟩/uart ]
    ttrue
    ```

+ `unregister[ project name [, ...] ]` **从系统取消注册项目**
    - project name ----------- [ string ]，项目名称
    - ... ------------------------- [ string ]，取消注册多个项目名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，取消注册一个项目
    ```shell
    land@fpk.unregister[ uart ]
    ttrue
    ```

+ `list[ [project] ]` **列出项目信息**
    - project ----------- [ string ]，提供时获取指定项目的详细信息
    - 返回 JSON

    示例，显示 `ifname` 项目的信息
    ```shell
    land@fpk.list[ifname]
    {
        "name":"ifname",
        "intro":"skinos common network connection",
        "desc":"skinos ip connect and wifi connect component",
        "type":"root",
        "version":"8.0.0",
        "author":"dimmalex@gmail.com",
        "com":
        {
            "ethcon":"ethernet connect component",
            "ltecon":"lte modem connect component"
        },
        "path":"⟨PRJ_ROOT⟩/ifname/",
        "size":"74923"
    }
    ```
    示例，显示所有项目信息
    ```
    land@fpk.list
    {
        "agent":
        {
            "path":"⟨PRJ_ROOT⟩/agent/",
            "size":"155997",
            "intro":"agent for remote or cloud control",
            "version":"7.0.0",
            "author":"dimmalex@gmail.com"
        },
        "arch":
        {
            "path":"⟨PRJ_ROOT⟩/arch/",
            "size":"289294",
            "intro":"mtk mt7981 platform layer for skinos",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "client":
        {
            "path":"⟨PRJ_ROOT⟩/client/",
            "size":"66180",
            "intro":"Client management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "clock":
        {
            "path":"⟨PRJ_ROOT⟩/clock/",
            "size":"59199",
            "intro":"System clock management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "forward":
        {
            "path":"⟨PRJ_ROOT⟩/forward/",
            "size":"157035",
            "intro":"Network forward function",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "ifname":
        {
            "path":"⟨PRJ_ROOT⟩/ifname/",
            "size":"74923",
            "intro":"skinos common network connection",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "land":
        {
            "path":"⟨PRJ_ROOT⟩/land/",
            "size":"153711",
            "intro":"component infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "modem":
        {
            "path":"⟨PRJ_ROOT⟩/modem/",
            "size":"185095",
            "intro":"modem management",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "network":
        {
            "path":"⟨PRJ_ROOT⟩/network/",
            "size":"479667",
            "intro":"network infrastructure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "pdriver":
        {
            "path":"⟨PRJ_ROOT⟩/pdriver/",
            "size":"467419",
            "intro":"Portable driver",
            "version":"6.0.0",
            "author":"dimmalex@gmail.com"
        },
        "tui":
        {
            "path":"⟨PRJ_ROOT⟩/tui/",
            "size":"37987",
            "intro":"Terminal user interface service",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "webs":
        {
            "path":"⟨PRJ_ROOT⟩/webs/",
            "size":"49283",
            "intro":"web server",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wifi":
        {
            "path":"⟨PRJ_ROOT⟩/wifi/",
            "size":"108456",
            "intro":"skinos wireless configure",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        },
        "wui":
        {
            "path":"⟨PRJ_ROOT⟩/wui/",
            "size":"20066",
            "intro":"web user interface page",
            "version":"8.0.0",
            "author":"dimmalex@gmail.com"
        }
    }
    ```


+ `install[ path [,...] ]` **安装 FPK 或项目目录到系统**
    - 每个参数 ----------- [ string ]，可为 **`.fpk` / `.tar.gz` 包**（先解压到临时目录再安装），或**已是展开的项目目录**（内含 **`prj.json`**）
    - ... ---------------- [ string ]，可一次安装多个路径
    - 失败返回 tfalse
    - 成功返回 ttrue（行为见上文：目录或压缩包）

    示例，安装 `wui` 项目 FPK 到系统
    ```shell
    land@fpk.install[ wui-7.0.0-x86.fpk ]
    ttrue
    ```

+ `uninstall[ project name [,...] ]` **从系统卸载项目**
    - project name ----------- [ string ]，项目名称
    - ... ---------------- [ string ]，卸载多个项目
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，从系统卸载 `wui` 项目
    ```shell
    land@fpk.uninstall[ wui ]
    ttrue
    ```

+ `number` **获取已安装项目的数量**
    - 失败返回 tfalse
    - 返回已安装项目的数量

    示例，获取已安装项目的数量
    ```shell
    land@fpk.number
    15
    ```

+ `project_add[ name, [intro] ]` **创建最小可写项目**（位于 **`PROJECT_APP_DIR`**，与 **`install`** 使用的可写项目区一致）
    - **name** ----------- [ string ]，目录名 / `prj.json` 的 **`name`**（必填）
    - **intro** ---------- [ string ]，可选，写入 **`prj.json`**
    - 失败返回 **NULL**（缺 **`name`** 时为 **EINVAL**）
    - 成功返回 JSON（含 **`path`**、默认 **`version`**、**`author`** 为 **`land@fpk`**）

+ `project_delete[ name ]` **删除整个项目目录**
    - **name** ----------- [ string ]，由 **`project_path`** 解析
    - 失败返回 **tfalse**
    - 成功返回 **ttrue**（对目录执行 **`rm -fr`**）

+ `project_check[ name ]` **检查项目存在并通过内部校验**
    - **name** ----------- [ string ]
    - 失败返回 **tfalse**
    - 成功返回 **ttrue**（项目通过 **`project_check`** 时）

+ `project_pack[ name ]` **将可写项目打成 `.fpk` 包**
    - **name** ----------- [ string ]，须解析到 **`PROJECT_APP_DIR`** 下（若在固件 **`PROJECT_DIR`** 下则为 **EPERM** / **NULL**）
    - 失败返回 **NULL**
    - 成功返回 talk 字符串路径：**`⟨TMP⟩/name-⟨version⟩-⟨hardware⟩.fpk`**（**`PROJECT_TMP_DIR`**、**`PROJECT_DEFAULT_VERSION`**、寄存器 **`hardware`**；内容为 **`tar zcf`**）

+ `com_add[ prj, name, [intro] ]` **向项目添加 shell 组件脚手架并登记**
    - **prj** ------------ [ string ]，项目名称（**`project_path`**）
    - **name** ----------- [ string ]，组件短名；运行对象名为 **`prj`⟨@⟩`name`**（**`PROJECT_OBJECT_GAPC`**）
    - **intro** ---------- [ string ]，可选，写入 **`prj.json` → `com`**
    - 复制模板 **`comshell`**，更新 **`prj.json`**，再调用 **`com_register( prj@name, comshell路径, 0 )`** —— 第二参数语义同 **`land@component.register`** / **`com_register(..., 0)`**（**`type` 0** 时走 **`COM_COM`** 映射查找）
    - 失败返回 **NULL**
    - 成功返回 talk 字符串：项目目录路径

+ `wui_add[ prj, name, menu ]` **添加 Web UI 菜单项并生成页面/语言文件骨架**
    - **prj** ------------ [ string ]，项目名称
    - **name** ----------- [ string ]，**`prj.json` → `wui`** 下的键
    - **menu** ----------- [ string ]，英文菜单文案（写入 **`en`**）
    - 生成 **`name.html`**、**`name-cn.json`** / **`name-en.json`**，更新 **`prj.json`**
    - 失败返回 **NULL**
    - 成功返回 JSON（新 **`wui`** 项，路径为绝对路径）

+ `obj_add[ prj, object, origin ]` **添加动态对象映射并注册**
    - **prj** ------------ [ string ]，项目名称
    - **object** --------- [ string ]，**`prj.json` → `obj`** 的键
    - **origin** --------- [ string ]， backing 组件目录名（**`obj`** 的值），**`land@component.register`** 第二参数为 **`prj`⟨@⟩`origin`**
    - 更新 **`prj.json`** 后执行 **`land@component.register[ object, prj@origin ]`**
    - 失败 **tfalse**，成功 **ttrue**

+ `init_add[ prj, level, call ]` **追加 init 项并运行时注册**
    - **prj** / **level** / **call** --- [ string ]；**`level`** 为 **`prj.json` → `init`** 的键，**`call`** 为方法串（如 **`arch@ethernet.setup`**）
    - 更新 **`prj.json`** 后 **`land@init.register[ level, call ]`**
    - 失败 **tfalse**，成功 **ttrue**

+ `uninit_add[ prj, level, call ]` **追加 uninit 项并注册**
    - 形式同 **`init_add`**，对应 **`prj.json` → `uninit`** 与 **`land@uninit.register`**

+ `joint_add[ prj, level, call ]` **追加 joint 项并注册**
    - 形式同 **`init_add`**，对应 **`prj.json` → `joint`** 与 **`land@joint.register`**（**`level`** 为事件名）

+ `wui_menu[ [type] ]` **获取 Web UI 菜单结构**
    - **type** ----------- [ string ]，可选；指定扫描 **`prj.json`** 顶层的哪个对象（默认 **`wui`**）
    - 若平台 **custom** 组件下 **`project`** 映射将某项目标为 **`disable`**，则跳过该项目（对象名因产品而异，常见为 **`arch@custom`**）
    - 各菜单项可能要求已有配置（**`config`** / **`attr`**）或 **`object`**/**`api`** 可用（**`com_have`**）；不满足的项会被过滤
    - 失败返回 NULL
    - 返回 JSON（**`page`**、**`lang`** 等路径改写为绝对路径）

    示例，获取 Web UI 菜单
    ```shell
    land@fpk.wui_menu
    {
        "wifi_aclient":
        {
            "menu":"Wireless",
            "cn":"5.8G客户端",
            "en":"5.8G Clients",
            "page":"/skinos/wifi/client.html",
            "object":"wifi@a",
            "lang":
            {
                "cn":"/skinos/wifi/cn",
                "en":"/skinos/wifi/en"
            }
        },
        "wui_webs":
        {
            "menu":"System",
            "cn":"WEB服务器",
            "en":"Web Server",
            "page":"/skinos/wui/admin.html",
            "config":"wui@admin",
            "lang":
            {
                "cn":"/skinos/wui/cn",
                "en":"/skinos/wui/en"
            }
        }
    }
    ```

### 生命周期 API

+ 通常意义上**没有** `setup[]` -- **`land@fpk`** 暴露在启动时使用的 **register/install** API。
+ 请参阅**组件 API** 中的 **`register[]`**、**`install[]`** 等。


### C 代码示例

```c
#include "skin/skin.h"

static void example_land_fpk(void)
{
    talk_t ret = scall("land@fpk", "number", NULL);
    (void)ret;
}
```
