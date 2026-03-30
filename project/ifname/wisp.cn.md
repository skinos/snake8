## ifname@wisp — WISP 网络管理
管理 WISP 网络。此组件依赖于无线站点接口（**`wifi`** `sta` / **`arch`** 无线 BSP）以及 **network** 项目（[`../network/frame.cn.md`](../network/frame.cn.md)）。  
通常 ifname@wisp 是第一个 WISP（2.4G）网络。如果系统中有多个 WISP 网络，ifname@wisp2 是第二个 WISP（5.8G）网络，编号依次递增。

### 配置 ( `ifname@wisp` )
**ifname@wisp** 是第一个 WISP(2.4G) 网络   
**ifname@wisp2** 是第二个 WISP(5.8G) 网络   

```json
// 属性介绍
{
    "status":"系统启动时启动",    // [ "enable", "disable" ]

    // MAC
    "mac":"设置接口 MAC 地址", // [ mac address ]

    // 无线连接
    "peer":"要连接的 SSID",              // [ string ]
    "peermac":"要连接的 BSSID",          // [ mac address ]
    "peermode":"连接模式",       // [ "hidden" ] 对端 AP 不广播 SSID；隐藏模式下必须设置信道  
    "channel":"无线信道",          // [ number ], 0-165, 0 为自动
    "nossid":"禁用 SSID",           // [ "disable", "enable" ], 连接后禁用本地 SSID
    "secure":"安全模式",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] 无安全
                                                            // [ wpapsk ]  WPAPSK
                                                            // [ wpa2psk ]  WPA2PSK
                                                            // [ wpapskwpa2psk ] WPA 混合
    "wpa_encrypt":"WPA 加密方式",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] AES
                                                            // [ tkip ] TKIP
                                                            // [ tkipaes ] 自动
    "wpa_key":"WPA 密钥",                   // [ string ], 值为至少 8 个字符的字符串。当 "secure" 为 wpapsk/wpa2psk/wpapskwpa2psk 时此参数为必填

    // IPv4
    "tid":"路由表标识号",         // [ number ] 独占路由表 ID，仅用于多 WAN
    "metric":"默认路由度量值",       // [ number  ]
    "mode":"IPV4 地址模式",            // [ "dhcpc" ] DHCP 客户端模式, [ "static" ] 手动设置, [ "pppoec" ] PPPoE 拨号
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
    "masq":"出站流量共享接口 IPv4 地址访问互联网",  // [ "disable", "enable" ]
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
        "resolve":"自定义 DNS1",                         // [ ipv6 address ], 当 "custom_dns" 为 "enable" 时生效
        "resolve2":"自定义 DNS2"                         // [ ipv6 address ], 当 "custom_dns" 为 "enable" 时生效
    },
    "masquerade":"出站流量共享接口 IPv6 地址访问互联网",   // [ "disable", "enable" ]

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

    // 连接失败动作配置
    "failed_threshold":"首次失败重置时间",                                   // [ number ]
    "failed_threshold2":"第二次失败重置时间",                                 // [ number ]
    "failed_threshold3":"第三次失败重置时间",                                  // [ number ]
    "failed_everytime":"每次失败重置时间"                                    // [ number ]
}
```   

示例，显示第一个 WISP（2.4G）的所有配置
```shell
ifname@wisp
{
    "peer":"V520-D21D20",    # 连接 V520-D21D20
    "secure":"wpapsk",       # 安全模式为 WPAPSK
    "wpa_encrypt":"aes",     # 加密方式使用 AES
    "wpa_key":"87654321",    # 密码 87654321

    "mode":"dhcpc",                                  # 模式为 DHCP 客户端
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

示例，将 WISP(2.4G) 网络的保活模式修改为 icmp  
```shell
ifname@wisp:keeplive/type=icmp
ttrue
```   

示例，将 WISP(2.4G) 网络的拨号模式修改为 DHCP
```shell
ifname@wisp:mode=dhcpc
ttrue
```   

示例，修改第一个 WISP(2.4G) 网络的 ICMP 保活目标地址  
```shell
ifname@wisp:keeplive/icmp/dest/test=8.8.8.8            # 修改 ICMP 保活第一个目标地址为 8.8.8.8
ttrue
ifname@wisp:keeplive/icmp/dest/test2=8.8.4.4           # 修改 ICMP 保活第二个目标地址为 8.8.4.4 
ttrue
ifname@wisp:keeplive/icmp/dest/test3=114.114.114.114   # 修改 ICMP 保活第三个目标地址为 114.114.114.114
ttrue
# 也可以用一条 JSON 更新完成以上三条命令
ifname@wisp:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   

示例，修改第一个 WISP（2.4G）连接的 SSID 和安全模式
```shell
ifname@wisp:peer=Myhotpot
ttrue
ifname@wisp:secure=wpapsk
ttrue
ifname@wisp:wpa_key=88888888
ttrue
```   

也可以用一条 JSON 更新完成以上命令
```shell
ifname@wisp|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```   

示例，禁用第一个 WISP(2.4G) 的保活功能
```shell
ifname@wisp:keeplive=disable
ttrue
```   

示例，禁用第一个 WISP(2.4G)
```shell
ifname@wisp:status=disable
ttrue
```   

示例，启用第一个 WISP(2.4G)
```shell
ifname@wisp:status=enable
ttrue
```     

示例，启用第二个 WISP(5.8G)
```shell
ifname@wisp2:status=enable
ttrue
```


### 组件 API
**可直接调用的** API：`ifname@wisp.method`、`ifname@wisp2.method`、...

**ifname@wisp** 是第一个 WISP 网络  
**ifname@wisp2** 是第二个 WISP 网络

+ `status[]` **获取 WISP 信息**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 状态信息   
    ```json
    // 方法返回的属性介绍
    {
        "status":"当前状态",        // [ "uping", "scanning", "block", "up", "failed", "down" ]
                                             // "uping" 正在连接
                                             // "scanning" 正在扫描 AP
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
        "addr3":"IPv6 地址 3",        // [ ipv6 address ], 可选，IPv6 启用时存在

        "peer":"对端 SSID",              // [ string ]
        "peermac":"对端 BSSID",          // [ MAC address ]
        "channel":"对端信道",        // [ 1- 165 ]
        "signal":"信号等级",         // [ 0, 1, 2, 3, 4 ], 0 无信号, 1 最弱, 4 最强
        "rate":"连接速率",           // [ number ], 可选，单位为 M
        "rssi":"对端 RSSI",              // [ number ], 可选，单位为 dBm
        "rssp":"对端信号百分比"  // [ number ], 可选，单位为 %
    }
    ```   

    示例，获取第一个 WISP 网络信息
    ```shell
    ifname@wisp.status
    {
        "status":"up",                     # 连接成功

        "mode":"dhcpc",                    # IPv4 连接模式为 DHCP
        "netdev":"ath11",                  # 网络设备为 ath11
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
        "addr":"fe80::50:f4ff:fe00:0",     # 本地 IPv6 地址为 fe80::50:f4ff:fe00:0

        "peer":"TP-link-2231",            # 对端为 TP-link-2231
        "peermac":"70:3A:D8:54:BC:90",    # 对端 BSSID 为 70:3A:D8:54:BC:90
        "channel":"10",                   # 信道为 10
        "rate":"270",                     # 速率为 270M
        "rssi":"-41",                     # RSSI 为 -41dBm
        "signal":"3"                      # 信号等级为 3
    }
    ```   

+ `netdev[]` **获取 WISP 网络设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`       
    - 成功：返回网络设备名字符串  

    示例，获取第一个 WISP 的网络设备名
    ```shell
    ifname@wisp.netdev
    ath11
    ```   

+ `ifdev[]` **获取接口设备名**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回接口设备组件名  

    示例，获取第一个 WISP 网络的接口设备名
    ```shell
    ifname@wisp.ifdev
    wifi@nsta
    ```   

+ `chlist[]` **获取 WISP 信道列表**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 格式的信道列表   

    ```json
    // 方法返回的属性介绍
    {
        "channel number":{}       // [ number ]:{}
        // ... 更多信道
    }
    ```

    示例，获取第一个 WISP 的信道列表
    ```shell
    ifname@wisp.chlist
    {
        "1":{},        # 信道 1
        "2":{},        # 信道 2
        "3":{},        # 信道 3
        "4":{},        # 信道 4
        "5":{},        # 信道 5
        "6":{},        # 信道 6
        "7":{},        # 信道 7
        "8":{},        # 信道 8
        "9":{},        # 信道 9
        "10":{},       # 信道 10
        "11":{}        # 信道 11
    }
    ```

+ `securelist[]` **获取支持的安全模式**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回 JSON 格式的安全模式列表

    ```json
    // 方法返回的属性介绍
    {
        "secure mode":{}       // [ string ]:{}
        // ... 更多安全模式
    }
    ```

    示例，获取第一个 WISP 支持的安全模式
    ```shell
    ifname@wisp.securelist
    {
        "disable":{},
        "wpapsk":{},
        "wpa2psk":{},
        "wpapskwpa2psk":{}
    }
    ```

+ `aplist[]` **使用 WISP 扫描周围的 AP**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 格式的扫描到的 AP 信息   

    ```json
    // 方法返回的属性介绍
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID 名称",                           // [ string ]
            "channel":"信道号",                   // [ number ], 0-165, 0 为自动
            "secure":"安全模式",                  // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                 // "disable" 无安全
                                                                 // "wpapsk"  WPAPSK
                                                                 // "wpa2psk"  WPA2PSK
                                                                 // "wpapskwpa2psk" WPA 混合
            "wpa_encrypt":"WPA 加密方式",                  // [ "aes", "tkip", "tkipaes" ]
                                                                 // "aes" AES
                                                                 // "tkip" TKIP
                                                                 // "tkipaes" 自动
            "sig":"信号等级(%)",                      // [ number ]
            "signal":"信号等级[0-4]",                 // [ "0", "1", "2", "3", "4" ]
            "chext":"扩展信道",                     // [ "none", "below", "above" ]
            "mode":"无线制式"                      // [ string ]
        }
        // ... 更多 AP
    }
    ```   

    示例，从第一个 WISP 扫描获取周围的 AP
    ```shell
    ifname@wisp.aplist
    {
        "80:EA:07:15:0E:E6":                    # 扫描到的第一个 AP
        {
            "ssid":"1411",                                 # 第一个 AP 的 SSID
            "channel":"6",                                 # 第一个 AP 的信道
            "secure":"wpapskwpa2psk",                      # 安全模式为 WPA 混合
            "wpa_encrypt":"aes",                           # 加密类型为 AES
            "sig":"70",                                    # 信号为 70%
            "signal":"3",                                  # 信号等级为 3，范围为 0-4
            "chext":"below",                               # 扩展信道为 below
            "mode":"11b/g/n"
        },
        "B4:82:C5:80:22:41":                    # 扫描到的第二个 AP
        {
            "ssid":"dimmalex-work",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11b/g/n"
        },
        "8C:74:A0:D6:68:B0":                    # 扫描到的第三个 AP
        {
            "ssid":"CMCC-ktfK",
            "channel":"11",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11b/g/n"
        }
    }
    ```   

+ `shut[]` **关闭 WISP 网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，关闭第一个 WISP 网络
    ```shell
    ifname@wisp.shut
    ttrue
    ```   
    示例，关闭第二个 WISP 网络
    ```shell
    ifname@wisp2.shut
    ttrue
    ```   

+ `setup[]` **启动 WISP 网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，启动第一个 WISP 网络
    ```shell
    ifname@wisp.setup
    ttrue
    ```   
    示例，启动第二个 WISP 网络
    ```shell
    ifname@wisp2.setup
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 与**组件 API** 中的条目相同。参考 **ifname** 包不会为 **`ifname@wisp`** 调度 **`init`/`uninit`**。

### 联合处理程序
+ `keepon[]` **清除连接失败计数器**   
    - 成功：返回 `ttrue`
    - 当网络连接确认存活时调用
    - 重置内部 `connect_failed` 计数器以防止不必要的设备重置

    示例，清除第一个 WISP 网络的连接失败计数器
    ```shell
    ifname@wisp.keepon
    ttrue
    ```   

+ `keepoff[]` **处理保活检查失败**   
    - 成功：返回 `ttrue`
    - 当保活检查失败时执行配置的动作
    - 动作取决于 `keeplive/action` 配置：
      - `"reboot"`：重启系统（如果运行时间 > 180 秒）
      - `"reset"`：重置接口设备
      - 其他：重置连接

    示例，处理第一个 WISP 网络的保活失败
    ```shell
    ifname@wisp.keepoff
    ttrue
    ```   

### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_ifname_wisp(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "ifname@wisp", "status") == NULL)
        return -1;
    ok = ssets_string("ifname@wisp", "value", "status");
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

/* 示例: scall("ifname@wisp", "status", NULL); 如果是 JSON 则调用 talk_free */
```
