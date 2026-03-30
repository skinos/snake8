## arch@data — 平台存储、OEM 及出厂数据

挂载 **出厂 / OEM / 配置 / 用户数据** 分区，填充默认配置，并提供 **出厂 EEPROM** 读写以及 **备份 / 恢复 / 恢复默认 / 恢复出厂设置** 流程。

### 配置项 ( `arch@data` )
```json
// Attributes introduction
{
    "language":"system language",               // [ string ], e.g. "en", "cn"
    "model":"product model string",             // [ string ], OEM model identifier
    "macid":"factory MAC address (no colon)",   // [ string ], e.g. "00037F1238FE"
    "mac":"factory MAC address (with colon)"    // [ string ], e.g. "00:03:7F:12:38:FE"
}
```

示例，显示所有配置
```shell
arch@data
{
    "language":"en",                           # 系统语言为英语
    "model":"R8",                              # 产品型号：R8
    "macid":"00037F1238FE",                    # 出厂 MAC 地址（无冒号格式）
    "mac":"00:03:7F:12:38:FE"                  # 出厂 MAC 地址（带冒号格式）
}
```

示例，设置产品型号
```shell
arch@data:model=MYMODEL
ttrue
```

示例，合并配置
```shell
arch@data|{"language":"en","macid":"00037F1238FE"}
ttrue
```

### 组件接口
+ `default[]` **标记配置以便在下次恢复默认时擦除**（启用时遵守 **`land@...` 的 `default` 锁**）。

+ `backup[ [timestamp_label] ]` **将配置归档到备份区域**；可选的标签参数。

+ `restore[ archive_path ]` **从备份恢复**（在代码中验证平台/软件/型号兼容性）。

+ `release[]` / `factory[]` **出厂流程辅助函数**（日志和锁检查详见 [`data/data.c`](data/data.c)）。

+ `restore_default[]` / `current_default[]` **默认镜像管理**。

+ `backup_eeprom[]` **EEPROM / 序列号备份读取**（返回 JSON；适用时需调用 **`talk_free`**）。

### 生命周期接口
+ `setup[]` **创建目录树、挂载 MMC 分区、合并 OEM/出厂/自定义默认配置** — 关键的平台初始化流程（参阅 [`data/data.c`](data/data.c)）。
    - 在默认的 **`odm/rk3568/prj.json`** 中 **未** 列出；在真实镜像中由 **land** / 产品 **`init`** 链调用。

+ `shut[]` **同步并卸载配置及中间分区**。

### C 代码示例
```c
#include "skin/skin.h"

static int example_factory_mac(void)
{
    return ssets_string("arch@data", "00037F1238FE", "macid") ? 0 : -1;
}
```
