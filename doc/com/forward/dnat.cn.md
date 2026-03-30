## forward@dnat — 目标 NAT 管理
管理目标 NAT, 将网关上的互联网端口代理给本地客户端访问

### 配置 ( `forward@dnat` )
```json
// 属性介绍 
{

    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], 此 LAN ifname 的代理规则
    {
        "rule name":                    // [ string ], 用户可自定义规则名称
        {
            "destip":"互联网服务器 IP 地址",    // [ ip address ]
            "destport":"互联网服务器端口",        // [ number ]
                                                                // 单个端口: 80
                                                                // 端口范围: 80-100
                                                                // 多个端口: 80,8080,8000
            "protocol":"协议类型",               // [ "tcp", "udp", "tcpudp" ], "tcpudp" 表示 TCP 和 UDP
            "targetport":"本地代理端口"           // [ number ]
        },
        // ... 更多规则
    }
    // ... 更多 ifname
}
```

示例, 显示当前所有 dnat 规则
```shell
forward@dnat
{
    "ifname@lan":                       // 用于 LAN
    {
        "proxy1":                          // 规则名称为 proxy1
        {
            "destip":"29.23.11.35",        // 互联网服务器 IP 为 29.23.11.35
            "destport":"28-90",            // 互联网服务器端口从 28 开始, 到 90 结束 
            "protocol":"tcpudp",           // 代理 tcp 和 udp
            "targetport":"100"             // 在此 LAN ifname 上的端口 100 代理服务器
        }
    },
    "ifname@lan2":                      // 用于 LAN2
    {
        "forweb":                          // 规则名称为 forweb
        {
            "destip":"129.232.91.5",       // 互联网服务器 IP 为 129.232.91.5
            "destport":"80",               // 互联网服务器端口为 80
            "protocol":"tcp",              // 代理 tcp
            "targetport":"8000"            // 在此 LAN ifname 上的端口 8000 代理服务器
        }
    }    
}
```

示例, 向 ifname@lan 添加名为 proxy2 的规则
```shell
forward@dnat:ifname@lan/proxy2={"destip":"202.96.134.144","destport":"53","protocol":"tcpudp","targetport":"500"}
ttrue
```   

示例, 将名为 proxy2 的规则的 destport 修改为 55
```shell
forward@dnat:ifname@lan/proxy2/destport=55
ttrue
```   

示例, 从 ifname@lan 删除名为 proxy2 的规则
```shell
forward@dnat:ifname@lan/proxy2=
ttrue
```   

示例, 同时修改多个属性 (**merge**)
```shell
forward@dnat|{"ifname@lan":{"proxy1":{"destip":"1.2.3.4","destport":"80","protocol":"tcp","targetport":"8080"}}}
ttrue
```

### 组件 API
使用标准 **`forward@dnat`** get/set/merge 进行配置 (参见上文)。

+ `on[]` **刷新某个 LAN ifname 的目标 NAT 代理规则**, *成功返回 ttrue*
    - 参数 **2** 传递 **`ifname`**; 从保存的配置中重建该 **ifname** 的规则 (在 **default** / **parasite** 网络模式下跳过)。

+ `off[]` **移除某个 ifname 的 DNAT 规则**

### 生命周期 API
+ `setup[]` / `shut[]` — 此组件**未**在默认的 **init** / **uninit** 调度中连接; 通过 **`on[]`** / **`off[]`** 刷新。

### Joint 处理器
| Joint 键 | 方法 |
|-----------|--------|
| `network/on` | `forward@dnat.on` |


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_dnat(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@dnat", "status") == NULL)
        return -1;
    return ssets_string("forward@dnat", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@dnat", "list", NULL); talk_free if JSON */
```
