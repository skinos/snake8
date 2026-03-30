***
## 策略路由
管理策略路由

#### 配置( forward@rule )
```json
// 属性介绍 
{
    "rule name":                                               // [ string ], 用户可自定义规则名称
    {
        "pref":"规则优先级",                                      // [ nubmer ], 范围 0-4294967295, 值越小优先级越高
                                                                                // 0 为 local 表优先级
                                                                                // 32766 为 main 表优先级
                                                                                // 50000 为 default 表优先级
                                                                                // 默认为 40000

        "markid":"使用标记 ID 选择数据包",                       // [ nubmer ], 范围 1-4294967295
                                                                                // 小于 100 为系统保留
                                                                                // 建议用户自定义规则使用 100 以上的值
        "srcifname":"使用源接口选择数据包",          // [ "ifname@lan", "ifname@lan2", ... ], 接口名称
        "src":"使用源 IP 地址选择数据包",               // [ ip address, network ]
        "srcmask":"使用源 IP 地址掩码选择数据包",   // [ netmask ], 当 "src" 为网络地址时必填

        "tid":"转发到哪个路由表"                             // [ number ], 范围 0-255
                                                                                // 0 为 local 表
                                                                                // 253 为 default 表
                                                                                // 254 为 main 表
                                                                                // 255 为 local 表
                                                                                // 小于 100 为系统保留
                                                                                // 建议用户自定义规则使用 100 以上的值 (并避免 253,254,255)
    }
    // ... 更多规则
}

```   
示例, 显示当前所有策略规则
```shell
forward@rule
{
    "myCustom1":                       # 规则名称为 myCustom1
    {                                  # 来自 ifname@lan 且源地址为 1.1.1.1 的数据包路由到路由表 101, 优先级为 38000
        "pref":"38000",
        "srcifname":"ifname@lan",
        "src":"1.1.1.1",
        "srcmask":"255.255.255.255",
        "tid":"101"
    },
    "youCustom":                       # 规则名称为 youCustom
    {                                  # 来自 ifname@lan 且标记 ID 为 300 的数据包路由到路由表 102, 默认优先级为 40000
        "srcifname":"ifname@lan",
        "markid":"300",
        "tid":"102"
    }
}
```  

### 组件 API

+ `status[]` **获取当前策略规则**
    - 失败返回 NULL, 错误返回 terror  
    - 成功返回描述信息的 json  
    ```json
    // 方法返回的 json 属性介绍
    {
        "rule name",                                                 // [ string ], 用户自定义的规则名称
        {
            "pref":"规则优先级",                                      // [ nubmer ], 范围 0-4294967295, 值越小优先级越高
                                                                                    // 0 为 local 表优先级
                                                                                    // 32766 为 main 表优先级
                                                                                    // 50000 为 default 表优先级
                                                                                    // 默认为 40000        
            "markid":"使用标记 ID 选择数据包",                       // [ nubmer ], 范围 1-4294967295
                                                                                    // 小于 100 为系统保留
                                                                                    // 建议用户自定义规则使用 100 以上的值
            "srcifname":"使用源接口选择数据包",          // [ "ifname@lan", "ifname@lan2", ... ], ifname
            "src":"使用源 IP 地址选择数据包",               // [ ip address, network ]
            "srcmask":"使用源 IP 地址掩码选择数据包",   // [ netmask ], 当 "src" 为网络地址时必填
            "tid":"转发到哪个路由表"                             // [ number ], 范围 0-255
                                                                                    // 0 为 local 表
                                                                                    // 253 为 default 表
                                                                                    // 254 为 main 表
                                                                                    // 255 为 local 表
                                                                                    // 小于 100 为系统保留
                                                                                    // 建议用户自定义规则使用 100 以上的值 (并避免 253,254,255)

        }
        // ... 更多规则
    }
    ```   

    示例, 获取当前策略规则
    ```shell
    forward@rule.status
    {
        "myCustom1":                       # 规则名称为 myCustom1
        {                                  # 来自 ifname@lan 且源地址为 1.1.1.1 的数据包路由到路由表 101, 优先级为 38000
            "srcifname":"ifname@lan",
            "src":"1.1.1.1",
            "srcmask":"255.255.255.255",
            "tid":"101",
            "pref":"38000"
        },
        "youCustom":                       # 规则名称为 youCustom
        {                                  # 来自 ifname@lan 且标记 ID 为 300 的数据包路由到路由表 102, 默认优先级为 40000
            "srcifname":"ifname@lan",
            "markid":"300",
            "tid":"102"
        }
    }
    ```   

+ `add[ name, [src], [srcmask], [srcifname], [markid], tid, [pref] ]` **添加策略规则**
    - 成功返回 ttrue
    - 失败返回 tfalse
    - 错误返回 terror   

    示例, 添加名为 senser 的规则, 使源地址 192.168.2.12 路由到路由表 1, 优先级为 33000
    ```shell
    forward@rule.add[ senser, 192.168.2.12, , , , 1, 33000 ]
    ttrue
    ```

    示例, 添加名为 video 的规则, 使所有其他访问路由到路由表 2, 优先级为 33300
    ```shell
    forward@rule.add[ video, , , , , 2, 33000 ]
    ttrue
    ```

+ `delete[ name ]` **删除策略规则**
    - 成功返回 ttrue
    - 失败返回 tfalse
    - 错误返回 terror   

    示例, 删除名为 video 的策略规则
    ```shell
    forward@rule.delete[ video ]
    ttrue
    ```   

    示例, 删除名为 senser 的策略规则
    ```shell
    forward@rule.delete[ senser ]
    ttrue
    ```   



+ `tidlist[]` **列出所有路由表**
    - 失败返回 NULL, 错误返回 terror   
    - 错误返回 terror   
    - 成功返回描述信息的 json  
    ```json
    // 方法返回的 json 属性介绍
    {
        "table id":"table name"                             // [ number ]: [ string ]
        // ... 更多 tid
    }
    ```   

    示例, 获取当前路由表列表
    ```shell
    forward@rule.tidlist
    {
        "1":"1",
        "2":"2",
        "253":"local",
        "254":"main",
        "255":"default",
    }
    ```   
