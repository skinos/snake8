## forward@mark — 数据包标记
使用标记 ID 标记数据包

### 配置 ( forward@mark )

```json
// 属性介绍
{
    "rule name":                    // [ string ], 用户可自定义规则名称
    {
        "markid":"使用此 ID 标记数据包",                       // [ number ], 范围 1-4294967295
                                                                            // 小于 100 为系统保留
                                                                            // 建议用户自定义规则使用 100 以上的值
        "src":"使用源 IP 地址选择数据包",               // [ ip address ]
        "dest":"使用目标 IP 地址选择数据包",         // [ ip address ]
        "protocol":"协议类型",                                    // [ "all", "tcp", "udp", "tcpudp" ], all 表示所有协议, tcpudp 表示 tcp 和 udp
        "srcport":"源端口",                                       // [ number ], 默认为所有源端口
        "destport":"目标端口"                                  // [ number ], 默认为所有目标端口
    }
    // ... 更多规则
}
```

示例, 显示所有标记规则配置
```shell
forward@mark
{
    
    "myCustom1":                       # 规则名称为 myCustom1
    {                                  # 将源地址为 192.168.8.250 且目标地址为 202.94.22.38 的数据包标记为 300
        "markid":"300",
        "src":"192.168.8.250",
        "dest":"202.94.22.38"
    },
    "youCustom":                       # 规则名称为 youCustom
    {                                  # 将源地址为 192.168.8.251 且 tcp 协议源端口为 1000-2000 且目标端口为 80/8080 的数据包标记为 301
        "markid":"301",
        "src":"192.168.8.251",
        "protocol":"tcp",
        "srcport":"1000-2000",
        "destport":"80,8080"
    }
}
```  
