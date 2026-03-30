## arch@pci — PCI 设备扫描与驱动绑定

PCIe 设备发现，匹配到 **`pcidrv@*`** 组件，提供 **`devlist` / `status` / `reset` / `resetd`**，与 **`arch@usb`** 和 **`arch@sdio`** 采用相同的服务模式。实现代码：[`pci/pci.c`](pci/pci.c)。

### 配置项 ( `arch@pci` )
```json
// Attributes introduction
{
    "search_time":"failure count before action",         // [ number ], default 15
    "miss_reset2exit":"tries before giving up",          // [ number ], default 3
    "disappear_reset2reboot":"disconnect events before reboot", // [ number ], default 3
    "mode_reboot":"semicolon-separated network_mode list"      // [ string ], modes that trigger reboot on device loss
}
```
JSON 键与 USB/SDIO 系列一致（`search_time`、`miss_reset2exit`、`disappear_reset2reboot`、`mode_reboot`、厂商/设备匹配表等）。详情请参阅 [`pci/pci.c`](pci/pci.c) 以及产品对应的 ODM **`pci.cfg`**（如有提供）。

示例，显示所有配置
```shell
arch@pci
{
    "search_time":"15",                        # 扫描最多 15 轮后执行操作
    "miss_reset2exit":"3",                     # 重置 3 次后放弃
    "disappear_reset2reboot":"3"               # 设备消失 3 次后重启
}
```

示例，合并策略字段
```shell
arch@pci|{"miss_reset2exit":"2"}
ttrue
```

### 组件 API
+ `devlist[]` **PCI 设备列表 JSON**。

+ `status[]` **上次持久化的绑定快照**。

+ `reset[ object ]` / `resetd[ … ]` **重置流水线**。

### 生命周期 API
+ `setup[]` **启动 PCI `service`**，*成功返回 `ttrue`*
    - 标准 **`odm/rk3568/prj.json`** → **`init` → `device` → `arch@pci.setup`**。

+ `shut[]` **停止 PCI 监控**。


### C 代码示例
```c
#include "skin/skin.h"

static void example_pci_devlist(void)
{
    talk_t ret = scall("arch@pci", "devlist", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
