## 本文件说明组件接口文档的格式、章节结构与风格要求; 以下内容不写入各组件的正式文档

### 基本约定

1. 组件接口文档与组件代码目录通常位于同一项目目录内且路径上相邻 (同一父目录或项目约定的同级位置); 文档用于描述使用逻辑、对外配置与接口, 具体目录布局以仓库约定为准
2. 组件接口文档名称通常为 `<组件名称>.md`; 对应实现代码常见为与组件同名的目录, 或与组件同名的 shell 脚本文件, 若实际结构不同须在组件文档 Dependencies 或 Architecture 中说明
3. 定稿组件接口文档的版式字符须与 **`~/snake8/doc/dev/com/land/auth.md`** (以下简称 **auth 范例**) 一致; 本指导中的 **Architecture** / **Dependencies** / **Joint Events Hook** 及中部教学内容 **不在** auth 范例内, 定稿时勿输出 `//''` 与教学 `//'…'` 行
4. 以下各章节若组件无对应内容则整节省略, 勿保留空标题或无信息小节 (含 **Architecture** / **Dependencies** / **Joint Events Hook** / **Configuration reference** / **Configuration example** / **Configuration settings example** / **Published Joint Events** / **Other** 等可选节). 特别注意: 若组件无自身配置, 则 **Configuration reference** / **Configuration example** / **Configuration settings example** 三节**全部省略**, 不要写"本组件不使用配置"之类的说明文字
5. `<>` 内的占位须按组件实际信息填写; 定稿中勿保留 `<组件名称>` 等字面量, 须为真实名称 (全文统一用此占位表示该组件在标题、**Configuration reference**、命令与输出示例及文件名中的名称)
6. `//''` 内的内容是给编写接口文档的人或 AI 介绍如何编写此段内容, 不应出现在接口文档中
7. 虽然本指导为中英文混合, 实际编写的组件接口文档必须为全英文 (含章节标题、正文与表格); 指导中的中文及占位说明在定稿时须改写为英文
8. 接口文档不应过多描述组件内部实现, 专注于接口功能, 不应出现源代码文件名等信息
9. `get` / `set`: 用于对 **Configuration** 做查询或修改时会进入组件侧对应实现; 二者**不是**对使用方暴露的接口. 组件接口定稿中**不要**对这两个符号做任何专门描述
10. **`~/snake8/doc/dev/com/land/auth.md`** 为版式真源; 本指导所有规则均以该文件为最终参照. 仓库内若存在与之一致的副本, 以该路径所指文件为准

### 格式禁令 (定稿时严格遵守)

11. **禁止**把本指导中的教学内容 (含一切 `//''`、`//'…'` 等) 原样复制进组件文档
12. **禁止**使用 `#####` 等标题级作为单条 API 的标题样式; API 词条统一用 `+` / `-` 列表体例
13. **禁止**在大节之间使用 Markdown `---` 水平线分段 (与 auth 范例一致)
14. **禁止**添加 `#### Configuration attributes` 标题; **Configuration reference** 节下直接进入 `` ```json `` 配置块
15. **禁止**对 `get` / `set` 做任何专门描述

### 可选章节

16. **Architecture**、**Dependencies**、**Joint Events Hook**、**Concepts** 为可选章节: 仅在有对外架构说明、依赖契约、「订阅的 Joint 事件」或需要解释组件核心概念时撰写; 无则整节省略. **auth 范例未含** Architecture / Dependencies / Joint Events Hook / Concepts
17. **Concepts** 节用于在 **API Reference** 之前解释组件的核心概念、术语或工作原理 (如 fpk 的 Project 概念、FPK 概念、安装路径符号等); 有则置于 **Configuration reference** (或 **Overview**, 若无配置) 与 **API Reference** 之间; 无则整节省略


---

## 与 auth 范例逐项同构 (定稿强制)

以下「**须有**」指: 若省略可选三节, 其余须与 auth 范例 **同序、同结构、同版式字符**; 语义文本可替换, **标点与空格模式**优先照抄范例对应行.

### 章节顺序与标题字面量

| 顺序 | auth 范例行号 (约) | 定稿须出现的 Markdown 结构 | 备注 |
|------|---------------------|------------------------------|------|
| 1 | 1 | `## <组件标识> — <英文副标题>` | 使用 em dash `—`, 与范例相同 |
| 2 | 3 | `### Overview` | 后接英文段落 + `-` 列表; 子层用 **4空格 + `>`** 续行 |
| 3 | 16 | `### Configuration reference ( <组件标识> )` | 可选, 无配置则整节省略 (含 Configuration example / settings example 一并省略) |
| 4 | 18–62 | `` ```json`` … `` ``` `` | 块内第一行须为 `// Attributes introduction ` (行尾保留一个空格) |
| 5 | 64 | `#### Configuration example` | 小写 **example**, 无配置则省略 |
| 6 | 134 | `#### Configuration settings example` | 小写 **settings** / **example**, 无配置则省略 |
| 7 | — | `### Concepts` | 可选, 用于解释组件核心概念; 有则置于 Configuration (或 Overview) 与 API Reference 之间 |
| 8 | 156 | `### API Reference` | |
| 9 | 158 | `#### Management APIs` | |
| 10 | 167 | `#### Query APIs` | |
| 11 | 270 | `#### Control APIs` | |
| 12 | 347 | `### Published Joint Events` | 可选, 无事件则整节省略 |
| 13 | 357 | `### Other` | 可选, 无内容则整节省略 |

### 空行与段间距 (逐段对齐 auth 范例)

- `### Overview` 末项列表结束后 → `### Configuration reference`: **连续两个空行** (对齐 auth 12–15行). 若无配置且无 Concepts, 则 → `### API Reference` 同样**连续两个空行**; 若有 Concepts, 则 → `### Concepts` **连续两个空行**
- `### Configuration reference` 的 `` ```json `` 块结束后 → `#### Configuration example`: **一个空行** (对齐 auth 62–64行)
- `#### Configuration example` 的 `` ```shell `` 结束后 → `#### Configuration settings example`: **一个空行** (对齐 auth 132–134行)
- `#### Configuration settings example` 最后一组 `` ```shell `` 结束后 → `### API Reference`: **连续两个空行** (对齐 auth 152–157行)
- `### API Reference` 标题后 → `#### Management APIs`: **一个空行** (对齐 auth 157–158行)
- `#### Management APIs` 内最后一条 API 结束后 → `#### Query APIs`: **连续两个空行** (对齐 auth 164–167行)
- `#### Query APIs` 末条 API 的围栏结束后 → `#### Control APIs`: **连续两个空行** (对齐 auth 268–270行)
- `#### Control APIs` 末条 API 的围栏结束后 → `### Published Joint Events`: **连续两个空行** (对齐 auth 343–347行)
- `### Published Joint Events` 表格结束后 → `### Other`: **连续两个空行** (对齐 auth 353–357行)
- 若整节省略 **Joint Events Hook**, 则 `#### Control APIs` 结束后到 `### Published Joint Events` 的空行数仍须与 auth 一致

### Configuration reference 内 `` ```json `` 块 (临摹 auth 18–61行)

#### JSON 块整体结构

- 块内**第一行**须为 `// Attributes introduction ` (**`introduction` 后保留一个行尾空格**, 对齐 auth 19行)
- 仅允许**英文** `//` 行注释与 JSONC 结构; **禁止**出现 `//''` / `//'…'` 教学注释
- 配置 JSON 块为**排版用 JSONC / 伪代码**, 不是严格可 `JSON.parse` 的 JSON

#### 属性描述的注释格式

每条属性行的格式为:
```
    "属性名": "属性值描述",                    // [ 类型 ], 附加说明
```

**行尾类型注释**须与 auth 范例逐字符一致, 以下是 auth 中实际出现的所有类型注释模式:

| 模式 | 含义 | auth 范例行号 |
|------|------|---------------|
| `[ string ]` | 值为任意字符串 | 26, 41 |
| `[ number ]` | 值为数字 | 25, 28, 29, 52 |
| `[ "disable", "enable" ]` | 值为若干可选之一 (逗号后有空格) | 27 |
| `[ json ]` | 值为 JSON 对象 (仅用于顶层类型标注, 嵌套展开时用 `{ json }` ) | 21 |
| `[ string ]: { json }` | 键为字符串, 值为 JSON 对象 | 23, 39 |
| `[ string ]: [ "disable","enable" ]` | 键为字符串, 值为可选值 (逗号后**无**空格) | 33, 55 |

**重要**: 在配置 JSON 块内, 可选值列表 `[ "disable","enable" ]` 逗号后**无空格**; 在 API 返回 JSON 块内, 可选值列表 `[ "enable", "disable" ]` 逗号后**有空格** (对齐 auth 175行). 两种风格可并存, 以各自邻近行为准, 勿自行统一.

#### 属性值描述的位置

属性值描述写在值的位置 (引号内), 用英文短语说明该属性的含义:
```
    "user name":       // [ string ]: { json }, username in system
    "key":"username password",                              // [ string ], encrypt password, cannot be space
```

#### 续行注释 (解释可选值含义)

当属性值为可选值时, 在类型注释下方用续行注释解释每个值的含义:
```
    "group name":"belongs state"  // [ string ]: [ "disable","enable" ], group name the the username belongs
                                        // "enable" for belongs, "disable" for not
```
续行注释的缩进列须与上行类型注释的 `[` 对齐 (对齐 auth 34–35行).

#### 占位句式 (表示"有多少X就有多少属性")

用于表示列表/对象中属性数量不定的情况, 有以下两种:

```
    // "...":"..."  How many <类型> show how many properties     // 键值均为标量时
    // "...":{ ... }  How many <类型> show how many properties   // 值为对象时
```
(对齐 auth 35, 43, 46, 56, 59行)

#### 嵌套对象的缩进

值为 JSON 对象时, 内部属性再缩进一层 (通常4空格), 递归适用上述所有规则:
```
    "user":          // [ json ], username list
    {
        "user name":       // [ string ]: { json }, username in system
        {                      // username configure in this json
            "id":"username identify number",                        // [ number ], linux system user id number
            ...
        }
        // "...":{...}  How many username show how many properties
    }
```

#### 空值属性

当属性值为空字符串时表示使用默认值:
```
    "key":"In this domain specialized password"        // [ string ], an empty one represents the use of the default password
```
(对齐 auth 41行)

### `#### Configuration example` (临摹 auth 64–132行)

- 小节标题下一行: **英文** `Example, …` 起句 (首词 **Example** + 逗号 + 空格)
- `Example,` 行**不要**追加 `(HE / Shell; …)` 类括号说明 (auth 无此模式)
- 每个示例的格式:
  ```
  Example, <英文说明>
  ```
  (空行)
  ```shell
  <组件名称>
  {
      "<属性>":"<值>",                         # <该属性取此值的含义>
      ...
  }
  ```
- shell 块内: 命令与返回体之间无额外空行; 行尾 `#` 注释用于说明每个属性的含义
- **最后一行输出后紧跟** `` ``` `` (不要在 closing fence 前插入额外空行, 对齐 auth 131–132行)
- 示例应**尽量覆盖更多属性**, 以反映本组件配置全貌

### `#### Configuration settings example` (临摹 auth 134–152行)

必须至少包含**两种**常用形式:

**(1) 单属性设置** (对齐 auth 136–140行):
```
Example, <英文说明变更意图>
```
(空行)
```shell
<组件名称>:<path/to/attribute>=<value>
ttrue
```

**(2) 多属性合并设置** (对齐 auth 148–152行):
```
Example, merge set <英文说明>( include "<attr1>" "<attr2>" "<attr3>" )
```
(空行)
```shell
<组件名称>|{"<key>":{"<nested>":{"<attr1>":"<v1>","<attr2>":"<v2>","<attr3>":"<v3>"}}}
ttrue
```

- `ttrue` 返回值紧跟命令行输出, 中间无额外空行
- 说明文字须交代变更意图及对应 **Configuration reference** 中的哪些属性

### `### API Reference` 下各 API 词条 (临摹 auth 160–343行)

#### 通用格式

每条 API 的完整格式:
```
+ `<组件>.<api[ 参数, … ]>` **<英文功能简述>**   ← 注意行尾可能有空格
    - <参数名> ----------- [ <类型> ], <说明, required or optional>
    - failed return <tfalse 或 NULL>
    - succeed return <ttrue 或 [ json ] 或 [ string ] 等>
```

#### `+` 行格式细则

- 以 **`+`** 开头, 接**单个空格** + `` `组件.api[ … ]` `` + **单个空格** + `**英文粗体**`
- 若 auth 范例该条在 `**` 后还有**行末空格**再换行, 定稿须同样处理 (如 auth 169, 192, 211, 230, 272, 293, 314行 `**` 后可见空格); auth 160行 `setup[]` 无行尾空格
- **禁止**在 `+` 行使用 `*斜体*` 返回值描述; 包括 `setup[]` 在内, 所有 API 的 `+` 行只写 `**粗体功能描述**`, 返回值在 `-` 行中描述

#### 参数行格式细则

- **4空格缩进** + `-` + **单个空格** + 文本
- 参数名与 `[ type ]` 之间的 **`-` 字符个数** 以 auth 范例**同槽位 API** 为准照抄:
  - `username ----------- [ string ]` (11个 `-`, 对齐 auth 170行)
  - `domain ------------- [ string ]` (14个 `-`, 对齐 auth 169行)
  - `password --------------- [ string ]` (15个 `-`, 对齐 auth 195行)
  - **不同 API 的同一参数名可能对齐不同**, 以 auth 各 API 自身为准, 勿统一
- optional 参数在说明中标注 `optional`, 如: `domain ------------- [ string ], optional, specify a specific domain, default is common`
- `failed return` / `succeed return` 子行**无**行尾 `//` 注释

#### 返回值格式

- **ttrue/tfalse 类**: `- failed return tfalse` + `- succeed return ttrue` (对齐 auth 161–162行)
- **NULL + JSON 类**: `- failed return NULL` + `- succeed return [ json ], <说明>` (对齐 auth 171–172行)
- **NULL + 标量类**: `- failed return NULL` + `- succeed return [ string ], <说明>` 或 `[ number ]` (对齐 auth 364行)
- `error return terror` 行: 仅在 auth 范例对应 API 有时才加; auth 中大多数 API 无此行

#### 返回值类型判断规则 (`[ string ]` vs `[ number ]`)

判断字段类型时, 以**语义意图**为准, 不以源码存储方式为准:
- **用 `[ number ]`**: 字段语义上是数字 — 计数器、固定单位的数值 (kB, jiffies, 端口号, PID, 百分比数值等)、ID 号
- **用 `[ string ]`**: 字段语义上是文本 — 名称、描述、路径、带单位的字符串 (如 "256M", "50%", "3d 2h 15m")、枚举值
- 源码用 `json_set_string` 存储数字并不意味着类型一定是 `[ string ]`; 若该字段的意图就是数字, 文档中应写 `[ number ]`

#### `+` 行可选参数格式

- **禁止**在 `+` 行中用 `[]` 包裹任何参数 (无论是 `[param]` 还是 `[[param]]`); 所有参数直接写裸名, 在 `-` 行中标注 `optional`
- 若 API 可以无参数调用, `+` 行写 `api` 即可, 不要写 `api[]` (除非源码中确实无参数, 如 `setup[]`)
- 若所有参数都是可选的 (如 `list` 可无参调用返回全部), `+` 行写 `list[ group ]`, 在 `-` 行中标注 `optional`
- 正确示例: `register[ object, ttydev, devcom, drvcom ]`、`restart[ delay, key ]`、`list[ group ]`
- 错误示例: `register[ object, [ttydev], [devcom], [drvcom] ]`、`restart[ [delay], [key] ]`、`list[ [group] ]`

#### 成功返回为 JSON 对象时的展开 (强制)

当成功返回为 JSON 对象时, **必须**在 `- succeed return [ json ], …` 后展开 JSON 结构:
1. 空一行
2. **4空格** + `` ```json ``
3. JSON 对象内容, 字段级 `//` 注释规则同 Configuration reference JSON
4. `` ``` ``

**禁止**省略 JSON 注解块。不得用"同 X 接口"或"参见 X"代替 — 即使两个接口返回相同结构, 也必须各自写出完整的 JSON 注解块。JSON 字段描述是接口文档的核心内容, 省略等于文档无效。

示例 (对齐 auth 173–180行):
```
    - succeed return [ json ], all domain "username" belong in the json
    ```json
    {
        "domain name": "enable or disable"  // [ string ]: [ "enable", "disable" ], domain name in system
                                                    // "enable": have this permissions
                                                    // "disable": no this permissions
        // "...":"..."  How many domain belongs show how many properties
    }
    ```
```

#### API 示例格式

- `Example, <英文说明>` → 空行 → `` ```shell `` → 命令 + 返回 → `` ``` ``
- `Example,` 行**不要**追加 `(HE / Shell; …)` 类括号说明 (auth 无此模式)
- 示例中组件 API 的调用格式: `land@<组件>.<api[ … ]>` (对齐 auth 184行)
- 两个 Example 之间是否空行, 以 auth 范例对应 API 为准 (如 `modify` 两示例间有空行, 对齐 auth 285–287行)

#### API 分类规则

- **Management APIs**: 侧重组件生命周期与系统调度 (setup, shut, service, online, offline 等)
- **Query APIs**: 面向业务数据的只读查询 (list, domain, check, match, status, info 等)
- **Control APIs**: 会改变运行状态或持久化配置的操作 (add, delete, modify, change, reset 等)
- 同一符号因参数不同表现为查询或控制时, 在 Query 和 Control 各列一条, 各自只描述该语义下的参数

### `### Published Joint Events` (临摹 auth 347–354行)

- 表格前**必须**有一段英文说明, 结构为: 叙述 + `**<joint 注册名>**` (对齐 auth 349行)
- 表格头两行字面量须字符级一致:
  ```
  | Event | Description |
  |-------|-------------|
  ```
- 数据行用反引号包裹事件名, 单元格内容用英文描述事件触发时机与参数含义 (对齐 auth 353行)

### `### Other` (可选, 临摹 auth 357–416行)

- 节名须为 `### Other`; 与 **Published Joint Events** 之间的空行数仍临摹 auth
- 用途: 收纳不适合放入前面任一节的内容 (含无法归入 Management/Query/Control 的 API)
- 若写 API, 仍须使用与 **API Reference** 相同的 `+` / `-` 体例与缩进规则
- **不要**机械照搬 auth 范例里的固定开场句 *Some helpful APIs* 或其实例 API 列表
- 无内容则**整节省略**


---

## English checklist for AI (copy-paste)

Use this block as instructions to an AI. The **final component document** must be **all English** (headings, body, tables). Treat **`~/snake8/doc/dev/com/land/auth.md`** as the layout ground truth. **Optional sections** not in auth: `### Architecture`, `### Dependencies`, `### Joint Events Hook`, `### Concepts` — omit entirely when unused. **Configuration sections** (`### Configuration reference`, `#### Configuration example`, `#### Configuration settings example`) — omit ALL three when the component has no configuration; do not write "this component has no configuration" text.

### One-line prompt (optional)

Write the component interface document in English only. Follow `project/tmptools/component.md` (this file) for rules, and match **`~/snake8/doc/dev/com/land/auth.md`** section-for-section for everything auth contains: same Markdown heading **literals**, same **blank-line counts** between matching blocks, same `+` / `-` API layout, same nested `` ```json`` / `` ```shell`` **indentation** (four spaces before nested fences where auth does), same table header **character lines**, and the same **per-line punctuation and spacing** (including the hyphen run length in lines like `- username ----------- [ string ]`, and any **trailing spaces after `**`** on a `+` line if the closest auth analogue has them). **Exception — `### Other`:** this section holds material that does **not** fit any earlier section (including APIs that cannot reasonably be placed under Management / Query / Control). Do **not** require the literal lead-in *Some helpful APIs* or auth's specific utility-API list; if you document APIs here, still use the same `+` / `-` style as **API Reference**. Do **not** emit `//''` or teaching `//'…` lines. Do **not** paste the teaching content from `component.md`; build the Configuration `` ```json`` block by hand so its first line is exactly `// Attributes introduction ` (note the **single trailing space** after `introduction`, as in auth). Do **not** add `#### Configuration attributes`. Do **not** use `#####` for APIs. Do **not** place markdown `---` between major sections if matching the current auth layout. Do **not** document `get` / `set` as user-facing APIs.

### Section order (each must appear when the component has that content)

1. `## <component-id> — <English subtitle>` — use em dash `—` as in auth.
2. `### Overview` — English paragraphs, blank lines, `-` bullets; continuation lines use **four spaces + `>`** as in auth (4-space indent then `>` then space then text).
3. `### Configuration reference ( <component-id> )` — **exactly one ASCII space** after `(` and **one** before `)`; no angle brackets in the shipped file. **Omit entirely if the component has no configuration** (together with items 4, 5, 6 below).
4. A `` ```json`` fence whose first inner line is exactly `// Attributes introduction ` (with trailing space). English `//` comments only inside the block. **Omit if no configuration.**
5. `#### Configuration example` — lowercase **example** in the heading. **Omit if no configuration.**
6. `#### Configuration settings example` — lowercase **settings** and **example** in the heading. **Omit if no configuration.**
7. `### Concepts` — optional; use when the component has core concepts, terminology, or working principles that need explanation before the API Reference. Place between Configuration (or Overview if no configuration) and API Reference. Omit entirely if not needed.
8. `### API Reference`
9. `#### Management APIs`, `#### Query APIs`, `#### Control APIs` — preserve **blank-line counts** between these blocks as in auth.
10. `### Published Joint Events` — include an **English paragraph before the table** (same structural pattern as auth). Table header lines must match **character-for-character**:
   - `| Event | Description |`
   - `|-------|-------------|`
   - **Omit entirely if the component publishes no joint events.**
11. `### Other` — heading literal **`### Other`** and the same blank-line pattern before it as in auth. **Content is not cloned from auth:** use this section only for material that does **not** fit any earlier section (including APIs that cannot reasonably live under Management / Query / Control). If you list APIs here, use the same `+` / `-` style as **API Reference**. Do **not** require the literal line *Some helpful APIs* or auth's example utility APIs. Omit the whole section if empty.

### Blank-line targets (re-check against the current auth.md)

- After the last content of `### Overview`: **two** blank lines before `### Configuration reference` (or before `### Concepts` or `### API Reference` if Configuration is omitted).
- After the closing fence of the `` ```json`` block: **one** blank line before `#### Configuration example`.
- After the closing fence of the last `#### Configuration example` sample: **one** blank line before `#### Configuration settings example`.
- After the closing fence of the last `#### Configuration settings example` sample: **two** blank lines before `### Concepts` or `### API Reference`.
- After `### Concepts` content (if present): **two** blank lines before `### API Reference`.
- After the `### API Reference` heading: **one** blank line before `#### Management APIs`.
- After the last `+` API under `#### Management APIs`: **two** blank lines before `#### Query APIs`.
- After the last `+` API under `#### Query APIs`: **two** blank lines before `#### Control APIs`.
- After the last `+` API under `#### Control APIs`: **two** blank lines before `### Published Joint Events` (keep this even when **Joint Events Hook** is omitted).
- After the **Published Joint Events** table: **two** blank lines before `### Other`.

### Configuration JSON block rules

- First line inside the fence: `// Attributes introduction ` (trailing space, as auth line 19).
- Each attribute line: `    "name": "description",                   // [ type ], additional info`
- For objects (nested JSON): use `[ string ]: { json }` notation (NOT `[ json ]`), as auth lines 23, 39.
- For optional-value enums: `// [ "disable","enable" ]` with **no space after comma** in config JSON (as auth lines 33, 55); use `// [ "enable", "disable" ]` **with space after comma** in API return JSON (as auth line 175). Both styles coexist; mirror the neighbour line you are imitating.
- Continuation comments explaining enum values: indent to align with the `[` in the type annotation above (as auth lines 34–35, 40–42). The continuation comment must use `//` and start at the same column as the `[` in the line above.
- Placeholder lines for variable-count properties: `// "...":"..."` or `// "...":{...}` (as auth lines 35, 43, 46, 56, 59).
- Empty-string values represent "use default": `"key":""` with comment explaining (as auth line 41).

### Configuration samples (`#### Configuration example` / `#### Configuration settings example`)

- Each sample: one line starting with `Example, ` (capital **E**, comma, space), then a blank line, then `` ```shell``.
- Do **not** put `(HE / Shell; …)` on the `Example,` line.
- Do **not** insert an extra blank line before the closing `` ``` `` that immediately follows the last line of shell output.
- In `#### Configuration example`: show a full configuration query with as many attributes as practical, each with `#` inline comments.
- In `#### Configuration settings example`: show at least two forms — single attribute set and merge set (multiple attributes in one command).

### API Reference layout

- Each API begins with `+`, one space, `` `component.api[ … ]` ``, one space, `**one-line English summary**`. Do **not** add `*italic return description*` after `**` — return values are described in the `-` lines below.
- Some `+` lines have **trailing spaces** after the closing `**` before the newline (as auth lines 169, 192, 211, 230, 272, 293, 314); others do not (as auth line 160). Mirror the closest auth analogue.
- Parameter and return lines: indent with **four spaces**, then `-`, then one space, then text. **No** trailing `//` on those `-` lines.
- Copy **hyphen run length** between the parameter label and `[ type ]` from the **closest matching pattern in auth** for that slot; do not normalize different APIs to the same run length.
- For JSON-shaped success: a `- succeed return [ json ], …` line, a blank line, then **four spaces + `` ```json``**, then the object with field-level `//` lines. **This JSON annotation block is MANDATORY** — never omit it, never replace with "same as X" or "see X". Even if two APIs return the same structure, each must have its own JSON block.
- When choosing `[ string ]` vs `[ number ]` for field types, analyze the **semantic intent** not the source code storage method. Counters, kB values, port numbers, PIDs, and fixed-unit numeric values → `[ number ]`. Names, paths, descriptions, values with mixed units like "256M" or "50%" → `[ string ]`.
- In the `+` line, do **not** wrap any parameter in `[]` — write all parameters as bare names: `api[ param1, param2, param3 ]`. Mark each optional one in its `-` line. If the API can be called with zero arguments, write `api` (no brackets) unless the source code explicitly shows `[]` like `setup[]`. Wrong: `api[ param1, [param2] ]`. Right: `api[ param1, param2 ]`.
- In shell examples, `#` inline comments should be **aligned to the same column** using spaces (as auth lines 68–131, 186–189). This applies to both Configuration example blocks and API example blocks.
- `error return terror`: only add this line if the closest auth analogue has it; most auth APIs do not.
- API classification: lifecycle/scheduling → **Management**; read-only queries → **Query**; state-changing operations → **Control**.

### Published Joint Events

- Keep the **intro paragraph + two-column table** pattern from auth. Replace `land@joint` with the real joint registration name.
- Use backticks for event names in the table as auth does for `auth/modify`.

### Hard bans (final doc)

- No `#### Configuration attributes`.
- No `#####` per-API headings.
- No `//''` or teaching `//'…` lines.
- No markdown `---` between major sections.
- No dedicated description of `get` / `set` as user APIs.
- No `[ json ]` notation for objects — use `{ json }`.
- No "this component has no configuration" text — just omit the Configuration sections.
- Shell example `#` comments must be aligned to the same column using spaces.
- No omitting JSON annotation blocks for APIs that return JSON — every `- succeed return [ json ]` line MUST be followed by a ````json` block with field-level `//` annotations.
- No `[param]` or `[[param]]` brackets around individual parameters in the `+` line — write `api[ param1, param2, param3 ]` with all parameters bare, and mark each optional one in its `-` line with `optional`. Example: `register[ object, ttydev, devcom, drvcom ]` not `register[ object, [ttydev], [devcom], [drvcom] ]`.
- No using `[ string ]` for semantically numeric fields (counters, kB, ports, PIDs) — use `[ number ]`.
- No `*italic*` return descriptions after `**` in the `+` line — return values go in the `-` lines only.


---

## 组件接口文档模板

### `<组件名称> — <简单介绍>`

```
## <组件名称> — <简单介绍>
```

### Overview

```
### Overview

<英文段落: 面向使用者说明组件解决什么问题、不负责什么>

- <特性或能力1>
- <特性或能力2>
    > <补充说明, 4空格缩进 + > + 空格 + 文本>
```

### Architecture (可选)

```
### Architecture

<英文段落: 系统级职责与主要数据去向的概括, 控制在少量段落或短列表>
```

### Dependencies (可选)

```
### Dependencies

<英文段落: 列出对外协作方及依赖方向>
```

### Concepts (可选)

```
### Concepts

**<概念名1>**
* <概念说明1>
* <概念说明2>

**<概念名2>**
* <概念说明>
```

### Configuration reference

```
### Configuration reference ( <组件名称> )

```json
// Attributes introduction 
{
    "属性1":              // [ 类型 ], 属性说明
    {
        "嵌套属性":       // [ string ]: { json }, 说明
        {
            "字段": "值描述",      // [ 类型 ], 说明
            // "...":"..."  How many <X> show how many properties
        }
    },
    "可选值属性": "值描述",        // [ "value1", "value2" ], 说明
                                       // "value1": 当为此值时的作用
                                       // "value2": 当为此值时的作用
}
```
```

### Configuration example

```
#### Configuration example

Example, show all the <component> configure
```shell
<组件名称>
{
    "属性1":                          # 属性1的含义
    {
        "字段":"值",                      # 字段的含义
        ...
    }
}
```
```

### Configuration settings example

```
#### Configuration settings example

Example, <单属性变更的英文说明>
```shell
<组件名称>:<path/to/attribute>=<value>
ttrue
```

Example, <另一单属性变更的英文说明>
```shell
<组件名称>:<path/to/attribute>=<value>
ttrue
```

Example, merge set <英文说明>( include "<attr1>" "<attr2>" "<attr3>" )
```shell
<组件名称>|{"<key>":{"<nested>":{"<attr1>":"<v1>","<attr2>":"<v2>","<attr3>":"<v3>"}}}
ttrue
```
```

### API Reference — Management APIs

```
#### Management APIs

+ `<组件>.setup[]` **initialize the <component>**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation
```

### API Reference — Query APIs

```
#### Query APIs

+ `<组件>.<api[ 参数名 ]>` **<英文功能简述>**
    - <参数名> ----------- [ string ]
    - failed return NULL
    - succeed return [ json ], <返回对象说明>
    ```json
    {
        "字段名": "值"  // [ string ]: [ "enable", "disable" ], <字段说明>
                                // "enable": <说明>
                                // "disable": <说明>
        // "...":"..."  How many <X> show how many properties
    }
    ```

    Example, <英文说明>
    ```shell
    land@<组件>.<api[ 参数值 ]>
    {
        "字段":"值"       # 说明
    }
    ```
```

### API Reference — Control APIs

```
#### Control APIs

+ `<组件>.<api[ 参数, … ]>` **<英文功能简述>**
    - <参数1> --------------- [ string ], <说明>
    - <参数2> --------------- [ string ], <说明>
    - <新参数> ------------- [ string ], optional, <说明>
    - failed return tfalse
    - succeed return ttrue

    Example, <英文说明>
    ```shell
    land@<组件>.<api[ 参数值, … ]>
    ttrue
    ```
```

### Published Joint Events

```
### Published Joint Events

The following joint events are published when <触发范围的英文说明>. Other components can subscribe at runtime (joint registration / **<land@joint>**).

| Event | Description |
|-------|-------------|
| `<event/name>` | <英文: 触发时机、关联API、参数含义> |
```

### Other (可选)

```
### Other

<可选: 一两句英文说明本节内容>

+ `<组件>.<api[ … ]>` **<英文功能简述>**
    - <参数/返回行, 同 API Reference 体例>
```
