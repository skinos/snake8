## arch@sdio — SDIO 总线扫描与 Wi-Fi/蓝牙驱动绑定

与 **`arch@usb` / `arch@pci`** 采用相同的结构模式：受监控的 **`service`**、设备列表、**`devlist` / `status` / `reset` / `resetd`**。根据 **`prj.json` `obj`** 绑定 SDIO 子设备，例如 **`sdiodrv@ap6398`**。实现代码：[`sdio/sdio.c`](sdio/sdio.c)。

### 配置项 ( `arch@sdio` )
```json
// Attributes introduction
{
    "search_time":"failure count before action",         // [ number ], default 15
    "miss_reset2exit":"tries before giving up",          // [ number ], default 3
    "disappear_reset2reboot":"disconnect events before reboot", // [ number ], default 3
    "mode_reboot":"semicolon-separated network_mode list"      // [ string ], modes that trigger reboot on device loss
}
```
产品特定的 JSON 配置（扫描时间、重置策略、匹配映射表）。请参照 [`sdio/sdio.c`](sdio/sdio.c) 中使用的键（`search_time`、`miss_reset2exit`、`disappear_reset2reboot`、`mode_reboot` 等）以及 ODM cfg（如有提供）。

示例，显示所有配置
```shell
arch@sdio
{
    "search_time":"15",                        # 扫描最多 15 轮后执行操作
    "miss_reset2exit":"5",                     # 重置 5 次后放弃
    "disappear_reset2reboot":"3"               # 设备消失 3 次后重启
}
```

示例，合并数值策略
```shell
arch@sdio|{"search_time":"20","miss_reset2exit":"5"}
ttrue
```

### 组件 API
+ `devlist[]` **枚举的 SDIO 设备列表**。

+ `status[]` **持久化的绑定状态**（代码内部路径 -- 与 USB 同系列）。

+ `reset[ object ]` / `resetd[ … ]` **驱动重置辅助接口**。

### 生命周期 API
+ `setup[]` **启动 SDIO `service`**，*成功返回 `ttrue`*
    - 标准 **`odm/rk3568/prj.json`** → **`init` → `device` → `arch@sdio.setup`**。

+ `shut[]` **停止 SDIO 监控**。


### C 代码示例
```c
#include "skin/skin.h"

static void example_sdio_status(void)
{
    talk_t ret = scall("arch@sdio", "status", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
