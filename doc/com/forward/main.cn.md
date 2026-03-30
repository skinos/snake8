## forward@main — 默认路由表管理
管理系统路由表, 不建议直接修改此配置, 建议通过方法进行管理

### 配置 ( `forward@main` )
```json
// 属性介绍 
{
    "rule name":        // [ string ], 用户可自定义规则名称
    {
        "target":"使用源 IP 地址选择数据包",            // [ ip address, network ]
        "mask":"使用源 IP 地址掩码选择数据包",      // [ netmask ], 当 "target" 为网络地址时必填
        "gw":"网关 IP 地址",                                     // [ ip address ]
        "metric":"路由跳数",                                          // [ number ]
        "ifname":"选择数据包输出 ifname",                    // [ "ifname@lan", "ifname@lan2", "ifname@wan", "ifname@lte", ... ], 逻辑 ifname
    }
    // ... 更多规则
}
```

示例, 显示当前所有默认路由规则设置
```shell
forward@main
{
    "myCustomRule1":    # 第一条规则名称为 "myCustomRule1"
    {                                # 使目标 192.168.1.0/255.255.255.0 路由到 ifname@wan 的 192.168.8.22, 跳数标记为 2
        "target":"192.168.1.0",
        "mask":"255.255.255.0",
        "gw":"192.168.8.22",
        "metric":"2",
        "ifname":"ifname@wan"
    }
    "youCustomRule":    # 第二条规则名称为 "youCustomRule"
    {                               # 使所有访问路由到 ifname@lan 的 192.168.9.22
        "gw":"192.168.9.22",
        "ifname":"ifname@lan"
    }
}
```  


### 组件 API
可从 HE / eline / HTTP `/he` **直接调用的** API。
+ `status[]` **获取当前默认路由表**
    - 失败返回 NULL, 错误返回 terror
    - 成功返回描述信息的 json   
    ```json
    // 方法返回的 json 属性介绍
    {
        "rule name":        // [ string ], 用户自定义的规则名称, 系统规则以 "~" 开头
        {
            "target":"使用源 IP 地址选择数据包",            // [ ip address, network ]
            "mask":"使用源 IP 地址掩码选择数据包",      // [ netmask ], 当 "target" 为网络地址时必填
            "gw":"网关 IP 地址",                                     // [ ip address ]
            "ifname":"选择数据包输出 ifname",                    // [ "ifname@lan", "ifname@lan2", ... ], 逻辑 ifname
            "netdev":"网络设备",                                     // [ string ]
            "flags":"路由标志",                                         // [ number ]
            "metric":"路由跳数",                                          // [ number ]
            "ref":"引用计数",                                       // [ number ]
            "use":"使用计数",                                             // [ number ]
            "status":"规则状态"                                          // [ "up", "down" ], "up" 表示启用, "down" 表示禁用
        }
        // ... 更多规则
    }
    ```

    示例, 获取当前路由规则
    ```shell
    forward@main.status
    {
        "myCustomRoute1":             // 这是用户添加的名为 "myCustomRoute1" 的规则
        {
            "target":"192.168.0.0",
            "mask":"255.255.255.0",
            "gw":"192.168.8.2",
            "ifname":"ifname@wan",
            "netdev":"eth0.2",
            "flags":1,
            "metric":2,
            "ref":0,
            "use":0,
            "status":"up"
        },
        "~auto1":                    // 这是系统规则
        {
            "target":"127.0.0.1",
            "mask":"255.255.255.0",
            "gw":"0.0.0.0",
            "netdev":"lo",
            "flags":1,
            "metric":0,
            "ref":0,
            "use":0,
            "status":"up"
        }
    }
    ```   

+ `add[ name, [target], [mask], [gateway], [ifname], [metric] ]` **添加路由规则**
    - name ----------- [ string ], 路由规则名称  
    - target --------- [ network, ip address ]
    - mask ----------- [ network mask ]
    - gateway -------- [ ip address ]
    - ifname --------- [ "ifname@lan", "ifname@lte", ... ] 可以通过调用 network@frame.list 获取列表
    - metric --------- [ number ]
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例, 添加名为 office1 的规则, 使地址 192.168.2.12 路由到 LAN 的 192.168.9.40
    ```shell
    forward@main.add[ office1, 192.168.2.12, 255.255.255.0, 192.168.9.40, ifname@lan ]
    ttrue
    ```   

    示例, 添加名为 office2 的规则, 使所有地址路由到 LAN 的 192.168.9.41
    ```shell
    forward@main.add[ office2, , , 192.168.9.41, ifname@lan ]
    ttrue
    ```   

+ `delete[ name ]` **删除路由规则**
    - name ----------- [ string ], 路由规则名称  
    - 成功返回 ttrue
    - 失败返回 tfalse
    
    示例, 删除名为 office2 的自定义路由
    ```shell
    forward@main.delete[ office2 ]
    ttrue
    ```

    示例, 删除名为 office1 的自定义路由
    ```shell
    forward@main.delete[ office1 ]
    ttrue
    ```

### 生命周期 API
+ `setup[]` **应用保存的静态路由**, *成功返回 ttrue* — 通常在默认 forward 软件包中按 **`init` → `app` → `forward@main.setup`** 顺序调度。也可以手动调用。
+ `shut[]` — 未列在默认的 **`uninit`** 中; 按产品需要添加。

### Joint 处理器
| Joint 键 | 方法 |
|-----------|--------|
| `network/on` | `forward@main.on` |
| `network/onextern` | `forward@main.on` |
| `network/onvpn` | `forward@main.on` |

+ `on[]` **在网络事件后重新应用匹配的静态路由**, *成功返回 ttrue*
    - 参数 **2** 是一个 JSON 对象; 当存在 **`ifname`** 时, 仅重新考虑与该逻辑 **ifname** 关联的规则, 并在需要时重新添加。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_main(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@main", "status") == NULL)
        return -1;
    return ssets_string("forward@main", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@main", "list", NULL); talk_free if JSON */
```
