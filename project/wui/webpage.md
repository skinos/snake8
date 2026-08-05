# Writing component management web pages

This guide explains how to build Web management pages for device components. Management pages run inside the **ace** framework (a Bootstrap-based single-page application) that loads HTML fragments via AJAX and uses hash-based routing. Each page corresponds to one or more **component objects** (e.g. `clock@restart`, `ifname@lan`) and communicates with the device through `he.js` to read/write JSON configuration and invoke component methods.

**Core workflow**: read the component markdown doc &rarr; understand the JSON configuration &rarr; map configuration attributes to form controls &rarr; use `he.js` to load/save configuration.

---

## 1. Preparation: reading the component doc

Every component has a markdown document (e.g. `ifname/lan.md`, `clock/restart.md`). Before writing a management page, focus on two sections:

### Configuration section

This section describes the component's JSON configuration. Each attribute represents a manageable field. For example, `clock@restart`:

```json
{
    "mode":"Restart plan mode",       // select dropdown
    "age":"The maximum runtime",      // text input (number)
    "point_hour":"...",               // select dropdown (0-23)
    "point_minute":"..."              // select dropdown (0-59)
}
```

### Component API section

This section lists callable methods such as `status[]` (get status), `setup[]`/`shut[]` (start/stop service), `delete[name]` (delete entry), etc. Pages can invoke these methods via `he.exec()`.

### Mapping configuration attributes to HTML controls

| Attribute type | HTML control | Examples |
|---------------|-------------|----------|
| String (IP, hostname, etc.) | `<input type="text">` | `ip`, `mask`, `server` |
| Number (port, seconds, etc.) | `<input type="text">` | `age`, `size`, `port` |
| Enum (limited choices) | `<select>` + `<option>` | `mode`, `level`, `location` |
| Boolean / enable-disable | `<input type="checkbox">` (ace-switch) | `status` |
| Password | `<input type="password">` | used with `page.password()` |
| Read-only info | `<div>` or `<span>` | status, MAC address, etc. |
| Object list | jqGrid table | log file list, client list |

---

## 2. File structure and naming

```
ace/
├── content/
│   └── mypage.html        ← HTML page fragment
├── js/
│   └── mypage.js          ← JS controller (complex pages)
└── lang/
    ├── mypage-cn.json     ← Chinese translation
    └── mypage-en.json     ← English translation
```

**Two JS organization patterns**:

- **Inline JS**: JS written inside a `<script>` tag in the HTML file; suitable for simple pages with fewer than ~10 configuration attributes (e.g. `restart.html`)
- **External JS**: HTML references a separate file via `<script src="/js/mypage.js"></script>`; suitable for complex pages (e.g. `syslog.html` + `syslog.js`)

---

## 3. Creating the HTML page skeleton

All pages follow a uniform skeleton. Copy-paste template:

```html
<!-- ajax layout which only needs content area -->
<div class="row">
  <div class="col-xs-12 form-btn-wrapper">
    <!-- PAGE CONTENT BEGINS -->
    <div class="form-horizontal" role="form">

      <!-- add form fields here -->

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

<!-- external JS pattern -->
<script src="/js/mypage.js"></script>

<!-- or inline JS pattern -->
<!--
<script type="text/javascript">
(function () {
  // code goes here
})();
</script>
-->
```

### Common control templates

**Text input** (for string / number attributes)

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

**Select dropdown** (for enum attributes)

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

**Toggle switch** (for enable/disable attributes)

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

**Read-only text** (for displaying status information)

```html
<div class="form-group">
  <label class="col-sm-3 control-label no-padding-right" data-i18n="MAC Address"></label>
  <div class="col-sm-9">
    <div id="mac" class="col-xs-10 col-sm-5 form-right-text"></div>
  </div>
</div>
```

**Conditional panel** (shown/hidden based on a switch or select value)

```html
<div id="advancedSets" style="display: none;">
  <!-- fields that are conditionally visible -->
</div>
```

**Separator line**

```html
<div class="hr hr32 hr-dotted"></div>
```

### Layout notes

- `data-i18n="Key"` — marks the element for translation; the framework replaces its text with the current locale value
- `col-sm-3` — label column, 3/12 width
- `col-sm-9` — input column, 9/12 width
- `col-xs-10 col-sm-5` — input control width: 5/12 on desktop, 10/12 on mobile
- The `id` attribute should match the JSON key name in the component configuration for easy mapping

---

## 4. Writing JS control logic

### Entry structure template

Whether inline or external, the entry structure is the same:

```javascript
// configuration object and component name
var config;
var object = "project@component";   // e.g. "clock@restart"

// load configuration into the form
function config_load() {
    he.load([object]).then(function (v) {
        config = v[0];
        // ... populate form (see section 5)
    });
}

// save configuration from the form
function config_save() {
    // ... collect and submit (see section 6)
}

// ===== entry point =====
$.i18n().load(page.lang('mypage')).then(function () {
    // 1. apply translations
    $.i18n().locale = lang;
    $('body').i18n();

    // 2. load configuration
    config_load();

    // 3. bind buttons
    $('#refresh').on(ace.click_event, function () {
        location.reload();
    });
    $('#apply').on(ace.click_event, function () {
        config_save();
    });
});
```

For inline JS, wrap in an IIFE to avoid polluting the global scope:

```javascript
(function () {
    var config;
    var object = "clock@restart";
    // ... same as above
})();
```

---

## 5. Loading configuration (he.load)

`he.load(commands)` sends HE commands to the device and returns a Promise. When the argument is an array, results are accessed by index: `v[0]`, `v[1]`, etc.

### Basic loading

```javascript
// load a single component configuration
he.load(["clock@restart"]).then(function (v) {
    config = v[0];   // config is a JSON object
});

// load configuration and status at the same time
he.load(["ifname@lan", "ifname@lan.status"]).then(function (v) {
    var config = v[0];   // configuration object
    var status = v[1];   // status object
});
```

### Populating form fields

Map each JSON attribute from the component markdown to its corresponding form control:

```javascript
he.load([object]).then(function (v) {
    config = v[0];

    // text / number → val()
    $('#ip').val(config.static.ip || '');
    $('#mask').val(config.static.mask || '');
    $('#age').val(config.age);

    // select dropdown → val()
    $('#mode').val(config.mode || 'disable');
    $('#level').val(config.level);

    // toggle switch → prop('checked'), using able2boole to convert "enable"/"disable" to boolean
    $('#status').prop('checked', able2boole(config.status));
    $('#dhcps').prop('checked', able2boole(config.dhcps.status));

    // read-only text → text() or html()
    $('#mac').text(config.mac);
});
```

### Conditional panel binding

Show or hide panels when a select or toggle value changes:

```javascript
// toggle switch controls panel visibility
$('#status').unbind('change').change(function () {
    if ($(this).prop('checked')) {
        $('#statusSets').show();
    } else {
        $('#statusSets').hide();
    }
}).trigger('change');   // trigger ensures it also runs on initial load

// select dropdown controls panel visibility
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

## 6. Saving configuration (he.exec)

The save flow always follows four steps: deep-copy &rarr; collect &rarr; compare &rarr; submit.

```javascript
function config_save() {
    if (!config) {
        return;
    }
    // 1. deep-copy the original configuration for comparison
    var copy = JSON.parse(JSON.stringify(config));

    // 2. collect form values back into the config object
    config.mode = $('#mode').val();
    config.age = $('#age').val();
    config.status = boole2able($('#status').prop('checked'));

    // nested objects
    if (!config.dhcps) {
        config.dhcps = {};
    }
    config.dhcps.status = boole2able($('#dhcps').prop('checked'));
    config.dhcps.startip = $('#startip').val();

    // 3. compare with the copy to detect changes
    if (ocompare(config, copy)) {
        page.alert({ message: $.i18n('Settings unchanged') });
        return;
    }

    // 4. submit configuration
    he.exec([object + "=" + JSON.stringify(config)]).then(function () {
        page.hint2succeed($.i18n('Modify successfully'));
        config_load();   // reload to confirm the result
    });
}
```

### Key function reference

- `JSON.parse(JSON.stringify(config))` — deep-copy a configuration object
- `ocompare(a, b)` — recursively compare two objects; returns `true` if equal
- `boole2able(bool)` — `true` &rarr; `"enable"`, `false` &rarr; `"disable"`
- `able2boole(str)` — `"enable"` &rarr; `true`, anything else &rarr; `false`

---

## 7. Input validation

Validate user input before saving. The framework provides the `check` object:

```javascript
// IP address validation
if (check.ip($('#ip').val()) == false) {
    page.alert({ message: $.i18n('IP Address') + " " + $.i18n('must be a valid IP address') });
    return;
}

// port validation (1-65535)
if (check.port($('#port').val()) == false) {
    page.alert({ message: $.i18n('Port') + " " + $.i18n('must be a valid port') });
    return;
}

// number validation
if (check.number($('#age').val()) == false) {
    page.alert({ message: $.i18n('Age') + " " + $.i18n('must be a valid number') });
    return;
}

// MAC address validation
if (check.mac($('#mac').val()) == false) {
    page.alert({ message: $.i18n('MAC') + " " + $.i18n('must be a valid MAC address') });
    return;
}
```

---

## 8. Advanced features

### Calling component methods

Methods listed in the Component API section of the markdown can be invoked via HE commands:

```javascript
// get status
he.load(["ifname@lan.status"]).then(function (v) {
    var status = v[0];
    // status.ip, status.mask, ...
});

// start / stop a service
he.exec(["ifname@wan.setup"]).then(function () { /* ... */ });
he.exec(["ifname@wan.shut"]).then(function () { /* ... */ });

// call a method with parameters
he.exec(["land@syslog.delete[logfile.txt]"]).then(function () { /* ... */ });
```

### Status polling

For pages that need real-time status updates, use `page.timing` to set up periodic refresh (automatically destroyed when the user navigates away):

```javascript
page.timing({
    refresh: function () {
        he.bkload(["ifname@lan.status"]).then(function (v) {
            // update status display (bkload shows no loading overlay)
        });
    },
    interval: 5000   // refresh every 5 seconds
});
```

### Table lists (jqGrid)

Used to display list data (e.g. log file lists, client lists):

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

// populate data
var rows = [];
for (var key in list) {
    rows.push({ name: key });
}
$('#list-table').jqGrid('clearGridData')
    .jqGrid('setGridParam', { data: rows })
    .trigger('reloadGrid');
```

### Confirmation dialog

Use before dangerous operations:

```javascript
page.confirm({ message: $.i18n('Are you sure?') }).then(function (result) {
    if (result) {
        // user clicked confirm
    }
});
```

### Rebooting the device

```javascript
he.reboot({
    title: $.i18n('Rebooting...'),
    hint: $.i18n('Make sure that the device is reconnected')
    // optional: restartTime (max wait seconds), href (redirect after reboot), cmds (extra commands before reboot)
});
```

After `machine.restart`, the UI shows a progress bar for up to `restartTime` / `arch@custom.restart_time` / **60** seconds. It probes `/login.html` every **2s** (ignores “alive” for the first **10s**, requires an offline sample, then **2** consecutive successes). When the web is back it shows **Reboot successfully** for about **1.2s** (overlay kept visible), then navigates to **`login.html`** (or `href`); on timeout it shows `hint` then navigates the same way. The bar pauses near **95%** while waiting.

---

## 9. Adding internationalization (i18n)

### Translation file format

Translation files are simple JSON key-value pairs. Keys are the English originals (matching `data-i18n` values in HTML), values are the translated text:

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

### Naming convention

- File name: `<pagename>-<locale>.json`
- Locale codes: `cn` (Chinese), `en` (English)
- Directory: `ace/lang/`

### Loading translations

Call at the JS entry point:

```javascript
$.i18n().load(page.lang('mypage')).then(function () {
    $.i18n().locale = lang;
    $('body').i18n();       // translate all data-i18n elements
    // ... subsequent logic
});
```

Use `$.i18n('Key')` in JS code to retrieve translated text.

---

## 10. Registering the menu entry

Add a `wui` field in the project's `prj.json` to register the page in the admin site menu:

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

**Field reference**:

| Field | Required | Description |
|-------|----------|-------------|
| `menu` | yes | Menu group name (e.g. `System`, `Network`, `Wireless`) |
| `cn` | yes | Chinese menu title |
| `en` | yes | English menu title |
| `page` | yes | HTML page file name (under `ace/content/`) |
| `config` | no | Associated configuration component object (e.g. `clock@restart`) |
| `object` | no | Associated component object; use either `config` or `object` |

To register multiple pages, add multiple entries under `wui`:

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

## Complete example: from markdown to management page

This example walks through building a page for `clock@restart` (scheduled restart).

### Step 1: Read clock/restart.md

From the Configuration section, the JSON structure is:

```json
{
    "mode": "disable|age|point|idle",
    "age": "number (seconds)",
    "point_hour": "number (0-23)",
    "point_minute": "number (0-59)",
    "point_age": "number (seconds)"
}
```

### Step 2: Plan the form

| Attribute | Control type | HTML id | Visible when |
|-----------|-------------|---------|--------------|
| `mode` | select | `mode` | always |
| `age` | text | `age` | mode=age |
| `point_hour` | select (0-23) | `point_hour` | mode=point |
| `point_minute` | select (0-59) | `point_minute` | mode=point |
| `point_age` | text | `point_age` | mode=point |

### Step 3: Write the HTML

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
        // dynamically generate hour/minute options
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

### Step 4: Register the menu

Add to the `wui` field in `prj.json`:

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

## Appendix A: he.js API reference

Pages must include `<script src="/api/he.js"></script>` (already included globally by the ace framework).

### Global variables

| Variable | Description |
|----------|-------------|
| `window.hepath` | HE endpoint path, typically `"/he"` |
| `window.talkkey` | Session key, maintained automatically |
| `window.username` | Current logged-in user name |

### Promise-style APIs

| API | Overlay | Default text | Purpose |
|-----|---------|-------------|---------|
| `he.load(a, loading)` | yes | "Loading" | Read configuration / status |
| `he.exec(a, loading)` | yes | "Running" | Write configuration / execute commands |
| `he.bkload(a)` | no | — | Background read (suitable for polling) |
| `he.bkexec(a)` | no | — | Background execute |

Parameter `a` is an array of HE command strings. Returns a Promise; use `.then(function(v){...})` to process results. `v[0]`, `v[1]`, etc. correspond to command results in order.

### HE command string syntax

| Operation | Command format | Example |
|-----------|---------------|---------|
| Read full config | `"component@name"` | `"clock@restart"` |
| Read one attribute | `"component@name:attr"` | `"land@machine:name"` |
| Set one attribute | `"component@name:attr=value"` | `"land@machine:name=MyDevice"` |
| Replace full config | `"component@name="+JSON.stringify(obj)` | `"clock@restart="+JSON.stringify(cfg)` |
| Merge partial attributes | `"component@name\|"+JSON.stringify(partial)` | `"ifname@lan\|"+JSON.stringify({status:"enable"})` |
| Call method (no params) | `"component@name.method"` | `"ifname@lan.status"` |
| Call method (with params) | `"component@name.method[param]"` | `"land@syslog.delete[log.txt]"` |
| Read attribute from method result | `"component@name.method:attr"` | `"land@machine.status:version"` |

### Low-level API

`he.cmd(a, args, func)` — the foundation for all higher-level APIs.

- `a`: command array or object
- `args`: `{ timeout: milliseconds, loading: "overlay text" }` (optional)
- `func`: callback function; pass `null` for synchronous call (not recommended)

When `a` is an object, results can be accessed by property name:

```javascript
he.cmd({
    machine: "land@machine",
    cpu: "land@machine.cpuinfo"
}, null, function (v) {
    // v.machine and v.cpu
});
```

### Return value decoding

| Server response | JS value |
|----------------|----------|
| `"ttrue"` / `"true"` | `true` |
| `"tfalse"` / `"false"` | `false` |
| `"terror"` / `"tpanic"` | `false` (console warning) |
| `"NULL"` | `null` |
| JSON object | parsed object |
| `"Auth Error"` | redirects to `login.html` |

### Reboot helpers

| API | Description |
|-----|-------------|
| `he.reboot(args)` | Progress + probe until web is back, then reload; runs `machine.restart` |
| `he.upgrade_reboot(args)` | Post-upgrade reboot (longer default wait time) |

Optional `args` fields: `title`, `restartTime` (seconds), `href` (redirect URL after reboot), `hint` (prompt text), `cmds` (extra command array to execute before reboot).

---

## Appendix B: Common helper function reference

From `ace/api/util.js`, available to all pages.

### Type conversion

| Function | Description | Usage |
|----------|-------------|-------|
| `able2boole(str)` | `"enable"` &rarr; `true`, else &rarr; `false` | Reading toggle config |
| `boole2able(bool)` | `true` &rarr; `"enable"`, `false` &rarr; `"disable"` | Writing toggle config |
| `date2string(str)` | `"13:28:36:03:02:2018"` &rarr; `"13:28:36 (03/02/2018)"` | Date display |
| `time2string(str)` | `"01:15:50:0"` &rarr; `"1Day 01:15:50"` | Uptime display |
| `byte2readable(n)` | Bytes &rarr; human-readable (`"1.5MB"`) | Traffic display |

### Comparison and validation

| Function | Description |
|----------|-------------|
| `ocompare(a, b)` | Deep-compare two objects; returns `true` if equal |
| `check.ip(value)` | Validate IPv4 address format |
| `check.port(value)` | Validate port number (1-65535) |
| `check.number(value)` | Validate numeric input |
| `check.mac(value)` | Validate MAC address format |

### Page interaction

| Function | Description |
|----------|-------------|
| `page.alert({message})` | Show alert dialog, returns Promise |
| `page.confirm({message})` | Show confirmation dialog, returns Promise (resolves `true`/`false`) |
| `page.hint2succeed(text)` | Top-bar success notification (green, auto-dismiss) |
| `page.hint2warning(text)` | Top-bar warning notification (yellow, auto-dismiss) |
| `page.overlay(text)` | Show full-screen loading overlay |
| `page.overlay2hide()` | Hide full-screen loading overlay |
| `page.timing({refresh, interval})` | Set up periodic refresh (auto-destroyed on page leave), returns timer |
| `page.lang(name)` | Returns translation file path object `{ lang: "/lang/name-lang.json" }` |
| `page.param(name, key)` | Get value from URL parameters |
| `page.password(inputID, iconID)` | Bind password show/hide toggle |

### Data utilities

| Function | Description |
|----------|-------------|
| `json2array(obj, defaults, keyName)` | Convert JSON object to array (for jqGrid) |
| `json_empty(obj)` | Check if an object is empty |
| `ipadd2array(ip, mask)` | Compute network address array from IP and mask |

For the full HE syntax, pipe rules, and special prefixes, see `doc/com/land/he.md`.
