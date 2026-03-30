## ifname@wan — WAN 网络管理
管理 WAN 网络。此组件依赖于面向 WAN 的接口（通常是 **`arch`** `ethernet`）以及 **network** 项目（`network@frame`、`skinnet`、多链路 **`connect`** -- 参见 [`../network/frame.cn.md`](../network/frame.cn.md)）。  
通常 `ifname@wan` 是第一个 WAN 网络。如果有多个 WAN，`ifname@wan2` 是第二个 WAN 网络，编号依次递增。

### 配置 ( `ifname@wan` )
**ifname@wan** 是第一个 WAN 网络   
**ifname@wan2** 是第二个 WAN 网络   

```json
// 属性介绍
{
    "status":"系统启动时启动",    // [ "enable", "disable" ], enable 表示开机后自动启动

    // MAC
    "mac":"设置接口 MAC 地址", // [ mac address ]

    // IPv4
    "tid":"路由表标识号",         // [ number ] 策略路由表 ID，主要用于多 WAN
    "metric":"默认路由度量值",       // [ number  ]
    "mode":"IPV4 地址模式",            // [ "dhcpc" ] DHCP 模式, [ "static" ] 手动设置, [ "pppoec" ] PPPoE 拨号
    "static":                                 // "mode" 为 "static" 时的详细配置
    {
        "ip":"IPv4 地址",                        // < ipv4 address >
        "mask":"IPv4 子网掩码",                      // < ipv4 netmask >
        "gw":"IPv4 网关",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // "mode" 为 "dhcpc" 时的详细配置
    {
        "static":"通过 DHCP 获取 IP 之前设置一个 IP 地址", // [ "disable", "enable" ]
        "routeopt":"DHCP 选项静态路由",                     // [ "disable", "enable" ]
        "custom_dns":"自定义 DNS",                                 // [ "disable", "enable" ]
        "dns":"自定义 DNS1",                                       // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
        "dns2":"自定义 DNS2"                                       // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
    },
    "pppoec":                                   // "mode" 为 "pppoec" 时的详细配置
    {
        "username":"PPPoE 用户名",                     // [ string ]
        "password":"PPPoE 密码",                     // [ string ]
        "service":"服务名称",                        // [ string ], 默认接受所有服务
        "mss":"TCP 最大报文段长度",                // [ number ], 单位为字节
        "lcp_echo_interval":"LCP 回显间隔",         // [ number ], 单位为秒
        "lcp_echo_failure":"LCP 回显失败次数",     // [ number ]
        "pppopt":"PPP 选项",                          // [ string ], 多个选项用冒号分隔
        "custom_dns":"自定义 DNS",                       // [ "disable", "enable" ]
        "dns":"自定义 DNS1",                             // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
        "dns2":"自定义 DNS2",                            // [ ip address ], 当 "custom_dns" 为 "enable" 时生效
        "txqueuelen":"发送队列大小"                     // [ number ]
    },
    "masq":"IPv4 出站 NAT",                                               // [ "disable", "enable" ]
    "mtu":"最大传输单元",                                            // [ number ], 单位为字节

    // IPv6
    "method":"IPv6 地址模式",             // [ "disable", "manual", "automatic", "slaac" ]
                                                    // "disable" 表示禁用 IPv6
                                                    // "manual" 表示手动设置 IPv6
                                                    // "automatic" 表示 DHCPv6
                                                    // "slaac" 表示无状态地址自动配置
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
        "mode":"获取 IPv6 的模式",                  // [ "try", "force", "disable" ]
        "prefix":"请求的 IPv6 前缀长度",    // [ "auto", "48", "52", "56", "60", "60", "disable" ]
        "custom_resolve":"自定义 DNS",                   // [ "disable", "enable" ]
        "resolve":"自定义 DNS1",                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
        "resolve2":"自定义 DNS2"                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
    },
    "masquerade":"IPv6 出站 NAT",                                                 // [ "disable", "enable" ]

    // 链路检测机制配置，也称为保活机制
    "keeplive":
    {
        "type":"保活模式",   // [ "disable" ] 禁用保活
                                  // [ "icmp" ] ping 保活
                                  // [ "dns" ] 测试 DNS 响应
                                  // [ "recv" ] 统计接收数据包进行保活
                                  // [ "auto" ] 当 DNS 响应测试失败时统计接收数据包进行保活

        "action":"保活失败时的动作",  // [ "reboot" ] 重启系统
                                                // [ "reset" ] 重置接口设备
                                                // [ others ] 重新拨号连接
        "icmp":                                                   // "type" 为 "icmp" 时的详细配置
        {
            "dest":                                                         // ICMP 保活的目标地址
            {
                "destination identify2":"目标地址1",                        // [ string ]:[ IP address ]
                // "...":"..." 可以配置多个目标 IP 地址。如果只有一个 PING 回显包返回，检测成功。如果没有 PING 回显包返回，检测失败  
            },
            "timeout":"等待 PING 回显包返回的最大时间",     // [ number ], 单位为秒
            "failed":"检测失败次数",                                   // [ number ], 如果检测失败次数超过此阈值，链路将被停用
            "interval":"每次成功检测的间隔"                         // [ number ], 单位为秒
        },
        "dns":                                                   // "type" 为 "dns" 时的详细配置
        {
            "timeout":"等待 DNS 解析包返回的最大时间",   // [ number ], 单位为秒
            "failed":"检测失败次数",                                   // [ number ], 如果检测失败次数超过此阈值，链路将被停用
            "interval":"每次成功检测的间隔"                         // [ number ], 单位为秒
        },
        "recv":                                                  // "type" 为 "recv" 时的详细配置
        {
            "timeout":"多少秒未收到数据包视为失败",// [ number ], 单位为秒
            "packets":"多少个数据包",                                              // [ number ]
            "failed":"失败次数"                                                    // [ number ]
        }
    },

    // 连接检测和失败动作配置
    "need_connect":"必须连接成功",                                             // [ "enable", "disable" ]
                                                                                              // "enable" 要求成功连接到接口设备
                                                                                              // "disable" 跳过连接检查
    "connect_failed_threshold":"首次失败重置时间",                           // [ number ], 默认 60 秒
    "connect_failed_threshold2":"第二次失败重置时间",                         // [ number ], 默认 180 秒
    "connect_failed_threshold3":"第三次失败重置时间",                          // [ number ], 默认 600 秒
    "connect_failed_everytime":"每次失败重置时间",                           // [ number ], 默认 1800 秒

    // 通用失败动作配置（用于保活/在线故障）
    "failed_threshold":"首次失败重置时间",                                   // [ number ], 默认 3
    "failed_threshold2":"第二次失败重置时间",                                 // [ number ], 默认 7
    "failed_threshold3":"第三次失败重置时间",                                  // [ number ], 默认 15
    "failed_everytime":"每次失败重置时间"                                    // [ number ], 默认 37
}
```   

示例，显示第一个 WAN 的所有配置
```shell
ifname@wan
{
    "mac":"88:12:4E:23:43:12",                       # 克隆 MAC 地址

    "mode":"pppoec",                                 # 模式为 PPPoE 客户端
    "pppoec":
    {
        "username":"1923221@gd.com",                # PPPoE 用户名为 1923221@gd.com
        "password":"FDAED13E"                       # PPPoE 密码为 FDAED13E
    },
    "masq":"enable",                                 # 出站流量共享接口 IPv4 地址访问互联网

    "method":"slaac",                                # IPv6 地址模式为 slaac

    "keeplive":                                      # 保活机制配置保存在此处
    {
        "type":"icmp",                               # 使用 ICMP 进行保活
        "icmp":                                      
        {
            "dest":                                             # ping 8.8.8.8 和 114.114.114.114
            {
                "test":"8.8.8.8",
                "test2":"114.114.114.114"
            },
            "timeout":"10",                                     # 超时超过 10 秒连续 5 次，则认为链路不可用
            "failed":"5",
            "interval":"5"
        }
    }
}
```   

示例，将第一个 WAN 网络的保活模式修改为 icmp  
```shell
ifname@wan:keeplive/type=icmp
ttrue
```   

示例，将第一个 WAN 的拨号模式修改为 DHCP
```shell
ifname@wan:mode=dhcpc
ttrue
```   

示例，修改第一个 WAN 网络的 ICMP 保活目标地址  
```shell
ifname@wan:keeplive/icmp/dest/test=8.8.8.8            # 修改 ICMP 保活第一个目标地址为 8.8.8.8
ttrue
ifname@wan:keeplive/icmp/dest/test2=8.8.4.4           # 修改 ICMP 保活第二个目标地址为 8.8.4.4 
ttrue
ifname@wan:keeplive/icmp/dest/test3=114.114.114.114   # 修改 ICMP 保活第三个目标地址为 114.114.114.114
ttrue
# 也可以用一条 JSON 更新完成以上三条命令
ifname@wan:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   

示例，修改第一个 WAN 的 PPPoE 用户名和密码
```shell
ifname@wan:pppoec/username=dimmalex@ashyelf.com
ttrue
ifname@wan:pppoec/password=123456
ttrue
```   

示例，禁用第一个 WAN
```shell
ifname@wan:status=disable
ttrue
```     

示例，禁用第二个 WAN
```shell
ifname@wan2:status=disable
ttrue
```

示例，一次修改多个属性（**合并**）
```shell
ifname@wan|{"status":"enable","mode":"dhcpc","masq":"enable"}
ttrue
```

示例，仅在一个路径下合并（子树 **`|{json}`**）
```shell
ifname@wan:pppoec|{"username":"user@isp","password":"secret"}
ttrue
```

### 组件 API
**可直接调用的** API：`ifname@wan.method`、`ifname@wan2.method`、...

**ifname@wan** 是第一个 WAN 网络  
**ifname@wan2** 是第二个 WAN 网络

+ `status[]` **获取 WAN 网络信息**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 状态信息   
    ```json
    // 方法返回的属性介绍
    {
        "status":"当前状态",        // [ "nodevice", "uping", "block", "up", "failed", "down" ]
                                             // "nodevice" 表示底层设备不存在
                                             // "uping" 正在连接
                                             // "block" 等待保活检查恢复
                                             // "up" 网络已连接
                                             // "failed" 保活失败
                                             // "down" 接口已关闭

        "mode":"IPV4 地址模式",     // [ "dhcpc" ] DHCP 模式, [ "static" ] 手动设置, [ "pppoec" ] PPPoE 拨号
        "netdev":"网络设备名",         // [ string ]
        "ifdev":"接口设备名",           // [ string ], 可选
        "gw":"网关 IP 地址",      // [ ip address ]
        "dns":"DNS IP 地址",         // [ ip address ]
        "dns2":"DNS2 IP 地址",       // [ ip address ]
        "ip":"IP 地址",              // [ ip address ]
        "mask":"子网掩码",          // [ ip address ]
        "delay":"延迟时间",           // [ "failed", "block", number ], 可选，"failed" 表示网络测试失败，"block" 表示测试中
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

    示例，获取第一个 WAN 网络信息
    ```shell
    ifname@wan.status
    {
        "status":"up",                     # 连接成功

        "mode":"static",                   # IPv4 连接模式为 static
        "netdev":"wan",                    # 网络设备为 wan
        "gw":"192.168.10.254",             # 网关为 192.168.10.254
        "dns":"114.114.114.114",           # DNS 为 114.114.114.114
        "dns2":"221.5.88.88",              # 备用 DNS 为 221.5.88.88
        "ip":"192.168.10.1",               # IP 地址为 192.168.10.1
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

+ `netdev[]` **获取 WAN 网络设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回网络设备名字符串  

    示例，获取第一个 WAN 网络的网络设备名
    ```shell
    ifname@wan.netdev
    wan
    ```   

+ `ifdev[]` **获取接口设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回接口设备组件名  

    示例，获取第一个 WAN 网络的接口设备名
    ```shell
    ifname@wan.ifdev
    vlan@wan
    ```   

+ `shut[]` **关闭 WAN 网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，关闭第一个 WAN 网络
    ```shell
    ifname@wan.shut
    ttrue
    ```   
    示例，关闭第二个 WAN 网络
    ```shell
    ifname@wan2.shut
    ttrue
    ```   

+ `setup[]` **启动 WAN 网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，启动第一个 WAN 网络
    ```shell
    ifname@wan.setup
    ttrue
    ```   
    示例，启动第二个 WAN 网络
    ```shell
    ifname@wan2.setup
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 与**组件 API** 中的条目相同。参考 **ifname** 包不会为 **`ifname@wan`** 调度 **`init`/`uninit`**；**network** 协议栈、产品集成或操作者在链路上线或下线时调用 **`setup[]` / `shut[]`**。

### 联合处理程序
+ `keepon[]` **清除连接失败计数器**   
    - 成功：返回 `ttrue`
    - 当网络连接确认存活时调用
    - 重置内部 `connect_failed` 计数器以防止不必要的设备重置

    示例，清除第一个 WAN 网络的连接失败计数器
    ```shell
    ifname@wan.keepon
    ttrue
    ```   

+ `keepoff[]` **处理保活检查失败**   
    - 成功：返回 `ttrue`
    - 当保活检查失败时执行配置的动作
    - 动作取决于 `keeplive/action` 配置：
      - `"reboot"`：重启系统（如果运行时间 > 180 秒）
      - `"reset"`：重置接口设备
      - 其他：重置连接

    示例，处理第一个 WAN 网络的保活失败
    ```shell
    ifname@wan.keepoff
    ttrue
    ```   


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_ifname_wan(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "ifname@wan", "status") == NULL)
        return -1;
    ok = ssets_string("ifname@wan", "value", "status");
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

/* 示例: scall("ifname@wan", "status", NULL); 如果是 JSON 则调用 talk_free */
```
