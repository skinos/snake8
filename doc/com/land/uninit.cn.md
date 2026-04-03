## land@uninit — 关机任务

管理设备关机任务。
每个 FPK 可以通过其附带的清单注册关机任务；系统在配置的 **uninit** 级别运行这些任务。

系统关机过程中有多个关机级别：
    - `delay5`     延迟关机
    - `delay4`     延迟关机
    - `delay3`     延迟关机
    - `delay2`     延迟关机
    - `delay`      延迟关机
    - `general`    通用应用关机
    - `app2`       应用2关机
    - `app`        应用关机
    - `extern`     互联网连接关机
    - `local`      本地接口关机
    - `manage`     skinos 管理框架关机
    - `network`    skinos 网络关机
    - `device`     skinos 设备关机
    - `bus`        skinos 总线关机
    - `land`       skinos land 层关机
    - `arch`       skinos 架构层关机

### 配置 ( `land@uninit` )

`land@uninit` 的**持久化配置对象**（关机任务**列表**和可选的**远程**目标）。与 `land@init` 具有相同的缓存文件行为。

```json
// 属性介绍（与 land@init 相同的布局：list + 可选 remote）
{
    "list":
    {
        "task name":                             // [ string ]，可自定义名称
        {
            "level":"shutdown levels",                          // [ string ]，关机级别
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
                                                                // "delay"
                                                                // "delay2"
                                                                // "delay3"
                                                                // "delay4"
                                                                // "delay5"
            "call":"specify component API or program"           // [ string ]
        }
        // "...":{ ... }     有多少个关机任务就显示多少条属性
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
        "tuishutdown":                               // 在 app 关机级别调用 tui@telnet.shut
        {
            "level":"app",
            "call":"tui@telnet.shut"
        },
        "sshshutdown":                               // 在 general 关机级别调用 tui@ssh.shut
        {
            "level":"general",
            "call":"tui@ssh.shut"
        }
    }
}
```  

示例，仅合并 **remote**
```shell
land@uninit|{"remote":{"ip":"","port":"515"}}
ttrue
```

### 组件 API

+ `register[ [shutdown level], call ]` **注册一个关机任务，重启后丢失**  
    - shutdown level ------- [ string ]，默认为 "general"
    - call ----------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，在 app 关机级别注册调用 wui@admin.shut
    ```shell
    land@uninit.register[ app, wui@admin.shut ]
    ttrue
    ```
    示例，在默认（general）关机级别注册调用 tui@ssh.shut
    ```
    land@uninit.register[ ,tui@ssh.shut ]
    ttrue    
    ```   

+ `unregister[ [shutdown level], call ]` **删除一个关机任务**  
    - shutdown level ------- [ string ]，默认为 "general"
    - call ----------------- [ string ]，组件 API 或程序
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除 app 关机级别的 tui@ssh.shut 调用
    ```shell
    land@uninit.unregister[ app, tui@ssh.shut ]
    ttrue
    ```

+ `list[ [shutdown levels] ]` **列出关机任务**  
    - shutdown level ----------- [ string ]，默认列出所有已注册的关机任务
    - 返回描述关机任务的 JSON   
    ```json
    // 方法返回的 JSON 属性介绍
    {
        "shutdown level":                 // [ string ]:{},  关机级别
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     该级别有多少个调用就显示多少条属性
        }
        // "...":{ ... }     有多少个关机级别就显示多少条属性
    }    
    ```  

    示例，显示所有关机任务
    ```shell
    land@uninit.list
    {
        "app2":                         // 在关机级别 app2 调用 agent@local.shut
        {
            "agent@local.shut":""
        },
        "app":                          // 在关机级别 app 调用 client@station.shut/clock@restart.shut/forward@alg.shut
        {
            "client@station.shut":"",  
            "clock@restart.shut":"",   
            "forward@alg.shut":""      
        },
        "manage":                       // 在关机级别 manage 调用 tui@telnet.shut 和 tui@ssh.shut
        {
            "tui@telnet.shut":"",      
            "tui@ssh.shut":""          
        }
        // ... 更多级别
    }
    ```

    示例，显示 land 关机级别的任务
    ```shell
    land@uninit.list[ land ]
    {
        "land@auth.shut":"",
        "network@hosts.shut":""
    }   
    ```


+ `add[ task name, call, [shutdown level] ]` **添加一个关机任务**
    - task name ------------ [ string ]，任务名称，可自定义
    - call ------------------ [ string ]，组件 API 或程序
    - shutdown level -------- [ string ]，默认为 "general"
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加名为 webshut 的任务，在 app 关机级别调用 wui@admin.shut
    ```shell
    land@uninit.add[ webshut, wui@admin.shut, app ]
    ttrue
    ```

    示例，添加名为 sshshut 的任务，在 general 关机级别调用 tui@ssh.shut
    ```
    land@uninit.add[ sshshut, tui@ssh.shut ]
    ttrue    
    ```

+ `delete[ task name ]` **删除一个关机任务**
    - task name ---- [ string ]，任务名称
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除 webshut
    ```shell
    land@uninit.delete[ webshut ]
    ttrue
    ```

+ `call[ shutdown level, [parameter] ]` **执行指定关机级别的所有关机任务**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - shutdown level ----------- [ string ]，要执行的关机级别（例如 "app"、"general"、"network"）
    - parameter ------- [ talk_t ]，传递给任务的可选参数
    - 此方法在系统关机过程中由系统调用
    - 它执行指定级别的所有已注册任务，如果配置了远程日志则发送 UDP 通知
    - 不建议手动调用


### 生命周期 API


+ `setup[]` **初始化 uninit 组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统启动期间自动调用的生命周期方法
    - 它从配置中注册所有关机任务，并在配置了远程日志时进行设置
    - 不建议手动调用

**说明：** **`land@uninit`** 与 **`init`** / **`joint`** 共用 **`land@init`** 实现，**无 **`shut[]`** 导出。

### C 代码示例

```c
#include "skin/skin.h"

static void example_land_uninit(void)
{
    talk_t ret = scall("land@uninit", "setup", NULL);
    (void)ret;
}
```
