## 本文件说明组件接口文档的格式、章节结构与风格要求; 以下内容不写入各组件的正式文档

1. 组件接口文档与组件实现通常位于同一项目内且路径上相邻 (同一父目录或项目约定的同级位置); 文档用于描述使用逻辑、对外配置与接口, 具体目录布局以仓库约定为准
2. 文档名称通常为 <组件名称>.md; 对应实现代码常见为与组件同名的子目录, 或与组件同名的 shell 脚本文件, 若实际结构不同须在组件文档 Dependencies 或 Architecture 中说明
3. 所有组件接口文档的风格及章节必须与此指导文档一致, 包括缩进方式也必须一致
4. 以下各章节若组件无对应内容则整节省略, 勿保留空标题或无信息小节
5. <> 内的占位须按组件实际信息填写; 定稿中勿保留 `<组件名称>` 等字面量, 须为真实名称 (全文统一用此占位表示该组件在标题、Configuration Reference、命令与输出示例及文件名中的名称; 若项目里简称与全称不同, 仍填真实名, 勿留尖括号占位)
6. //'' 内的内容是给编写接口文档的人或 AI 介绍如何编写此段内容, 不应出现在接口文档中
7. 虽然本指导为中英文混合, 实际编写的组件接口文档必须为全英文 (含章节标题、正文与表格); 指导中的中文及占位说明在定稿时须改写为英文
8. **Configuration attributes** 里那种带 `json` 标记的片段只是**排版用的示意**（可含 // 注释、占位的属性与值）, 属 JSONC / 伪代码, 不是严格可 `json.parse` 的 JSON; 与下文**通过命令行查询本组件配置的示例**不是同一类内容
9. **Joint Events Hook** 与 **Published Joint Events** 中的表格仅为列数示例; 事件参数或字段多于表头时需扩展列或在单元格内用列表/附段说明. 组件文档定稿时这两节须按 §7 全英文撰写 (表头与单元格); 勿把本指导里仅用于说明的 `//''` 整句复制进组件文档
10. **`get` / `set`**: 用于对 **Configuration** 做查询或修改时会进入组件侧对应实现; 二者**不是**对使用方暴露的接口. 组件接口定稿中**不要**对这两个符号做任何专门描述
11. 接口文档不应过多描述组件内部实现, 专注于接口功能, 而不是描述内部实现, 不应出现源代码文件名等信息

---



## <组件名称> — <简单介绍>

### Overview

//'定稿英文: 面向**使用者**说明「这个组件解决什么问题、不负责什么」, 用能力与场景概括即可; 避免从源码复述初始化顺序或文件名'
<概述组件职责、能力边界与主要特性>


### Architecture

//'定稿英文: **系统级**职责与主要数据去向 (配置、状态、事件) 的**概括**, 控制在少量段落或短列表; **禁止**把 **Architecture** 写成实现 walkthrough (逐函数、逐文件、逐寄存器写入顺序)'
<说明组件在系统中的职责、设计要点及与上下层的关系, 并界定对外暴露的核心概念>


### Dependencies

//'定稿英文: 列出**对外协作方** (其它组件、子系统、运行环境前提) 及**依赖方向**; 用产品或配置名指称即可; 避免罗列内部头文件或实现源文件, 除非该依赖本身就是集成契约 (例如必须链接的 SDK)'
<列出依赖与关联: 上下层组件、同类型多实例、功能相近组件等, 并说明依赖方向或协作关系>


---



### Configuration Reference ( <组件名称> )

#### Configuration attributes

```json
// 
{

    //'配置项类型多样, 须先判定**属性名**与**值**的形态类别, 再按下列约定书写; 若值为 JSON 对象, 其内部可继续嵌套, 递归适用同类规则'


    //'1, 属性名固定的属性, 即属性名由组件代码约定不可变'

        //'a. 可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性名由组件代码约定不可变, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",             // [ "<value>", "<value2>", "<value3>", ... ]
                                                                      // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                      // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                      // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                      // <介绍默认值>

        //'b. 字符串值属性: 当属性值是字符串时使用此方式描述, 属性名由组件代码约定不可变, 属性值为任意字符串'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ string ], <attribute value的附加介绍, 如默认值等>

        //'c. 数字值属性: 当属性值是数字时使用此方式描述, 属性名由组件代码约定不可变, 属性值为数字'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>

        //'d. IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性名由组件代码约定不可变, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ ip address ], <attribute value的附加介绍, 如默认值等>

        //'e. MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性名由组件代码约定不可变, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ mac address ], <attribute value的附加介绍, 如默认值等>

        //'f. JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性名由组件代码约定不可变, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute>": { JSON },                                        // [ json ], <这个 JSON 的概要性介绍>


    //'2, 可选名属性, 即属性名只有几个可选项, 通常这种属性存在于一个列表中, 最常用于接口列表的属性介绍, 这种情况下接口名称会作为属性的可选名'

        //'a. 可选名可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ "<name>", "<name2>", "<name3>", ... ]:  [ "<value>", "<value2>", "<value3>", ... ]
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                            // <...>
                                                                            // <介绍默认值或其他附加介绍>

        //'b. 可选名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ "<name>", "<name2>", "<name3>", ... ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>

        //'c. 可选名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ "<name>", "<name2>", "<name3>", ... ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>

        //'d. 可选名IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ "<name>", "<name2>", "<name3>", ... ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>

        //'e. 可选名MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ "<name>", "<name2>", "<name3>", ... ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>

        //'f. 可选名JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性名为上文所列可选名之一, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ "<name>", "<name2>", "<name3>", ... ]: [ json ], <这个 JSON 的概要性介绍>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍>
                                                                       // <...>


    //'3, 字符串名属性, 即属性名是任意字符串时使用此方式描述, 通常这种属性存在于一个列表中, 最常用于规则名, 这种情况下规则名可以是任意的字符串'

        //'a. 字符串名可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ string ]: [ "<value>", "<value2>", "<value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                            // <...>
                                                                            // <介绍默认值或其他附加介绍>

        //'b. 字符串名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ string ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. 字符串名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ string ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. 字符串名IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ string ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. 字符串名MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ string ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. 字符串名JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ string ]: [ json ], <这个 JSON 的概要性介绍>
                                                                       // <attribute name的附加介绍>


    //'4, 数字名属性, 即属性名是数字时使用此方式描述, 通常这种属性存在于一个列表中, 最常用于有顺序的规则名, 这种情况下规则名可以是数字'

        //'a. 数字名可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ number ]: [ "<value>", "<value2>", "<value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                            // <...>
                                                                            // <介绍默认值或其他附加介绍>

        //'b. 数字名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ number ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. 数字名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ number ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. 数字名IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ number ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. 数字名MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ number ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. 数字名JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ number ]: [ json ], <这个 JSON 的概要性介绍>
                                                                       // <attribute name的附加介绍>


    //'5, IP地址属性, 即对象键为 IP 地址时使用此方式描述, 常见于以地址为键的规则表或映射类配置块'

        //'a. IP地址名可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ "<value>", "<value2>", "<value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                            // <...>
                                                                            // <介绍默认值或其他附加介绍>

        //'b. IP地址名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. IP地址名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ ip address ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. IP地址名IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ ip address ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. IP地址名MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ ip address ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. IP地址名JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ ip address ]: [ json ], <这个 JSON 的概要性介绍>
                                                                       // <attribute name的附加介绍>



    //'6, MAC地址属性, 即对象键为 MAC 地址时使用此方式描述, 常见于以地址为键的规则表或映射类配置块'

        //'a. MAC地址名可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性值约定为若干可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ mac address ]: [ "<value>", "<value2>", "<value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍>
                                                                            // <...>
                                                                            // <介绍默认值或其他附加介绍>

        //'b. MAC地址名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ mac address ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. MAC地址名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ mac address ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. MAC地址名IP地址值属性: 当属性值是 IP 地址时使用此方式描述, 属性值为 IP 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ mac address ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. MAC地址名MAC地址值属性: 当属性值是 MAC 地址时使用此方式描述, 属性值为 MAC 地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",         // [ mac address ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. MAC地址名JSON值属性: 当属性值是 JSON 时使用此方式描述, 属性值为 JSON, 支持嵌套, 内层可继续包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ mac address ]: [ json ], <这个 JSON 的概要性介绍>
                                                                       // <attribute name的附加介绍>

}
```

#### Configuration Example

//'**本节提供通过命令行（HE / Shell）查询本组件配置的示例**（具体语法以项目为准）. **要求**: (1) 采用项目中**常用**的查询方式; (2) 示例输出中**每个属性及其取值**均须可理解——与 **Configuration attributes** 已有条目一致, 或于行尾 **#** 后注明**该属性取该值的含义**（组件定稿语言见 §7）; (3) **尽量覆盖更多属性**, 以反映本组件配置全貌, 避免仅列少数条目; 单条命令展示不全时, 可再换一种查询方式补全. 定稿英文标题: **Configuration Example**'

```shell
# Query configuration (HE / Shell; project-specific syntax)
<组件名称>
{
    "<attribute>":"<value>",                    # <what this attribute means at this value>
    "<attribute2>":"<value2>",                    # <...>
    # <one line per attribute you show; include as many as practical>
}

```

#### Configuration write example (mandatory)

//'**本节提供通过命令行（HE / Shell）修改本组件配置的示例**（具体语法以项目为准）. **必须**至少**两种**常用形式: 下列「单属性赋值」与「多属性一次写入」各示一组; 另有惯用形式可继续追加. 各组示例**紧接的说明文字**须交代变更意图及与 **Configuration attributes** 中**哪些属性**对应（组件定稿语言见 §7）'

```shell
# Change one configuration attribute (HE / Shell; project-specific syntax)
<组件名称>:<attribute>=<attribute value>
```

```shell
# Change several configuration attributes at once (HE / Shell; project-specific syntax)
<组件名称>|{"<attribute>":"<attribute value>","<attribute2>":"<attribute2 value>","<attribute3>":"<attribute3 value>"}
```



---



### API Reference

//'**JSON 对象返回值与 Configuration attributes 同构**: 仅当成功分支为 **JSON 对象** 时适用; 须在 `succeed return json { ... }` 内逐字段按 **Configuration attributes** 规则书写 (字段级 typing + 注释、对使用者的语义). **标量 / 字符串 API** 的成功返回值**不是** JSON 对象, **禁止**用空 JSON 块或伪字段表描述, 成功分支用**一行**写清 **[ string ]** 或 **[ number ]** 等形态即可 (见下模板)**

//'通过返回值对 API 分为**四类**, 以下分别说明描述格式'
//'**boole_t API**: 通常返回 **ttrue** / **tfalse**, 或 **terror**'
//'**talk_t JSON API**: 成功时返回 **JSON 对象** (或平台约定的 JSON talk); 失败 **NULL**; 可 **terror**; 成功对象须按上文「同构」规则展开'
//'**标量 / 字符串 API**: 成功时返回**单一标量 talk**（**不是** JSON 对象）, 文档中用 **[ string ]** 或 **[ number ]**（等与实现对齐的**一种**形态）描述即可; 失败 **NULL**; 可 **terror**. 例如仅返回一段文本 token、或仅返回一个整数计数等查询（后者文档标 **[ number ]**）'
//'**复合类 API**: 依具体接口在 **ttrue** / **tfalse** / **NULL** / **JSON** / **terror** 等分支中择一, 互斥分支勿写成同一次调用并存'

//'Management、Query、Control 各子节中「单条 API」采用相同描述体例; 完整模板仅在 **Management APIs** 给出一次, Query 与 Control 仅界定收录范围, 不重复展开模板'
//'上文的 boole_t、talk_t JSON、标量/字符串、复合类 指返回值形态, 与三个子节名称 (Management / Query / Control) 正交: 任一子节下的接口均可能是其中任一类, 书写时以 **Management APIs** 节中的模板为准'

//'**Management APIs**: 侧重组件生命周期与系统调度; 收录管理用或系统调用的 API, 例如 setup, shut, service, online, offline'
//'**Query APIs** / **Control APIs**: 面向业务数据与运行参数 — 只读查询归入 Query, 会改变运行状态或持久化配置的归入 Control; 生命周期与系统调度类接口不归入上述两节, 应归入 **Management APIs**'
//'请勿将同一业务接口同时列入 Management 与 Query/Control (生命周期/系统调度与查询、控制职责相分离)'
//'若实现为同一符号 (同一函数或命令), 仅因参数组合不同而分别表现为查询或控制 (例如某参数缺省为查询、指定后为控制), 须在 Query 与 Control 各列一条: 各条仅描述该语义下的参数与行为, 通过参数约定体现差异; 两节出现相同 API 名称属正常体例, 不构成不当重复'
#### Management APIs

//'以下为单条 API 通用模板 (适用于 Management、Query、Control; 按实际返回值选用 boole_t / talk_t JSON / 标量/字符串 / 复合类 之一)'
//'参数行中的 [ <string> ] 仅为示例, 实际参数类型可为 number、JSON 字符串等, 与实现一致即可'

//'boole_t API 的描述格式'
##### `<API name[ argument, argument2, argument3, ... ]>` - <API介绍>
    - <argument> ---------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument2> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument3> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <...>
    - failed return <tfalse>
    - succeed return <ttrue>
    - error return <terror> //'若适用则列出, 否则省略


//'talk_t JSON API 的描述格式 (成功体为 JSON 对象)'
##### `<API name[ argument, argument2, argument3, ... ]>` - <API介绍>
    - <argument> ---------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument2> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument3> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - failed return NULL
    - error return <terror> //'若适用则列出, 否则省略
    - succeed return json
    {
        //'**强制**: 描述方式与 **Configuration attributes** 完全相同 (字段级 typing + 注释行); 见 **API Reference** 节首「JSON 对象返回值与 Configuration attributes 同构」; 本节占位不得理解为可省略子字段'
    }


//'标量 / 字符串 API 的描述格式 (成功时为**单个**非 JSON 值, 如一段文本或一个计数; 与 talk_t JSON 区分)'
##### `<API name[ argument, argument2, argument3, ... ]>` - <API介绍>
    - <argument> ---------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument2> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument3> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - failed return NULL
    - error return <terror> //'若适用则列出, 否则省略
    - succeed return [ string ] 或 [ number ]（与实现一致，择一）, <单一返回值对使用者的语义、取值范围或编码说明> //'勿使用 `succeed return json { }` 描述此类接口'


//'复合类 API 的描述格式'
//'下列返回情形按具体 API 只取其一, 勿将互斥分支写成同一调用会同时发生'
##### `<API name[ argument, argument2, argument3, ... ]>` - <API介绍>
    - <argument> ---------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument2> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - <argument3> --------- [ <string> ], <参数介绍, 是否必需项> //'若适用则列出, 否则省略
    - failed return <tfalse>
    - succeed return <ttrue>
    - failed return NULL
    - error return <terror> //'若适用则列出, 否则省略
    - succeed return json
    {
        //'**强制**: 描述方式与 **Configuration attributes** 完全相同 (字段级 typing + 注释行); 见 **API Reference** 节首「JSON 对象返回值与 Configuration attributes 同构」; 本节占位不得理解为可省略子字段'
    }

    Example, <调用目的说明>
    ```shell
    <调用 API 的命令>
    {
        //'**强制**: 返回值字段级描述与 **Configuration attributes** 及 talk_t JSON 成功分支相同; 不得仅复述「返回 JSON」'
    }
    ```


//'Query APIs 中列出用于查询组件信息或状态的 API, 如 status, info, list 等, 具体以对应实现为准'
//'与 Control 共用同一符号时: 本节条目仅描述查询语义下的参数形态与含义; 控制语义在 **Control APIs** 中另列一条, 通过两节中的参数约定区分 (无需合并为单条)'
#### Query APIs

//'本节不重复模板: 每条 API 的书写格式与 **Management APIs** 节中 boole_t、talk_t JSON、标量/字符串、复合类 四类模板相同, 按该 API 实际返回值选用对应模板即可'

//'Control APIs 中列出用于控制组件行为、配置或运行状态的 API, 如 modify, change, reset, stop 等, 具体以对应实现为准'
//'与 Query 共用同一符号时: 本节条目仅描述控制语义下的参数形态与含义; 查询语义在 **Query APIs** 中另列一条, 通过两节中的参数约定区分 (无需合并为单条)'
#### Control APIs

//'本节不重复模板: 每条 API 的书写格式与 **Management APIs** 节中 boole_t、talk_t JSON、标量/字符串、复合类 四类模板相同, 按该 API 实际返回值选用对应模板即可'



### Joint Events Hook

//'表格列扩展与定稿英文要求见本指导文首第 9 条'

| Event | Description |
|-------|-------------|
| `<Joint 事件名称>` | <触发组件作出什么操作> |
| `<逐行列出本组件注册或订阅的 Joint 事件>` | <同上：说明订阅后组件作出的操作或用途> |



### Published Joint Events

//'表格列扩展与定稿英文要求见本指导文首第 9 条'

| Event | When | Argument | Argument2 | Argument3 |
|-------|------|----------|-----------|-----------|
| `<Joint 事件名称>` | <说明触发该事件的原因或目的> | <第一个事件参数的含义, 无则留空> | <第二个事件参数的含义, 无则留空> | <第三个事件参数的含义, 无则留空> |
| `<逐行列出本组件对外发布的 Joint 事件>` | | | | |


### Other

<不便归入上述章节但对使用者仍属必要的信息>
