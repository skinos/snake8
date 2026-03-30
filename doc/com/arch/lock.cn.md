## arch@lock — 功能锁

全局 **安全开关**，**`arch@data`**、**`arch@firmware`** 和 **`land@machine`** 通过 **`config_sget( LOCK_COM, … )`** 查询。宏 **`LOCK_COM`** → **`arch@lock`**。

### 配置 ( `arch@lock` )
```json
// Attributes introduction
{
    "config":"disable or enable global config lock",     // [ "disable", "enable" ] — enable blocks configure operations where enforced
    "upgrade":"disable or enable upgrade lock",          // [ "disable", "enable" ] — blocks `arch@firmware` entry points
    "default":"disable or enable factory-default lock",  // [ "disable", "enable" ] — blocks `arch@data` default / release / factory
    "backup":"disable or enable backup lock",            // [ "disable", "enable" ]
    "restore":"disable or enable restore lock"           // [ "disable", "enable" ] — restore and restore_default on `arch@data`
}
```

示例，显示所有配置
```shell
arch@lock
{
    "config":"disable",                        # 全局配置锁：关闭
    "upgrade":"disable",                       # 升级锁：关闭
    "default":"disable",                       # 恢复出厂设置锁：关闭
    "backup":"disable",                        # 备份锁：关闭
    "restore":"disable"                        # 还原锁：关闭
}
```

示例，启用升级锁
```shell
arch@lock:upgrade=enable
ttrue
```

示例，合并
```shell
arch@lock|{"upgrade":"enable","backup":"enable"}
ttrue
```

### C 代码示例
```c
#include "skin/skin.h"

static boole upgrade_locked(void)
{
    char buf[16];
    const char *p = sgets_string(buf, sizeof(buf), "arch@lock", "upgrade");
    return (p != NULL && strcmp(p, "enable") == 0);
}
```
