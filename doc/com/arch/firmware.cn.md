## arch@firmware — 固件升级

Flash / FPK 安装、**`.zz` 压缩包** 方式升级、TFTP 以及 **在线** 升级检查。遵循 **`land@…`**（或锁组件）的 **`upgrade`** 锁。在长时间操作期间发送 **`machine/status`** joint 事件。实现代码：[`firmware/firmware.c`](firmware/firmware.c)。

### 配置 ( `arch@firmware` )
在默认源码中 **`arch@firmware`** **没有** 独立的持久化 JSON 对象——行为由 **方法参数**、**注册值** 以及 [`firmware/firmware.c`](firmware/firmware.c) 内部的 **`LOCK_COM` / `CUSTOM_COM`** 读取驱动。

*（如果您的产品添加了 cfg 文件并关联了 `config_get`，请在此处记录。）*

### 组件 API
+ `fpk[ filename, offset, size ]` **通过 `FPK_COM` `install` 安装 FPK**，可选切片（`offset`/`size` 默认为整个文件）。

+ `zz[ … ]` **多镜像压缩包升级**，解压到 `/tmp/.zztar`，返回每个文件的结果 JSON（完整参数列表和 **`msgs`** 结构详见代码）。

+ `tftp_upgrade[ … ]` **基于 TFTP 的刷写路径**，针对已定义的分区名称。

+ `online_check[]` **查询远程清单**，成功时返回包含 **`version`**、**`url`**、**`changelog`** 等信息的 JSON。

+ `online_upgrade[]` **下载并应用** 远程升级（设置注册值 **`machine_state`** 阶段；使用 **`joint_calls("machine/status", NULL)`**）。


### 发布的 Joint 事件
+ **`machine/status`** — 在在线升级 / zz 方式升级期间，当 **`register_value_set`** 更新 **`machine_state`**（例如 **`downloading`**、**`upgrading`**、**`succeed`**、**`failed`**）时发送。通过 [`firmware/firmware.c`](firmware/firmware.c) 中的 **`joint_calls("machine/status", NULL)`** 实现。

### C 代码示例
```c
#include "skin/skin.h"

static void example_fpk(const char *path)
{
    talk_t ret = scalls("arch@firmware", "fpk", path);
    if (ret > tpanic)
        talk_free(ret);
}
```
