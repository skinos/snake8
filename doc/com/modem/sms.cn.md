## modem@sms — LTE/NR 短信管理
管理 LTE/NR 基带模块短信服务。

### 配置 ( `modem@sms` )
**modem@sms** 是绑定到第一个 LTE 基带模块的短信服务。
**modem@sms2** 是绑定到第二个 LTE 基带模块的短信服务。

```json
// 属性说明 
{
    // 启用或禁用短信转命令处理。
    // "enable" 表示接收到的短信可以被解析为 HE 命令。
    "he":"启用或禁用 HE 命令",                  // [ "disable", "enable" ]

    // HE 命令的可选发送者白名单。
    // 多个匹配模式用 ';' 分隔, 每个模式按子串匹配。
    "he_contact":"设置可发送 HE 命令的联系人",      // [ string ]

    // HE 命令文本前的可选命令前缀。
    // 如已配置, 不包含此前缀的消息将被忽略。
    "he_prefix":"设置 HE 命令的前缀"              // [ string ]
}
```

示例，显示所有配置
```shell
modem@sms
{
    "he":"enable",                                         # 启用短信转命令功能
    "he_contact":"17688704240;10000;+8617688704240",       # 仅接受这些电话号码的命令（子串匹配）
    "he_prefix":"CMD:"                                     # 短信内容必须以 "CMD:" 开头才会被当作命令处理
}
```

> **安全警告**: 启用 `he` 功能时, 强烈建议配置 `he_contact` 以限制哪些电话号码可以执行命令。如果没有此限制, 任何接收到的短信都可能执行系统命令。

#### 短信转命令 (HE) 功能
当 `he` 设置为 `enable` 时, 接收到的短信可以被解释为系统命令并执行。执行结果会通过短信自动回复给发送者。

**字符转换规则**:

由于某些特殊字符在短信中可能难以输入或传输, 因此执行以下字符转换:

| 短信字符 | 转换为 | 含义 |
|--------------|--------------|---------|
| `>` 或 `O` | `@` | 对象分隔符 |
| `!` 或 `F` | `.` | 属性分隔符 |
| `?` 或 `A` | `:` | 方法调用 |
| `%` 或 `U` | `/` | 路径分隔符 |
| `G` | `=` | 赋值 |
| `R` | `\|` | 管道 |
| `(`, `{`, `S` | `[` | 参数开始 |
| `)`, `}`, `E` | `]` | 参数结束 |

**示例**: 通过短信发送命令
```
# 原始命令: land@machine.restart[5]
# 短信输入: landOmachineFrestartS5E
# 或: land>machine!restart{5}
```

**HE 命令执行流程**:
1. 接收短信并存储在接收目录中
2. 如果 `he` 已启用, 检查短信内容
3. 如果已配置 `he_contact`, 发送者号码必须匹配 (子串匹配)
4. 如果已配置 `he_prefix`, 短信内容必须以此前缀开头
5. 根据转换规则进行字符转换
6. 作为 HE 系统命令执行
7. 执行结果通过短信回复给发送者

**注意**: UCS-2 编码的短信不会作为 HE 命令处理。

### 组件 API
**可直接调用** 的 API, 通过 HE / eline / HTTP `/he` 调用。
**modem@sms** 是第一个 LTE 基带模块短信服务。   
**modem@sms2** 是第二个 LTE 基带模块短信服务。


+ `send[ contact, content ]` **发送短信**
    - contact ----------- [ string ], 目标电话号码
    - content ----------- [ string ], 短信文本内容
    - 参数无效或发送命令失败: 返回 tfalse
    - 请求已接受: 返回 ttrue
    - 此 API 是"发送即忘"的发送请求; 此返回值不保证送达回执
    
    示例: 向 17688704240 发送 "i miss you"
    ```shell
    modem@sms.send[ 8617688704240, i\ miss\ you ]
    ttrue
    ```

+ `list[]` **列出所有已接收的短信**  
    - 返回描述已接收短信列表的 JSON 对象
    - 如果没有消息, 返回空 JSON 对象 (`{}`)
    - 每个键是短信守护进程生成的短信文件标识符
    ```json
    // 方法返回的 JSON 属性说明
    {
        "SMS Identify name":           // [ string ]:{}
        {
            "id":"短信标识名",    // [ string ]
            "contact":"短信发送者",      // [ string ]
            "date":"接收时间",       // [ string ], 典型格式 "YY-MM-DD HH:MM:SS"
            "content":"短信内容"      // [ string ], UTF-8 内容
        }
        // ... 更多短信
    }
    ```
    
    示例: 列出所有已接收的短信
    ```shell
    modem@sms.list
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        },
        "sms2.ugFCOH":
        {
            "id":"sms2.ugFCOH",
            "contact":"10001",
            "date":"25-11-28 11:22:46",
            "content":"温馨提醒：您所订购的套餐月基本费扣费规则为每月1日凌晨一次性扣取，为避免扣费后手机余额不足被停机，请及时缴费。更多详情请访问"中国电信"APP( http://a.189.cn/FnnBmF )"我们百倍努力，期待您的十分满意评价"。【中国电信】"
        }
    }
    ```

+ `delete[ SMS Identify name ]` **删除短信**
    - SMS Identify name ----------- [ string ], `list[]` 返回的 `id`
    - 无效 `id`: 返回 tfalse
    - 有效请求: 返回 ttrue
    - 这将从本地接收存储中删除短信记录文件
    
    示例: 删除短信 `sms2.ugFCOH`
    ```shell
    modem@sms.list                        # 先列出
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        },
        "sms2.ugFCOH":
        {
            "id":"sms2.ugFCOH",
            "contact":"10001",
            "date":"25-11-28 11:22:46",
            "content":"温馨提醒：您所订购的套餐月基本费扣费规则为每月1日凌晨一次性扣取，为避免扣费后手机余额不足被停机，请及时缴费。更多详情请访问"中国电信"APP( http://a.189.cn/FnnBmF )"我们百倍努力，期待您的十分满意评价"。【中国电信】"
        }
    }
    modem@sms.delete[ sms2.ugFCOH ]     # 删除 sms2.ugFCOH
    ttrue
    modem@sms.list                      # 再次列出
    {
        "sms2.X54gbI":
        {
            "id":"sms2.X54gbI",
            "contact":"17266972742",
            "date":"25-11-28 01:43:27",
            "content":"那就是说他"
        },
        "sms2.UcSHRq":
        {
            "id":"sms2.UcSHRq",
            "contact":"10000",
            "date":"25-11-28 10:57:11",
            "content":"广东省森林防灭火指挥部办公室提醒您：当前，天气持续晴朗干燥，全省所有县区均发布森林火险红色预警，高火险天气极易引发森林火灾。请广大市民群众积极配合，进山不带火、入林不吸烟，严格火源管控，不因一时疏忽大意引发山火而触犯法律法规。绿美广东，人人有责！（省预警中心11月28日发布）"
        },
        "sms2.mnw1mj":
        {
            "id":"sms2.mnw1mj",
            "contact":"17688704240",
            "date":"25-11-28 21:36:26",
            "content":"fuck you"
        },
        "sms2.qiS9pF":
        {
            "id":"sms2.qiS9pF",
            "contact":"10692682810037173",
            "date":"25-11-28 12:44:34",
            "content":"【口味王】您账户有1500积分即将清零，过期无效！立即登录查看→ l.kwwblcj.xyz/k/?A 拒收请回复R"
        }
    }    
    ```

+ `action[ event, file ]` **短信事件处理 (内部使用)**
    - 此 API 由短信守护进程内部使用, 用于处理接收到的短信
    - 当 `he` 启用时, 接收到的短信会触发此 API 执行 HE 命令
    - 执行结果通过短信自动回复给发送者
    - **注意**: 此 API 不适用于手动调用

#### 短信存储
短信存储在以下目录中:

| 目录 | 用途 |
|-----------|---------|
| `{var}/modem@sms.sent/` | 已成功发送的短信 |
| `{var}/modem@sms.failed/` | 发送失败的短信 |
| `{var}/modem@sms.checked/` | 已检查的短信 |
| `{var}/modem@sms.outgoing/` | 发送队列中的短信 |
| `{internal}/modem@sms.incoming/` | 已接收的短信 (由 `list[]` 查询) |

#### 编码支持
- **GSM 7-bit**: 标准短信编码
- **UCS-2**: 非 ASCII 字符 (例如中文) 的 Unicode 编码。列出时自动转换为 UTF-8
- **Binary**: 二进制短信内容不作为 HE 命令处理

#### 依赖
短信服务依赖以下外部组件:

- `smsd` - 短信守护进程, 用于底层短信处理
- `sendsms.sh` - 发送短信的脚本
- `iconv` - 字符编码转换工具

#### 完整配置示例
```json
{
    "he": "enable",
    "he_contact": "17688704240;10000;+8617688704240",
    "he_prefix": "CMD:"
}
```

**说明**:
- `he`: 启用短信转命令功能
- `he_contact`: 仅包含 "17688704240"、"10000" 或 "+8617688704240" 的号码发送的短信会被作为命令处理 (子串匹配)
- `he_prefix`: 短信内容必须以 "CMD:" 开头才会被作为命令处理

**HE 命令短信示例**:
```
CMD:landOmachineFstatus
```
这将执行 `land@machine.status` 并回复结果。

### 生命周期 API
+ `setup[]` / `shut[]` -- 由 **`usbdrv@...`** / 基带模块驱动注册驱动; **未**列在默认基带模块包的 **`init`** 表中。
+ 请参阅基带模块驱动组件文档 (**`modem@ec2x`**, **`modem@rm500u`**, ...) 了解启动方式。


### C 代码示例
**读取和更新配置**

```c
#include "skin/skin.h"

static int example_config_modem_sms(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "modem@sms", "status") == NULL)
        return -1;
    return ssets_string("modem@sms", "enable", "status") ? 0 : -1;
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

/* e.g. scall("modem@sms", "list", NULL); talk_free if JSON */
```
