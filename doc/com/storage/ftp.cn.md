## storage@ftp — FTP 服务器管理 (ProFTPD)

使用 **`storage@ftp`** 管理 FTP 服务器。持久化默认值从 [`ftp.cfg`](ftp.cfg) 合并。不建议直接在磁盘上修改配置 — 请优先使用 HE API 或 Web UI。

### 配置 ( `storage@ftp` )
```json
// Attributes introduction (see README.md for full field semantics)
{
    "status":"enable or disable the FTP service",     // [ "enable", "disable" ]
    "mode":"login mode",                              // [ "anonymous", "user" ]
    "root":"optional chroot base",                      // [ string ], default PROJECT_MNT_DIR
    "anonymous": { "path":"...", "permission":"..." },
    "user": { "share name": { "path":"...", "permission":"...", "user": { } } }
}
```

示例，显示当前 FTP 配置
```shell
storage@ftp
{
    "status":"enable",                         # FTP 服务已启用
    "mode":"user",                             # 登录模式：需要用户认证
    "root":"/mnt",                             # chroot 基础目录
    "anonymous":                               # 匿名访问设置
    {
        "path":"/mnt",                             # 匿名访问根路径
        "permission":"read"                        # 匿名权限：仅读取
    },
    "user":                                    # 用户认证共享列表
    {
        "share1":                                  # 名为 "share1" 的共享
        {
            "path":"/mnt/sda1",                        # share1 映射到 /mnt/sda1
            "permission":"all",                        # 完全读写权限
            "user":                                    # 按用户的权限覆盖
            {
            }
        }
    }
}
```

示例，启用服务
```shell
storage@ftp:status=enable
ttrue
```

示例，一次更改多个属性（**合并**）
```shell
storage@ftp|{"status":"enable","mode":"anonymous"}
ttrue
```

### 组件 API
+ `setup[]` **根据已保存的配置启动 FTP（启用时）**，*成功返回 ttrue*
    - 通常在启动期间作为 **`storage@ftp.setup`** 调用（通过已安装包的 **init** 调度）。如果注册的 **`platform`** 为 **`slave`**，FTP 将被有意跳过（仅调试日志），调用仍返回 **`ttrue`**。如果 **`/usr/sbin/proftpd`** 不存在，返回 **`tfalse`**。当 **`status`** 为 **`enable`** 时，注册并启动 **`service`** 子进程（**`cstart`** → **`_service`**，写入 **`proftpd.conf`** 并 **`execlp`** **`proftpd`**）。

+ `shut[]` **拆除此组件的 FTP 服务注册**，*成功返回 ttrue*
    - 调用 **`sdelete( COM_IDPATH )`** 移除组件的服务注册。**`_set`** 在 **`config_set`** 之前调用 **`_shut`**，以便在应用新 JSON 之前清除正在运行的 **`proftpd`** 实例；您也可以在平台关机路径运行组件 **`shut`** 方法时显式调用 **`shut`**。


### 生命周期 API
+ `setup[]` — 仅在 **`init` → `app`** 期间作为 **`storage@ftp.setup`** 运行。**`storage@ftp.shut`** **没有** 默认的 **`uninit`** 条目。

+ `shut[]` — 手动调用，从 **`_set`** 调用，或在产品清单中添加 **`uninit`**。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_storage_ftp(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "storage@ftp", "status") == NULL)
        return -1;
    ok = ssets_string("storage@ftp", "enable", "status");
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

/* Example: scall("storage@ftp", "setup", NULL); */
```
