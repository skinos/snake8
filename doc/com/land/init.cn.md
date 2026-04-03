## land@init — 启动任务

管理设备初始化任务。
每个 FPK 可以通过其附带的清单注册启动任务；系统在配置的 **init** 级别运行这些任务。

系统启动时有多个启动级别：
    - `arch`       skinos 架构层启动
    - `land`       skinos land 层启动
    - `bus`        skinos 总线启动
    - `device`     skinos 设备启动
    - `network`    skinos 网络启动
    - `manage`     skinos 管理框架启动
    - `local`      本地接口启动
    - `extern`     互联网连接启动
    - `app`        应用启动
    - `app2`       应用2启动
    - `general`    通用应用启动
    - `delay`      应用延迟 1 秒启动
    - `delay2`     应用延迟 2 秒启动
    - `delay3`     应用延迟 3 秒启动
    - `delay4`     应用延迟 4 秒启动
    - `delay5`     应用延迟 5 秒启动

*不建议普通应用开发者注册 extern 及更早的启动级别*


### 配置 ( `land@init` )

`land@init` 的**持久化配置对象**（启动任务**列表**和可选的**远程**通知目标）。`register` / `unregister` / `list` 还使用每个对象的**缓存文件**；详见下方说明。

```json
// 属性介绍
{
    "list":
    {
        "task name":                             // [ string ]，可自定义名称
        {
            "level":"boot levels",                          // [ string ]，启动级别
                                                                    // "arch"
                                                                    // "land"
                                                                    // "bus"
                                                                    // "device"
                                                                    // "network"
                                                                    // "manage"
                                                                    // "local"
                                                                    // "extern"
                                                                    // "app"
                                                                    // "app2"
                                                                    // "general"
                                                                    // "delay"
                                                                    // "delay2"
                                                                    // "delay3"
                                                                    // "delay4"
                                                                    // "delay5"
            "call":"component API or program"               // [ string ]
        }
        // "...":{ ... }     有多少个启动任务就显示多少条属性
    },
    "remote":                                          // 可选；在 setup 时读取，用作 call 使用的 UDP 通知目标
    {
        "ip":"remote ip address",                      // [ string ]，为空则清除远程目标
        "port": "udp port"                             // [ number ]，缺失或非正数时默认为 515
    }
}
// 示例
{
    "list":
    {
        "tuisetup":                               // 在 app 启动级别调用 tui@telnet.setup
        {
            "level":"app",
            "call":"tui@telnet.setup"
        },
        "webreset":                               // 在 general 启动级别调用 wui@admin.reset
        {
            "level":"general",
            "call":"wui@admin.reset"
        }
    },
    "remote":
    {
        "ip":"192.168.1.100",
        "port": "515"
    }
}
```  

示例，合并 **remote** 和一个 **list** 条目（仅修改列出的键）
```shell
land@init|{"remote":{"ip":"192.168.1.50","port":"515"}}
ttrue
```

`register` / `unregister` / `list` 使用每个对象的**缓存文件**（与上述级别映射相同的布局）。`add` / `delete` 修改配置中的持久化 **`list`**；这些条目在 **setup** 运行时（通常在启动时）应用到缓存中。在此之前，`list` 仍然只反映缓存中已有的内容。


### 组件 API

+ `register[ [boot level], call ]` **注册一个启动任务，重启后丢失**  
    - boot level ----------- [ string ]，默认为 "general"
    - call ----------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，在 app 启动级别注册调用 wui@admin.reset
    ```shell
    land@init.register[ app, wui@admin.reset ]
    ttrue
    ```
    示例，在默认（general）启动级别注册调用 tui@ssh.setup
    ```shell
    land@init.register[ ,tui@ssh.setup ]
    ttrue    
    ```   

+ `unregister[ [boot level], call ]` **删除一个启动任务**  
    - boot level ----------- [ string ]，默认为 "general"
    - call ----------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除 app 启动级别的 tui@ssh.setup 调用
    ```shell
    land@init.unregister[ app, tui@ssh.setup ]
    ttrue
    ```

+ `list[ [boot level] ]` **列出启动任务**  
    - boot level ----------- [ string ]，省略或为空则列出所有级别；否则只返回该级别
    - 返回描述启动任务的 JSON
    ```json
    // 方法返回的 JSON 属性介绍
    {
        "boot level":                  // [ string ]:{},  启动级别
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     该级别有多少个调用就显示多少条属性
        }
        // "...":{ ... }     有多少个启动级别就显示多少条属性
    }    
    ```  

    示例，显示所有启动任务
    ```shell
    land@init.list
    {
        "app2":                         // 在启动级别 app2 调用 agent@local.setup
        {
            "agent@local.setup":""
        },
        "app":                          // 在启动级别 app 调用 client@station.setup/clock@restart.setup/forward@alg.setup
        {
            "client@station.setup":"",  
            "clock@restart.setup":"",   
            "forward@alg.setup":""      
        },
        "manage":                       // 在启动级别 manage 调用 tui@telnet.setup 和 tui@ssh.setup
        {
            "tui@telnet.setup":"",      
            "tui@ssh.setup":""          
        }
        // ... 更多启动级别
    }
    ```

    示例，显示 land 启动级别的任务
    ```shell
    land@init.list[ land ]
    {
        "land@auth.setup":"",
        "land@joint.setup":"",
        "land@init.setup":"",
        "land@uninit.setup":"",
        "network@hosts.setup":""
    }   
    ```

+ `add[ task name, call, [boot level] ]` **添加一个启动任务**
    - task name ------------ [ string ]，任务名称，可自定义
    - call ----------------- [ string ]，组件 API 或程序
    - boot level ----------- [ string ]，默认为 "general"
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加名为 websetup 的任务，在 app 启动级别调用 wui@admin.setup
    ```shell
    land@init.add[ websetup, wui@admin.setup, app ]
    ttrue
    ```

    示例，添加名为 sshsetup 的任务，在 general 启动级别调用 tui@ssh.setup
    ```shell
    land@init.add[ sshsetup, tui@ssh.setup ]
    ttrue    
    ```

+ `delete[ task name ]` **删除一个启动任务**
    - task name ---- [ string ]，任务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除名为 sshsetup 的任务
    ```shell
    land@init.delete[ sshsetup ]
    ttrue
    ```

+ `call[ level, [parameter] ]` **执行指定启动级别的所有启动任务**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - level ----------- [ string ]，要执行的启动级别（例如 "app"、"general"、"network"）
    - parameter ------- [ talk_t ]，传递给任务的可选参数
    - 此方法在启动过程中由系统调用
    - 它执行指定级别的所有已注册任务，如果配置了远程日志则发送 UDP 通知
    - 不建议手动调用


### 生命周期 API


+ `setup[]` **初始化 init 组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统启动期间自动调用的生命周期方法
    - 它从配置中注册所有启动任务，并在配置了远程日志时进行设置
    - 不建议手动调用

**说明：** land 的 **`init`** 与 **`joint`** / **`uninit`** 共用同一 **`land@init`** 实现，**未导出 **`shut[]`**；本仓库中带 **`_shut`** 的是 **`land@syslog`**。

### C 代码示例

```c
#include "skin/skin.h"

static void example_land_init(void)
{
    talk_t ret = scall("land@init", "setup", NULL);
    (void)ret;
}
```
