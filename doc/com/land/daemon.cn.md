## land@daemon — 守护进程与看门狗

**daemon** 可执行程序是网关的主要监控进程。
它负责喂硬件看门狗、监控内存使用、检查本地网络接口，
并定期扫描已注册的服务——重启任何意外退出的服务。所有行为通过
下面描述的 `land@daemon` 配置对象控制。

### 配置 ( `land@daemon` )

`land@daemon` 的**持久化配置对象**（看门狗、内存、本地链路检查、服务扫描间隔）。由 **`daemon`** 可执行程序在运行时读取。

```json
// 属性介绍
{
    "service_check":"service scan interval",                           // [ number ]，秒
    "watchdog_file":"watchdog device file path",                       // [ string ]，文件路径（例如 /dev/watchdog）
    "watchdog_interval":"watchdog feed interval",                      // [ number ]，微秒

    "memory_check":"memory check interval",                            // [ number ]，秒
    "memory_warn":"warn threshold of free memory",                     // [ number ]，kB
    "memory_reboot":"reboot threshold of free memory",                 // [ number ]，kB

    "local_check":"network local check interval",                      // [ number ]，秒
    "local_disbuild":"reboot threshold when local is missing at setup",// [ number ]，次数
    "local_disappear":"reboot threshold when local disappears",        // [ number ]，次数
    "local_ifname":"object name that provides netdev API"              // [ string ]，例如 "ifname@local"
}
```

示例，显示所有配置
```shell
land@daemon
{
    "service_check":"5",                       # 每 5 秒扫描一次已注册的服务
    "watchdog_file":"/dev/watchdog",           # 硬件看门狗设备路径
    "watchdog_interval":"1000000",             # 每 1000000 微秒（1 秒）喂一次看门狗
    "memory_check":"10",                       # 每 10 秒检查一次空闲内存
    "memory_warn":"3000",                      # 空闲内存低于 3000 kB 时发出警告
    "memory_reboot":"800",                     # 空闲内存低于 800 kB 时重启
    "local_check":"30",                        # 每 30 秒检查一次本地网络接口
    "local_disbuild":"20",                     # 启动时本地接口连续缺失 20 次后重启
    "local_disappear":"10",                    # 本地接口连续消失 10 次后重启
    "local_ifname":"ifname@local"              # 本地网络接口对象名称
}
```

示例，一次合并多个调优字段
```shell
land@daemon|{"service_check":"5","watchdog_interval":"1000000"}
ttrue
```

#### 注意事项
- `watchdog_interval` 以微秒为单位解析，然后转换为 `tv_sec/tv_usec`。
- `memory_warn` 和 `memory_reboot` 与空闲内存（kB）进行比较。
- `local_disbuild` 和 `local_disappear` 是重试次数，不是时间长度。
- 如果配置中未设置 `watchdog_file`，daemon 会尝试注册变量 `watchdog_file`。

#### 配置示例
仅启用服务检查：
```json
{
    "service_check":"5"
}
```

启用每 1 秒喂一次看门狗：
```json
{
    "watchdog_file":"/dev/watchdog",
    "watchdog_interval":"1000000"
}
```

启用内存和本地看门狗策略：
```json
{
    "memory_check":"10",
    "memory_warn":"3000",
    "memory_reboot":"800",
    "local_check":"30",
    "local_disbuild":"20",
    "local_disappear":"10",
    "local_ifname":"ifname@local"
}
```

### 可执行命令

**`daemon`** 程序（不是长期运行会话上的 HE 方法）接受以下控制命令：

立即停止 daemon：
```shell
daemon exit
```

15 秒后停止 daemon：
```shell
daemon stop15exit
```

600 秒后停止 daemon：
```shell
daemon delay600exit
```

显示 flash ID：
```shell
daemon flashid
```

### C 代码示例

**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_land_daemon(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "land@daemon", "status") == NULL)
        return -1;
    return ssets_string("land@daemon", "enable", "status") ? 0 : -1;
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

/* 例如 scall("land@daemon", "list", NULL); 如果返回 JSON 则需 talk_free */
```
