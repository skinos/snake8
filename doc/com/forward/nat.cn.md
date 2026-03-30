## forward@nat — 系统 NAT/DMZ 管理
管理从外部 **ifname** 到本地网络客户端的 NAT 端口映射

### 配置 ( `forward@nat` )
```json
// 属性介绍 
{
    "ifname@…":                    // [ "ifname@lte", "ifname@lte2", "ifname@wan", ... ], 此外部 ifname 的 NAT 规则
    {
        "mode":"NAT 模式",               // [ "forward", "dnat" ], "forward" 表示双向 NAT, "dnat" 表示目标 NAT
        "dmzhost":"DMZ 主机 IP 地址", // [ ip address ]
        "rule":                             // NAT 规则列表
        {
            "rule name":                        // [ string ]:{}
            {
                "targetport":"目标端口",         // [ number ], 外部 ifname 上的端口
                                                        // 单个端口: 8080
                                                        // 多个端口: 80,8000,8080
                                                        // 端口范围: 80-800
                "protocol":"协议类型",         // [ "tcp", "udp", "tcpudp", "icmp" ], "tcpudp" 表示 TCP 和 UDP
                "destip":"目标地址",     // [ ip address ], 本地网络 IP 地址
                "destport":"目标端口"       // [ number ]
                                                        // 单个端口: 8080
                                                        // 多个端口: 80,8000,8080
                                                        // 端口范围: 80-800
            }
            // ... 更多规则
        }
    }
    // ... 更多 ifname
}
```  

示例, 显示所有 NAT 设置
```shell
forward@nat
{
    "ifname@wan":   // 用于 WAN
    {
        "mode":"dnat",                        // dnat 模式
        "rule":
        {
            "rdesktop":                       // 将 ifname@wan 的 3389 映射到 192.168.31.250 的 TCP 3389
            {
                "targetport":"3389",
                "protocol":"tcp",
                "destip":"192.168.31.250",
                "destport":""
            }
        }
    },
    "ifname@lte":    // 用于 LTE
    {
        "rule":
        {
            "ssh":                           // 将 ifname@lte 的 22 映射到 192.168.31.230 的 22
            {
                "targetport":"22",
                "protocol":"tcp",
                "destip":"192.168.31.230",
                "destport":""
            },
            "devport":
            {
                "targetport":"10101",       // 将 ifname@lte 的 10101 映射到 192.168.31.230 的 10101
                "protocol":"tcp",
                "destip":"192.168.31.230",
                "destport":""
            }
        },
        "dmzhost":""
    },
    "ifname@lte2":    // 用于 LTE2
    {
        "dmzhost":"192.168.31.250"          // 将 ifname@lte2 的所有 tcp/udp 协议映射到 192.168.31.250
    }
}
```

示例, 添加名为 "telnet" 的 NAT 规则, 使 LTE 的 tcp 端口 23 映射到 192.168.31.230 的 23
```shell
forward@nat:ifname@lte/rule/telnet={"targetport":"23","protocol":"tcp","destip":"192.168.31.230","destport":"23"}
ttrue
```

示例, 添加名为 "web" 的 NAT 规则, 使 WAN 的 tcp 端口 80 映射到 192.168.31.231 的 8080
```shell
forward@nat:ifname@wan/rule/web={"targetport":"80","protocol":"tcp","destip":"192.168.31.231","destport":"8080"}
ttrue
```

示例, 添加名为 "dns" 的 NAT 规则, 使 WAN 的 tcp 端口 53 映射到 192.168.31.231 的 53
```shell
forward@nat:ifname@wan/rule/dns={"targetport":"53","protocol":"udp","destip":"192.168.31.231"}
ttrue
```

示例, 添加名为 "test" 的 NAT 规则, 使 WAN 的 tcp 端口 1000-1200 映射到 192.168.31.231 的 1000-1200
```shell
forward@nat:ifname@wan/rule/test={"targetport":"1000-1200","protocol":"tcp","destip":"192.168.31.231"}
ttrue
```

示例, 删除 WAN 上名为 web 的规则
```shell
forward@nat:ifname@wan/rule/web=
ttrue
```

示例, 删除 WAN 上名为 telnet 的规则
```shell
forward@nat:ifname@lte/rule/telnet=
ttrue
```

示例, 将 LTE 的 DMZ 主机修改为 192.168.31.250
```shell
forward@nat:ifname@lte/dmzhost=192.168.31.250
ttrue
```

示例, 删除 LTE 的 DMZ 主机
```shell
forward@nat:ifname@lte/dmzhost=
ttrue
```

示例, 同时修改多个属性 (**merge**)
```shell
forward@nat|{"ifname@wan":{"mode":"dnat","rule":{"web":{"targetport":"80","protocol":"tcp","destip":"192.168.1.10","destport":"8080"}}}}
ttrue
```

### 组件 API
配置也可通过标准 **`forward@nat`** get/set/merge 进行读写 (参见上文 **配置**)。

+ `on[]` **刷新某个外部 ifname 的 NAT/端口映射**, *成功返回 ttrue*
    - 参数 **2** 传递 **`ifname`**; 从保存的配置中重建该外部 **ifname** 的 NAT 规则 (在 **default** / **parasite** 网络模式下跳过)。

+ `off[]` **移除某个外部 ifname 的 NAT**

### 生命周期 API
+ `setup[]` / `shut[]` — **未**列在 **`forward@nat`** 默认的 **init** / **uninit** 调度中; 规则通过 **`on[]`** / **`off[]`** 和配置更改来应用。

### Joint 处理器
| Joint 键 | 方法 |
|-----------|--------|
| `network/onextern` | `forward@nat.on` |
| `network/onvpn` | `forward@nat.on` |


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_nat(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@nat", "status") == NULL)
        return -1;
    return ssets_string("forward@nat", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@nat", "list", NULL); talk_free if JSON */
```
