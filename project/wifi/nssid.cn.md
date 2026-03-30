## wifi@nssid — 2.4G SSID 管理
管理 2.4G SSID
通常 wifi@nssid 是第一个 2.4G SSID。如果系统中有多个 2.4G SSID, wifi@nssid2 将是第二个 2.4G SSID, 依此递增

### 配置 ( `wifi@nssid` )
**wifi@nssid** 是第一个 2.4G SSID   
**wifi@nssid2** 是第二个 2.4G SSID   

```json
// 属性说明
{
    "status":"SSID 状态",                 // [ "enable", "disable" ]
    "ssid":"SSID 名称",                     // [ string ]
    "isolated":"客户端隔离",          // [ "enable", "disable" ]
    "broadcast":"广播 SSID",           // [ "enable", "disable" ]
    "secure":"安全模式",                 // [ "disable", "wpapsk", "wpa2psk", "wpa3psk", "wpapskwpa2psk", "wpa2pskwpa3psk" ]
                                                // disable 为禁用安全
                                                // wpapsk 为 WPAPSK
                                                // wpa2psk 为 WPA2PSK
                                                // wpa3psk 为 WPA3PSK
                                                // wpapskwpa2psk 为 WPA1/WPA2 PSK 自动
                                                // wpa2pskwpa3psk 为 WPA2/WPA3 PSK 自动
    "wpa_encrypt":"WPA 加密模式",       // [ "aes", "tkip", "tkipaes" ]
                                                // aes 为 AES
                                                // tkip 为 TKIP
                                                // tkipaes 为自动
    "wpa_key":"WPA 密码",               // [ string ], 长度必须大于 8
    "wpa_rekey":"WPA 密钥重协商时间", // [ number ], 单位为秒, 空表示不重协商

    "acl":"访问控制功能",        // [ "disable", "accept", "drop" ]
                                                // disable 
                                                // accept 为白名单
                                                // drop 为黑名单
    "acl_table":                            // 白名单或黑名单, 当 acl 为 "accept" 或 "drop" 时有效
    {
        "MAC Address":"",                   // [ MAC address ]:""
        //... 更多 MAC 地址
    },
    "maxsta":"最大客户端数量",   // [ nubmer ], 空表示无限制

    "wmm":"WMM 状态"                       // [ "enable", "disable" ]
}
```

示例, 显示第一个 2.4G SSID 所有配置
```shell
wifi@nssid
{
    "status":"enable",              # 启用 SSID
    "ssid":"5228-test-2.4g",        # SSID 名称为 5228-test-2.4g
    "isolated":"disable",           # 客户端未隔离, 客户端之间可以互相访问
    "broadcast":"enable",           # 广播 SSID 名称
    "secure":"wpapskwpa2psk",       # 安全模式为 WPA 自动
    "wpa_encrypt":"tkipaes",        # WPA 加密为自动
    "wpa_key":"22222222",           # WPA 加密密钥为 22222222
    "wpa_rekey":"",                 # WPA 加密密钥不重协商
    "acl":"accept",                 # 白名单
    "acl_table":                    # 白名单内容, 仅 MAC 地址 00:22:33:11:33:22/00:22:33:11:33:23/00:22:33:11:33:24/00:22:33:11:33:EB 可以访问
    {
        "00:22:33:11:33:22":"",
        "00:22:33:11:33:23":"",
        "00:22:33:11:33:24":"",
        "00:22:33:11:33:EB":""
    },
    "maxsta":"64",                  # 最多支持 64 个客户端同时接入
    "wmm":"enable"                  # 启用 WMM
}
```  

示例, 修改第一个 2.4G SSID 名称为 myNewSSID
```shell
wifi@nssid:ssid=myNewSSID
ttrue
```

示例, 禁用第一个 2.4G SSID
```shell
wifi@nssid:status=disable
ttrue
```

示例, 启用第一个 2.4G SSID
```shell
wifi@nssid:status=enable
ttrue
```

示例, 修改第一个 2.4G SSID 安全模式为 WPAPSK 并将 WPA 密钥改为 88888888
```shell
wifi@nssid:secure=wpapsk            # 修改安全模式为 WPAPSK
ttrue
wifi@nssid:wpa_encrypt=tkipaes      # 修改 WPA 加密为自动
ttrue
wifi@nssid:wpa_key=88888888         # 修改 WPA 加密密钥为 88888888
ttrue
# 也可以使用一条命令完成上述三条命令的操作
wifi@nssid|{"secure":"wpapsk", "wpa_encrypt":"tkipaes", "wpa_key":"88888888"}
ttrue
```

示例, 禁用第二个 2.4G SSID
```shell
wifi@nssid2:status=disable
ttrue
```

### 组件 API
**可直接调用** 的 API: `wifi@nssid.method`, `wifi@nssid2.method`, ... (HE / eline / HTTP `/he`).

**wifi@nssid** 是第一个 2.4G SSID  
**wifi@nssid2** 是第二个 2.4G SSID

+ `status[]` **获取 SSID 信息**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此信息的 json   
    ```json
    // 方法返回的 talk 属性说明
    {
        "status":"当前状态",              // [ "up", "down" ], "up" 为启用, "down" 为禁用
        "secure":"安全模式",                 // [ "disable", "wpapsk", "wpa2psk", "wpapskwpa2psk" ]
                                                        // disable 为禁用安全
                                                        // wpapsk 为 WPAPSK
                                                        // wpa2psk 为 WPA2PSK
                                                        // wpapskwpa2psk 为 WPA 自动
        "rx_bytes":"接收字节数",             // [ nubmer ]
        "rx_packets":"接收数据包数",         // [ number ]
        "rx_errs":"接收错误包数",       // [ number ]
        "rx_drops":"接收丢弃包数",      // [ nubmer ]
        "tx_bytes":"发送字节数",                // [ nubmer ]
        "tx_packets":"发送数据包数",            // [ nubmer ]
        "tx_errs":"发送错误包数",        // [ nubmer ]
        "tx_drops":"发送丢弃包数",         // [ nubmer ]
        "mac":"MAC 地址",                    // [ MAC address ]
        "livetime":"在线时间",               // [ hour:minute:second:day ]
        "ssid":"SSID 名称",                     // [ string ]
        "bssid":"BSSID",                        // [ MAC address ]
        "channel":"当前信道",            // [ number ]
        "rate":"当前速率"                   // [ number ]
    }
    ```

    示例, 获取第一个 2.4G SSID 状态
    ```shell
    wifi@nssid.status
    {
        "status":"up",
        "secure":"wpapskwpa2psk",
        "rx_bytes":"767164641",
        "rx_packets":"22258095",
        "rx_errs":"489663",
        "rx_drops":"0",
        "tx_bytes":"369735875",
        "tx_packets":"2036548",
        "tx_errs":"0",
        "tx_drops":"0",
        "mac":"00:03:7F:12:88:70",
        "livetime":"14:45:36:2",
        "ssid":"dimmalex-home",
        "bssid":"00:03:7F:12:88:70",
        "channel":"11",
        "rate":"300"
    }
    ```

+ `stalist[]` **获取客户端列表**   
    - 失败返回 NULL
    - 错误返回 terror    
    - 成功返回描述此信息的 json   

    ```json
    // 方法返回的 talk 属性说明
    {
        "client MAC address":              // [ MAC address ]:{}
        {
            "livetime":"在线时间",               // [ hour:minute:second:day ]
            "rssi":"信号强度",               // [ number ], 单位可能为 dBm 或 %
        },
        // ... 更多客户端
    }
    ```

    示例, 获取第一个 2.4G SSID 的客户端列表
    ```shell
    wifi@nssid.stalist
    {
        "78:11:DC:92:D3:9E":                  # 客户端 1
        {
            "apidx":"0",
            "livetime":"14:53:17:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "88:C3:97:75:1B:C0":                 # 客户端 2
        {
            "apidx":"0",
            "livetime":"14:53:14:2",
            "rssi":"-52",
            "ifdev":"wifi@nssid"
        },
        "40:31:3C:4D:78:35":                 # 客户端 3
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

    示例, 从第一个 2.4G SSID 断开客户端 00:03:7F:13:BD:30
    ```shell
    wifi@nssid.stabeat[ 00:03:7F:13:BD:30 ]
    ttrue
    ```

### 生命周期 API
+ `setup[]` / `shut[]` -- 当在 `project/wifi` 中为此对象定义时, 它们启动/停止底层无线服务。参考 **wifi** FPK 不会为这些对象调度 **`init`/`uninit`**; 启动通常由驱动程序、**network** 协议栈或产品集成驱动。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_wifi_nssid(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wifi@nssid", "status") == NULL)
        return -1;
    ok = ssets_string("wifi@nssid", "value", "status");
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

/* Example: scall("wifi@nssid", "status", NULL); then talk_free if JSON */
```
