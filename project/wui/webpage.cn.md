# 编写组件管理网页指南

本文档指导如何为设备组件编写 Web 管理页面。管理页面运行在 **ace** 框架（基于 Bootstrap 的单页应用）中，通过 AJAX 加载 HTML 片段，以 hash 路由切换页面。每个页面对应一个或多个**组件对象**（如 `clock@restart`、`ifname@lan`），页面通过 `he.js` 与设备通信来读写组件的 JSON 配置、调用组件方法。

**核心思路**：阅读组件的 markdown 文档 &rarr; 理解其 JSON 配置结构 &rarr; 将配置属性映射为表单控件 &rarr; 用 `he.js` 读取/保存配置。

---

## 1. 准备工作：阅读组件文档

每个组件都有一份 markdown 文档（如 `ifname/lan.md`、`clock/restart.md`），编写管理页面前需要重点关注两个章节：

### Configuration 章节

此章节描述组件的 JSON 配置结构，每个属性就是一个可管理的字段。例如 `clock@restart` 的配置：

```json
{
    "mode":"Restart plan mode",       // select 下拉框
    "age":"The maximum runtime",      // text 输入框（数字）
    "point_hour":"...",               // select 下拉框（0-23）
    "point_minute":"..."              // select 下拉框（0-59）
}
```

### Component API 章节

此章节列出可调用的方法，如 `status[]`（获取状态）、`setup[]`/`shut[]`（启停服务）、`delete[name]`（删除条目）等。页面可通过 `he.exec()` 调用这些方法。

### 配置属性到 HTML 控件的映射

| 配置属性类型 | HTML 控件 | 示例 |
|-------------|----------|------|
| 字符串（IP、主机名等） | `<input type="text">` | `ip`、`mask`、`server` |
| 数字（端口、秒数等） | `<input type="text">` | `age`、`size`、`port` |
| 枚举值（有限选项） | `<select>` + `<option>` | `mode`、`level`、`location` |
| 布尔/启停（enable/disable） | `<input type="checkbox">` (ace-switch) | `status` |
| 密码 | `<input type="password">` | 配合 `page.password()` |
| 只读信息 | `<div>` 或 `<span>` | 状态、MAC 地址等 |
| 对象列表 | jqGrid 表格 | 日志文件列表、客户端列表 |

---

## 2. 文件结构与命名

```
ace/
├── content/
│   └── mypage.html        ← HTML 页面片段
├── js/
│   └── mypage.js          ← JS 控制器（复杂页面）
└── lang/
    ├── mypage-cn.json     ← 中文翻译
    └── mypage-en.json     ← 英文翻译
```

**两种 JS 组织方式**：

- **内联 JS**：JS 写在 HTML 的 `<script>` 标签内，适合配置项少于 10 个的简单页面（如 `restart.html`）
- **外部 JS**：HTML 末尾 `<script src="/js/mypage.js"></script>` 引用独立文件，适合逻辑复杂的页面（如 `syslog.html` + `syslog.js`）

---

## 3. 创建 HTML 页面骨架

所有页面遵循统一骨架结构，以下为可直接复制的模板：

```html
<!-- ajax layout which only needs content area -->
<div class="row">
  <div class="col-xs-12 form-btn-wrapper">
    <!-- PAGE CONTENT BEGINS -->
    <div class="form-horizontal" role="form">

      <!-- 在此添加表单字段 -->

      <div class="clearfix form-actions">
        <div class="col-md-offset-3 col-md-9">
          <button class="btn btn-second" type="button" id="refresh">
            <span data-i18n="Refresh"></span>
          </button>
          &nbsp; &nbsp; &nbsp;
          <button class="btn btn-main" type="button" id="apply">
            <span data-i18n="Apply"></span>
          </button>
        </div>
      </div>

    </div>
    <!-- PAGE CONTENT ENDS -->
  </div>
</div>

<!-- 外部 JS 方式 -->
<script src="/js/mypage.js"></script>

<!-- 或内联 JS 方式 -->
<!--
<script type="text/javascript">
(function () {
  // 代码写在这里
})();
</script>
-->
```

### 常用控件模板

**文本输入框**（对应字符串/数字类型的配置属性）

```html
<div class="form-group">
  <label class="col-sm-3 control-label no-padding-right" data-i18n="IP Address"></label>
  <div class="col-sm-9">
    <div class="clearfix">
      <input type="text" id="ip" class="col-xs-10 col-sm-5" maxlength="128" />
    </div>
  </div>
</div>
```

**下拉选择框**（对应枚举类型的配置属性）

```html
<div class="form-group">
  <label class="col-sm-3 control-label no-padding-right" data-i18n="Mode"></label>
  <div class="col-sm-9">
    <select class="col-xs-10 col-sm-5" id="mode">
      <option value="disable" data-i18n="Disable"></option>
      <option value="age" data-i18n="Age"></option>
      <option value="point" data-i18n="Point"></option>
    </select>
  </div>
</div>
```

**开关控件**（对应 enable/disable 类型的配置属性）

```html
<div class="form-group">
  <label class="col-sm-3 control-label no-padding-right" data-i18n="DHCP Server"></label>
  <div class="col-sm-9">
    <label>
      <input id="dhcps" class="ace ace-switch ace-switch-6" type="checkbox" />
      <span class="lbl"></span>
    </label>
  </div>
</div>
```

**只读文本**（用于展示状态信息）

```html
<div class="form-group">
  <label class="col-sm-3 control-label no-padding-right" data-i18n="MAC Address"></label>
  <div class="col-sm-9">
    <div id="mac" class="col-xs-10 col-sm-5 form-right-text"></div>
  </div>
</div>
```

**条件面板**（根据开关或选择框的值显示/隐藏）

```html
<div id="advancedSets" style="display: none;">
  <!-- 此处放置需要条件显示的字段 -->
</div>
```

**分隔线**

```html
<div class="hr hr32 hr-dotted"></div>
```

### 布局说明

- `data-i18n="Key"` — 为元素标记翻译键，框架自动替换为当前语言的文本
- `col-sm-3` — 标签列，占 3/12 宽度
- `col-sm-9` — 输入列，占 9/12 宽度
- `col-xs-10 col-sm-5` — 输入控件宽度，桌面 5/12，移动 10/12
- `id` 属性建议与组件配置中的 JSON 键名一致，方便代码对照

---

## 4. 编写 JS 控制逻辑

### 入口结构模板

不论内联还是外部 JS，入口结构一致：

```javascript
// 配置对象和组件名
var config;
var object = "project@component";   // 如 "clock@restart"

// 加载配置到表单
function config_load() {
    he.load([object]).then(function (v) {
        config = v[0];
        // ... 填充表单（见第 5 节）
    });
}

// 从表单保存配置
function config_save() {
    // ... 收集并提交（见第 6 节）
}

// ===== 入口 =====
$.i18n().load(page.lang('mypage')).then(function () {
    // 1. 应用翻译
    $.i18n().locale = lang;
    $('body').i18n();

    // 2. 加载配置
    config_load();

    // 3. 绑定按钮
    $('#refresh').on(ace.click_event, function () {
        location.reload();
    });
    $('#apply').on(ace.click_event, function () {
        config_save();
    });
});
```

内联 JS 时用 IIFE 包裹避免全局污染：

```javascript
(function () {
    var config;
    var object = "clock@restart";
    // ... 同上
})();
```

---

## 5. 读取配置（he.load）

`he.load(commands)` 向设备发送 HE 命令，返回 Promise。参数为数组时，结果通过 `v[0]`、`v[1]` 等按序访问。

### 基本读取

```javascript
// 读取单个组件配置
he.load(["clock@restart"]).then(function (v) {
    config = v[0];   // config 是 JSON 对象
});

// 同时读取配置和状态
he.load(["ifname@lan", "ifname@lan.status"]).then(function (v) {
    var config = v[0];   // 配置对象
    var status = v[1];   // 状态对象
});
```

### 填充表单

对照组件 markdown 中的 JSON 结构，将每个属性填充到对应的表单控件：

```javascript
he.load([object]).then(function (v) {
    config = v[0];

    // 文本/数字 → val()
    $('#ip').val(config.static.ip || '');
    $('#mask').val(config.static.mask || '');
    $('#age').val(config.age);

    // 下拉选择 → val()
    $('#mode').val(config.mode || 'disable');
    $('#level').val(config.level);

    // 开关 → prop('checked')，配合 able2boole 将 "enable"/"disable" 转为布尔值
    $('#status').prop('checked', able2boole(config.status));
    $('#dhcps').prop('checked', able2boole(config.dhcps.status));

    // 只读文本 → text() 或 html()
    $('#mac').text(config.mac);
});
```

### 条件面板绑定

当选择框或开关的值变化时，显示/隐藏对应的面板：

```javascript
// 开关控制面板显隐
$('#status').unbind('change').change(function () {
    if ($(this).prop('checked')) {
        $('#statusSets').show();
    } else {
        $('#statusSets').hide();
    }
}).trigger('change');   // trigger 确保加载时也执行一次

// 下拉选择控制面板显隐
$('#mode').unbind('change').change(function (e) {
    var mode = e.target.value;
    switch (mode) {
        case 'idle':
            $('#idleSets').show();
            $('#pointSets').hide();
            break;
        case 'point':
            $('#idleSets').hide();
            $('#pointSets').show();
            break;
        default:
            $('#idleSets').hide();
            $('#pointSets').hide();
            break;
    }
}).trigger('change');
```

---

## 6. 保存配置（he.exec）

保存流程固定为四步：深拷贝 &rarr; 收集 &rarr; 比较 &rarr; 提交。

```javascript
function config_save() {
    if (!config) {
        return;
    }
    // 1. 深拷贝原始配置，用于后续比较
    var copy = JSON.parse(JSON.stringify(config));

    // 2. 从表单收集值写回 config 对象
    config.mode = $('#mode').val();
    config.age = $('#age').val();
    config.status = boole2able($('#status').prop('checked'));

    // 嵌套对象
    if (!config.dhcps) {
        config.dhcps = {};
    }
    config.dhcps.status = boole2able($('#dhcps').prop('checked'));
    config.dhcps.startip = $('#startip').val();

    // 3. 用 ocompare 比较是否有变更
    if (ocompare(config, copy)) {
        page.alert({ message: $.i18n('Settings unchanged') });
        return;
    }

    // 4. 提交配置
    he.exec([object + "=" + JSON.stringify(config)]).then(function () {
        page.hint2succeed($.i18n('Modify successfully'));
        config_load();   // 重新加载确认结果
    });
}
```

### 关键函数说明

- `JSON.parse(JSON.stringify(config))` — 深拷贝配置对象
- `ocompare(a, b)` — 递归比较两个对象，相同返回 `true`
- `boole2able(bool)` — `true` &rarr; `"enable"`，`false` &rarr; `"disable"`
- `able2boole(str)` — `"enable"` &rarr; `true`，其他 &rarr; `false`

---

## 7. 输入验证

保存前应对用户输入进行校验，框架提供了 `check` 对象：

```javascript
// IP 地址校验
if (check.ip($('#ip').val()) == false) {
    page.alert({ message: $.i18n('IP Address') + " " + $.i18n('must be a valid IP address') });
    return;
}

// 端口校验（1-65535）
if (check.port($('#port').val()) == false) {
    page.alert({ message: $.i18n('Port') + " " + $.i18n('must be a valid port') });
    return;
}

// 数字校验
if (check.number($('#age').val()) == false) {
    page.alert({ message: $.i18n('Age') + " " + $.i18n('must be a valid number') });
    return;
}

// MAC 地址校验
if (check.mac($('#mac').val()) == false) {
    page.alert({ message: $.i18n('MAC') + " " + $.i18n('must be a valid MAC address') });
    return;
}
```

---

## 8. 高级功能

### 调用组件方法

组件 markdown 中 Component API 章节列出的方法均可通过 HE 命令调用：

```javascript
// 获取状态
he.load(["ifname@lan.status"]).then(function (v) {
    var status = v[0];
    // status.ip, status.mask, ...
});

// 启动/停止服务
he.exec(["ifname@wan.setup"]).then(function () { /* ... */ });
he.exec(["ifname@wan.shut"]).then(function () { /* ... */ });

// 带参数调用方法
he.exec(["land@syslog.delete[logfile.txt]"]).then(function () { /* ... */ });
```

### 状态轮询

对需要实时更新的状态页，用 `page.timing` 设定定时刷新（离开页面时自动销毁）：

```javascript
page.timing({
    refresh: function () {
        he.bkload(["ifname@lan.status"]).then(function (v) {
            // 更新状态显示（bkload 不显示加载遮罩）
        });
    },
    interval: 5000   // 每 5 秒刷新
});
```

### 表格列表（jqGrid）

用于展示列表数据（如日志文件列表、客户端列表）：

```html
<!-- HTML -->
<table id="list-table"></table>
<div id="list-pager"></div>
```

```javascript
// JS
jqtable.create('#list-table', '#list-pager', {
    caption: ' ',
    colNames: [$.i18n('Name'), $.i18n('Operation')],
    colModel: [
        { name: 'name', width: 180 },
        $.extend(true, {}, jqtable.actionOptions, {
            formatoptions: {
                delOptions: {
                    onclickSubmit: function (params, data) { my_delete(data); }
                },
                editformbutton: false, editbutton: false
            }
        })
    ],
    pager: '#list-pager',
    rowNum: 10,
    autowidth: true,
    loadonce: true,
    shrinkToFit: true
});

// 填充数据
var rows = [];
for (var key in list) {
    rows.push({ name: key });
}
$('#list-table').jqGrid('clearGridData')
    .jqGrid('setGridParam', { data: rows })
    .trigger('reloadGrid');
```

### 确认对话框

用于危险操作前的确认：

```javascript
page.confirm({ message: $.i18n('Are you sure?') }).then(function (result) {
    if (result) {
        // 用户点击了确认
    }
});
```

### 重启设备

```javascript
he.reboot({
    title: $.i18n('Rebooting...'),
    hint: $.i18n('Make sure that the device is reconnected')
    // 可选: restartTime (秒), href (重启后跳转), cmds (重启前执行的额外命令)
});
```

---

## 9. 添加国际化翻译

### 翻译文件格式

翻译文件是简单的 JSON 键值对，键为英文原文（与 HTML 中 `data-i18n` 的值一致），值为目标语言翻译：

```json
// ace/lang/mypage-cn.json
{
    "Mode": "模式",
    "Disable": "禁用",
    "Age": "按时长",
    "Point": "按时间点",
    "Maximum runtime(sec)": "最大运行时长(秒)",
    "Settings unchanged": "设置未变更",
    "Modify successfully": "修改成功"
}
```

```json
// ace/lang/mypage-en.json
{
    "Settings unchanged": "Settings unchanged",
    "Modify successfully": "Modify successfully"
}
```

### 命名规则

- 文件名：`<页面名>-<语言代码>.json`
- 语言代码：`cn`（中文）、`en`（英文）
- 放置目录：`ace/lang/`

### 加载翻译

在 JS 入口处调用：

```javascript
$.i18n().load(page.lang('mypage')).then(function () {
    $.i18n().locale = lang;
    $('body').i18n();       // 翻译所有 data-i18n 元素
    // ... 后续逻辑
});
```

在 JS 代码中使用 `$.i18n('Key')` 获取翻译文本。

---

## 10. 注册菜单入口

在工程的 `prj.json` 文件中添加 `wui` 字段，将页面注册到管理站点的菜单中：

```json
{
    "name": "myproject",
    "intro": "My project description",
    "type": "root",
    "version": "8.0.0",

    "wui":
    {
        "webs":
        {
            "menu": "System",
            "cn": "我的功能",
            "en": "My Feature",
            "page": "mypage.html",
            "config": "myproject@mycomponent"
        }
    }
}
```

**字段说明**：

| 字段 | 必填 | 说明 |
|------|------|------|
| `menu` | 是 | 所属菜单分组（如 `System`、`Network`、`Wireless`） |
| `cn` | 是 | 中文菜单标题 |
| `en` | 是 | 英文菜单标题 |
| `page` | 是 | HTML 页面文件名（在 `ace/content/` 下） |
| `config` | 否 | 关联的配置组件对象（如 `clock@restart`） |
| `object` | 否 | 关联的组件对象，与 `config` 二选一 |

注册多个页面时，在 `wui` 下添加多个条目：

```json
"wui":
{
    "page1":
    {
        "menu": "Network",
        "cn": "LAN配置",
        "en": "LAN Settings",
        "page": "lan.html",
        "config": "ifname@lan"
    },
    "page2":
    {
        "menu": "Network",
        "cn": "WAN配置",
        "en": "WAN Settings",
        "page": "wan.html",
        "config": "ifname@wan"
    }
}
```

---

## 完整示例：从 markdown 到管理页面

以 `clock@restart`（定时重启）为例，演示完整流程。

### 第一步：阅读 clock/restart.md

从 Configuration 章节得知配置结构：

```json
{
    "mode": "disable|age|point|idle",
    "age": "数字（秒）",
    "point_hour": "数字（0-23）",
    "point_minute": "数字（0-59）",
    "point_age": "数字（秒）"
}
```

### 第二步：规划表单

| 配置属性 | 控件类型 | HTML id | 显示条件 |
|---------|---------|---------|---------|
| `mode` | select | `mode` | 始终显示 |
| `age` | text | `age` | mode=age |
| `point_hour` | select (0-23) | `point_hour` | mode=point |
| `point_minute` | select (0-59) | `point_minute` | mode=point |
| `point_age` | text | `point_age` | mode=point |

### 第三步：编写 HTML

```html
<div class="row">
  <div class="col-xs-12 form-btn-wrapper">
    <div class="form-horizontal" role="form">

      <div class="form-group">
        <label class="col-sm-3 control-label no-padding-right" data-i18n="Reboot Mode"></label>
        <div class="col-sm-9">
          <select class="col-xs-10 col-sm-5" id="mode">
            <option value="disable" data-i18n="Disable"></option>
            <option value="age" data-i18n="Age"></option>
            <option value="point" data-i18n="Point"></option>
          </select>
        </div>
      </div>

      <div id="ageSets" style="display: none;">
        <div class="form-group">
          <label class="col-sm-3 control-label no-padding-right"
                 data-i18n="Reboot time interval(sec)"></label>
          <div class="col-sm-9">
            <div class="clearfix">
              <input type="text" id="age" class="col-xs-10 col-sm-5" maxlength="128" />
            </div>
          </div>
        </div>
      </div>

      <div id="pointSets" style="display: none;">
        <div class="form-group">
          <label class="col-sm-3 control-label no-padding-right"
                 data-i18n="Point Reboot Time"></label>
          <div class="col-sm-9">
            <select id="point_hour"></select>
            <select id="point_minute"></select>
          </div>
        </div>
      </div>

      <div class="clearfix form-actions">
        <div class="col-md-offset-3 col-md-9">
          <button class="btn btn-second" type="button" id="refresh">
            <span data-i18n="Refresh"></span>
          </button>
          &nbsp; &nbsp; &nbsp;
          <button class="btn btn-main" type="button" id="apply">
            <span data-i18n="Apply"></span>
          </button>
        </div>
      </div>

    </div>
  </div>
</div>

<script type="text/javascript">
(function () {
    var cfg;
    var obj = "clock@restart";

    function load_cfg() {
        he.load([obj]).then(function (v) {
            cfg = v[0];
            $('#mode').val(cfg.mode || 'disable');
            $('#age').val(cfg.age);
            $('#point_hour').val(cfg.point_hour);
            $('#point_minute').val(cfg.point_minute);

            $('#mode').unbind('change').change(function (e) {
                var mode = e.target.value;
                $('#ageSets').toggle(mode === 'age');
                $('#pointSets').toggle(mode === 'point');
            }).trigger('change');
        });
    }

    function save_cfg() {
        if (!cfg) { return; }
        var copy = JSON.parse(JSON.stringify(cfg));

        cfg.mode = $('#mode').val();
        if (cfg.mode === 'age') {
            cfg.age = $('#age').val();
        } else if (cfg.mode === 'point') {
            cfg.point_hour = $('#point_hour').val();
            cfg.point_minute = $('#point_minute').val();
        }

        if (ocompare(cfg, copy)) {
            page.alert({ message: $.i18n('Settings unchanged') });
            return;
        }
        he.exec([obj + "=" + JSON.stringify(cfg)]).then(function () {
            page.hint2succeed($.i18n('Modify successfully'));
            load_cfg();
        });
    }

    $.i18n().load(page.lang('manage')).then(function () {
        $.i18n().locale = lang;
        $('body').i18n();
        // 动态生成小时/分钟选项
        var h = '', m = '';
        for (var i = 0; i < 24; i++) { h += '<option value="' + i + '">' + i + '</option>'; }
        for (var i = 0; i < 60; i++) { m += '<option value="' + i + '">' + i + '</option>'; }
        $('#point_hour').html(h);
        $('#point_minute').html(m);

        load_cfg();
        $('#refresh').on(ace.click_event, function () { location.reload(); });
        $('#apply').on(ace.click_event, function () { save_cfg(); });
    });
})();
</script>
```

### 第四步：注册菜单

在 `prj.json` 的 `wui` 字段中添加：

```json
"wui": {
    "webs": {
        "menu": "System",
        "cn": "自动重启",
        "en": "Auto Reboot",
        "config": "clock@restart",
        "page": "restart.html"
    }
}
```

---

## 附录 A：he.js API 参考

页面必须引入 `<script src="/api/he.js"></script>`（ace 框架已全局引入）。

### 全局变量

| 变量 | 说明 |
|------|------|
| `window.hepath` | HE 端点路径，通常为 `"/he"` |
| `window.talkkey` | 会话密钥，自动维护 |
| `window.username` | 当前登录用户名 |

### Promise 风格 API

| API | 遮罩 | 默认提示 | 用途 |
|-----|------|---------|------|
| `he.load(a, loading)` | 有 | "Loading" | 读取配置/状态 |
| `he.exec(a, loading)` | 有 | "Running" | 写入配置/执行命令 |
| `he.bkload(a)` | 无 | — | 后台读取（适合轮询） |
| `he.bkexec(a)` | 无 | — | 后台执行 |

参数 `a` 为 HE 命令数组，返回 Promise，通过 `.then(function(v){...})` 处理结果。`v[0]`、`v[1]` 等按序对应命令结果。

### HE 命令字符串语法

| 操作 | 命令格式 | 示例 |
|------|---------|------|
| 读取完整配置 | `"component@name"` | `"clock@restart"` |
| 读取单个属性 | `"component@name:attr"` | `"land@machine:name"` |
| 设置单个属性 | `"component@name:attr=value"` | `"land@machine:name=MyDevice"` |
| 替换完整配置 | `"component@name="+JSON.stringify(obj)` | `"clock@restart="+JSON.stringify(cfg)` |
| 合并部分属性 | `"component@name\|"+JSON.stringify(partial)` | `"ifname@lan\|"+JSON.stringify({status:"enable"})` |
| 调用方法（无参数） | `"component@name.method"` | `"ifname@lan.status"` |
| 调用方法（带参数） | `"component@name.method[param]"` | `"land@syslog.delete[log.txt]"` |
| 读取方法返回的属性 | `"component@name.method:attr"` | `"land@machine.status:version"` |

### 底层 API

`he.cmd(a, args, func)` — 所有上层 API 的基础。

- `a`：命令数组或对象
- `args`：`{ timeout: 毫秒, loading: "提示文字" }`（可选）
- `func`：回调函数；传 `null` 表示同步调用（不推荐）

当 `a` 为对象时，可用属性名访问结果：

```javascript
he.cmd({
    machine: "land@machine",
    cpu: "land@machine.cpuinfo"
}, null, function (v) {
    // v.machine 和 v.cpu
});
```

### 返回值解码

| 服务器返回 | JS 值 |
|-----------|-------|
| `"ttrue"` / `"true"` | `true` |
| `"tfalse"` / `"false"` | `false` |
| `"terror"` / `"tpanic"` | `false`（控制台警告） |
| `"NULL"` | `null` |
| JSON 对象 | 解析后的对象 |
| `"Auth Error"` | 跳转到 `login.html` |

### 重启相关

| API | 说明 |
|-----|------|
| `he.reboot(args)` | 显示重启进度条，执行 `machine.restart` |
| `he.upgrade_reboot(args)` | 升级后重启（默认等待时间更长） |

`args` 可选字段：`title`、`restartTime`（秒）、`href`（重启后跳转地址）、`hint`（提示文字）、`cmds`（重启前执行的额外命令数组）。

---

## 附录 B：常用辅助函数参考

来自 `ace/api/util.js`，页面可直接使用。

### 类型转换

| 函数 | 说明 | 示例 |
|------|------|------|
| `able2boole(str)` | `"enable"` &rarr; `true`，其他 &rarr; `false` | 读取开关配置 |
| `boole2able(bool)` | `true` &rarr; `"enable"`，`false` &rarr; `"disable"` | 写入开关配置 |
| `date2string(str)` | `"13:28:36:03:02:2018"` &rarr; `"13:28:36 (03/02/2018)"` | 日期显示 |
| `time2string(str)` | `"01:15:50:0"` &rarr; `"1Day 01:15:50"` | 运行时长显示 |
| `byte2readable(n)` | 字节数 &rarr; 人类可读 (`"1.5MB"`) | 流量显示 |

### 比较与校验

| 函数 | 说明 |
|------|------|
| `ocompare(a, b)` | 深度比较两个对象，相同返回 `true` |
| `check.ip(value)` | 验证 IPv4 地址格式 |
| `check.port(value)` | 验证端口号（1-65535） |
| `check.number(value)` | 验证是否为数字 |
| `check.mac(value)` | 验证 MAC 地址格式 |

### 页面交互

| 函数 | 说明 |
|------|------|
| `page.alert({message})` | 弹出提示对话框，返回 Promise |
| `page.confirm({message})` | 弹出确认对话框，返回 Promise（resolve `true`/`false`） |
| `page.hint2succeed(text)` | 顶部成功提示（绿色，自动消失） |
| `page.hint2warning(text)` | 顶部警告提示（黄色，自动消失） |
| `page.overlay(text)` | 显示全屏加载遮罩 |
| `page.overlay2hide()` | 隐藏全屏加载遮罩 |
| `page.timing({refresh, interval})` | 设置定时刷新（离开页面自动销毁），返回 timer |
| `page.lang(name)` | 返回翻译文件路径对象 `{ lang: "/lang/name-lang.json" }` |
| `page.param(name, key)` | 从 URL 参数中获取值 |
| `page.password(inputID, iconID)` | 绑定密码显示/隐藏切换 |

### 数据工具

| 函数 | 说明 |
|------|------|
| `json2array(obj, defaults, keyName)` | 将 JSON 对象转为数组（用于 jqGrid） |
| `json_empty(obj)` | 判断对象是否为空 |
| `ipadd2array(ip, mask)` | 根据 IP 和掩码计算网络地址数组 |

完整的 HE 语法、管道、前缀等规则详见 `doc/com/land/he.md`。
