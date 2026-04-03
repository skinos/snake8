## land@register — 寄存器变量

寄存器变量是附加到任何对象的轻量级**易失性**键值槽。
它们通常用于保存运行时状态（接口设备名称、连接标志、指标计数器等），
其他组件可以读写这些变量而无需持久化到闪存。除非产品明确将其保存到其他地方，否则值在重启后丢失。

### 配置 ( `land@register` )

`land@register` 的**持久化配置对象**（通过 `land@register`、`land@register:path` 查询/设置，合并 `|{json}` 等）。


`land@register` **没有**持久化的 JSON 配置；使用**组件 API** 来读写每个对象的寄存器槽。

### 组件 API

+ `list[ [object] ]` **列出所有寄存器**，显示指定对象的寄存器列表
    - object ----------- [ string ]，省略时列出默认对象的寄存器
    - 无数据或失败返回 NULL 
    - 返回描述列表的 JSON  

    ```json
    // 方法返回的 JSON 属性介绍
    {
        "register name":"register value size in byte"
        // ... 更多寄存器列表
    }    
    ```
    示例，获取对象 ifname@wan 的寄存器列表
    ```shell
    land@register.list[ifname@wan]
    {
        "ifdev":"20",                    // 名为 ifdev 的寄存器大小为 20 字节
        "tid":"4",                       // 名为 tid 的寄存器大小为 4 字节
        "mode":"20",                     // 名为 mode 的寄存器大小为 20 字节
        "method":"20",
        "connect_failed":"4",
        "netdev":"20",
        "keeplive":"20",
        "metric":"20",
        "custom_dns":"20",
        "dns":"20",
        "dns2":"20",
        "delay_buf":"240",
        "delay_pos":"4",
        "delay":"4"
    }
    ```

+ `int[ [object], register name ]` **以整数形式显示寄存器值**  
    - object ----------- [ string ]，省略则使用默认对象（与单参数形式相同）
    - register name ----------- [ string ]
    - 无数据或失败返回 NULL     
    - 返回由 **`number2x`** 编码的 talk 整型值（命令行上通常显示为十进制数字）

    示例，显示 ifname@wan 对象的 tid
    ```shell
    land@register.int[ifname@wan, tid]
    5
    ```

+ `boole[ [object], register name ]` **以布尔值形式显示寄存器值**
    - object ----------- [ string ]，省略则使用默认对象
    - register name ----------- [ string ]
    - 无数据或失败返回 NULL     
    - 返回 **`string2x`** 包装的 **`true`** / **`false`** 字符串

    示例，显示 ifname@wan 对象的 keeplive
    ```shell
    land@register.boole[ifname@wan, keeplive]
    true
    ```

+ `string[ [object], register name ]` **以字符串形式显示寄存器值**
    - object ----------- [ string ]，省略则使用默认对象
    - register name ----------- [ string ]
    - 无数据或失败返回 NULL     
    - 返回 **`string2x`** 的 talk 字符串；槽位未设置或出错时为 **NULL**

    示例，显示 ifname@wan 对象的 mode 寄存器
    ```shell
    land@register.string[ifname@wan, mode]
    dhcpc
    ```

+ `dump10[ [object], register name ]` **以十六进制转储寄存器字节，每 10 字节换行**
    - object --------------- [ string ]，省略则使用默认对象
    - register name -------- [ string ]
    - 失败返回 tfalse；成功时输出到标准输出并返回 NULL

+ `dump100[ [object], register name ]` **以十六进制转储寄存器字节，每 100 字节换行**
    - object --------------- [ string ]，省略则使用默认对象
    - register name -------- [ string ]
    - 失败返回 tfalse；成功时输出到标准输出并返回 NULL

+ `set_int[ [object], register name, value ]` **设置寄存器的整数值**
    - object --------------- [ string ]，省略则设置全局寄存器命名空间
    - register name -------- [ string ]
    - value ---------------- [ number ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，将 ifname@wan 对象的 tid 设置为 5
    ```shell
    land@register.set_int[ifname@wan, tid, 5]
    ttrue
    ```

+ `set_boole[ [object], register name, value ]` **设置寄存器的布尔值**
    - object --------------- [ string ]，省略则设置全局寄存器
    - register name -------- [ string ]
    - value ---------------- [ "true", "false" ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，将 ifname@wan 对象的 keeplive 设置为 true
    ```shell
    land@register.set_boole[ifname@wan, keeplive, true]
    ttrue
    ```

+ `set_string[ [object], register name, [value] ]` **设置寄存器的字符串值**
    - object --------------- [ string ]，省略则设置全局寄存器
    - register name -------- [ string ]
    - value ---------------- [ string ]，省略或为空字符串则清除为空
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，设置 ifname@wan 对象的 myreg3 值
    ```shell
    land@register.set_string[ifname@wan, myreg3, myreg3valueisnull ]
    ttrue
    ```

### 生命周期 API

+ `setup[]` / `shut[]` — **当为 `land@register` 实现时**，启动/停止组件服务或钩子。调度遵循已安装 FPK 的 **init** / **uninit** / **joint** 清单。
### C 代码示例

**调用组件方法**

```c
#include "skin/skin.h"

static void print_register_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `list[ [object] ]`

```c
talk_t ret = scalls("land@register", "list", "ifname@wan");
if (ret > tpanic)
{
    printf("register list json ready\n");
    talk_free(ret);
}
else print_register_call_error("list", ret);
```

##### `int[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "int", "ifname@wan,tid");
if (ret > tpanic)
{
    printf("tid=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("int", ret);
```

##### `boole[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "boole", "ifname@wan,keeplive");
if (ret > tpanic)
{
    printf("keeplive=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("boole", ret);
```

##### `string[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "string", "ifname@wan,mode");
if (ret > tpanic)
{
    printf("mode=%s\n", x2string(ret));
    talk_free(ret);
}
else print_register_call_error("string", ret);
```

##### `dump10[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "dump10", "ifname@wan,mac");
if (ret == tfalse || ret == terror || ret == tpanic) print_register_call_error("dump10", ret);
```

##### `dump100[ [object], register name ]`

```c
talk_t ret = scalls("land@register", "dump100", "ifname@wan,mac");
if (ret == tfalse || ret == terror || ret == tpanic) print_register_call_error("dump100", ret);
```

##### `set_int[ [object], register name, value ]`

```c
talk_t ret = scalls("land@register", "set_int", "ifname@wan,tid,7");
if (ret != ttrue) print_register_call_error("set_int", ret);
```

##### `set_boole[ [object], register name, value ]`

```c
talk_t ret = scalls("land@register", "set_boole", "ifname@wan,keeplive,true");
if (ret != ttrue) print_register_call_error("set_boole", ret);
```

##### `set_string[ [object], register name, [value] ]`

```c
talk_t ret = scalls("land@register", "set_string", "ifname@wan,myreg3,myvalue");
if (ret != ttrue) print_register_call_error("set_string", ret);
```
