## agent@heclient — HE 客户端 — Heport 远程管理客户端
连接远程 Heport 服务进行管理：远程命令、状态上报，以及协调控制相关的代理组件（**`agent@portc`**、**`agent@gtog`** 等）。

### 配置 ( `agent@heclient` )
```json
{
    // 基本配置
    "status":"连接 Heport 服务器进行远程管理",   // [ "disable", "enable" ]
    "server":"Heport 服务器地址",                            // [ string ], 域名或 IP 地址
    "port":"Heport 服务器端口",                                 // [ number ], 默认 20002
    "user":"绑定到账户的用户名",               // [ string ]
    "vcode":"账户的验证码",                 // [ string ], 可选
    "type":"设备类型",                                        // [ string ], 默认 "router"

    // 绑定外部网络
    "extern":"绑定外部 ifname 以连接服务器",         // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], 可选
                                                                    // 调用 network@frame.local[extern] 获取列表
                                                                    // 未设置时，使用默认网关连接服务器

    // 连接控制
    "connect_timeout":"连接超时时间（秒）",       // [ number ], 默认 20
    "keeplive_interval":"保活间隔时间（秒）",          // [ number ], 默认 10
    "keeplive_timeout":"保活超时时间（秒）"         // [ number ], 默认 35
}
```

示例，显示所有配置
```shell
agent@heclient
{
    "status":"enable",                        # 远程管理已启用
    "server":"cls.ashyelf.com",               # Heport 服务器地址
    "port":"20002",                           # Heport 服务器端口
    "user":"ashyelf",                         # 绑定到账户 ashyelf
    "type":"router",                          # 设备类型为 router
    "connect_timeout":"20",                   # 连接超时 20 秒
    "keeplive_interval":"10",                 # 保活间隔 10 秒
    "keeplive_timeout":"35"                   # 保活超时 35 秒
}
```

示例，启用 HE 客户端并绑定到账户 dimmalex@gmail.com
```shell
agent@heclient={"status":"enable","server":"devport.ashyelf.com","port":"20002","user":"dimmalex@gmail.com","vcode":"123456"}
ttrue
```

示例，禁用 HE 客户端
```shell
agent@heclient:status=disable
ttrue
```

示例，修改 Heport 服务器为 heport.ashyelf.com
```shell
agent@heclient:server=heport.ashyelf.com
ttrue
```

示例，修改保活间隔为 30 秒
```shell
agent@heclient:keeplive_interval=30
ttrue
```

示例，设置外部网络接口为 ifname@lte
```shell
agent@heclient:extern=ifname@lte
ttrue
```


### 组件 API
**可直接调用的** API，来自 HE / eline / HTTP `/he`。
+ `setup[]` **设置 HE 客户端，启动连接服务**
    setup 将读取配置，如果 status 为 "enable"，则启动后台服务进程连接 Heport 服务器
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置 HE 客户端
    ```shell
    agent@heclient.setup
    ttrue
    ```

+ `shut[]` **关闭 HE 客户端及所有托管组件**
    停止后台客户端并按配置拆除托管的 **`agent@gtog`** / **`agent@portc`** 状态。
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭 HE 客户端
    ```shell
    agent@heclient.shut
    ttrue
    ```

+ `status[]` **获取 HE 客户端当前状态信息**
    - 失败返回 NULL
    - 成功返回描述状态信息的 JSON
    ```json
    // API 返回的属性介绍
    {
        "status":"当前状态",        // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                             // "uping" 正在连接服务器
                                             // "down" 服务未运行
                                             // "online" 已成功连接到服务器
                                             // "usererror" 用户名错误或不存在
                                             // "vcodeerror" 验证码错误
        "server":"解析后的 Heport 服务器 IP" // [ ip address ], 仅在 status 为 "uping" 或 "online" 时可用
    }
    ```

    示例，获取已连接时的 HE 客户端状态
    ```shell
    agent@heclient.status
    {
        "status":"online",                    # 已成功连接到服务器
        "server":"114.132.219.158"            # Heport 服务器解析后的 IP 地址
    }
    ```

    示例，获取正在连接时的状态
    ```shell
    agent@heclient.status
    {
        "status":"uping",                     # 正在连接服务器
        "server":"114.132.219.158"
    }
    ```

    示例，获取服务停止时的状态
    ```shell
    agent@heclient.status
    {
        "status":"down"                       # 服务未运行
    }
    ```

    示例，获取用户名错误时的状态
    ```shell
    agent@heclient.status
    {
        "status":"usererror"                  # 用户名错误，服务将退出
    }
    ```

    示例，获取验证码错误时的状态
    ```shell
    agent@heclient.status
    {
        "status":"vcodeerror"                 # 验证码错误，服务将退出
    }
    ```

+ `update[]` **通知服务重新发送设备信息到服务器**
    请求运行中的客户端刷新并上传设备快照（机器、网关、可选的 GNSS/传感器等）到服务器。
    - 成功返回 ttrue，服务进程正在运行且信号已发送
    - 失败返回 tfalse，服务进程未运行

    示例，更新设备信息到服务器
    ```shell
    agent@heclient.update
    ttrue
    ```

+ `adjust[ {adjust configuration} ]` **调整其他组件的配置和状态**
    adjust 将修改指定组件的配置并相应地重启它们。
    如果新配置与现有配置相同，则仅根据其 status 字段启动或停止组件。
    如果配置不同，则先关闭组件，应用新配置，然后启动它
    - {adjust configuration} ------ json
    ```json
    // 传递给 API 的 JSON 属性介绍
    {
        "component name":                           // [ string ], 完整的组件对象名（例如 "agent@portc", "agent@gtog"）
        {
            // 此组件的完整配置
        }
        // ...更多其他组件配置
    }
    ```
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，调整为启用端口客户端，禁用网络客户端
    ```shell
    agent@heclient.adjust[{"agent@portc":{"status":"enable"},"agent@gtog":{"status":"disable"}}]
    ttrue
    ```

    示例，调整为启用 GTOG 并配置服务器
    ```shell
    agent@heclient.adjust[{"agent@gtog":{"status":"enable","server":"192.168.1.1","port":"20000"}}]
    ttrue
    ```

+ `service[]` **内部（不通过 HE 调用）**
    由 **`setup[]`** 启动的后台工作进程：维护与 Heport 服务的会话，注册设备，保持链路活跃，执行远程 **`object.method`** 请求并返回结果。遇到**账户/验证**错误时，客户端停止且不自动重启；遇到**连接/超时/外部接口未就绪**时，按平台策略重试。

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@heclient`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。

### 联合处理程序
| 联合键 | 方法 |
|-----------|--------|
| `network/online` | `agent@heclient.setup` |
| `machine/status` | `agent@heclient.update` |


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_heclient(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@heclient", "status") == NULL)
        return -1;
    return ssets_string("agent@heclient", "enable", "status") ? 0 : -1;
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

/* 示例: scall("agent@heclient", "list", NULL); 如果是 JSON 则调用 talk_free */
```
