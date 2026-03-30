## arch@custom — 产品自定义默认配置

OEM/自定义默认值：主机名和 SSID 自动命名、**在线升级** 存储 URL 凭据、**`land@machine`** 项目启用/禁用映射，以及允许的 **`network@frame`** 工作模式。像任何 HE 对象一样持久化存储；在启动时由 **land** 和 **arch** 代码路径读取使用。宏定义：**`CUSTOM_COM`** → **`arch@custom`**（`core/land/skin/skinhead.h`）。示例文件：[`custom.cfg`](custom.cfg)。

### 配置项 ( `arch@custom` )
```json
// Attributes introduction
{
    "verify":"enable or disable verify step",           // [ "enable", "disable" ]
    "restart_time":"max seconds before restart allowed", // [ number ], used by machine policy
    "upgrade_time":"seconds budget for online upgrade", // [ number ], `arch@firmware` / WUI

    "name_custom":"hostname source",                    // [ "enable", "disable", "sn", "imei" ]
    "name_prefix":"hostname prefix string",
    "name_mackey":"MAC suffix width",                 // e.g. "4" or "8" hex digits

    "nssid_custom":"2.4G SSID auto pattern",
    "nssid_prefix":"2.4G SSID prefix",
    "nssid_mackey":"MAC suffix width",

    "assid_custom":"5 GHz SSID auto pattern",
    "assid_prefix":"5 GHz SSID prefix",
    "assid_mackey":"MAC suffix width",

    "firmware_store":"online upgrade base URL or store id",
    "firmware_store_user":"credentials string for store",

    "project": {
        "land":"enable",
        "clock":"disable"
    },
    "mode": {
        "gateway":"",
        "dgateway":""
    }
}
```

**`project`** 映射表在 **`land@machine.setup`** 过程中用于跳过指定的项目。**`mode`** 映射表非空时将限制 **`network@frame.mode_list`** 的返回结果。

示例，显示所有配置
```shell
arch@custom
{
    "restart_time":"25",                       # 启动 25 秒后允许重启
    "upgrade_time":"30",                       # 在线升级超时为 30 秒
    "name_custom":"enable",                    # 根据 MAC 地址自动生成主机名
    "name_prefix":"SkinOS",                    # 主机名前缀：SkinOS
    "name_mackey":"4",                         # 主机名中使用 MAC 地址末 4 位十六进制数
    "nssid_custom":"enable",                   # 根据 MAC 地址自动生成 2.4G SSID
    "nssid_prefix":"SkinOS",                   # 2.4G SSID 前缀：SkinOS
    "nssid_mackey":"4",                        # SSID 中使用 MAC 地址末 4 位十六进制数
    "project":                                 # 项目启用/禁用映射
    {
        "land":"enable",                           # land 项目已启用
        "wifi":"enable",                           # wifi 项目已启用
        "modem":"enable"                           # modem 项目已启用
    },
    "mode":                                    # 允许的网络工作模式
    {
        "gateway":"",                              # 允许网关模式
        "dgateway":""                              # 允许双网关模式
    }
}
```

示例，设置升级超时时间
```shell
arch@custom:upgrade_time=120
ttrue
```

示例，合并多个字段
```shell
arch@custom|{"verify":"enable","upgrade_time":"90"}
ttrue
```

示例，对 `project` 子树进行合并
```shell
arch@custom:project|{"wifi":"enable","tui":"disable"}
ttrue
```

### C 代码示例
```c
#include "skin/skin.h"

static int example_custom_upgrade_time(void)
{
    char buf[32];
    if (sgets_string(buf, sizeof(buf), "arch@custom", "upgrade_time") == NULL)
        return -1;
    return ssets_string("arch@custom", "90", "upgrade_time") ? 0 : -1;
}
```
