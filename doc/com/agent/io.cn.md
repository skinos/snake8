## agent@io — IO 代理 -- GPIO 和网络 IO 管理
管理设备 GPIO 输入/输出，支持 IO 状态监控，状态变化时触发动作，并通过 TCP/UDP/MQTT 客户端和服务器向远程服务器报告 IO 状态

### 配置 ( `agent@io` )
```json
{
    "status":"IO 代理服务状态",                    // [ "disable", "enable" ]

    // GPIO 初始化映射
    "init":                                                // 定义每个 GPIO 的初始状态
    {
        "g1":"GPIO 1 初始状态",                       // [ "00", "01", "02", "0f", "0r", "0b", "10", "11", "2N-ON-OFF" ]
                                                              // "00" 或 "0f": 输入下降沿触发
                                                              // "01" 或 "0r": 输入上升沿触发
                                                              // "02" 或 "0b": 输入双边沿触发
                                                              // "10": 输出低电平
                                                              // "11": 输出高电平
                                                              // "2N-ON-OFF": 输出定时器, N 为模式, ON 为高电平持续时间(毫秒), OFF 为低电平持续时间(毫秒)
        "g2":"GPIO 2 初始状态"
        // ... 更多 GPIO（最多 g20）
    },

    // GPIO 触发动作
    "trigger":                                             // 定义 GPIO 状态变化时的动作
    {
        "g1":                                              // GPIO 1 的触发配置
        {
            "rising":                                      // 上升沿动作 (0->1)
            {
                "1":"command to execute",                  // 字符串命令格式: "agent@io.modify[g2,11]"
                "2":{"obj":"agent@io","op":"call","m":"modify","p":"g3,10"}  // JSON 命令格式
            },
            "falling":                                     // 下降沿动作 (1->0)
            {
                "1":"command to execute"
            },
            "both":                                        // 任意边沿变化时的动作
            {
                "1":"command to execute"
            }
        }
        // ... 更多 GPIO 触发器
    },

    // TCP/UDP 客户端（最多 9 个：client, client2, client3, ... client9）
    "client":
    {
        "status":"客户端状态",                          // [ "disable", "enable" ]
        "proto":"传输协议",                      // [ "tcp", "udp" ]
        "server":"远程服务器地址",                  // [ string ], 域名或 IP 地址
        "port":"远程服务器端口",                       // [ number ]
        "interval":"重连间隔（秒）",        // [ number ], 默认 10
        "id":"注册时的设备标识",           // [ string ], 可选
        "user":"注册时的用户名",                // [ string ], 可选
        "vcode":"注册时的验证码"       // [ string ], 可选
    },

    // MQTT 客户端（最多 9 个：mqtt, mqtt2, mqtt3, ... mqtt9）
    "mqtt":
    {
        "status":"MQTT 客户端状态",                     // [ "disable", "enable" ]
        "server":"MQTT 代理地址",                    // [ string ], 域名或 IP 地址
        "port":"MQTT 代理端口",                         // [ number ], 必填；典型值为 1883（省略时不会使用默认值）
        "mqtt_id":"MQTT 客户端 ID",                        // [ string ], 默认为设备 macid
                                                              // 设置为 "NULL" 表示使用随机 ID 并启用清洁会话
        "mqtt_username":"MQTT 用户名",                   // [ string ], 可选
        "mqtt_password":"MQTT 密码",                   // [ string ], 可选
        "mqtt_keepalive":"MQTT 保活时间（秒）",      // [ number ], 默认 60
        "mqtt_interval":"重连间隔（秒）",   // [ number ], 默认 10
        "mqtt_publish":"MQTT 发布主题",               // [ string ], 用于发布 IO 状态的主题
        "mqtt_publish_qos":"MQTT 发布 QoS 级别",       // [ number ], 0, 1 或 2, 默认 0
        "mqtt_subscribe":                                  // MQTT 订阅主题
        {
            "topic/name":"qos level"                       // [ "0", "1", "2" ]
        }
    },

    // TCP/UDP 服务器（最多 9 个：server, server2, server3, ... server9）
    "server":
    {
        "status":"服务器状态",                          // [ "disable", "enable" ]
        "proto":"传输协议",                      // [ "tcp", "udp" ]
        "port":"监听端口",                              // [ number ]
        "timeout":"客户端连接超时时间（秒）",  // [ number ], 0 表示无超时
        "limit":"最大 TCP 连接数（仅 TCP 服务器）",   // [ number ]
        "id":"设备标识",                            // [ string ], 可选
        "user":"用户名",                                 // [ string ], 可选
        "vcode":"验证码"                        // [ string ], 可选
    }
}
```

示例，显示所有配置
```shell
agent@io
{
    "status":"enable",
    "init":
    {
        "g1":"0b",                                         # gpio1 输入双边沿
        "g2":"11",                                         # gpio2 输出高电平
        "g3":"10"                                          # gpio3 输出低电平
    },
    "trigger":
    {
        "g1":
        {
            "rising":
            {
                "1":"agent@io.modify[g2,11]"               # 当 g1 上升沿时，设置 g2 为高电平
            },
            "falling":
            {
                "1":"agent@io.modify[g2,10]"               # 当 g1 下降沿时，设置 g2 为低电平
            }
        }
    },
    "client":
    {
        "status":"enable",
        "proto":"tcp",
        "server":"192.168.8.100",
        "port":"8899"
    },
    "mqtt":
    {
        "status":"enable",
        "server":"mqtt.example.com",
        "port":"1883",
        "mqtt_id":"mydevice001",
        "mqtt_username":"user1",
        "mqtt_password":"pass1",
        "mqtt_publish":"device/io/state",
        "mqtt_publish_qos":"1",
        "mqtt_subscribe":
        {
            "device/io/control":"1"
        }
    }
}
```

示例，启用 IO 代理，GPIO1 为输入，GPIO2 为输出
```shell
agent@io={"status":"enable","init":{"g1":"0b","g2":"10"}}
ttrue
```

示例，配置 TCP 客户端报告 IO 状态
```shell
agent@io:client={"status":"enable","proto":"tcp","server":"192.168.8.100","port":"8899"}
ttrue
```

示例，配置 MQTT 客户端
```shell
agent@io:mqtt={"status":"enable","server":"mqtt.example.com","port":"1883","mqtt_publish":"device/io/state","mqtt_subscribe":{"device/io/cmd":"1"}}
ttrue
```


### 组件 API
**可直接调用的** API，来自 HE / eline / HTTP `/he`。
+ `setup[]` **设置 IO 代理，启动服务**
    setup 将读取配置，检查 init 映射中是否定义了 GPIO 且 status 不为 "disable"，然后启动后台服务进程
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置 IO 代理
    ```shell
    agent@io.setup
    ttrue
    ```

+ `shut[]` **关闭 IO 代理服务**
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭 IO 代理
    ```shell
    agent@io.shut
    ttrue
    ```

+ `status[]` **获取所有 GPIO 输出和输入状态**
    - 失败返回 NULL，服务未运行
    - 成功返回包含当前 GPIO 状态的 JSON
    ```json
    // API 返回的属性介绍
    {
        "g1":"IO 状态码",                // 状态码格式：第一位为模式，第二位为状态
                                                // "00": 输入低电平
                                                // "01": 输入高电平
                                                // "10": 输出低电平
                                                // "11": 输出高电平
                                                // "12": 输出定时器模式
        "g2":"IO 状态码"
        // ... 更多 GPIO
    }
    ```

    示例，获取所有 GPIO 状态
    ```shell
    agent@io.status
    {
        "g1":"01",                                # gpio1 输入，当前状态为高电平
        "g2":"11",                                # gpio2 输出高电平
        "g3":"10"                                 # gpio3 输出低电平
    }
    ```

+ `current[]` **获取当前 IO 状态字符串**
    - 失败返回 NULL，服务未运行
    - 成功返回格式为 "gN=STATE;" 的 IO 状态行字符串

    示例，获取当前 IO 状态行
    ```shell
    agent@io.current
    g1=01;g2=11;g3=10;
    ```

+ `dump[]` **获取所有 GPIO 状态（包括缓存的输入状态）**
    类似于 status，但使用缓存的输入状态而不是从硬件读取
    - 失败返回 NULL，服务未运行
    - 成功返回包含 GPIO 状态的 JSON
    ```json
    {
        "g1":"IO 状态码",                // "00": 输入低电平, "01": 输入高电平, "0x": 输入未知
                                                // "10": 输出低电平, "11": 输出高电平, "12": 输出定时器
        "g2":"IO 状态码"
    }
    ```

    示例，转储所有 GPIO 状态
    ```shell
    agent@io.dump
    {
        "g1":"01",
        "g2":"11",
        "g3":"0x"                                 # gpio3 输入状态未知
    }
    ```

+ `list[]` **获取活跃对等节点的网络连接状态（参见下方说明）**
    - 失败返回 NULL，服务未运行
    - 成功返回 JSON；每个键是配置中的**连接名称**（例如 `client`、`client2`、`mqtt`，UDP 服务器为 `server:peer-ip:peer-port`）
    - **不包含：** TCP **监听**服务器单元（`proto` TCP 服务器）在实现中被跳过——仅显示客户端、MQTT 客户端和 UDP 服务器子套接字
    - **UDP 服务器：** 条目使用复合键：`<server-name>:<peer-ip>:<peer-port>` 而不是裸服务器名
    ```json
    // API 返回的属性介绍
    {
        "connection name":
        {
            "rx":"总接收字节数",           // [ number ]
            "tx":"总发送字节数",               // [ number ]
            "ip":"对端 IP 地址",                // [ ip address ], 仅用于客户端连接
            "connect":"连接状态"            // [ "ok" ], 已连接时存在
        }
        // ... 更多连接
    }
    ```

    示例，列出所有连接
    ```shell
    agent@io.list
    {
        "client":
        {
            "rx":"1024",
            "tx":"512",
            "ip":"192.168.8.100",
            "connect":"ok"
        },
        "mqtt":
        {
            "rx":"2048",
            "tx":"1024",
            "connect":"ok"
        }
    }
    ```

+ `modify[ io name, value ]` **在运行时修改 GPIO 状态**
    - io name ---- [ string ], GPIO 名称，如 "g1", "g2"
    - value ------ [ string ], 目标状态码
        - "0b" 或 "02": 切换为输入双边沿
        - "0f" 或 "00": 切换为输入下降沿
        - "0r" 或 "01": 切换为输入上升沿
        - "10": 切换为输出低电平
        - "11": 切换为输出高电平
        - "2N-ON-OFF": 切换为输出定时器，ON=高电平持续时间(毫秒)，OFF=低电平持续时间(毫秒)
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置 gpio2 为输出高电平
    ```shell
    agent@io.modify[g2,11]
    ttrue
    ```

    示例，设置 gpio1 为输入双边沿
    ```shell
    agent@io.modify[g1,0b]
    ttrue
    ```

    示例，设置 gpio3 为定时器模式（500ms 开，500ms 关）
    ```shell
    agent@io.modify[g3,2-500-500]
    ttrue
    ```

+ `report[]` **触发立即向所有连接报告 IO 状态**
    向服务发送 SIGUSR1 信号，将当前 IO 状态报告给所有已连接的 TCP/UDP/MQTT 对等节点
    - 成功返回 ttrue，信号已发送
    - 失败返回 tfalse，服务未运行

    示例，触发立即报告
    ```shell
    agent@io.report
    ttrue
    ```

+ `service[]` **内部后台服务（不直接调用）**
    这是由 setup 启动的主事件驱动服务，它处理：
    1. 根据 init 映射初始化 GPIO（设置输入/输出模式）
    2. 初始化 libevent 和 mosquitto 库
    3. 注册信号处理程序（SIGINT/SIGTERM 退出，SIGWINCH IO 变化，SIGUSR1 报告）
    4. 创建 UNIX 域套接字控制接口用于进程间通信
    5. 从配置创建 MQTT 客户端（mqtt, mqtt2, ... mqtt9）
    6. 从配置创建 TCP/UDP 客户端（client, client2, ... client9）
    7. 从配置创建 TCP/UDP 服务器（server, server2, ... server9）
    8. 运行事件循环处理所有 IO 事件

    **IO 状态报告协议（TCP/UDP）：**
    - 格式：`gN=STATE;gN=STATE;...`
    - 示例：`g1=01;g2=11;g3=10;`

    **IO 远程控制协议（TCP/UDP）：**
    - 格式：`gN=STATE;gN=STATE;...`（与报告相同，必须以分号结尾）
    - 示例：`g1=10;g2=11;`（设置 g1 输出低电平，g2 输出高电平）

    **TCP/UDP 客户端注册包：**
    - 格式：`macid=<macid>;id=<id>;user=<user>;vcode=<vcode>;`

    **MQTT 客户端：**
    - 向配置的主题发布 IO 状态
    - 订阅配置的主题用于远程控制
    - 支持 TLS，CA/证书/密钥文件位于：`<config_path>/io-<name>.ca`、`io-<name>.crt`、`io-<name>.key`

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@io`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。
+

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_io(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@io", "status") == NULL)
        return -1;
    return ssets_string("agent@io", "enable", "status") ? 0 : -1;
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

/* 示例: scall("agent@io", "list", NULL); 如果是 JSON 则调用 talk_free */
```
