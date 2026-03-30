## arch@ethernet — SoC 以太网 / 交换机

为每个端口注册 **`ethernet@lanN`** 对象到 **`network@frame`**，应用 **`network_mode`** 特定的交换机/端口 JSON，并暴露链路级方法（`up`、`down`、`status`、`online` 等）。根对象 **`arch@ethernet`**；子对象 **`ethernet@lan1`** 等。实现代码：[`ethernet/ethernet.c`](ethernet/ethernet.c)。默认配置示例：[`odm/rk3568/ethernet.cfg`](odm/rk3568/ethernet.cfg)。

### 配置 ( `arch@ethernet` )
**`arch@ethernet`** 的 **已保存配置对象**（通过 `arch@ethernet`、`arch@ethernet:path`、合并 `|{json}` 等方式查询/设置）。结构由注册值 **`network_mode`**（回退 **`NETWORK_MODE`**）选择。

```json
// Attributes introduction
{
    "default": {
        "mode":"switch or vlan",               // [ string ], physical switching mode
        "ifdev": {                             // kernel netdev name mapping per port
            "lan1":"kernel netdev name",       // [ string ]
            "lan2":"…"                         // [ string ]
        },
        "phy": { }                             // optional PHY configuration
    }
}
```

**子对象** `ethernet@lanN` 使用相同模式 JSON 的 **`ifdev/<name>`** 分支。

示例，显示所有配置
```shell
arch@ethernet
{
    "default":                                 # 默认模式配置
    {
        "mode":"switch",                           # 物理交换模式：交换机
        "ifdev":                                   # 端口到网络设备映射
        {
            "lan1":"lan1",                             # 端口 lan1 映射到内核网络设备 lan1
            "lan2":"lan2",                             # 端口 lan2 映射到内核网络设备 lan2
            "lan3":"lan3",                             # 端口 lan3 映射到内核网络设备 lan3
            "lan4":"wan"                               # 端口 lan4 映射到内核网络设备 wan
        }
    }
}
```

示例，合并 `ifdev` 映射
```shell
arch@ethernet:default|{"mode":"switch","ifdev":{"lan1":"eth0","lan2":"eth1"}}
ttrue
```

### 组件 API
+ `netdev[]` **返回此 `ethernet@…` 对象的内核接口名称**。

+ `up[]` / `down[]` **管理性地启用/禁用接口**（此处不涉及 IP 配置）。

+ `connect[]` / `connected[]` **链路状态辅助方法**（`connected` 检查 **`IFF_RUNNING`**）。

+ `status[]` **返回 JSON 格式的链路/驱动状态**（返回结构详见代码）。

+ `reset[]` / `setmac[]` / `hwnat[]` / `keeplive[]` **平台钩子**，实现于 [`ethernet/ethernet.c`](ethernet/ethernet.c)。

+ `online[]` / `offline[]` **当载波出现/消失时通知协议栈**。

### 生命周期 API
+ `setup[]` **向 `network@frame` 注册端口，可选 VLAN 表，从注册值应用 MAC 覆盖**，*成功返回 `ttrue`*
    - 默认 **`odm/rk3568/prj.json`** → **`init` → `bus` → `arch@ethernet.setup`**。

+ `shut[]` **注销并拆除**，*成功返回 `ttrue`*
    - 此示例的默认 **`uninit`** 中 **未** 列出；需显式调用或在产品 **`prj.json`** 中添加。

### Joint 处理器
在默认 **`odm/rk3568/prj.json`** 中 **`arch@ethernet`** **未** 声明任何 Joint 处理器（产品镜像可能将 **`network/online`** 类型的键映射到 **`online[]`** / **`offline[]`**）。


### C 代码示例
```c
#include "skin/skin.h"

static void example_ethernet_status(void)
{
    talk_t ret = scall("ethernet@lan1", "status", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```
