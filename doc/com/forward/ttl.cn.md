## forward@ttl — TTL 设置
管理每个 LAN **ifname** 的出站 IP 数据包 TTL

### 配置 ( `forward@ttl` )
```json
// 属性介绍 
{
    "ifname@…":                   // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], 此 ifname 的 TTL 策略
    {
        "mode":"TTL 设置模式",     // [ "disable", "fix", "inc", "dec" ], TTL 修改模式; "disable" 关闭 TTL (WUI); 旧值 "none" 视同 disable
        "fix":"输出数据包 TTL",         // [ number ], 当 "mode" 为 "fix" 时有效
        "inc":"增加 TTL",          // [ number ], 当 "mode" 为 "inc" 时有效
        "dec":"减少 TTL"             // [ number ], 当 "mode" 为 "dec" 时有效
    }
    // ... 更多 ifname

}
```   

示例, 显示当前所有 TTL 配置
```shell
forward@ttl
{
    "ifname@lan":
    {
        "mode":"fix",     // 固定输出数据包的 TTL
        "fix":"99"        // TTL 值为 99
    }
}
```  

示例, 将 TTL 修改为 70
```shell
forward@ttl:ifname@lan/fix=70
ttrue
```  

示例, 合并某个 LAN 的 TTL 设置
```shell
forward@ttl:ifname@lan|{"mode":"fix","fix":"64"}
ttrue
```

### 组件 API
使用标准 **`forward@ttl`** get/set/merge 进行配置。

+ `on[]` **在 LAN ifname 启动后重新应用 TTL 策略**, *成功返回 ttrue*
    - 参数 **2** 传递 **`ifname`**; 从保存的配置中重新应用该 **ifname** 的 TTL 设置。

+ `off[]` **移除某个 ifname 的 TTL 修改**

### 生命周期 API
+ `setup[]` / `shut[]` — 此组件**未**在默认的 **init** / **uninit** 调度中连接; 使用 **`on[]`** / **`off[]`**。

### Joint 处理器
| Joint 键 | 方法 |
|-----------|--------|
| `network/on` | `forward@ttl.on` |

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_ttl(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@ttl", "status") == NULL)
        return -1;
    return ssets_string("forward@ttl", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@ttl", "list", NULL); talk_free if JSON */
```
