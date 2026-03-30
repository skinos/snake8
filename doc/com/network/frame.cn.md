## network@frame — 网络框架管理

## 概述

**网络框架**组件 (`network@frame`) 是 LAN/WAN/VPN 注册、路由策略和防火墙钩子的中枢。当存在多条**上行链路**时, 它与**多链路调度服务**协同工作, 使用编号的**优先级槽位** (`"1"`...`"10"`) 选择活跃的默认路由。支持的策略包括**冷备份**、**热备份**和**懒热备份** (在当前上行链路故障之前不会自动回切)。

### 架构

框架组件由两个主要部分组成:

1. **frame** - 主网络框架组件, 管理网络基础设施
2. **connect** - 服务进程, 处理多链路连接管理和切换

### 依赖关系 (概念)

- **Land 平台** -- 组件模型、配置和组件间调用, 被每个 Skinos 根使用。
- **网络辅助库** -- 接口日志、路由、防火墙辅助工具和拨号/DHCP 方式启动的共享工具 (被 **network@frame** 和 **ifname@...** 文档共同使用)。

多上行链路调度在已注册到 **network@frame** 的**逻辑接口** (`ifname@wan`, `ifname@lte`, ...) 上运行; 有关每个角色的 JSON 模型, 请参阅 **ifname** 文档。

---

### 配置 ( `network@frame` )
#### 配置属性

```json
// 属性说明
{
    "type": "多链路连接类型",                          // [ "cold", "hot", "hot2", "hot3", "hot4", "hot5", "lazy", "lazy2", "lazy3", "lazy4", "lazy5" ]
                                                                   // "cold" / "hot" / "lazy" 使用优先级槽位 "1".."10" (编号越小优先级越高)
                                                                   // "hotN" / "lazyN" (N=2..5): 仅槽位 "1".."N" 参与调度决策; 更高槽位可能仍存在但不在该策略范围内
    "concom": "多链路连接管理组件",    // [ string ], 可以自定义数据调度组件以实现更个性化的需求

    "1": "外部 ifname 对象",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ] 
    "2": "外部 ifname 对象",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "3": "外部 ifname 对象",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "4": "外部 ifname 对象",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "5": "外部 ifname 对象",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "6": "外部 ifname 对象",          // [ "ifname@wan", ... ]
    "7": "外部 ifname 对象",          // 可选; 与 "1".."6" 相同的约定 (系统最多支持 "10" 个槽位)
    "8": "外部 ifname 对象",
    "9": "外部 ifname 对象",
    "10": "外部 ifname 对象",

    // 延迟切换功能的配置参数, 仅在 dbdc 模式下使用(保留), 该功能可通过低延迟连接控制数据
    "delay_count": "最近统计延迟次数",   // [ number ]
    "delay_divide": "延迟分界线",                // [ number ], 单位为 ms
    "delay_diff": "延迟差值",                 // [ number ], 单位为 ms

    // 多 DNS 时的自定义 DNS
    "custom_dns": "自定义 DNS",                       // [ "disable", "enable", "ifname@lte", "ifname@lte2", ... ]
    "dns": "自定义 DNS1",                             // [ ip address ], 当 "custom_dns" 为 "enable" 时有效
    "dns2": "自定义 DNS2"                             // [ ip address ], 当 "custom_dns" 为 "enable" 时有效
}
```

示例，显示所有配置
```shell
network@frame
{
    "type":"hot",                              # 多链路调度模式：热备份
    "1":"ifname@wan",                          # 优先级槽位 1：WAN（以太网）
    "2":"ifname@lte",                          # 优先级槽位 2：LTE 基带模块
    "3":"ifname@lte2",                         # 优先级槽位 3：第二个 LTE 基带模块
    "4":"ifname@wisp",                         # 优先级槽位 4：WISP（无线中继）
    "custom_dns":"disable"                     # 使用活跃上行链路的 DNS（非自定义）
}
```

### 连接类型

| 类型 | 描述 | 调度器考虑的优先级槽位 |
|------|-------------|-----------------------------------------------|
| `cold` | 冷备份 -- 同一时间仅一条上行链路处于活跃状态; 其他链路被关闭 | `"1"` .. `"10"` |
| `hot` | 热备份 -- 多条上行链路可同时保持在线; 默认路由优先选择编号最小的在线槽位 | `"1"` .. `"10"` |
| `hot2` | 热备份; 调度器仅评估 `"1"` .. `"2"` | 2 |
| `hot3` | 热备份; 调度器仅评估 `"1"` .. `"3"` | 3 |
| `hot4` | 热备份; 调度器仅评估 `"1"` .. `"4"` | 4 |
| `hot5` | 热备份; 调度器仅评估 `"1"` .. `"5"` | 5 |
| `lazy` | 懒热备份 -- 切换到备份链路后, 在该备份链路故障之前不会回切到更高优先级的链路 | `"1"` .. `"10"` |
| `lazy2` | 懒备份; 调度器仅评估 `"1"` .. `"2"` | 2 |
| `lazy3` | 懒备份; 调度器仅评估 `"1"` .. `"3"` | 3 |
| `lazy4` | 懒备份; 调度器仅评估 `"1"` .. `"4"` | 4 |
| `lazy5` | 懒备份; 调度器仅评估 `"1"` .. `"5"` | 5 |

### 配置示例

```json
{
    "type": "hot4",                  // 4 个外部连接的多链路调度
    "1": "ifname@lte",
    "2": "ifname@lte2",
    "3": "ifname@wan",
    "4": "ifname@wisp",
    "delay_count": "10",             // 收集最近 10 次延迟统计并根据延迟调度流量
    "delay_divide": "150", 
    "delay_diff": "100"
}
```

---

## API 参考

### 管理 API

#### `setup[]` - 设置网络基础设施
- 准备网络框架 (路由策略、防火墙钩子、本地和外部逻辑接口的注册)。
- 当前设备模式存在多链路设置时, 在后台启动**多链路调度服务**。

#### `shut[]` - 关闭网络基础设施
- 停止多链路调度器 (如果正在运行), 解除注册, 并适当清除框架管理的防火墙状态。

#### `service[]` - 启动多链路调度器
- 内部使用, 启动执行上行链路选择的调度器可执行文件; 日常管理通过 `network@frame` 和配置进行。

### 查询 API

#### `status[]` - 显示外部连接状态

当多个外部连接共存时, 返回外部连接的状态。

**返回值:**
- `NULL` - 失败
- `terror` - 错误 (例如, 操作模式不正确, 仅在多连接模式下工作)
- 描述网络信息的 JSON 对象

```json
// 方法返回的 talk 属性说明
{
    "ifname object": {           // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
        "status": "是否在线",    // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ], "up" 为在线
        "inuse": "是否在用"        // [ "disable", "enable" ], enable 为正在使用, disable 为未使用
    }
}
```

**状态值:**
- `nodevice` - 找不到对应模块
- `reset` - 重置设备
- `setup` - 建立连接
- `register` - 注册到对端
- `uping` - 正在连接
- `scanning` - 正在扫描对端
- `block` - 等待 keeplive 成功
- `up` - 已就绪, 可连接互联网 (信号/网络/SIM 卡均正常)
- `failed` - Keeplive 失败
- `down` - 接口已关闭

**示例:**
```shell
network@frame.status
{
    "ifname@lte": {
        "status": "up",
        "inuse": "enable"
    },
    "ifname@lte2": {
        "status": "down",
        "inuse": "disable"
    }
}
```

#### `list[]` - 列出所有连接

返回所有已注册连接的列表。

```json
// 方法返回的 talk 属性说明
{
    "ifname object": "对应的 ifdev 对象"    // [ "ifname@wan", ... ]: [ string ]
}
```

**示例:**
```shell
network@frame.list
{
    "ifname@lan": "bridge@lan",
    "ifname@lte": "modem@lte",
    "ifname@lte2": "modem@lte2"
}
```

#### `local[]` - 列出所有本地连接及信息

返回本地 (LAN) 连接的详细信息。

```json
// 方法返回的 talk 属性说明
{
    "ifname object": {
        "status": "当前状态",        // [ "uping", "down", "up" ]
        "mode": "IPV4 地址模式",      // [ "dhcpc" ] 为 DHCP, [ "static" ] 为手动设置
        "netdev": "网络设备名",          // [ string ]
        "ifdev": "接口设备名",            // [ string ], 可选
        "gw": "网关 IP 地址",       // [ ip address ], 可选
        "dns": "DNS IP 地址",          // [ ip address ], 可选
        "dns2": "DNS2 IP 地址",        // [ ip address ], 可选
        "ip": "IP 地址",               // [ ip address ]
        "mask": "网络掩码",           // [ ip address ]
        "ontime": "在线系统运行时间",        // [ string ], 可选, 在线系统运行时间
        "livetime": "在线时间",        // [ string ], 格式为 hour:minute:second:day
        "rx_bytes": "接收字节数",     // [ number ]
        "rx_packets": "接收数据包数", // [ number ]
        "tx_bytes": "发送字节数",  // [ number ]
        "tx_packets": "发送数据包数",// [ number ]
        "mac": "MAC 地址",             // [ mac address ]
        "method": "IPv6 地址模式",    // [ "manual", "automatic", "slaac" ], 可选
        "addr": "IPv6 地址",           // [ ipv6 address ], 可选
        "addr2": "IPv6 地址 2",         // [ ipv6 address ], 可选
        "addr3": "IPv6 地址 3"          // [ ipv6 address ], 可选
    }
}
```

#### `extern[]` - 列出所有外部连接及信息

返回外部 (WAN) 连接的详细信息。

```json
// 方法返回的 talk 属性说明
{
    "ifname object": {
        "status": "当前状态",
        "mode": "IPV4 地址模式",      // [ "dhcpc", "static", "pppoe" ]
        "netdev": "网络设备名",
        "ifdev": "接口设备名",
        "gw": "网关 IP 地址",
        "dns": "DNS IP 地址",
        "dns2": "DNS2 IP 地址",
        "ip": "IP 地址",
        "mask": "网络掩码",
        "delay": "延迟时间",            // [ "failed", "block", number ]
        "ontime": "在线系统运行时间",
        "livetime": "在线时间",
        "rx_bytes": "接收字节数",
        "rx_packets": "接收数据包数",
        "tx_bytes": "发送字节数",
        "tx_packets": "发送数据包数",
        "mac": "MAC 地址",
        "method": "IPv6 地址模式",
        "addr": "IPv6 地址",
        
        // LTE 特有属性 (适用于 ifname@lte, ifname@lte2 等)
        "imei": "IMEI 号码",
        "imsi": "IMSI 号码",
        "iccid": "ICCID 号码",          // [ number, "nosim", "pin", "puk" ]
        "plmn": "MCC 和 MNC",            // [ number, "noreg", "dereg" ]
        "name": "基带模块名称",
        "operator": "运营商名称",
        "nettype": "网络类型",
        "signal": "信号等级",         // [ "0", "1", "2", "3", "4" ]
        "rssi": "信号强度",
        "csq": "CSQ 数值",
        "rsrp": "RSRP 值",
        "rsrq": "RSRQ 值",
        "sinr": "SINR 值",
        "band": "当前频段",
        "ci": "小区标识",
        "lac": "位置区域码",
        "channel": "信道",
        
        // WISP 特有属性 (适用于 ifname@wisp, ifname@wisp2)
        "peer": "对端 SSID",
        "peermac": "对端 BSSID",
        "rate": "连接速率",
        "rssp": "对端信号百分比"
    }
}
```

#### `vpn[]` - 列出所有 VPN 连接及信息

返回 VPN 连接的信息。

```json
{
    "ifname object": {
        "mode": "当前模式",
        "status": "当前状态",
        "ifdev": "对应的 ifdev 对象",
        "netdev": "Linux 网络设备名",
        "ip": "IP 地址",
        "rx_bytes": "接收字节数",
        "rx_packets": "接收数据包数",
        "tx_bytes": "发送字节数",
        "tx_packets": "发送数据包数",
        "mac": "MAC 地址"
    }
}
```

#### `outer[]` - 列出所有外部和 VPN 连接

合并外部连接和 VPN 连接信息。

#### `default[]` - 获取当前默认连接

返回当前默认网关连接信息。

- 不带参数调用: 返回包含连接信息的 JSON
- 带一个参数 (ifname) 调用: 匹配时返回 `ttrue`, 否则返回 `tfalse`

#### `gateway[]` - 获取当前网关连接

返回当前网关连接信息 (与 `default[]` 类似)。

### 接口管理 API

#### `register[ifname,concom,ifdev,type]` - 注册接口

将网络接口注册到框架。

**参数:**
- `ifname` - 接口名称 (例如 "ifname@lte")
- `concom` - 连接组件名称
- `ifdev` - 接口设备名称
- `type` - 接口类型 ("local", "extern", "vpn")

#### `unregister[ifname,type]` - 注销接口

从框架中注销网络接口。

#### `add[ifdev,netdev]` - 添加设备

向框架添加网络设备 (VLAN 或桥接)。

#### `delete[ifdev]` - 删除设备

从框架中移除网络设备。

### 上线/下线 API

#### `online[info]` - IPv4 上线通知

当接口上线时调用 (IPv4)。

**Info JSON 结构:**
```json
{
    "ifname": "接口名称",
    "ifdev": "设备名称",
    "netdev": "网络设备",
    "ip": "IP 地址",
    "mask": "网络掩码",
    "gw": "网关",
    "dns": "DNS 服务器",
    "dns2": "备用 DNS"
}
```

#### `offline[ifname]` - IPv4 下线通知

当接口下线时调用 (IPv4)。

#### `upline[info]` - IPv6 上线通知

当接口上线时调用 (IPv6)。

#### `downline[ifname]` - IPv6 下线通知

当接口下线时调用 (IPv6)。

---

## Connect 服务

Connect 服务是一个独立进程, 管理多链路连接。当配置了多链路模式时, 由框架组件启动。

### 功能

1. **冷备份**
   - 同一时间仅一条上行链路处于活跃状态; 当使用更高优先级的链路时, 较低优先级的链路被关闭。
   - 当默认路由改变时, 可能会清除现有连接跟踪, 以避免会话卡在旧路径上。

2. **热备份**
   - 多条上行链路可同时保持在线; **默认路由**跟踪最佳可用槽位 (在线的最小索引)。
   - 如果首选上行链路故障, 流量自动转移到下一个合格槽位。

3. **懒备份**
   - 类似热备份, 但故障切换后**不会回切**到恢复的更高优先级上行链路, 直到当前上行链路再次故障 -- 减少链路之间的反复切换。

对于 `hot2`...`hot5` 和 `lazy2`...`lazy5`, 仅前 *N* 个编号槽位参与**调度决策**; 额外的槽位可能仍存在于配置中, 但不在该策略的扫描范围内 (参见上表)。

### 信号

Connect 服务响应以下信号:

- `SIGHUP` - 刷新连接
- `SIGTERM` / `SIGINT` - 优雅关闭
- `SIGPIPE` - 忽略 (防止断管崩溃)

### 控制接口

在包含维护 CLI 的系统上, 可以通过以下方式向调度器发送信号:

```shell
# 退出 connect 服务
connect exit

# 刷新连接
connect flush
```

### 状态查询

```shell
# 查询 connect 服务状态
network@frame.status
```

---

## 部署说明

### 路径和存储

安装布局 (库、调度器二进制文件、运行时套接字和注册文件的位置) 由**固件镜像**和项目打包决定。运维人员应使用 **`he`**、Web UI 或产品特定工具 -- 而非硬编码路径。

### 防火墙和策略路由

框架创建出站 NAT、策略路由标记和分流所需的 **iptables/nftables 结构**。设备上可见的确切链名可能因产品线而异。

### 路由表

- 默认表优先级: 100
- 默认表名: "default"

---

#### Joint 事件
当网络接口状态变化时, 以下 joint 事件会被发布 (joint 总线上的 JSON)。其他组件可以在运行时订阅 (joint 注册 / **`land@joint`**)。

| 事件 | 描述 |
|-------|-------------|
| `network/on` | 当本地接口 (LAN) 获得 IPv4 连接后发送。在接口获取 IP 地址并准备好进行本地网络通信后触发。 |
| `network/off` | 当本地接口 (LAN) 断开或失去 IPv4 连接时发送。在接口被禁用或连接丢失时触发。 |
| `network/up` | 当本地接口 (LAN) 获得 IPv6 连接后发送。在接口获取 IPv6 地址后触发。 |
| `network/down` | 当本地接口 (LAN) 断开或失去 IPv6 连接时发送。 |
| `network/onextern` | 当外部接口 (WAN/LTE/WiFi ISP) 获得 IPv4 连接后发送。在成功建立到互联网服务提供商的连接后触发。此事件在外部连接的 `network/online` 之前发出。 |
| `network/offextern` | 当外部接口 (WAN/LTE/WiFi ISP) 断开或失去 IPv4 连接时发送。在 ISP 连接丢失时触发。 |
| `network/upextern` | 当外部接口 (WAN/LTE/WiFi ISP) 获得 IPv6 连接后发送。 |
| `network/downextern` | 当外部接口 (WAN/LTE/WiFi ISP) 断开或失去 IPv6 连接时发送。 |
| `network/onvpn` | 当 VPN 接口获得 IPv4 连接后发送。在 VPN 隧道成功建立后触发。 |
| `network/offvpn` | 当 VPN 接口断开或失去 IPv4 连接时发送。在 VPN 隧道关闭或中断时触发。 |
| `network/upvpn` | 当 VPN 接口获得 IPv6 连接后发送。 |
| `network/downvpn` | 当 VPN 接口断开或失去 IPv6 连接时发送。 |
| `network/online` | 当系统建立到互联网的默认路由时发送 (IPv4)。在外部接口上线且路由表更新后触发。这表示设备具有完整的互联网访问能力。 |
| `network/offline` | 当系统失去到互联网的默认路由时发送 (IPv4)。在所有外部接口都断开或主连接故障时触发。 |
| `network/upline` | 当系统通过外部接口或 VPN 建立 IPv6 互联网连接时发送。 |
| `network/downline` | 当系统失去 IPv6 互联网连接时发送。 |

---

## 示例

### 基本多 WAN 设置

```shell
# 配置 4 个外部连接为热备份模式
config network@frame hot4
set type=hot4
set 1=ifname@wan
set 2=ifname@lte
set 3=ifname@wisp
set 4=ifname@lte2
commit
```

### 自定义 DNS 配置

```shell
# 为多 WAN 配置自定义 DNS
config network@frame hot4
set type=hot4
set custom_dns=enable
set dns=8.8.8.8
set dns2=8.8.4.4
commit
```

### 查询连接状态

```shell
# 获取所有外部连接状态
network@frame.extern

# 获取当前默认网关
network@frame.default

# 检查特定接口是否为默认
network@frame.default[ifname@lte]
```

### 手动连接控制

```shell
# 刷新连接 (触发重新评估)
connect flush

# 停止 connect 服务
connect exit
```

---

## 注意事项

1. **多链路调度器**仅在当前设备**网络模式**的**合并网络配置文件**包含描述多上行链路行为的 **`connect`** 部分时才启动 (例如 `type` 和编号槽位)。如果该部分不存在, 则仅运行每个接口的启动, 不需要调度器。
2. 最多存在 **10** 个优先级槽位; 配置键 **`"1"` ... `"10"`** 按顺序引用它们 (索引越小 = 优先级越高)。
3. 哪些逻辑接口显示为**本地**、**外部**或 **VPN** 由该模式的**网络配置文件**驱动, 加上多 WAN 产品中用于禁用链路处理的任何动态注册。
4. DNS 在默认上行链路改变时跟随变化, **除非** `custom_dns` 为 `enable` 或应用了按接口的 DNS 覆盖。
5. 在**冷**备份中, 默认路由变更时清除 IPv4 连接跟踪, 以避免绑定到旧上行链路的陈旧会话。

---

### 生命周期 API
+ `setup[]` / `shut[]` -- 启动或关闭框架服务和多链路 **`connect`** 集成。**`network@hosts.setup`** 在 **`init` -> `land`** 下调度; **`network@frame`** 本身通常在 **`connect`** 可执行文件 / 平台序列中, 当配置文件包含 **`connect`** 部分时启动。


### 已发布的 Joint 事件
**IPv4 / IPv6 上行链路和调度通知** 作为 JSON 在 joint 总线上发出, 包括 (非详尽列表):

| 事件 (示例) | 触发时机 |
|------------------|------|
| `network/on`, `network/off` | 逻辑外部链路上线/下线 |
| `network/onextern`, `network/offextern` | 外部 ifname 范围 |
| `network/onvpn`, `network/offvpn` | VPN 范围 |
| `network/online`, `network/offline` | 连接后 / DHCP 方式在线 |
| `network/upline`, `network/downline`, `network/upextern`, ... | IPv6 / 双栈类似事件 |

负载结构遵循 **`_online` / `_offline` / `_upline` / `_downline`** 中构建的 `talk_t` JSON (通常包括 **`ifname`** 和状态字段)。

### C 代码示例
```c
#include "skin/skin.h"

static void example_frame_list(void)
{
    talk_t ret = scall("network@frame", "list", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
