## wifi@a — 5.8G 射频管理
管理 5.8G 射频

### 配置 ( `wifi@a` )
```json
// 属性说明
{
    "mode":"射频模式",                 // [ "a", "anac", "ac" ]
                                                // "a" 为 11A
                                                // "anac" 为 11A/AC
                                                // "ac" 为 11AC
    
    "bandwidth":"射频带宽",       // [ "40", "80", "160" ]
                                                // "40" 为 40M
                                                // "80" 为 80M
                                                // "160" 为 160M

    "channel":"射频信道",           // [ 0, 36-165 ], "0" 为自动选择信道
    "beacon":"信标间隔",          // [ number ]
    "country":"国家代码",            // [ "cn", "en", "jp", ... ]

    "dtim":"射频 DTIM",                 // [ number ]
    "ldpc":"是否使用 LDPC",            // [ "disable", "enable" ]
    "shortgi":"短 GI",                // [ "disable", "enable" ]
    "stbc":"射频 STBC"                  // [ "disable", "enable" ]
}
```

示例, 显示 5.8G 射频所有配置
```shell
wifi@a
{
    "mode":"ac",               # 5.8G 射频为 11AC
    "bandwidth":"80",          # 5.8G 带宽 80M
    "channel":"165",           # 5.8G 信道为 165

    "beacon":"100",            # 5.8G 信标间隔为 100ms
    "dtim":"1", 
    
    "ldpc":"enable",           # 启用 LDPC
    "shortgi":"enable",        # 启用短 GI
    "stbc":"enable"            # 启用 STBC
}
```  

示例, 修改 5.8G 射频信道为自动
```shell
wifi@a:channel=0
ttrue
```

示例, 修改 5.8G 射频信道为 36
```shell
wifi@a:channel=36
ttrue
```

示例, 同时修改多个属性 (**合并**)
```shell
wifi@a|{"mode":"ac","bandwidth":"80","channel":"0"}
ttrue
```

### 组件 API
**可直接调用** 的 API: `wifi@a.method`, `wifi@a2.method`, ... (HE / eline / HTTP `/he`).

+ `chlist[]` **获取 5.8G 射频信道列表**   
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

    示例, 获取 5.8G 射频信道列表
    ```shell
    wifi@a.chlist
    {
        "36":{},
        "40":{},
        "44":{},
        "48":{},
        "52":{},
        "56":{},
        "60":{},
        "64":{},
        "149":{},
        "153":{},
        "157":{},
        "161":{},
        "165":{}
    }
    ```

+ `stalist[]` **获取 5.8G 射频的客户端列表**   
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

    示例, 获取 5.8G 射频的客户端列表
    ```shell
    wifi@a.stalist
    {
        "78:11:DC:92:D3:9E":                  // 客户端 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "88:C3:97:75:1B:C0":                 // 客户端 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@assid"
        },
        "40:31:3C:4D:78:35":                 // 客户端 3
        {
            "apidx":"0",
            "livetime":"14:52:22:2",
            "rssi":"-61",
            "ifdev":"wifi@assid"
        }
    }
    ```

+ `stabeat[ MAC address ]` **断开客户端连接**   
    - 失败返回 tfalse
    - 错误返回 terror    
    - 成功返回 ttrue

    示例, 从 5.8G 射频断开客户端 00:03:7F:13:BD:30
    ```shell
    wifi@a.stabeat[ 00:03:7F:13:BD:30 ]
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

static int example_config_wifi_a(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@a", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@a", "value", "status");
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

/* Example: scall("wifi@a", "status", NULL); then talk_free if JSON */
```
