## client@dhcps — DHCP 服务器管理
管理 DHCP 服务器  
DHCP 服务器设置按逻辑 **ifname** 进行配置。**`client@dhcps.list`** 返回 DHCP 租约列表中当前已知的客户端（服务器已分配的地址），而非对每个设备的实时探测。

### 配置 ( `client@dhcps` )
```json
// Attributes introduction 
{
    "interface name":                              // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], DHCP settings for this logical ifname
    {
        "status":"dhcp server status",         // [ "disable", "enable" ]
        "startip":"dhcp pool start ip",        // [ ip address ] first IP of the DHCP range
        "endip":"dhcp pool end ip",            // [ ip address ] last IP of the DHCP range
        "mask":"subnet mask",                  // [ ip address ] optional, defaults to interface static mask
        "lease":"lease time",                  // [ number ] seconds; values below 120 may be raised to 120 when applied
        "gw":"gateway address",                // [ ip address ] optional, router option sent to clients
        "dns":"primary dns server",            // [ ip address ] optional, DNS server option sent to clients
        "dns2":"secondary dns server",         // [ ip address ] optional, secondary DNS server
        "routeopt_table":"static routes",      // [ array ] optional, static route options (RFC 3442 classless route option)
        // each entry has "target" (network), "mask" (CIDR bits), "gw" (gateway)
        "options":"extra config lines",        // [ string ] optional, extra dnsmasq config lines separated by ';'
        "mode":"dhcp mode",                    // [ "static" ] optional, "static" for static IP assignment mode
        "dnsproxy":"dns proxy settings"        // [ object ] optional, DNS proxy/redirect configuration
        // contains "dns" field for the DNS server to redirect all queries to
    }
    // more ifname
}
```   

示例，显示所有 DHCP 服务器配置
```shell
client@dhcps
{
    "ifname@lan":                              # ifname@lan 的 DHCP 设置
    {
        "status":"enable",                         # DHCP 服务器已启用
        "startip":"192.168.31.100",                # DHCP 地址池起始地址 192.168.31.100
        "endip":"192.168.31.254",                  # DHCP 地址池结束地址 192.168.31.254
        "mask":"255.255.255.0",                    # 子网掩码 255.255.255.0
        "lease":"86400",                           # 租约时间 86400 秒（24 小时）
        "gw":"192.168.31.1",                       # 发送给客户端的网关地址
        "dns":"8.8.8.8",                           # 客户端的主 DNS 服务器
        "dns2":"114.114.114.114"                   # 客户端的备 DNS 服务器
    }
}
```

### 组件 API
可从 HE / eline / HTTP `/he` **直接调用** 的 API。
+ `list[]` **列出当前 DHCP 客户端信息**   
    - 返回一个 **JSON 对象**（如果尚无租约信息则为空 **`{}`**）。每个键是客户端 **MAC** 地址；每个值包含 **`ip`** 和 **`name`**（来自 DHCP 的主机名）。
    - 要查看在线时长和更丰富的客户端视图，请使用 **`client@station.list`**。
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "ip":"ip address",                    // [ IP address ]
            "name":"client name",                 // [ string ]
        }
        // ... more client
    }
    ```   

    示例，列出 DHCP 服务器当前所有客户端
    ```shell
    client@dhcps.list
    {
        "04:CF:8C:39:91:7A":            # 第一个客户端
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",    # 主机名为 xiaomi-aircondition-ma2_mibt917A
            "ip":"192.168.31.140",                        # IP 为 192.168.31.140
        },
        "40:31:3C:B5:6D:4C":            # 第二个客户端
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C"
        },
        "14:13:46:C9:97:C7":            # 第三个客户端
        {
            "ip":"192.168.31.9",
            "name":"Watch"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "name":"Xiaomi-14-Ultra"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "name":"Xiaomi-13-Ultra"
        }
    }
    ```   

### 生命周期 API
以下通常在平台 **`init`** / **`uninit`** 期间（FPK 加载后）调用，但也可手动调用：


+ `setup[]` **启动 DHCP 服务端**，*成功返回 ttrue*
    - 在系统启动时调用。在 **slave** 构建上，不启动 DHCP 服务。
    - 否则启动镜像所使用的 DHCP/IPv6 辅助服务（**dnsmasq**，以及可用时的 **odhcpd**）。

+ `shut[]` **停止 DHCP 服务端**，*成功返回 ttrue*
    - 在关机时调用。停止已启动的 DHCP 相关服务。

### Joint 处理程序
具有此包的设备上典型的 **joint** 连接（事件 → 处理程序）：

+ `reset[]` **在 WAN/LAN ifname 变更后重启 DHCP 服务**，*成功返回 ttrue*
    - **`network/on`** 和 **`network/off`** → **`client@dhcps.reset`**。
    - 在 **slave** 构建上不执行任何操作。否则重启 DHCP 服务，使寻址和租约与新的网络状态保持一致。

+ `on[]` **重新加载 DHCP 配置而无需完全重启**，*成功返回 ttrue*
    - **`network/online`** 和 **`network/offline`** → **`client@dhcps.on`**。
    - 在 **slave** 构建上不执行任何操作。否则通知 DHCP 服务重新加载其配置。

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_client_dhcps(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@dhcps", "status") == NULL)
        return -1;
    return ssets_string("client@dhcps", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@dhcps", "list", NULL); talk_free if JSON */
```
