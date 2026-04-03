## land@auth — 用户与权限管理

管理用户名和权限。尽可能使用下方的**组件 API** 而非直接编辑原始配置。
配置结构分为三层：
- 用户名可以属于多个组
- 组可以拥有域的权限
- 由于三层结构相对复杂，组和域权限暂时采用一对一关系
    > 即只有用户名和组，每个组拥有独立的域权限
- 组在系统中是预建的（基于功能内置），可以创建和删除对应的用户名
    > 例如添加了存储功能后，存储中内置了 "nas" 组，可以添加或删除属于 "nas" 组的用户名


### 配置 ( `land@auth` )

`land@auth` 的**持久化配置对象**（通过 `land@auth` 查询/设置，路径位于 `user/` 和 `group/` 下，合并 `|{json}` 等）。直接编辑容易出错；日常账户操作请使用 API。

```json
// 属性介绍
{
    "user":        //  用户名列表，系统中所有账户都在此节点下
    {
        "user name":                  // [ string ]，可自定义用户名
        {
            "id":"username identify number",                        // [ number ]
            "key":"username password",                              // [ string ]
            "key_check":"key need strength",                        // [ "disable", "enable" ]
            "key_failed_time":"Number of consecutive failures",     // [ number ]
            "key_failed_wait":"failures to block",                  // [ number ]，单位为秒

            "group":                     // 用户名所属的组列表
            {
                "group name":"belongs state"  // [ string ]: [ "disable","enable" ]，"enable" 表示属于，"disable" 表示不属于
                // "...":"..."                // 属于多少个组就显示多少条属性
            },
            "domain":                    // 此用户名的所有域配置列表
            {
                "domain name":
                {
                    "key":"In this domain specialized password"        // [ string ]，为空表示使用默认密码
                }
                // "...":{ ... }                                  // 有多少个域就显示多少条属性
            }
        }
        // "...":{...}                  // 有多少个用户名就显示多少条属性
    },
    "group":     // 组列表，系统中所有组都在此节点下
    {
        "group name":
        {
            "id":"group identify",     // [ number ]
            "domain":                  // 该组下所有域权限列表
            {
                "domain name":"enable state"  // [ string ]: [ "disable","enable" ]
                // "...":"..."             // 有多少个域就显示多少条属性
            }
        }
        // "...":{...}                  // 有多少个组就显示多少条属性
    }
}
```

示例，显示所有账户配置
```shell
land@auth
{
    "user":
    {
        "admin":                       # 用户名：admin
        {
            "id":"0",                         # admin 用户 ID 为 0
            "key":"eYgJU9Koun1yPYJ78JeH2Q==", # admin 默认域的加密密码
            "group":                          # admin 属于的组：admin、nas、vpn
            {
                "admin":"enable",
                "vpn":"enable",
                "nas":"enable"
            },
            "domain":
            {
                "admin":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="       # admin 域的加密密码
                },
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="       # nas 域的加密密码
                }
            }
        },
        "eason":                         # 用户名：eason
        {
            "id":"1000",                      # eason 用户 ID 为 1000
            "key":"CL088bD9dcJUgNzhCKBnfg==", # eason 默认域的加密密码
            "group":                          # eason 属于的组：nas
            {
                "nas":"enable"
            }
        }
    },
    "group":
    {
        "admin":                       # admin 组
        {
            "id":"0",                  # admin 组标识为 0
            "domain":                  # admin 组拥有 admin 域权限
            {
                "admin":"enable"
            }
        },
        "vpn":                         # vpn 组
        {
            "id":"1001",               # vpn 组标识为 1001
            "domain":                  # vpn 组拥有 vpn 域权限
            {
                "vpn":"enable"
            }
        },
        "nas":                         # nas 组
        {
            "id":"1002",               # nas 组标识为 1002
            "domain":                  # nas 组拥有 nas 域权限
            {
                "nas":"enable"
            }
        }
    }
}
```  

示例，合并部分字段（仅作说明；实际账户操作请优先使用 API）
```shell
land@auth|{"user":{"admin":{"key_check":"enable"}}}
ttrue
```

### 组件 API

+ `domain[ username ]` **按用户已启用的组合并域相关 JSON**   
    - username ----------- [ string ]
    - 失败返回 NULL（例如缺少用户名）
    - 成功返回一个 JSON 对象（可能为空 `{}`）。实现会遍历 **`user/<用户名>/group`**：对每个值为 **`enable`** 的组，从已保存配置读取 **`group/<组名>/domain`**，并用 **`json_patch`** 依次合并。返回值**不是**简单的 `"域名":"enable"` 平面映射，其结构取决于各组下 **`domain`** 对象里实际存的内容。

    示例（具体形状取决于你的 `group/*/domain` 配置）
    ```shell
    land@auth.domain[ admin ]
    { }
    ```

+ `check[ [domain], username, password ]` **检查用户名和密码是否正确**   
    - domain ----------- [ string ]，指定特定域，默认为 common
    - username ----------- [ string ] 
    - password ----------- [ string ] 
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，检查用户名 admin 是否正确
    ```shell
    land@auth.check[ ,admin, admin ]
    ttrue
    ```

    示例，使用错误密码检查用户名 admin
    ```
    land@auth.check[ ,admin, passwrong ]
    tfalse
    ```

+ `match[ [domain], username, ciphertext ]` **使用加密密码检查用户名是否正确**   
    - domain ----------- [ string ]，指定特定域，默认为 common
    - username ----------- [ string ] 
    - ciphertext ----------- [ string ]，加密密码
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，检查用户名 admin 是否正确
    ```shell
    land@auth.match[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM= ]
    ttrue
    ```

    示例，使用错误密码检查用户名 admin
    ```
    land@auth.match[ ,admin, admin ]
    tfalse
    ```


+ `modify[ [domain], username, password, [new password], [new username] ]` **使用原始密码修改用户名或密码**   
    - domain ----------- [ string ]，指定特定域，默认为 common
    - username ----------- [ string ] 
    - password ----------- [ string ] 
    - new password ----------- [ string ] 
    - new username ----------- [ string ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，将 admin 的密码修改为 12345
    ```shell
    land@auth.modify[ ,admin, admin, 12345 ]
    ttrue
    ```
    示例，将用户名 admin 修改为 Bob
    ```
    land@auth.modify[ ,admin, 12345, , Bob ]
    ttrue
    ```

+ `change[ [domain], username, ciphertext, [new password], [new username] ]` **使用原始加密密码修改用户名或密码**   
    - domain ----------- [ string ]，指定特定域，默认为 common
    - username ----------- [ string ] 
    - ciphertext ----------- [ string ]，加密密码 
    - new password ----------- [ string ] 
    - new username ----------- [ string ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，将 admin 的密码修改为 12345
    ```shell
    land@auth.change[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM=, 12345 ]
    ttrue
    ```
    示例，将用户名 admin 修改为 Bob
    ```
    land@auth.change[ ,admin, ODI3Y2NiMGVlYThhNzA2YzRjMzRhMTY4OTFmODRlN2I=, , Bob ]
    ttrue
    ```


+ `add[ [domain], username, password [, group, ... ] ]` **添加新用户名**   
    - domain ----------- [ string ]，可选；用于在 **`user/<名>/domain/<域>/key`** 下保存域专属密钥
    - username ----------- [ string ]（必填）
    - password ----------- [ string ]（解析上可选；若提供则经 **`simple_encode`** 后写入）
    - group ... -------- 从第 4 个参数起的可选组名：每个非 NULL 名称会在新用户的 **`group`** 映射中置为 **`"enable"`**
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，添加用户名 xiaomi，密码为 xiaomin123
    ```shell
    land@auth.add[ ,xiaomi, xiaomin123 ]
    ttrue
    ```

+ `delete[ username[, ...] ]` **删除用户名**   
    - username ----------- [ string ] 
    - ... ----------- [ string ]，可以删除多个用户名
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除用户名 xiaomi
    ```shell
    land@auth.delete[ xiaomi ]
    ttrue
    ```
    
    示例，删除用户名 Alice 和 Bob
    ```
    land@auth.delete[ Alice, Bob ]
    ttrue
    ```

+ `list[ [group] ]` **列出用户（可选按组成员过滤）**   
    - group ----------- [ string ]，可选；若指定，仅返回 **`group/<组名> == "enable"`** 的用户
    - 失败返回 NULL
    - 返回 JSON：每个用户包含 **`key`**（默认密码字段）及从该用户 **`domain`** 节点经 **`json_cut_value`** 得到的 **`domain`** 子树，风格与配置中一致（常为 **`simple_encode`** 后的串）。   
    ```json
    // 实现返回形状（示意）
    {
        "user name":
        {
            "key":"编码后或明文密码串",
            "domain": { "...": { "key":"..." } }
        }
    }
    ```

    示例，列出所有 admin 组的用户名
    ```shell
    land@auth.list[ admin ]
    {
        "admin":
        {
            "key":"eYgJU9Koun1yPYJ78JeH2Q==",
            "domain":
            {
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="
                },
                "wui":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="
                }
            }
        }
    }
    ```

    示例，列出所有用户名
    ```
    land@auth.list
    {
        "admin":
        {
            "key":"eYgJU9Koun1yPYJ78JeH2Q==",
            "domain":
            {
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="
                },
                "wui":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="
                }
            }
        }
        "eason":
        {
            "key":"CL088bD9dcJUgNzhCKBnfg=="
        }
    }
    ```


+ `md5[ string ]` **计算字符串的 MD5 哈希值**
    - string ----------- [ string ]，要计算哈希的字符串
    - 失败返回 NULL
    - 返回 MD5 哈希字符串（32 位十六进制字符）

    示例，计算 "admin" 的 MD5
    ```shell
    land@auth.md5[ admin ]
    21232f297a57a5a743894a0e4a801fc3
    ```

+ `b64_encode[ string ]` **对字符串进行 Base64 编码**
    - string ----------- [ string ]，要编码的字符串
    - 失败返回 NULL
    - 返回 Base64 编码后的字符串

    示例，Base64 编码 "admin"
    ```shell
    land@auth.b64_encode[ admin ]
    YWRtaW4=
    ```

+ `b64_decode[ string ]` **对字符串进行 Base64 解码**
    - string ----------- [ string ]，要解码的 Base64 字符串
    - 失败返回 NULL
    - 返回解码后的字符串

    示例，Base64 解码 "YWRtaW4="
    ```shell
    land@auth.b64_decode[ YWRtaW4= ]
    admin
    ```

+ `encode[ string, [key] ]` **简单加密字符串**
    - string ----------- [ string ]，要加密的字符串
    - key -------------- [ string ]，可选，作为令牌；省略或 NULL 使用实现内默认
    - 失败返回 NULL
    - 返回加密后的字符串

    示例，加密 "admin"
    ```shell
    land@auth.encode[ admin ]
    eYgJU9Koun1yPYJ78JeH2Q==
    ```

+ `decode[ string, [key] ]` **简单解密字符串**
    - string ----------- [ string ]，要解密的加密字符串
    - key -------------- [ string ]，可选，作为令牌（须与 **`encode`** 一致）
    - 失败返回 NULL
    - 返回解密后的字符串

    示例，解密 "eYgJU9Koun1yPYJ78JeH2Q=="
    ```shell
    land@auth.decode[ eYgJU9Koun1yPYJ78JeH2Q== ]
    admin
    ```

### 生命周期 API


+ `setup[]` **初始化认证组件**，*成功返回 ttrue，失败返回 tfalse，错误返回 terror*
    - 这是系统启动期间自动调用的生命周期方法
    - 当寄存器 **`scope`** 为 **`wrt`** 或 **`platform`** 为 **`slave`** 时，**`setup`** 直接返回 **`ttrue`**，不合并 passwd/shadow（**`set`** 同样受此保护：写入返回 **false** 且不执行 **`_refresh`**）。
    - 否则执行 **`_refresh`**，将配置中的用户/组合并到系统账户文件（如 **`/var/passwd`** 及相关文件，具体因镜像而异）。
    - 不建议手动调用


### 发布的 Joint 事件

以下 Joint 事件在认证数据变更时发布。其他组件可以在运行时订阅（Joint 注册 / **`land@joint`**）。

| 事件 | 描述 |
|------|------|
| `auth/modify` | 当认证配置或用户账户被修改时发送。在 `set`（配置变更）、`add`（新增账户）、`delete`（删除账户）、`modify`（密码/用户名变更）或 `change`（通过密文变更密码）后触发。事件参数为受影响的账户名，整个配置变更时为 `NULL`。 |


### C 代码示例

**读取和更新配置**

```c
#include "skin/skin.h"

static int auth_config_get_and_set(void)
{
    char key_check[32];
    talk_t user_node;
    talk_t domain_key;
    boole ok;

    /* 1) 读取一个字符串配置值 */
    if (sgets_string(key_check, sizeof(key_check), "land@auth", "user/admin/key_check") == NULL)
    {
        return -1;
    }
    printf("admin key_check=%s\n", key_check);

    /* 2) 读取一个对象节点 */
    user_node = sgets("land@auth", "user/admin");
    if (user_node == NULL || user_node <= tpanic)
    {
        return -1;
    }
    printf("admin default key=%s\n", json_string(user_node, "key"));
    talk_free(user_node);

    /* 3) 更新字符串配置值 */
    ok = ssets_string("land@auth", "enable", "user/admin/key_check");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@auth", "disable", "user/admin/group/vpn");
    if (ok == false)
    {
        return -1;
    }

    /* 4) 使用 talk/json 接口更新一个域密码值 */
    domain_key = string2x("new_encoded_key_here");
    ok = ssets("land@auth", domain_key, "user/admin/domain/wui/key");
    talk_free(domain_key);
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

注意事项：
- 使用 `sgets_string()` / `sgets()` 读取认证配置路径。
- 使用 `ssets_string()` / `ssets()` 更新认证配置路径。
- 任何持有 JSON（或其他堆分配的图节点）的 `talk_t` 返回值在使用完毕后必须通过 `talk_free()` 释放；仅使用 `he` 操作的通常不需要直接调用这些 C API。

**调用组件方法**

```c
#include "skin/skin.h"

static void print_auth_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `domain[ username ]`

```c
talk_t ret = scalls("land@auth", "domain", "admin");
if (ret > tpanic)
{
    printf("admin domain list ready\n");
    talk_free(ret);
}
else print_auth_call_error("domain", ret);
```

##### `check[ [domain], username, password ]`

```c
talk_t ret = scalls("land@auth", "check", "wui,admin,admin_password");
if (ret != ttrue) print_auth_call_error("check", ret);
```

##### `match[ [domain], username, ciphertext ]`

```c
const char *ciphertext = "CIPHER_KEY_TEXT";
talk_t ret = scalls("land@auth", "match", "wui,admin,%s", ciphertext);
if (ret != ttrue) print_auth_call_error("match", ret);
```

##### `modify[ [domain], username, password, [new password], [new username] ]`

```c
talk_t ret = scalls("land@auth", "modify", "wui,admin,oldpass,newpass,admin2");
if (ret != ttrue) print_auth_call_error("modify", ret);
```

##### `change[ [domain], username, ciphertext, [new password], [new username] ]`

```c
const char *ciphertext = "CIPHER_KEY_TEXT";
talk_t ret = scalls("land@auth", "change", "wui,admin,%s,newpass,admin2", ciphertext);
if (ret != ttrue) print_auth_call_error("change", ret);
```

##### `add[ [domain], username, password [, group, ... ] ]`

```c
talk_t ret = scalls("land@auth", "add", "wui,alice,alice_password");
if (ret != ttrue) print_auth_call_error("add", ret);
```

##### `delete[ username[, ...] ]`

```c
talk_t ret = scalls("land@auth", "delete", "alice,bob");
if (ret != ttrue) print_auth_call_error("delete", ret);
```

##### `list[ [group] ]`

```c
talk_t ret = scalls("land@auth", "list", "admin");
if (ret > tpanic)
{
    printf("admin users json ready\n");
    talk_free(ret);
}
else print_auth_call_error("list", ret);
```
