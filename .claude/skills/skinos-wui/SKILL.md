---
name: skinos-wui
description: |
  Author Skinos/landos Web UI pages for project components: HTML form layout,
  prj.json wui registration, cn/en.json i18n, and he.js load/save/call patterns.
  Ground truth is project/wui/ace/api/he.js + util.js and templates
  project/tmptools/page.html / testcom.html (not tiger7 or other branches).
  Use when writing or fixing *.html under project/, adding prj.json "wui",
  "加网页", "WUI", "he.load", "he.exec", "data-i18n", "Apply 按钮", or when a
  page throws "he.xxx is not a function".
  Do NOT invent he.save / he.set / he.get — snake8 he.js has no such APIs.
  Do NOT use for HE shell/eline grammar alone (skinos-he) or full project
  scaffolding without a page (skinos-project).
---

# Skinos WUI (project HTML pages)

Write management pages that run inside **wui@ace** (AJAX fragment + hash route)
and talk to components via **`he.js`**.

**Canonical templates (copy these, not foreign trees):**

| File | Use |
|------|-----|
| [`project/tmptools/page.html`](../../../project/tmptools/page.html) | Minimal load / save / Apply |
| [`project/tmptools/testcom.html`](../../../project/tmptools/testcom.html) | Config + status + `he.exec` methods |
| [`project/wui/ace/api/he.js`](../../../project/wui/ace/api/he.js) | **Only** allowed `he.*` APIs |
| [`project/wui/ace/api/util.js`](../../../project/wui/ace/api/util.js) | `able2boole` / `boole2able` / `ocompare` / `page.*` |
| [`project/wui/webpage.md`](../../../project/wui/webpage.md) | Long-form control cookbook |

Feature pages live at **`project/<proj>/<page>.html`** (project root), **not**
under `project/wui/ace/content/` (that tree is the ace shell’s built-in pages).

---

## Hard rules (read first)

1. **`he` API is snake8-only.** Before any `he.FOO`, confirm `FOO` exists in
   `project/wui/ace/api/he.js`. If unsure, open that file.
2. **Never use** `he.save`, `he.set`, `he.get`, `he.call`, or any API copied
   from tiger7 / another product. Snake8 save/write/call = **`he.exec`**.
3. Prefer **`project/tmptools/*.html`** and sibling pages under the same repo
   (`tui/ssh.html`, `clock/restart.html`) over foreign HTML.
4. After editing HTML/i18n only: `make obj=<proj>` → FPK upload. Confirm the
   **menu page URL** via `land@fpk.wui_menu` / **`land@fpk.path[ <proj> ]`**
   (install root is dynamic by `gBOARDID` / FPK vs image). **Never** assume
   `/usr/share/skinos/...` or any fixed path — older firmware copies there may
   be stale after hot FPK.
5. Comments in new page JS: **English**.

---

## `he.js` surface (complete)

| API | Overlay | Use |
|-----|---------|-----|
| `he.load(cmds [, loading])` | yes | Read config / status (initial load) |
| `he.bkload(cmds)` | no | Background / poll read |
| `he.exec(cmds [, loading])` | yes | **Write config** or call methods |
| `he.bkexec(cmds)` | no | Background write / call |
| `he.cmd(cmds, args, cb)` | optional | Low-level; prefer load/exec |
| `he.reboot(args)` / `he.upgrade_reboot(args)` | progress | Device restart UX |

`cmds` is an **array of HE strings**. Results are `v[0]`, `v[1]`, … in order.

### HE strings used from the page

| Goal | Example |
|------|---------|
| Read full config | `"ddns@scripts"` |
| Read one field | `"ddns@scripts:client/isp"` |
| Replace config | `"ddns@scripts="+JSON.stringify(cfg)` |
| Merge fields | `"ddns@scripts\|"+JSON.stringify(partial)` |
| Call method | `"ddns@scripts.status"` / `"ddns@scripts.setup"` |
| Method + args | `"land@syslog.delete[name]"` |

Same grammar as shell `he '…'` — see **skinos-he** / `doc/com/land/he.md`.

---

## Register the page (`prj.json` → `wui`)

```json
"wui": {
  "scripts": {
    "menu": "Application",
    "en": "DDNS",
    "cn": "动态域名",
    "page": "scripts.html",
    "config": "ddns@scripts",
    "lang": {
      "cn": "cn.json",
      "en": "en.json"
    }
  }
}
```

| Field | Meaning |
|-------|---------|
| key (`scripts`) | Menu entry id (`<proj>_<key>` in `land@fpk.wui_menu`) |
| `en` / `cn` | Menu title |
| `menu` | Parent group; omit → **Application** |
| `page` | HTML filename in **this project root** |
| `config` | Bound object `project@com` (optional but usual) |
| `lang` | Project-relative JSON paths |

Ace loads FPK pages as:

`#app?page=<base64(absolute page path)>&lang=<base64(lang path)>&object=…`

Absolute paths come from **`land@fpk.path[<proj>]`** only (do not hardcode).
Examples of what that API may return (illustrative, board-dependent):
`/mnt/internal/skinos/ddns/…` after hot FPK, or an image-baked tree under the
board’s `PROJECT_DIR` — both are valid; always query.

---

## Page JS pattern (mandatory shape)

Follow `page.html` / `testcom.html`:

```javascript
(function () {
    var object = "myproj@mycom";   // or comname
    var cfg;
    var langjson = base64.decode( page.param( 'lang', location.hash ) );

    function load_cfg() {
        he.load( [ object ] ).then( function (v) {
            cfg = v[0] || {};
            $('#status').prop( 'checked', able2boole( cfg.status ) );
            $('#name').val( cfg.name || '' );
            $('#status').unbind('change').change(function () {
                if ( $(this).prop('checked') ) { $('#statusSets').show(); }
                else { $('#statusSets').hide(); }
            }).trigger('change');
        });
    }

    function save_cfg() {
        if ( !cfg ) { cfg = {}; }
        var copy = JSON.parse( JSON.stringify( cfg ) );
        cfg.status = boole2able( $('#status').prop('checked') );
        if ( cfg.status == "enable" ) {
            cfg.name = $('#name').val();
        }
        if ( ocompare( cfg, copy ) ) {
            page.alert( { message: $.i18n('Settings unchanged') } );
            return;
        }
        // WRITE = he.exec  — never he.save
        he.exec( [ object + "=" + JSON.stringify( cfg ) ] ).then( function () {
            page.hint2succeed( $.i18n('Modify successfully') );
            load_cfg();
        });
    }

    $.i18n().load( page.lang( langjson ) ).then( function () {
        $.i18n().locale = lang;   // or window.lang — both used in-tree
        $('body').i18n();
        load_cfg();
        $('#refresh').on( ace.click_event, function () { location.reload(); } );
        $('#apply').on( ace.click_event, function () { save_cfg(); } );
    });
})();
```

### Helpers (from `util.js`, already global)

| Helper | Role |
|--------|------|
| `able2boole(s)` | `"enable"`/`"disable"` → checkbox bool |
| `boole2able(b)` | checkbox → `"enable"`/`"disable"` |
| `ocompare(a,b)` | deep equal; **true** means unchanged |
| `page.param` / `page.lang` | hash `lang=` / i18n path map |
| `page.password(id, iconId)` | show/hide password |
| `page.timing({ refresh, interval })` | poll without overlay |
| `page.alert` / `page.confirm` / `page.hint2succeed` | UX |

### Status / method calls

```javascript
// poll without blocking overlay
he.bkload( [ object + ".status" ] ).then( function (v) { /* ... */ });

// one-shot action
he.exec( [ object + ".setup" ] ).then( function () { /* ... */ });
```

---

## HTML / i18n conventions

- Skeleton: `form-horizontal` + `form-group` + Refresh (`btn-second`) / Apply (`btn-main`).
- Labels: `data-i18n="English Key"`; keys go in `cn.json` / `en.json`.
- Enable switch: `ace ace-switch ace-switch-6` + `able2boole` / `boole2able`.
- Separators: `<div class="hr hr32 hr-dotted"></div>` (see peer pages).
- Keep markup as an **AJAX fragment** (no full `<html>` document).

---

## Checklist before saying “done”

- [ ] Every `he.*` call exists in `project/wui/ace/api/he.js`
- [ ] Save path uses **`he.exec([ obj+"="+JSON.stringify(cfg) ])`**
- [ ] `prj.json` `wui.page` / `lang` files exist in the project
- [ ] `data-i18n` keys covered in `cn.json` (and `en.json` if needed)
- [ ] Built FPK uploaded; verified page URL via `land@fpk.wui_menu` (hard-refresh browser)
- [ ] Component `_get` / `_set` (or default config) match what the page reads/writes

---

## Related skills

| Skill | When |
|-------|------|
| **skinos-project** | New project / com / prj.json beyond the page |
| **skinos-component-doc** | Component HE/API markdown next to the com |
| **skinos-he** | Live device HE / eline / `ashy` |
| **device-upgrade** | Upload FPK / `.zz` |

More control recipes and jqGrid notes: [`reference.md`](reference.md) and
`project/wui/webpage.md`.
