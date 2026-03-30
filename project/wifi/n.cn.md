## wifi@n — 2.4G 射频管理
管理 2.4G 射频。使用哪种驱动程序和主机协议栈取决于**具体产品**; **`arch`** 区域提供板级集成, 而配置通过与其他组件相同的 **`land`** / `he` 模型流转。

### 配置 ( `wifi@n` )
```json
// 属性说明
{
    "mode":"射频模式",                 // [ "n", "b", "bg", "x" ]
                                                // "n" 为 11N
                                                // "b" 为 11B
                                                // "bg" 为 11BG
                                                // "x" 为 11AX
    
    "bandwidth":"射频带宽",       // [ "20", "2040", "40" ]
                                                // "20" 为 20M
                                                // "2040" 为 20M 和 40M
                                                // "40" 为 40M

    "channel":"射频信道",           // [ 0-14 ], "0" 为自动选择信道
    "beacon":"信标间隔",          // [ number ]
    "country":"国家代码",            // [ "cn", "en", "jp", ... ]

    "dtim":"射频 DTIM",                 // [ number ]
    "ldpc":"是否使用 LDPC",            // [ "disable", "enable" ]
    "shortgi":"短 GI",                // [ "disable", "enable" ]
    "stbc":"射频 STBC"                  // [ "disable", "enable" ]
}
```

示例, 显示 2.4G SSID 所有配置
```shell
wifi@n
{
    "mode":"n",                # 2.4G 射频为 11N
    "bandwidth":"40",          # 2.4G 带宽 40M
    "channel":"11",            # 2.4G 信道为 11

    "beacon":"100",            # 2.4G 信标间隔为 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # 启用 LDPC
    "shortgi":"enable",        # 启用短 GI
    "stbc":"enable"            # 启用 STBC
}
```  

示例, 修改 2.4G 射频信道为自动
```shell
wifi@n:channel=0
ttrue
```

示例, 修改 2.4G 射频信道为 11
```shell
wifi@n:channel=11
ttrue
```

示例, 同时修改多个属性 (**合并**)
```shell
wifi@n|{"mode":"an","bandwidth":"40","channel":"0"}
ttrue
```

### 组件 API
**可直接调用** 的 API: `wifi@n.method`, `wifi@n2.method`, ... (HE / eline / HTTP `/he`).

+ `chlist[]` **获取 2.4G 射频信道列表**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此列表的 json   
    ```json
    // 方法返回的 talk 属性说明
    {
        "channel number":{}                // [ number ]:{}
        // 更多信道编号
    }
    ```

    示例, 获取 2.4G 射频信道列表
    ```shell
    wifi@n.chlist
    {
        "1":{},
        "2":{},
        "3":{},
        "4":{},
        "5":{},
        "6":{},
        "7":{},
        "8":{},
        "9":{},
        "10":{},
        "11":{},
        "12":{},
        "13":{}
    }
    ```

+ `stalist[]` **获取 2.4G 射频的客户端列表**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此列表的 json   
    ```json
    // 方法返回的 talk 属性说明
    {
        "client MAC address":              // [ MAC address ]:{}
        {
            "livetime":"在线时间",               // [ hour:minute:second:day ]
            "rssi":"信号强度",               // [ number ], 单位可能为 dBm 或 %
        }
        // ... 更多客户端
    }
    ```

    示例, 获取 2.4G 射频的客户端列表
    ```shell
    wifi@n.stalist
    {
        "78:11:DC:92:D3:9E":                  // 客户端 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "88:C3:97:75:1B:C0":                 // 客户端 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "40:31:3C:4D:78:35":                 // 客户端 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@nssid"
        }
    }
    ```

+ `stabeat[ MAC address ]` **断开客户端连接**  
    - 失败返回 tfalse
    - 错误返回 terror    
    - 成功返回 ttrue

    示例, 从 2.4G 射频断开客户端 00:03:7F:13:BD:30
    ```shell
    wifi@n.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

### 生命周期 API
+ `setup[]` **启动组件服务**, *成功返回 ttrue, 失败返回 tfalse*
    - 当 **`setup[]`** 已为此组件连线时, 由平台 **`init`** 调度调用。

+ `shut[]` **停止组件服务**, *成功返回 ttrue, 失败返回 tfalse*
    - 当 **`shut[]`** 已连线时, 由平台 **`uninit`** 调度调用。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_wifi_n(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@n", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@n", "value", "status");
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

/* Example: scall("wifi@n", "status", NULL); then talk_free if JSON */
```
