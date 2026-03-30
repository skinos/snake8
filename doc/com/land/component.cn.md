## land@component — 组件注册

系统中的每个对象名称都必须有组件实现作为支撑。
`land@component` 提供了在运行时将对象名称绑定到组件的 API——
当映射关系尚未通过已安装的 FPK 包建立时非常有用。它没有自己的 JSON 配置；所有工作都通过下面的 `register` / `unregister` 调用完成。

### 配置 ( `land@component` )

`land@component` 的**持久化配置对象**（通过 `land@component`、`land@component:path` 查询/设置，合并 `|{json}` 等）。


`land@component` **没有** JSON 配置对象。映射仅通过**组件 API** 中的 **`register`** / **`unregister`** 完成。

### 组件 API


+ `register[ object, component name ]` **注册一个对象**
    - object ------------------ [ string ]
    - component name ----------- [ string ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，使用 ifname@ethcon 注册一个对象
    ```shell
    land@component.register[ ifname@wan3, ifname@ethcon ]
    ttrue
    ```

+ `unregister[ object ]` **删除一个对象**
    - object ------------------ [ string ]
    - 失败返回 tfalse
    - 成功返回 ttrue

    示例，删除一个对象
    ```shell
    land@component.unregister[ ifname@wan3 ]
    ttrue
    ```

### 生命周期 API

+ `setup[]` — 在平台初始化时调用，用于注册内置组件。


### C 代码示例

**调用组件方法**

```c
#include "skin/skin.h"

static void print_component_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `register[ object, component name ]`

```c
talk_t ret = scalls("land@component", "register", "ifname@wan3,ifname@ethcon");
if (ret != ttrue) print_component_call_error("register", ret);
```

##### `unregister[ object ]`

```c
talk_t ret = scalls("land@component", "unregister", "ifname@wan3");
if (ret != ttrue) print_component_call_error("unregister", ret);
```
