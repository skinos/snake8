## ifname@lte — LTE/NR 网络管理
管理 LTE/NR 网络和 4G/NR 基带链路。它将此处的**逻辑 LTE 接口**与**调制解调器管理**（`modem@lte`、... -- 参见 [`../modem/lte.cn.md`](../modem/lte.cn.md)）以及用于上行链路调度的**网络框架**（[`../network/frame.cn.md`](../network/frame.cn.md)）配对。  
通常 `ifname@lte` 是第一个 LTE/NR 网络实例。如果有多个 LTE/NR 调制解调器，`ifname@lte2` 是第二个实例，编号依次递增。


### 配置 ( `ifname@lte` )
**ifname@lte** 是第一个 LTE 网络   
**ifname@lte2** 是第二个 LTE 网络   

```json
// 属性介绍
{
    "status":"系统启动时启动",    // [ "enable", "disable" ]

    // LTE 调制解调器 PDP 的配置文件属性
    "pin":"SIM 卡 PIN 码",                       // [ string ]
    "profile":"使用自定义 PDP 配置文件",        // [ "disable", "enable" ]
    "profile_cfg":                             // 自定义配置文件设置，当 "profile" 为 "enable" 时使用
    {
        "dial":"拨号号码",                     // [ number ]
        "cid":"拨号 CID",                         // [ number ], 默认为 1
        "type":"IP 地址类型",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"认证方式",           // [ "pap", "chap", "papchap" ]
        "apn":"APN 名称",                         // [ string ]
        "user":"用户名",                       // [ string ]
        "passwd":"用户密码"                  // [ string ]
    },

    // 备用 SIM 卡配置
    // 注意：当前 `ifname@lte` 实现主要管理链路行为。
    // 这些备用 SIM 字段是调制解调器端的功能，仅在底层调制解调器组件支持时才生效。
    "bsim":"备用 SIM 卡功能",                         // [ "disable", "enable" ]
    "bsim_cfg":                                               // 备用 SIM 设置，当 "bsim" 为 "enable" 时使用
    {
        "mode":"指定活动 SIM 卡",                           // [ "auto", "bsim", "msim", "detect" ]
                                                                        // "auto" 根据规则自动切换
                                                                        // "bsim" 使用备用 SIM 卡
                                                                        // "msim" 使用主 SIM 卡
                                                                        // "detect" 需要检测 IO 支持的自动检测
        "signal_failed":"信号检测失败多少次后切换 SIM 卡",   // [ number ]
        "attach_failed":"附着到网络失败多少次后切换 SIM 卡", // [ number ]
        "failed":"连接互联网失败多少次后切换 SIM 卡",       // [ number ]
        "failover":"备用 SIM 卡使用时长",                                       // [ number ], 单位为秒
        "keeplive_switch":"保活失败时切换",                                    // [ "disable", "enable" ]

        // 备用配置文件属性
        "pin":"SIM 卡 PIN 码",                       // [ string ]
        "profile":"自定义配置文件",            // [ "disable", "enable" ]
        "profile_cfg":                             // 自定义配置文件保存在此处，当 "profile" 值为 enable 时使用此 JSON
        {
            "dial":"拨号号码",                     // [ number ]
            "cid":"拨号 CID",                         // [ number ], 默认为 1
            "type":"IP 地址类型",                 // [ "ipv4", "ipv6", "ipv4v6" ]
            "apn":"APN 名称",                         // [ string ]
            "user":"用户名",                       // [ string ]
            "passwd":"用户密码"                  // [ string ]
        }
    },

    // SIM 卡检测属性
    "need_simcard":"必须检测到 SIM 卡",                                                 // [ "enable", "disable" ]
                                                                                                    // "enable" 要求检测 SIM 卡，检测失败将重置调制解调器
                                                                                                    // "disable" 允许在没有 SIM 卡的情况下运行
    "simcard_failed_threshold":"首次失败重置时间",                                   // [ number ], 默认 60 秒
    "simcard_failed_threshold2":"第二次失败重置时间",                                 // [ number ], 默认 180 秒
    "simcard_failed_threshold3":"第三次失败重置时间",                                  // [ number ], 默认 300 秒
    "simcard_failed_everytime":"每次失败重置时间",                                   // [ number ], 默认 1800 秒

    // 信号/PLMN 检测属性
    "need_plmn":"必须注册到 PLMN",                                                       // [ "enable", "disable" ]
                                                                                                    // "enable" 要求 PLMN 注册
                                                                                                    // "disable" 跳过 PLMN 注册检查
    "need_signal":"信号必须有效",                                                      // [ "enable", "disable" ]
                                                                                                    // "enable" 要求有效的信号强度
                                                                                                    // "disable" 跳过信号检查
    "signal_failed_threshold":"首次失败重置时间",                                    // [ number ], 默认 120 秒
    "signal_failed_threshold2":"第二次失败重置时间",                                  // [ number ], 默认 300 秒
    "signal_failed_threshold3":"第三次失败重置时间",                                   // [ number ], 默认 600 秒
    "signal_failed_everytime":"每次失败重置时间",                                    // [ number ], 默认 1800 秒

    // 附着检测属性
    "need_attach":"必须附着成功",                                                       // [ "enable", "disable" ]
                                                                                                    // "enable" 要求成功附着到网络
                                                                                                    // "disable" 跳过附着检查
    "attach_failed_threshold":"首次失败重置时间",                                    // [ number ], 默认 60 秒
    "attach_failed_threshold2":"第二次失败重置时间",                                  // [ number ], 默认 180 秒
    "attach_failed_threshold3":"第三次失败重置时间",                                   // [ number ], 默认 600 秒
    "attach_failed_everytime":"每次失败重置时间",                                    // [ number ], 默认 1800 秒

    // IPv4
    "tid":"路由表标识号",            // [ number ] 独占路由表 ID，仅用于多 WAN
    "metric":"默认路由度量值",          // [ number  ]
    "mode":"IPV4 地址模式",               // [ "dhcpc" ] DHCP 客户端模式, [ "static" ] 手动设置, [ "ppp" ] PPP 拨号
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
    "ppp":                                    // "mode" 为 "ppp" 时的详细配置
    {
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
    "automatic":                              // "method" 为 "automatic" 时的详细配置
    {
        "mode":"获取 IPv6 的模式",                  // [ "try", "force", "disable" ]
        "prefix":"请求的 IPv6 前缀长度",    // [ "auto", "48", "52", "56", "60", "60", "disable" ]
        "custom_resolve":"自定义 DNS",                   // [ "disable", "enable" ]
        "resolve":"自定义 DNS1",                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
        "resolve2":"自定义 DNS2"                         // [ ipv6 address ], 当 "custom_resolve" 为 "enable" 时生效
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
                                                // [ "reset" ] 重置调制解调器
                                                // [ others ] 重新拨号连接
        "icmp":                                                             // "type" 为 "icmp" 时的详细配置
        {
            "dest":                                                           // ICMP 保活的目标地址
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

示例，显示第一个 LTE 的所有配置   
```shell
ifname@lte
{
    // PDP 配置文件
    "profile":"enable",                # 自定义 APN 配置文件
    "profile_cfg":
    {
        "dial":"*99#",                     # 拨号号码为 *99#
        "type":"ipv4v6",                   # IP 地址类型为 ipv4 和 ipv6
        "apn":"internet",                  # APN 为 internet
        "user":"card",                     # 用户名为 card
        "passwd":"card"                    # 密码为 card
    }

    "mode":"ppp",                      # PPP 模式
    "ppp":                                 # 当 "mode" 为 ppp 时使用 ppp 配置
    {
        "lcp_echo_interval":"10",          # LCP 回显间隔为 10 秒
        "lcp_echo_failure":"12"            # LCP 回显失败次数为 12
    },
    "masq":"enable",                                 # 出站流量共享接口 IPv4 地址访问互联网

    "method":"slaac",                                # IPv6 地址模式为 slaac

    "keeplive":                                      # 保活机制配置保存在此处
    {
        "type":"recv",                               # 使用统计接收数据包进行保活
        "recv":                                      # 如果在 20X30 秒（10 分钟）内未收到 1 个数据包，则认为链路不可用
        {
            "timeout":"20",
            "failed":"30",
            "packets":"1"
        }
    }
}
```   

示例，显示第二个 LTE/NR 的所有配置   
```shell
ifname@lte2
{
    // PDP 配置文件
    "profile":"enable",                # 自定义 APN 配置文件
    "profile_cfg":
    {
        "dial":"*99#",                     # 拨号号码为 *99#
        "type":"ipv4v6",                   # IP 地址类型为 ipv4 和 ipv6
        "apn":"internet",                  # APN 为 internet
        "user":"card",                     # 用户名为 card
        "passwd":"card"                    # 密码为 card
    }

    "mode":"ppp",                      # PPP 模式
    "ppp":                                 # 当 "mode" 为 ppp 时使用 ppp 配置
    {
        "lcp_echo_interval":"10",          # LCP 回显间隔为 10 秒
        "lcp_echo_failure":"12"            # LCP 回显失败次数为 12
    },
    "masq":"enable",                                 # 出站流量共享接口 IPv4 地址访问互联网

    "method":"slaac",                                # IPv6 地址模式为 slaac

    "keeplive":                                      # 保活机制配置保存在此处
    {
        "type":"dns",                               # 使用 DNS 计数进行保活
        "dns":                                      # 检测 DNS 服务器超时 8 秒，失败 4 次提示失败，检测成功后休眠 5 秒
        {
            "timeout":"8",
            "failed":"4",
            "interval":"5"
        }
    }
}
```   

示例，将第一个 LTE 网络的保活模式修改为 icmp  
```shell
ifname@lte:keeplive/type=icmp
ttrue
```   

示例，修改第一个 LTE 网络的 ICMP 保活目标地址  
```shell
ifname@lte:keeplive/icmp/dest/test=8.8.8.8            # 修改 ICMP 保活第一个目标地址为 8.8.8.8
ttrue
ifname@lte:keeplive/icmp/dest/test2=8.8.4.4           # 修改 ICMP 保活第二个目标地址为 8.8.4.4 
ttrue
ifname@lte:keeplive/icmp/dest/test3=114.114.114.114   # 修改 ICMP 保活第三个目标地址为 114.114.114.114
ttrue
# 也可以用一条 JSON 更新完成以上三条命令
ifname@lte:keeplive/icmp/dest|{"test":"8.8.8.8", "test2":"8.8.4.4", "test3":"114.114.114.114"}
ttrue
```   

示例，将第一个 LTE 网络的模式修改为 ppp  
```shell
ifname@lte:mode=ppp
ttrue
```   

示例，启用自定义配置文件并设置 APN
```shell
ifname@lte:profile=enable
ttrue
ifname@lte:profile_cfg/apn=NewAPN
ttrue
```   

也可以用一条 JSON 更新完成以上命令
```shell
ifname@lte|{"profile":"enable","profile_cfg":{"apn":"NewAPN"}}
ttrue
```   

示例，禁用第一个 LTE 网络   
```shell
ifname@lte:status=disable
ttrue
```     

示例，禁用第二个 LTE 网络
```shell
ifname@lte2:status=disable
ttrue
```


### 组件 API
**可直接调用的** API：`ifname@lte.method`、`ifname@lte2.method`、...

**ifname@lte** 是第一个 LTE 网络  
**ifname@lte2** 是第二个 LTE/NR 网络

+ `status[]` **获取 LTE 网络信息**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回 JSON 状态信息   
    ```json
    // API 返回的属性介绍
    {
        "status":"当前状态",        // [ "nodevice", "reset", "setup", "register", "idle", "noimsi", "noimei", "uping", "block", "up", "failed", "down" ]
                                             // "nodevice" 找不到对应的模块
                                             // "reset" 调制解调器正在重置
                                             // "setup" 调制解调器正在初始化
                                             // "register" 调制解调器正在注册网络
                                             // "idle" 调制解调器已激活但暂时未准备好拨号
                                             // "noimsi" IMSI 锁定检查失败或不可用
                                             // "noimei" IMEI 锁定检查失败或不可用
                                             // "uping" 正在连接
                                             // "block" 等待保活检查恢复
                                             // "up" 已准备好访问互联网（信号/网络/SIM 卡正常）
                                             // "failed" 保活失败
                                             // "down" 调制解调器已关闭

        "mode":"IPV4 地址模式",     // [ "dhcpc" ] DHCP 模式, [ "static" ] 手动设置, [ "ppp" ] PPP 拨号
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

        // LTE/NR 基带状态，参数与 modem@lte 或 modem@lte2 相同
        "imei":"IMEI 号码",           // [ string ]
        "imsi":"IMSI 号码",           // [ string ]
        "iccid":"ICCID 号码",         // [ number, "nosim", "pin", "puk" ]
                                                // number 表示 ICCID
                                                // "nosim" 表示未检测到 SIM 卡
                                                // "pin" 表示 SIM 卡需要 PIN 码
                                                // "puk" 表示 SIM 卡 PIN 码错误
        "plmn":"MCC 和 MNC",           // [ number, "noreg", "unreg", "dereg" ]
                                                // number 表示 MCC 和 MNC
                                                // "noreg" 表示无法注册到运营商
                                                // "unreg" 表示当前未注册
                                                // "dereg" 表示注册被运营商拒绝
        "name":"调制解调器名称",             // [ string ], LTE 调制解调器型号或名称
        "operator":"运营商名称",      // [ string ]
        "nettype":"网络类型",        // 格式因模块而异
                                         // 2G 通常显示 GSM, GPRS, EDGE, CDMA
                                         // 3G 通常显示 WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G 通常显示 LTE, FDD, TDD
        "signal":"信号等级",         // [ "0", "1", "2", "3", "4" ], "0" 无信号, "1" 最弱, "4" 最强
        "rssi":"信号强度",       // [ number ], 单位为 dBm
        "csq":"CSQ 数值",              // [ number ], 可选
        "rsrp":"RSRP 值",             // [ string ], 可选，格式因模块而异
        "rsrq":"RSRQ 值",             // [ string ], 可选，格式因模块而异
        "sinr":"SINR 值",             // [ string ], 可选，格式因模块而异  
        "band":"当前频段",           // [ string ], 可选，格式因模块而异
        "ci":"小区标识",            // [ string ], 可选
        "lac":"位置区域码",      // [ string ], 可选
        "channel":"信道"   // [ string ], 可选    
    }
    ```   

    示例，获取第一个 LTE 网络信息
    ```shell
    ifname@lte.status
    {
        "mode":"dhcpc",                    # IPv4 连接模式为 DHCP
        "netdev":"usb1",                   # 网络设备为 usb1
        "gw":"10.84.136.246",
        "dns":"120.80.80.80",
        "dns2":"221.5.88.88",
        "ifdev":"modem@lte",
        "ontime":"28826",
        "status":"up",                     # 连接成功
        "delay":"26",
        "ip":"10.84.136.245",
        "mask":"255.255.255.252",
        "livetime":"00:31:58:0",
        "rx_bytes":"4407784",
        "rx_packets":"34234",
        "tx_bytes":"4440236",
        "tx_packets":"47893",
        "mac":"02:50:F4:00:00:00",
        "imei":"868186042111714",
        "ci":"4A37D91",
        "lac":"25E3",
        "plmn":"46001",
        "csq":"23",
        "nettype":"FDD LTE",
        "rsrp":"-97",
        "rssi":"-66",
        "rsrq":"-9",
        "sinr":"-18",
        "band":"LTE BAND 1",
        "channel":"100",
        "signal":"4",
        "operator":"China Unicom",
        "imsi":"460018708133639",
        "iccid":"8986012580155265717",
        "name":"Quectel-EC2X"
    }
    ```   

    示例，获取第二个 LTE 网络信息
    ```shell
    ifname@lte2.status
    {
        "status":"up",                     # 连接成功

        "mode":"dhcpc",                    # IPv4 连接模式为 DHCP
        "netdev":"usb0",                   # 网络设备为 usb0
        "gw":"10.137.89.154",              # 网关为 10.137.89.118
        "dns":"114.114.114.114",           # DNS 为 114.114.114.114
        "dns2":"8.8.8.8",                  # 备用 DNS 为 8.8.8.8
        "ip":"10.137.89.117",              # IP 地址为 10.137.89.117
        "mask":"255.255.255.252",          # 子网掩码为 255.255.255.252
        "livetime":"00:15:50:0",           # 已在线 15 分 50 秒
        "rx_bytes":"1256",                 # 接收 1256 字节
        "rx_packets":"4",                  # 接收 4 个数据包
        "tx_bytes":"1320",                 # 发送 1320 字节
        "tx_packets":"4",                  # 发送 4 个数据包
        "mac":"02:50:F4:00:00:00",         # 网络设备 MAC 地址为 02:50:F4:00:00:00

        "method":"slaac",                  # IPv6 地址模式为 slaac
        "addr":"fe80::50:f4ff:fe00:0",     # 本地 IPv6 地址为 fe80::50:f4ff:fe00:0

        "imei":"867160040494084",          # IMEI 为 867160040494084
        "imsi":"460015356123463",          # IMSI 为 460015356123463
        "iccid":"89860121801097564807",    # ICCID 为 89860121801097564807
        "csq":"3",                         # CSQ 数值为 3
        "signal":"3",                      # 信号等级为 3
        "plmn":"46001",                    # PLMN 为 46001
        "nettype":"WCDMA",                 # 网络类型为 WCDMA
        "rssi":"-107",                     # 信号强度为 -107
        "operator":"China Unicom"          # 运营商名称为 China Unicom
    }
    ```   


+ `netdev[]` **获取网络设备名**  
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回网络设备名字符串  

    示例，获取第一个 LTE 网络的网络设备名
    ```shell
    ifname@lte.netdev
    usb0
    ```   

+ `ifdev[]` **获取接口设备名**   
    - 失败：返回 `NULL`
    - 错误：返回 `terror`   
    - 成功：返回接口设备组件名  

    示例，获取第一个 LTE 网络的接口设备名
    ```shell
    ifname@lte.ifdev
    modem@lte
    ```   

+ `operator[]` **从接口设备获取运营商/配置文件信息**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回调制解调器组件的运营商相关信息

    示例，获取第一个 LTE 网络的运营商信息
    ```shell
    ifname@lte.operator
    {
        // 调制解调器相关字段，通常包含运营商/APN 配置文件信息
    }
    ```

+ `reset[]` **通过接口设备重置 LTE 调制解调器**
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`
    - 成功：返回 `ttrue`

    示例，重置第一个 LTE 调制解调器
    ```shell
    ifname@lte.reset
    ttrue
    ```

+ `lock_imei[]` **设置或查询调制解调器 IMEI 锁定规则**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回调制解调器锁定结果（取决于实现）

    示例，锁定第一个 LTE 的 IMEI
    ```shell
    ifname@lte.lock_imei|{"value":"enable"}
    ttrue
    ```

+ `lock_imsi[]` **设置或查询调制解调器 IMSI 锁定规则**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回调制解调器锁定结果（取决于实现）

    示例，锁定第一个 LTE 的 IMSI
    ```shell
    ifname@lte.lock_imsi|{"value":"enable"}
    ttrue
    ```

+ `custom_set[]` **向调制解调器发送自定义 AT/驱动设置**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回调制解调器响应（取决于实现）

    示例
    ```shell
    ifname@lte.custom_set|{"cmd":"AT+QCFG=\"nwscanmode\",3,1"}
    ```

+ `custom_watch[]` **从调制解调器查询自定义监控值**
    - 失败：返回 `NULL`
    - 错误：返回 `terror`
    - 成功：返回调制解调器响应（取决于实现）

    示例
    ```shell
    ifname@lte.custom_watch|{"cmd":"AT+QNWINFO"}
    ```

+ `shut[]` **关闭调制解调器网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，关闭第一个 LTE 网络
    ```shell
    ifname@lte.shut
    ttrue
    ```   
    示例，关闭第二个 LTE 网络
    ```shell
    ifname@lte2.shut
    ttrue
    ```   

+ `setup[]` **启动调制解调器网络**   
    - 失败：返回 `tfalse`
    - 错误：返回 `terror`   
    - 成功：返回 `ttrue`

    示例，启动第一个 LTE 网络
    ```shell
    ifname@lte.setup
    ttrue
    ```   
    示例，启动第二个 LTE 网络
    ```shell
    ifname@lte2.setup
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 与**组件 API** 中的条目相同。参考 **ifname** 包不会为 **`ifname@lte`** 调度 **`init`/`uninit`**；调制解调器的启动由 **network** 协议栈或产品集成驱动。

### 联合处理程序
+ `keepon[]` **清除连接失败计数器**   
    - 成功：返回 `ttrue`
    - 当网络连接确认存活时调用
    - 重置内部 `connect_failed` 计数器以防止不必要的调制解调器重置

    示例，清除第一个 LTE 网络的连接失败计数器
    ```shell
    ifname@lte.keepon
    ttrue
    ```   

+ `keepoff[]` **处理保活检查失败**   
    - 成功：返回 `ttrue`
    - 当保活检查失败时执行配置的动作
    - 动作取决于 `keeplive/action` 配置：
      - `"reboot"`：重启系统（如果运行时间 > 180 秒）
      - `"reset"`：通过接口设备重置调制解调器
      - 其他：重置连接

    示例，处理第一个 LTE 网络的保活失败
    ```shell
    ifname@lte.keepoff
    ttrue
    ```   


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_ifname_lte(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "ifname@lte", "status") == NULL)
        return -1;
    ok = ssets_string("ifname@lte", "value", "status");
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

/* 示例: scall("ifname@lte", "status", NULL); 如果是 JSON 则调用 talk_free */
```
