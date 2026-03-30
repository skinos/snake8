## arch@net — 网络拓扑绑定

将**特定运行模式**下的拓扑 JSON（网桥、`extern`、`connect` 等）映射到当前运行的 **`network_mode`** 切片中。组件对象为 **`arch@net`**；配置**按模式分片**（与 `arch@ethernet` 相同的模式）。实现代码：[`net/net.c`](net/net.c)。

### 配置项 ( `arch@net` )
**`arch@net`** 的**持久化配置对象**（通过 `arch@net`、`arch@net:path`、合并 `|{json}` 等方式进行查询/设置）。读写操作以寄存器 **`network_mode`** 为键；如果该寄存器为空，则 **`_get` / `_set` 会失败**。

```json
// Attributes introduction
// Top-level keys are mode names (e.g. "default", "gateway", "dgateway", …).
// Shape is product-specific; see ODM net.cfg (e.g. odm/rk3568/net.cfg).
{
    "default": {
        "bridge": { },                         // bridge configuration per mode
        "extern": { },                         // external uplink interface mapping
        "local": { }                           // local interface mapping
    }
}
```

示例，显示所有配置（在 `network_mode` 已设置之后）
```shell
arch@net
{
    "bridge":                                  # 当前模式的网桥配置
    {
        "lan":                                     # LAN 网桥
        {
            "member":                                  # 网桥成员接口
            {
                "ethernet@lan2":"",                        # 以太网端口 2
                "ethernet@lan3":"",                        # 以太网端口 3
                "wifi@nssid":"",                           # 2.4G SSID
                "wifi@assid":""                            # 5.8G SSID
            },
            "stp":"disable"                            # STP 已禁用
        }
    },
    "local":                                   # 本地接口映射
    {
        "ifname@lan":                              # LAN 逻辑接口
        {
            "concom":"ifname@ethcon",                  # 连接组件
            "ifdev":"bridge@lan"                       # 绑定到 bridge@lan
        }
    },
    "extern":                                  # 外部上行链路映射
    {
        "ifname@wan":                              # WAN 逻辑接口
        {
            "concom":"ifname@ethcon",                  # 连接组件
            "ifdev":"ethernet@lan1"                    # 绑定到以太网端口 1
        }
    }
}
```

示例，在某一模式子树下合并配置
```shell
arch@net:gateway|{"bridge":{"lan":{"stp":"enable"}}}
ttrue
```


### C 代码示例
```c
#include "skin/skin.h"

static int example_arch_net(void)
{
    talk_t v = sgets("arch@net", NULL);
    if (v == NULL || v <= tpanic)
        return -1;
    talk_free(v);
    return 0;
}
```
