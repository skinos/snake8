## modem@lte — LTE/NR 基带模块管理
管理 LTE/NR 基带模块服务。

本文档仅描述基带模块侧组件 (`modem@lte`, `modem@lte2`)。
重点关注基带控制、SIM/AT 操作和基带模块运行时状态。
有关端到端拨号、路由和 `network@frame` 集成, 请参阅逻辑 LTE 接口文档 [`../ifname/lte.cn.md`](../ifname/lte.cn.md) (`ifname@lte`, ...)。

### 配置 ( `modem@lte` )
**modem@lte** 是第一个 LTE 基带模块  
**modem@lte2** 是第二个 LTE 基带模块

```json
// 属性说明 
{
    // 基带状态
    // "enable": 随系统自动启动基带模块服务
    // "disable": 保持基带模块服务停止, 直到手动启动
    "status":"系统启动时启动",    // [ "enable", "disable" ]

    // 基带短信功能
    // 当需要基带模块短信收发功能时启用此项
    "sms":"短信状态",                        // [ "disable", "enable" ]

    // 基带 GNSS 功能
    // 当需要基带模块输出 GNSS/NMEA 数据时启用此项
    // 注意: 由外部 NMEA 框架处理, atd 本身不处理
    "gnss":"GNSS 状态",                      // [ "disable", "enable" ]

    // 基带 AT 端口功能
    // 为外部工具暴露基带模块 AT 透传服务
    "atport":"AT 端口状态",                 // [ "disable", "enable" ]

    // 锁定属性
    // lock_nettype: 首选 RAT 锁定策略, 由基带模块驱动程序应用 (atd 本身不处理)
    // lock_imei/lock_imsi:
    //   "enable" -> 学习当前值并将其存储为锁定目标
    //   "<value>" -> 强制精确值匹配
    //   "disable" -> 不进行锁定检查
    "lock_nettype":"网络类型",             // [ "auto", "2g", "3g", "4g", "nsa", "sa" ]
    "lock_imei":"IMEI 锁定功能",          // [ "disable", "enable", "specific imei string" ]
    "lock_imsi":"IMSI 锁定功能",          // [ "disable", "enable", "specific imsi string" ]

    // 自定义 AT 命令
    // custom_set: 在 setup 阶段执行一次
    // custom_watch: 在 watch 阶段周期性执行
    // 响应文本保存在状态 JSON 的 custom_* 结果字段中
    "custom_set":                              // 基带模块 setup 时的自定义 AT 设置列表
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...更多 AT 命令
    },
    "custom_watch":                            // 基带模块 watch 时的自定义 AT 监视列表
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...更多 AT 命令
    },

    // watch 属性
    // 周期性基带模块监视任务的轮询间隔
    "watch_interval":"基带模块监视间隔",  // [ number ], 单位: 秒, 默认为 8

    // 独立工作使用的属性
    // pin/profile/profile_cfg 用于基带模块侧的自主 setup 流程
    "pin":"SIM PIN 码",                      // [ string ]
    "profile":"使用自定义配置文件",            // [ "disable", "enable" ]
    "profile_cfg":                             // 自定义配置文件, 当 "profile" 为 "enable" 时使用
    {
        "dial":"拨号号码",                     // [ number ]
        "cid":"拨号 CID",                         // [ number ], 默认为 1
        "type":"IP 地址类型",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"认证方式",           // [ "pap", "chap", "papchap" ]
        "apn":"APN 名称",                         // [ string ]
        "user":"用户名",                       // [ string ]
        "passwd":"用户密码"                  // [ string ]
    }

}
```

示例: 显示第一个 LTE 基带模块的完整配置
```shell
modem@lte
{
    "gnss":"enable",                   # 启用 GNSS 功能
 
    "custom_set":                      # setup 时先执行 AT+COPS=3,2, 然后执行 AT+CPIN=1234
    {
        "1":"AT+COPS=3,2",
        "2":"AT+CPIN=1234"
    },
    "custom_watch":                    # watch 周期中执行 AT+CPIN 和 ATI
    {
        "2":"ATI"
        "1":"AT+CPIN",
    }
}
```  

示例: 为第一个 LTE 基带模块启用短信功能
```shell
modem@lte:sms=enable
ttrue
```  

### 组件 API
**可直接调用** 的 API, 通过 HE / eline / HTTP `/he` 调用。
**modem@lte** 是第一个 LTE 基带模块
**modem@lte2** 是第二个 LTE 基带模块

+ `status[]` **获取基带模块状态信息**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 JSON 状态对象
    - 这是上层获取的主要健康快照
      (SIM 状态、注册状态、射频质量、运营商等)
    ```json
    // 方法返回的 JSON 属性说明
    {
        "status":"当前状态",        // [ "setup", "register", "up", "idle", "noimei", "noimsi", "reset", "down", "nodevice" ]
                                             // "setup" 基带模块初始化中
                                             // "register" 网络注册中
                                             // "up" 基带模块已就绪, 可进行网络访问
                                             // "idle" 基带模块进入空闲/错误状态
                                             // "reset" 基带模块正在重置
                                             // "down" 基带模块服务已停止
                                             // "nodevice" 未检测到基带模块设备
        "imei":"IMEI 号码",           // [ string ]
        "imsi":"IMSI 号码",           // [ string ]
        "iccid":"ICCID 号码",         // [ number, "nosim", "pin", "puk" ]
                                                // number 为 ICCID
                                                // "nosim" 未检测到 SIM 卡
                                                // "pin" SIM 卡需要 PIN 码
                                                // "puk" SIM PIN 已锁定, 需要 PUK 码
        "mversion":"基带模块版本",     // [ string ]
        "name":"基带模块名称",            // [ string ]
        "plmn":"MCC 和 MNC",           // [ number, "noreg", "unreg", "dereg" ]
                                                // number 为 MCC 和 MNC
                                                // "noreg" 无法注册到运营商
                                                // "unreg" 当前未注册
                                                // "dereg" 运营商拒绝注册
        "nettype":"网络类型",        // 格式因模块而异
                                         // 2G 通常显示 GSM, GPRS, EDGE, CDMA
                                         // 3G 通常显示 WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G 通常显示 LTE, FDD, TDD
        "signal":"信号等级",         // [ "0", "1", "2", "3", "4" ], "0" 为无信号, "1" 为最弱信号, "4" 为最强信号
        "csq":"CSQ 数值",              // [ number ]
        "rssi":"信号强度",       // [ number ], 单位为 dBm
        "rsrp":"RSRP 值",             // 可选, 格式因模块而异
        "rsrq":"RSRQ 值",             // 可选, 格式因模块而异
        "sinr":"SINR 值",             // 可选, 格式因模块而异 
        "band":"当前频段",           // 可选, 格式因模块而异
        "operator":"运营商名称",      // [ string ]
        "na":"5G 网络接入状态"  // 可选, [ "enable" ], 当基带模块设置了 NA/NSA(5G) 网络接入标志时出现
    }
    ```

    示例: 获取第一个 LTE 基带模块的状态
    ```shell
    modem@lte.status
    {
        "imei":"867160040494084",          # IMEI 为 867160040494084
        "imsi":"460015356123463",          # IMSI 为 460015356123463
        "iccid":"89860121801097564807",    # ICCID 为 89860121801097564807
        "csq":"3",                         # CSQ 数值为 3
        "signal":"3",                      # 信号等级为 3
        "status":"up",                     # 基带模块已就绪, 可进行网络连接
        "plmn":"46001",                    # PLMN 为 46001
        "nettype":"WCDMA",                 # 网络类型为 WCDMA
        "rssi":"-107",                     # 信号强度为 -107
        "operator":"ChinaMobile"           # 运营商名称为 ChinaMobile
    }
    ```

+ `tty[ [type] ]` **获取基带模块 TTY 设备**
    - type ----------- [ string ], TTY 类型 
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 指定类型时返回 TTY 路径
    - 成功: 未指定类型时返回 TTY JSON 对象
    - 典型用法:
      - `stty`: AT/状态通道
      - `mtty`: 数据/PPP 或辅助 AT 通道
      - `gtty`: GNSS/NMEA 通道 (如果可用)
    ```json
    // 方法返回的 JSON 属性说明
    {
        "stty":"状态 TTY 设备",             // [ string ]
        "mtty":"杂项 TTY 设备",               // [ string ]
        "gtty":"GNSS TTY 设备"                // [ string ] 
    }
    ```

    示例, 获取第一个 LTE 基带模块的 TTY 列表
    ```shell
    modem@lte.tty
    {
        "stty":"/dev/ttyUSB1",
        "mtty":"/dev/ttyUSB2",
        "gtty":"/dev/ttyUSB3"
    }
    ```

    示例, 获取第一个 LTE 基带模块的状态 TTY
    ```shell
    modem@lte.tty[ stty ]
    /dev/ttyUSB1
    ```

    示例, 获取第一个 LTE 基带模块的 GNSS TTY
    ```shell
    modem@lte.tty[ gtty ]
    /dev/ttyUSB3
    ```

+ `sim[]` **获取 SIM 卡状态**
    - 无 SIM 卡: 返回 tfalse
    - 错误: 返回 terror
    - SIM 卡正常工作: 返回 ttrue
    - SIM 卡需要 PIN: 返回 "pin"
    - SIM 卡需要 PUK: 返回 "puk"
    - 此 API 在拨号前由网络编排器频繁轮询
    
    示例: 获取第一个 LTE 基带模块的 SIM 状态
    ```shell
    modem@lte.sim
    ttrue
    ```

    示例: 获取第二个 LTE 基带模块的 SIM 状态
    ```shell
    modem@lte2.sim
    pin
    ```

+ `pin[ [pin code] ]` **提交 PIN 码以解锁 SIM 卡**
    - pin code ----------- [ string ]
    - 失败: 返回 tfalse
    - 错误: 返回 terror
    - 成功: 返回 ttrue
    - 如果未提供 PIN 码, 则使用基带模块侧已配置的 `pin`
    - 如果 SIM 卡处于 PUK 状态, 操作被拒绝并返回 tfalse
    
    示例: 使用显式 PIN 解锁 SIM 卡
    ```shell
    modem@lte.pin[ 123456 ]
    ttrue
    ```

    示例: 不带参数解锁 SIM 卡 (使用已配置的 `pin`)
    ```shell
    modem@lte2.pin[]
    ttrue
    ```

+ `imei[]` **获取基带模块 IMEI**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 IMEI 字符串
    
    示例, 获取第一个 LTE 基带模块的 IMEI
    ```shell
    modem@lte.imei
    8986032474898527548
    ```

+ `imsi[]` **获取 SIM IMSI**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 IMSI 字符串
    
    示例, 获取第一个 LTE 基带模块的 IMSI
    ```shell
    modem@lte.imsi
    460115664109131
    ```

+ `iccid[]` **获取 SIM ICCID**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 ICCID 字符串
    
    示例, 获取第一个 LTE 基带模块的 ICCID
    ```shell
    modem@lte.iccid
    8986032474898527548
    ```

+ `plmn[]` **获取基带模块网络的 PLMN**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 PLMN 字符串
    
    示例, 获取第一个 LTE 基带模块的 PLMN
    ```shell
    modem@lte.plmn
    46011
    ```

+ `signal[]` **获取基带模块网络的信号**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回信号等级数值, 0-4, 0 表示无信号, 4 表示最强信号
    - 注意: 当信号为 0 时, 返回 NULL 而非 0
    
    示例, 获取第一个 LTE 基带模块的信号
    ```shell
    modem@lte.signal
    2
    ```

+ `operator[]` **获取基带模块当前运营商配置文件**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回运营商配置文件 JSON
    - 当未显式提供 profile_cfg 时, 上层使用此作为默认 APN/配置文件
    ```json
    // 方法返回的 JSON 属性说明
    {
        "name":"运营商名称",               // [ string ]
        "dial":"*拨号号码",                // [ string ]
        "apn":"APN 代码"                      // [ string ]
    }
    ```

    示例, 获取第一个 LTE 基带模块的配置文件
    ```shell
    modem@lte.operator
    {
        "name":"中国联通",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ```


+ `at[ at command ]` **执行原始 AT 命令**
    - at command ----------- [ string ]
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回基带模块 AT 响应字符串
    - 这是一个透传/调试 API, 建议用于诊断而非正常控制流程
    
    示例: 使用原始 AT 命令查询 SIM 状态
    ```shell
    modem@lte.at[at+cpin?]
    +CPIN: READY

    OK
    ```

    示例: 使用原始 AT 命令查询网络注册状态
    ```shell
    modem@lte2.at[at+creg?]
    +CREG: 2,1,"A538","1EB3FB7",2

    OK
    ```

+ `reset[]` **重置基带模块**
    - 失败: 返回 NULL
    - 错误: 返回 terror
    - 成功: 返回 ttrue
    - 这将触发基带模块重置工作流和服务重启序列
    
    示例, 重置第一个 LTE 基带模块
    ```shell
    modem@lte.reset
    ttrue
    ```

    示例, 重置第二个 LTE 基带模块
    ```shell
    modem@lte2.reset
    ttrue
    ```

+ `reset_clear[]` **清除基带模块重置计数器**
    - 成功: 返回 ttrue
    - 清除内部的 reset_times 和 reset_uptime 计数器
    - 在手动干预后使用, 以防止不必要的升级
    
    示例, 清除第一个 LTE 基带模块的重置计数器
    ```shell
    modem@lte.reset_clear
    ttrue
    ```

+ `netdev[]` **获取基带模块网络设备名**
    - 失败: 返回 NULL
    - 成功: 返回网络设备名字符串 (例如 "usb0", "wwan0")
    
    示例, 获取第一个 LTE 基带模块的网络设备
    ```shell
    modem@lte.netdev
    usb0
    ```

+ `custom_set[]` **获取 custom_set AT 命令执行结果**
    - 失败: 返回 NULL
    - 成功: 返回包含每个 custom_set AT 命令响应的 JSON 对象
    - 键与 `custom_set` 配置中的键一致
    
    示例, 获取第一个 LTE 基带模块的 custom_set 结果
    ```shell
    modem@lte.custom_set
    {
        "1":"OK",
        "2":"+CPIN: READY OK"
    }
    ```

+ `custom_watch[]` **获取 custom_watch AT 命令执行结果**
    - 失败: 返回 NULL
    - 成功: 返回包含每个 custom_watch AT 命令响应的 JSON 对象
    - 键与 `custom_watch` 配置中的键一致
    
    示例, 获取第一个 LTE 基带模块的 custom_watch 结果
    ```shell
    modem@lte.custom_watch
    {
        "1":"+CPIN: READY OK",
        "2":"Quectel EC200T OK"
    }
    ```

+ `lock_imei[ [value] ]` **获取或设置 IMEI 锁定目标值**
    - 无参数: 返回当前存储的锁定 IMEI 字符串, 如未设置则返回 NULL
    - 带参数: 设置锁定 IMEI 值, 成功返回 ttrue, 失败返回 tfalse
    
    示例, 获取第一个 LTE 基带模块的锁定 IMEI
    ```shell
    modem@lte.lock_imei
    867160040494084
    ```

    示例, 设置第一个 LTE 基带模块的锁定 IMEI
    ```shell
    modem@lte.lock_imei[ 867160040494084 ]
    ttrue
    ```

+ `lock_imsi[ [value] ]` **获取或设置 IMSI 锁定目标值**
    - 无参数: 返回当前存储的锁定 IMSI 字符串, 如未设置则返回 NULL
    - 带参数: 设置锁定 IMSI 值, 成功返回 ttrue, 失败返回 tfalse
    
    示例, 获取第一个 LTE 基带模块的锁定 IMSI
    ```shell
    modem@lte.lock_imsi
    460015356123463
    ```

    示例, 设置第一个 LTE 基带模块的锁定 IMSI
    ```shell
    modem@lte.lock_imsi[ 460015356123463 ]
    ttrue
    ```

+ `order[ command, [value] ]` **执行通用驱动命令**
    - command ----------- [ string ], 驱动命令名称
    - value ------------- [ JSON ], 传递给驱动的可选参数
    - 失败: 返回 NULL
    - 成功: 返回驱动响应
    - 这是一个透传 API, 将任意命令转发到基带模块驱动层
    
    示例, 执行驱动特定命令
    ```shell
    modem@lte.order[ nettype ]
    LTE
    ```

+ `sms_send[ number, message ]` **发送短信**
    - 需要 `sms` 配置为 "enable"
    - 失败: 返回 tfalse
    - 成功: 返回 ttrue
    - 请求被转发到短信服务对象
    
    示例, 发送短信
    ```shell
    modem@lte.sms_send[ 10086, hello ]
    ttrue
    ```

+ `sms_list[]` **列出已接收的短信**
    - 需要 `sms` 配置为 "enable"
    - 失败: 返回 tfalse
    - 成功: 返回短信列表 JSON
    - 请求被转发到短信服务对象

+ `sms_delete[ index ]` **删除短信**
    - 需要 `sms` 配置为 "enable"
    - index ----------- 要删除的短信索引
    - 失败: 返回 tfalse
    - 成功: 返回 ttrue
    - 请求被转发到短信服务对象


#### 按基带模块状态的 API 可用性
并非所有 API 在每种基带模块状态下都可用。以下表格显示了每个 API 因状态限制而返回错误/NULL 的情况:

| API | 不可用状态 (返回 NULL 或 terror) |
|---|---|
| `status` | 始终可用 (在所有状态下返回特定状态的 JSON) |
| `tty` | 始终可用 (直接从注册表读取) |
| `imei`, `imsi`, `iccid`, `operator` | nodevice, down, reset |
| `sim`, `pin` | nodevice, down, reset, idle, noimei, noimsi |
| `plmn`, `signal` | nodevice, down, reset, idle, noimei, noimsi |
| `at` | nodevice, down, reset |
| `reset` | 需要 devbus 存在 |
| `custom_set`, `custom_watch` | nodevice, down, reset |
| `order` | nodevice, down, reset |
| `lock_imei`, `lock_imsi` | 始终可用 |
| `reset_clear` | 始终可用 |
| `netdev` | 始终可用 (直接从注册表读取) |
| `sms_send`, `sms_list`, `sms_delete` | 需要 `sms` 为 "enable" 且 sms_object 存在 |

### 生命周期 API
+ `setup[]` / `shut[]` -- 由 **`usbdrv@...`** / 基带模块驱动注册驱动; **未**列在默认基带模块包的 **`init`** 表中。
+ 请参阅基带模块驱动组件文档 (**`modem@ec2x`**, **`modem@rm500u`**, ...) 了解启动方式。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_modem_lte(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "modem@lte", "status") == NULL)
        return -1;
    return ssets_string("modem@lte", "enable", "status") ? 0 : -1;
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

/* e.g. scall("modem@lte", "list", NULL); talk_free if JSON */
```
