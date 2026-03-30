
## 在终端上执行 HE 命令管理网关   
HE 命令行用于查询和更改网关配置、调用组件方法以及执行重启或重置等操作。除了终端之外，相同的 HE 语法也可以通过其他控制协议使用。

> **从这里开始**
> - **提示符是 `$ `？** 你在 **[eline](eline.cn.md)** 中 — **直接**输入 HE 命令（不需要 `he`）。使用 **`ashy`** 打开 shell，然后运行 **`he '…'`**。
> - **提示符是 `# `？** 某些镜像使用经典 HE 循环 — 直接输入 HE 命令，或使用 **`elf`**（如果你的欢迎横幅显示它）进入 **`~ #`** shell。
> - **已经在 `~ #`？** 每个 HE 行必须是 **`he '<完整行>'`**（一个带引号的参数）。详情见[进入 Linux shell 并使用 he 前缀运行 HE](#进入-linux-shell-并使用-he-前缀运行-he)。
> - **交互式前端：** **[eline.cn.md](eline.cn.md)** 涵盖 `set`、透传命令、`exit` 和 Ctrl+D。

### 登录后看到的内容（Telnet、SSH 或串口命令行）

许多产品运行 **eline** 作为登录 shell。你会得到带有 GNU readline（历史记录和行编辑）的 **`$ `** 提示符。在那里你**直接**输入 **HE 命令** — **不需要 `he` 前缀** — 并且可以使用 **`set`**、**`ashy`**、**`exit`** 和透传行，如 **[eline.cn.md](eline.cn.md)** 中所述。

要在普通的 **Linux shell**（`ash`）中工作，请在 **`$ `** 提示符下输入 **`ashy`**。Eline 将被 **`/bin/ash --login`** 替换（root 用户的提示符通常是 **`~ #`**）。**在该 shell 内，每个 HE 命令必须通过 `he` 工具运行**，例如 **`he 'land@machine'`** 或 **`he 'land@machine:name=DemoGateway'`**。当 HE 文本包含 shell 会特殊处理的字符时（`|`、`{`、`}`、`[`、`]`、`:`、`=`、`,`、JSON 中的空格等），请用**单引号**包裹。

某些固件仍然使用带有 **`# `** 提示符的经典 **HE 循环**（直接输入 HE，与 eline 语法相同）。两种情况下的欢迎横幅文本相似。

你可以通过以下方式访问网关：   
- **Telnet**   
- **SSH**   
- **TTL UART**（波特率通常为 57600 / 8 数据位 / 1 停止位 / 无流控 / 无校验）   
- **RS232 UART**（相同的典型串口参数）   
- **RS485 UART**（相同的典型串口参数）   

##### Telnet 客户端   
1. 在 Web UI 的 **<系统>** 下打开 **Telnet 服务器**。   
2. 使用你的用户名和密码登录。如果你进入了 **eline**（`$ `），按上述方式使用；当你需要在 shell 中使用 **`he`** 时，使用 **`ashy`**。

##### SSH 客户端   
1. 在 **<系统>** 下打开 **SSH 服务器**。   
2. 登录；然后按照上述 **eline / `ashy` / `he`** 流程操作。

##### TTL / RS232 / RS485   
1. 在 **<应用>** 下，将 **Serial#TTL** / **Serial#RS232** / **Serial#485** 或 **UART** / **UART2** / **UART3** 设置为 **命令行**。   
2. 使用串口终端连接并登录；同样的 **eline / `ashy` / `he`** 规则适用。

登录后，可能会出现一个简短的 ASCII 横幅，例如：   
```
               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
        Ashy   EEEEEEEEE   LLLLLLLLLL   FF

 -----------------------------------------------------------
 Command Help
 -----------------------------------------------------------
 @ ----------------------- List all the component
 <com> ------------------- Show component configure
 <com>. ------------------ List all component interface
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
 -----------------------------------------------------------
```   
活动提示符是 **`$ `**（eline）、**`# `**（经典 HE 循环）或 **`~ #`**（**`ashy`** 后的 Linux shell），取决于镜像和步骤 — 使用你的会话实际显示的提示符。

---

## HE 命令格式

#### 快速入门（30 秒）

下面的行是你在 **Linux shell** 中输入的内容（**`ashy`** 后的 `~ #`）。在 **eline**（`$ `）中，只输入相同的**负载** — 省略 **`he`** 和周围的引号 — 参见 **[eline.cn.md](eline.cn.md)**。

```shell
~ # he 'land@machine'                         # 1) 查询完整组件配置
~ # he 'land@machine:name'                    # 2) 查询单个属性
~ # he 'land@machine:name=DemoGateway'      # 3) 修改单个属性
~ # he 'land@machine.status'                  # 4) 调用方法
~ # he 'land@machine.status:version'          # 5) 从方法 JSON 中获取单个字段
```

#### `he` 程序如何连接参数

实现通过**连接 `he` 之后的每个参数（不插入空格）**来构建一个字符串。示例：

- `he 'land@machine:name'` → 一个参数 → 正确。
- `he land@machine:name` → 一个令牌（无空格） → 正确。
- `he land@machine name` → **错误**：变成 `land@machinename`。

所以在 shell 中，当有空格时，或者当你不确定时，**始终**将完整的 HE 行作为**一个**带引号的字符串传递。这也可以阻止 shell 解释 `|`、`=`、`;` 等。

如果该组合字符串的**第一个字符**是 **`+`**、**`=`** 或 **`-`**，它会选择特殊模式（循环/仅解析/静默），而不是正常的组件命令 — 参见[进入 Linux shell 并运行 HE](#进入-linux-shell-并使用-he-前缀运行-he) 下的**高级 `he` 前缀**。

| 用法 | 格式 | 简要描述 | Linux shell 中的示例（`he '…'`） |
|---|---|---|---|
| 查询所有组件配置 | `component` | 返回组件的完整配置对象。 | `he 'land@machine'` |
| 查询单个配置属性 | `component:attr/path` | 返回指定属性路径处的字符串或 JSON 值。 | `he 'land@machine:name'` |
| 设置完整组件配置 | `component={json}` | 用提供的 JSON 对象替换组件配置。 | `he 'forward@alg={"ftp":"enable"}'` |
| 设置单个配置属性 | `component:attr/path=value` | 设置单个属性值。支持字符串或 JSON 值。 | `he 'gnss@nmea:client/server=192.168.8.251'` |
| 合并多个属性 | `component:attr/path\|{json}` 或 `component\|{json}` | 仅更新指定字段，其他字段保持不变。 | `he 'gnss@nmea:client|{"status":"disable","proto":"tcp"}'` |
| 清除单个配置属性 | `component:attr/path=` | 清除/删除指定的属性值。 | `he 'gnss@nmea:client='` |
| 调用不带参数的方法 | `component.method` | 调用不带参数的组件 API 方法。 | `he 'client@station.list'` |
| 调用带参数的方法 | `component.method[param1,param2,...]` | 调用带有一个或多个参数的组件 API 方法。 | `he 'clock@date.ntpsync[ntp1.aliyun.com]'` |
| 从方法 JSON 返回单个字段 | `component.method:attr/path` | 调用方法并仅从 JSON 结果返回一个字段。 | `he 'ifname@lte.status:ip'` |

#### 如何快速选择格式

| 目标 | 推荐格式 |
|---|---|
| 读取所有设置 | `component` |
| 读取单个设置 | `component:attr/path` |
| 替换完整配置对象 | `component={json}` |
| 仅更新多个字段 | `component\|{json}` 或 `component:attr/path\|{json}` |
| 设置单个字段 | `component:attr/path=value` |
| 清除单个字段 | `component:attr/path=` |
| 调用 API | `component.method[...]` |
| 调用 API 并仅返回单个字段 | `component.method:attr/path` |

#### **HE 命令介绍**

+ 每个命令占一行，以回车结束。HE 命令可分为三种类型：   
	+ **查询配置**              用于查询网关某些功能组件的当前配置
	+ **修改配置**             用于修改网关某些组件的配置
	+ **调用组件方法**         用于执行网关组件的某些功能操作，如查询状态、启动和禁用
+ 命令的返回类型   
	+ **字符串**         用于表示单条信息。（调用的方法返回 `talk_t` 类型的字符串。）*输出后你会得到下一个提示符：Linux shell 中是 **`~ #`**，eline 中是 **`$ `**，经典 HE 循环中是 **`# `**。*
	+ **ttrue** 或 **tfalse**      `ttrue` 表示成功，`tfalse` 表示失败。*与上述相同的下一提示符规则。*
	+ **{JSON}**                   以 `{` 开始、以 `}` 结束的 JSON 对象（调用的方法返回 `talk_t` 类型的 JSON 结构），用于表示一些复杂信息。*与上述相同的下一提示符规则。*
	+ **空**                       用于表示没有可用信息（如果调用的方法返回 `null` 则为 `NULL`）。*与上述相同的下一提示符规则。*

#### **查询配置**

+ 查询网关组件的所有配置时，输入**组件名**按回车
通常返回值会是一个 JSON
    ```shell
    ~ # he 'land@machine'                    # 查询 land@machine（网关基本信息）的配置
    {                                 # 返回一个完整的 JSON 对象
        "mode":"nmisp",                              # 网关运营商模式 Next Mobile(LTE/NR) 互联网服务提供商连接
        "name":"ASHYELF-12AAD0",                     # 网关名称为 ASHYELF-12AAD0
        "mac":"00:03:7F:12:AA:D0",                   # 网关 MAC 为 88:12:4E:20:46:B0
        "macid":"00037F12AAD0",                      # 网关序列号 ID 为 88124E2046B0
        "language":"cn",                             # 网关语言为中文
        "cfgversion":"1"                             # 网关配置版本为 1（提示修改过一次）
    }
    ~ #
    ```   

+ 查询组件指定属性时，在**组件名:属性路径**后按回车
返回值可以是字符串或 JSON，取决于相应属性的值
    ```shell
    ~ # he 'land@machine:name'                    # 查询 land@machine 组件配置下 name 属性的值
    A218-120108                      # 返回字符串
    ~ #
    ```   
+ 当组件有多层属性时，**属性路径**将用 **/** 分隔多层**属性名**。属性路径用于定位属性，如下所示
    ```shell
    ~ # he 'ifname@lan:static'                    # 查询 ifname@lan（LAN 网络管理）组件配置下 static 属性的值
    {
        "ip":"192.168.8.1",
        "mask":"255.255.255.0"
    }
    ~ # he 'ifname@lan:static/ip'                    # 查询 ifname@lan（LAN 网络管理）组件配置下 static 属性下的 ip 属性的值
    192.168.8.1
    ~ #
    ```   

    上面是通过**属性/下级属性/下级属性/...**这种类似路径的方式来定位多层属性

#### **修改配置**

+ 修改组件配置类似于查询组件配置，首先给出**组件名**，然后给出**属性路径**以修改属性，最后给出**值**
    修改整个组件的值时：
    ```shell
    组件名=值
    ```   
    修改整个组件的值时，通常值必须是 JSON（以 `{` 开始，以 `}` 结束）。
    修改成功返回 **ttrue**，修改失败返回 **tfalse**。见以下示例
    ```shell
    ~ # he 'forward@alg'                    # 查询 ALG 配置
    {
        "amanda":"disable",
        "ftp":"enable",
        "h323":"disable",
        "irc":"disable",
        "pptp":"enable",
        "gre":"enable",
        "rtsp":"enable",
        "sip":"enable",
        "snmp":"disable",
        "tftp":"disable",
        "udplite":"disable"
    }
    ~ #
    ~ # he 'forward@alg={"amanda":"enable","ftp":"enable","h323":"enable","irc":"enable","pptp":"enable","gre":"enable","rtsp":"enable","sip":"enable","snmp":"enable","tftp":"enable","udplite":"enable"}'                    # 修改整个 ALG 配置 
    ttrue                          # 返回 ttrue，修改成功
    ~ # he 'forward@alg'                    # 配置修改后，再次检查配置
    {
        "amanda":"enable",
        "ftp":"enable",
        "h323":"enable",
        "irc":"enable",
        "pptp":"enable",
        "gre":"enable",
        "rtsp":"enable",
        "sip":"enable",
        "snmp":"enable",
        "tftp":"enable",
        "udplite":"enable"
    }
    ~ #
    ```   

+ 修改组件指定属性时：
    ```shell
    组件名:属性路径=值
    ```   
    值可以是 JSON（以 `{` 开始，以 `}` 结束）或字符串
    修改成功返回 **ttrue**，修改失败返回 **tfalse**。见以下示例
    ```shell
    ~ # he 'gnss@nmea'                    # 查询 GNSS 配置
    {
        "status":"enable",
        "client":
        {
            "status":"disable",
            "proto":"tcp",
            "server":"",
            "port":"9000",
            "id":"",
            "user":"",
            "vcode":"",
            "login":"",
            "login_string":"",
            "frame_start":"disable",
            "frame_string_string":"",
            "frame_end":"disable",
            "frame_end_string":""
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ # he 'gnss@nmea:client'                    # 查询 GNSS 配置中的 client 属性
    {
        "status":"disable",
        "proto":"tcp",
        "server":"",
        "port":"9000",
        "id":"",
        "user":"",
        "vcode":"",
        "login":"",
        "login_string":"",
        "frame_start":"disable",
        "frame_string_string":"",
        "frame_end":"disable",
        "frame_end_string":""
    }
	~ # he 'gnss@nmea:client={"status":"enable","server":"192.168.8.250","port":"8000","interval":"30","id":"2232"}'                    # 修改 gnss@nmea（GNSS 配置）中的 client 属性
	ttrue
	~ # he 'gnss@nmea:client'                    # 查询 gnss@nmea（GNSS 配置）中修改 client 属性的结果
    {
        "status":"enable",
        "server":"192.168.8.250",
        "port":"8000",
        "interval":"30",
        "id":"2232"
    }
	~ # he 'gnss@nmea:client/server=192.168.8.251'                    # 更改 gnss@nmea（GPS 管理）client 属性下 server 属性的值
	ttrue
	~ # he 'gnss@nmea:client'                    # 配置修改后，再次检查配置
	{
	    "status":"enable",
	    "server":"192.168.8.251",
	    "port":"8000",
	    "interval":"30",
	    "id":"2232"
	}
	~ # he 'gnss@nmea'                    # 查询所有 GNSS 配置
    {
        "status":"enable",
        "client":
        {
            "status":"enable",
            "server":"192.168.8.251",
            "port":"8000",
            "interval":"30",
            "id":"2232"
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ #
    ```   

+ 修改多个指定属性而不影响其他属性时：
    ```shell
    组件名|{"属性1":"值1", "属性2":"值2" , "属性3":"值3"}
    ```   
    值1、值2、值3 通常是字符串
    修改成功返回 **ttrue**，修改失败返回 **tfalse**。见以下示例
    ```shell
	~ # he 'gnss@nmea:client'                    # 查询 GNSS 配置中的 client 属性
	{
	    "status":"enable",
	    "server":"192.168.8.251",
	    "port":"8000",
	    "interval":"30",
	    "id":"2232"
	}
	~ # he 'gnss@nmea:client|{"status":"disable","server":"192.168.2.11","proto":"tcp"}'                    # 修改 gnss@nmea（GNSS 配置）client 属性的多个值
	ttrue
	~ # he 'gnss@nmea:client'                    # 配置修改后，再次检查配置
	{
        "status":"disable",
        "server":"192.168.2.11",
        "port":"8000",
        "interval":"30",
        "id":"2232",
        "proto":"tcp"
	}
	~ #
    ```   


+ 清除组件配置的某个属性，在 = 后按回车：
    ```shell
    组件名:属性路径=
    ```   
    修改成功返回 **ttrue**，修改失败返回 **tfalse**。见以下示例
    ```shell
    ~ # he 'gnss@nmea'                    # 查询所有 GNSS 配置
    {
        "status":"enable",
        "client":
        {
            "status":"disable",
            "server":"192.168.2.11",
            "port":"8000",
            "interval":"30",
            "id":"2232",
            "proto":"tcp"
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ # he 'gnss@nmea:client='                    # 清除 gnss@nmea（GNSS 配置）的 client 属性
	ttrue
	~ # he 'gnss@nmea'                    # 配置修改后，再次检查配置
    {
        "status":"enable",
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ #
    ```   


#### **调用组件方法**

调用组件方法，需要给出组件名和方法名，如果有参数也可以给出
+ 调用不带参数的组件方法时    
    ```shell
    组件名.组件方法
    ```   
    ```shell
    ~ # he 'client@station.list'                    # 调用 client@station（客户端访问）的 list 方法获取当前客户端列表
    {
        "B4:2E:99:3F:ED:12":
        {
            "ip":"192.168.8.250",
            "device":"lan"
        }
    }
    ~ #
    ```   


+ 调用带参数的组件方法时
    ```shell
    组件名.组件方法[ 参数 ]
    ```   
    ```shell
    ~ # he 'clock@date.ntpsync[ntp1.aliyun.com]'                    # 调用 clock@date（系统日期）的 ntpsync 方法，与 ntp1.aliyun.com 进行 NTP 时间同步
    ttrue
    ~ #
    ```   

+ 调用带多个参数的组件方法时
    ```shell
    组件名.组件方法[ 参数1, 参数2, 参数3, ... ]
    ```   
    ```shell
    ~ # he 'land@auth.add[,xiaomi,4431232]'                    # 调用 land@auth 的 add 方法添加账户，第一个参数为空（无），第二个参数为 xiaomi，第三个参数为 4431232
    ttrue
    ~ # he 'land@auth.list'                    # 调用 land@auth 的 list 方法获取所有当前账户
    {
        "admin":
        {
            "key":"|*|V223KJDFLKJLKJA23232323543453453452323KJKHUI2KJKHKJKJKH22232321334"
        },
        "xiaomi":
        {
            "key":"|*|V24E4555304E4455784E304530524455304E446B33515452454E6A637A52444E45"
        }
    }
    ~ #
    ```   


+ 当调用组件方法返回 JSON 时，你可以要求仅返回 JSON 中指定的属性值
    ```shell
    组件名.组件方法:属性路径
    ```   
    ```shell
    ~ # he 'ifname@lte.status'                    # 调用 ifname@lte 的 status 方法查询第一个 LTE 连接的状态
    {
        "mode":"dhcpc",
        "ifname":"ifname@lte",
        "netdev":"usb1",
        "gw":"10.33.13.104",
        "dns":"120.80.80.80",
        "dns2":"221.5.88.88",
        "ifdev":"modem@lte2",
        "ontime":"754",
        "metric":"10",
        "status":"up",
        "ip":"10.33.13.103",
        "mask":"255.255.255.240",
        "livetime":"05:03:36:0",
        "rx_bytes":"2681665",
        "rx_packets":"27616",
        "tx_bytes":"1433806",
        "tx_packets":"21926",
        "mac":"02:50:F4:00:00:00",
        "tid":"2",
        "devcom":"modem@device",
        "name":"Quectel-EC2X",
        "imei":"864337056708978",
        "mversion":"20CEFHLGR06A05M1G",
        "iccid":"89860121801097564807",
        "imsi":"460015356123463",
        "pcid":"184",
        "lac":"2604",
        "ci":"6DA5A09",
        "netera":"4G",
        "rsrp":"-96",
        "nettype":"FDD LTE",
        "rssi":"-66",
        "rsrq":"-9",
        "sinr":"-19",
        "band":"LTE BAND 3",
        "plmn":"46001",
        "csq":"25",
        "signal":"4",
        "operator":"ChinaUnion",
        "operator_advise":
        {
            "name":"ChinaUnion",
            "dial":"*99#",
            "apn":"3gnet"
        },
        "state":"connected"
    }
    ~ # he 'ifname@lte.status:ip'                    # 调用 ifname@lte 的 status 方法，仅查询第一个 LTE 连接状态中的 ip 地址
    10.33.13.103
    ~ # he 'ifname@lte.status:operator_advise'                    # 调用 ifname@lte 的 status 方法，仅查询第一个 LTE 连接状态中的 operator_advise
    {
        "name":"ChinaUnion",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ~ # he 'ifname@lte.status:operator_advise/apn'                    # 调用 ifname@lte 的 status 方法，仅查询第一个 LTE 连接状态中的 APN
    3gnet
    ~ #
    ```   


---
##  参考组件文档使用 HE 命令管理网关

有两种方式访问组件文档。每个组件都可以通过其文档进行管理。
- 访问[在线组件文档](../com/)查看组件文档。随着新功能的开发，此在线文件会不断添加/更新。
- 联系技术支持

#### 组件文档要点   
- 在[在线组件文档](../com/)中，系统中的项目以目录形式列出，每个项目包含组件文档
- 点击项目进入后会列出该项目下的所有组件文档
- 点击组件打开组件文档，开头是功能描述
- 然后是**配置**。配置为 JSON 格式。文档还包括查询/修改示例，可以通过 HE 命令查询或修改配置。
- 通常后面是该组件的**方法**介绍。也提供了调用示例，可以在 HE 命令中执行。
- **以下示例**使用 **`~ # he '…'`**（**`ashy`** 后的 Linux shell），因此可以逐字复制。在 **eline**（`$ `）中，只需输入引号内的字符串 — 不需要 **`he`** 包装器。

#### 参考文档查询组件配置   
组件名在**配置**中给出，例如 **Syslog** — [在线路径](../com/land/syslog.cn.md)或此仓库中的 **[syslog.cn.md](syslog.cn.md)** — 组件名 **land@syslog**

- 输入**组件名**返回该组件的所有配置。每个配置的属性和示例在组件文档的**配置**中描述。
    ```shell
    ~ # he 'land@syslog'                    # 输入组件名
    {                               # 返回所有组件配置的 JSON
        "status":"enable",                # 启用 syslog 功能
        "location":"",                     # 省略/空使用默认文件路径策略（该文件在系统 var 目录下使用随机前缀）
        "level":"info",                   # 日志级别为普通信息
        "trace":"disable",                # 禁用代码信息
        "size":"100",                     # 日志缓冲区为 100k
        "remote":"192.168.8.250",         # 将 syslog 发送到远程服务器 192.168.8.250
        "port":"514"                      # 将 syslog 发送到远程服务器端口 514
    }
    ~ #
    ```

- 通过在**组件名**后提供带有**属性路径**的属性来查询特定属性。
    ```shell
    ~ # he 'land@syslog:level'                    # 查询 level 属性的值
    info                                    # level 的值为 info
    ~ #
    ```

#### 参考组件文档修改组件配置   
参考 **Syslog** 文档（[../com/land/syslog.cn.md](../com/land/syslog.cn.md) 或 [syslog.cn.md](syslog.cn.md)）。**配置**中描述的属性可以在终端通过 `组件名:属性路径=值` 修改。
- 在终端修改 land@syslog 远程日志服务器的 remote 属性
    ```shell
    ~ # he 'land@syslog:remote=192.168.8.250'                    # 将 remote 的值改为 192.168.8.250
    ttrue                                    # 返回 ttrue 表示成功
    ~ #
    ```

- 通过将目标字段封装在 JSON 对象中同时修改多个属性（其余保持不变）。
    ```shell
    ~ # he 'land@syslog|{"remote":"192.168.8.251","port":"500"}'                    # 将 remote 的值改为 192.168.8.251，将 port 的值改为 500
    ttrue                                                    # 返回 ttrue 表示成功
    ~ #
    ```

- 在终端设置完整的 syslog 配置。要修改所有配置，提供相同的 JSON 对象。
    ```shell
    ~ # he 'land@syslog={"status":"enable","location":"","debug":"arch@usb","level":"info","trace":"disable","size":"100"}'
    ttrue                                    # 返回 ttrue 表示成功
    ~ #
    ```

#### 参考组件文档调用组件方法   
参考相同的 **Syslog** 文档。那里描述的方法可以在终端通过 `组件名.组件方法` 调用。
- 调用组件 land@syslog 的 show 方法显示当前日志
    ```shell
    ~ # he 'land@syslog.show'
    Dec 15 15:47:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 102 times
    Dec 15 15:47:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 103 times
    Dec 15 15:47:30 V520-12CC70 user.warn syslog: modem@lte check simcard failed 104 times
    Dec 15 15:47:35 V520-12CC70 user.warn syslog: modem@lte check simcard failed 105 times
    Dec 15 15:47:40 V520-12CC70 user.warn syslog: modem@lte check simcard failed 106 times
    Dec 15 15:47:45 V520-12CC70 user.warn syslog: modem@lte check simcard failed 107 times
    Dec 15 15:47:50 V520-12CC70 user.warn syslog: modem@lte check simcard failed 108 times
    Dec 15 15:47:55 V520-12CC70 user.warn syslog: modem@lte check simcard failed 109 times
    Dec 15 15:48:00 V520-12CC70 user.warn syslog: modem@lte check simcard failed 110 times
    Dec 15 15:48:05 V520-12CC70 user.warn syslog: modem@lte check simcard failed 111 times
    Dec 15 15:48:10 V520-12CC70 user.warn syslog: modem@lte check simcard failed 112 times
    Dec 15 15:48:15 V520-12CC70 user.warn syslog: modem@lte check simcard failed 113 times
    Dec 15 15:48:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 114 times
    Dec 15 15:48:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 115 times
    Dec 15 15:48:30 V520-12CC70 user.warn syslog: modem@lte check simcard failed 116 times
    Dec 15 15:48:35 V520-12CC70 user.warn syslog: modem@lte check simcard failed 117 times
    ~ #
    ```
- 调用组件 land@syslog 的 clear 方法清除所有当前日志
    ```shell
    ~ # he 'land@syslog.clear'
    ttrue
    ~ #
    ```

---

## 进入 Linux shell 并使用 `he` 前缀运行 HE

### 典型路径：**eline** → **`ashy`** → **`he`**

1. Telnet / SSH / 串口登录后，你通常会看到带有 **`$ `** 的 **eline**。输入 HE 时**不需要** `he` 包装器；使用 **`exit`** 或 Ctrl+D 离开 eline。详情：**[eline.cn.md](eline.cn.md)**（`set`、透传 OS 命令、readline 历史记录）。  
2. 输入 **`ashy`**。此进程将被 **`/bin/ash --login`**（BusyBox `ash`；root 的提示符通常是 **`~ #`**）替换。  
3. 从 **`~ #`**，运行 **`he '<一个完整的 HE 行>'`**。参见本文档前面的**`he` 程序如何连接参数** — **一个 shell 参数**是最安全的。  
4. 从 `ash` **`exit`** 通常会**结束整个登录会话**，因为 eline 已被替换。在进行危险测试之前打开一个备用的 SSH/Telnet 会话。

### 备选路径：经典 **`# `** 循环 → **`elf`**

某些镜像首先显示 **`# `**。如果横幅文档说明了它，**`elf`**（或你固件使用的关键字）会产生相同的 BusyBox shell：

```shell
# elf

BusyBox v1.22.1 (2021-04-20 10:35:42 CST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

~ #
```

### 从 shell 运行 HE

```shell
he '<HE 命令行>'
```

当行包含 `|`、`{`、`}`、`[`、`]`、`:`、`=`、`,` 或 JSON 中的空格时，使用**单引号**。

### 高级 `he` 前缀（组合字符串的第一个字符）

在所有参数连接后：

| 前导字节 | 行为 |
|---|---|
| **`+`** | 循环执行（`loop_he`） |
| **`=`** | 仅解析并打印结构（`print_he`） |
| **`-`** | 静默模式（源码中的 `slient_he` — 最小输出） |

示例（整个负载仍必须遵循"一个逻辑行"规则）：

```shell
he '+land@machine'
he '=land@machine:name'
he '-land@machine.status'
```

### 故障排除

| 症状 | 典型原因 | 快速检查 |
|---|---|---|
| `tfalse` | 无效的路径、值或验证 | `he 'component:attr/path'` |
| 空返回 | 字段缺失或 API `NULL` | 用 `he '…'` 查询父路径 |
| 错误/合并的文本 | 多个 `argv` 无空格粘在一起 | 使用 **`he '单引号行'`** |
| JSON 错误 | HE 行中的 JSON 错误 | 修复 JSON；保持 shell 引号 |

### 示例会话（shell）

```shell
# elf

BusyBox v1.22.1 (2021-04-20 10:35:42 CST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

~ # he 'land@machine'
{
    "platform":"smtk",
    "hardware":"mt7621",
    "custom":"v520",
    "scope":"donkey",
    "language":"cn",
    "macid":"88124E200030",
    "mac":"88:12:4E:20:00:30",
    "name":"V520-200030",
    "title":"V520-200030",
    "mode":"donkeycon"
}
~ # he 'land@machine.status'
{
    "custom":"v520",
    "version":"4.3.3w-042021",
    "publish":"042021",
    "macid":"88124E200030",
    "mac":"88:12:4E:20:00:30",
    "magic":"D7441F2CE4605020",
    "model":"V520",
    "name":"V520-200030",
    "mode":"donkeycon",
    "livetime":"00:18:28:0",
    "current":"12:11:16:04:20:2021"
}
~ #
```

---

## 另请参阅

- **[eline.cn.md](eline.cn.md)** — `$ ` 提示符、`set`、`ashy`、透传列表和 Ctrl+D 行为。  
- **[TERMINOLOGY.cn.md](TERMINOLOGY.cn.md)** — 共享术语。  
- **[he.md](he.md)** — 本文档的英文版本。
