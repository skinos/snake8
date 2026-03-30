## agent@net — 网络客户端 — 一个 GTOG VPN 实例
每个 **`agent@net`**、**`agent@net2`**、... 是一个网状 VPN 成员，通常由 **`agent@gtog`** 管理。配置涵盖服务器可达性、VPN 寻址、保活、可选 DNS/路由，以及通过 **`endpoint` / `branch` / `leaf`** 进行的对等节点更新。

### 配置 ( `agent@net` )
**agent@net** 是第一个 GTOG 网络
**agent@net2** 是第二个 GTOG 网络

```json
{
    // 服务器连接
    "server":"GTOG 网络服务器地址",                    // [ string ], 域名或 IP 地址
                                                                  // 未设置时，使用 heclient 的服务器
    "port":"GTOG 网络服务器端口",                         // [ number ], 默认 20002
    "key":"与网状协调者通信的共享密钥", // [ string ], 省略时使用产品默认值

    // 绑定外部网络
    "extern":"绑定外部 ifname 以连接服务器",       // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], 可选
                                                                  // 未设置时，使用默认网关连接服务器

    // 网络标识
    "netid":"网络标识字符串",                         // [ string ], 由服务器分配的唯一网络标识符
    "network":"网络地址（CIDR 格式）",                 // [ string ], 例如 "10.0.0.0/24"，由服务器或配置分配
    "lport":"本地 WireGuard 监听端口",                     // [ number ], 由 gtog 根据 port_start 自动分配

    // 保活
    "keepintval":"保活间隔（秒）",               // [ number ], 默认 15
    "keepfailed":"保活最大重试次数",                   // [ number ], 默认 4
    "keeptimeout":"保活超时时间（秒）",               // [ number ], 默认 15

    // 接口
    "mtu":"WireGuard 接口 MTU",                           // [ number ], 可选

    // DNS（仅在 custom_dns 为 "enable" 时生效）
    "custom_dns":"使用自定义 DNS 设置",                    // [ "disable", "enable" ]
    "dns":"主 DNS 服务器",                                // [ ip address ], 可选
    "dns2":"备用 DNS 服务器",                             // [ ip address ], 可选
    "domain":"DNS 搜索域",                              // [ string ], 可选

    // 路由
    "metric":"路由度量值",                             // [ number ], 可选
    "defaultroute":"设置为默认路由",                     // [ "disable", "enable" ]
    "route_table":                                             // 自定义路由表，仅在 defaultroute 不为 "enable" 时使用
    {
        "route name":
        {
            "target":"目标网络或主机",            // [ ip address or network ]
            "mask":"子网掩码"                               // [ mask ], 默认 "255.255.255.255"（主机路由）
        }
    }
}
```

示例，显示所有配置
```shell
agent@net
{
    "port":"20002",                            # 服务器端口
    "netid":"office-vpn",                      # 网络标识符
    "network":"10.0.1.0/24",                   # VPN 网络地址
    "keepintval":"10",                         # 保活间隔 10 秒
    "keepfailed":"3",                          # 最多 3 次保活失败
    "keeptimeout":"35"                         # 保活超时 35 秒
}
```

示例，配置自定义 DNS 和默认路由
```shell
agent@net={"port":"20002","netid":"office-vpn","network":"10.0.1.0/24","custom_dns":"enable","dns":"8.8.8.8","dns2":"8.8.4.4","defaultroute":"enable","mtu":"1420"}
ttrue
```

示例，配置自定义路由表
```shell
agent@net={"port":"20002","netid":"office-vpn","network":"10.0.1.0/24","route_table":{"r1":{"target":"192.168.10.0","mask":"255.255.255.0"},"r2":{"target":"172.16.0.0","mask":"255.255.0.0"}}}
ttrue
```

示例，设置外部接口
```shell
agent@net:extern=ifname@lte
ttrue
```


### 组件 API
**可直接调用的** API，来自 HE / eline / HTTP `/he`。
**agent@net** 是第一个 GTOG 网络
**agent@net2** 是第二个 GTOG 网络

+ `setup[]` **设置此网络客户端，启动服务**
    在网络对象（agent@net）上调用时，为此网络启动后台服务进程
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置第一个网络
    ```shell
    agent@net.setup
    ttrue
    ```

+ `shut[]` **关闭此网络客户端**
    调用网络离线，停止后台服务进程，并关闭 WireGuard 接口
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭第一个网络
    ```shell
    agent@net.shut
    ttrue
    ```

+ `status[]` **获取网络客户端当前状态和详情**
    - 失败返回 NULL
    - 成功返回描述详细状态信息的 JSON
    ```json
    // API 返回的属性介绍
    {
        "status":"当前状态",             // [ "uping", "down", "up", "failed", "block" ]
                                                  // "uping" 启动中; "down" 已停止; "up" 健康; "failed"/"block" 保活问题
        "delay":"保活往返延迟",   // [ number ], 单位为毫秒，仅在 status 为 "up" 时存在
        "ip":"本地 VPN IP 地址",           // [ ip address ], 仅在接口启动时存在
        "mask":"VPN 子网掩码",                  // [ mask ], 仅在接口启动时存在
        "dstip":"目标 IP 地址",      // [ ip address ], 仅在点对点时存在
        "livetime":"连接在线时长",      // [ string ], 可读格式，例如 "1 hours 30 minutes"
        "rx_bytes":"总接收字节数",     // [ number ]
        "rx_packets":"总接收数据包数", // [ number ]
        "tx_bytes":"总发送字节数",         // [ number ]
        "tx_packets":"总发送数据包数",     // [ number ]
        "server":"GTOG 服务器地址",        // [ string ]
        "pref":"自身优先级值",        // [ number ]
        "mode":"当前节点模式",            // [ number ] 1=尚无协调者, 2=叶节点, 3=分支节点, 4=本设备为协调者
        "mip":"主节点 VPN IP 地址",         // [ ip address ], 当前主节点的 VPN IP
        "mmacid":"主节点 MAC 标识",        // [ string ], 当前主节点的 macid
        "mpref":"主节点优先级值",     // [ number ], 当前主节点的优先级
        "tid":"路由表 ID"                 // [ number ], 策略路由表 ID
    }
    ```

    示例，获取作为叶节点连接时的状态
    ```shell
    agent@net.status
    {
        "status":"up",                            # 已连接且保活正常
        "delay":"45",                             # 到主节点的往返延迟为 45ms
        "ip":"10.0.1.3",                          # 本地 VPN IP
        "mask":"255.255.255.0",                   # VPN 子网掩码
        "livetime":"2 hours 15 minutes",          # 连接在线时长
        "rx_bytes":"1048576",                     # 已接收 1MB
        "rx_packets":"1024",
        "tx_bytes":"524288",                      # 已发送 512KB
        "tx_packets":"512",
        "server":"cls.ashyelf.com",               # GTOG 服务器
        "pref":"50",                              # 自身优先级
        "mode":"2",                               # 叶节点模式
        "mip":"10.0.1.1",                         # 主节点 VPN IP
        "mmacid":"001122334455",                  # 主节点 macid
        "mpref":"100"                             # 主节点优先级
    }
    ```

    示例，获取正在连接时的状态
    ```shell
    agent@net.status
    {
        "status":"uping"                          # 服务正在运行，正在连接
    }
    ```

    示例，获取服务停止时的状态
    ```shell
    agent@net.status
    {
        "status":"down"                           # 服务未运行
    }
    ```

    示例，获取作为主节点时的状态
    ```shell
    agent@net.status
    {
        "status":"up",
        "delay":"30",
        "ip":"10.0.1.1",
        "mask":"255.255.255.0",
        "livetime":"5 hours 10 minutes",
        "rx_bytes":"10485760",
        "rx_packets":"10240",
        "tx_bytes":"5242880",
        "tx_packets":"5120",
        "server":"cls.ashyelf.com",
        "pref":"100",
        "mode":"4"                                # 主节点模式，无主节点信息（自身即为主节点）
    }
    ```

+ `list[]` **列出此网络的所有端点**
    返回此网络端点文件中存储的完整端点列表
    - 失败返回 NULL
    - 成功返回包含所有端点信息的 JSON
    ```json
    // API 返回的 JSON 属性介绍
    {
        "endpoint mac identify":
        {
            "point":"端点 VPN IP 地址",         // [ ip address ]
            "extend":"端点本地网络",         // [ network address ]
            "pubkey":"WireGuard 公钥",           // [ string ]
            "nattype":"NAT 类型",                      // [ "1", "2" ], 影响中继与仅叶节点角色
            "pref":"协调者优先级",           // [ number ], 越高 = 越有可能成为候选者
            "ip":"公网 IP 地址",         // [ ip address ]
            "port":"公网端口",             // [ number ]
            "macid":"设备 MAC 标识"              // [ string ]
        }
        // ... 更多端点
    }
    ```

    示例，列出所有端点
    ```shell
    agent@net.list
    {
        "001122334455":
        {
            "point":"10.0.1.1",
            "extend":"192.168.1.0/24",
            "pubkey":"abc123def456...",
            "nattype":"1",
            "pref":"100",
            "ip":"1.2.3.4",
            "port":"10004",
            "macid":"001122334455"
        },
        "aabbccddeeff":
        {
            "point":"10.0.1.2",
            "extend":"192.168.2.0/24",
            "pubkey":"ghi789jkl012...",
            "nattype":"2",
            "pref":"50",
            "ip":"5.6.7.8",
            "port":"10004",
            "macid":"aabbccddeeff"
        }
    }
    ```

+ `endpoint[ {endpoint list} ]` **更新此网络的完整端点列表**
    替换整个端点列表，重新配置 WireGuard 对等节点，确定节点角色（master/branch/leaf），并更新主节点信息
    - {endpoint list} ------ json
    ```json
    // {endpoint list} 属性介绍
    {
        "endpoint mac identify":
        {
            "ip":"设备公网 IP 地址",              // [ ip address ]
            "port":"设备公网端口",                  // [ number ]
            "pubkey":"设备 WireGuard 公钥",       // [ string ]
            "nattype":"设备 NAT 类型",                  // [ "1", "2" ]
            "pref":"协调者优先级",              // [ number ]

            "point":"端点 VPN IP 地址",            // [ ip address ]
            "extend":"端点本地网络"             // [ network address ]
        }
        // ... 更多端点
    }
    ```

    列表更改后，每个设备根据 **`nattype`**、**`pref`** 和对等节点可见性选择 **master / branch / leaf**（与 **`agent@gtog.endpoint`** 相同的规则）。

    示例，更新端点列表
    ```shell
    agent@net.endpoint[{"001122334455":{"ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"},"aabbccddeeff":{"ip":"5.6.7.8","port":"10004","pubkey":"def...","nattype":"2","pref":"50","point":"10.0.1.2"}}]
    ttrue
    ```

+ `branch[ {branch information} ]` **添加或更新一个 branch（中继）节点**
    添加或刷新一个具有中继能力的对等节点；当拓扑允许时，协调者角色可能转移到更高 **`pref`** 的节点。

    - {branch information} ------ json
    ```json
    // {branch information} 属性介绍
    {
        "macid":"设备 MAC 标识",                    // [ string ]
        "ip":"设备公网 IP 地址",                  // [ ip address ]
        "port":"设备公网端口",                      // [ number ]
        "pubkey":"设备 WireGuard 公钥",           // [ string ]
        "nattype":"设备 NAT 类型",                      // [ "1", "2" ]
        "pref":"设备主节点优先级",                // [ number ]

        "point":"端点 VPN IP 地址",                // [ ip address ]
        "extend":"端点本地网络"                 // [ network address ]
    }
    ```

    示例，添加一个 branch 节点
    ```shell
    agent@net.branch[{"macid":"001122334455","ip":"1.2.3.4","port":"10004","pubkey":"abc...","nattype":"1","pref":"100","point":"10.0.1.1","extend":"192.168.1.0/24"}]
    ttrue
    ```

+ `leaf[ {leaf information} ]` **添加或更新一个 leaf 节点**
    添加或刷新一个非中继对等节点；路由遵循当前协调者路径。

    - {leaf information} ------ json
    ```json
    // {leaf information} 属性介绍
    {
        "macid":"设备 MAC 标识",                    // [ string ]
        "ip":"设备公网 IP 地址",                  // [ ip address ]
        "port":"设备公网端口",                      // [ number ]
        "pubkey":"设备 WireGuard 公钥",           // [ string ]

        "point":"端点 VPN IP 地址",                // [ ip address ]
        "extend":"端点本地网络"                 // [ network address ]
    }
    ```

    示例，添加一个 leaf 节点
    ```shell
    agent@net.leaf[{"macid":"aabbccddeeff","ip":"5.6.7.8","port":"10004","pubkey":"def...","point":"10.0.1.2","extend":"192.168.2.0/24"}]
    ttrue
    ```

+ `online[ {network information} ]` **内部 -- 链路已上线**
    从配置和提供的快照中应用 DNS、策略路由、伪装和 MTU。
    - {network information} ------ json
    ```json
    {
        "ifname":"网络对象名",        // [ string ], 例如 "agent@net"
        "netdev":"WireGuard 设备名",      // [ string ], 例如 "wg0"
        "gw":"网关 IP 地址",             // [ ip address ], 主节点的 VPN IP
        "dns":"主 DNS 服务器",            // [ ip address ], 可选，仅在 custom_dns 为 "enable" 时
        "dns2":"备用 DNS 服务器",         // [ ip address ], 可选
        "domain":"DNS 搜索域",          // [ string ], 可选
        "masq":"启用 NAT 伪装"         // [ "enable" ], 当未配置扩展网络时设置
    }
    ```
    - 成功返回 ttrue

+ `offline[]` **内部 -- 链路已下线**
    还原 **`online[]`** 的副作用（DNS、NAT/MSS 调整等）。
    - 成功返回 ttrue

+ `service[]` **内部（不通过 HE 调用）**
    此 **`agent@net*`** 对象的后台工作进程：启动 WireGuard，向网状协调者注册，同步可调参数（**`network`**、保活字段等），为当前角色（**master / branch / leaf**）维护可达性，并在隧道状态变化时驱动 **`online[]` / `offline[]`**。**硬配置/禁用**错误会停止且不自动重启；**链路/套接字**问题和**外部接口未就绪**会重试。

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@net`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_net(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@net", "status") == NULL)
        return -1;
    return ssets_string("agent@net", "enable", "status") ? 0 : -1;
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

/* 示例: scall("agent@net", "list", NULL); 如果是 JSON 则调用 talk_free */
```
