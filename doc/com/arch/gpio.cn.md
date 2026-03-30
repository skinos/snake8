## arch@gpio — GPIO 和 LED 控制

板级 GPIO 编号、LED 模式、**`in` / `out` / `timer` / `listen` / `action`**，以及用于 netlink/按键处理的长驻 **`service`**。实现代码：[`gpio/gpio.c`](gpio/gpio.c)，可选 [`gpio-init.sh`](gpio-init.sh)，[`odm/rk3568/gpio.cfg`](odm/rk3568/gpio.cfg)（ODM 默认配置示例）。

### 配置 ( `arch@gpio` )
**`arch@gpio`** 的 **已保存配置对象**（通过 `arch@gpio`、`arch@gpio:path`、合并 `|{json}` 等方式查询/设置）。

```json
// Attributes introduction
{
    "led":"disable or omit for normal LED policy",   // [ "disable" ], factory mode overrides via register
    "sys":"GPIO number for system LED",              // [ string / number as string ], ODM-specific
    "sys_out":"active level 0 or 1",                 // [ string ]
    "modem@lte_state":"93",                          // example: LTE LED GPIO map (see gpio.cfg)
    "modem@lte_state_out":"1"
}
```

附加键为 **GPIO 别名 → 引脚编号** 字符串，在 **`setup[]`** 时读取并存储到对象注册值中，供 **`in[]`** / **`out[]`** 使用。

示例，显示所有配置
```shell
arch@gpio
{
    "sys":"92",                                # 系统 LED GPIO 引脚 92
    "sys_out":"1",                             # 系统 LED 有效电平：高
    "modem@lte_state":"93",                    # LTE 模组状态 LED GPIO 引脚 93
    "modem@lte_state_out":"1",                 # LTE 状态 LED 有效电平：高
    "modem@lte_reset":"6",                     # LTE 模组复位 GPIO 引脚 6
    "modem@lte_reset_out":"1",                 # LTE 复位有效电平：高
    "modem@lte2_state":"91",                   # 第二 LTE 模组状态 LED GPIO 引脚 91
    "modem@lte2_state_out":"1",                # 第二 LTE 状态 LED 有效电平：高
    "modem@lte2_reset":"148",                  # 第二 LTE 模组复位 GPIO 引脚 148
    "modem@lte2_reset_out":"1",                # 第二 LTE 复位有效电平：高
    "wifi@asta_state":"90",                    # 5.8G 客户端状态 LED GPIO 引脚 90
    "wifi@asta_state_out":"1"                  # 5.8G 客户端状态 LED 有效电平：高
}
```

示例，合并多个 GPIO 映射
```shell
arch@gpio|{"sys":"92","sys_out":"1"}
ttrue
```

### 组件 API
**可直接调用** 的 API：通过 HE / eline / HTTP `/he` 调用 `arch@gpio.method`。

+ `in[ name_or_number ]` **采样 GPIO 输入**，*返回 JSON 或标量读取值；引脚无效时返回 `NULL` / 错误*
    - 传入 **GPIO 数字编号** 或 `arch@gpio` JSON 中的 **已配置别名**。

+ `out[ name_or_level, value ]` **驱动 GPIO 输出**（详见代码——逗号分隔参数）。

+ `timer[ … ]` **为命名输出（如 `sys`）配置定时器式闪烁模式**。

+ `listen[ … ]` / `action[]` **按键 / 事件钩子**，通过 netlink 实现（详见 [`gpio/gpio.c`](gpio/gpio.c)）。

### 生命周期 API
+ `setup[]` **加载 GPIO 映射，应用 LED 策略，存在 `gpio-init.sh` 时运行该脚本，启动 `service`**，*成功返回 `ttrue`*
    - 默认 **`odm/rk3568/prj.json`** → **`init` → `arch` → `arch@gpio.setup`**。

+ `shut[]` **停止受监控的 `service`**，*成功返回 `ttrue`*
    - 默认 **`uninit` → `arch` → `arch@gpio.shut`**。

**注意：** 同一 **`prj.json`** 在 **`init` → `arch`** 下引用了 **`arch@hotplug.setup`**，但默认 **`com`** 在此目录树中未定义 **`hotplug`** 组件——如需使用，请在 ODM/产品包中提供。


### C 代码示例
```c
#include "skin/skin.h"

static void example_gpio_out(void)
{
    (void)scalls("arch@gpio", "out", "power,1");
}
```
