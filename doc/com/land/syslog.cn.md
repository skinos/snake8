## land@syslog — 系统日志

管理系统日志服务。对象名称：`land@syslog`。

### 配置 ( `land@syslog` )

`land@syslog` 的**持久化配置对象**（通过 `land@syslog`、`land@syslog:path` 查询/设置，合并 `|{json}` 等）。

```json
// 属性介绍
{
    "status":"syslog status",          // [ "disable", "enable", "tui", "file", "both" ]
                                            // "enable" 启用系统日志
                                            // "disable" 不记录日志
                                            // "tui" 输出到终端
                                            // "both" 同时输出到终端和系统日志
                                            // "file" 输出到文件
    "trace":"record code information", // [ "disable", "enable" ]

    "level":"default log level",            // [ "verb", "debug", "info", "warn", "fault" ]
                                            // "verb" 调试业务数据
                                            // "debug" 调试代码
                                            // "info" 一般信息
                                            // "warn" 警告信息
                                            // "fault" 错误信息
    "fault":"debug type",              // [ string ]，多个类型用 ";" 分隔（例如："arch@usb;arch@pci"）；细化 fault 日志掩码
    "warn":"warn type",                // [ string ]，多个类型用 ";" 分隔（例如："arch@usb;arch@pci"）；细化 warn 日志掩码
    "info":"info type",                // [ string ]，多个类型用 ";" 分隔（例如："arch@usb;arch@pci"）；细化 info 日志掩码
    "debug":"debug type",              // [ string ]，多个类型用 ";" 分隔（例如："arch@usb;arch@pci"）；细化 debug 日志掩码
    "verb":"verb type",                // [ string ]，多个类型用 ";" 分隔（例如："arch@usb;arch@pci"）；细化 verbose 日志掩码

    "klog":"kernel log status",         // [ "disable", "enable" ]
    "size":"log size",                  // [ string ]，十进制数，单位为 KB。日志超过记录池大小时将被覆盖
    "location":"location of logs",      // 可选；选择文件日志的基础路径
                                            // 未设置或其他值：使用系统 var 目录下带随机前缀的默认文件
                                            // "storage"：来自 machine sginfo 的第一个非配置/内部存储路径
                                            // "internal" 或以 "sd"、"mm" 开头的名称：来自 machine sginfo 对应键的路径
                                            // 以 "/" 开头的绝对路径：直接作为日志路径使用
    "remote":"Remote Log server",     // [ string ]，留空则禁用远程日志服务器
    "port":"Remote Log port"          // [ string ]，十进制数，默认为 514
}
```

示例，显示所有系统日志配置
```shell
land@syslog
{
    "status":"enable",                # 启用系统日志功能
    "location":"",                    # 省略或不识别的值使用默认 var 路径（该文件在系统 var 目录下使用随机前缀）；或使用 storage/internal/sd*/mm*/绝对路径
    "debug":"arch@usb;arch@pci",      # 记录 arch@usb 和 arch@pci 的调试信息
    "level":"info",                   # 默认日志级别为一般信息
    "trace":"disable",                # 禁用代码信息记录
    "size":"100",                     # 日志缓冲区为 100k
    "remote":"192.168.8.250",         # 将系统日志发送到远程服务器 192.168.8.250
    "port":"514"                      # 发送系统日志到远程服务器端口 514
}
```  

示例，设置远程系统日志服务器为 112.43.230.74
```shell
land@syslog:remote=112.43.230.74
```  

示例，清除远程系统日志服务器
```shell
land@syslog:remote=
```  

示例，设置系统日志级别为 debug
```shell
land@syslog:level=debug
```  

示例，一次合并多个字段
```shell
land@syslog|{"level":"info","remote":"192.168.8.1","port":"514"}
ttrue
```

### 组件 API

+ `path[]` **显示日志位置和最大大小**，*成功返回 talk，失败返回 NULL，错误返回 terror*   
    ```json
    // 方法返回的 talk 属性介绍
    {
        "path":"log file pathname",   // [ string ]
        "size":"log file size limit"  // [ number ]，单位为 KB
    }    
    ```

    ```shell
    # 示例，显示当前日志文件路径名和大小限制
    land@syslog.path
    {
        "path":"/var/3618760113-landlog.txt",  # 当前系统日志保存在 /var/3618760113-landlog.txt
        "size":"100"                            # 文件大小限制为 100k
    }
    ```

+ `clear[]` **清除日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    ```shell
    # 示例
    land@syslog.clear
    ttrue
    ```

+ `show[]` **显示日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    ```shell
    # 示例
    land@syslog.show
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>66>> (AT+QNWINFO^M^M +QNWINFO: "FDD LTE","46001","LTE BAND 3",1650^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (at+qeng="servingcell"^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>22>> (at+qeng="servingcell"^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CREG?^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>9>> (AT+CREG?^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>48>> (AT+CREG?^M^M +CREG: 2,1,"2604","6DA5A09",7^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CSQ^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>28>> (AT+CSQ^M^M +CSQ: 18,99^M ^M OK^M )    
    ttrue
    ```

+ `info[ message ]` **记录 info 级别的日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    - message ----------- [ string ]，要记录的 info 消息
    ```shell
    # 示例
    land@syslog.info[ "this is log for info level" ]
    ttrue
    ```

+ `debug[ message ]` **记录 debug 级别的日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    - message ----------- [ string ]，要记录的 debug 消息
    ```shell
    # 示例
    land@syslog.debug[ "this is log for debug level" ]
    ttrue
    ```

+ `warn[ message ]` **记录 warn 级别的日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    - message ----------- [ string ]，要记录的 warn 消息
    ```shell
    # 示例
    land@syslog.warn[ "this is log for warn level" ]
    ttrue
    ```

+ `fault[ message ]` **记录 fault 级别的日志**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*   
    - message ----------- [ string ]，要记录的 fault 消息
    ```shell
    # 示例
    land@syslog.fault[ "this is log for fault level" ]
    ttrue
    ```

+ `mask[]` **以十六进制字节转储当前日志掩码**
    ```shell
    # 示例，转储当前日志掩码
    land@syslog.mask
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    ```

+ `add_fault[ component, ... ]` **将组件添加到 fault 级别日志掩码**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - component ----------- [ string ]，要添加到 fault 日志掩码的组件名称（例如 "arch@usb"、"modem@lte"）
    - 可以将多个组件指定为单独的参数
    - 允许对哪些组件可以记录 fault 级别日志进行细粒度控制
    ```shell
    # 示例
    land@syslog.add_fault[ modem@lte, modem@lte2 ]
    ttrue
    ```

+ `add_warn[ component, ... ]` **将组件添加到 warn 级别日志掩码**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - component ----------- [ string ]，要添加到 warn 日志掩码的组件名称

+ `add_info[ component, ... ]` **将组件添加到 info 级别日志掩码**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - component ----------- [ string ]，要添加到 info 日志掩码的组件名称

+ `add_debug[ component, ... ]` **将组件添加到 debug 级别日志掩码**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - component ----------- [ string ]，要添加到 debug 日志掩码的组件名称

+ `add_verb[ component, ... ]` **将组件添加到 verbose 级别日志掩码**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - component ----------- [ string ]，要添加到 verbose 日志掩码的组件名称

+ `del_fault[ component, ... ]` **从 fault 级别日志掩码中移除组件**，*始终返回 ttrue*
    - component ----------- [ string ]，要从 fault 日志掩码中移除的组件名称

+ `del_warn[ component, ... ]` **从 warn 级别日志掩码中移除组件**，*始终返回 ttrue*
    - component ----------- [ string ]，要从 warn 日志掩码中移除的组件名称

+ `del_info[ component, ... ]` **从 info 级别日志掩码中移除组件**，*始终返回 ttrue*
    - component ----------- [ string ]，要从 info 日志掩码中移除的组件名称

+ `del_debug[ component, ... ]` **从 debug 级别日志掩码中移除组件**，*始终返回 ttrue*
    - component ----------- [ string ]，要从 debug 日志掩码中移除的组件名称

+ `del_verb[ component, ... ]` **从 verbose 级别日志掩码中移除组件**，*始终返回 ttrue*
    - component ----------- [ string ]，要从 verbose 日志掩码中移除的组件名称

+ `list[]` **列出所有日志文件**，*成功返回 talk，失败返回 NULL*
    - 返回将日志文件名映射到完整路径的 JSON
    ```json
    // 方法返回的 JSON 属性介绍
    {
        "log file name":"log file full path"
        // "...":"..."  有多少个日志文件就显示多少条属性
    }
    ```
    ```shell
    # 示例
    land@syslog.list
    {
        "3618760113-landlog.txt":"/var/3618760113-landlog.txt"
    }
    ```

+ `delete[ log file name ]` **删除指定的日志文件**，*成功返回 ttrue，失败返回 tfalse*
    - log file name ----------- [ string ]，要删除的日志文件名
    ```shell
    # 示例
    land@syslog.delete[ 3618760113-landlog.txt ]
    ttrue
    ```


### 生命周期 API


+ `setup[]` **初始化系统日志组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统启动期间自动调用的生命周期方法
    - 它根据配置（status、level、远程服务器等）初始化日志系统
    - 不建议手动调用

+ `shut[]` **关闭系统日志组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统关闭期间自动调用的生命周期方法
    - 它停止 klogd 和 syslogd 进程
    - 不建议手动调用


### Joint 处理程序

**Joint** 处理程序可能在存储变更时调用 **`land@syslog.setup`**，以便日志位置逻辑可以重新运行。

| Joint 键 | 方法 |
|----------|------|
| `storage/insert` | `land@syslog.setup` |
| `storage/remove` | `land@syslog.setup` |


### C 代码示例

**读取和更新配置**

```c
#include "skin/skin.h"

static int syslog_config_get_and_set(void)
{
    char level[32];
    char remote[64];
    char port[32];
    boole ok;

    /* 1) 读取一个字符串设置 */
    if (sgets_string(level, sizeof(level), "land@syslog", "level") == NULL)
    {
        return -1;
    }
    printf("current syslog level=%s\n", level);

    /* 2) 读取远程目标 */
    if (sgets_string(remote, sizeof(remote), "land@syslog", "remote") == NULL)
    {
        return -1;
    }
    if (sgets_string(port, sizeof(port), "land@syslog", "port") == NULL)
    {
        return -1;
    }
    printf("remote=%s port=%s\n", remote, port);

    /* 3) 更新常用设置 */
    ok = ssets_string("land@syslog", "debug", "level");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "file", "status");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "storage", "location");
    if (ok == false)
    {
        return -1;
    }

    /* 4) 更新远程目标 */
    ok = ssets_string("land@syslog", "192.168.8.250", "remote");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "514", "port");
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

注意事项：
- 使用 `sgets_string()` / `sgets()` 获取系统日志配置值。
- 使用 `ssets_string()` 更新系统日志配置值。

**调用组件方法**

```c
#include "skin/skin.h"

static void print_syslog_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `path[]`

```c
talk_t ret = scall("land@syslog", "path", NULL);
if (ret > tpanic)
{
    printf("path=%s size=%s\n", json_string(ret, "path"), json_string(ret, "size"));
    talk_free(ret);
}
else print_syslog_call_error("path", ret);
```

##### `clear[]`

```c
talk_t ret = scall("land@syslog", "clear", NULL);
if (ret != ttrue) print_syslog_call_error("clear", ret);
```

##### `show[]`

```c
talk_t ret = scall("land@syslog", "show", NULL);
if (ret != ttrue) print_syslog_call_error("show", ret);
```

##### `info[]`

```c
talk_t ret = scalls("land@syslog", "info", "this is info log from C");
if (ret != ttrue) print_syslog_call_error("info", ret);
```

##### `warn[]`

```c
talk_t ret = scalls("land@syslog", "warn", "this is warn log from C");
if (ret != ttrue) print_syslog_call_error("warn", ret);
```

##### `fault[]`

```c
talk_t ret = scalls("land@syslog", "fault", "this is fault log from C");
if (ret != ttrue) print_syslog_call_error("fault", ret);
```
