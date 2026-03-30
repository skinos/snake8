## tui@telnet — Telnet 服务器管理
管理网关 **telnetd** 服务（BusyBox 或 `PATH` 中的系统 `telnetd`）。

#### 平台说明
- 在 **`scope=wrt`** 或 **`platform=slave`** 上，telnet **设计为禁用**：`_setup` 返回成功但不启动服务；`_set` 拒绝配置更改。
- 如果 `which telnetd` 找不到可执行文件（代码中有路径长度检查），`_setup` 失败（`tfalse`）。
- 如果此组件 **没有存储的配置**（`config_get` 返回空），`_setup` 返回成功且不从配置对象应用设置。

### 配置 ( `tui@telnet` )
```json
// Attributes introduction 
{
    "status":"enable or disable the service", // [ "disable", "enable" ]
    "port":"service port",                    // string; empty uses default 23 in code
    "manager":                                // optional; access control via iptables
    // Either:
    // (1) JSON object — keys are labels, values are IPv4 or MAC strings:
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
    // (2) Single string — semicolon-separated IPv4 or MAC (same as WUI textarea):
    // "192.168.8.111;00:03:7F:12:AA:B0"
}
```

**`manager` 行为（当 `status` 为 `enable` 时）：**
- 如果 `manager` **缺失或为空**，此组件不添加额外的 iptables 规则；谁能连接取决于其余的防火墙规则。
- 如果 `manager` 有 **一个或多个** 地址：到监听端口的流量被发送到链 `tui_telnet`；仅匹配列出的 **IPv4**（`inet_pton`）或 **MAC**（否则视为 `--mac-source`）的 **ACCEPT**；该链以 **DROP** 结束以拒绝其他人。


示例，显示所有 telnet 服务器配置
```shell
tui@telnet
{
    "status":"enable",             # 系统启动时启动此服务
    "port":"23",                   # 服务端口 23
    "manager":                     # 仅 192.168.8.111 和 00:03:7F:12:AA:B0 可以访问
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

示例，修改 telnet 服务器端口
```shell
tui@telnet:port=2323
ttrue
```  

示例，禁用 telnet 服务器
```shell
tui@telnet:status=disable
ttrue
```  

示例，设置 IP 192.168.8.250 可以访问 telnet 服务器
```shell
tui@telnet:manager/pc3=192.168.8.250
ttrue
```  

示例，清除 manager 允许所有 IP 访问 telnet 服务器
```shell
tui@telnet:manager=
ttrue
```  

示例，manager 作为分号分隔的字符串（与 Web UI 相同的样式）
```shell
tui@telnet:manager="192.168.8.111;00:03:7F:12:AA:B0"
ttrue
```  

示例，一次更改多个属性（**合并**）
```shell
tui@telnet|{"status":"enable","port":"2323"}
ttrue
```

### 组件 API
+ `setup[]` **应用已保存的 telnet 配置并启动或跳过服务**，*成功返回 ttrue*
    - 通常在启动期间作为 **`tui@telnet.setup`** 调用（通过已安装包的 **init** 调度）。验证 **`telnetd`** 是否存在；在 **`scope=wrt`** 或 **`platform=slave`** 上返回成功但不启动。如果配置缺失，返回成功但不应用存储的设置。如果 **`status`** 不是 **`enable`**，返回成功但不启动 **`telnetd`**。否则应用可选的 **`manager`** iptables 规则（链 **`tui_telnet`**）并启动 **`service`** 子进程（**`telnetd -F -p <port>`**）。
    - 如果在 **`PATH`** 中找不到 **`telnetd`**，*失败返回 tfalse*。

    示例，手动运行 setup
    ```shell
    tui@telnet.setup
    ttrue
    ```

+ `shut[]` **停止 telnet 服务并移除此组件的 iptables 链**，*成功返回 ttrue*
    - 刷新并删除过滤链 **`tui_telnet`**，移除到该链的 **`INPUT`** 跳转，并删除受监控的服务条目（对 **`COM_IDPATH`** 执行 **`sdelete`**）。不移除已保存的配置。

    示例，关闭 telnet
    ```shell
    tui@telnet.shut
    ttrue
    ```

### 生命周期 API
+ `setup[]` — 在 **`init` → `app`** 期间作为 **`tui@telnet.setup`** 运行。**没有** 默认的 **`uninit`**；如需要请使用 **`shut[]`** 或扩展包清单。

+ `shut[]` — 在默认包的 **`uninit`** 中 **不会** 自动连接；参见 **组件 API**。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_tui_telnet(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "tui@telnet", "status") == NULL)
        return -1;
    ok = ssets_string("tui@telnet", "value", "status");
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

/* Example: scall("tui@telnet", "status", NULL); then talk_free if JSON */
```
