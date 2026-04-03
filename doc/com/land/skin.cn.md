# libskin API 文档

> **读者对象：** 本卷面向**嵌入式集成**（C 级别 API）。仅使用 **`he`** 或 Web UI 的操作人员应先从 `he.cn.md` 和各组件指南开始。

## 概述

**libskin** 是 Skinos 组件背后的平台库：通信、配置、日志记录、服务和相关设施。

**主头文件：** `#include "skin.h"` 按依赖顺序引入 `stdhead.h`（标准 C/POSIX 包含）、`skinhead.h`（类型、限制、`*_COM` 常量）和 `skinapi.h`（如 `scalls`、`machine_config` 等快捷方式）。这与磁盘上伞形头文件旁边的布局一致。如需更小的编译面，你可以只包含所需的头文件（例如 `talk.h` + `com.h`）；其他 Markdown 中的示例默认使用完整的 `skin.h` 入口点，除非另有说明。

---

## 目录

1. [核心数据类型](#1-核心数据类型)
2. [通信数据类型（JSON/Talk）操作 API (talk.h)](#2-jsontalk-操作-api-talkh)
   - [2.0 概要](#20-概要)
   - [2.6 示例程序（每个 `talk.h` 函数）](#26-示例程序每个-talkh-函数)
3. [参数结构 API (param.h)](#3-参数结构-api-paramh)
   - [3.0 概要](#30-概要)
   - [3.4 示例程序（每个 `param.h` 函数）](#34-示例程序每个-paramh-函数)
4. [对象和属性路径 API (path.h)](#4-对象和属性路径-api-pathh)
   - [4.0 概要](#40-概要)
   - [4.4 示例程序（每个 `path.h` API）](#44-示例程序每个-pathh-api)
5. [组件通信 API (com.h)](#5-组件通信-api-comh)
   - [5.0 概要](#50-概要)
   - [5.6 Shell 调用上下文 (com.h)](#56-shell-调用上下文-comh)
   - [5.7 示例程序（每个 `com.h` 函数）](#57-示例程序每个-comh-函数)
6. [组件配置管理 API (config.h)](#6-配置管理-api-configh)
   - [6.0 概要](#60-概要)
   - [6.4 示例程序（每个 `config.h` 函数）](#64-示例程序每个-configh-函数)
7. [组件数据库 API (dbs.h)](#7-数据库-api-dbsh)
   - [7.0 概要](#70-概要)
   - [7.3 示例程序（每个 `dbs.h` 函数）](#73-示例程序每个-dbsh-函数)
8. [组件寄存器 API (register.h)](#8-寄存器-api-registerh)
   - [8.0 概要](#80-概要)
   - [8.7 示例程序（每个 `register.h` API）](#87-示例程序每个-registerh-api)
9. [日志 API (log.h)](#9-日志-api-logh)
   - [9.0 概要](#90-概要)
10. [服务管理 API (serv.h)](#10-服务管理-api-servh)
    - [10.0 概要](#100-概要)
    - [10.6 示例程序（每个 `serv.h` 函数）](#106-示例程序每个-servh-函数)
11. [项目信息 API (project.h)](#11-项目信息-api-projecth)
    - [11.0 概要](#110-概要)
    - [11.5 示例程序（每个 `project.h` API）](#115-示例程序每个-projecth-api)
12. [HE 命令 API (he2com.h)](#12-he-命令-api-he2comh)
    - [12.0 概要](#120-概要)
    - [12.4 示例程序（每个 `he2com.h` 函数）](#124-示例程序每个-he2comh-函数)
13. [链表 API (link.h)](#13-链表-api-linkh)
14. [工具函数 API (utility.h)](#14-工具函数-api-utilityh)
15. [Skin API 宏 (skinapi.h)](#15-skin-api-宏-skinapih)
16. [预定义组件常量 (skinhead.h)](#16-预定义组件常量-skinheadh)
17. [完整使用示例](#17-完整使用示例)
18. [编译和使用](#18-编译和使用)
19. [重要说明](#19-重要说明)
20. [相关文档](#20-相关文档)

---

## 1. 核心数据类型

### 1.1 talk_t - 通信数据类型

talk_t 是 SkinOS 中组件间通信的核心数据类型，基于 JSON 实现。

**特殊返回值：**
这些是哨兵 `talk_t` 值（将小整数转换为指针类型）。相同的符号根据 API 是**通用组件调用**还是**服务层** API 可能具有略微不同的含义 — 参见 `talk.h` 中的注释。

| 常量 | 值 | 典型上下文 | 含义 |
|----------|-------|-----------------|--------|
| tnull | NULL | 接口：无负载；服务：OK / 空闲 | 根据 API 系列，"无值" 与 "一切正常" |
| ttrue | (talk_t)2 | 两者 | 成功（布尔是） |
| tfalse | (talk_t)3 | 接口：失败；服务：失败，可能需要重启 | 操作失败 |
| terror | (talk_t)4 | 接口：错误；服务：错误，不需要重启 | 操作错误 |
| tpanic | (talk_t)5 | 两者 | 调用/调度失败（错误的组件、缺少的 API 等） |

在你调用的 API 中始终阅读 **`com.h` / `serv.h` / `scall*`** 文档：不要在没有上下文的情况下假设 `tnull` 总是"错误"或总是"成功"。

---

## 2. 通信数据类型（JSON/Talk）操作 API (talk.h)

### 2.0 概要

`talk.h` 是 **talk/JSON** 模型的公共接口。它包含 **`json.h`**：JSON `talk_t` 就是 `json_t`，值类型（`JSON_STRING`、`JSON_NUMBER`、`JSON_OBJECT`、`JSON_POINTER`、...）来自 `json.h` 中的 `json_value_type` 枚举。

| 层 | 思路 |
|-------|------|
| **X / 标量** | `string2x`、`number2x`、`pointer2x`、`text2x` 和匹配的 `x2*` — 不是 JSON 对象的叶子节点。 |
| **AXP** | 名称/值**对**（`axp_*`）；一个属性字符串 + 一个值（`talk_t`）。 |
| **JSON 对象** | AXP 的有序映射（`json_*`）：附加/分离，迭代 `json_next` / `json_prev`，类型化的 getter/setter，通用 `json_value` / `json_set_value`。 |
| **I/O** | `string2json` / `json2string`、`file2json` / `json2file`、`json_save` / `json_load` — **`json2string` 是堆内存（`free`）**；对于来自 talk API 的 `talk_t` 图节点使用 **`talk_free`**。 |
| **合并** | `json_sync` / `json_patch` 返回 **0/1**；无效图会 **`assert`**（见 §2.3）。 |

在对未知节点使用 `axp_string()` / `x2number()` / ... 之前，使用 **`json_check()`**、**`x_check()`** 或 **`axp_value()`** / **`json_value()`** 确认形状。

---

### 2.1 X 类型操作（基本数据类型）

#### x_check
```c
boole x_check(talk_t v);
```
**描述：** 检查 talk_t 是否为 X 类型（字符串/数字/指针）
**参数：** v - 要检查的值
**返回：** 如果是 X 类型返回 true，否则返回 false

#### string2x / number2x / pointer2x / text2x
```c
talk_t string2x(const char *string);
talk_t number2x(int i);
talk_t pointer2x(void *pointer);
talk_t text2x(const char *string);
```
**描述：** 创建 X 类型值
**参数：**
- string - 字符串值
- i - 整数值
- pointer - 指针值
**返回：** 创建的 X 类型 talk_t

#### x2string / x2number / x2pointer / x2text
```c
const char *x2string(talk_t x);
int x2number(talk_t x);
void *x2pointer(talk_t x);
const char *x2text(talk_t x);
```
**描述：** 从 X 类型中提取值
**返回：** 对应的字符串/整数/指针值

**示例：**
```c
talk_t str_x = string2x("hello");
const char *str = x2string(str_x);  // 获取字符串 "hello"
talk_free(str_x);

talk_t num_x = number2x(42);
int num = x2number(num_x);  // 获取整数 42
talk_free(num_x);
```

### 2.2 AXP 类型操作（属性-值对）

#### axp_check
```c
boole axp_check(talk_t v);
```
**描述：** 检查值是否为 AXP 类型

#### axp_create
```c
talk_t axp_create(const char *attr, const char *string, talk_t v);
```
**描述：** 创建 AXP 对象
**参数：**
- attr - 属性名
- string - 字符串值（为 NULL 时使用 v）
- v - talk_t 值
**返回：** 创建的 AXP 对象

#### axp_set_name / axp_set_string / axp_set_number / axp_set_pointer / axp_set_json
```c
boole axp_set_name(talk_t axp, const char *attr);
boole axp_set_string(talk_t axp, const char *string);
boole axp_set_number(talk_t axp, int i);
boole axp_set_pointer(talk_t axp, void *pointer);
boole axp_set_json(talk_t axp, talk_t json);
```
**描述：** 设置 AXP 的各种属性

#### axp_name / axp_string / axp_number / axp_pointer / axp_json
```c
const char *axp_name(talk_t axp);
const char *axp_string(talk_t axp);
int axp_number(talk_t axp);
void *axp_pointer(talk_t axp);
talk_t axp_json(talk_t axp);
```
**描述：** 获取 AXP 的各种属性

**示例：**
```c
talk_t axp = axp_create("name", "device1", NULL);
const char *name = axp_name(axp);      // 获取属性名 "name"
const char *val = axp_string(axp);     // 获取值 "device1"
talk_free(axp);
```

### 2.3 JSON 对象操作

#### json_check
```c
boole json_check(talk_t json);
```
**描述：** 检查值是否为 JSON 对象

#### json_create
```c
talk_t json_create(talk_t axp);
```
**描述：** 创建 JSON 对象
**参数：** axp - 第一个属性（可以为 NULL 表示空对象）
**返回：** 创建的 JSON 对象

#### json_set_string / json_set_number / json_set_pointer / json_set_json
```c
boole json_set_string(talk_t json, const char *attr, const char *string);
boole json_set_number(talk_t json, const char *attr, int i);
boole json_set_pointer(talk_t json, const char *attr, void *pointer);
boole json_set_json(talk_t json, const char *attr, talk_t v);
```
**描述：** 在 JSON 对象中设置属性值

#### json_string / json_number / json_pointer / json_json
```c
const char *json_string(talk_t json, const char *attr);
int json_number(talk_t json, const char *attr);
void *json_pointer(talk_t json, const char *attr);
talk_t json_json(talk_t json, const char *attr);
```
**描述：** 从 JSON 对象获取属性值

#### json_cut_axp / json_delete_axp
```c
talk_t json_cut_axp(talk_t json, const char *attr);
boole json_delete_axp(talk_t json, const char *attr);
```
**描述：** 从 JSON 中剪切/删除指定属性

#### json_attach_axp / json_detach_axp
```c
boole json_attach_axp(talk_t json, talk_t axp);
boole json_detach_axp(talk_t json, talk_t axp);
```
**描述：** 将 AXP 附加到/从 JSON 分离

#### json_next / json_prev
```c
talk_t json_next(talk_t json, talk_t axp);
talk_t json_prev(talk_t json, talk_t axp);
```
**描述：** 遍历 JSON 对象属性

#### json_empty / json_size
```c
boole json_empty(talk_t json);
int json_size(talk_t json);
```
**描述：** 测试是否为空 / 计算顶级属性数。**如果 `json` 不是有效的 JSON 对象，`json_size` 返回 `-1`**（设置 `errno`）；否则返回 `>= 0`。

#### json_value / json_set_value / json_cut_value
```c
talk_t json_value(talk_t json, const char *attr);
boole json_set_value(talk_t json, const char *attr, talk_t v);
talk_t json_cut_value(talk_t json, const char *attr);
```
**描述：** 以任意 `talk_t` 形式访问属性值（嵌套对象、字符串/数字/指针叶子节点等）。`json_set_value` 将 `v` 链接到对象中；`json_cut_value` 移除值节点但不释放它（调用者拥有它）。当你知道标量类型时，优先使用 `json_string` / `json_number` / ...。

#### axp_value / axp_set_value / axp_cut_value / axp_delete_value
```c
talk_t axp_value(talk_t axp);
boole axp_set_value(talk_t axp, talk_t v);
talk_t axp_cut_value(talk_t axp);
boole axp_delete_value(talk_t axp);
```
**描述：** 在 AXP 级别的相同概念：获取/设置/剪切/删除名称/值对的值部分。`axp_delete_value` 释放分离的值。

#### json_sync / json_patch
```c
int json_sync(talk_t src, talk_t dest);
int json_patch(talk_t src, talk_t dest);
```
**描述：** JSON 对象上的深度合并语义。两者在没有变化时返回 **`0`**，在 `dest` 被修改时返回 **`1`**。无效的 `src`/`dest`（`json_check` 失败）在当前实现中**通过 `assert()` 中止** — 不是负返回码。**`json_patch`** 遵循来自 `src` 的补丁模式：操作符键 `"."` / `'|'` / `'='`（`talk.h` 中的 `JSON_PATCH_OP`、`JSON_PATCH_SYNC`、`JSON_PATCH_MIRROR`）控制合并还是镜像（删除 `src` 中不存在的键）。

**示例：**
```c
// 创建 JSON 对象
talk_t json = json_create(NULL);
json_set_string(json, "name", "router");
json_set_number(json, "port", 8080);

// 获取值
const char *name = json_string(json, "name");  // "router"
int port = json_number(json, "port");          // 8080

// 遍历 JSON（值可能是字符串、数字、嵌套对象等 — 除非值是字符串，否则不要使用 axp_string()）
talk_t axp = NULL;
while ((axp = json_next(json, axp)) != NULL) {
    printf("%s = ", axp_name(axp));
    talk_t val = axp_value(axp);
    if (val == NULL) {
        printf("(null)\n");
    } else {
        talk_print(val);  /* 打印表示形式；参见 talk.h 中的 talk_print */
    }
}

talk_free(json);
```

### 2.4 序列化/反序列化

#### string2json / json2string
```c
talk_t string2json(const char *string);
char *json2string(talk_t json);
```
**描述：** 在字符串和 JSON 之间转换

#### file2json / json2file / json_save / json_load
```c
talk_t file2json(const char *path);
int json2file(talk_t json, const char *path);
boole json_save(talk_t json, const char *pathformat, ...);
talk_t json_load(const char *pathformat, ...);
```
**描述：** 从文件读取/写入 JSON

**示例：**
```c
// 从字符串解析 JSON
talk_t json = string2json("{\"name\":\"test\",\"value\":123}");

// 转换为字符串
char *str = json2string(json);
free(str);

// 保存到文件
json_save(json, "/tmp/config.json");

// 从文件加载
talk_t loaded = json_load("/tmp/config.json");
talk_free(json);
talk_free(loaded);
```

### 2.5 其他操作

#### talk_dup / talk_equal / talk_free / talk_print
```c
talk_t talk_dup(talk_t json);
boole talk_equal(talk_t json, talk_t json2);
void talk_free(talk_t json);
int talk_print(talk_t json);
```
**描述：** 复制/比较/释放/打印 talk 对象

### 2.6 示例程序（每个 `talk.h` 函数）

以下程序是**教学性**的（不是最小生产代码）。它至少调用一次 `talk.h` 中声明的**每个函数**。**`talk.h` 宏**（`JSON_PATCH_OP`、`JSON_STRING_PREFIX`、`ttrue`、...）不是函数；此示例在注释中使用了 **`JSON_PATCH_OP`**，并通过 `json_set_string(..., ".", ...)` 构建补丁模式，与库实现一致。

**构建：** 编译并链接 libskin（与其他示例相同），例如 `#include "skin.h"` 以便解析 `boole` 和 `memory_exit` 路径。

```c
/*
 * skin.md §2.6 — 触及每个 talk.h API 函数一次。
 * 需要：#include "skin.h"（或等效的 boole / 类型）。
 */
#include "skin.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int stackvar = 42;

    /* ---- X / 标量：x_check, string2x, number2x, pointer2x, text2x, x2* ---- */
    talk_t sx = string2x("abc");
    if (x_check(sx))
        (void)x2string(sx);
    talk_free(sx);

    talk_t nx = number2x(100);
    if (x_check(nx))
        (void)x2number(nx);
    talk_free(nx);

    talk_t px = pointer2x(&stackvar);
    if (x_check(px))
        (void)x2pointer(px);
    talk_free(px);

    talk_t tx = text2x("literal");
    if (x_check(tx))
        (void)x2text(tx);
    talk_free(tx);

    (void)json_check((talk_t)tnull); /* false；验证 json_check */

    /* ---- AXP：create / check / set_* / name / string / number / pointer / json ---- */
    talk_t nested_for_axp = json_create(NULL);
    json_set_string(nested_for_axp, "nk", "nv");

    talk_t ax = axp_create("label", "textval", NULL);
    if (axp_check(ax)) {
        (void)axp_name(ax);
        (void)axp_string(ax);
        axp_set_name(ax, "renamed");
        axp_set_string(ax, "s2");
        axp_set_number(ax, 10);
        (void)axp_number(ax);
        axp_set_pointer(ax, &stackvar);
        (void)axp_pointer(ax);
        axp_set_json(ax, nested_for_axp);
        (void)axp_json(ax);
    }
    talk_free(ax);

    talk_t ax2 = axp_create("withval", NULL, number2x(5));
    talk_t vv = axp_value(ax2);
    if (vv != NULL && x_check(vv))
        (void)x2number(vv);
    talk_t cutchild = axp_cut_value(ax2);
    if (cutchild != NULL)
        talk_free(cutchild);
    axp_set_value(ax2, string2x("after"));
    axp_delete_value(ax2);
    talk_free(ax2);

    /* ---- JSON：json_create, json_check, json_set_*, json_*, cut/delete, attach/detach ---- */
    talk_t j = json_create(NULL);
    if (!json_check(j))
        return 1;

    json_set_string(j, "a", "1");
    json_set_number(j, "b", 2);
    json_set_pointer(j, "ptr", &stackvar);
    talk_t inner = json_create(NULL);
    json_set_string(inner, "in", "side");
    json_set_json(j, "inner", inner);

    (void)json_string(j, "a");
    (void)json_number(j, "b");
    (void)json_pointer(j, "ptr");
    (void)json_json(j, "inner");

    talk_t axp_a = json_axp(j, "a");
    if (axp_a != NULL && axp_check(axp_a))
        (void)axp_name(axp_a);

    talk_t cut_axp = json_cut_axp(j, "b");
    if (cut_axp != NULL)
        talk_free(cut_axp);

    json_delete_axp(j, "ptr");

    talk_t loose = axp_create("attached", "yes", NULL);
    json_attach_axp(j, loose);
    json_detach_axp(j, loose);
    json_attach_axp(j, loose);

    talk_t jnav = json_create(NULL);
    json_set_string(jnav, "first", "1");
    json_set_string(jnav, "second", "2");
    talk_t n1 = json_next(jnav, NULL);
    if (n1 != NULL) {
        talk_t n2 = json_next(jnav, n1);
        if (n2 != NULL)
            (void)json_prev(jnav, n2);
    }
    talk_free(jnav);

    (void)json_size(j);
    (void)json_size((talk_t)tnull);

    talk_t jempty = json_create(NULL);
    json_set_string(jempty, "x", "y");
    (void)json_empty(jempty);
    talk_free(jempty);

    json_set_value(j, "leaf", number2x(99));
    talk_t leafcut = json_cut_value(j, "leaf");
    if (leafcut != NULL)
        talk_free(leafcut);

    talk_t subo = json_create(NULL);
    json_set_string(subo, "z", "1");
    json_set_json(j, "sub", subo);
    talk_t jv = json_value(j, "sub");
    if (jv != NULL && json_check(jv))
        (void)json_string(jv, "z");

    /* json_sync / json_patch */
    talk_t src = string2json("{\"s1\":\"x\",\"s2\":2}");
    talk_t dst = string2json("{\"s1\":\"y\"}");
    (void)json_sync(src, dst);
    talk_free(src);
    talk_free(dst);

    talk_t ps = json_create(NULL);
    json_set_string(ps, JSON_PATCH_OP, ".");
    json_set_string(ps, "k", "new");
    talk_t pd = json_create(NULL);
    json_set_string(pd, "k", "old");
    (void)json_patch(ps, pd);
    talk_free(ps);
    talk_free(pd);

    /* ---- 序列化 ---- */
    talk_t sj = string2json("{\"demo\":true}");
    char *jstr = json2string(sj);
    if (jstr != NULL)
        free(jstr);
    if (json2file(sj, "/tmp/skin_talk_demo.json") < 0) { /* 忽略 */ }
    talk_t fj = file2json("/tmp/skin_talk_demo.json");
    if (fj != NULL)
        talk_free(fj);
    if (json_save(sj, "/tmp/skin_talk_demo_saved.json")) { /* 忽略 */ }
    talk_t ld = json_load("/tmp/skin_talk_demo_saved.json");
    if (ld != NULL)
        talk_free(ld);
    talk_free(sj);

    /* ---- talk_dup, talk_equal, talk_free, talk_print ---- */
    talk_t o1 = string2json("{\"q\":1}");
    talk_t o2 = talk_dup(o1);
    if (talk_equal(o1, o2))
        (void)talk_print(o2);
    talk_free(o1);
    talk_free(o2);

    talk_free(j);
    return 0;
}
```

---

## 3. 参数结构 API (param.h)

### 3.0 概要

`param_t` 是一个**有序选项列表**（最多 `PARAM_OPTIONS_MAX` = 10），用于调用组件（`ccall` / `scall` / ...）和驱动服务（`srun`、`sreset`、...）。每个槽位要么是**文本**（`PARAM_OPTION_TEXT`），要么是**原始指针**（`PARAM_OPTION_POINTER`）；参见 `param.h`。构建它的典型方式：

| 来源 | 作用 |
|--------|------|
| `param_create("a,b,c")` | 解析逗号分隔的描述；嵌入的 JSON 对象允许作为选项（例如 `eth0,{"mask":"24"},up`）。 |
| `param_build(json)` | 从属性命名为 `"1"` ... `"10"` 的 JSON 对象填充槽位。 |
| `param_found` + `param_insert*` / `param_add*` | 从指针开始或增长列表；**指针选项不会被复制，也不会被 `param_free()` 释放**。 |

**索引：** `param_string` / `param_talk` / `param_pointer` 使用**基于 1 的**序号；**`-1`** 选择**最后一个**选项。**`param_insert*`** 前置；**`param_add*`** 追加。**`param_shift` / `param_unshift`** 移动可见窗口（`start` / `end`）。**`param_import` / `param_import2`** 将一个或两个 JSON 对象合并到现有 `param_t` 中。**`param_combine`** 返回单个可读字符串形式（内部缓冲区；在 `param_free()` 之前有效）。**`param_free()`** 释放结构和任何拥有的 `talk_t` / 内部字符串；之后不要使用该 `param_t`。

**解析规则（`param_create`）：** 逗号仅在**未**处于成对的 `{` `}`、`[` `]` 内，且**不在**未成对引号 `"` 分隔的片段中时分段。括号/引号不平衡 → **`NULL`** 且 **`EINVAL`**。最多 **`PARAM_OPTIONS_MAX`** 个逗号字段；超出部分**静默丢弃**，不报错误。

---

### 3.1 创建和释放

#### param_create
```c
param_t param_create(const char *a);
```
**描述：** 从字符串创建参数结构
**参数：** a - 选项描述字符串，例如 `"opt1,opt2,opt3"`；**`NULL`** 表示**空**列表（零个选项）。
**返回：** 成功时为非 **`NULL`** 的 **`param_t`**；若去掉首尾空白后 **`a`** 为空 → **`NULL`**、**`EINVAL`**。解析失败（**`{}` / `[]` / 引号**不平衡）→ **`NULL`**、**`EINVAL`**。分配失败会调用 **`memory_exit`**。

#### param_build
```c
param_t param_build(talk_t json);
```
**描述：** 从 JSON 创建参数结构

#### param_found
```c
param_t param_found(void *pointer, void *pointer2, void *pointer3, void *pointer4);
```
**描述：** 从指针创建参数结构

#### param_free
```c
void param_free(param_t parameter);
```
**描述：** 释放参数结构

### 3.2 参数操作

#### param_inserts / param_insertt / param_insertp
```c
param_t param_inserts(param_t param, const char *option);
param_t param_insertt(param_t param, const talk_t option);
param_t param_insertp(param_t param, void *option);
```
**描述：** 在参数头部插入选项

#### param_adds / param_addt / param_addp
```c
param_t param_adds(param_t param, const char *option);
param_t param_addt(param_t param, const talk_t option);
param_t param_addp(param_t param, void *option);
```
**描述：** 在参数末尾添加选项

#### param_shift / param_unshift
```c
boole param_shift(param_t parameter, int number);
boole param_unshift(param_t parameter, int number);
```
**描述：** 调整参数起始位置

#### param_import / param_import2
```c
param_t param_import(param_t parameter, talk_t json);
param_t param_import2(param_t parameter, talk_t json, talk_t json2);
```
**描述：** 将 JSON 导入参数

### 3.3 获取参数

#### param_size
```c
int param_size(param_t parameter);
```
**描述：** 获取参数数量

#### param_string / param_talk / param_pointer
```c
const char *param_string(param_t parameter, int serial);
talk_t param_talk(param_t parameter, int serial);
void *param_pointer(param_t parameter, int serial);
```
**描述：** 获取指定序号的参数值（从 1 开始，-1 表示最后一个）

#### param_combine
```c
const char *param_combine(param_t parameter);
```
**描述：** 获取参数的字符串表示

**示例：**
```c
// 创建参数
param_t p = param_create("eth0,192.168.1.1,24");

// 获取参数
const char *ifname = param_string(p, 1);  // "eth0"
const char *ip = param_string(p, 2);      // "192.168.1.1"

// 添加参数
p = param_adds(p, "gateway");

param_free(p);
```

### 3.4 示例程序（每个 `param.h` 函数）

以下每个 **`param_*` 函数**至少出现一次。**`param_import` / `param_import2` 接管传入的 `talk_t` JSON 的所有权**（之后不要 `talk_free` 它们）。来自 **`param_found` / `param_insertp` / `param_addp`** 的指针选项不会被 **`param_free()`** 释放。

```c
#include "skin.h"

static void demo_param_all(void)
{
    int stack = 42;
    talk_t jbuild = string2json("{\"1\":\"opt1\",\"2\":\"opt2\"}");
    talk_t jimp = string2json("{\"z\":1}");
    talk_t j2a = string2json("{\"a\":1}");
    talk_t j2b = string2json("{\"b\":2}");
    talk_t jt = string2x("embedded");
    param_t p;

    p = param_create("a,b,c");
    param_free(p);

    p = param_build(jbuild);
    talk_free(jbuild);
    (void)param_size(p);
    (void)param_string(p, 1);
    (void)param_string(p, -1);
    (void)param_talk(p, 1);
    (void)param_combine(p);
    param_free(p);

    p = param_found((void *)&stack, NULL, NULL, NULL);
    p = param_inserts(p, "head");
    p = param_insertt(p, jt);
    talk_free(jt);
    p = param_insertp(p, (void *)0x1234);
    p = param_adds(p, "tail");
    p = param_addt(p, string2json("{\"k\":1}"));
    p = param_addp(p, (void *)0x5678);
    (void)param_shift(p, 1);
    (void)param_unshift(p, 1);
    p = param_import(p, jimp);
    p = param_import2(p, j2a, j2b);
    (void)param_pointer(p, 1);
    (void)param_combine(p);
    param_free(p);
}
```

---

## 4. 对象和属性路径 API (path.h)

### 4.0 概要

`path.h` 定义了**组件**和**配置路径**如何表示和导航。

| 类型 | 作用 |
|------|------|
| **`obj_t`** | 解析后的**组件路径**。解析器最多保存 **`OBJ_MAX_LEVEL`** = **2** 段（`project@component`）。**单个**已注册别名经 **`com_path()`** 解析并规范为两层。保存项目/组件字符串、可选的 `com` / 寄存器句柄和缓冲区；**`obj_create` / `obj_free`**。 |
| **`attr_t`** | 配置或 JSON 内的解析后**属性路径**（例如 `wan/ip`，多级使用 `OBJECT_CONFIG_GAPS` / `/`）。**`attr_create` / `attr_free`**、`attr_layer`、`attr_combine`。 |
| **JSON 辅助函数** | **`attr_get` / `attr_set` / `attr_cut`**（和 `*s` / `*_string` 变体）通过属性路径遍历 **`talk_t` JSON 树** — 与 `config_get` 路径相同的逻辑布局。 |

从 `obj_*` / `attr_*` 返回的 `const char *` 视图指向内部缓冲区；在对应的 **`obj_free` / `attr_free`** 之后它们无效。

---

### 4.1 对象操作 (obj_t)

#### obj_create
```c
obj_t obj_create(const char *string);
```
**描述：** 从字符串创建对象
**参数：** string - 一般为 `project@component`，或由 **`com_register`** 映射的**单个**组件别名（会解析为真实项目 + 组件）。
**返回：** 对象指针；无效输入或解析失败时为 **`NULL`**（设置 **`errno`**）。
**说明：** 成功 **`obj_create`** 后，**`obj_level()`** 通常为 **2**，**`obj_prj()`** / **`obj_com()`** 一般均非 **`NULL`**。

#### obj_free
```c
void obj_free(obj_t object);
```
**描述：** 释放对象

#### obj_prj / obj_com / obj_level / obj_layer / obj_name
```c
const char *obj_prj(obj_t object);
const char *obj_com(obj_t object);
int obj_level(obj_t object);
const char *obj_layer(obj_t object, int level);
const char *obj_name(obj_t object);
```
**描述：** 获取项目名、组件名、级别和其他信息

**示例：**
```c
obj_t o = obj_create("land@machine");
const char *prj = obj_prj(o);    // "land"
const char *com = obj_com(o);    // "machine"
obj_free(o);
```

### 4.2 属性操作 (attr_t)

#### attr_create
```c
attr_t attr_create(const char *attribute);
```
**描述：** 创建属性路径
**参数：** attribute - 属性路径，例如 "wan/ip" 或 "config/gateway"
**返回：** 属性指针

#### attr_free
```c
void attr_free(attr_t attribute);
```
**描述：** 释放属性

#### attr_level / attr_layer / attr_combine
```c
int attr_level(attr_t attribute);
const char *attr_layer(attr_t attribute, int level);
const char *attr_combine(attr_t attribute);
```
**描述：** 获取属性级别信息

### 4.3 属性访问

#### attr_get / attr_gets / attr_get_string / attr_gets_string
```c
talk_t attr_get(talk_t json, attr_t attribute);
talk_t attr_gets(talk_t json, const char *attribute, ...);
const char *attr_get_string(char *buffer, int buflen, talk_t json, attr_t attribute);
const char *attr_gets_string(char *buffer, int buflen, talk_t json, const char *attribute, ...);
```
**描述：** 根据属性路径从 JSON 获取值

#### attr_cut / attr_cuts
```c
talk_t attr_cut(talk_t json, attr_t attribute);
talk_t attr_cuts(talk_t json, const char *attribute, ...);
```
**描述：** 在指定路径处从 JSON 剪切值

#### attr_set / attr_sets / attr_set_string / attr_sets_string
```c
boole attr_set(talk_t json, talk_t v, attr_t attribute);
boole attr_sets(talk_t json, talk_t v, const char *attribute, ...);
boole attr_set_string(talk_t json, const char *v, attr_t attribute);
boole attr_sets_string(talk_t json, const char *v, const char *attribute, ...);
```
**描述：** 根据属性路径设置 JSON 值

**示例：**
```c
attr_t a = attr_create("network/wan/ip");
talk_t json = json_create(NULL);
attr_set_string(json, "192.168.1.1", a);
const char *ip = attr_get_string(buf, sizeof(buf), json, a);
attr_free(a);
talk_free(json);
```

### 4.4 示例程序（每个 `path.h` API）

涵盖 **`obj_*`**、**`attr_*`** 和 JSON 辅助函数 **`attr_get*` / `attr_set*` / `attr_cut*`**（包括 `*s` / `*_string` 形式）。

```c
#include "skin.h"

static void demo_path_all(void)
{
    char buf[256];
    obj_t o = obj_create("land@machine");
    (void)obj_prj(o);
    (void)obj_com(o);
    (void)obj_level(o);
    (void)obj_layer(o, 0);
    (void)obj_name(o);

    attr_t a = attr_create("wan/ip");
    (void)attr_level(a);
    (void)attr_layer(a, 0);
    (void)attr_combine(a);

    talk_t j = json_create(NULL);
    (void)attr_set_string(j, "192.168.1.1", a);
    (void)attr_get_string(buf, sizeof buf, j, a);
    (void)attr_get(j, a);
    (void)attr_gets(j, "%s", "wan/ip");
    (void)attr_gets_string(buf, sizeof buf, j, "%s", "wan/ip");
    (void)attr_set(j, string2x("v"), a);
    (void)attr_sets(j, string2x("v2"), "%s", "wan/ip");
    (void)attr_sets_string(j, "text", "%s", "wan/ip");

    talk_t j2 = string2json("{\"a\":{\"b\":\"c\"}}");
    attr_t ab = attr_create("a/b");
    talk_t cut = attr_cut(j2, ab);
    if (cut > (void *)tpanic && cut && json_check(cut))
        talk_free(cut);
    talk_t j3 = string2json("{\"x\":{\"y\":1}}");
    talk_t cx = attr_cuts(j3, "%s", "x");
    if (cx > (void *)tpanic && cx && json_check(cx))
        talk_free(cx);

    attr_free(ab);
    attr_free(a);
    talk_free(j3);
    talk_free(j2);
    talk_free(j);
    obj_free(o);
}
```

---

## 5. 组件通信 API (com.h)

### 5.0 概要

`com.h` 是**动态组件加载器和 RPC 层**：打开组件模块（共享库 **`.com` 等** 与 **`COM_FILE_EXECUTE`** 可执行辅助进程），解析 **`_api`** 符号，并使用 **`param_t`** 或原始 **`talk_t`** 调用它们。

| 主题 | 详情 |
|-------|---------|
| **类型** | `com_t` 句柄；**`COM_FILE_*`** 文件种类；可选的 **`comget_t` / `comset_t` / `comfetch_t` / `comsave_t`** 标准钩子的 typedef。 |
| **发现** | `com_list`、`com_project_list`、`com_register` 别名、**`com_path`** -> 缓冲区 + 返回 **`char`** 组件类型代码。 |
| **打开 / 符号** | `com_open` / `com_sopen`，**`com_symbol`**（名称使用 **`COM_API_PREFIX`**，例如 `_status`）。 |
| **调用** | **`ccall` / `scall`** + 变体：`*t`（JSON）、`*s`（格式字符串）、`*st`、`*4p`、`*_string`（结果写入用户缓冲区）。结果是 **`void *`**：通常是 **`talk_t` JSON**（调用者 **`talk_free`**）或哨兵 **`ttrue` / `tfalse` / `terror` / `tpanic` / `tnull`** — 见 §1.1。 |
| **Shell 子进程** | 在 **`COM_FILE_EXECUTE`** 由 shell RPC 路径**派生子进程**运行时，使用 **`execute_object` / `execute_param` / `execute_api` / `execute_pipe`**（§5.6）。父进程设置环境变量 **`OBJECT`**、**`PARAM_SIZE`**、**`PARAM1..N`**、**`API`**、**`cpipe`**。 |

---

### 5.1 组件文件类型（`com_t`）

与当前 `com.h` 中 `com_t.type` 字段一致，仅定义以下两项：

```c
#define COM_FILE_LIB      1    /* 共享库（.com 等） */
#define COM_FILE_EXECUTE  2    /* 可执行辅助进程；应答管道使用固定 fd SHELL_COM_PIPE (7) */
```

### 5.2 组件发现与注册

#### com_project_list
```c
talk_t com_project_list(void);
```
**描述：** 获取工程组件列表
**返回：** 键为 `"project@component"`、值为路径的 JSON 对象

#### com_list
```c
talk_t com_list(const char *project);
```
**描述：** 获取组件列表
**参数：** project - 工程名（**`NULL`** 表示全部）
**返回：** JSON 对象

#### com_register / com_unregister
```c
boole com_register(const char *target, const char *origin);
boole com_unregister(const char *target);
```
**描述：** 注册/注销组件别名

#### com_register_list
```c
talk_t com_register_list(void);
```
**描述：** 获取已注册别名列表

#### com_path
```c
char com_path(obj_t obj, char *buffer, int buflen);
```
**描述：** 获取组件文件路径
**返回：** 组件类型（**`COM_FILE_*`**）或失败时为 **0**

**示例：**
```c
talk_t list = com_list(NULL);
talk_free(list);

com_register("mywan", "network@wan");

obj_t o = obj_create("land@machine");
char path[PATH_MAX];
char type = com_path(o, path, sizeof(path));  /* COM_FILE_* 或失败为 0 */
obj_free(o);
```

### 5.3 组件打开与关闭

#### com_open / com_sopen
```c
com_t com_open(obj_t obj);
com_t com_sopen(const char *com);
```
**描述：** 打开组件
**返回：** 组件句柄

#### com_symbol
```c
void *com_symbol(com_t com, const char *name);
```
**描述：** 从组件获取符号地址

#### com_close
```c
void com_close(com_t com);
```
**描述：** 关闭组件

#### com_exist / com_sexist
```c
boole com_exist(obj_t obj, const char *api);
boole com_sexist(const char *com, const char *api);
```
**描述：** 判断组件/API 是否存在

**示例：**
```c
com_t c = com_sopen("land@machine");
if (c) {
    void *fn = com_symbol(c, "_status");
    com_close(c);
}
if (com_sexist("land@machine", "status")) {
    /* API 存在 */
}
```

### 5.4 组件调用

#### ccall / scall
```c
void *ccall(obj_t com, const char *api, param_t parameter);
void *scall(const char *com, const char *api, param_t parameter);
```
**描述：** 调用组件 API
**参数：**
- com - 组件对象或字符串
- api - API 名
- parameter - 参数（可为 **`NULL`**，视具体 API 而定）
**返回：**
- JSON 对象 - 成功并带数据
- ttrue - 操作成功
- tfalse - 操作失败
- terror - 操作错误
- tpanic - 调用错误

#### ccallt / scallt / ccall2t / scall2t
```c
void *ccallt(obj_t com, const char *api, talk_t json);
void *scallt(const char *com, const char *api, talk_t json);
void *ccall2t(obj_t com, const char *api, talk_t json, talk_t json2);
void *scall2t(const char *com, const char *api, talk_t json, talk_t json2);
```
**描述：** 以 JSON 为参数调用组件 API

#### ccallst / scallst
```c
void *ccallst(obj_t com, const char *api, const char *json, talk_t json2);
void *scallst(const char *com, const char *api, const char *json, talk_t json2);
```
**描述：** 字符串 + JSON 双参数调用

#### ccall4p / scall4p
```c
void *ccall4p(obj_t obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4);
void *scall4p(const char *obj, const char *api, void *pointer, void *pointer2, void *pointer3, void *pointer4);
```
**描述：** 以四个指针为参数调用

#### ccalls / scalls / ccall2s / scall2s
```c
void *ccalls(obj_t com, const char *api, const char *paramformat, ...);
void *scalls(const char *com, const char *api, const char *paramformat, ...);
void *ccall2s(obj_t com, const char *api, const char *option, const char *option2);
void *scall2s(const char *com, const char *api, const char *option, const char *option2);
```
**描述：** 使用格式化字符串构造参数调用

**示例：**
```c
param_t p = param_create("eth0");
void *r = scall("network@frame", "info", p);
if (r > (void *)tpanic && json_check((talk_t)r)) {
    talk_free((talk_t)r);
}
param_free(p);

void *r2 = scalls("network@frame", "set", "%s,%s", "ip", "192.168.1.1");

talk_t json = string2json("{\"enable\":1}");
void *r3 = scallt("land@machine", "config", json);
talk_free(json);
```

### 5.5 字符串结果调用

#### ccall_string / scall_string / ccallt_string / scallt_string / ccalls_string / scalls_string
```c
const char *ccall_string(char *buffer, int buflen, obj_t com, const char *api, param_t parameter);
const char *scall_string(char *buffer, int buflen, const char *com, const char *api, param_t parameter);
const char *ccallt_string(char *buffer, int buflen, obj_t com, const char *api, talk_t json);
const char *scallt_string(char *buffer, int buflen, const char *com, const char *api, talk_t json);
const char *ccalls_string(char *buffer, int buflen, obj_t com, const char *api, const char *paramformat, ...);
const char *scalls_string(char *buffer, int buflen, const char *com, const char *api, const char *paramformat, ...);
```
**描述：** 调用组件 API 并将结果写入调用者缓冲区

**示例：**
```c
char buf[256];
param_t p = param_create("");
const char *ver = scall_string(buf, sizeof(buf), "land@machine", "version", p);
param_free(p);
```

### 5.6 Shell 派生子进程上下文（`com.h`）

当 **`COM_FILE_EXECUTE`** 组件作为 shell RPC 路径的**子进程**运行时，从环境变量读取上下文（由加载器在派生子进程时通过 **`execute_ccall`** 设置）：

#### execute_object / execute_param / execute_api / execute_pipe
```c
obj_t       execute_object(void);
param_t     execute_param(void);
const char *execute_api(void);
int         execute_pipe(void);
```
**描述：** **`execute_object`** 根据 **`OBJECT`** 构建 **`obj_t`**。**`execute_param`** 根据 **`PARAM_SIZE`** 与 **`PARAM1`…`PARAMn`** 构建 **`param_t`**（经 **`param_adds`**；缺失的环境变量对应位置为 **`NULL` 字符串槽**）。仅在**分配失败**时返回 **`NULL`** — 无参数调用在 **`PARAM_SIZE`** 缺失或为 0 时仍得到**非 NULL**、选项数为 0 的 **`param_t`**。**`execute_api`** 等价于 **`getenv("API")`**。**`execute_pipe`** 为 **`atoi(cpipe)`**，未设置时为 **`-1`**，即应向 **`talk2fd`** 写入 JSON 应答的描述符（子进程 **`dup2`** 后为 **`SHELL_COM_PIPE`** = 7）。

**脱离**该子进程上下文时，**`execute_object` / `execute_api`** 通常因环境未设置而得到 **`NULL`**；**`execute_pipe`** 为 **`-1`**。

### 5.7 示例程序（覆盖 `com.h` 各入口）

下面每个 **`com.h` 入口**各调用一次。需要**真实组件与运行中的 SkinOS** 才有意义结果；离线编译可能得到 **`tpanic` / `NULL` / `false`**，本段旨在展示**名称、参数形态及对堆上 JSON 返回的典型 `talk_free` 处理**。

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"

static void demo_com_all(void)
{
    char cbuf[1024];
    obj_t o = obj_create("land@machine");
    attr_t ak = attr_create("walk/key");
    attr_t fad = attr_create("/var/tmp/skin_walkthrough.db");
    param_t pm = param_create("a");
    talk_t j1 = string2json("{\"a\":1}");
    talk_t j2 = string2json("{\"b\":2}");
    int x = 1, y = 2, z = 3, w = 4;
    void *rv;
    com_t ch;
    talk_t tl;

    (void)execute_object();
    (void)execute_param();
    (void)execute_api();
    (void)execute_pipe();

    tl = com_project_list();
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    (void)com_register("demo_alias@com", "land@machine");
    (void)com_unregister("demo_alias@com");
    tl = com_register_list();
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    (void)com_path(o, cbuf, sizeof cbuf);
    tl = com_list(NULL);
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);

    ch = com_open(o);
    if (ch) {
        (void)com_symbol(ch, "_api");
        com_close(ch);
    }
    ch = com_sopen("land@machine");
    if (ch) com_close(ch);

    (void)com_exist(o, "status");
    (void)com_sexist("land@machine", "status");

    rv = ccall(o, "status", pm);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall("land@machine", "status", pm);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccallt(o, "status", j1);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scallt("land@machine", "status", j1);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccallst(o, "status", "{}", j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scallst("land@machine", "status", "{}", j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall2t(o, "status", j1, j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall2t("land@machine", "status", j1, j2);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall4p(o, "status", &x, &y, &z, &w);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall4p("land@machine", "status", &x, &y, &z, &w);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccalls(o, "status", "%s,%s", "a", "b");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scalls("land@machine", "status", "%s,%s", "a", "b");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    rv = ccall2s(o, "status", "o1", "o2");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = scall2s("land@machine", "status", "o1", "o2");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)ccall_string(cbuf, sizeof cbuf, o, "status", pm);
    (void)scall_string(cbuf, sizeof cbuf, "land@machine", "status", pm);
    (void)ccallt_string(cbuf, sizeof cbuf, o, "status", j1);
    (void)scallt_string(cbuf, sizeof cbuf, "land@machine", "status", j1);
    (void)ccalls_string(cbuf, sizeof cbuf, o, "status", "%s", "a");
    (void)scalls_string(cbuf, sizeof cbuf, "land@machine", "status", "%s", "a");

    (void)cset(o, j1, ak);
    (void)csets(o, j1, "%s", "walk/key");
    (void)sset("land@machine", j1, ak);
    (void)ssets("land@machine", j1, "%s", "walk/key");
    (void)cset_string(o, "s", ak);
    (void)csets_string(o, "s", "%s", "walk/key");
    (void)sset_string("land@machine", "s", ak);
    (void)ssets_string("land@machine", "s", "%s", "walk/key");

    rv = cget(o, ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = cgets(o, "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sget("land@machine", ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sgets("land@machine", "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)cget_string(cbuf, sizeof cbuf, o, ak);
    (void)cgets_string(cbuf, sizeof cbuf, o, "%s", "walk/key");
    (void)sget_string(cbuf, sizeof cbuf, "land@machine", ak);
    (void)sgets_string(cbuf, sizeof cbuf, "land@machine", "%s", "walk/key");

    (void)csave(o, fad, j1, ak);
    (void)csaves(o, fad, j1, "%s", "walk/key");
    (void)ssave("land@machine", "/var/tmp/skin_walkthrough.db", j1, ak);
    (void)ssaves("land@machine", "/var/tmp/skin_walkthrough.db", j1, "%s", "walk/key");
    (void)csave_string(o, fad, "sv", ak);
    (void)csaves_string(o, fad, "sv", "%s", "walk/key");
    (void)ssave_string("land@machine", "/var/tmp/skin_walkthrough.db", "sv", ak);
    (void)ssaves_string("land@machine", "/var/tmp/skin_walkthrough.db", "sv", "%s", "walk/key");

    rv = cfetch(o, fad, ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = cfetchs(o, fad, "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sfetch("land@machine", "/var/tmp/skin_walkthrough.db", ak);
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);
    rv = sfetchs("land@machine", "/var/tmp/skin_walkthrough.db", "%s", "walk/key");
    if (rv > (void *)tpanic && rv && json_check((talk_t)rv)) talk_free((talk_t)rv);

    (void)cfetch_string(cbuf, sizeof cbuf, o, fad, ak);
    (void)cfetchs_string(cbuf, sizeof cbuf, o, fad, "%s", "walk/key");
    (void)sfetch_string(cbuf, sizeof cbuf, "land@machine", "/var/tmp/skin_walkthrough.db", ak);
    (void)sfetchs_string(cbuf, sizeof cbuf, "land@machine", "/var/tmp/skin_walkthrough.db", "%s", "walk/key");

    talk_free(j1);
    talk_free(j2);
    param_free(pm);
    attr_free(fad);
    attr_free(ak);
    obj_free(o);
}
```

---

## 6. 配置管理 API (config.h)

### 6.0 概要

`config.h` 读取和写入组件的**运行时配置**（**不是** `dbs.h` 的持久化路径）。

| 模式 | 含义 |
|---------|---------|
| **`config_get` / `config_set`** | `obj_t` + **`attr_t`** 属性路径。 |
| **`*gets` / `*sets` / `*sget` / `*sset`...** | `printf` 风格的属性路径或使用 **`const char *com`** 代替 `obj_t`。 |
| **`*_string`** | 通过调用者**缓冲区** / C 字符串读/写，不拥有新的 `talk_t`。 |
| **返回** | **`config_get*`** 通常返回堆 **`talk_t`** -> **`talk_free`**；根据注释，调度错误返回 **`tpanic`**。**`config_set`** 复制 **`v`**；除非文档另有说明，调用者仍然拥有原始 `v`。 |
| **列表 / 文件** | **`config_list(project)`** — `NULL` project = 整个系统；**`config_path`** 在项目下构建路径。 |

当你需要**按组件的持久数据库文件**（`fa` + key）而不仅仅是实时配置时，使用 **`dbs_*`**。

---

### 6.1 配置获取

#### config_get / config_gets / config_sget / config_sgets
```c
talk_t config_get(obj_t com, attr_t attr);
talk_t config_gets(obj_t com, const char *attr, ...);
talk_t config_sget(const char *com, attr_t attr);
talk_t config_sgets(const char *com, const char *attr, ...);
```
**描述：** 获取配置值
**返回：** talk_t 值或特殊返回值

#### config_get_string / config_gets_string / config_sgets_string
```c
const char *config_get_string(char *buffer, int buflen, obj_t com, attr_t attr);
const char *config_gets_string(char *buffer, int buflen, obj_t com, const char *attr, ...);
const char *config_sgets_string(char *buffer, int buflen, const char *com, const char *attr, ...);
```
**描述：** 以字符串形式获取配置值

### 6.2 配置设置

#### config_set / config_sets / config_sset / config_ssets
```c
boole config_set(obj_t com, talk_t v, attr_t attr);
boole config_sets(obj_t com, talk_t v, const char *attr, ...);
boole config_sset(const char *com, talk_t v, attr_t attr);
boole config_ssets(const char *com, talk_t v, const char *attr, ...);
```
**描述：** 设置配置值

#### config_set_string / config_sset_string / config_ssets_string
```c
boole config_set_string(obj_t com, const char *string, attr_t attr);
boole config_sset_string(const char *com, const char *string, attr_t attr);
boole config_ssets_string(const char *com, const char *string, const char *attr, ...);
```
**描述：** 以字符串形式设置配置值

### 6.3 配置列表和路径

#### config_list
```c
talk_t config_list(const char *project);
```
**描述：** 获取配置列表
**参数：** project - 项目名（NULL 表示整个系统）
**返回：** JSON 格式的配置列表

#### config_path
```c
const char *config_path(char *buffer, int buflen, const char *project, const char *filename, ...);
```
**描述：** 获取配置文件路径

### 6.4 示例程序（每个 `config.h` 函数）

```c
#include "skin.h"

static void demo_config_all(void)
{
    char buf[512];
    obj_t o = obj_create("land@machine");
    attr_t a = attr_create("walk/key");
    talk_t v = string2x("v");
    talk_t g;

    g = config_get(o, a);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_gets(o, "%s", "walk/key");
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_sget("land@machine", a);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    g = config_sgets("land@machine", "%s", "walk/key");
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);

    (void)config_get_string(buf, sizeof buf, o, a);
    (void)config_gets_string(buf, sizeof buf, o, "%s", "walk/key");
    (void)config_sgets_string(buf, sizeof buf, "land@machine", "%s", "walk/key");

    (void)config_set(o, v, a);
    (void)config_sets(o, v, "%s", "walk/key");
    (void)config_sset("land@machine", v, a);
    (void)config_ssets("land@machine", v, "%s", "walk/key");
    (void)config_set_string(o, "s", a);
    (void)config_sset_string("land@machine", "s", a);
    (void)config_ssets_string("land@machine", "s", "%s", "walk/key");

    g = config_list(NULL);
    if (g > (void *)tpanic && g && json_check(g)) talk_free(g);
    (void)config_path(buf, sizeof buf, "land", "demo.cfg");

    talk_free(v);
    attr_free(a);
    obj_free(o);
}
```

---

## 7. 数据库 API (dbs.h)

### 7.0 概要

`dbs.h` 处理按组件和**数据库文件**（命名空间路径）作用域的**持久键/值存储**。

| 概念 | 作用 |
|---------|------|
| **`fa`（文件属性）** | 哪个逻辑数据库文件/表空间（例如类似路径的字符串）。 |
| **`attr`** | 该文件**内部**的键路径（与 `attr_t` 路径相同的样式）。 |
| **`*fetch` / `*save`** | 读/写 **`talk_t`** 值；**`*_string`** 变体使用调用者缓冲区。 |
| **`dbs_table`** | 使用 **`param_t`** 调用命名的**表 API**（`list`、`query`、...）。 |
| **`dbs_path`** | 解析组件 + `fa` 的文件系统路径。 |

命名镜像 **`config_*`**：**`dbs_fetch` + obj** 与 **`dbs_sfetch` + 字符串 com**，`*s` 表示可变参数路径段。这是**持久存储**；**`config_*`** 面向**当前配置**语义。

**整文件与键路径：** 在 **`dbs_fetch`**（以及保存路径的同类逻辑）中，若 **`attr` 为 `NULL`** 或 **`attr_level(attr) <= 0`**，则按**整个** JSON 文件读写（**`file2json`** / 整文件写入）。否则在解析后的文件 JSON 内按 **`attr`** 层级导航。

---

### 7.1 数据获取

#### dbs_fetch / dbs_sfetch / dbs_fetchs / dbs_sfetchs
```c
talk_t dbs_fetch(obj_t com, attr_t fa, attr_t attr);
talk_t dbs_sfetch(const char *com, const char *fa, attr_t attr);
talk_t dbs_fetchs(obj_t com, attr_t fa, const char *attr, ...);
talk_t dbs_sfetchs(const char *com, const char *fa, const char *attr, ...);
```
**描述：** 从数据库获取数据
**参数：**
- fa - 逻辑数据库文件/命名空间（与组件名组合后在 **`PROJECT_DBS_DIR`** 下形成磁盘路径）。
- attr - 文件**内部**键路径；**`NULL`** 或层级 ≤0 的 **`attr_t`** 表示**整文件** JSON（见 §7.0）。

#### dbs_fetch_string / dbs_fetchs_string / dbs_sfetchs_string
```c
const char *dbs_fetch_string(char *buffer, int buflen, obj_t com, attr_t fa, attr_t attr);
const char *dbs_fetchs_string(char *buffer, int buflen, obj_t com, attr_t fa, const char *attr, ...);
const char *dbs_sfetchs_string(char *buffer, int buflen, const char *com, const char *fa, const char *attr, ...);
```
**描述：** 以字符串形式获取数据库数据

### 7.2 数据保存

#### dbs_save / dbs_ssave / dbs_saves / dbs_ssaves
```c
boole dbs_save(obj_t com, attr_t fa, talk_t value, attr_t attr);
boole dbs_ssave(const char *com, const char *fa, talk_t value, attr_t attr);
boole dbs_saves(obj_t com, attr_t fa, talk_t v, const char *attr, ...);
boole dbs_ssaves(const char *com, const char *fa, talk_t value, const char *attr, ...);
```
**描述：** 保存数据到数据库

#### dbs_save_string / dbs_ssave_string / dbs_ssaves_string
```c
boole dbs_save_string(obj_t com, attr_t fa, const char *value, attr_t attr);
boole dbs_ssave_string(const char *com, const char *fa, const char *value, attr_t attr);
boole dbs_ssaves_string(const char *com, const char *fa, const char *value, const char *attr, ...);
```
**描述：** 以字符串形式保存数据到数据库

**示例：**
```c
// 保存数据
talk_t data = string2json("{\"ip\":\"1.1.1.1\"}");
dbs_ssave("network@frame", "/mnt/dbs/net.db", data, attr_create("wan"));
talk_free(data);

// 获取数据
talk_t loaded = (talk_t)dbs_sfetch("network@frame", "/mnt/dbs/net.db", attr_create("wan"));
if (loaded > (void *)tpanic && json_check(loaded)) {
    talk_free(loaded);
}
```

### 7.3 示例程序（每个 `dbs.h` 函数）

```c
#include "skin.h"

static void demo_dbs_all(void)
{
    char buf[512];
    obj_t o = obj_create("land@machine");
    attr_t fa = attr_create("/var/tmp/skin_dbs_walk.db");
    attr_t key = attr_create("row/k");
    talk_t val = string2json("{\"n\":1}");
    talk_t t;

    t = dbs_fetch(o, fa, key);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_fetchs(o, fa, "%s", "row/k");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_sfetch("land@machine", "/var/tmp/skin_dbs_walk.db", key);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = dbs_sfetchs("land@machine", "/var/tmp/skin_dbs_walk.db", "%s", "row/k");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);

    (void)dbs_fetch_string(buf, sizeof buf, o, fa, key);
    (void)dbs_fetchs_string(buf, sizeof buf, o, fa, "%s", "row/k");
    (void)dbs_sfetchs_string(buf, sizeof buf, "land@machine", "/var/tmp/skin_dbs_walk.db", "%s", "row/k");

    (void)dbs_save(o, fa, val, key);
    (void)dbs_saves(o, fa, val, "%s", "row/k");
    (void)dbs_ssave("land@machine", "/var/tmp/skin_dbs_walk.db", val, key);
    (void)dbs_ssaves("land@machine", "/var/tmp/skin_dbs_walk.db", val, "%s", "row/k");
    (void)dbs_save_string(o, fa, "x", key);
    (void)dbs_ssave_string("land@machine", "/var/tmp/skin_dbs_walk.db", "x", key);
    (void)dbs_ssaves_string("land@machine", "/var/tmp/skin_dbs_walk.db", "x", "%s", "row/k");

    t = dbs_table(o, fa, "list", NULL);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    (void)dbs_path(buf, sizeof buf, o, fa);

    talk_free(val);
    attr_free(key);
    attr_free(fa);
    obj_free(o);
}
```

---

## 8. 寄存器 API (register.h)

### 8.0 概要

`register.h` 暴露了一个**基于 mmap 的文件键/值存储**，按对象名（例如 **`land@machine`**）共享：二进制安全，与环境变量不同。

| 层 | 作用 |
|-------|------|
| **底层** | **`register_open` / `register_search` / `register_close`** 操作后备文件；通过 **`register_var_t`** 的变量布局。 |
| **对象 API** | **`register_set` / `register_pointer` / `register_value`** 和 **`register_sync` / `register_ssync`**（刷新 mmap）。 |
| **锁** | **`register_lock` / `register_lockw` / `register_unlock`** — 记录锁 + `flock`；见 §8.2 和 `register.h` 注释。 |
| **类型化宏** | **`reg_int`**、**`reg_string`**、**`reg_set_*`**、**`reg_*v` / `reg_*p`** 等，用于常见 C 类型。 |
| **列表** | **`reg_list` / `reg_slist`** -> 变量的 JSON（**`talk_free`**）。 |

（无关但经常与同一代码库一起使用：**`utility.h`** 声明了 **`directory_subsize` / `directory_sum`** 但没有实现 — 见 §14.5。）

---

### 8.1 底层操作

#### register_open / register_search / register_close
```c
register_file_t register_open(const char *object, int flags, int mode, int value_number, int total_size);
register_var_t register_search(register_file_t h, void *point, const char *name);
void register_close(register_file_t h);
```
**描述：** 打开/搜索/关闭寄存器文件

### 8.2 通用操作

#### register_set / register_sset
```c
void *register_set(obj_t this, const char *name, const void *v, int size, int capacity);
void *register_sset(const char *object, const char *name, const void *v, int size, int capacity);
```
**描述：** 设置寄存器值

#### register_pointer / register_spointer
```c
void *register_pointer(obj_t this, const char *name);
void *register_spointer(const char *object, const char *name);
```
**描述：** 获取寄存器值指针（读写）

#### register_value / register_svalue
```c
const void *register_value(obj_t this, const char *name);
const void *register_svalue(const char *object, const char *name);
```
**描述：** 获取寄存器值（只读）

#### register_size / register_ssize
```c
int register_size(obj_t this, const char *name);
int register_ssize(const char *object, const char *name);
```
**描述：** 获取寄存器值大小

#### register_sync / register_ssync
```c
void register_sync(obj_t this);
void register_ssync(const char *object);
```
**描述：** 将寄存器同步到磁盘

#### register_lock / register_lockw / register_unlock
```c
boole register_lock(obj_t this, void *point, const char *name, int flag);
boole register_lockw(obj_t this, void *point, const char *name, int flag);
boole register_unlock(obj_t this, void *point, const char *name);
```
**描述：** 对寄存器变量值范围的建议/记录锁。`flag` 是 `F_WRLCK` 或 `F_RDLCK`。**`register_lock`** 使用 `fcntl(F_SETLK)`（不等待*范围*锁）；**`register_lockw`** 使用 `fcntl(F_SETLKW)`（等待）。两者都可能首先**阻塞在寄存器文件的 `flock(LOCK_SH)` 上** — 见 `register.h` 注释。成功时，`point`（如果非 NULL）可以接收指向变量的 mmap 存储的指针。

### 8.3 整数操作

#### reg_int / reg_sint
```c
int reg_int(obj_t this, const char *name);
int reg_sint(const char *object, const char *name);
```
**描述：** 获取整数寄存器值

#### reg_set_int / reg_sset_int
```c
#define reg_set_int(this, name, v) register_set(this, name, &v, sizeof(int), sizeof(int))
#define reg_sset_int(this, name, v) register_sset(this, name, &v, sizeof(int), sizeof(int))
```
**描述：** 设置整数寄存器值

#### reg_intv / reg_sintv / reg_intp / reg_sintp
```c
#define reg_intv(this, name) (const int*)register_value(this, name)
#define reg_sintv(this, name) (const int*)register_svalue(this, name)
#define reg_intp(this, name) (int*)register_pointer(this, name)
#define reg_sintp(this, name) (int*)register_spointer(this, name)
```
**描述：** 获取整数指针（只读/读写）

### 8.4 布尔操作

#### reg_boole / reg_sboole
```c
boole reg_boole(obj_t this, const char *name);
boole reg_sboole(const char *object, const char *name);
```
**描述：** 获取布尔寄存器值

#### reg_set_boole / reg_sset_boole
```c
#define reg_set_boole(this, name, v) register_set(this, name, &v, sizeof(boole), sizeof(boole))
#define reg_sset_boole(this, name, v) register_sset(this, name, &v, sizeof(boole), sizeof(boole))
```

#### reg_boolev / reg_sboolev / reg_boolep / reg_sboolep
```c
#define reg_boolev(this, name) (const boole*)register_value(this, name)
#define reg_sboolev(this, name) (const boole*)register_svalue(this, name)
#define reg_boolep(this, name) (boole*)register_pointer(this, name)
#define reg_sboolep(this, name) (boole*)register_spointer(this, name)
```

### 8.5 字符串操作

#### reg_string / reg_sstring
```c
const char *reg_string(obj_t this, const char *name);
const char *reg_sstring(const char *object, const char *name);
```
**描述：** 获取字符串寄存器值

#### reg_set_string / reg_sset_string
```c
char *reg_set_string(obj_t this, const char *name, const char *v);
char *reg_sset_string(const char *object, const char *name, const char *v);
```
**描述：** 设置字符串寄存器值

#### reg_stringv / reg_sstringv / reg_stringp / reg_sstringp
```c
#define reg_stringv(this, name)       ((const char *)register_value(this, name))
#define reg_sstringv(this, name)      ((const char *)register_svalue(this, name))
#define reg_stringp(this, name)       ((char *)register_pointer(this, name))
#define reg_sstringp(this, name)      ((char *)register_spointer(this, name))
```
**描述：** 将寄存器存储作为 C 字符串处理的简写。**`reg_stringv` / `reg_sstringv`** 返回只读视图；**`reg_stringp` / `reg_sstringp`** 返回可写的 mmap 后备存储（与 `register_pointer` 相同的生命周期和边界规则）。宏参数名称与 `register.h` 匹配（`this` 对于 `reg_*` 是 `obj_t`，对于 `reg_s*` 变体是**对象名称字符串**）。

### 8.6 列表操作

#### reg_list / reg_slist
```c
talk_t reg_list(obj_t this);
talk_t reg_slist(const char *object);
```
**描述：** 获取寄存器列表

**示例：**
```c
// 设置整数
int val = 42;
reg_sset_int("land@machine", "counter", val);

// 获取整数
int cnt = reg_sint("land@machine", "counter");

// 设置字符串
reg_sset_string("land@machine", "hostname", "router1");

// 获取字符串
const char *host = reg_sstring("land@machine", "hostname");

// 使用指针修改
int *p = reg_sintp("land@machine", "counter");
*p = 100;
register_ssync("land@machine");
```

### 8.7 示例程序（每个 `register.h` API）

触及 `register.h` 中的**每个函数和类型化宏**。**`register_open` / mmap 路径**需要工作的寄存器后端；**`register_lock*`** 使用 **`fcntl`** 记录锁（来自 `<fcntl.h>` 的 `F_RDLCK` / `F_WRLCK`）。

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"
#include <fcntl.h>

static void demo_register_all(void)
{
    int i = 7;
    boole b = true;
    register_file_t h;
    register_var_t rv;
    obj_t o = obj_create("land@machine");
    talk_t tl;

    h = register_open("land@machine", O_RDWR, 0644, REGISTER_VAR_NUM, REGISTER_VAR_SIZE);
    if (h) {
        rv = register_search(h, NULL, "demo_var");
        (void)rv;
        (void)register_value_size(h, "demo_var");
        (void)register_value_pointer(h, "demo_var");
        (void)register_value_set(h, "demo_var", &i, sizeof i, sizeof i);
        register_close(h);
    }

    (void)register_set(o, "rw_i", &i, sizeof i, sizeof i);
    (void)register_sset("land@machine", "rw_i", &i, sizeof i, sizeof i);
    (void)register_pointer(o, "rw_i");
    (void)register_spointer("land@machine", "rw_i");
    (void)register_value(o, "rw_i");
    (void)register_svalue("land@machine", "rw_i");
    (void)register_size(o, "rw_i");
    (void)register_ssize("land@machine", "rw_i");
    register_sync(o);
    register_ssync("land@machine");

    (void)register_lock(o, NULL, "rw_i", F_RDLCK);
    (void)register_lockw(o, NULL, "rw_i", F_RDLCK);
    (void)register_unlock(o, NULL, "rw_i");

    tl = reg_list(o);
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);
    tl = reg_slist("land@machine");
    if (tl > (void *)tpanic && tl && json_check(tl)) talk_free(tl);

    errno = 0;
    (void)reg_int(o, "rw_i");
    (void)reg_sint("land@machine", "rw_i");
    reg_set_int(o, "rw_i", i);
    reg_sset_int("land@machine", "rw_i", i);
    (void)reg_intv(o, "rw_i");
    (void)reg_sintv("land@machine", "rw_i");
    (void)reg_intp(o, "rw_i");
    (void)reg_sintp("land@machine", "rw_i");

    (void)reg_boole(o, "rw_b");
    (void)reg_sboole("land@machine", "rw_b");
    reg_set_boole(o, "rw_b", b);
    reg_sset_boole("land@machine", "rw_b", b);
    (void)reg_boolev(o, "rw_b");
    (void)reg_sboolev("land@machine", "rw_b");
    (void)reg_boolep(o, "rw_b");
    (void)reg_sboolep("land@machine", "rw_b");

    (void)reg_string(o, "rw_s");
    (void)reg_sstring("land@machine", "rw_s");
    (void)reg_set_string(o, "rw_s", "txt");
    (void)reg_sset_string("land@machine", "rw_s", "txt");
    (void)reg_stringv(o, "rw_s");
    (void)reg_sstringv("land@machine", "rw_s");
    (void)reg_stringp(o, "rw_s");
    (void)reg_sstringp("land@machine", "rw_s");

    obj_free(o);
}
```

---

## 9. 日志 API (log.h)

### 9.0 概要

`log.h` 定义了**严重级别**、**输出选项**和**子系统类型/子类型**常量，它们被打包到单个 **`unsigned int flags`** 中传递给 **`landlog()`**。运行时将 `flags` 拆分为级别、选项、类型和子类型，然后在格式化和写入（TUI、syslog、文件等）之前根据 **`register`** 键（如 **`log_mask`** / **`log_options`**）进行过滤。

| 入口点 | 作用 |
|-------------|------|
| **`landlog(flags, file, line, fmt, ...)`** | 底层日志记录器；通常通过 **`journal(flags, ...)`** 或预定义宏（`default_info`、`network_debug`、`shell_fault`、...）调用。 |
| **宏** | 系列包括 **default_***、**shell_***、**land_***、**auth_***、**network_***、... — 每个展开为完全组合的 `flags` 字加上 `__FILE__` / `__LINE__`。 |

**组合自定义 `flags`：** 将**级别** + **选项** + **（类型左移 `LANDLOG_TYPE_OFFSET`）** + **（子类型左移 `LANDLOG_SUBTYPE_OFFSET`）**进行 OR — 见 **§9.3.1** 和 `log.h` 中的 `LANDLOG_*_OFFSET` / `*_MASK` 宏。设置 **`LANDLOG_ERRNO`** 时附加 `strerror(errno)`。

---

### 9.1 日志级别

```c
#define LANDLOG_FAULT     (1)    // 故障
#define LANDLOG_WARN      (2)    // 警告
#define LANDLOG_INFO      (4)    // 信息
#define LANDLOG_DEBUG     (8)    // 调试
#define LANDLOG_VERBOSE   (16)   // 详细
```

### 9.2 日志选项

```c
#define LANDLOG_TUI       (1<<8)   // 输出到 TUI
#define LANDLOG_SYSLOG    (2<<8)   // 输出到系统日志
#define LANDLOG_FILE      (4<<8)   // 输出到文件
#define LANDLOG_TRACE     (8<<8)   // 输出跟踪信息
#define LANDLOG_ERRNO     (16<<8)  // 包含 errno
```

### 9.3 日志类型

| 类型 | 描述 |
|------|-------------|
| LANDLOG_DEFAULT | 默认类型 |
| LANDLOG_LAND | Land 系统 |
| LANDLOG_ARCH | 硬件架构 |
| LANDLOG_NETWORK | 网络 |
| LANDLOG_IFNAME | 接口名 |
| LANDLOG_AGENT | 代理 |
| LANDLOG_CENTER | 中心服务 |
| LANDLOG_WUI | Web UI |
| LANDLOG_CLIENT | 客户端 |
| LANDLOG_MODEM | 调制解调器 |
| LANDLOG_UART | UART |
| LANDLOG_VPN | VPN |

此表中的名称是**类型/子系统标识符**（`LANDLOG_LAND`、`LANDLOG_NETWORK`、...）。在实际 `flags` 值中，它们占据**类型**位字段（见下文），而不仅仅是低字节。

### 9.3.1 为 `landlog()` 组合 `flags`

`landlog()` 将 `flags` 视为实现中的位布局：

| 字段 | 位（概念上） | 宏 |
|-------|---------------------|--------|
| 级别 | 低字节 | `LANDLOG_FAULT`、`LANDLOG_WARN`、`LANDLOG_INFO`、`LANDLOG_DEBUG`、`LANDLOG_VERBOSE`（`LANDLOG_LEVEL_MASK`、`LANDLOG_LEVEL_OFFSET`） |
| 选项 | 下一个字节 | `LANDLOG_TUI`、`LANDLOG_SYSLOG`、`LANDLOG_FILE`、`LANDLOG_TRACE`、`LANDLOG_ERRNO`（`LANDLOG_OPTION_MASK`、`LANDLOG_OPTION_OFFSET`） |
| 类型 | 下一个字节 | 例如 `(LANDLOG_LAND << LANDLOG_TYPE_OFFSET)`（`LANDLOG_TYPE_MASK`、`LANDLOG_TYPE_OFFSET`） |
| 子类型 | 高字节 | 例如 `(LANDLOG_LAND_DEFAULT << LANDLOG_SUBTYPE_OFFSET)`（`LANDLOG_SUBTYPE_MASK`、`LANDLOG_SUBTYPE_OFFSET`） |

预定义的宏如 `default_info(...)` 展开为完整的组合，例如
`(LANDLOG_DEFAULT << LANDLOG_TYPE_OFFSET) | (LANDLOG_DEFAULT_NONE << LANDLOG_SUBTYPE_OFFSET) | LANDLOG_INFO`。
构建自定义 flags 时，将**左移后的**类型/子类型常量与级别和选项进行 OR，而不是将原始 `LANDLOG_LAND` 单独作为整个 `flags` 字。

### 9.4 日志函数

#### landlog
```c
void landlog(unsigned int flags, const char *filename, int line, const char *format, ...);
```
**描述：** 内部日志记录函数

#### journal
```c
#define journal(flags, ...) landlog((flags), (__FILE__), (__LINE__), __VA_ARGS__)
```
**描述：** 带标识符的日志

### 9.5 预定义日志宏

**默认日志：**
- default_verbose / default_debug / default_info / default_warn / default_warning / default_fault / default_faulting

**Shell 日志：**
- shell_verbose / shell_debug / shell_info / shell_warn / shell_warning / shell_fault / shell_faulting

**Land 日志：**
- land_verbose / land_debug / land_info / land_warn / land_warning / land_fault / land_faulting

**Auth 日志：**
- auth_verbose / auth_debug / auth_info / auth_warn / auth_warning / auth_fault / auth_faulting

**Network 日志：**
- network_verbose / network_debug / network_info / network_warn / network_warning / network_fault / network_faulting

**示例：**
```c
// 记录信息
land_info("System started successfully");

// 记录调试
network_debug("Interface %s is up", "eth0");

// 记录警告（带 errno）
land_warning("Failed to open file: %s", filename);

// 使用 journal 自定义类型
journal((LANDLOG_NETWORK<<16)|(LANDLOG_NETWORK_DEFAULT<<24)|LANDLOG_INFO, 
        "Custom log entry");
```

---

## 10. 服务管理 API (serv.h)

### 10.0 概要

服务 API 委托给**服务管理组件**（见 `skinhead.h` 中的 `SERVICE_COM` / `land@service`）。它们调度或控制调用组件 API（`com` + `api` + 参数）的**命名**后台工作器。在 **`srun` / `crun` / ...** 上，**`delay`** 参数是服务实际启动前的**秒数**计数（见 `serv.h`）。

**命名：** **`s*`** 变体将组件作为 **`const char *`**（例如 `"land@machine"`）；**`c*`** 变体使用 **`obj_t`**。

**操作系列：**

| 系列 | 典型入口点 | 行为（根据 `serv.h`） |
|--------|----------------------|-------------------------|
| Run | `srun`、`srunt`、`srun2t`、`sruns`、... | 注册并启动；首先**停止并删除**具有**相同名称**的现有服务。 |
| Reset | `sreset`、`sresett`、`sreset2t`、`sresets`、... | 如果未注册，注册；如果已存在，**重启**。 |
| Start | `sstart`、`sstartt`、`sstart2t`、`sstartst`、`sstarts`、... | **仅在未运行时启动**（幂等启动）。 |
| Stop / remove | `sdelete`、`sstop`、`soff`、`soffdel` | 删除；停止；**暂停**（保持注册）；暂停**并**删除注册。 |
| Query | `spid`、`sinfo`、`sdump`、`slist` | PID、JSON 信息/转储、完整列表 — 在适用时调用者对返回的 `talk_t` 使用 **`talk_free()`**。 |

**底层：** `serv_call(cmd, v, timeout)` 向守护进程发送命令。**`v`** 参数是**所有权转移**的（在实现内部被释放）。返回 JSON（完成后 `talk_free`）、**`ttrue`**（正常，无负载）、**`terror`** 或 **`tpanic`**（超时/IPC 失败）；详见 `serv.h`。

---

### 10.1 服务注册和运行

#### srun / crun / srunt / crunt / srun2t / crun2t / sruns / cruns
```c
boole srun(int delay, const char *com, const char *api, param_t param, const char *nameformat, ...);
boole crun(int delay, obj_t com, const char *api, param_t param, const char *nameformat, ...);
boole srunt(int delay, const char *com, const char *api, talk_t json, const char *nameformat, ...);
boole crunt(int delay, obj_t com, const char *api, talk_t json, const char *nameformat, ...);
boole srun2t(int delay, const char *com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...);
boole crun2t(int delay, obj_t com, const char *api, talk_t json, talk_t json2, const char *nameformat, ...);
boole sruns(int delay, const char *name, const char *com, const char *api, const char *paramformat, ...);
boole cruns(int delay, const char *name, obj_t com, const char *api, const char *paramformat, ...);
```
**描述：** 注册并运行服务（首先停止同名服务）
**参数：**
- delay - 延迟秒数
- com - 组件
- api - API 名称
- param/json - 参数
- nameformat - 服务名称格式

### 10.2 服务重置

#### sreset / creset / sresett / cresett / sreset2t / creset2t / sresets / cresets
```c
boole sreset(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole creset(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**描述：** 重置或启动服务（如果不存在则注册）

### 10.3 服务启动

#### sstart / cstart / sstartt / cstartt / sstart2t / cstart2t / sstarts / cstarts
```c
boole sstart(const char *com, const char *api, param_t param, const char *nameformat, ...);
boole cstart(obj_t com, const char *api, param_t param, const char *nameformat, ...);
```
**描述：** 启动服务

### 10.4 服务控制

#### sdelete / sstop / soff / soffdel
```c
boole sdelete(const char *nameformat, ...);
boole sstop(const char *nameformat, ...);
boole soff(const char *nameformat, ...);
boole soffdel(const char *nameformat, ...);
```
**描述：** 删除/停止/关闭/关闭并删除服务

### 10.5 服务查询

#### spid / sinfo / sdump / slist
```c
pid_t spid(const char *nameformat, ...);
talk_t sinfo(const char *nameformat, ...);
talk_t sdump(const char *nameformat, ...);
talk_t slist(void);
```
**描述：** 获取服务 PID/信息/详细信息/列表

**示例：**
```c
// 运行服务（延迟 5 秒）
sruns(5, "wan_monitor", "network@wan", "monitor", "eth0");

// 停止服务
sstop("wan_monitor");

// 删除服务
sdelete("wan_monitor");

// 获取服务 PID
pid_t pid = spid("wan_monitor");

// 获取所有服务列表
talk_t list = slist();
talk_free(list);
```

### 10.6 示例程序（每个 `serv.h` 函数）

**`serv_call` 的 `v` 参数是所有权转移的**（在实现内部被释放）。**`slist()`** 传递 **`NULL`**。所有其他调用需要**运行中的服务守护进程**才能成功。

```c
#include "skin.h"

static void demo_serv_all(void)
{
    obj_t o = obj_create("land@machine");
    param_t pm = param_create("a");
    talk_t j = string2json("{\"k\":1}");
    talk_t j2 = string2json("{\"k\":2}");
    talk_t r;

    r = serv_call("list", NULL, 1);
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = serv_call("dump", string2json("{\"name\":\"x\"}"), 1);
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);

    (void)srun(0, "land@machine", "status", pm, "demo_svc");
    (void)crun(0, o, "status", pm, "demo_svc");
    (void)srunt(0, "land@machine", "status", j, "demo_svc");
    (void)crunt(0, o, "status", j, "demo_svc");
    (void)srun2t(0, "land@machine", "status", j, j2, "demo_svc");
    (void)crun2t(0, o, "status", j, j2, "demo_svc");
    (void)sruns(0, "demo_named", "land@machine", "status", "%s", "a");
    (void)cruns(0, "demo_named", o, "status", "%s", "a");

    (void)sreset("land@machine", "status", pm, "demo_svc");
    (void)creset(o, "status", pm, "demo_svc");
    (void)sresett("land@machine", "status", j, "demo_svc");
    (void)cresett(o, "status", j, "demo_svc");
    (void)sreset2t("land@machine", "status", j, j2, "demo_svc");
    (void)creset2t(o, "status", j, j2, "demo_svc");
    (void)sresets("demo_named", "land@machine", "status", "%s", "a");
    (void)cresets("demo_named", o, "status", "%s", "a");

    (void)sstart("land@machine", "status", pm, "demo_svc");
    (void)cstart(o, "status", pm, "demo_svc");
    (void)sstartt("land@machine", "status", j, "demo_svc");
    (void)cstartt(o, "status", j, "demo_svc");
    (void)sstart2t("land@machine", "status", j, j2, "demo_svc");
    (void)cstart2t(o, "status", j, j2, "demo_svc");
    (void)sstartst("land@machine", "status", "{}", j2, "demo_svc");
    (void)cstartst(o, "status", "{}", j2, "demo_svc");
    (void)sstarts("demo_named", "land@machine", "status", "%s", "a");
    (void)cstarts("demo_named", o, "status", "%s", "a");

    (void)sdelete("demo_svc");
    (void)sstop("demo_svc");
    (void)soff("demo_svc");
    (void)soffdel("demo_svc");
    (void)spid("demo_svc");

    r = sinfo("demo_svc");
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = sdump("demo_svc");
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);
    r = slist();
    if (r > (void *)tpanic && r && json_check(r)) talk_free(r);

    talk_free(j2);
    talk_free(j);
    param_free(pm);
    obj_free(o);
}
```

---

## 11. 项目信息 API (project.h)

### 11.0 概要

`project.h` 管理**多项目文件系统布局**（`PROJECT_INFOFILE` / **`prj.json`**，默认版本 **`PROJECT_DEFAULT_VERSION`**）。

| 区域 | API |
|------|------|
| **发现** | **`project_scan`**（刷新）、**`project_list`**（缓存）、**`project_dirty`**、**`project_check`**。 |
| **路径** | **`project_path`**、**`project_storage`**、**`project_osc_path`**、**`project_var_path`**、**`project_internal_path`** — 加上使用 **`PROJECT_ID`** 的 **`project2path`**、**`exe2path`**、... 宏。 |
| **引导** | **`project_add_init` / `project_add_uninit` / `project_add_joint` / `project_add_object`** 在项目元数据中注册条目。 |
| **国际化** | **`project_i18n`**、**`project_i18n_get`**。 |

大多数函数将路径返回到 **`buffer`** 或堆 **`talk_t`** 列表中 — 在适用时使用 **`talk_free`**；参见 `project.h` 中的每个原型。

---

### 11.1 项目扫描和列表

#### project_scan / project_list / project_dirty
```c
talk_t project_scan(void);
talk_t project_list(void);
void project_dirty(void);
```
**描述：** 扫描/列出/标记项目列表为脏

#### project_check
```c
boole project_check(const char *name, const char *prjpath);
```
**描述：** 检查项目 JSON 格式

### 11.2 项目路径

#### project_path / project2path
```c
const char *project_path(char *buffer, int buflen, const char *name);
#define project2path(buffer, buflen) project_path(buffer, buflen, PROJECT_ID)
```
**描述：** 获取项目目录

#### project_storage / project2storage
```c
const char *project_storage(char *buffer, int buflen, const char *name, const char *type);
#define project2storage(buffer, buflen, type) project_storage(buffer, buflen, PROJECT_ID, type)
```
**描述：** 获取项目存储目录

#### project_osc_path / osc2path / project_var_path / var2path / project_internal_path / internal2path
```c
const char *project_osc_path(char *buffer, int buflen, const char *name, const char *execute, ...);
#define oscpath(buffer, buflen, ...) project_osc_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_var_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define var2path(buffer, buflen, ...) project_var_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
const char *project_internal_path(char *buffer, int buflen, const char *name, const char *variable, ...);
#define internal2path(buffer, buflen, ...) project_internal_path(buffer, buflen, PROJECT_ID, __VA_ARGS__)
```
**描述：** 获取可执行文件/变量/内部文件路径

### 11.3 项目配置

#### project_add_init / project_add_uninit / project_add_joint / project_add_object
```c
boole project_add_init(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_uninit(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_joint(const char *name, const char *prjpath, const char *level, const char *call);
boole project_add_object(const char *name, const char *prjpath, const char *object, const char *com);
```
**描述：** 添加 init/uninit/事件/对象操作

### 11.4 国际化

#### project_i18n / project_i18n_get
```c
talk_t project_i18n(const char *project, const char *prefix);
const char *project_i18n_get(talk_t lang, const char *text);
```
**描述：** 获取语言 JSON / 获取翻译文本

**示例：**
```c
// 获取项目列表
talk_t projects = project_list();
talk_free(projects);

// 获取当前项目路径
char path[PATH_MAX];
project2path(path, sizeof(path));

// 获取可执行文件路径
exe2path(path, sizeof(path), "myapp");
```

### 11.5 示例程序（每个 `project.h` API）

使用 **`PROJECT_ID`** 用于 **`project2path` / `exe2path` / ...** 宏 — 如果你的构建尚未定义它，请在 **`#include "skin.h"`** 之前定义它（例如 **`-DPROJECT_ID=land`**）。

```c
#ifndef PROJECT_ID
#define PROJECT_ID "land"
#endif
#include "skin.h"

static void demo_project_all(void)
{
    char buf[PATH_MAX];
    talk_t t;
    talk_t lang;

    t = project_scan();
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    t = project_list();
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    project_dirty();
    (void)project_check("land", "/PRJ");

    (void)project_path(buf, sizeof buf, "land");
    (void)project2path(buf, sizeof buf);
    (void)project_storage(buf, sizeof buf, "land", "config");
    (void)project2storage(buf, sizeof buf, "config");
    (void)project_osc_path(buf, sizeof buf, "land", "app");
    (void)osc2path(buf, sizeof buf, "openvpn");
    (void)ko2path(buf, sizeof buf, "drv.ko");
    (void)misc2path(buf, sizeof buf, "m.bin");
    (void)project_var_path(buf, sizeof buf, "land", "x");
    (void)var2path(buf, sizeof buf, "x");
    (void)project_internal_path(buf, sizeof buf, "land", "i");
    (void)internal2path(buf, sizeof buf, "i");

    (void)project_add_init("land", "/PRJ", "app", "land@machine.init");
    (void)project_add_uninit("land", "/PRJ", "app", "land@machine.stop");
    (void)project_add_joint("land", "/PRJ", "ev", "land@machine.on");
    (void)project_add_object("land", "/PRJ", "machine", "land@machine");

    lang = project_i18n("land", "net");
    if (lang > (void *)tpanic && lang && json_check(lang)) {
        (void)project_i18n_get(lang, "k");
        talk_free(lang);
    }
}
```

---

## 12. HE 命令 API (he2com.h)

### 12.0 概要

`he2com.h` 解析和执行 **HE** 字符串 — CLI/配置语言映射到组件上的 **get/set/call**（类似于 shell `project@component:attr=value` 或 `project@component.method[param]`）。

| 部分 | 作用 |
|-------|------|
| **类型** | **`HE_GET` / `HE_SET` / `HE_OR` / `HE_CALL` / `HE_DBS_*`** — 存储在 **`he_t` 的 `flags` 字段**中的操作类型。 |
| **`he_t`** | 解析后的命令：对象、文件属性、方法、**`param_t`**、值 JSON、缓冲区 — **`he_free`**。 |
| **解析** | **`string2he`**、**`json2he`**；**序列化** **`he2json`**、**`he2string`**。 |
| **执行** | **`he_execute`**、**`string_he_execute`**、**`json_he_execute`**、**`talk_he_command`**（批量数组）；**`line_he_command`** 用于面向终端的使用。 |

结果通常是 **`talk_t`** JSON（如果不是哨兵则 **`talk_free`**）或 **`ttrue` / `tfalse` / `NULL` / `terror` / `tpanic`**（根据 `he2com.h`）。所有权：附加到 **`he_t`** 的任何内容由 **`he_free`** 释放；执行 API 记录返回的 JSON 是否是新的。

---

### 12.1 HE 命令类型

```c
#define HE_GET       0      // 获取配置值
#define HE_SET       1      // 设置配置值
#define HE_OR        2      // 修改配置值
#define HE_CALL      3      // API 调用
#define HE_DBS_GET   4      // 获取数据库值
#define HE_DBS_SET   5      // 设置数据库值
#define HE_DBS_OR    6      // 修改数据库值
#define HE_DBS_CALL  7      // 数据库 API 调用
```

### 12.2 HE 结构操作

#### string2he / json2he
```c
he_t string2he(const char *cmd);
he_t json2he(talk_t cmd);
```
**描述：** 从字符串/JSON 解析 HE 命令

#### he2json / he2string
```c
talk_t he2json(he_t h);
const char *he2string(he_t h);
```
**描述：** 将 HE 结构转换为 JSON/字符串

#### he_free
```c
void he_free(he_t h);
```
**描述：** 释放 HE 结构

### 12.3 HE 命令执行

#### he_execute / string_he_execute / line_he_command / json_he_execute / talk_he_command
```c
talk_t he_execute(he_t h);
talk_t string_he_execute(const char *cmd);
int line_he_command(const char *cmd);
talk_t json_he_execute(talk_t cmd);
talk_t talk_he_command(talk_t cmd);
```
**描述：** 执行 HE 命令

**示例：**
```c
// 执行字符串 HE 命令
talk_t result = string_he_execute("land@machine.status");
if (result > (void *)tpanic && json_check(result)) {
    talk_free(result);
}

// 带参数执行
result = string_he_execute("network@frame.set{\"ip\":\"192.168.1.1\"}");

// 行命令（打印到终端）
int rc = line_he_command("land@machine.version");
```

### 12.4 示例程序（每个 `he2com.h` 函数）

**`HE_*`** 符号是**整数常量**（见 §12.1）；第一行强制它们出现在代码片段中。**`json2he()`** 至少需要 **`"obj"`**；对于调用形状，添加带有方法名的 **`"op"`**（见 **`he2com.h`** / HE JSON 形态）。

```c
#include "skin.h"

static void demo_he2com_all(void)
{
    he_t h;
    talk_t t;
    talk_t cmdj;
    talk_t hj;
    talk_t batch;

    (void)(HE_GET | HE_SET | HE_OR | HE_CALL | HE_DBS_GET | HE_DBS_SET | HE_DBS_OR | HE_DBS_CALL);

    h = string2he("land@machine.status");
    t = he2json(h);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    (void)he2string(h);
    he_free(h);

    cmdj = string2json("{\"obj\":\"land@machine\",\"op\":\"status\"}");
    hj = json2he(cmdj);
    talk_free(cmdj);
    if (hj) {
        t = he2json(hj);
        if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
        (void)he2string(hj);
        t = he_execute(hj);
        if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
        he_free(hj);
    }

    t = string_he_execute("land@machine.status");
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);

    (void)line_he_command("land@machine.version");

    cmdj = string2json("{\"obj\":\"land@machine\",\"op\":\"status\"}");
    t = json_he_execute(cmdj);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    talk_free(cmdj);

    batch = string2json("{\"r\":{\"obj\":\"land@machine\",\"op\":\"status\"}}");
    t = talk_he_command(batch);
    if (t > (void *)tpanic && t && json_check(t)) talk_free(t);
    talk_free(batch);
}
```

---

## 13. 链表 API (link.h)

### 13.1 宏定义

#### link_entry
```c
#define link_entry(node, type, member) ((type*)((char*)(node) - (unsigned long)(&((type*)0)->member)))
```
**描述：** 从链表节点获取实际数据结构

#### link_each
```c
#define link_each(var, head) for((var) = (*head); NULL != (var); (var = (var)->next))
```
**描述：** 遍历链表

### 13.2 链表操作

#### link_init
```c
boole link_init(link_t node);
```
**描述：** 初始化链表节点

#### link_child
```c
link_t link_child(lhead_t head);
```
**描述：** 获取链表的第一个节点

#### link_size
```c
int link_size(lhead_t head);
```
**描述：** 获取链表中的节点数

#### link_delete / link_remove
```c
boole link_delete(lhead_t head, link_t elm);
link_t link_remove(lhead_t head, link_t elm);
```
**描述：** 从链表中删除节点

#### link_push / link_pop
```c
boole link_push(lhead_t head, link_t elm);
link_t link_pop(lhead_t head);
```
**描述：** 在链表末尾推入/弹出节点

#### link_insert / link_pull
```c
boole link_insert(lhead_t head, link_t elm, link_t next);
link_t link_pull(lhead_t head);
```
**描述：** 在指定位置插入/拉取节点

**示例：**
```c
// 定义包含链表节点的结构
struct my_struct {
    int data;
    link_struct link;
};

// 初始化链表头
link_struct *head = NULL;

// 创建节点
struct my_struct *node = malloc(sizeof(struct my_struct));
node->data = 42;
link_init(&node->link);

// 添加到链表
link_push(&head, &node->link);

// 遍历链表
link_t pos;
link_each(pos, &head) {
    struct my_struct *entry = link_entry(pos, struct my_struct, link);
    printf("%d\n", entry->data);
}
```

---

## 14. 工具函数 API (utility.h)

本章列出最常见的入口点。**`utility.h` 声明了更多辅助函数**（网络、shell、套接字、UART、时间等）；参阅头文件了解完整原型。

**约定（典型模式，不保证对每个符号都适用）：**

- 许多函数用**负整数**或 **`false`** 指示失败，并在系统调用失败时设置 **`errno`**；检查 `utility.h` 中声明的注释。
- 分配辅助函数（`md5_encode`、`b64_encode`、`url_encode`、...）通常返回**堆分配的**缓冲区；**调用者释放**，除非头文件另有说明。
- **线程安全：** 工具函数不统一可重入；除非有文档说明，否则将全局进程状态、静态缓冲区（如果有）和子进程/shell 辅助函数视为**非线程安全**。

### 14.1 字符串处理

#### char2char / low2upp / upp2low
```c
void char2char(char *src, char a, char b);
void low2upp(char *str);
void upp2low(char *str);
```
**描述：** **`char2char`** 将 **`src`** 中所有 **`a`** 替换为 **`b`**（原地修改，NUL 结尾）。**`low2upp` / `upp2low`** 对整串逐字节使用 **`toupper` / `tolower`**，按 **`(unsigned char)`** 转换。**`NULL` 的 `src` / `str`** → **`EINVAL`** 且直接返回（与编码辅助函数文档一致）。

### 14.2 编码/解码

#### md5_encode / b64_encode / b64_decode
```c
char *md5_encode(const char *s, int len);
char *b64_encode(const char *s, int len);
char *b64_decode(const char *s, int *len);
```
**描述：** MD5 摘要为**十六进制字符串**；Base64 编解码。**`md5_encode`**：**`NULL`** 或非法长度 → **`NULL`**、**`EINVAL`**；分配失败 → **`NULL`**、**`ENOMEM`**。返回串由调用者释放。

#### url_encode / url_decode
```c
char *url_encode(char const *s, int len, int *new_length);
int url_decode(char *str, int len);
```
**描述：** URL 编码/解码

#### simple_encode / simple_decode
```c
char *simple_encode(const char *message, const char *tok);
char *simple_decode(const char *message, const char *tok);
```
**描述：** **AES-128-CBC**（密钥/IV 由 **`tok`** 与实现内固定盐派生），再经 **Base64** 封装 — **非** XOR。失败返回 **`NULL`**，**`errno`** 按编码辅助函数约定（**`EINVAL`**、**`ENOMEM`** 等）。

#### string2hex / hex2string / hex2printf
```c
void string2hex(const char *src, char *dest, int len);
void hex2string(const char *src, char *dest, int len);
void hex2printf(const char *src, char *dest, int len);
```
**描述：** 字符串和十六进制转换

### 14.3 MAC 地址处理

#### string2mac / mac2string / mac2int / mac2serial / mac2add / macrang
```c
boole string2mac(const char *macbuf, hp_mac_t mac);
boole mac2string(hp_mac_t mac, char *macbuf);
unsigned int mac2int(hp_mac_t mac);
boole mac2serial(hp_mac_t mac, char *macbuf);
void mac2add(hp_mac_t mac, int i);
boole macrang(hp_mac_t mac, hp_mac_t start, hp_mac_t end, int mod);
```
**描述：** MAC 地址转换

### 14.4 信号处理

#### signal_noprocess / signal_register
```c
void signal_noprocess(int signo);
sighandler_t signal_register(int signo, sighandler_t func, int sa_flags);
```
**描述：** 空操作信号处理器/注册信号处理器

### 14.5 目录操作

#### directory_size / directory_subsize / directory_sum / directory_subsum
```c
int directory_size(const char *dir);
int directory_subsize(const char *dir);
int directory_sum(const char *dir);
int directory_subsum(const char *dir);
```
**描述：** 获取目录大小或条目数。**`directory_subsize` 和 `directory_sum` 在头文件中声明但在此代码树中未实现**（链接调用将失败）；如需使用请使用 `directory_size` / `directory_subsum` 或添加实现。

### 14.6 文件锁定

#### fd_lock / fd_unlock / fd_lock_pid
```c
boole fd_lock(int fd, boole ex, int start, int whence, int len, int wait);
boole fd_unlock(int fd, boole ex, int start, int whence, int len);
pid_t fd_lock_pid(int fd, boole ex, int start, int whence, int len);
```
**描述：** 文件区域加锁/解锁/获取锁拥有者

#### fd_block / fd_nonblock
```c
boole fd_block(int fd);
boole fd_nonblock(int fd);
```
**描述：** 设置文件阻塞/非阻塞模式

#### lock_open / lock_close
```c
int lock_open(const char *filename, int flags, int mode, int block);
int lock_close(int fd);
```
**描述：** 带锁打开/关闭文件

### 14.7 文件读写

#### string2file / string3file / file2string
```c
int string2file(const char *filename, const char *format, ...);
int string3file(const char *filename, const char *format, ...);
const char *file2string(const char *filename, char *buffer, int bufsize);
```
**描述：** 写字符串到文件/追加到文件/从文件读取

#### number2file / file2number
```c
int number2file(const char *filename, int number);
int file2number(const char *filename);
```
**描述：** 写/读数字到/从文件

### 14.8 时间函数

#### time_stamp / uptime_int / uptime_string / uptime_desc / livetime_desc / date_desc / date_set / date_adjust
```c
long long time_stamp(void);
unsigned long uptime_int(void);
const char *uptime_string(char *buffer, int buflen);
const char *uptime_desc(char *buffer, int buflen);
const char *livetime_desc(unsigned int ontime, char *buffer, int buflen);
const char *date_desc(char *buffer, int buflen);
boole date_set(time_t seconds, const char* zone);
time_t date_adjust(time_t seconds, const char* zone);
```
**描述：** 时间戳/运行时间/日期操作

### 14.9 系统命令

#### shell / execute / silent_execute
```c
int shell(const char *format, ...);
int execute(int timeout, boole silent, const char *format, ...);
#define silent_execute(...) execute(0, 1, __VA_ARGS__)
```
**描述：** **`shell`** 将命令格式化到 **`LINE_MAX`** 缓冲区，拒绝**空串**、**超长**命令以及对 **`/bin/sh -c`** 不安全的模式（如 `` ` ``、`|`、`;`、`<`、`$(` / `${`、不当的 `&` 等），再调用 **`system(3)`** — 返回其状态，失败时 **`errno`** 反映最后一次相关错误。**`execute`** **`fork`** 后以 **`execvp`** 执行**首个空白分隔**的词为 argv\[0\]（最多再 **19** 个参数）；可选 **`timeout`** 秒与 **`SIGALRM`**，超时则 **`SIGKILL`**。正常子进程退出时返回 **`WEXITSTATUS`**，否则 **`-1`** 且 **`errno`** 表示 fork/exec/wait/信号等问题。**`silent_execute`** 即 **`execute(0, 1, …)`**（子进程在可行时将 stdio 重定向到 **`/dev/null`**）。

#### killpid
```c
boole killpid(pid_t pid, int timeout);
```
**描述：** 强制终止进程

#### ifconfig / iptables / ip6tables / ebtables
```c
int ifconfig(const char *format, ...);
int iptables(const char *format, ...);
int ip6tables(const char *format, ...);
int ebtables(const char *format, ...);
```
**描述：** 执行网络配置命令（带锁）

#### insmod / rmmod / lsmod
```c
int insmod(const char *module);
int rmmod(const char *module);
boole lsmod(const char *module);
```
**描述：** **`insmod`**：若 **`/proc/modules`** 中**尚无**该模块名则通过 **`shell`** 调用 **`modprobe`**；已加载则返回 **`-1`**、**`EEXIST`**。**`rmmod`**：仅当模块**出现在** **`/proc/modules`** 时执行 **`rmmod`**；未加载则 **`-1`**、**`EINVAL`**。其余返回值遵循 **`shell`**。**`lsmod`** 检查 **`/proc/modules`** 中是否存在。

### 14.10 网络工具

#### ip2subnet / netmask2cidr / netmask2num
```c
const char *ip2subnet(const char *ip, const char *mask, char *subnet, int len);
const char *netmask2cidr(const char *netmask, char *buf, int buflen);
unsigned int netmask2num(const char *mask);
```
**描述：** IP 子网计算

#### netdev_flags / netdev_info / netdev_flew
```c
int netdev_flags(const char *card, short flag);
int netdev_info(const char *card, char *ip, int ip_len, char *pppip, int pppip_len, char *mask, int mask_len, char *mac, int mac_len);
int netdev_flew(const char *card, unsigned long long *rx_bytes, unsigned long long *rx_packets, unsigned long long *rx_errs, unsigned long long *rx_drops, unsigned long long *tx_bytes, unsigned long long *tx_packets, unsigned long long *tx_errs, unsigned long long *tx_drops);
```
**描述：** 网络接口标志/信息/流量统计

#### route_info / routes_info / outer_info / gateway_info
```c
int route_info(const char *destname, const char *mask, const char *metric, char *gateway, char *netdev);
int routes_info(const char *tid, const char *destname, const char *mask, const char *metric, char *gateway, char *netdev);
boole outer_info(char *gateway, char *netdev);
boole gateway_info(char *gateway, char *netdev);
```
**描述：** 路由信息查询

#### domain2ip
```c
const char *domain2ip(const char *addr, char *ipbuf, int ipbuflen, int timeout);
```
**描述：** 域名解析

### 14.11 套接字工具

#### socket_reuse / socket_nocheck / socket_block / socket_nonblock
```c
boole socket_reuse(int sock);
boole socket_nocheck(int sock);
boole socket_block(int sock);
boole socket_nonblock(int sock);
```
**描述：** 套接字选项设置

#### socket_keepalive
```c
boole socket_keepalive(int sock, int keepintvl, int keepidle, int keepcnt);
```
**描述：** 设置 TCP 保活

#### tcp_connect / udp_connect
```c
int tcp_connect(const char *peer, int port, int timeout, int keepintvl, int keepidle, int keepcnt);
int udp_connect(const char *peer, int port, int timeout);
```
**描述：** 创建 TCP/UDP 连接

#### unix_connect / unix_listen
```c
int unix_connect(const char *peer, const char *local, int type);
int unix_listen(const char *local, int type);
```
**描述：** Unix 域套接字连接/监听

### 14.12 Talk 传输

#### talk2fd / talk2tcp / talk2udp / talk2socket
```c
int talk2fd(int fd, talk_t talk, int errcode);
int talk2tcp(int fd, talk_t talk, int errcode, int timeout);
int talk2udp(int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout);
int talk2socket(int fd, talk_t talk, int errcode, struct sockaddr *addr, int addrlen, int timeout);
```
**描述：** 发送 talk 数据

#### fd2talk / tcp2talk / udp2talk / socket2talk
```c
talk_t fd2talk(int fd);
talk_t tcp2talk(int fd, int timeout);
talk_t udp2talk(int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout);
talk_t socket2talk(int fd, struct sockaddr *addr, socklen_t *addrlen, int timeout);
```
**描述：** 接收 talk 数据

### 14.13 系统工具

#### random_long
```c
unsigned long random_long(void);
```
**描述：** 获取随机数

#### partition_dev / partlabel_dev
```c
boole partition_dev(const char *name, char *mtd, char *mtdblock);
boole partlabel_dev(const char *name, char *mmc);
```
**描述：** 获取 MTD/MMC 设备路径

#### fileline_merge
```c
boole fileline_merge(const char *gap, const char *src, const char *adjust, const char *merge);
```
**描述：** 合并文件行

### 14.14 UART

#### uart_open
```c
int uart_open(const char *path, int speed, int parity, int databit, int stopbit, int flow, int timeout);
```
**描述：** 打开 UART
**参数：**
- path - 设备路径
- speed - 波特率（9600、115200 等）
- parity - 校验（0 无、1 奇、2 偶）
- databit - 数据位（5、6、7、8）
- stopbit - 停止位（1、2）
- flow - 流控（0 无、1 软件、2 硬件）
- timeout - 超时

---

## 15. Skin API 宏 (skinapi.h)

### 15.1 内存和格式错误处理

```c
#define memory_exit(i) do { default_fault("memory oops"); exit(i); } while(0)
#define format_error(string) do { default_fault("format oops: %s", string); } while(0)
```

### 15.2 FPK API

```c
#define fpk_list(...) scalls(FPK_COM, "list", __VA_ARGS__)
#define fpk_register(...) scalls(FPK_COM, "register", __VA_ARGS__)
#define fpk_unregister(...) scalls(FPK_COM, "unregister", __VA_ARGS__)
```

### 15.3 Init API

```c
#define init_list(...) scalls(INIT_COM, "list", __VA_ARGS__)
#define init_register(item, call) scall2s(INIT_COM, "register", item, call)
```

### 15.4 Uninit API

```c
#define uninit_list(...) scalls(UNINIT_COM, "list", __VA_ARGS__)
#define uninit_register(item, call) scall2s(UNINIT_COM, "register", item, call)
```

**注意：** 与 **`skinapi.h`** 一致：**`uninit_list`** 将 **`"list"`** 发往 **`UNINIT_COM`**（**不是** **`INIT_COM`**）。

### 15.5 Joint API

```c
#define joint_list(...) scalls(JOINT_COM, "list", __VA_ARGS__)
#define joint_register(item, call) scall2s(JOINT_COM, "register", item, call)
#define joint_unregister(item, call) scall2s(JOINT_COM, "unregister", item, call)
#define joint_calls(joint, string) scall2s(JOINT_COM, "call", joint, string)
#define joint_callt(joint, json) scallst(JOINT_COM, "call", joint, json)
```

### 15.6 Machine API

```c
#define machine_config(...) sgets(MACHINE_COM, __VA_ARGS__)
#define machine_status(...) scalls(MACHINE_COM, "status", __VA_ARGS__)
#define machine_restart(delay, key) scalls(MACHINE_COM, "restart", "%d,%s", delay, (key)?(key):"")
#define machine_reboot(delay, key) scalls(MACHINE_COM, "reboot", "%d,%s", delay, (key)?(key):"")
#define machine_default(delay, key) scalls(MACHINE_COM, "default", "%d,%s", delay, (key)?(key):"")
```

**注意：** **`key` 为 `NULL`** 时向 **`scalls`** 传入 **`""`**（标准 C）。见 **`skinapi.h`** 文件头注释。

---

## 16. 预定义组件常量 (skinhead.h)

### 16.0 限制与平台上限（`skinhead.h`）

| 宏 | 典型含义 |
|----|----------|
| **`NAME_MAX`** | 若系统未定义则为 **256** — Skin 辅助函数中单路径分量名的合理上限。 |
| **`PATH_MAX`** | 若未定义则为 **512** — 构建文件系统路径的缓冲区大小。 |
| **`LINE_MAX`** | 若未定义则为 **1024** — 如 **`shell`** / **`execute`** 的命令缓冲区。 |
| **`JSON_LINE_MAX`** | **65535** — 部分解析器对单行 JSON/字符串块的上限。 |

### 16.1 硬件项目组件

```c
#define GPIO_COM        "arch@gpio"
#define DATA_COM        "arch@data"
#define CONFIG_COM      "arch@config"
#define FIRMWARE_COM    "arch@firmware"
#define FACTORY_COM     "arch@factory"
#define NET_CONFIG      "arch@net"
#define ETHERNET_COM    "arch@ethernet"
#define USB_COM         "arch@usb"
#define PCI_COM         "arch@pci"
#define SDIO_COM        "arch@sdio"
#define CUSTOM_COM      "arch@custom"
#define LOCK_COM        "arch@lock"
```

### 16.2 核心项目组件

```c
#define HEART_COM       "land@heart"
#define MACHINE_COM     "land@machine"
#define SERVICE_COM     "land@service"
#define FPK_COM         "land@fpk"
#define INIT_COM        "land@init"
#define UNINIT_COM      "land@uninit"
#define JOINT_COM       "land@joint"
#define AUTH_COM        "land@auth"
#define REG_COM         "land@register"
```

### 16.3 网络项目组件

```c
#define NETWORK_COM     "network@frame"
#define CONNECT_COM     "network@connect"
#define KEEPLIVE_COM    "network@keeplive"
#define BRIDGE_COM      "network@bridge"
#define VLAN_COM        "network@vlan"
#define HOSTS_COM       "network@hosts"
```

### 16.4 接口名组件

```c
#define LAN_COM         "ifname@lan"
#define LAN1_COM        "ifname@lan1"
#define LAN2_COM        "ifname@lan2"
#define LAN3_COM        "ifname@lan3"
#define LAN4_COM        "ifname@lan4"
#define WLTE_COM        "ifname@lte"
#define WAN_COM         "ifname@wan"
#define WAN2_COM        "ifname@wan2"
#define WISP_COM        "ifname@wisp"
```

### 16.5 转发项目组件

```c
#define NAT_COM         "forward@nat"
#define DNAT_COM        "forward@dnat"
#define FIREWALL_COM    "forward@firewall"
#define RULE_COM        "forward@rule"
#define ROUTE_COM       "forward@route"
#define ROUTES_COM      "forward@routes"
```

### 16.6 无线项目组件

```c
#define WIFI_AP_COM     "wifi@ap"
#define WIFI_STA_COM    "wifi@sta"
#define NRADIO_COM      "wifi@n"
#define NSSID_COM       "wifi@nssid"
#define NSTA_COM        "wifi@nsta"
```

### 16.7 调制解调器组件

```c
#define OPERATOR_COM    "modem@operator"
#define MODEM_COM       "modem@atd"
#define LTE_COM         "modem@lte"
#define SMSD_COM        "modem@smsd"
#define SMS_COM         "modem@sms"
#define ATPROXY_COM     "modem@atproxy"
```

### 16.8 UART 项目组件

```c
#define UART_COM        "uart@frame"
#define SERIAL_COM      "uart@serial"
#define SERIAL2_COM     "uart@serial2"
```

---

## 17. 完整使用示例

### 17.1 基本 JSON 操作

```c
#include "skin.h"

int main() {
    // 创建 JSON 对象
    talk_t config = json_create(NULL);
    
    // 设置配置值
    json_set_string(config, "hostname", "router1");
    json_set_number(config, "port", 8080);
    json_set_string(config, "ip", "192.168.1.1");
    
    // 嵌套 JSON
    talk_t network = json_create(NULL);
    json_set_string(network, "wan_ip", "10.0.0.1");
    json_set_string(network, "lan_ip", "192.168.1.1");
    json_set_json(config, "network", network);
    
    // 打印 JSON
    talk_print(config);
    
    // 转换为字符串
    char *str = json2string(config);
    printf("JSON: %s\n", str);
    free(str);
    
    // 保存到文件
    json_save(config, "/tmp/config.json");
    
    // 释放
    talk_free(network);
    talk_free(config);
    
    return 0;
}
```

### 17.2 组件通信

```c
#include "skin.h"

int main() {
    // 方法 1：使用 scall 直接调用
    void *result = scalls("land@machine", "status", "");
    if (result > (void *)tpanic && json_check((talk_t)result)) {
        talk_print((talk_t)result);
        talk_free((talk_t)result);
    }
    
    // 方法 2：使用参数结构
    param_t p = param_create("eth0");
    result = scall("network@frame", "info", p);
    param_free(p);
    
    // 方法 3：使用 JSON 参数
    talk_t json = string2json("{\"interface\":\"eth0\",\"enable\":1}");
    result = scallt("network@frame", "config", json);
    talk_free(json);
    
    // 方法 4：获取字符串结果
    char buf[256];
    const char *ver = scalls_string(buf, sizeof(buf), "land@machine", "version", "");
    printf("Version: %s\n", ver);
    
    return 0;
}
```

### 17.3 配置管理

```c
#include "skin.h"

int main() {
    // 获取配置
    char buf[128];
    
    // 获取字符串配置
    sgets_string(buf, sizeof(buf), "network@frame", "wan/ip");
    printf("WAN IP: %s\n", buf);
    
    // 获取 JSON 配置
    talk_t cfg = (talk_t)sgets("network@frame", "wan");
    if (cfg > (void *)tpanic && json_check(cfg)) {
        const char *ip = json_string(cfg, "ip");
        const char *mask = json_string(cfg, "mask");
        printf("IP: %s, Mask: %s\n", ip, mask);
        talk_free(cfg);
    }
    
    // 设置配置
    ssets_string("network@frame", "192.168.1.1", "wan/%s", "ip");
    
    // 设置 JSON 配置
    talk_t wan_config = string2json("{\"ip\":\"192.168.1.1\",\"mask\":\"255.255.255.0\"}");
    sset("network@frame", wan_config, attr_create("wan"));
    talk_free(wan_config);
    
    return 0;
}
```

### 17.4 寄存器操作

```c
#include "skin.h"

int main() {
    // 设置整数
    int counter = 100;
    reg_sset_int("land@machine", "boot_count", counter);
    
    // 获取整数
    int boot_cnt = reg_sint("land@machine", "boot_count");
    printf("Boot count: %d\n", boot_cnt);
    
    // 设置字符串
    reg_sset_string("land@machine", "device_name", "MyRouter");
    
    // 获取字符串
    const char *name = reg_sstring("land@machine", "device_name");
    printf("Device name: %s\n", name);
    
    // 使用指针修改
    int *p = reg_sintp("land@machine", "boot_count");
    (*p)++;
    register_ssync("land@machine");
    
    return 0;
}
```

### 17.5 服务管理

```c
#include "skin.h"

int main() {
    // 运行服务（延迟 5 秒）
    sruns(5, "wan_monitor", "network@wan", "monitor", "eth0");
    
    // 立即运行服务
    sruns(0, "dhcp_client", "client@dhcps", "start", "eth0");
    
    // 停止服务
    sstop("wan_monitor");
    
    // 删除服务
    sdelete("wan_monitor");
    
    // 获取服务 PID
    pid_t pid = spid("dhcp_client");
    if (pid > 0) {
        printf("Service PID: %d\n", pid);
    }
    
    // 获取服务信息
    talk_t info = sinfo("dhcp_client");
    if (info) {
        talk_print(info);
        talk_free(info);
    }
    
    // 获取所有服务列表
    talk_t list = slist();
    if (list && json_check(list)) {
        talk_print(list);
        talk_free(list);
    }
    
    return 0;
}
```

### 17.6 HE 命令执行

```c
#include "skin.h"

int main() {
    // 执行简单 HE 命令
    talk_t result = string_he_execute("land@machine.status");
    if (result > (void *)tpanic && json_check(result)) {
        talk_print(result);
        talk_free(result);
    }
    
    // 执行带参数的 HE 命令
    result = string_he_execute("network@frame.get[wan/ip]");
    if (result > (void *)tpanic) {
        if (json_check(result)) {
            talk_print(result);
        } else {
            printf("Result: %s\n", x2string(result));
        }
        talk_free(result);
    }
    
    // 执行设置命令
    result = string_he_execute("network@frame.set[wan/ip=192.168.1.1]");
    
    // 行命令（打印到终端）
    int rc = line_he_command("land@machine.version");
    
    return 0;
}
```

### 17.7 网络工具

```c
#include "skin.h"

int main() {
    // 获取网络接口信息
    char ip[32], mask[32], mac[32];
    netdev_info("eth0", ip, sizeof(ip), NULL, 0, mask, sizeof(mask), mac, sizeof(mac));
    printf("IP: %s, Mask: %s, MAC: %s\n", ip, mask, mac);
    
    // 获取流量统计
    unsigned long long rx_bytes, rx_packets, tx_bytes, tx_packets;
    netdev_flew("eth0", &rx_bytes, &rx_packets, NULL, NULL, 
                &tx_bytes, &tx_packets, NULL, NULL);
    printf("RX: %llu bytes, TX: %llu bytes\n", rx_bytes, tx_bytes);
    
    // 获取默认网关
    char gateway[32], netdev[32];
    if (outer_info(gateway, netdev)) {
        printf("Gateway: %s via %s\n", gateway, netdev);
    }
    
    // 域名解析
    char ipbuf[32];
    const char *resolved = domain2ip("www.example.com", ipbuf, sizeof(ipbuf), 5);
    if (resolved) {
        printf("Resolved: %s\n", resolved);
    }
    
    // 执行 iptables 命令
    iptables("-A INPUT -p tcp --dport 80 -j ACCEPT");
    
    return 0;
}
```

### 17.8 文件操作

```c
#include "skin.h"

int main() {
    // 写入文件
    string2file("/tmp/test.txt", "Hello %s\n", "World");
    
    // 追加到文件
    string3file("/tmp/test.txt", "Line %d\n", 2);
    
    // 从文件读取
    char buf[256];
    const char *content = file2string("/tmp/test.txt", buf, sizeof(buf));
    printf("Content: %s\n", content);
    
    // 写入数字
    number2file("/tmp/counter.txt", 42);
    
    // 读取数字
    int num = file2number("/tmp/counter.txt");
    printf("Number: %d\n", num);
    
    // 带锁打开
    int fd = lock_open("/tmp/locked.txt", O_RDWR|O_CREAT, 0644, -1);
    if (fd >= 0) {
        write(fd, "test", 4);
        lock_close(fd);
    }
    
    return 0;
}
```

### 17.9 日志记录

```c
#include "skin.h"

int main() {
    // 记录不同级别
    land_verbose("This is a verbose message");
    land_debug("Debug information: %d", 42);
    land_info("System started");
    land_warn("Warning: low memory");
    land_warning("Failed to open: %s", "file.txt");  // 带 errno
    land_fault("Critical error occurred");
    land_faulting("System call failed");  // 带 errno
    
    // 网络相关日志
    network_info("Interface %s is up", "eth0");
    network_debug("IP address: %s", "192.168.1.1");
    
    // 使用 journal 自定义类型
    journal((LANDLOG_LAND<<16)|(LANDLOG_LAND_SERVICE<<24)|LANDLOG_INFO, 
            "Custom log entry");
    
    return 0;
}
```

### 17.10 综合示例：配置管理工具

```c
#include "skin.h"

// 显示组件配置
void show_config(const char *com, const char *attr) {
    char buf[256];
    const char *val = sgets_string(buf, sizeof(buf), com, attr);
    if (val) {
        printf("%s.%s = %s\n", com, attr, val);
    } else {
        printf("%s.%s = (not set)\n", com, attr);
    }
}

// 设置组件配置
void set_config(const char *com, const char *attr, const char *value) {
    if (ssets_string(com, value, attr)) {
        printf("Set %s.%s = %s [OK]\n", com, attr, value);
    } else {
        printf("Set %s.%s = %s [FAILED]\n", com, attr, value);
    }
}

// 调用组件方法
void call_method(const char *com, const char *api, const char *param) {
    void *result = scalls(com, api, "%s", param);
    if (result == tnull) {
        printf("Call %s.%s(%s) = null / no payload\n", com, api, param);
    } else if (result == ttrue) {
        printf("Call %s.%s(%s) = true\n", com, api, param);
    } else if (result == tfalse) {
        printf("Call %s.%s(%s) = false\n", com, api, param);
    } else if (result == terror) {
        printf("Call %s.%s(%s) = error\n", com, api, param);
    } else if (result == tpanic) {
        printf("Call %s.%s(%s) = panic / bad call\n", com, api, param);
    } else if (json_check((talk_t)result)) {
        printf("Call %s.%s(%s) = ", com, api, param);
        talk_print((talk_t)result);
        talk_free((talk_t)result);
    } else {
        printf("Call %s.%s(%s) = (unhandled pointer result)\n", com, api, param);
    }
}

int main(int argc, char *argv[]) {
    // 显示机器信息
    printf("=== Machine Info ===\n");
    show_config("land@machine", "version");
    show_config("land@machine", "serial/board");
    
    // 显示网络配置
    printf("\n=== Network Config ===\n");
    show_config("network@frame", "wan/ip");
    show_config("network@frame", "wan/mask");
    show_config("network@frame", "wan/gateway");
    
    // 设置配置示例
    printf("\n=== Setting Config ===\n");
    set_config("land@machine", "hostname", "MyRouter");
    
    // 调用方法示例
    printf("\n=== Calling Methods ===\n");
    call_method("land@machine", "status", "");
    
    return 0;
}
```

---

## 18. 编译和使用

### 18.1 包含头文件

**伞形头文件（推荐用于应用和快速移植）：**

```c
#include "skin.h"   /* stdhead.h + skinhead.h + skinapi.h */
```

**分层（组件/最小包含）：**

- `skinhead.h` — 长度、`boole`、`PROJECT_OBJECT_GAPS`、`MACHINE_COM`、...
- `skinapi.h` — **不包含其他头文件**；它只展开诸如 `scalls()` / `machine_config()` 等宏。你必须在 `skinapi.h` *之前*包含 **`com.h`**（用于 `scalls`、`obj_t`、...）和 **`skinhead.h`**（用于 `MACHINE_COM`、`INIT_COM`、...），除非你已经使用了 `skin.h`。
- 单独的模块：`talk.h`、`com.h`、`register.h`、`config.h`、`utility.h`、...（包含你使用的；如果缺少 typedef 或限制，添加 `skinhead.h` / `stdhead.h`）

### 18.2 编译选项

```bash
gcc -o myapp myapp.c \
    -I/path/to/skin \
    -L/path/to/lib \
    -lskin \
    -levent \
    -lpthread \
    -ldl \
    -lm
```

### 18.3 链接库

- libskin.so - SkinOS 核心库
- libevent.so - 事件循环库
- libpthread.so - 线程库
- libdl.so - 动态加载库
- libm.so - 数学库

---

## 19. 重要说明

1. **内存管理：** 对 talk/JSON API 分配的 `talk_t` 值使用 `talk_free()`；对 `json2string()` 和典型的 `utility.h` 分配器使用 `free()`，除非 API 另有说明。
2. **错误处理：** 失败后检查返回值和 `errno`；对于 `scall*` / `ccall*`，处理 `tpanic`、`terror`、`tfalse` 和 JSON 结果（见 §1.1 和 `com.h`）。
3. **线程安全：** libskin 没有**全面保证**。除非另有说明，假定**非线程安全**；对 mmap 的寄存器文件和共享资源使用 `register_lock` / 进程级锁定；`utility.h` shell/网络辅助函数通常调用子进程或全局状态。
4. **路径格式：** 组件路径使用 `project@component`；配置/属性路径通常使用 `level1/level2`（见 `path.h`、`OBJECT_CONFIG_GAPS`）。
5. **日志级别：** 为生产环境调整级别和输出选项；避免 verbose/debug 刷屏（见 §9.3.1 了解 `flags` 的组合）。

---

## 20. 相关文档

- Skin 库下的公共头文件 — 原型和 Doxygen 风格注释的权威来源
- `skin.h` — 伞形包含
- `skinhead.h` — 常量和组件名称宏
- `skinapi.h` — 便捷宏（`scalls`、`machine_*`、...）
- 可选的配套文档（如果在你的代码树中存在）：`COM_API.md`、`SKIN_API.md`

---

*文档版本：1.0*
*最后更新：2026-03-22*
