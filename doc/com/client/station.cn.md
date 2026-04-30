## client@station — 所有本地客户端管理
管理所有本地客户端  
显示 **谁在局域网上**：哪些设备在线，可用时显示来自 DHCP 的名称，以及您保存的每个 MAC 的设置（昵称、固定 IP 等）。**`list`** 返回此组合视图。客户端出现或离开时的通知请参见下方的 **Joint 事件**。

### UI 路由参数
- `station.html` 依赖 URL hash 参数 `mac`（例如：`#app?page=.../station.html&mac=00:11:22:33:44:55&lang=...`）。
- 该值来自 `list.html` 当前行的 MAC。若 `mac` 缺失，页面会直接返回，无法加载或删除该客户端记录。

### 配置 ( `client@station` )
```json
// Attributes introduction 
{
    "client MAC address":                                // [ MAC address ]
    {
        "ifname":"specify logical ifname",                    // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], default "ifname@lan"; call "network@frame.list[local]" for the list
        "name":"specify hostname",                            // [ string ]
        "bindip":"specify ip address on dhcp assignment",     // [ ip address ] fixed address for this MAC when using DHCP
        "arpbind":"bind ip set the arp table",                // [ "disable", "enable" ] keep a fixed IP↔MAC binding on the LAN when enable and bindip is set
        "lease":"specify lease on dhcp assignment"            // [ number ] the unit is second
    }
    // more client MAC address rule
}
```   

示例，显示所有客户端配置
```shell
client@station
{
    "00:03:7F:22:43:2B":                       # 第一个客户端 MAC 地址
    {
        "ifname":"ifname@lan",                     # 属于 ifname@lan 网络
        "name":"Office-Printer",                   # 自定义主机名：Office-Printer
        "bindip":"192.168.31.100",                 # DHCP 分配时固定 IP 为 192.168.31.100
        "arpbind":"enable",                        # 保持局域网上的 IP-MAC 固定绑定
        "lease":"0"                                # 使用默认租约时间
    },
    "F6:F7:73:82:0A:FC":                       # 第二个客户端 MAC 地址
    {
        "ifname":"ifname@lan",                     # 属于 ifname@lan 网络
        "name":"Xiaomi-Phone",                     # 自定义主机名：Xiaomi-Phone
        "bindip":"192.168.31.222",                 # DHCP 分配时固定 IP 为 192.168.31.222
        "arpbind":"disable"                        # 不进行 IP-MAC 绑定
    }
}
```

示例，为 00:51:45:CB:78:80 绑定 IP 192.168.31.222
```shell
client@station:00:51:45:CB:78:80/bindip=192.168.31.222
ttrue
```

示例，清除 00:51:45:CB:78:89 的绑定 IP
```shell
client@station:00:51:45:CB:78:89/bindip=
ttrue
```

示例，一次更改多个属性（**合并**）
```shell
client@station|{"00:51:45:CB:78:80":{"bindip":"192.168.31.222","name":"Phone1"}}
ttrue
```

### 组件 API
可从 HE / eline / HTTP `/he` **直接调用** 的 API。
+ `add[ mac, name, ]` **添加一个带名称的客户端**
    - mac -------------- [ mac address ]，格式可以是 AA:BB:CC:DD:EE:FF 或 AABBCCDDEEFF（无效格式返回 **tfalse**）
    - name ------------- [ string ]，为该 MAC 保存的可选显示名称
    - 成功返回 ttrue
    - 失败返回 tfalse（无效 MAC 或保存失败）

    示例，添加一个客户端，自定义名称为 "NewPhone"
    ```shell
    client@station.add[ 00:03:7F:22:43:2B, NewPhone ]
    ttrue
    ```   

    示例，添加一个客户端，自定义名称为 "OldPhone"
    ```shell
    client@station.add[ 345212EDFE10, OldPhone ]
    ttrue
    ```   

+ `delete[ mac ]` **删除一个客户端**
    - mac -------------- [ mac address ]，格式可以是 AA:BB:CC:DD:EE:FF 或 AABBCCDDEEFF
    - 成功返回 ttrue
    - 失败返回 tfalse（无效 MAC 格式，或该 MAC 不在已保存的配置中）
    
    示例，删除一个客户端，MAC 为 00:03:7F:22:43:2B
    ```shell
    client@station.delete[ 00:03:7F:22:43:2B ]
    ttrue
    ```   

    示例，删除一个客户端，MAC 为 34:52:12:ED:FE:10
    ```shell
    client@station.delete[ 345212EDFE10 ]
    ttrue
    ```   


+ `list[]` **列出当前所有客户端信息**   
    - 结合 **当前可见的 LAN 客户端**、**DHCP 租约名称/地址**（**`client@dhcps.list`**）以及每个 MAC 的 **已保存设置**。并非每个客户端都包含所有字段（例如，离线设备可能缺少实时在线信息）。
    - 返回以 MAC 为键的 **JSON**。如果服务不可用，调用可能像其他组件 API 一样失败；否则结果是一个 JSON 对象（可能为空）。
    - 典型字段包括 **`ip`**、**`name`**、**`ifname`**、**`netdev`**、**`uptime`**、**`livetime`**（客户端被发现时的人类可读在线时间）。
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "name":"client name",                 // [ string ]
            "ip":"ip address",                    // [ IP address ]
            "ifname":"connected ifname",          // [ "ifname@lan", "ifname@lan2", ... ]
            "netdev":"kernel netdev",             // [ string ], e.g. br-lan
            "uptime": 0,                          // [ number ]
            "livetime":"connected time"           // [ string ], format is day:hour:minute:second (e.g., "14:39:34:1" means 1 day 14 hours 39 minutes 34 seconds), offline when no such property
            // ... more client attribute
        }
        // ... more client
    }
    ```   

    示例，列出当前所有客户端
    ```shell
    client@station.list
    {
        "04:CF:8C:39:91:7A":            # 第一个客户端
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",    # 主机名为 xiaomi-aircondition-ma2_mibt917A
            "ip":"192.168.31.140",                        # IP 为 192.168.31.140
            "ifname":"ifname@lan"                         # 客户端来自 ifname@lan
        },
        "40:31:3C:B5:6D:4C":            # 第二个客户端
        {
            "ip":"192.168.31.61",
            "ifname":"ifname@lan",                        # 客户端来自 ifname@lan
            "name":"minij-washer-v5_mibt6D4C",
            "livetime":"14:39:34:1"                       # 在线时长为 1 天 14 小时 39 分 34 秒
        },
        "14:13:46:C9:97:C7":            # 第三个客户端
        {
            "ip":"192.168.31.9",
            "ifname":"ifname@lan",                        # 客户端来自 ifname@lan
            "livetime":"14:39:26:1"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "ifname":"ifname@lan2",                       # 客户端来自 ifname@lan2
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "ifname":"ifname@lan2",                        # 客户端来自 ifname@lan2
            "name":"Xiaomi-13-Ultra",
            "livetime":"00:00:36:0"
        }
    }
    ```   

+ `ip2mac[ ip ]` **将 IPv4 解析为 MAC**
    - **ip** — IPv4 地址。返回当前在 LAN 上关联的 **MAC**，如果未知或未找到则返回 **NULL**。
    - 只读。要获取完整客户端列表，请使用 **`list`**。

### 生命周期 API
+ `setup[]` **启动 LAN 客户端监控**，*成功返回 ttrue* — **`init` → `general` → `client@station.setup`**（在默认包中）。在 **slave** 构建上不启动监控；否则应用 **`bindip`** / **`arpbind`** 绑定并启动后台监控。

+ `shut[]` **停止 LAN 客户端监控**，*成功返回 ttrue* — 清除固定绑定并停止监控（**未** 列在默认 **`uninit`** 中；按产品添加）。


### 已发布的 Joint 事件
其他组件可以 **订阅**（参见 **`joint_register`**）。当 LAN 客户端 **出现**、**消失** 或 **更换 IP** 时触发。

| 事件 | 描述 |
|-------|------|
| `station/appear` | 新客户端或旧地址报告消失后的 **新 IP**。载荷：**`ip`**、**`mac`**、**`ifname`**、**`netdev`**。 |
| `station/disappear` | 客户端离线或 **IP** 即将更改。载荷：**`ip`**、**`mac`**、**`ifname`**、**`netdev`**。 |

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_client_station(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@station", "status") == NULL)
        return -1;
    return ssets_string("client@station", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@station", "list", NULL); talk_free if JSON */
```
