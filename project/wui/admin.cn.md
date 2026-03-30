## wui@admin — 管理员 WEB 服务器管理
设备管理网页的管理。管理 Web 服务配置为 **`wui@admin`**；以下属性在服务绑定到该对象后适用。

### 配置 ( `wui@admin` )
```json
// 属性介绍
{
    "status":"系统启动时启动",     // [ disable, enable ] — 仅 disable 时在 setup 时跳过启动服务

    "port":"服务端口",                  // [ number ], 1-65535；省略或 0 = 不启用 HTTP 监听
    "sslport":"HTTPS 端口",                 // [ number ], 1-65535；省略或 0 = 不启用 HTTPS 监听
    "termport":"终端端口",             // [ number ], 1-65535, 默认为 81
    "session_timeout":"会话超时时间",    // [ number ] 秒（evhttp 空闲）；默认 300
    "talk_timeout":"通信超时时间",          // [ number ] 秒，用于 /public, /he, /upload, /download；默认 61
    "key_lifetime":"密钥有效期",         // [ number ] 秒，会话密钥的有效期；默认 600
    "auth_object":"认证对象",            // [ string ] — 用于验证登录的对象；如果省略则使用平台默认值
    "auth_api":"认证 API",                  // [ string ], 默认 "match"

    "webpage_path":"文档根目录",         // [ string ], 可选；如果未设置，webpath 使用项目默认的 misc 路径

    "publist":                   // 有效的公共命令列表；如果省略，可能适用内置的默认允许列表
    {
        "command match":"compare type"     // [ string ]: "sub" = 子串匹配, "equal" = 完全匹配, 其他值 = 前缀匹配
    },
    "helist":                   // 有效的 HE 命令列表（与 publist 相同的比较规则）
    {
        "command match":"compare type"
        // ... 更多命令匹配规则
    },

    "manager":                              // 仅允许指定的 IP 地址或 MAC 地址访问
    {
        // "...":"..." 可以配置多个允许访问的主机
        "host name":"IP address or MAC address", // [ string ]: [ IP/MAC address ]
        "host name2":"IP address or MAC address" // [ string ]: [ IP/MAC address ]
    },

    // 自定义网页 HTML
    "css_file":"CSS 文件路径",                   // [ string ], 文件名必须位于 /PRJ/wui/admin/assets/css/ 或 /mnt/config/wui/，通过 <%csspath(); %> 显示
    "logo_file":"LOGO 文件路径",                 // [ string ], 文件名必须位于 /PRJ/wui/admin/assets/css/ 或 /mnt/config/wui/
    "login_file":"文件路径",                     // [ string ], 只读，文件名必须位于 /PRJ/wui/admin/ 或 /mnt/config/wui/
    "index_file":"文件路径",                     // [ string ], 只读，文件名必须位于 /PRJ/wui/admin/ 或 /mnt/config/wui/

    // 自定义网页框架
    "logo_title":"页面中间的文字",    // [ string ]
    "logo_width":"LOGO 宽度",                    // [ string ]
    "logo_height":"LOGO 高度",                  // [ string ]
    "logo_align":"center",                        // [ center, right ]
    "logo_model":"是否显示",                   // [ enable, disable ]
    "nav_bar":"是否显示",                      // [ enable, disable ]

    // 自定义网页显示
    "bigversion":"是否显示",                   // [ enable, disable ]
    "copyright":"是否显示",                    // [ enable, disable ]
    "firmware_id":"是否显示",                  // [ enable, disable ]
    "repo_online":"是否显示",                  // [ disable, enable ]
    "upgrade_online":"是否显示",               // [ disable, enable ]

    // 自定义网页菜单
    "menu":
    {
        "wan":"是否显示",                      // [ enable, disable  ]
        "wan2":"是否显示",                     // [ enable, disable  ]
        "wisp":"是否显示",                     // [ enable, disable  ]
        "wisp2":"是否显示",                    // [ enable, disable  ]
        "lte":"是否显示",                      // [ enable, disable  ]
        "lte2":"是否显示",                     // [ enable, disable  ]
        "lan":"是否显示",                      // [ enable, disable  ]
        "sta":"是否显示",                      // [ enable, disable  ]
        "connection":"是否显示",               // [ enable, disable  ]
        "opmode":"是否显示",                   // [ enable, disable  ]
        "language":"是否显示",                 // [ enable, disable  ]

        "terminal":"是否显示",                 // [ enable, disable  ]
        "development":"是否显示"               // [ enable, disable  ]
    }

}
```


HTTPS 使用以组件命名的证书文件，例如 **`<component>.ca`**、**`<component>.crt`**、**`<component>.key`**，存放在项目配置中，当 **`sslport`** 非零时使用。

示例，显示所有配置
```shell
wui@admin
{
    "status":"enable",             # 系统启动时启动此服务
    "login":"disable",             # 无需登录即可访问网页
    "port":"80",                   # 服务端口 80
    "sslport":"443",               # HTTPS 端口 443
    "manager":                     # 仅允许 192.168.8.111 和 00:03:7F:12:AA:B0 访问
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  
示例，修改网页服务器端口
```shell
wui@admin:port=2222
ttrue
```  
示例，禁用网页服务器
```shell
wui@admin:status=disable
ttrue
```  

示例，一次修改多个属性（**合并**）
```shell
wui@admin|{"status":"enable","port":"80","sslport":"443"}
ttrue
```

### 组件 API
+ `setup[]` **应用已保存的 `wui@admin` 配置并启动或跳过管理 Web 服务**，*成功返回 ttrue*
    - 如果 **`status`** 为 **`disable`**，则不启动 HTTP/HTTPS 服务。
    - 否则启动长期运行的 **`service`**（静态页面以及 `/auth`、`/he`、`/public`、`/upload`、`/download` 等接口）。

    示例，手动运行 setup
    ```shell
    wui@admin.setup
    ttrue
    ```

+ `shut[]` **停止管理 Web 服务**，*成功返回 ttrue*
    - 停止为此对象（与 **`wui@admin`** 同名）注册的服务实例。

    示例，关闭管理 Web 服务
    ```shell
    wui@admin.shut
    ttrue
    ```

### 生命周期 API
+ `setup[]` -- 在默认包中，作为 **`wui@admin.setup`** 在 **`init` -> `app`** 阶段运行。

+ `shut[]` -- 在默认包中，作为 **`wui@admin.shut`** 在 **`uninit` -> `app`** 阶段运行。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_wui_admin(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "wui@admin", "status") == NULL)
        return -1;
    ok = ssets_string("wui@admin", "value", "status");
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

/* 示例: scall("wui@admin", "status", NULL); 如果是 JSON 则调用 talk_free */
```
