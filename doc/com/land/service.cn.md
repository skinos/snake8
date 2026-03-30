## land@service — 服务管理

`land@service` 管理网关上的长时间运行的后台任务（服务）。
服务将任何组件 API 调用包装为受监控的进程：
如果进程意外退出，系统将自动重启它。
使用下方的组件 API 来注册、启动、停止、重置和查询服务。

### 概念
* 服务是一个 Linux 后台进程
* 服务通常是一个永不退出的无限循环
* 如果服务意外退出，系统将重启该服务
* 对象中的所有 API 都可以在服务模式下运行。但是 API 不应退出；否则系统会频繁重新运行它。


### 配置 ( `land@service` )

`land@service` 的**持久化配置对象**（通过 `land@service`、`land@service:path` 查询/设置，合并 `|{json}` 等）。


`land@service` **没有**单独的 JSON 配置树。所有行为通过下方的**组件 API** 驱动（`run`、`delete`、`list` 等）。

### 组件 API

+ `run[ [delay], service name, object, API, [parameter list,,,] ]` **添加服务**，注册并启动服务，已存在时更新并重置
    - delay ------------------ [ number ]，启动前延迟（微秒；例如 5000000 表示 5 秒）
    - service name ----------- [ string ]，服务名称，可自定义
    - object ----------------- [ string ]，组件名称
    - API -------------------- [ string ]，组件的 API
    - parameter list --------- [ string ]，参数列表
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加 wui@admin.service 运行，将此服务命名为 wuiserver
    ```shell
    land@service.run[ , wuiserver, wui@admin, service ]
    ttrue
    ```

    示例，添加 tui@ssh.service 延迟 5 秒运行，将此服务命名为 sshserver，第一个参数为 192.168.8.1，第二个参数为 23
    ```
    land@service.run[ 5000000, sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `delete[ service name ]` **停止并删除服务**   
    - service name ----------- [ string ]，服务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，停止并删除服务 wuiserver
    ```shell
    land@service.delete[ wuiserver ]
    ttrue   
    ```

+ `start[ service name, [object], [API], [parameter list,,,] ]` **添加服务**，注册并启动服务，未运行时启动
    - service name ----------- [ string ]，服务名称，可自定义
    - object ----------------- [ string ]，组件名称
    - API -------------------- [ string ]，组件的 API
    - parameter list --------- [ string ]，参数列表
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加 wui@admin.service 运行，将此服务命名为 wuiserver
    ```shell
    land@service.start[ wuiserver, wui@admin, service ]
    ttrue
    ```

    示例，添加 tui@ssh.service 运行，将此服务命名为 sshserver，第一个参数为 192.168.8.1，第二个参数为 23
    ```
    land@service.start[ sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `stop[ service name ]` **停止服务**   
    - service name ----------- [ string ]，服务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，停止服务 wuiserver
    ```shell
    land@service.stop[ wuiserver ]
    ttrue   
    ```

+ `reset[ service name, [object], [API], [parameter list,,,] ]` **重置服务**，注册并启动服务，已存在时重置
    - service name ----------- [ string ]，服务名称，可自定义
    - object ----------------- [ string ]，组件名称
    - API -------------------- [ string ]，组件的 API
    - parameter list --------- [ string ]，参数列表
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加 wui@admin.service 运行，将此服务命名为 wuiserver
    ```shell
    land@service.reset[ wuiserver, wui@admin, service ]
    ttrue
    ```

    示例，添加 tui@ssh.service 运行，将此服务命名为 sshserver，第一个参数为 192.168.8.1，第二个参数为 23
    ```
    land@service.reset[ sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `off[ service name ]` **关闭服务控制**，之后系统在服务退出时不再重启
    - service name ----------- [ string ]，服务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，关闭控制服务 wuiserver
    ```shell
    land@service.off[ wuiserver ]
    ttrue   
    ```

+ `offdel[ service name ]` **关闭服务控制并在服务退出后自动删除**
    - service name ----------- [ string ]，服务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，关闭控制并在服务退出后自动删除 `wuiserver`
    ```shell
    land@service.offdel[ wuiserver ]
    ttrue
    ```

+ `pid[ service name ]` **获取服务 PID**   
    - service name ----------- [ string ]，服务名称
    - 失败返回 NULL
    - 返回描述 PID 的字符串

    示例，获取服务 wuiserver 的 PID
    ```shell
    land@service.pid[ wuiserver ]
    2041   
    ```

+ `info[ service name ]` **获取服务信息**   
    - service name ----------- [ string ]，服务名称
    - 返回描述服务信息的 JSON  
    
    ```json
    // API 返回的 JSON 属性介绍
    {
        "name":"service name",                                         // [ string ]
        "status":"service state",                                      // [ "start", "reset", "off", "stop", "unregister", "finish" ]
        "delay":"delay to run",                                        // [ number ]，单位为微秒
        "obj":"object name",                                           // [ string ]
        "op":"API name",                                               // [ string ]
        "1":"first parameter",                                         // [ string or talk ]
        "2":"second parameter",                                        // [ string or talk ]
        "3":"third parameter",                                         // [ string or talk ]
        "pid":"pid number",                                            // [ number ]
        "last_start":"the system uptime when the service last start"   // [ number ]
    }    
    ```

    示例，获取服务 `wuiserver` 的信息
    ```shell
    land@service.info[ wuiserver ]
    {
        "name":"wuiserver",         # 服务名称为 wuiserver
        "delay":"0",                # 无启动延迟
        "obj":"wui@admin",          # 对象为 wui@admin
        "op":"service",             # 组件 API 为 service
        "1":"/tmp/webpage",         # 第一个参数为 /tmp/webpage
        "pid":"2041",               # 服务进程 ID 为 2041
        "last_start":"130"          # 上次启动时系统运行时间为 130 秒
    }
    ```

+ `dump[ service name ]` **获取服务调试信息**   
    - service name ----------- [ string ]，服务名称
    - 返回描述服务内部调试信息的 JSON

    示例，获取服务 wuiserver 的调试信息
    ```shell
    land@service.dump[ wuiserver ]
    {
        ...
    }
    ```

+ `list[]` **列出所有服务**   
    - 返回描述所有服务信息的 JSON  

    ```json
    // API 返回的 JSON 属性介绍
    {
        "service name":
        {
            "name":"service name",                                         // [ string ]
            "status":"service state",                                      // [ "start", "reset", "off", "stop", "unregister", "finish" ]
            "delay":"delay to run",                                        // [ number ]，单位为微秒
            "obj":"object name",                                           // [ string ]
            "op":"API name",                                               // [ string ]
            "pid":"pid number",                                            // [ number ]
            "last_start":"the system uptime when the service last start"   // [ number ]
        }
        // "...":{ service information }  // 每个服务显示一条属性
    }    
    ```

    示例，列出所有服务
    ```shell
    land@service.list
    {
        "clock@restart":
        {
            "name":"clock@restart",
            "delay":"0",
            "obj":"clock@restart",
            "op":"service",
            "pid":"1745",
            "last_start":"40"
        },
        "tui@telnet":
        {
            "name":"tui@telnet",
            "delay":"0",
            "obj":"tui@telnet",
            "op":"service",
            "pid":"1758",
            "last_start":"40"
        },
        "tui@ssh":
        {
            "name":"tui@ssh",
            "delay":"0",
            "obj":"tui@ssh",
            "op":"service",
            "pid":"1763",
            "last_start":"40"
        },
        "wui@admin":
        {
            "name":"wui@admin",
            "delay":"0",
            "obj":"wui@admin",
            "op":"service",
            "1":"/tmp/webpage",
            "pid":"2041",
            "last_start":"130"
        }
    }
    ```

### 生命周期 API

+ `setup[]` / `shut[]` — **当为 `land@service` 实现时**，启动/停止组件服务或钩子。调度遵循已安装 FPK 的 **init** / **uninit** / **joint** 清单。
### C 代码示例

**调用组件方法**

```c
#include "skin/skin.h"

static void print_service_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `run[ [delay], service name, object, API, [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "run", "0,wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("run", ret);
```

##### `delete[ service name ]`

```c
talk_t ret = scalls("land@service", "delete", "wuiserver");
if (ret != ttrue) print_service_call_error("delete", ret);
```

##### `start[ service name, [object], [API], [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "start", "wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("start", ret);
```

##### `stop[ service name ]`

```c
talk_t ret = scalls("land@service", "stop", "wuiserver");
if (ret != ttrue) print_service_call_error("stop", ret);
```

##### `reset[ service name, [object], [API], [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "reset", "wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("reset", ret);
```

##### `off[ service name ]`

```c
talk_t ret = scalls("land@service", "off", "wuiserver");
if (ret != ttrue) print_service_call_error("off", ret);
```

##### `offdel[ service name ]`

```c
talk_t ret = scalls("land@service", "offdel", "wuiserver");
if (ret != ttrue) print_service_call_error("offdel", ret);
```

##### `pid[ service name ]`

```c
talk_t ret = scalls("land@service", "pid", "wuiserver");
if (ret > tpanic)
{
    printf("pid=%s\n", x2string(ret));
    talk_free(ret);
}
else print_service_call_error("pid", ret);
```

##### `info[ service name ]`

```c
talk_t ret = scalls("land@service", "info", "wuiserver");
if (ret > tpanic)
{
    printf("status=%s obj=%s op=%s\n",
           json_string(ret, "status"),
           json_string(ret, "obj"),
           json_string(ret, "op"));
    talk_free(ret);
}
else print_service_call_error("info", ret);
```

##### `dump[ service name ]`

```c
talk_t ret = scalls("land@service", "dump", "wuiserver");
if (ret > tpanic)
{
    printf("service dump json ready\n");
    talk_free(ret);
}
else print_service_call_error("dump", ret);
```

##### `list[]`

```c
talk_t ret = scall("land@service", "list", NULL);
if (ret > tpanic)
{
    printf("service list json ready\n");
    talk_free(ret);
}
else print_service_call_error("list", ret);
```
