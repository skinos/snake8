## clock@ntps — NTP 服务器管理
管理 NTP 服务器

### 配置 ( `clock@ntps` )
```json
// attribute introduction
{
    "status":"NTP server status",      // [ "disable", "enable"]
    "local":"local network interface"  // [ "ifname@lan", "ifname@lan2", ... ], network ifname name for NTP server to bind, auto-detect if not set
}
```

示例，显示配置
```shell
clock@ntps
{
    "status":"enable"           # NTP 服务器已启用
}
```
示例，禁用 NTP 服务器
```shell
clock@ntps:status=disable
ttrue
```

示例，一次更改多个属性（**合并**）
```shell
clock@ntps|{"status":"enable","local":"ifname@lan"}
ttrue
```

### 生命周期 API
+ `setup[]` **根据已保存的配置启动或跳过 NTP 服务器**，*成功返回 ttrue*
    - 安装后，**`init`** 通常在 **`general`** 阶段运行 **`clock@ntps.setup`**。如果 **`status`** 为 **`enable`**，则启动运行 **`ntpd`** 的 **`service`** 子进程，绑定到 **`local`**（或自动检测的 **`local_netdev`**）。

+ `shut[]` **停止此组件的 NTP 服务器监控**，*成功返回 ttrue*
    - **`sdelete( COM_IDPATH )`**。在默认集成的 **`uninit`** 中 **不会** 自动运行；如果需要在关机时执行，请显式调用。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_clock_ntps(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@ntps", "status") == NULL)
        return -1;
    ok = ssets_string("clock@ntps", "enable", "status");
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

/* Example: scall("clock@ntps", "setup", NULL); */
```
