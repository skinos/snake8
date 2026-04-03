## land@joint — Joint 事件管理

管理设备 Joint 事件任务。
每个 FPK 可以通过其附带的清单注册 **joint** 处理程序；系统在匹配的事件发生时调度它们。

Joint 事件是字符串名称。**发布者**发布一个短字符串或 JSON 载荷；**消费者**是平台为该名称注册的任何处理程序——通常来自 (1) 每个已安装 FPK 清单中的 **joint** 部分，(2) 通过 **`land@fpk`** / **`joint_register`** 的运行时注册，以及 (3) **`land@joint`** 缓存和持久化的 **`list`**（`register` / `add` / `delete`）。

JSON 载荷（例如来自 **`network@frame`**）通常携带接口上下文，如 **`ifname`**、**`netdev`**、**`ifnametype`**、**`ontime`** 等。字符串载荷用于更简单的信号（例如 **`date/modify`** 携带 `"set"` / `"ntp"`）。

### 概念

下表总结了**常见事件名称**、含义、通常的**发布者**以及典型参考镜像中的**示例消费者**。产品 FPK 几乎总是会添加或重新映射行；请在运行的设备上检查 **`land@joint.list`** 以获取实时映射。

| Joint 事件 | 用途 | 通常由谁发布 | 示例消费者（参考） |
|------------|------|-------------|-------------------|
| `machine/status` | 系统/设备状态转换（例如进入重启）。 | **`land@machine`**（以及升级/恢复期间的其他平台代码）。 | 通常在产品上扩展（云端、LED、隧道辅助程序通过 **`land@joint`**）。 |
| `date/modify` | 系统时间被更改（手动设置或 NTP）。 | **`clock@date`**。 | 最小参考集中无。 |
| `auth/modify` | 认证或账户数据变更。 | **`land@auth`**。 | 最小参考集中无。 |
| `network/on` | **本地**逻辑接口（`ifnametype` = local）上的 IPv4 **启用**。 | **`network@frame`**。 | 例如 **`forward@ttl.on`**，某些镜像上的测试/诊断处理程序。 |
| `network/onvpn` | **VPN** 逻辑接口上的 IPv4 **启用**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/onextern` | **外部**（WAN 类型）逻辑接口上的 IPv4 **启用**。 | **`network@frame`**。 | 例如 **`forward@nat.on`**、**`forward@firewall.on`**。 |
| `network/online` | 默认网关/活动**连接**路径**在线**；JSON = 连接状态。 | **`network@frame`**、**`network@connect`**。 | 例如 **`clock@date.online`**、**`agent@heclient.setup`**。 |
| `network/off` | **本地**接口上的 IPv4 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/offvpn` | **VPN** 接口上的 IPv4 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/offextern` | **外部**接口上的 IPv4 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/offline` | 失去默认 IPv4 连接/综合离线信号。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/up` | **本地**接口上的 IPv6 **启用**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/upvpn` | **VPN** 接口上的 IPv6 **启用**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/upextern` | **外部**接口上的 IPv6 **启用**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/upline` | IPv6 默认路径/线路启用。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/down` | **本地**接口上的 IPv6 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/downvpn` | **VPN** 接口上的 IPv6 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/downextern` | **外部**接口上的 IPv6 **关闭**。 | **`network@frame`**。 | 最小参考集中无。 |
| `network/downline` | IPv6 默认路径/线路关闭。 | **`network@frame`**。 | 最小参考集中无。 |
| `station/appear` | 一个站点（LAN 客户端）出现或获得新 IP（JSON 包含客户端字段）。 | **`client@station`**。 | 最小参考集中无。 |
| `station/disappear` | 一个站点离开或 IP 被释放。 | **`client@station`**。 | 最小参考集中无。 |
| `storage/insert` | 可移动/外部存储变为可用（平台特定）。 | 平台/热插拔集成。 | 例如 **`land@syslog.setup`**。 |
| `storage/remove` | 可移动存储被移除或卸载。 | 平台/热插拔集成。 | 例如 **`land@syslog.setup`**。 |

**运行时注册：** 组件也可以通过编程方式订阅（例如 **`agent@gtog`** 订阅 `network/online`）。这些订阅在持久化到 **`land@joint`** 或随 FPK 发布之前可能不会出现在静态清单中。

**大型产品**通常将 `network/onextern`、`network/on`、`network/online`、`machine/status` 等映射到 `forward@nat.on`、`forward@firewall.on`、`agent@heclient.*`、`client@dhcps.reset` 等。请参阅 **`doc/com/land/joint.md`** 获取叙述性列表，以及设备上的 **`land@joint.list`** 获取有效映射。


### 配置 ( `land@joint` )

`land@joint` 的**持久化配置对象**（Joint 任务**列表**和可选的**远程** UDP 通知设置）。

```json
// 属性介绍
{
    "remote":
    {
        "ip":"remote ip address",            // [ string ]，为空则清除远程目标
        "port": "udp port"                   // [ number ]，缺失或非正数时默认为 515
    },
    "list":
    {
        "task name":                             // [ string ]，可自定义名称
        {
            "level":"joint event name",                 // [ string ]，与 `init` 文档中启动级别相同的字段命名思路
            "call":"component API or program"           // [ string ]
        }
        // "...":{ ... }     有多少个 Joint 事件任务就显示多少条属性
    }
}
// 示例
{
    "remote":
    {
        "ip":"192.168.8.222",                          // 远程 IP 为 192.168.8.222
        "port": "2230"                                 // [ number ]，缺失或非正数时默认为 515
    },
    "list":
    {
        "online_talk_to_me":                               // 当 network/online Joint 事件发生时执行 tui@telnet.reset
        {
            "level":"network/online",
            "call":"tui@telnet.reset"
        },
        "offline_talk_to_me":                              // 当 network/offline Joint 事件发生时执行 tui@ssh.shut
        {
            "level":"network/offline",
            "call":"tui@ssh.shut"
        }
    }
}
```  

示例，合并 **remote** 和一个 **list** 任务
```shell
land@joint|{"remote":{"ip":"192.168.8.1","port":"2230"}}
ttrue
```

### 组件 API

+ `register[ joint event name, call ]` **注册一个 Joint 事件任务，重启后丢失**  
    - joint event name ----------- [ string ] 
    - call ----------------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，注册当 network/online Joint 事件发生时执行 wui@admin.reset
    ```shell
    land@joint.register[ network/online, wui@admin.reset ]
    ttrue
    ```
    示例，注册当 network/up Joint 事件发生时执行 tui@ssh.reset
    ```
    land@joint.register[ network/up, tui@ssh.reset ]
    ttrue    
    ```   

+ `unregister[ joint event name, call ]` **删除一个 Joint 事件任务**  
    - joint event name ------- [ string ]
    - call ------------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除当 network/up Joint 事件发生时执行 tui@ssh.reset
    ```shell
    land@joint.unregister[ network/up, tui@ssh.reset ]
    ttrue
    ```

+ `list[ [joint event name] ]` **列出 Joint 事件任务**  
    - joint event name ----------- [ string ]，默认列出系统中所有已注册的 Joint 事件任务
    - 返回描述事件任务的 JSON   
    ```json
    // 方法返回的 JSON 属性介绍
    {
        "joint event name":                 // [ string ]:{},  Joint 事件名称
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     该 Joint 事件有多少个执行就显示多少条属性
        }
        // "...":{ ... }     有多少个 Joint 事件名称就显示多少条属性
    }    
    ```  

    示例，显示所有 Joint 事件任务
    ```shell
    land@joint.list
    {
        "storage/insert":
        {
            "land@syslog.setup":""
        },
        "storage/remove":
        {
            "land@syslog.setup":""
        },
        "network/online":
        {
            "agent@heclient.setup":"",
            "client@dhcps.reset":""
        },
        "machine/status":
        {
            "agent@heclient.update":""
        },
        "network/onextern":
        {
            "arch@ethernet.hwnat":"",
            "forward@rule.on":"",
            "forward@nat.on":"",
            "forward@firewall.on":""
        },
        "network/on":
        {
            "client@dhcps.reset":"",
            "forward@rule.on":"",
            "forward@dnat.on":""
        }
        // ... 更多 Joint 事件
    }
    ```  

    示例，显示 network/online Joint 事件任务
    ```shell
    land@joint.list[ network/online ]
    {
        "agent@heclient.setup":"",
        "client@dhcps.reset":""
    }   
    ```


+ `add[ task name, call, joint event name ]` **添加一个 Joint 事件任务**
    - task name ------------- [ string ]，任务名称，可自定义
    - call ------------------ [ string ]，组件 API 或程序
    - joint event name ------ [ string ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加名为 webreset 的任务，当 network/online Joint 事件发生时执行 wui@admin.reset
    ```shell
    land@joint.add[ webreset, wui@admin.reset, network/online ]
    ttrue
    ```
    示例，添加名为 sshreset 的任务，当 network/up Joint 事件发生时执行 tui@ssh.reset
    ```
    land@joint.add[ sshreset, tui@ssh.reset, network/up ]
    ttrue    
    ```

+ `delete[ task name ]` **删除一个 Joint 事件任务**
    - task name ---- [ string ]，任务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除名为 webreset 的任务
    ```shell
    land@joint.delete[ webreset ]
    ttrue
    ```


+ `call[ joint event name, [parameter] ]` **在指定 Joint 事件发生时执行所有 Joint 事件任务**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - joint event name ----------- [ string ]，要执行的 Joint 事件（例如 "network/online"、"network/offline"）
    - parameter ------------------ [ talk_t ]，传递给任务的可选参数
    - 此方法在 Joint 事件发生时由系统调用
    - 它执行指定 Joint 事件的所有已注册任务，如果配置了远程日志则发送 UDP 通知
    - 不建议手动调用


### 生命周期 API


+ `setup[]` **初始化 Joint 组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统启动期间自动调用的生命周期方法
    - 它从配置中注册所有 Joint 事件任务，并在配置了远程日志时进行设置
    - 不建议手动调用

**说明：** **`land@joint`** 与 **`init`** / **`uninit`** 共用 **`land@init`** 实现，**无 **`shut[]`** 导出。


### Joint 处理程序

此对象**定义**了每个 **Joint 键**运行哪个 `component.method`（参见本文件前面的 **Joint 事件目录**部分）。它本身不是订阅者；其他组件**发布**目录中列出的事件。


### C 代码示例

**读取和更新配置**

```c
#include "skin/skin.h"

static int joint_config_get_and_set(void)
{
    char remote_ip[64];
    talk_t task;
    boole ok;

    /* 1) 读取远程目标设置 */
    if (sgets_string(remote_ip, sizeof(remote_ip), "land@joint", "remote/ip") == NULL)
    {
        return -1;
    }
    printf("joint remote ip=%s\n", remote_ip);

    /* 2) 读取 list 下的一个任务对象 */
    task = sgets("land@joint", "list/online_talk_to_me");
    if (task != NULL && task > tpanic)
    {
        printf("task level=%s call=%s\n", json_string(task, "level"), json_string(task, "call"));
        talk_free(task);
    }

    /* 3) 更新远程 IP/端口 */
    ok = ssets_string("land@joint", "192.168.8.222", "remote/ip");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@joint", "2230", "remote/port");
    if (ok == false)
    {
        return -1;
    }

    /* 4) 更新一个列表条目 */
    ok = ssets_string("land@joint", "network/online", "list/online_talk_to_me/level");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@joint", "tui@telnet.reset", "list/online_talk_to_me/call");
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

注意事项：
- Joint 配置主要在 `remote/*` 和 `list/*` 下。
- 使用 `sgets_string()` / `sgets()` 进行读取，使用 `ssets_string()` 进行更新。

**调用组件方法**

```c
#include "skin/skin.h"

static void print_joint_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `register[ joint event name, call ]`

```c
talk_t ret = scalls("land@joint", "register", "network/online,wui@admin.reset");
if (ret != ttrue) print_joint_call_error("register", ret);
```

##### `unregister[ joint event name, call ]`

```c
talk_t ret = scalls("land@joint", "unregister", "network/online,wui@admin.reset");
if (ret != ttrue) print_joint_call_error("unregister", ret);
```

##### `list[ [joint event name] ]`

```c
talk_t ret = scalls("land@joint", "list", "network/online");
if (ret > tpanic)
{
    printf("network/online event list ready\n");
    talk_free(ret);
}
else print_joint_call_error("list", ret);
```

##### `add[ task name, call, joint event name ]`

```c
talk_t ret = scalls("land@joint", "add", "webreset,wui@admin.reset,network/online");
if (ret != ttrue) print_joint_call_error("add", ret);
```

##### `delete[ task name ]`

```c
talk_t ret = scalls("land@joint", "delete", "webreset");
if (ret != ttrue) print_joint_call_error("delete", ret);
```
