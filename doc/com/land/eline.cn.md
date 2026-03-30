## 在终端上使用 eline 与 HE 命令交互

**Eline** 是网关上的交互式命令行前端。它使用 GNU **readline**（行编辑、命令历史），并将大多数输入传递给与经典 shell 相同的 HE 解释器（`line_he_command`）。提示符为 **`$ `** 而非 **`# `**。HE 语法——查询和修改组件配置、调用方法、JSON 规则——与 **[he.cn.md](he.cn.md)** 中相同；本文档重复了要点并描述了 **eline 新增的功能**（内置命令、**`set`** 模式和透传 OS 命令行）。

> **从这里开始**
> - **看到 `$ `** -- 按照 **[he.cn.md](he.cn.md)** 中的方式输入 HE 命令（无需 `he` 前缀）。
> - **需要 BusyBox `ash` 或普通 shell 工具？** 输入 **`ashy`**。之后你将处于 **`~ #`** 提示符下，必须运行 **`he '…'`**；参见 **he.cn.md** -> *`he` 程序如何拼接参数*（为何需要一个引号参数）。
> - **不进入 shell 直接退出 eline：** 在 **`$ `** 下按 **`exit`** 或 Ctrl+D。

---

## 打开 eline 会话的方式

你可以通过与经典 HE 终端相同的访问方式使用 eline：

- **Telnet** -- 在 Web UI 的 **<系统>** 下启用 **Telnet 服务器**，然后使用客户端连接。
- **SSH** -- 在 **<系统>** 下启用 **SSH 服务器**，然后使用客户端连接。
- **TTL / RS232 / RS485 UART** -- 在 **<应用>** 下将 **Serial#TTL** / **Serial#RS232** / **Serial#485**（或 **UART** / **UART2** / **UART3**）设置为 **命令行**，然后使用串口终端（典型设置：57600, 8N1, 无流控）。

认证通过后，看到 **`$ `**（eline）还是 **`# `**（经典 HE 循环）取决于用户的**登录 shell**。如果 shell 设置为 **eline**（例如 **`/usr/bin/eline`**），你会得到 **`$ `** 提示符。你也可以在已有 shell 中运行 **`eline`**（当它在 **`PATH`** 中时）。

如果 **`/etc/banner.he`** 存在，eline 会在第一个提示符之前运行一次 **`cat /etc/banner.he`**（与经典终端相同风格的欢迎文本；之后的提示符为 **`$ `**）。

---

## Eline 与经典 HE 终端的对比

| 方面 | 经典 HE 终端 (`# `) | Eline (`$ `) |
|------|---------------------|--------------|
| 行编辑 | 取决于通道；通常较基础 | Readline：光标键、历史记录、常用编辑键 |
| 提示符 | `# ` | `$ ` |
| HE 命令 | 是 | 是（与 **he.cn.md** 格式相同） |
| 交互式 **`set`** 会话 | 否 | **`set <object>`**（见下文） |
| 选定的 OS 命令 | 因产品而异 | 通过 **`shell()`** 固定的**透传**列表 |

---

## HE 命令格式 -- 快速入门（30 秒）

```shell
$ land@machine
$ land@machine:name
$ land@machine:name=DemoGateway
$ land@machine.status
$ land@machine.status:version
```

| 用法 | 格式 | 简要描述 | 示例 |
|------|------|---------|------|
| 查询组件所有配置 | `component` | 返回组件的完整配置对象。 | `land@machine` |
| 查询一个配置属性 | `component:attr/path` | 返回路径处的字符串或 JSON 值。 | `land@machine:name` |
| 设置组件完整配置 | `component={json}` | 用 JSON 对象替换组件配置。 | `forward@alg={"ftp":"enable"}` |
| 设置一个配置属性 | `component:attr/path=value` | 设置一个属性（字符串或 JSON）。 | `gnss@nmea:client/server=192.168.8.251` |
| 合并多个属性 | `component:attr/path\|{json}` 或 `component\|{json}` | 仅更新列出的字段。 | `gnss@nmea:client\|{"status":"disable","proto":"tcp"}` |
| 清除一个配置属性 | `component:attr/path=` | 清除该属性。 | `gnss@nmea:client=` |
| 无参数调用方法 | `component.method` | 调用无参数的 API 方法。 | `client@station.list` |
| 带参数调用方法 | `component.method[param1,param2,...]` | 调用带参数的方法。 | `clock@date.ntpsync[ntp1.aliyun.com]` |
| 从方法 JSON 取一个字段 | `component.method:attr/path` | 调用方法并返回一个字段。 | `ifname@lte.status:ip` |

#### 如何快速选择格式

| 目标 | 推荐格式 |
|------|---------|
| 读取所有设置 | `component` |
| 读取一个设置 | `component:attr/path` |
| 替换整个配置对象 | `component={json}` |
| 仅更新部分字段 | `component\|{json}` 或 `component:attr/path\|{json}` |
| 设置一个字段 | `component:attr/path=value` |
| 清除一个字段 | `component:attr/path=` |
| 调用 API | `component.method[...]` |
| 调用 API 并只返回一个字段 | `component.method:attr/path` |

#### HE 命令类别和返回类型

+ 每个 HE 行是一条命令，以回车结束。命令分为：**查询配置**、**修改配置**和**调用组件方法**。
+ 典型返回：普通**字符串**、**`ttrue` / `tfalse`**、**JSON 对象** `{...}`，或无数据时为空——与 **he.cn.md** 中的语义相同。输出后，eline 显示下一个 **`$ `** 提示符。

完整语法、嵌套示例和故障排除请使用 **[he.cn.md](he.cn.md)**。

---

## 实际示例 -- 在 `$` 提示符下的 HE 命令

以下示例使用 **`$ `** 作为 eline 提示符。你输入的行前面有 **`$ `**；其他行是输出。

#### 列出所有组件

```shell
$ @
{ ... }                                           # 组件索引（形状取决于固件）
$ 
```

#### 查询一个组件的完整配置

```shell
$ land@machine
{
    "mode":"nmisp",
    "name":"ASHYELF-12AAD0",
    "mac":"00:03:7F:12:AA:D0",
    "macid":"00037F12AAD0",
    "language":"cn",
    "cfgversion":"1"
}
$ 
```

#### 查询一个属性和嵌套路径

```shell
$ land@machine:name
ASHYELF-12AAD0
$ 
$ ifname@lan:static
{
    "ip":"192.168.8.1",
    "mask":"255.255.255.0"
}
$ ifname@lan:static/ip
192.168.8.1
$ 
```

#### 设置一个属性（`ttrue` / `tfalse`）

```shell
$ land@machine:name=DemoGateway
ttrue
$ land@machine:name
DemoGateway
$ 
```

#### 设置嵌套字段

```shell
$ gnss@nmea:client/server=192.168.8.251
ttrue
$ gnss@nmea:client/server
192.168.8.251
$ 
```

#### 用 JSON 替换整个子树

```shell
$ gnss@nmea:client={"status":"enable","server":"192.168.8.250","port":"8000"}
ttrue
$ gnss@nmea:client
{
    "status":"enable",
    "server":"192.168.8.250",
    "port":"8000",
    ...
}
$ 
```

#### 仅合并部分字段（保留其余）

```shell
$ gnss@nmea:client|{"status":"disable","proto":"tcp"}
ttrue
$ 
```

#### 清除一个属性

```shell
$ gnss@nmea:client/server=
ttrue
$ 
```

#### 调用方法，然后从结果中读取一个字段

```shell
$ land@machine.status
{
    "version":"1.2.3",
    ...
}
$ land@machine.status:version
1.2.3
$ 
```

#### 带参数的方法

```shell
$ clock@date.ntpsync[ntp1.aliyun.com]
ttrue
$ 
```

---

## 实际示例 -- 交互式 `set <object>` 模式

在 **`set`** 内，提示符类似 **`land@machine:`**。属性名称**相对于该对象**（与你在 HE 中 **`:`** 后面写的路径相同，例如 **`name`**、**`static/ip`**）。

#### 编辑并保存

```shell
$ set land@machine
{
    "name":"ASHYELF-12AAD0",
    "language":"cn",
    ...
}
land@machine: name
ASHYELF-12AAD0
land@machine: name=DemoGateway
land@machine: g
{
    "name":"DemoGateway",
    "language":"cn",
    ...
}
land@machine: s
ttrue
$ land@machine:name
DemoGateway
$ 
```

#### 在 `set` 中查询嵌套字段

```shell
$ set ifname@lan
{ ... }
ifname@lan: static/ip
192.168.8.1
ifname@lan: e
$ 
```

#### 放弃编辑不保存

```shell
$ set land@machine
{ ... }
land@machine: name=WrongName
land@machine: e
$ land@machine:name
ASHYELF-12AAD0
$ 
```

#### 按 Ctrl+D 离开 `set`（与 `e` 效果相同，放弃会话）

在 **`object:`** 提示符下按 **Ctrl+D** 会释放内存中的配置并返回 **`$ `**，不执行 **`sset`**。

---

## 实际示例 -- 内置命令、透传和会话控制

#### 退出 eline

```shell
$ exit
```

（会话结束。在 **`$ `** 下按 **Ctrl+D** 也会退出。）

#### 完整登录 shell（`ash`）

```shell
$ ashy
```

Eline 被替换为 **`/bin/ash --login`**。离开该 shell 后，远程会话通常会结束，因为原始 eline 进程已消失——请做好相应计划（例如打开第二个会话用于测试）。

#### 网络/系统命令（透传）

这些通过设备 shell 运行，而非 HE：

```shell
$ ping -c 2 192.168.8.1
$ ip link show
$ ifconfig
$ route -n
$ ls /tmp
$ cd /tmp
$ curl -s http://127.0.0.1/
$ 
```

在前缀需要空格的地方使用**空格**（例如 **`ping `**、**`ip `**、**`ls `**）。**`router`** 不会被当作 **`route`** 处理。

#### Readline 历史

```shell
$ land@machine:name
$ land@machine:language
$ 
```

按两次**向上键**可回调 **`land@machine:language`**，编辑后按**回车**。

---

## 内置 eline 命令（不发送到 HE）

由 eline 在 HE 解析之前处理：

| 输入 | 操作 |
|------|------|
| **`exit`** | 退出 eline（精确匹配；例如 `exita` 不会被当作 exit）。 |
| **`ashy`** | 用 **`/bin/ash --login`** 替换当前进程。失败时打印错误并退出进程。 |
| **`set <object>`** | 进入 HE 对象名称的**交互式配置模式**（例如 **`set land@machine`**）。见下一节。 |

**Ctrl+D (EOF)：** 在顶层 **`$ `** 提示符下，eline 退出。在 **`set`** 模式内，EOF 离开 **set** 模式并返回 **`$ `**（内存中的配置被释放）。

---

## 交互式 `set <object>` 模式

1. 输入 **`set <object>`**，使用与 HE 中相同的对象字符串（例如 **`set land@machine`**）。
2. Eline 使用 **`sget`** 加载配置，打印配置，然后显示 **`object:`** 提示符（例如 **`land@machine:`**）。
3. 后续输入行：

| 行格式 | 含义 |
|--------|------|
| **`attr=value`** | 在**内存中**的配置上设置 **`attr`** 为 **`value`**。**`=`** 后为空值则清除该字段。属性路径遵循 HE 风格的命名（包括支持嵌套路径的 **`/`**）。 |
| **`g`** | **获取** -- 再次打印内存中的配置。 |
| **`s`** | **保存** -- 用 **`sset`** 写回。打印 **`ttrue`** 或 **`tfalse`**，然后离开 **set** 模式。 |
| **`e`** | **退出** -- 不执行 **`sset`** 离开 **set** 模式；内存中的对象被释放。 |
| 无 **`=`**，且不是 **`g`** / **`s`** / **`e`** | 将整行视为属性路径，对内存中的配置执行 **`attr_gets`**；如果存在则打印值。 |

输入行存储在 readline **历史**中。在你输入 **`s`** 之前，你编辑的是一个**工作副本**；**`e`** 或 EOF 放弃它而不执行 **`sset`**。

---

## 透传行（系统 shell）

如果一行匹配以下规则之一，eline 使用 **`shell()`** 运行它而非 HE。匹配方式为整行的**字符串前缀**（无额外修剪）。

| 前缀/规则 | 备注 |
|-----------|------|
| `arp ` (4 字符 + 空格) | 例如 `arp -a` |
| `ping ` | |
| `traceroute` | 前 10 个字符：匹配 **`traceroute`**、**`traceroute6`** 等 |
| `ifconfig` | 前 8 个字符 |
| `route` | **`route`** 单独使用或 **`route `**... -- **`router`** 不匹配 |
| `netstat` | 前 7 个字符 |
| `iperf` | 前 5 个字符（`iperf`、`iperf3` 等） |
| `tcpdump` | 前 7 个字符 |
| `mkdir ` | |
| `telnet ` | |
| `wg ` | |
| `ip ` | |
| `tip ` | |
| `cd ` | **单独 `cd`**（无空格）**不**匹配 -> 当作 HE 处理 |
| `ls ` | **单独 `ls`** **不**匹配 -> 当作 HE 处理；使用例如 **`ls /`** |
| `tftp ` | |
| `curl ` | |
| **`reboot`** | 前 6 个字符为 **`reboot`** 的任何行（例如 `reboot`、`reboot -f`） |

所有**其他**行进入 **`line_he_command`**（与不带 shell 包装器的 **`he`** 相同的入口点）。

---

## 技巧

- 使用**上/下键**回调之前的行（readline **历史**）。
- **空行**或首字节不可打印的行会被跳过（不执行 HE 调用，不透传）；它不会进入历史记录。
- 完整的 HE 语法、**`he` argv 拼接**、返回类型以及 **`+` / `=` / `-`** 模式在 **[he.cn.md](he.cn.md)** 中。
- 共享术语：**[TERMINOLOGY.cn.md](TERMINOLOGY.cn.md)**（如果存在于此目录树中）。

---

## 使用组件文档管理任何模块（与 **he.cn.md** 相同的思路）

你通过**组件名称**、**配置路径**和**方法**来管理网关，与每个组件的 Markdown 文档中描述的完全一致。Eline 只是改变了**提示符**（`$ `）并增加了 **`set`** / 透传；从文档到输入行的映射不变。

#### 在哪里找到组件文档

- 当你的产品附带该目录树时，浏览**[在线组件文档](../com/)**（与 **he.cn.md** 中的布局相同）。它随功能增长而更新。
- 在这个 **land** 仓库中，许多主题也以本地文件形式提供——例如 **[syslog.cn.md](syslog.cn.md)** 用于系统日志。
- 或者联系技术支持获取适合你固件的正确文档包。

#### 如何阅读组件文档

- 打开项目，然后打开组件页面。它通常以**功能概述**开始。
- **配置**展示 JSON 结构：字段名称、嵌套和含义。这些名称就是你在 HE 中输入的 **`component:attr/path`** 段（以及在 **`set component`** 后使用的属性名称）。
- **方法**列出可调用的 API。每个都对应终端上的 **`component.method`** 或 **`component.method[args]`**，如文档所述。

一旦你知道了**组件名称**（例如来自**配置**的 **`land@syslog`**），你就可以使用与 **[he.cn.md](he.cn.md)** 相同的规则从 eline 查询、修改和调用方法。

#### 从文档查询配置

- **完整对象** -- 单独输入组件名称：

```shell
$ land@syslog
{
    "status":"enable",
    "location":"",
    "level":"info",
    "trace":"disable",
    "size":"100",
    "remote":"192.168.8.250",
    "port":"514"
}
$ 
```

- **一个字段** -- 使用文档中的 **`component:path`**：

```shell
$ land@syslog:level
info
$ 
```

#### 从文档修改配置

- **单个属性** -- **`component:attr=value`**：

```shell
$ land@syslog:remote=192.168.8.250
ttrue
$ 
```

- **合并多个键** -- **`|`** 后跟 JSON（仅更改列出的键）：

```shell
$ land@syslog|{"remote":"192.168.8.251","port":"500"}
ttrue
$ 
```

- **替换整个配置** -- **`component={...}`**，使用文档中的完整 JSON：

```shell
$ land@syslog={"status":"enable","location":"","level":"info","trace":"disable","size":"100"}
ttrue
$ 
```

#### 从文档调用方法

文档中的方法名称对应 **`component.method`**（参数对应 **`[...]`**，如文档所述）。

```shell
$ land@syslog.show
Dec 15 15:47:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 102 times
Dec 15 15:47:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 103 times
...                                         # 更多设备返回的行
$ land@syslog.clear
ttrue
$ 
```

#### 使用 **`set land@syslog`** 的相同工作流

阅读**配置**后，你可以交互式编辑该对象：属性行只使用**路径部分**（每个键不需要重复 **`land@syslog:`** 前缀）。

```shell
$ set land@syslog
{ ... }                                     # 当前 JSON 打印一次
land@syslog: level
info
land@syslog: remote=192.168.8.250
land@syslog: g
{ ... "remote":"192.168.8.250", ... }
land@syslog: s
ttrue
$ 
```

对**任何**其他组件应用相同的模式：打开其文档 -> 记下**组件名称**和字段路径 -> 根据需要使用 **`$ component`**、**`$ component:path`**、**`$ component:path=value`**、**`$ component.method`** 或 **`$ set component`**。

---

## 另请参阅

- **[he.cn.md](he.cn.md)** -- 完整的 HE 命令参考。
- **[eline.md](eline.md)** -- 本文档的英文版本。
