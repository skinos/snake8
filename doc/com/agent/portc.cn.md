## agent@portc — 端口客户端 — 远程端口代理
连接端口代理服务，使远程会话可以到达本地 **TCP**、**UDP** 或**串口**目标。维护到服务器的备用链接，以便快速响应新的代理请求。

### 配置 ( `agent@portc` )
```json
{
    // 服务器连接
    "server":"端口代理服务器地址",                                  // [ string ], 域名或 IP 地址
                                                                              // 未设置时，使用 heclient 的服务器
    "port":"端口代理服务器端口",                                       // [ number ], 默认 20005
    "user":"注册时的用户名",                                    // [ string ], 未设置时，使用 heclient 的 user
    "vcode":"验证码",                                           // [ string ], 未设置时，使用 heclient 的 vcode

    // 绑定外部网络
    "extern":"绑定外部 ifname 以连接服务器",                   // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], 可选
                                                                              // 未设置时，使用默认网关连接服务器

    // 连接池
    "pond":"连接池中的空闲连接数",                       // [ number ], 默认 3

    // 超时控制
    "connect_timeout":"连接超时时间（秒）",                     // [ number ], 默认 15
    "idle_keeplive_interval":"空闲连接的保活间隔",      // [ number ], 默认 8，单位为秒
    "idle_keeplive_timeout":"空闲连接的超时时间",                 // [ number ], 默认 30，单位为秒
    "use_keeplive_timeout":"活跃代理连接的超时时间"           // [ number ], 默认 360，单位为秒
}
```

示例，显示所有配置
```shell
agent@portc
{
    "port":"20005",                           # 服务器端口
    "pond":"3"                                # 连接池中有 3 个空闲连接
}
```

示例，设置端口代理服务器和端口
```shell
agent@portc={"server":"proxy.ashyelf.com","port":"20005","user":"ashyelf"}
ttrue
```

示例，修改连接池大小为 5
```shell
agent@portc:pond=5
ttrue
```

示例，修改空闲保活间隔为 15 秒
```shell
agent@portc:idle_keeplive_interval=15
ttrue
```

示例，设置外部网络接口
```shell
agent@portc:extern=ifname@lte
ttrue
```


### 组件 API
**可直接调用的** API，来自 HE / eline / HTTP `/he`。
+ `setup[]` **设置端口客户端，启动连接服务**
    启动后台服务进程以连接端口代理服务器
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置端口客户端
    ```shell
    agent@portc.setup
    ttrue
    ```

+ `shut[]` **关闭端口客户端**
    停止后台服务进程
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭端口客户端
    ```shell
    agent@portc.shut
    ttrue
    ```

+ `status[]` **获取端口客户端当前状态**
    - 失败返回 NULL
    - 成功返回描述状态信息的 JSON
    ```json
    // API 返回的属性介绍
    {
        "status":"当前状态",             // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                                  // "uping" 正在连接服务器
                                                  // "down" 服务未运行
                                                  // "online" 已成功连接到服务器
                                                  // "usererror" 用户名错误或不存在
                                                  // "vcodeerror" 验证码错误
        "server":"解析后的服务器 IP"           // [ ip address ], 仅在 status 为 "uping" 或 "online" 时可用
    }
    ```

    示例，获取已连接时的状态
    ```shell
    agent@portc.status
    {
        "status":"online",                        # 已连接到服务器
        "server":"114.132.219.158"                # 解析后的服务器 IP
    }
    ```

    示例，获取正在连接时的状态
    ```shell
    agent@portc.status
    {
        "status":"uping",                         # 正在连接服务器
        "server":"114.132.219.158"
    }
    ```

    示例，获取服务停止时的状态
    ```shell
    agent@portc.status
    {
        "status":"down"                           # 服务未运行
    }
    ```

    示例，获取用户名错误时的状态
    ```shell
    agent@portc.status
    {
        "status":"usererror"                      # 用户名错误
    }
    ```

+ `list[]` **获取所有代理连接信息**
    - 失败返回 NULL
    - 成功返回包含所有连接详情的 JSON
    ```json
    // API 返回的属性介绍
    {
        "file descriptor number":                               // [ number ], 服务器连接的文件描述符
        {
            "local_ip":"连接服务器使用的本地 IP",       // [ ip address ]
            "local_port":"连接服务器使用的本地端口",   // [ number ]
            "tx":"发送到服务器的字节数",                        // [ number ]
            "rx":"从服务器接收的字节数",                  // [ number ]
            "last":"最后接收时间（运行时间标记）",           // [ number ]
            "port":"服务器端口号",                   // [ number ], 仅在已配对时存在
            "hand_ip":"代理目标 IP 地址",                // [ ip address ], 仅在已配对时存在
            "hand_port":"代理目标端口或串口设备",   // [ string ], 端口号或 "uart@serial"，仅在已配对时存在
            "hand_proto":"代理协议类型",                 // [ "t", "u", "d" ], t=tcp, u=udp, d=串口设备，仅在已配对时存在
            "hand_tx":"发送到代理目标的字节数",             // [ number ], 仅在已配对时存在
            "hand_rx":"从代理目标接收的字节数"        // [ number ], 仅在已配对时存在
        }
        // ... 更多连接
    }
    ```

    示例，列出所有连接，包含一个空闲连接和一个活跃代理
    ```shell
    agent@portc.list
    {
        "5":                                                    # fd 5, 空闲连接
        {
            "local_ip":"192.168.8.1",
            "local_port":"45678",
            "tx":"128",
            "rx":"64",
            "last":"12345"
        },
        "7":                                                    # fd 7, 活跃代理到本地 Web
        {
            "local_ip":"192.168.8.1",
            "local_port":"45680",
            "tx":"4096",
            "rx":"8192",
            "last":"12350",
            "port":"80",
            "hand_ip":"192.168.8.1",
            "hand_port":"80",
            "hand_proto":"t",
            "hand_tx":"2048",
            "hand_rx":"4096"
        }
    }
    ```

+ `service[]` **内部（不通过 HE 调用）**
    由 **`setup[]`** 启动的后台工作进程：使用 **`agent@heclient`**（或本地字段）作为未设置时的服务器身份，维护 **`pond`** 个空闲服务器链接池，在代理连接/断开时更新链接，并应用配置中的超时字段。**认证错误**会停止且不自动重启；**网络/套接字**问题和**外部接口未就绪**会重试。

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@portc`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_portc(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@portc", "status") == NULL)
        return -1;
    return ssets_string("agent@portc", "enable", "status") ? 0 : -1;
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

/* 示例: scall("agent@portc", "list", NULL); 如果是 JSON 则调用 talk_free */
```