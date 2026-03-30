## arch@usb — USB 设备匹配与调制解调器启动

受监控的 **`service`** 循环：对 USB 设备进行上电/匹配到 **`usbdrv@*`** 驱动，将状态持久化到 **`var/usbdevice`**，当预期设备缺失时可选择**重启/退出**。实现代码：[`usb/usb.c`](usb/usb.c)。匹配表示例：[`odm/rk3568/usb.cfg`](odm/rk3568/usb.cfg)。

### 配置项 ( `arch@usb` )
```json
// Attributes introduction
{
    "status":"disable to skip starting the USB service",   // [ "disable" ], optional
    "search_time":"failure count before reboot/exit",      // [ number ], default 15 in code
    "mode_reboot":"semicolon-separated network_mode list", // default "misp;nmisp;dmisp"
    "miss_reset2exit":"tries before giving up when device set incomplete",  // default 3
    "disappear_reset2reboot":"disconnect events before reboot",             // default 3
    "match": {
        "type":"busid or product-specific",                // [ string ], match method
        "modem@lte": { "fd880000": "" }                    // USB bus-id to driver mapping
    }
}
```

示例，显示所有配置
```shell
arch@usb
{
    "search_time":"15",                        # 扫描最多 15 轮后执行操作
    "miss_reset2exit":"3",                     # 重置 3 次后放弃
    "disappear_reset2reboot":"3",              # 设备消失 3 次后重启
    "mode_reboot":"misp;nmisp;dmisp",          # 仅在这些网络模式下重启
    "match":                                   # USB 设备匹配表
    {
        "type":"busid",                            # 匹配方式：按 USB 总线 ID
        "modem@lte":                               # 第一个 LTE 调制解调器驱动的匹配规则
        {
            "fd880000":"",                             # USB 总线 fd880000
            "fd8c0000":""                              # USB 总线 fd8c0000
        },
        "modem@lte2":                              # 第二个 LTE 调制解调器驱动的匹配规则
        {
            "fd800000":"",                             # USB 总线 fd800000
            "fd840000":""                              # USB 总线 fd840000
        }
    }
}
```

示例，禁用 USB 管理器
```shell
arch@usb:status=disable
ttrue
```

示例，合并匹配表
```shell
arch@usb|{"match":{"type":"busid","modem@lte":{"fd880000":""}}}
ttrue
```

### 组件 API
+ `hublist[]` **USB 集线器拓扑 JSON**。

+ `devlist[]` **当前设备列表 JSON**（VID/PID/总线 ID 等）。

+ `status[]` **上次持久化到 **`var`** 的 `usbdevice` JSON**。

+ `reset[ object ]` **为已绑定的驱动对象生成 `resetd` 子进程**。

+ `resetd[ … ]` **重置路径的内部工作进程**（详见代码）。

### 生命周期 API
+ `setup[]` **启动 `service`（除非 `status` 为 `disable`）**，*成功返回 `ttrue`*
    - 标准 **`odm/rk3568/prj.json`** → **`init` → `bus` → `arch@usb.setup`**。

+ `shut[]` **停止监控**，*成功返回 `ttrue`*
    - 在此示例 **`prj.json`** 的 **`uninit`** 中**未包含**此操作。


### C 代码示例
```c
#include "skin/skin.h"

static void example_usb_list(void)
{
    talk_t ret = scall("arch@usb", "devlist", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
