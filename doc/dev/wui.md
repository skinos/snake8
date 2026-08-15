# Web UI pages and language files

A project can ship an HTML fragment that the **ace** Web shell loads as a menu page. The page talks to a component with the same HE grammar as `he '…'`, through `he.js`. Language files map English UI strings to translations.

Generate the skeleton with `prj add_wui`. Templates: [`project/tmptools/page.html`](../../project/tmptools/page.html), [`project/tmptools/cn.json`](../../project/tmptools/cn.json), [`project/tmptools/en.json`](../../project/tmptools/en.json). A fuller status + buttons example is [`project/tmptools/testcom.html`](../../project/tmptools/testcom.html).

The page is not a C program. Bind it to an object from [comexe.md](comexe.md) (or a shell component) via the `wui` `config` field.

Allowed JS APIs live in `project/wui/ace/api/he.js`. Save and method calls use **`he.exec`**. There is no `he.save`, `he.set`, or `he.get`.

---

## Generate with `prj`

From the SDK root, after the component exists (so `config` can point at it):

```bash
prj add_wui myproj sensor
# webpage menu title(Chinese): 传感器
# webpage menu title(English): Sensor
```

### What appears on disk

```text
project/myproj/
├── sensor.html
├── sensor-cn.json      # copy of tmptools/cn.json
├── sensor-en.json      # copy of tmptools/en.json
└── prj.json
```

`prj` rewrites the JS component name in the HTML from `tmptools@com` to `myproj@sensor`, and registers:

```json
"wui": {
    "sensor": {
        "page": "sensor.html",
        "config": "myproj@sensor",
        "lang": {
            "cn": "sensor-cn.json",
            "en": "sensor-en.json"
        },
        "cn": "传感器",
        "en": "Sensor"
    }
}
```

Optional: add `"menu": "Application"` (or another group). If `menu` is omitted, ace uses a default group.

HTML files sit at the **project root**, not under `project/wui/ace/content/` (that tree is the ace shell itself).

---

## Page structure

The file is an AJAX fragment (no `<html>` / `<body>`). Layout:

1. Form groups (`form-horizontal`) with `data-i18n="English Key"` on labels.
2. A button row: Refresh + Apply.
3. One IIFE `<script>` that loads i18n, then config, then binds buttons.

### Load and save (required pattern)

```javascript
var comname = "myproj@sensor";
var comcfg;
var langjson = base64.decode( page.param( 'lang', location.hash ) );

function load_comcfg()
{
    he.load( [ comname ] ).then( function(v){
        comcfg = v[0];
        if ( !comcfg )
        {
            comcfg = {};
        }
        $('#status').prop( 'checked', able2boole(comcfg.status) );
        $('#property').val( comcfg.property );
    });
}

function save_comcfg()
{
    var comcfgcopy;

    if ( !comcfg )
    {
        comcfg = {};
    }
    comcfgcopy = JSON.parse(JSON.stringify(comcfg));
    comcfg.status = boole2able( $('#status').prop('checked') );
    if ( comcfg.status == "enable" )
    {
        comcfg.property = $('#property').val();
    }
    if ( ocompare( comcfg, comcfgcopy ) )
    {
        page.alert( { message: $.i18n('Settings unchanged') } );
        return;
    }
    he.exec( [ comname+"="+JSON.stringify(comcfg) ] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        load_comcfg();
    });
}
```

| Helper | Role |
|--------|------|
| `he.load([ cmds ])` | Read config or call a query method; results in `v[0]`, `v[1]`, … |
| `he.exec([ cmds ])` | Write config or call a method |
| `able2boole` / `boole2able` | `"enable"` / `"disable"` ↔ checkbox |
| `ocompare` | Skip apply when nothing changed |
| `page.hint2succeed` / `page.alert` | Toast / dialog |
| `$.i18n('English Key')` | Translated string |

HE strings inside the arrays are the same as shell `he '…'`:

| Goal | Example |
|------|---------|
| Full config | `"myproj@sensor"` |
| One field | `"myproj@sensor:status"` |
| Replace | `"myproj@sensor="+JSON.stringify(cfg)` |
| Merge | `"myproj@sensor\|"+JSON.stringify(partial)` |
| Method | `"myproj@sensor.setup"` |
| Method + args | `"myproj@sensor.modify[ 10 ]"` |

`testcom.html` loads config and status together: `he.load( [ comname, comname+".status" ] )`, then starts/stops with `he.exec( [ comname+".start" ] )`.

Poll without a loading overlay: `he.bkload` / `he.bkexec`.

---

## Language files

Keys are the **English** strings used in `data-i18n="…"` and `$.i18n('…')`.

`sensor-cn.json` (Chinese):

```json
{
    "Custom Status": "自定义状态",
    "Custom Property": "自定义属性",
    "Refresh": "刷新",
    "Apply": "应用",
    "Settings unchanged": "设置未修改",
    "Modify successfully": "修改成功"
}
```

`sensor-en.json` can be a short override (the template only maps a few status words). Shared chrome such as Refresh / Apply is often already in ace; still add keys you use on the page if they are missing.

Keep key text in HTML in English. Do not put Chinese in `data-i18n`.

---

## `wui` fields

| Field | Meaning |
|-------|---------|
| key (`sensor`) | Page id |
| `en` / `cn` | Menu title |
| `menu` | Parent menu group |
| `page` | HTML filename in this project |
| `config` | Bound object; menu is shown when that object exists |
| `lang` | Locale → JSON file in the project |
| `object` / `api` | Alternate visibility check via `com_have` when `config` is unset |
| `mode` | Hide the page when register `network_mode` matches a key whose value is `"disable"` |

Full list: [`../com/land/prj.json.md`](../com/land/prj.json.md) §9. Control cookbook: [`../../project/wui/webpage.md`](../../project/wui/webpage.md).

---

## Build and check

```bash
./mkdel
make obj=myproj
```

Confirm the menu from the device with `land@fpk.wui_menu`. Page and language paths are under `land@fpk.path[ myproj ]` — never assume a fixed `/usr/share/skinos/…` URL after a hot FPK.
