## forward@alg — 应用层网关管理

### 配置 ( `forward@alg` )
```json
// 属性介绍 
{
    "amanda":"amanda ALG 功能",            // [ "disable", "enable" ]
    "ftp":"ftp ALG 功能",                  // [ "disable", "enable" ]
    "h323":"h323 ALG 功能",                // [ "disable", "enable" ]
    "irc":"irc ALG 功能",                  // [ "disable", "enable" ]
    "pptp":"pptp ALG 功能",                // [ "disable", "enable" ]
    "gre":"gre ALG 功能",                  // [ "disable", "enable" ]
    "sip":"sip ALG 功能",                  // [ "disable", "enable" ]
    "rtsp":"rtsp ALG 功能",                // [ "disable", "enable" ]
    "snmp":"snmp ALG 功能",                // [ "disable", "enable" ]
    "tftp":"tftp ALG 功能",                // [ "disable", "enable" ]
    "udplite":"udplite ALG 功能"           // [ "disable", "enable" ]
}
```   

示例, 显示当前所有 ALG 设置
```shell
forward@alg
{
    "amanda":"disable",
    "ftp":"enable",
    "h323":"disable",
    "irc":"disable",
    "pptp":"enable",
    "gre":"enable",
    "rtsp":"enable",
    "sip":"enable",
    "snmp":"disable",
    "tftp":"disable"
}
```   

示例, 禁用 FTP ALG
```shell
forward@alg:ftp=disable
ttrue
```   

示例, 禁用 sip ALG
```shell
forward@alg:sip=disable
ttrue
```   

示例, 启用 h323 ALG
```shell
forward@alg:h323=enable
ttrue
```   

示例, 显示 FTP ALG 设置
```shell
forward@alg:ftp
disable
```   

示例, 同时修改多个属性 (**merge**)
```shell
forward@alg|{"ftp":"enable","sip":"disable"}
ttrue
```

### 组件 API
**可直接调用的** API: 对 **`forward@alg`** 进行标准配置的 get/set/merge 操作 (参见 **配置**)。除以下 **`setup[]` / `shut[]`** 外, 没有单独的操作方法。

### 生命周期 API
+ `setup[]` **从配置加载 ALG 内核辅助模块**, *成功返回 ttrue* — 在默认软件包中按 **`init` → `app` → `forward@alg.setup`** 顺序执行。在 **default** / **parasite** 网络模式下, ALG 将被跳过; 否则根据保存的标志启用/禁用辅助模块 (FTP, SIP, ...)。

+ `shut[]` **卸载 ALG 辅助模块**, *成功返回 ttrue* — 在平台关闭时调用。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_forward_alg(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@alg", "status") == NULL)
        return -1;
    return ssets_string("forward@alg", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@alg", "list", NULL); talk_free if JSON */
```
