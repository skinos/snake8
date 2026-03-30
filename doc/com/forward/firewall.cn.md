## forward@firewall — 系统防火墙管理
管理防火墙, 限制来自外部 **ifname** (互联网) 的访问

### 配置 ( `forward@firewall` )
```json
// 属性介绍 
{
    "ifname@…":                  // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], 此外部 ifname 的防火墙
    {
        "status":"禁用或启用防火墙",          // [ "enable", "disable" ]
        "default":"默认访问动作",              // [ "drop", "accept" ]

        "icmp_access":"ICMP 协议访问",        // [ "disable", "enable" ]
        "espah_access":"ESP/AH 协议访问",     // [ "disable", "enable" ]
        "telnet_access":"TELNET 服务器访问",      // [ "disable", "enable" ]
        "ssh_access":"SSH 服务器访问",            // [ "disable", "enable" ]
        "wui_access":"WEB 服务器访问",            // [ "disable", "enable" ]

        "nat_through":"forward@nat 中 NAT 规则设置自动透传",    // [ "disable", "enable" ]
        "icmp_through":"ICMP 协议透传",                            // [ "disable", "enable" ]
        "espah_through":"ESP/AH 协议透传",                         // [ "disable", "enable" ]

        "rule":                              // 防火墙规则设置
        {
            "rule name":                                   // [ string ], 用户可自定义规则名称
            {
                "action":"丢弃、接受或返回",                     // [ "drop", "accept", "return" ], "drop" 表示禁止, "accept" 表示通过, "return" 表示不与后续规则匹配
                "src":"源地址",                                  // [ string ]:
                                                                          // 单个 IP: 192.168.8.222
                                                                          // 多个 IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                          // IP 范围: 192.168.8.2-192.168.8.4
                                                                          // 单个 MAC: 00:23:43:13:34:40
                                                                          // 留空表示所有 IP 地址
                "srcport":"源端口",                                 // [ number ]: 当 "proto" 为 "tcp" 或 "udp" 时有效
                                                                          // 单个端口: 8080
                                                                          // 多个端口: 80,8000,8080
                                                                          // 端口范围: 80-800
                                                                          // 留空表示所有端口

                "protocol":"协议类型",                              // [ "tcp", "udp", "all", ... ]
                "dest":"目标地址",                            // [ string ]:
                                                                          // 单个 IP: 202.96.11.32
                                                                          // 多个 IP: 2.3.1.2,4.34.2.1,72.32,192.1
                                                                          // IP 范围: 202.96.132.11-202.96.132.20
                                                                          // 留空表示所有 IP 地址

                "destport":"目标端口"                           // [ number ]: 当 "proto" 为 "tcp" 或 "udp" 时有效
                                                                          // 单个端口: 8080
                                                                          // 多个端口: 80,8000,8080
                                                                          // 端口范围: 80-800
                                                                          // 留空表示所有端口
            }
            // ... 更多规则
        }
    }
    // ... 更多 ifname
}
```  

示例, 显示当前所有防火墙设置
```shell
forward@firewall
{
    "ifname@lte":                 // 第一个 LTE 网络防火墙设置
    {
        "status":"enable",             // 启用防火墙
        "default":"drop",              // 默认动作为丢弃
        "rule":
        {
            "pcweb":                        // 名为 "pcweb" 的规则, 接受目标 192.168.8.222 和目标端口 TCP 80 的访问
            {
                "action":"accept",
                "dest":"192.168.8.222",
                "protocol":"tcp",
                "destport":"80"
            }
        }
    },
    "ifname@lte2":               // 第二个 NR/LTE 网络防火墙设置
    {
        "status":"disable",           // 禁用防火墙
        "default":"drop",
        "rule":
        {
        }
    }
}
```

示例, 为 ifname@lte 添加名为 webwork 的规则, 使 113.23.64.28 可以通过 ifname@lte 访问网关的 web 端口 80
```shell
forward@firewall:ifname@lte/rule/webwork={"action":"accept","src":"113.23.64.28","protocol":"tcp","destport":"80"}
ttrue
```

示例, 为 ifname@lte 添加名为 webpass 的规则, 使 113.23.64.28 可以通过 ifname@lte 访问 192.168.8.250 的 web 端口 80
```shell
forward@firewall:ifname@lte/rule/webpass={"action":"accept","src":"113.23.64.28","protocol":"tcp","dest":"192.168.8.250","destport":"80"}
ttrue
```

示例, 启用第一个 LTE 网络防火墙
```shell
forward@firewall:ifname@lte/status=enable
ttrue
```

示例, 将第一个 LTE 网络的默认动作修改为丢弃
```shell
forward@firewall:ifname@lte/default=drop
ttrue
```

示例, 删除 ifname@lte 的名为 webwork 的规则
```shell
forward@firewall:ifname@lte/rule/webwork=
ttrue
```

示例, 删除 ifname@lte 的名为 webpass 的规则
```shell
forward@firewall:ifname@lte/rule/webpass=
ttrue
```

示例, 同时修改多个属性 (**merge**)
```shell
forward@firewall|{"ifname@lte":{"status":"enable","default":"drop"}}
ttrue
```

### 组件 API
使用标准 **`forward@firewall`** get/set/merge 进行配置。

+ `on[]` **刷新某个外部 ifname 的入站防火墙规则**, *成功返回 ttrue*
    - 参数 **2** 传递 **`ifname`**; 从保存的配置中重建该 **ifname** 的防火墙 (在 **default** / **parasite** 网络模式下跳过)。

+ `off[]` **移除某个 ifname 的防火墙**

### 生命周期 API
+ `setup[]` / `shut[]` — 此组件**未**在默认的 **init** / **uninit** 调度中连接; 使用 **`on[]`** / **`off[]`**。

### Joint 处理器
| Joint 键 | 方法 |
|-----------|--------|
| `network/onextern` | `forward@firewall.on` |
| `network/onvpn` | `forward@firewall.on` |


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_firewall(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@firewall", "status") == NULL)
        return -1;
    return ssets_string("forward@firewall", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@firewall", "list", NULL); talk_free if JSON */
```
