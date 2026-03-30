## tui@ssh — SSH 服务器管理
使用 **Dropbear**（`PATH` 中的 `dropbear`，非 OpenSSH `sshd`）管理网关 **SSH** 服务。

#### 平台说明
- 在 **`scope=wrt`** 或 **`platform=slave`** 上，SSH **设计为禁用**：`_setup` 返回成功但不启动服务；`_set` 拒绝配置更改。
- 如果 `which dropbear` 找不到可执行文件，`_setup` 失败（`tfalse`）。
- 如果此组件 **没有存储的配置**（`config_get` 返回空），`_setup` 返回成功且不从配置对象应用设置。

#### 主机密钥
- 确保 `/etc/dropbear` 目录存在。
- 如果项目配置提供了 **`dsskey`** / **`rsakey`** 文件，它们将被复制到 `/etc/dropbear/` 下的 `dropbear_dss_host_key` / `dropbear_rsa_host_key`。
- 如果为此包安装了 **`dropbearkey.sh`**，将执行它以生成缺失的密钥类型（例如 ed25519、ecdsa、rsa），取决于镜像支持情况。

### 配置 ( `tui@ssh` )
```json
// Attributes introduction 
{
    "status":"enable or disable the service", // [ "disable", "enable" ]
    "port":"service port",                    // string; empty uses default 22 in code
    "manager":                                // optional; same rules as telnet
    // (1) JSON object — values are IPv4 or MAC strings
    // (2) Single string — semicolon-separated list (WUI textarea)
}
```

**`manager` 行为（当 `status` 为 `enable` 时）：** 与 `tui@telnet` 相同的 iptables 模式，但过滤链名称为 **`tui_ssh`**。非空 `manager` → 从 `INPUT` 跳转到监听端口 → 每地址 **ACCEPT** → 最终 **DROP**。空或缺少 `manager` → 此组件不为该端口添加规则。


示例，显示所有 SSH 服务器配置
```shell
tui@ssh
{
    "status":"enable",             # 系统启动时启动此服务
    "port":"22",                   # 服务端口 22
    "manager":                     # 仅 192.168.8.111 和 00:03:7F:12:AA:B0 可以访问 SSH 服务器
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

示例，修改 SSH 服务器端口
```shell
tui@ssh:port=2222
ttrue
```  

示例，禁用 SSH 服务器
```shell
tui@ssh:status=disable
ttrue
```  

示例，设置 IP 192.168.8.250 可以访问 SSH 服务器
```shell
tui@ssh:manager/pc3=192.168.8.250
ttrue
```  

示例，清除 manager 允许所有 IP 访问 SSH 服务器
```shell
tui@ssh:manager=
ttrue
```  

示例，manager 作为分号分隔的字符串（与 Web UI 相同的样式）
```shell
tui@ssh:manager="192.168.8.111;00:03:7F:12:AA:B0"
ttrue
```  

示例，一次更改多个属性（**合并**）
```shell
tui@ssh|{"status":"enable","port":"2222"}
ttrue
```

### 组件 API
+ `setup[]` **应用已保存的 SSH (Dropbear) 配置并启动或跳过服务**，*成功返回 ttrue*
    - 通常在启动期间作为 **`tui@ssh.setup`** 调用（通过已安装包的 **init** 调度）。验证 **`dropbear`** 是否存在；在 **`scope=wrt`** 或 **`platform=slave`** 上返回成功但不启动。如果配置缺失，返回成功但不应用存储的设置。如果 **`status`** 不是 **`enable`**，返回成功但不启动 **Dropbear**。否则准备 **`/etc/dropbear`** 主机密钥（如果存在则从项目 **`dsskey`** / **`rsakey`** 获取，然后在安装时运行 **`dropbearkey.sh`**），应用可选的 **`manager`** iptables 规则（链 **`tui_ssh`**），并启动 **`service`** 子进程（**`dropbear -F -p <port> -K 300`**）。
    - 如果在 **`PATH`** 中找不到 **`dropbear`**，*失败返回 tfalse*。

    示例，手动运行 setup
    ```shell
    tui@ssh.setup
    ttrue
    ```

+ `shut[]` **停止 Dropbear 服务并移除此组件的 iptables 链**，*成功返回 ttrue*
    - 刷新并删除过滤链 **`tui_ssh`**，移除到该链的 **`INPUT`** 跳转，并删除受监控的服务条目（对 **`COM_IDPATH`** 执行 **`sdelete`**）。不移除已保存的配置。

    示例，关闭 SSH
    ```shell
    tui@ssh.shut
    ttrue
    ```

### 生命周期 API
+ `setup[]` — 在 **`init` → `app`** 期间作为 **`tui@ssh.setup`** 运行（通常与 **`tui@telnet.setup`** 一起）。**没有** 默认的 **`uninit`** 钩子；如果需要有序关闭，请显式调用 **`shut[]`**。

+ `shut[]` — 在默认包的 **`uninit`** 中 **不会** 自动调用；参见 **组件 API**。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_tui_ssh(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "tui@ssh", "status") == NULL)
        return -1;
    ok = ssets_string("tui@ssh", "value", "status");
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

/* Example: scall("tui@ssh", "status", NULL); then talk_free if JSON */
```
