## ifname@lan — 本地/LAN 网络管理
管理本地 (LAN) 网络。此组件依赖于本地网络接口或交换机 (SoC)，通常通过 **`arch`**（`ethernet`、桥接/VLAN 布线）以及 **network** 项目（`network@frame` 注册 -- 参见 [`../network/frame.cn.md`](../network/frame.cn.md)）。  
通常 `ifname@lan` 是第一个本地网络。如果有多个本地网络，`ifname@lan2` 是第二个本地网络，编号依次递增。

### 配置 ( `ifname@lan` )
**ifname@lan** 是第一个本地网络   
**ifname@lan2** 是第二个本地网络   

```json
// 属性介绍
{
    "status":"系统启动时启动",    // [ "enable", "disable" ], enable 表示开机后自动启动

    // IPv4
    "mode":"IPV4 地址模式",            // [ "dhcpc" ] DHCP 客户端模式, [ "static" ] 手动设置 IPv4
    "static":                                 // "mode" 为 "static" 时的详细配置
    {
        "ip":"IPv4 地址",                        // < ipv4 address >
        "mask":"IPv4 子网掩码",                      // < ipv4 netmask >
        "ip2":"IPv4 地址 2",                     // < ipv4 address >
        "mask2":"IPv4 子网掩码 2",                   // < ipv4 netmask >
        "ip3":"IPv4 地址 3",                     // < ipv4 address >
        "mask3":"IPv4 子网掩码 3",                   // < ipv4 netmask >
        "gw":"IPv4 网关",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // "mode" 为 "dhcpc" 时的详细配置
    {
        "static":"通过 DHCP 获取 IP 之前设置一个 IP 地址", // [ "disable", "enable" ], 临时备用地址
        "routeopt":"DHCP 选项静态路由",                     // [ "disable", "enable" ], 接受无类别静态路由
        "custom_dns":"自定义 DNS",                                 // [ "disable", "enable" ]
        "dns":"自定义 DNS1",                                       // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
        "dns2":"自定义 DNS2"                                       // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
    },
    "dhcps":                                               // DHCP 服务器设置的详细配置
    {
        "status":"是否启动 DHCP 服务",                      // [ "disable", "enable" ]
        "startip":"IPv4 分配池的起始地址",     // [ ipv4 address ]
        "endip":"IPv4 分配池的结束地址",            // [ ipv4 address ]
        "mask":"IPv4 分配池中的子网掩码",                 // [ ipv4 netmask ]
        "lease":"分配地址的租约时间",                       // [ number ], 单位为秒
        "gw":"指定 IPv4 网关",                                 // [ ipv4 address ], 默认为本地网络 IP 地址
        "dns":"指定 IPv4 DNS",                                    // [ ipv4 address ], 默认为本地网络 IP 地址
        "dns2":"指定 IPv4 备用 DNS",                            // [ ipv4 address ]
        "options":"dnsmasq 原始选项"                               // [ string ], 多个选项用分号分隔
    },

    // IPv6
    "method":"IPv6 地址模式",             // [ "disable", "manual", "automatic" ]
                                                    // "disable" 表示禁用 IPv6
                                                    // "manual" 表示手动设置 IPv6
                                                    // "automatic" 表示 DHCPv6
    "manual":                                 // "method" 为 "manual" 时的详细配置
    {
        "addr":"IPv6 地址",                      // < ipv6 address >
        "prefix":"IPv6 前缀",                     // < number >, 1-128
        "hop":"IPv6 网关",                       // [ ipv6 address ]
        "resolve":"IPv6 DNS",                       // [ ipv6 address ]
        "resolve2":"IPv6 DNS2"                      // [ ipv6 address ]
    },
    "automatic":                             // "method" 为 "automatic" 时的详细配置
    {
        "custom_resolve":"自定义 DNS",                   // [ "disable", "enable" ]
        "resolve":"自定义 DNS1",                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
        "resolve2":"自定义 DNS2"                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
    },
    "addrpool":
    {
        "status":"DHCPv6 服务类型",                                      // [ "disable", "enable" ]
        "startaddr":"IPv6 分配池的起始地址",     // [ ipv6 address ]
        "endaddr":"IPv6 分配池的结束地址",            // [ ipv4 address ]
        "prefix":"IPv6 分配池中的子网掩码",                 // [ ipv4 netmask ]
        "leasetime":"分配的租约时间",                                // [ number ], 单位为秒
        "hop":"指定 IPv4 网关",                                  // [ ipv4 address ], 默认为本地网络 IP 地址
        "resolve":"指定 IPv4 DNS",                                  // [ ipv4 address ], 默认为本地网络 IP 地址
        "resolve2":"指定 IPv4 备用 DNS"                           // [ ipv4 address ]
    }

}
```   

示例，显示第一个本地网络的所有配置
```shell
ifname@lan
{
    "mode":"static",                                 # 模式为 static
    "static":
    {
        "ip":"192.168.1.1",                          # IPv4 地址为 192.168.1.1
        "mask":"255.255.255.0"                       # IPv4 子网掩码为 255.255.255.0
    },
    "method":"automatic",                            # IPv6 地址模式为 automatic

    "dhcps":
    {
        "status":"enable",           # 启用 DHCP 服务器
        "startip":"192.168.1.2",     # IPv4 地址池起始于 192.168.1.2
        "endip":"192.168.1.100",     # IPv4 地址池结束于 192.168.1.100
        "mask":"255.255.255.0",      # 子网掩码为 255.255.255.0
        "lease":"86400",             # 租约时间为 86400
        "gw":"",                     # 未配置网关，默认分配 192.168.1.1
        "dns":""                     # 未配置 DNS，默认分配 192.168.1.1
    }    
}
```   

示例，修改第一个本地网络的 IP 地址（重启后生效）
```shell
ifname@lan:static/ip=192.168.2.1
ttrue
```   

示例，禁用第一个本地网络上的 DHCP 服务器（重启后生效）
```shell
ifname@lan:dhcps/status=disable
ttrue
```     

示例，修改第一个本地网络的 DHCP 地址池：起始 IP `192.168.2.100`，结束 IP `192.168.2.200`
```shell
ifname@lan:dhcps|{"startip":"192.168.2.100","endip":"192.168.2.200"}
ttrue
```

示例，一次修改多个属性（**合并**）
```shell
ifname@lan|{"status":"enable","mode":"static"}
ttrue
```

### 组件 API
**可直接调用的** API：`ifname@lan.method`、`ifname@lan2.method`、...

**ifname@lan** 是第一个本地网络  
**ifname@lan2** 是第二个本地网络

+ `status[]` **获取本地网络信息** 
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 状态信息  
    ```json
    // 方法返回的属性介绍
    {
        "status":"当前状态",        // [ "nodevice", "uping", "down", "up" ]
                                             // "nodevice" 表示底层设备不存在
                                             // "uping" 表示正在连接
                                             // "down" 表示接口已关闭
                                             // "up" 表示连接已建立

        "mode":"IPV4 地址模式",     // [ "dhcpc" ] DHCP 模式, [ "static" ] 手动设置
        "netdev":"网络设备名",         // [ string ]
        "ifdev":"接口设备名",           // [ string ], 可选
        "gw":"网关 IP 地址",      // [ ip address ], 可选
        "dns":"DNS IP 地址",         // [ ip address ], 可选
        "dns2":"DNS2 IP 地址",       // [ ip address ], 可选
        "ip":"IP 地址",              // [ ip address ]
        "mask":"子网掩码",          // [ ip address ]
        "ontime":"在线运行时间",       // [ string ], 可选，在线系统运行时间
        "livetime":"在线时长",       // [ string ], 格式为 小时:分钟:秒:天
        "rx_bytes":"发送字节数",        // [ number ]
        "rx_packets":"发送数据包数",    // [ number ]
        "tx_bytes":"接收字节数",     // [ number ]
        "tx_packets":"接收数据包数", // [ number ]
        "mac":"MAC 地址",            // [ mac address ]

        "method":"IPv6 地址模式",   // [ "manual", "automatic", "slaac" ], 可选，IPv6 启用时存在
                                            // "manual" 手动设置
                                            // "automatic" DHCPv6
                                            // "slaac" 无状态地址自动配置
        "addr":"IPv6 地址",          // [ ipv6 address ], 可选，IPv6 启用时存在
        "addr2":"IPv6 地址 2",        // [ ipv6 address ], 可选，IPv6 启用时存在
        "addr3":"IPv6 地址 3"         // [ ipv6 address ], 可选，IPv6 启用时存在

    }
    ```   

    示例，获取第一个本地网络信息
    ```shell
    ifname@lan.status
    {
        "status":"up",                     # 连接成功

        "mode":"static",                   # IPv4 连接模式为 static
        "netdev":"lan",                    # 网络设备为 lan
        "ip":"192.168.1.1",                # IP 地址为 192.168.1.1
        "mask":"255.255.255.0",            # 子网掩码为 255.255.255.0
        "livetime":"01:15:50:0",           # 已在线 1 小时 15 分 50 秒
        "rx_bytes":"1256",                 # 接收 1256 字节
        "rx_packets":"4",                  # 接收 4 个数据包
        "tx_bytes":"1320",                 # 发送 1320 字节
        "tx_packets":"4",                  # 发送 4 个数据包
        "mac":"02:50:F4:00:00:00",         # 网络设备 MAC 地址为 02:50:F4:00:00:00

        "method":"slaac",                  # IPv6 地址模式为 slaac
        "addr":"fe80::50:f4ff:fe00:0"      # 本地 IPv6 地址为 fe80::50:f4ff:fe00:0
    }
    ```   

+ `netdev[]` **获取网络设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回网络设备名字符串  

    示例，获取第一个本地网络的网络设备名
    ```shell
    ifname@lan.netdev
    lan
    ```   

+ `ifdev[]` **获取接口设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回接口设备组件名  

    示例，获取第一个本地网络的接口设备名
    ```shell
    ifname@lan.ifdev
    vlan@lan
    ```   

+ `shut[]` **关闭本地网络**  
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，关闭第一个本地网络
    ```shell
    ifname@lan.shut
    ttrue
    ```   
    示例，关闭第二个本地网络
    ```shell
    ifname@lan2.shut
    ttrue
    ```   

+ `setup[]` **启动本地网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，启动第一个本地网络
    ```shell
    ifname@lan.setup
    ttrue
    ```   
    示例，启动第二个本地网络
    ```shell
    ifname@lan2.setup
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 与**组件 API** 中的条目相同。参考 **ifname** 包不会为 **`ifname@lan`** 调度 **`init`/`uninit`**；**network** 协议栈或产品代码调用 **`setup[]` / `shut[]`**。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_ifname_lan(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "ifname@lan", "status") == NULL)
        return -1;
    ok = ssets_string("ifname@lan", "value", "status");
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

/* 示例: scall("ifname@lan", "status", NULL); 如果是 JSON 则调用 talk_free */
```
