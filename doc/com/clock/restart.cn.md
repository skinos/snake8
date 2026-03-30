## clock@restart — 计划系统重启

管理基于运行时长、时钟时间或空闲/客户端条件的自动系统重启（`clock@restart`）。

### 配置 ( `clock@restart` )
```json
// attribute introduction
{
    "mode":"Restart plan mode",                        // [ "disable", "age", "point", "idle" ]
                                                         // "disable": disable restart auto
                                                         // "age": maximum runtime to restart
                                                         // "point": fixed-point to restart
                                                         // "idle": idle specified period to restart
    "delay":"delay some second to work",               // [ number ], The unit is seconds, valid for all mode

    "age":"The maximum runtime",                               // [ number ], The unit is seconds, valid for "mode" is "age"

    "point_age":"The maximum runtime",                         // [ number ], valid for "mode" is "point"
    "point_hour":"Specifies when hour to restart",             // [ number ], valid for "mode" is "point"
    "point_minute":"Specifies when minute of hour to restart", // [ number ], valid for "mode" is "point"

    "idle_start":"Start runtime",                              // [ number ], valid for "mode" is "idle"
    "idle_time":"Idle time",                                   // [ number ], valid for "mode" is "idle"
    "idle_hour":"Specifies when hour to restart",              // [ number ], valid for "mode" is "idle"
    "idle_minute":"Specifies when minute of hour to restart",  // [ number ], valid for "mode" is "idle"
    "idle_age":"The maximum runtime"                           // [ number ], valid for "mode" is "idle"
}
```   

示例，显示 age 模式的配置，系统运行 2880 秒后将自动重启
```shell
clock@restart
{
    "mode":"age",
    "age":"2880"
}
```   

示例，显示 point 模式的配置，系统将在 23:45 重启
```shell
clock@restart
{
    "mode":"point",
    "point_hour":"23",
    "point_minute":"45",
    "point_age":"2880"
}
```

示例，禁用重启功能
```shell
clock@restart:mode=disable
ttrue
```   

示例：设置系统运行 3600 秒后自动重启
```shell
clock@restart|{"mode":"age","age":"3600"}
ttrue
```   

示例：设置在 03:30 自动重启
```shell
clock@restart|{"mode":"point","point_hour":"03","point_minute":"30"}
ttrue
```


### 生命周期 API
+ `setup[]` **当模式处于活动状态时启动重启计划服务**，*成功返回 ttrue*
    - 安装后，**`init`** 通常在 **`app`** 阶段运行 **`clock@restart.setup`**。对于 **`mode`** 为 **`age`**、**`point`** 或 **`idle`** 时，启动 **`service`** 子进程；**`disable`** 则跳过服务。

+ `shut[]` **停止重启监控**，*成功返回 ttrue*
    - **`sdelete( COM_IDPATH )`**。在默认集成的 **`uninit`** 中 **不会** 自动运行。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_clock_restart(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@restart", "mode") == NULL)
        return -1;
    ok = ssets_string("clock@restart", "disable", "mode");
    return ok ? 0 : -1;
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

/* Example: scall("clock@restart", "setup", NULL); — usually only from init */
```
