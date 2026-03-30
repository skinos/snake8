## agent@local — 本地代理 -- LAN 管理服务
提供本地局域网管理服务，包括 JSON TCP 命令接口和 UDP 广播发现/命令接口，用于局域网上的设备发现、查询和配置

### 配置 ( `agent@local` )
```json
{
    // JSON TCP 服务
    "json":"JSON TCP 命令服务",                          // [ "disable", "enable" ]
    "json_port":"JSON TCP 服务监听端口",                 // [ number ], 默认 22220
    "json_command":"JSON TCP 服务命令权限",       // [ "query", "all" ]
                                                                   // 省略或为空时：等同于 "query"（不是 "all"）
                                                                   // "query" 仅允许查询命令（get, status, list, info）
                                                                   // "all" 允许所有命令，包括 set 和 call
    "json_manager":"JSON TCP 服务访问控制",           // [ string 或 json ]
                                                                   // 字符串格式：IP 或 MAC 地址用分号分隔 "192.168.8.100;00:11:22:33:44:55"
                                                                   // json 格式：{ "host1":"192.168.8.100", "host2":"00:11:22:33:44:55" }
                                                                   // 未设置时，不进行访问控制（允许所有）
                                                                   // 设置后，仅列出的 IP/MAC 可以访问，其他被拒绝

    // 广播 UDP 服务
    "broadcast":"广播 UDP 发现服务",              // [ "disable", "enable" ]
    "broadcast_port":"广播 UDP 服务监听端口",       // [ number ], 默认 22222
    "broadcast_group":"用于发现的广播组名",     // [ string ], 默认 "default"
    "broadcast_command":"广播服务命令权限",  // [ "query", "all", other ]
                                                                   // "query" 仅允许查询命令（get, status, list, info）
                                                                   // "all" 允许所有命令，包括 set 和 call
                                                                   // 其他值：禁用所有命令，仅基本发现
    "broadcast_manager":"广播服务访问控制"      // [ string 或 json ], 格式与 json_manager 相同
}
```

示例，显示所有配置
```shell
agent@local
{
    "broadcast":"enable",                     # 广播服务已启用
    "broadcast_port":"22222",                 # 广播监听端口 22222
    "broadcast_group":"default",              # 广播组名
    "broadcast_command":"query",              # 仅允许查询命令
    "json":"disable",                         # JSON TCP 服务已禁用
    "json_port":"22220"                       # JSON TCP 监听端口 22220
}
```

示例，启用 JSON TCP 服务并设置访问控制
```shell
agent@local={"json":"enable","json_port":"22220","json_command":"all","json_manager":"192.168.8.100;192.168.8.101"}
ttrue
```

示例，启用广播服务并赋予完整命令权限
```shell
agent@local:broadcast=enable
ttrue
agent@local:broadcast_command=all
ttrue
```

示例，禁用 JSON TCP 服务
```shell
agent@local:json=disable
ttrue
```

示例，设置广播组名
```shell
agent@local:broadcast_group=mygroup
ttrue
```


### 组件 API
**可直接调用的** API，来自 HE / eline / HTTP `/he`。
+ `setup[]` **设置本地代理服务**
    setup 将读取配置并启动已启用的服务：
    1. 如果 json 为 "enable"，设置 iptables 访问控制规则并启动 JSON TCP 服务
    2. 如果 broadcast 为 "enable"，设置 iptables 访问控制规则并启动广播 UDP 服务
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，设置本地代理
    ```shell
    agent@local.setup
    ttrue
    ```

+ `shut[]` **关闭所有本地代理服务**
    shut 将清理 json 和 broadcast 服务的 iptables 规则，并停止两个服务进程
    - 成功返回 ttrue
    - 失败返回 tfalse

    示例，关闭本地代理
    ```shell
    agent@local.shut
    ttrue
    ```

+ `json_start[]` **仅启动 JSON TCP 服务**
    - 成功返回 ttrue

    示例，启动 JSON 服务
    ```shell
    agent@local.json_start
    ttrue
    ```

+ `json_stop[]` **仅停止 JSON TCP 服务**
    - 成功返回 ttrue

    示例，停止 JSON 服务
    ```shell
    agent@local.json_stop
    ttrue
    ```

+ `broadcast_start[]` **仅启动广播 UDP 服务**
    - 成功返回 ttrue

    示例，启动广播服务
    ```shell
    agent@local.broadcast_start
    ttrue
    ```

+ `broadcast_stop[]` **仅停止广播 UDP 服务**
    - 成功返回 ttrue

    示例，停止广播服务
    ```shell
    agent@local.broadcast_stop
    ttrue
    ```

+ `json_service[]` **内部 JSON TCP 服务（不直接调用）**
    这是由 setup 启动的 JSON TCP 命令服务，它处理：
    1. 在本地网络接口上监听 TCP 端口（默认 22220）
    2. 等待系统启动（至少 60 秒运行时间）
    3. 接受来自管理工具的 TCP 连接
    4. 接收 JSON 命令请求并执行
    5. 返回包含结果的 JSON 响应

    服务将根据情况退出并返回不同的值：
    - 返回 terror：无法获取本地网络设备
    - 返回 tfalse：套接字错误或接受连接错误

    **命令权限 (json_command)：**
    - "query" 模式：仅允许 GET 操作和方法名中包含 "stat"、"list" 或 "info" 的 CALL 操作
    - "all" 模式：允许所有操作，包括 SET、OR 和任何 CALL

    **JSON 命令协议：**
    - 请求格式：
    ```json
    {
        "1":{"obj":"upnp@miniupnpd","op":"=","v":{"status":"enable"}},    // JSON 命令格式
        "2":"land@syslog:size=100",                                         // 字符串命令格式
        "3":"land@machine:mac=000371f12300"                                 // 字符串命令格式
    }
    ```
    - 响应格式：
    ```json
    {
        "1":"ttrue",
        "2":"ttrue",
        "3":"tfalse"
    }
    ```

+ `broadcast_service[]` **内部广播 UDP 服务（不直接调用）**
    这是由 setup 启动的 UDP 广播发现/命令服务，它处理：
    1. 在本地网络接口上监听 UDP 端口（默认 22222）
    2. 支持新协议（ENQ/ACK）和旧协议（逗号/管道分隔）
    3. 响应设备发现请求并返回设备信息
    4. 执行通过广播接收的命令并返回结果
    5. 支持地址分配用于设备 IP 配置

    服务将根据情况退出并返回不同的值：
    - 返回 terror：无法获取本地网络设备
    - 返回 tfalse：套接字错误或接收错误

    **广播协议：**

    *新协议 (ENQ/ACK)：*
    - 发现请求：`<group>$ENQ<commands separated by US char>`
    - 定向请求：`<macid>$ENQ<commands or JSON>`
    - 响应：`<macid>$ACK<results separated by US char>`

    *旧协议（逗号/管道）：*
    - 基本发现请求：`<group>\0`
    - 基本发现响应：`<macid>|<lanip>|<port>`
    - 查询请求：`<group>,<command1>,<command2>,...`
    - 查询响应：`<macid>|<result1>|<result2>|...`
    - 地址分配请求：`<macid>>{"ip":"...","mask":"...","gw":"...","dns":"..."}`

    **命令权限 (broadcast_command)：**
    - "query" 模式 (mode=1)：允许 GET 和有限的 CALL 操作（stat, list, info）。对于 agent@local、factory、gpio、machine 对象仅允许 CALL
    - "all" 模式 (mode=2)：允许所有操作
    - 其他值 (mode=0)：禁用所有命令执行，仅基本发现

### 生命周期 API
+ `setup[]` / `shut[]` -- 为 **`agent@local`** **实现时**，启动/停止组件服务或钩子。调度遵循已安装的 FPK **init** / **uninit** / **joint** 清单。
+

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_agent_local(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "agent@local", "status") == NULL)
        return -1;
    return ssets_string("agent@local", "enable", "status") ? 0 : -1;
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

/* 示例: scall("agent@local", "list", NULL); 如果是 JSON 则调用 talk_free */
```
