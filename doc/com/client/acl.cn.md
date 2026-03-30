## client@acl — 访问控制管理
管理本地 ifname 的客户端访问控制列表（ACL）   
每个逻辑 **`ifname`**（例如 `ifname@lan`），ACL 控制匹配的流量是 **丢弃（drop）**、**接受（accept）** 还是 **交给后续规则处理（return）**，使用 **源地址（`src`）**、**目标地址**、**端口**、可选的 **时间窗口** 等条件。在 **`client@acl`** 下配置；当 **`status`** 为 **enable** 时，您定义的规则将在该 **ifname** 上生效。

### 配置 ( `client@acl` )
```json
// Attributes introduction 
{
    "interface name":                                     // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        // client access control list from this ifname
        "status":"enable or disable the access control list",       // [ "disable", "enable" ]
        "rule":                                                     // access control list, valid when "status" be "enable"
        {
            "rule name":                                                 // [ string ], user can custom the rule name
            {
                "action":"drop or accept",                               // [ "drop", "accept", "return" ]
                                                                                  // "drop" for forbid
                                                                                  // "accept" for pass
                                                                                  // "return" for no more matching
                "proto":"protocol type",                                 // [ "domain", "tcp", "udp", "all", "layer7" ]
                                                                                  // "domain", for domain filtering, "dest" should be the domain name
                                                                                  // "tcp", for TCP protocol
                                                                                  // "udp", for UDP protocol
                                                                                  // "layer7", for layer7 application protocol
                                                                                  // "all", or space or none for all protocols

                "src":"local source address",                         // [ string ] who may send (LAN side); you may also use the older key "source" if "src" is omitted
                                                                                  // single IP: 192.168.8.222
                                                                                  // multiple IP: 192.168.8.2,192.168.8.3,192.168.8.4
                                                                                  // range of IP: 192.168.8.2-192.168.8.4
                                                                                  // single MAC: 00:23:43:13:34:40
                                                                                  // space or none for all ip address
                "dest":"internet destination address",                   // [ string ] packets destination
                                                                                  // single IP: 202.96.11.32, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // multiple IP: 2.3.1.2,4.34.2.1,72.32,192.1, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // range of IP: 202.96.132.11-202.96.132.20, vaild when "proto" be "tcp" or "udp" or "all"
                                                                                  // domain: www.baidu.com, vaild when "proto" be "domain"
                                                                                  // layer7: Future expansion, vaild when "proto" be "layer7"
                                                                                  // space for all ip address, vaild when "proto" be "tcp" or "udp" or "all"
                "destport":"internet destination port",                  // [ number ] valid when "proto" be "tcp" or "udp"
                                                                                  // single port: 8080
                                                                                  // multiple port: 80,8000,8080
                                                                                  // range of port: 80-800
                                                                                  // space or none for all port
                "key":"keyword",                                         // [ string ] matching of keyword in packets, valid when "proto" be "tcp" or "udp" or "all"

                "timer":"specifying an effective time",                  // [ "disable", "enable" ]
                "timer_cfg":                                                 // effective time, valid when "timer" be "enable"
                {
                    "datestart":"starting date",                             // [ string ], format is YYYY-MM-DD
                    "datestop":"ending date",                                // [ string ], format is YYYY-MM-DD
                    "timestart":"start time of day",                         // [ string ], format is hh:mm:ss
                    "timestop":"end time of day",                            // [ string ], format is hh:mm:ss
                    "monthdays":"designated month days",                     // [ string ], optional, limits rule to certain days of the month
                    "weekdays":"designated week number"                      // [ string ], format is 1,2,3,..., 0 for Sunday
                }
            }
            // ... more rule
        }
    }
    // ... more ifname
}
```   

示例，显示当前所有设置
```shell
client@acl
{
    "ifname@lan":
    {
        "status":"enable",                      # 启用访问控制列表
        "rule":
        {
            "dis163":                             # 所有客户端在任何时间都无法访问域名 www.163.com
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.163.com",
                "timer":"enable",
                "timer_cfg":
                {
                    "datestart":"",
                    "datestop":"",
                    "timestart":"00:00:00",
                    "timestop":"23:59:59",
                    "weekdays":"1,2,3,4,5,6,7"
                }
            },
            "dis164":                             # 所有客户端无法访问域名 www.qq.com
            {
                "src":"",
                "proto":"domain",
                "action":"drop",
                "dest":"www.qq.com",
            }
        }
    }
}        
```

示例，添加名为 "disqq" 的 ACL 规则，ifname@lan 上的所有客户端无法访问域名 www.qq.com（省略 **`src`** 或将其留空以匹配该 **ifname** 上的任何 LAN 源地址）
```shell
client@acl:ifname@lan/rule/disqq={"proto":"domain","dest":"www.qq.com","action":"drop"}
ttrue
```

示例，删除名为 "disqq" 的 ACL 规则
```shell
client@acl:ifname@lan/rule/disqq=
ttrue
```

### 生命周期 API
+ `setup[]` / `shut[]` — 当为 **`client@acl`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装 FPK 的 **init** / **uninit** / **joint** 清单。
+


### Joint 处理程序
**Joint** 连接将事件映射到以下处理程序（与日常 **组件 API** 方法分开）：

+ `on[]` **当 LAN ifname 启动时刷新 ACL**，*成功返回 ttrue，失败返回 tfalse*
    - **`network/on`** → **`client@acl.on`**。
    - 参数 **2** 是包含 **`ifname`**（例如 `ifname@lan`）的 JSON 对象。
    - 在 **default** / **parasite** 网络模式下，不应用 ACL 更改。
    - 否则，当 **`status`** 为 **enable** 时，清除该 **ifname** 的 ACL 并从保存的配置重新应用。

+ `off[]` **当 LAN ifname 关闭时拆除 ACL**，*成功返回 ttrue*
    - **`network/off`** → **`client@acl.off`**。
    - 参数 **2** 是包含 **`ifname`** 的 JSON 对象。移除该 **ifname** 的 ACL 规则；如果缺少 **`ifname`**，则不执行任何操作，调用仍然成功。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_client_acl(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@acl", "status") == NULL)
        return -1;
    return ssets_string("client@acl", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@acl", "list", NULL); talk_free if JSON */
```
