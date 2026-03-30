## clock@date — 系统日期和时间

管理系统日期和时间，包括时区、手动设置、NTP 客户端同步和状态报告（`clock@date`）。

### 配置 ( `clock@date` )
```json
// Attributes introduction
{
    "timezone":"time zone",                             // [ number ], -12 to 12, West 12 to East 12, support half time zone like 3:30, -3:30
    "ntpclient":"whether to start the NTP client",      // [ "disable", "enable" ]
    "ntpserver":"NTP Server",                           // [ string ]
    "ntpserver2":"NTP Server 2",                        // [ string ]
    "ntpserver3":"NTP Server 3",                        // [ string ]
    "ntpinterval":"NTP Synchronization interval"        // [ number ], interval (in seconds) for time synchronization with the NTP server
}
```   

示例，显示所有配置
```shell
clock@date
{
    "timezone":"8",                   # 时区为东八区，中国
    "ntpclient":"enable",             # 启用 NTP 客户端与 NTP 服务器同步
    "ntpserver":"ntp1.aliyun.com",    # ntp1.aliyun.com, ntp2.aliyun.com, ntp3.aliyun.com, 依次尝试直到成功
    "ntpserver2":"ntp2.aliyun.com",
    "ntpserver3":"ntp3.aliyun.com",
    "ntpinterval":"86400"             # 每 86400 秒同步一次
}
```  
示例，将时区修改为西五区
```shell
clock@date:timezone=-5
ttrue
```  
示例，禁用 NTP 客户端时间同步
```shell
clock@date:ntpclient=disable
ttrue
```  
示例，一次更改多个属性（**合并** — 仅更新列出的字段；**`|`** 加 JSON 对象）
```shell
clock@date|{"timezone":"8","ntpclient":"enable","ntpserver":"pool.ntp.org"}
ttrue
```


### 组件 API
+ `status[]` **获取日期信息**
    - 失败时返回 NULL
    - 错误时返回 terror
    - 成功时返回描述日期信息的 json
    ```json
    // Attributes introduction of json by the API return
    {
        "source":"The source of the time",                // [ "ntp", "set", "lte", "gps" ]
                                                             // ntp: 表示来源于 NTP，NTP 优先级最高，NTP 同步成功会覆盖所有其他时间
                                                             // set: 表示手动设置的时间
                                                             // rtc: 表示来源于 RTC 时间  
                                                             // lte: 表示来源于 LTE 时间 
                                                             // gps: 表示来源于 GPS 时间 
                                                             // 此节点为空或不存在表示尚未设置
        "current":"current date",                         // [ string ], format is hour:minute:second:month:day:year
        "livetime":"system live time",                    // [ string ], format is hour:minute:second:day
        "uptime":"system uptime in second"                // [ number ]
    }    
    ```   
    示例，获取当前日期   
    ```shell
    clock@date.status
    {
        "current":"12:29:41:05:10:2022",         # 当前为 2022 年 5 月 10 日 12:29:41
        "livetime":"00:01:58:0",                 # 系统运行 1 分 58 秒
        "uptime":"118"                           # 系统运行 118 秒
    }
    ```   

+ `current[ [current date], [time zone] ]` **设置当前日期或获取当前时间**
    - [current date] ------ [ string ]，格式为 hour:minute:second:month:day:year
    - [time zone] --------- [ number ]，-12 到 12，西十二区到东十二区，支持半时区如 3:30、-3:30
    - 设置当前日期或时区成功时返回 ttrue
    - 设置当前日期或时区失败时返回 tfalse
    - 无参数且成功时返回描述当前时间的 json
    - 无参数且获取当前时间失败时返回 NULL
    ```json
    {
        "sec":"The number of seconds since 1970.01.01:00:00:00",  // [ number ]
        "usec":"current microsecond",                             // [ number ] 
        "minuteswest":"Minutes west of Greenwich",                // [ number ]
        "dsttime":"type of DST correction"                        // [ number ]
    }
    ```   

    示例，设置当前日期为 2019 年 7 月 8 日 11:12:23   
    ```shell
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```   
    示例，设置时区为中国   
    ```shell
    clock@date.current[ , 8 ]
    ttrue
    ```   
    示例，获取当前时间
    ```shell
    clock@date.current
    {
        "sec":"1747327771",
        "usec":"803417",
        "minuteswest":"-480",
        "dsttime":"0"
    }
    ```   


+ `ntpsync[ [NTP server] ]` **与 NTP 服务器同步时间**
    - [NTP server] ------ [ string ]，NTP 服务器   
    - 成功返回 ttrue
    - 失败返回 tfalse
    - 错误返回 terror

    示例，与 time.window.com 同步时间
    ```shell
    clock@date.ntpsync[ time.window.com ]
    ttrue
    ```   
    示例，与配置中的 NTP 服务器同步时间
    ```shell
    clock@date.ntpsync
    ttrue
    ```


### 生命周期 API
+ `setup[]` **应用已保存的时区和相关启动时状态**，*成功返回 ttrue*
    - 在默认 clock 包的 **`init`** 中 **不会** 自动运行；如果时区必须在其他服务之前应用，请从您的集成中调用 **`setup[]`**。

+ `shut[]` **停止此组件的受监控子进程（NTP 客户端服务）**，*成功返回 ttrue*
    - 在默认集成的 **`uninit`** 中 **不会** 自动运行；如果需要在关机时执行，请显式调用。

### Joint 处理程序
| Joint 键 | 调用 |
|-----------|------|
| `network/online` | `clock@date.online` |

在默认 clock 包中注册为 **joint** 处理程序。当协议栈将 WAN 路径标记为 **在线** 时，如果配置中 **`ntpclient`** = **`enable`**，**`online[]`** 将启动内嵌的 NTP 客户端 **`service`**。


### 已发布的 Joint 事件
+ **`date/modify`** — 当系统时钟被更改时发出。第二个参数是简短的来源标签，例如 **`set`**（手动 `current[...]`）或 **`ntp`**（NTP 同步成功）。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_clock_date(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@date", "timezone") == NULL)
        return -1;
    ok = ssets_string("clock@date", "8", "timezone");
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

/* Example: scall("clock@date", "status", NULL); then talk_free if JSON */
```
