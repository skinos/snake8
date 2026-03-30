## agent@gtog — 网关对网关 (GTOG) — 网状 VPN
管理多个基于 **WireGuard** 的网状 VPN（**`agent@net`**、**`agent@net2`**、...）：注册网络、推送端点列表，以及添加 **branch**（具有中继能力的）或 **leaf** 对等节点。拓扑和可达性取决于每个设备的 NAT 特征和配置的优先级。

### 配置 ( `agent@gtog` )
```json
{
    "net_max":"GTOG 网络的最大数量",           // [ number ], 产品默认值
    "port_start":"网络的起始本地 UDP 端口"    // [ number ], 产品默认值；后续网络使用递增的端口
}
```

示例，显示所有配置
```shell
agent@gtog
{
    "net_max":"10",                            # 最多支持 10 个网络
    "port_start":"10004"                       # 第一个网络使用端口 10004
}
```

示例，设置最大网络数为 5
```shell
agent@gtog:net_max=5
ttrue
```


#### 网络配置 ( agent@net )
每个注册的网络都有自己的配置对象（agent@net 是第一个，agent@net2 是第二个，依此类推）

> **完整字段列表：** `server`、`extern`、`key`、`lport`、DNS、路由、`mtu` 等字段记录在 **`net.cn.md`**（网络客户端）中。下表仅为简要概述；请使用 **`net.cn.md`** 作为每网络选项的权威参考。

```json
{
    "port":"网络服务器端口",                              // [ number ]
    "netid":"网络标识字符串",                         // [ string ], 唯一的网络标识符
    "network":"网络地址（CIDR 格式）",                 // [ string ], 例如 "10.0.0.0/24"
    "keepintval":"到主节点/服务器的保活间隔",         // [ number ], 单位为秒
    "keepfailed":"保活最大失败次数",                  // [ number ]
    "keeptimeout":"到主节点/服务器的保活超时"          // [ number ], 单位为秒
}
```

示例，显示网络配置
```shell
agent@net
{
    "port":"10000",
    "netid":"office-vpn",
    "network":"10.0.1.0/24",
    "keepintval":"10",
    "keepfailed":"3",
    "keeptimeout":"35"
}
```


### 组件 API
+ `setup[]` **设置所有 GTOG 网络基础设施**
    读取 **`agent@gtog`** 限制，将每个已注册的 **`agent@net*`** 接入 **`network@frame`**，并挂钩 **init** / **`network/online`**，以便实例可以按顺序启动。
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置 GTOG 基础设施
    ```shell
    agent@gtog.setup
    ttrue
    ```

+ `shut[]` **关闭所有 GTOG 网络客户端**
    停止每个 **`agent@net*`** 服务并移除 **`setup[]`** 安装的 **`network@frame`** / 事件钩子。
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭所有 GTOG 网络
    ```shell
    agent@gtog.shut
    ttrue
    ```

+ `register[ netid, {network configure} ]` **注册一个新的 GTOG 网络**
    注册一个新网络，将其分配到下一个可用的网络槽位，保存配置并向系统注册
    - netid -------------------- [ string ], 网络标识符
    - {network configure} ------ json
    ```json
    // {network configure} 的 JSON 属性介绍
    {
        "port":"网络服务器端口",                         // [ number ]
        "netid":"网络标识",                           // [ string ]
        "network":"网络地址",                          // [ string ], 例如 "10.0.1.0/24"
        "keepintval":"到主节点/服务器的保活间隔",    // [ number ]
        "keepfailed":"到主节点/服务器的保活失败次数",// [ number ]
        "keeptimeout":"到主节点/服务器的保活超时"     // [ number ]
    }
    ```
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，注册一个新网络
    ```shell
    agent@gtog.register[office-vpn,{"port":"10000","netid":"office-vpn","network":"10.0.1.0/24","keepintval":"10","keepfailed":"3","keeptimeout":"35"}]
    ttrue
    ```

+ `unregister[ netid ]` **注销一个 GTOG 网络**
    查找并移除指定 netid 的网络，停止其服务，关闭接口并清理配置
    - netid ---- [ string ], 要移除的网络标识符
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，注销一个网络
    ```shell
    agent@gtog.unregister[office-vpn]
    ttrue
    ```

+ `list[]` **列出所有已注册的 GTOG 网络及其状态**
    - 失败返回 NULL
    - 成功返回包含所有网络信息的 JSON
    ```json
    // API 返回的属性介绍
    {
        "network object name":
        {
            "netid":"网络标识符",              // [ string ]
            "netdev":"网络设备名",            // [ string ], WireGuard 接口名
            "lport":"本地监听端口",               // [ number ]
            "master":"当前主节点端点",        // [ string ], 当前主节点的 "ip:port"
            "pref":"当前主节点优先级值"   // [ number ]
        }
        // ... 更多网络
    }
    ```

    示例，列出所有网络
    ```shell
    agent@gtog.list
    {
        "agent@net":
        {
            "netid":"office-vpn",
            "netdev":"wg0",
            "lport":"10004",
            "master":"1.2.3.4:10000",
            "pref":"100"
        },
        "agent@net2":
        {
            "netid":"home-vpn",
            "netdev":"wg1",
            "lport":"10005",
            "master":"5.6.7.8:10000",
            "pref":"50"
        }
    }
    ```

+ `endpoint[ netid, {endpoint list} ]` **更新网络的完整端点列表**
    替换指定网络的整个端点列表。本地设备使用此列表来了解网络中的所有对等节点、选举主节点并建立连接
    - netid ---------------- [ string ], 网络标识符
    - {endpoint list} ------ json
    ```json
    // {endpoint list} 属性介绍
    {
        "endpoint mac identify":
        {
            "ip":"设备公网 IP 地址",              // [ ip address ]
            "port":"设备公网端口",                  // [ number ]
            "pubkey":"设备 WireGuard 公钥",       // [ string ]
            "nattype":"设备 NAT 类型",                  // [ "1", "2" ], 影响节点是否可作为中继或仅为叶节点
            "pref":"主节点优先级值",             // [ number ], 越高 = 越有可能成为网状网络的协调者

            "point":"端点 VPN IP 地址",            // [ ip address ], VPN 网络中分配的 IP
            "extend":"端点本地网络"             // [ string ], 通过此端点路由的本地网络
        }
        // ... 更多端点
    }
    ```

    示例，更新端点列表
    ```shell
    agent@gtog.endpoint[office-vpn,{"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc123...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def456...","nattype":"2","pref":"50","point":"10.0.1.2","extend":"192.168.2.0/24"}}]
    ttrue
    ```

+ `branch[ netid, {branch information} ]` **向网络添加一个 branch（中继）节点**
    注册一个具有中继能力的对等节点，当 **`nattype`** 允许 branch 拓扑时，设备应使用该节点。
    - netid -------------------- [ string ], 网络标识符
    - {branch information} ----- json
    ```json
    // {branch information} 属性介绍
    {
        "macid":"设备 MAC 标识",                    // [ string ]
        "ip":"设备公网 IP 地址",                  // [ ip address ]
        "port":"设备公网端口",                      // [ number ]
        "pubkey":"设备 WireGuard 公钥",           // [ string ]
        "nattype":"设备 NAT 类型",                      // [ "1", "2" ]
        "pref":"主节点优先级值",                 // [ number ]
        "point":"端点 VPN IP 地址",                // [ ip address ]
        "extend":"端点本地网络"                 // [ string ]
    }
    ```

    示例，添加一个 branch 节点
    ```shell
    agent@gtog.branch[office-vpn,{"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc123...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"}]
    ttrue
    ```

+ `leaf[ netid, {leaf information} ]` **向网络添加一个 leaf 节点**
    注册一个作为 **leaf** 参与的对等节点（该端点没有中继角色）。
    - netid ------------------ [ string ], 网络标识符
    - {leaf information} ----- json
    ```json
    // {leaf information} 属性介绍
    {
        "macid":"设备 MAC 标识",                    // [ string ]
        "ip":"设备公网 IP 地址",                  // [ ip address ]
        "port":"设备公网端口",                      // [ number ]
        "pubkey":"设备 WireGuard 公钥",           // [ string ]
        "point":"端点 VPN IP 地址",                // [ ip address ]
        "extend":"端点本地网络"                 // [ string ]
    }
    ```

    示例，添加一个 leaf 节点
    ```shell
    agent@gtog.leaf[office-vpn,{"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def456...","point":"10.0.1.2","extend":"192.168.2.0/24"}]
    ttrue
    ```

+ `state[]` **获取 VPN 实例的运行状态**（别名：`status[]`）
    仅在每网络对象（`agent@net`、`agent@net2`、...）上有意义。
    在主 **`agent@gtog`** 对象上调用时返回 NULL。
    - 失败返回 NULL
    - 成功返回描述实例状态的 JSON：
    ```json
    {
        "status":"当前状态",      // "up", "down", "uping", "failed", "block"
        "ip":"隧道本地 IP",
        "mask":"隧道掩码",
        "dstip":"隧道对端 IP",
        "netdev":"WireGuard 接口名",
        "delay":"主节点保活延迟（毫秒）",
        "livetime":"可读的在线时长",
        "rx_bytes":"...", "rx_packets":"...",
        "tx_bytes":"...", "tx_packets":"...",
        "server":"主节点服务器地址",
        "pref":"优先级值",
        "mode":"当前模式",
        "mip":"主节点 IP",
        "mmacid":"主节点 MAC ID",
        "mpref":"主节点优先级",
        "tid":"配置事务 ID"
    }
    ```

+ `online[]` **内部**
    当 **`network/online`** 事件触发时调用，以便实例可以在继续启动之前刷新可达性上下文（例如网关）。

+ `offline[]` **内部**
    当 VPN 链路断开时调用。清理 DNS 解析器条目，移除 iptables MASQUERADE 规则，并清除接口的 TCP MSS 钳位。

+ `service[]` **内部（不通过 HE 调用）**
    每个 **`agent@net*`** 的工作进程：应用保存的 VPN 设置，管理 WireGuard 接口和对等节点集合，根据当前角色（**master / branch / leaf**）与网状网络协调者交换保活信息，并在 **`endpoint` / `branch` / `leaf`** API 更新拓扑时调整对等节点。**配置错误**通常会停止且不自动重启；**暂时性链路丢失**会重试。

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@gtog`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_gtog(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@gtog", "status") == NULL)
        return -1;
    return ssets_string("agent@gtog", "enable", "status") ? 0 : -1;
}
```

**调用组件方法**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* 示例: scall("agent@gtog", "list", NULL); 如果是 JSON 则调用 talk_free */
```
