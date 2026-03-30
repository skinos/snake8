## land@machine — 系统信息与控制

提供基本网关/设备信息、重启/重置控制和系统资源查询。对象名：`land@machine`。
管理网关基本信息

**谁应该阅读什么：** JSON 和 `he` / shell 示例适用于使用命令行的操作员和集成商。末尾的可选 **C 语言**代码片段与实际头文件匹配（`#include "skin/skin.h"`、`talk_free`、`scall`、`json_*`、...）；如果你不嵌入平台 SDK，可以跳过它们。

**注意：** `cpuinfo`、`meminfo`、`psinfo`、`psnumber` 和 `fsinfo`（以及基于相同数据构建的 API）适用于**基于 Linux** 的固件镜像。启动时，machine 组件完成其常规注册，应用**配置的主机名**（在系统限制内），并发布 **Web UI、telnet、SSH 和 LAN IP** 设置，以便系统的其他部分可以读取它们。

**策略锁定：** 在 **`lock`** 配置中，**`default=enable`** 禁止 **`restart`** 和 **`default`**（恢复默认值）。**`factory=enable`** 禁止 **`release`** 和 **`factory`**。另外，**`restart_block`** / **`restart_unblock`** 切换是否允许 **`restart`**；**`default_block`** / **`default_unblock`** 切换是否允许 **`default`**、**`release`** 和 **`factory`**。

### 配置（`land@machine`）

`land@machine` 的**保存配置对象**（通过 `land@machine`、`land@machine:path`、合并 `|{json}` 等查询/设置）。

```json
// 属性介绍 
{
    // 主要属性
    "mode":"网关运营商模式",                 // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                       // "ap": 接入点
                                                       // "wisp": 2.4G 无线互联网服务提供商连接
                                                       // "nwisp": 5.8G 无线互联网服务提供商连接（需要主板支持 5.8G 无线基带）
                                                       // "gateway": 有线 WAN 网关
                                                       // "dgateway": 双有线 WAN 网关
                                                       // "misp": LTE 移动互联网服务提供商连接（需要主板支持 LTE 基带）
                                                       // "nmisp": 下一代移动（NR/LTE）互联网服务提供商连接（需要主板支持 NR/LTE 基带）
                                                       // "dmisp": 双移动（LTE/NR）互联网服务提供商连接（需要主板支持两个 LTE/NR 基带）
                                                       // "mwm": 多 LTE 和无线网关
                                                       // "mix": 来自多个互联网连接的自定义混合连接
                                                       // "mbridge" 或 "default": LAN dhcps 状态设置为禁用
    "name":"网关名称",                          // < string >，名称不能包含空格
    "mac":"网关 MAC 地址",                    // < mac address >
    "macid":"网关 MAC 标识或序列号 ID",    // < string >
    "sn":"主板序列号",                     // 当产品公开时存在；此处为只读
    "language":"网关语言",                  // [ "cn", "en", "jp", ... ]，两个字母的语言代码，"cn" 表示中文，"en" 表示英文
    // 其他属性
    "cfgversion":"网关配置版本",       // [ string ]
    "gpversion":"网关组配置版本",  // [ string ]
    "broken":"网关系统状态"                 // [ "ill", "disable" ]
}
```

示例，显示所有基本配置
```shell
land@machine
{
    "mode":"nmisp",                              # 网关运营商模式 下一代移动（LTE/NR）互联网服务提供商连接
    "name":"ASHYELF-12AAD0",                     # 网关名称为 ASHYELF-12AAD0
    "mac":"00:03:7F:12:AA:D0",                   # 网关 MAC 为 00:03:7F:12:AA:D0
    "macid":"00037F12AAD0",                      # 网关序列号 ID 为 88124E2046B0
    "language":"cn",                             # 网关语言为中文
    "cfgversion":"1"                             # 网关配置版本为 1（提示修改过一次）
}
```  

示例，修改网关名称
```shell
land@machine:name=NewMachineName
ttrue
```  

示例，显示修改后的网关名称
```shell
land@machine:name
NewMachineName
```  

示例，将运营商模式修改为 "mix"
```shell
land@machine:mode=mix
ttrue
```  

示例，将网关语言修改为英文
```shell
land@machine:language=en
ttrue
```  

示例，一次更改多个属性（合并；仅列出的字段更改）
```shell
land@machine|{"name":"DemoGW","language":"en"}
ttrue
```

### 组件 API

+ `status[]` **获取网关基本信息**   
    - 返回描述网关基本信息的 JSON   

    ```json
    // 方法返回的 json 属性介绍
    {
        "mode":"网关运营商模式",              // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                         // "ap": 接入点
                                                         // "wisp": 2.4G 无线互联网服务提供商连接
                                                         // "nwisp": 5.8G 无线互联网服务提供商连接（需要主板支持 5.8G 无线基带）
                                                         // "gateway": 有线 WAN 网关
                                                         // "dgateway": 双有线 WAN 网关
                                                         // "misp": LTE 移动互联网服务提供商连接（需要主板支持 LTE 基带）
                                                         // "nmisp": 下一代移动（NR/LTE）互联网服务提供商连接（需要主板支持 NR/LTE 基带）
                                                         // "dmisp": 双移动（LTE/NR）互联网服务提供商连接（需要主板支持两个 LTE/NR 基带）
                                                         // "mwm": 多 LTE 和无线网关                                                         
                                                         // "mix": 来自多个互联网连接的自定义混合连接
        "name":"网关名称",
        "platform":"网关平台标识",
        "hardware":"网关硬件标识",
        "custom":"网关自定义标识",
        "scope":"网关范围标识",
        "version":"网关版本",
        "cfgversion":"网关配置版本",        // [ string ]
        "gpversion":"网关组配置版本",   // [ string ]
        "livetime":"系统运行时间",                // 小时:分钟:秒:天
        "current":"当前日期",                        // 小时:分钟:秒:月:日:年
        "mac":"网关 MAC 地址",
        "macid":"网关 MAC 标识或序列号 ID",
        "model":"网关型号",
        "features":"产品功能标志",
        "cmodel":"网关自定义型号",
        "oem":"OEM 标识字符串",
        "magic":"网关 magic 标识",
        "datecode":"生产日期代码",
        "rand":"公开时的可选伪随机整数",
        "wui_port":"产品配置中的 Web UI 端口",
        "telnet_port":"产品配置中的 telnet 端口",
        "ssh_port":"产品配置中的 SSH 端口",
        "local_ip":"产品配置中的 LAN 静态 IP"
    }    
    ```

    **`version`** 通常是固件或软件构建标识符。当重启正在进行时，**`version`** 可能会短暂显示**状态**字符串。

    示例，获取网关基本信息
    ```shell
    land@machine.status
    {
        "mode":"nmisp",                                   # 网关运营商模式 下一代移动（NR/LTE）互联网服务提供商连接
        "name":"ASHYELF-12AAD0",                          # 网关名称为 ASHYELF-12AAD0
        "platform":"smtk2",                               # 网关平台标识为 smtk2（提示基础 sdk 是 smtk 的第二个版本）
        "hardware":"mt7621",                              # 网关硬件标识为 mt7621（提示芯片是 MT7621）
        "custom":"d228",                                  # 网关自定义标识为 d228（提示产品名称是 D228）
        "scope":"std",                                    # 网关范围标识为 std（提示这是 D228 的标准发布版）
        "version":"tiger7-20220218",                      # 网关版本为 tiger7-20220218（提示版本发布于 2022.02.18）
        "cfgversion":"6",                                 # 网关 cfgversion 为 6，配置可能已修改 6 次
        "livetime":"00:06:35:0",                          # 网关已运行 6 分 35 秒
        "current":"14:54:30:05:04:2022",                  # 当前日期是 2022.05.04，现在是 14:54 30 秒
        "mac":"00:03:7F:12:AA:D0",                        # 网关 MAC 为 00:03:7F:12:AA:D0
        "macid":"00037F12AAD0",                           # 网关序列号 ID 为 00037F12AAD0
        "model":"5228",                                   # 网关型号为 5228
        "magic":"870E2935E4605D02"                        # 网关 magic 为 870E2935E4605D02
    }
    ```


+ `restart[ [delay] ]` **重启网关**   
    - delay ------- [ number ]，延迟若干秒后重启系统
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，立即重启网关（调用可能不会返回，因为系统正在重启）
    ```shell
    land@machine.restart
    # ---- system restart -----
    ```

    示例，10 秒后重启网关（调用将立即返回）
    ```
    land@machine.restart[ 10 ]
    ttrue
    ```

+ `restart_block[]` **禁用所有重启网关的操作**   
    - 失败返回 tfalse
    - 成功返回 ttrue
    
    示例，禁用所有重启网关的操作
    ```shell
    land@machine.restart_block
    ttrue
    # 然后在 land@machine.restart_block 之后你无法重启网关
    land@machine.restart
    tfalse, Operation not permitted
    ```

+ `restart_unblock[]` **启用重启网关**   
    - 失败返回 tfalse
    - 成功返回 ttrue

+ `reboot[ [delay] ]` **与 `restart` 相同**

+ `reboot_block[]` / `reboot_unblock[]` **与 `restart_block` / `restart_unblock` 相同**。

    示例
    ```shell
    # 首先，禁用所有重启网关的操作
    land@machine.restart_block
    ttrue
    # 然后在 land@machine.restart_block 之后你无法重启网关
    land@machine.restart
    tfalse, Operation not permitted
    # 启用重启网关
    land@machine.restart_unblock
    ttrue
    # 然后你可以重启网关
    land@machine.restart
    # ---- system restart ----
    ```

+ `default[ [delay] ]` **恢复网关默认配置并重启**   
    - delay ------- [ number ]，延迟若干秒后恢复系统默认
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，立即恢复网关默认配置并重启（调用可能不会返回，因为系统正在重启）
    ```shell
    land@machine.default
    # ---- system restart -----
    ```

    示例，10 秒后恢复网关默认配置并重启（调用将立即返回）
    ```
    land@machine.default[ 10 ]
    ttrue
    ```

+ `default_block[]` **禁用所有恢复网关默认配置的操作**   
    - 失败返回 tfalse
    - 成功返回 ttrue
    
    示例，禁用所有恢复网关默认配置的操作
    ```shell
    land@machine.default_block
    ttrue
    # 然后在 land@machine.default_block 之后你无法恢复网关默认配置
    land@machine.default
    tfalse, Operation not permitted
    ```

+ `default_unblock[]` **启用所有恢复网关默认配置的操作**   
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例
    ```shell
    # 首先，禁用所有恢复网关默认配置的操作
    land@machine.default_block
    ttrue
    # 然后在 land@machine.default_block 之后你无法恢复网关默认配置
    land@machine.default
    tfalse, Operation not permitted
    # 启用恢复网关默认配置
    land@machine.default_unblock
    ttrue
    # 然后你可以恢复网关默认配置
    land@machine.default
    # ---- system restart ----
    ```

+ `release[ [delay] ]` **产品发布重置，然后重启** — 与 **`default`** 相同的阻止和 **`lock`** 规则（包括 **`default_block`** / **`factory=enable`**）。可选的 **delay** 参数行为与 **`restart`** 相同。

+ `factory[ [delay] ]` **出厂重置，然后重启** — 与 **`release`** 相同的阻止和 **`lock`** 规则。

    ```shell
    land@machine.release
    land@machine.factory
    ```


+ `cpuinfo[]` **获取网关 CPU 信息**   
    - 返回描述 CPU 统计信息的 JSON  

    ```json
    // 方法返回的 json 属性介绍
    {
        "cpu":
        {
            "user":"用户模式下执行的正常进程",
            "nice":"用户模式下执行的低优先级进程",
            "system":"内核模式下执行的进程",
            "idle":"空闲时间",
            "iowait":"等待 I/O 完成",
            "irq":"服务中断",
            "softirq":"服务软中断"
        }
        //"cpu 核心标识":{ cpu 信息 }     有多少个 CPU 核心就显示多少个属性
    }
    ```

    示例，获取网关 cpu 信息
    ```shell
    land@machine.cpuinfo
    {
        "cpu":                       # 总 CPU
        {
            "user":"389",
            "nice":"0",
            "system":"2769",
            "idle":"34014",
            "iowait":"12",
            "irq":"0",
            "softirq":"493"
        },
        "cpu0":                      # CPU 核心 0
        {
            "user":"142",
            "nice":"0",
            "system":"996",
            "idle":"8383",
            "iowait":"0",
            "irq":"0",
            "softirq":"13"
        },
        "cpu1":                      # CPU 核心 1
        {
            "user":"58",
            "nice":"0",
            "system":"396",
            "idle":"8851",
            "iowait":"0",
            "irq":"0",
            "softirq":"144"
        },
        "cpu2":                      # CPU 核心 2
        {
            "user":"131",
            "nice":"0",
            "system":"864",
            "idle":"8194",
            "iowait":"12",
            "irq":"0",
            "softirq":"168"
        },
        "cpu3":                      # CPU 核心 3
        {
            "user":"56",
            "nice":"0",
            "system":"512",
            "idle":"8584",
            "iowait":"0",
            "irq":"0",
            "softirq":"167"
        }
    }
    ```

+ `psinfo[]` **获取网关进程信息**   
    - 返回描述进程的 JSON  

    ```json
    // 方法返回的 json 属性介绍
    {
         // 有多少个进程就显示多少个属性
        "进程标识":
        {
            "name":"进程名称",
            "state":"进程状态",
            "ppid":"父进程标识",
            "fdsize":"当前分配的文件描述符槽位数",
            "vmsize":"整个进程的虚拟内存使用量"
        }
    }
    ```

    示例，获取网关进程信息
    ```shell
    land@machine.psinfo
    {
        "1":
        {
            "name":"procd",
            "state":"S",
            "ppid":"0",
            "fdsize":"32",
            "vmsize":"2736"
        },
        "2":
        {
            "name":"kthreadd",
            "state":"S",
            "ppid":"0",
            "fdsize":"32"
        },
        "3":
        {
            "name":"ksoftirqd/0",
            "state":"S",
            "ppid":"2",
            "fdsize":"32"
        },
        "4":
        {
            "name":"kworker/0:0",
            "state":"S",
            "ppid":"2",
            "fdsize":"32"
        },
        # ......
    }
    ```

+ `psnumber[]` **进程数量** — 大致的进程数量；可能与 **`psinfo`** 返回的对象数量略有不同。

    ```shell
    land@machine.psnumber
    42
    ```


+ `meminfo[]` **获取网关内存信息**   
    - 返回描述系统内存使用情况的 JSON  

    ```json
    // 方法返回的 json 属性介绍
    {
        "total":"总内存大小",       // 单位为 KB  
        "free":"空闲内存大小",         // 单位为 KB  
        "buffers":"缓冲区缓存大小",     // 单位为 KB  
        "cached":"页面缓存大小"         // 单位为 KB  
    }
    ```

    示例，获取网关内存信息
    ```shell
    land@machine.meminfo
    {
        "total":"125584",
        "free":"54076",
        "buffers":"6520",
        "cached":"22024"
    }
    ```


+ `fsinfo[]` **获取网关文件系统信息**   
    - 返回描述挂载文件系统的 JSON

    ```json
    // 方法返回的 json 属性介绍
    {
         // 有多少个文件系统就显示多少个属性
        "文件系统挂载路径":
        { 
            "filesystem":"设备路径",
            "size":"总大小",               // K 结尾表示 KB，M 结尾表示 MB
            "used":"已使用",            // K 结尾表示 KB，M 结尾表示 MB
            "available":"可用大小",      // K 结尾表示 KB，M 结尾表示 MB
            "use":"使用率"                 // 百分比
        }
    }
    ```

    示例，获取网关文件系统信息
    ```shell
    land@machine.fsinfo
    {
        "/":                                             # 根文件系统
        {
            "filesystem":"/dev/root",
            "size":"9.5M",
            "used":"9.5M",
            "available":"0",
            "use":"100%"
        },
        "/tmp/mnt/config":                                     # 配置文件系统
        {
            "filesystem":"/dev/mtdblock7",
            "size":"1.0M",
            "used":"200.0K",
            "available":"824.0K",
            "use":"20%"
        },
        "/tmp/mnt/internal":                                  # 内部固件存储
        {
            "filesystem":"/dev/mtdblock6",
            "size":"3.8M",
            "used":"260.0K",
            "available":"3.5M",
            "use":"7%"
        }
    }
    ```

+ `sginfo[]` **获取网关存储文件系统信息**   
    - 返回将**配置**存储使用情况与**扩展**存储的**每个挂载**使用情况相结合的 JSON（每个条目可用时可以包括 **`path`**、**`size`**、**`used`**、**`available`**、**`use`**）  

    ```json
    // 方法返回的 json 属性介绍
    {
         // 有多少个存储文件系统就显示多少个属性
        "存储文件系统标识":
        { 
            "filesystem":"设备路径",
            "size":"总大小",               // K 结尾表示 KB，M 结尾表示 MB
            "used":"已使用",            // K 结尾表示 KB，M 结尾表示 MB
            "available":"可用大小",      // K 结尾表示 KB，M 结尾表示 MB
            "use":"使用率"                 // 百分比
        }
    }
    ```

    示例，获取网关存储文件系统信息
    ```shell
    land@machine.sginfo
    {
        "config":                                 # 配置文件系统
        {
            "size":"1.0M",
            "used":"200.0K",
            "available":"824.0K",
            "use":"20%"
        },
        "internal":                                    # 内部固件存储
        {
            "path":"/tmp/mnt/int",
            "size":"3.8M",
            "used":"260.0K",
            "available":"3.5M",
            "use":"7%"
        }
    }
    ```

+ `esinfo[]` **获取网关扩展存储文件系统信息**   
    - 仅返回**附加**存储的 JSON（内置配置和内部区域不列出）  

    ```json
    // 方法返回的 json 属性介绍
    {
         // 有多少个扩展存储文件系统就显示多少个属性
        "存储文件系统标识":
        { 
            "filesystem":"设备路径",
            "size":"总大小",               // K 结尾表示 KB，M 结尾表示 MB
            "used":"已使用",            // K 结尾表示 KB，M 结尾表示 MB
            "available":"可用大小",      // K 结尾表示 KB，M 结尾表示 MB
            "use":"使用率"                 // 百分比
        }
    }
    ```

    示例，获取网关扩展存储文件系统信息
    ```shell
    land@machine.esinfo
    {
        "usb1":                                        # 示例附加卷；内置区域省略
        {
            "path":"/tmp/mnt/usb1",
            "size":"1.8G",
            "used":"120.0M",
            "available":"1.6G",
            "use":"7%"
        }
    }
    ```


### 生命周期 API

+ `setup[]` **初始化 machine 组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统在启动期间自动调用的生命周期方法
    - 它扫描并注册所有系统项目，设置主机名，并初始化网络接口
    - 不建议手动调用


### 发布的 Joint 事件

当机器状态变化时发布以下 joint 事件。其他组件可以在运行时订阅（joint 注册 / **`land@joint`**）。

| 事件 | 描述 |
|-------|-------------|
| `machine/status` | 当机器状态更新时发送。在以下场景触发：1) 在 `restart`/`reboot` 开始时，状态在寄存器中标记为 `restarting` 后；2) 固件升级过程中状态变为 `downloading`、`upgrading`、`failed` 或 `succeed` 时；3) 固件恢复操作期间。此事件通知其他组件有关系统状态转换，以便它们可以相应准备（例如，保存数据、关闭连接或更新 LED 指示器）。 |


### C 代码示例

**读取和更新配置** — `sgets` / `sgets_string`、`ssets_string` / `ssets`（与 HE 相同的路径）。

```c
#include "skin/skin.h"

static int machine_config_get_and_set(void)
{
    char name[64];
    talk_t mode;
    boole ok;

    /* 1) 获取字符串配置：name */
    if (sgets_string(name, sizeof(name), "land@machine", "name") == NULL)
    {
        return -1;
    }
    printf("current machine.name = %s\n", name);

    /* 2) 获取通用配置值：mode */
    mode = sgets("land@machine", "mode");
    if (mode == NULL || mode <= tpanic)
    {
        return -1;
    }
    printf("current machine.mode = %s\n", x2string(mode));
    talk_free(mode);

    /* 3) 设置字符串配置值 */
    ok = ssets_string("land@machine", "DemoMachine", "name");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@machine", "mix", "mode");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@machine", "en", "language");
    if (ok == false)
    {
        return -1;
    }

    /* 4) 读回以验证 */
    if (sgets_string(name, sizeof(name), "land@machine", "name") == NULL)
    {
        return -1;
    }
    printf("updated machine.name = %s\n", name);

    return 0;
}
```

注意：
- 使用 `sgets_string()` / `sgets()` 获取配置值。
- 使用 `ssets_string()` / `ssets()` 更新配置值。
- 作为 `talk_t` 返回的引用已分配 JSON 的值必须在下面的 C 示例中使用 `talk_free()` 释放（仅使用 `he` 的用户可以忽略这一点）。

**调用组件方法** — `scall` / `scalls`（`com.h`）。

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `status[]`

```c
talk_t ret = scall("land@machine", "status", NULL);
if (ret > tpanic)
{
    printf("name=%s, mode=%s\n", json_string(ret, "name"), json_string(ret, "mode"));
    talk_free(ret);
}
else print_call_error("status", ret);
```

##### `restart[[delay]]`

```c
talk_t ret = scalls("land@machine", "restart", "10"); /* 延迟 10 秒 */
if (ret != ttrue) print_call_error("restart", ret);
```

##### `restart_block[]`

```c
talk_t ret = scall("land@machine", "restart_block", NULL);
if (ret != ttrue) print_call_error("restart_block", ret);
```

##### `restart_unblock[]`

```c
talk_t ret = scall("land@machine", "restart_unblock", NULL);
if (ret != ttrue) print_call_error("restart_unblock", ret);
```

##### `reboot[[delay]]`

```c
talk_t ret = scalls("land@machine", "reboot", "5"); /* 延迟 5 秒 */
if (ret != ttrue) print_call_error("reboot", ret);
```

##### `reboot_block[]`

```c
talk_t ret = scall("land@machine", "reboot_block", NULL);
if (ret != ttrue) print_call_error("reboot_block", ret);
```

##### `reboot_unblock[]`

```c
talk_t ret = scall("land@machine", "reboot_unblock", NULL);
if (ret != ttrue) print_call_error("reboot_unblock", ret);
```

##### `default[[delay]]`

```c
talk_t ret = scalls("land@machine", "default", "15"); /* 延迟 15 秒 */
if (ret != ttrue) print_call_error("default", ret);
```

##### `default_block[]`

```c
talk_t ret = scall("land@machine", "default_block", NULL);
if (ret != ttrue) print_call_error("default_block", ret);
```

##### `default_unblock[]`

```c
talk_t ret = scall("land@machine", "default_unblock", NULL);
if (ret != ttrue) print_call_error("default_unblock", ret);
```

##### `release[]`

```c
talk_t ret = scall("land@machine", "release", NULL);
if (ret != ttrue) print_call_error("release", ret);
```

##### `factory[]`

```c
talk_t ret = scall("land@machine", "factory", NULL);
if (ret != ttrue) print_call_error("factory", ret);
```

##### `cpuinfo[]`

```c
talk_t ret = scall("land@machine", "cpuinfo", NULL);
if (ret > tpanic)
{
    talk_t cpu = json_json(ret, "cpu");
    printf("cpu idle=%s\n", json_string(cpu, "idle"));
    talk_free(ret);
}
else print_call_error("cpuinfo", ret);
```

##### `psinfo[]`

```c
talk_t ret = scall("land@machine", "psinfo", NULL);
if (ret > tpanic)
{
    talk_t p1 = json_json(ret, "1");
    if (p1 != NULL) printf("pid1 name=%s\n", json_string(p1, "name"));
    talk_free(ret);
}
else print_call_error("psinfo", ret);
```

##### `psnumber[]`

```c
talk_t ret = scall("land@machine", "psnumber", NULL);
if (ret > tpanic)
{
    printf("process count=%s\n", x2string(ret));
    talk_free(ret);
}
else print_call_error("psnumber", ret);
```

##### `meminfo[]`

```c
talk_t ret = scall("land@machine", "meminfo", NULL);
if (ret > tpanic)
{
    printf("mem total=%sKB free=%sKB\n", json_string(ret, "total"), json_string(ret, "free"));
    talk_free(ret);
}
else print_call_error("meminfo", ret);
```

##### `fsinfo[]`

```c
talk_t ret = scall("land@machine", "fsinfo", NULL);
if (ret > tpanic)
{
    talk_t root = json_json(ret, "/");
    if (root != NULL) printf("/ use=%s\n", json_string(root, "use"));
    talk_free(ret);
}
else print_call_error("fsinfo", ret);
```

##### `sginfo[]`

```c
talk_t ret = scall("land@machine", "sginfo", NULL);
if (ret > tpanic)
{
    talk_t internal = json_json(ret, "internal");
    if (internal != NULL) printf("internal path=%s\n", json_string(internal, "path"));
    talk_free(ret);
}
else print_call_error("sginfo", ret);
```

##### `esinfo[]`

```c
talk_t ret = scall("land@machine", "esinfo", NULL);
if (ret > tpanic)
{
    talk_t usb1 = json_json(ret, "usb1");
    if (usb1 != NULL) printf("usb1 available=%s\n", json_string(usb1, "available"));
    talk_free(ret);
}
else print_call_error("esinfo", ret);
```
