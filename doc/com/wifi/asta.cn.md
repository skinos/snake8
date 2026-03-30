## wifi@asta — 5.8G 客户端管理
管理 5.8G 客户端


### 配置 ( `wifi@asta` )
```json
// 属性说明
{
    "status":"启用或禁用此功能",     // [ "enable", "disable" ]

    // 无线连接
    "peer":"要连接的 SSID",              // [ string ]
    "peermac":"要连接的 BSSID",          // [ mac address ]
    "peermode":"连接模式",       // [ "hidden" ] 表示对端不广播 SSID。在隐藏模式下, channel 不能为空  
    "channel":"无线信道",          // [ number ], 0, 36-165, 0 为自动
    "nossid":"禁用本地 SSID",           // [ "disable", "enable" ], 连接后禁用本地 SSID
    "secure":"安全模式",           // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                            // [ disable ] 为无安全
                                                            // [ wpapsk ] 为 WPAPSK
                                                            // [ wpa2psk ] 为 WPA2PSK
                                                            // [ wpapskwpa2psk ] 为 WPA 混合
    "wpa_encrypt":"WPA 加密",           // [ "aes", "tkip", "tkipaes" ]
                                                            // [ aes ] 为 AES
                                                            // [ tkip ] 为 TKIP
                                                            // [ tkipaes ] 为自动
    "wpa_key":"WPA 密钥"                    // [ string ], 至少 8 个字符的字符串。当 "secure" 为 "wpapsk" 或 "wpa2psk" 或 "wpapskwpa2psk" 时此参数为必填

}
```

示例, 显示 5.8G 客户端所有配置
```shell
{
    # WIFI 对端
    "peer":"V520-D21D20-5G",    # 连接 V520-D21D20-5G
    "secure":"wpapsk",       # 安全模式为 WPAPSK
    "wpa_encrypt":"aes",     # 加密使用 AES
    "wpa_key":"87654321",    # 密码 87654321
}
```

示例, 修改 5.8G 客户端连接的 SSID
```shell
wifi@asta:peer=Myhotpot
ttrue
wifi@asta:secure=wpapsk
ttrue
wifi@asta:wpa_key=88888888
ttrue
# 也可以使用一条命令完成上述三条命令的操作
wifi@asta|{"peer":"Myhotpot", "secure":"wpapsk", "wpa_key":"88888888"}
ttrue
```

示例, 禁用 5.8G 客户端连接
```shell
wifi@asta:status=disable
ttrue
```

示例, 启用 5.8G 客户端连接
```shell
wifi@asta:status=enable
ttrue
```


### 组件 API
**可直接调用** 的 API: `wifi@asta.method`, `wifi@asta2.method`, ... (HE / eline / HTTP `/he`).

+ `status[]` **获取 5.8G 客户端信息**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此信息的 json   
    ```json
    // 方法返回的 talk 属性说明
    {
        "status":"当前状态",        // [ "uping", "down", "up" ]
                                             // "uping" 为正在连接
                                             // "down" 为网络已断开
                                             // "up" 为网络连接成功

        "peer":"对端 SSID",              // [ string ]
        "peermac":"对端 BSSID",          // [ MAC address ]
        "channel":"对端信道",        // [ 0, 36-165 ]
        "rate":"连接速率",           // [ number ], 单位为 M
        "rssi":"对端 RSSI",              // [ number ], 单位为 dBm
        "signal":"信号等级",         // [ 0, 1, 2, 3 4 ], 0 为无信号, 1 为最弱信号, 4 为最强信号
    }
    ```

    ```shell
    # 示例, 获取 5.8G 客户端信息
    wifi@asta.status
    {

        "status":"up",                     # 连接成功
        "peer":"TP-link-2231-5G",            # 对端为 TP-link-2231-5G
        "peermac":"70:3A:D8:54:BC:90",    # 对端 BSSID 为 70:3A:D8:54:BC:90
        "channel":"10",                   # 信道为 10
        "rate":"270",                     # 速率为 270M
        "rssi":"-41",                     # RSSI 为 -41dBm
        "signal":"3"                      # 信号等级为 3
    }
    ```

+ `netdev[]` **获取 5.8G 客户端网络设备名**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此信息的字符串   
    
    示例, 获取 5.8G 客户端网络设备名
    ```shell
    wifi@asta.netdev
    ath11
    ```


+ `aplist[]` **使用 5.8G 客户端扫描周围 AP**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此信息的 json   
    ```json
    // 方法返回的 talk 属性说明
    {
        "AP BSSID":                                   // [ mac address ]
        {
            "ssid":"SSID 名称",                           // [ string ]
            "channel":"信道编号",                   // [ number ], 0,36-165, 0 为自动
            "secure":"安全模式",                  // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                                 // "disable" 为无安全
                                                                 // "wpapsk" 为 WPAPSK
                                                                 // "wpa2psk" 为 WPA2PSK
                                                                 // "wpapskwpa2psk" 为 WPA 混合
            "wpa_encrypt":"WPA 加密",                  // [ "aes", "tkip", "tkipaes" ]
                                                                 // "aes" 为 AES
                                                                 // "tkip" 为 TKIP
                                                                 // "tkipaes" 为自动
            "sig":"信号等级(%)",                      // [ number ]
            "signal":"信号等级[0-4]",                 // [ "0", "1", "2", "3", "4" ]
            "chext":"扩展信道",                     // [ "none", "below", "above" ]
            "mode":"无线制式"                      // [ string ]
        }
        // ... 更多 AP
    }
    ```

    示例, 通过 5.8G 客户端扫描获取周围 AP
    ```shell
    wifi@asta.aplist
    {
        "80:EA:07:15:0E:E6":                    # 扫描到的第一个 AP
        {
            "ssid":"1411",                                 # 第一个 AP 的 SSID
            "channel":"36",                                # 第一个 AP 的信道
            "secure":"wpapskwpa2psk",                      # 安全模式为 WPA 混合
            "wpa_encrypt":"aes",                           # 加密类型为 AES
            "sig":"70",                                    # 信号为 70%
            "signal":"3",                                  # 信号等级为 3, 范围 0-4
            "chext":"below",                               # 扩展信道为 below
            "mode":"11a/n/ac ABOVE"
        },
        "B4:82:C5:80:22:81":                    # 扫描到的第二个 AP
        {
            "ssid":"dimmalex-work",
            "channel":"42",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"52",
            "signal":"3",
            "chext":"none",
            "mode":"11a/n/ac"
        },
        "8C:74:A0:D6:68:B0":                    # 扫描到的第三个 AP
        {
            "ssid":"CMCC-ktfK",
            "channel":"165",
            "secure":"wpapskwpa2psk",
            "wpa_encrypt":"aes",
            "sig":"0",
            "signal":"0",
            "chext":"none",
            "mode":"11a/n/ac"
        }
    }
    ```

+ `shut[]` **关闭 5.8G 客户端**   
    - 失败返回 tfalse
    - 错误返回 terror    
    - 成功返回 ttrue

    示例, 关闭 5.8G 客户端
    ```shell
    wifi@asta.shut
    ttrue
    ```

+ `setup[]` **启动 5.8G 客户端**   
    - 失败返回 tfalse
    - 错误返回 terror    
    - 成功返回 ttrue

    示例, 启动 5.8G 客户端
    ```shell
    wifi@asta.setup
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 当在 `project/wifi` 中为此对象定义时, 它们启动/停止底层无线服务。参考 **wifi** FPK 不会为这些对象调度 **`init`/`uninit`**; 启动通常由驱动程序、**network** 协议栈或产品集成驱动。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_wifi_asta(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@asta", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@asta", "value", "status");
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

/* Example: scall("wifi@asta", "status", NULL); then talk_free if JSON */
```
