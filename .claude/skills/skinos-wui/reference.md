# Skinos WUI — reference

Companion to [SKILL.md](SKILL.md). Prefer the skill for daily work; use this for
anti-patterns and where to read more.

## Source of truth

| Topic | File |
|-------|------|
| Allowed `he.*` | `project/wui/ace/api/he.js` |
| `page.*`, `able2boole`, `ocompare` | `project/wui/ace/api/util.js` |
| Minimal page | `project/tmptools/page.html` |
| Config + status + actions | `project/tmptools/testcom.html` |
| Simple real pages | `project/tui/ssh.html`, `project/clock/restart.html` |
| Cookbook (controls, validation, jqGrid) | `project/wui/webpage.md` |
| Menu merge of FPK pages | `project/wui/ace/js/index.js` (`land@fpk.wui_menu`) |

## Anti-patterns

| Wrong | Right |
|-------|--------|
| `he.save(...)` | `he.exec([ obj+"="+JSON.stringify(cfg) ])` |
| `he.set` / `he.get` / `he.call` | `he.exec` / `he.load` / `he.bkload` |
| Copy HTML/JS from tiger7 or another branch | Copy `tmptools/page.html` or a snake8 peer page |
| Assume `/usr/share/skinos/<proj>/…` after FPK | Check `land@fpk.wui_menu` → `page` (often `/mnt/internal/skinos/…`) |
| Put feature HTML under `project/wui/ace/content/` | Put it in `project/<proj>/` and register `prj.json` `wui` |
| Invent `he` helpers “like the other product” | Open `he.js` first |

## Load vs exec (mental model)

- **load / bkload** — HE **query** (config get, `.status`, `.list`, …).
- **exec / bkexec** — HE **mutate or run** (`obj={…}`, `obj:path=val`, `.setup`, `.shut`, …).

Both wrap `he.cmd`. Overlay text differs; behavior is the same HE channel.

## `page.lang` for project pages

FPK menus pass an absolute lang path (e.g. `/mnt/internal/skinos/ddns/cn.json`)
via hash `lang=` (base64). Decode with:

```javascript
var langjson = base64.decode( page.param( 'lang', location.hash ) );
$.i18n().load( page.lang( langjson ) )
```

If `langjson` starts with `/`, `page.lang` uses it as-is for `window.lang`.

Built-in ace pages use short names: `page.lang('mypage')` → `/lang/mypage-<lang>.json`.

## FPK vs rootfs HTML dual copies

If a project was once baked into firmware (`/usr/share/skinos/<proj>/`) and later
updated only via FPK (`/mnt/internal/skinos/<proj>/`):

- Menu from `land@fpk.wui_menu` points at the **FPK path** (correct).
- Manually opening `/skinos/<proj>/….html` may still hit the **old** rootfs file.

When debugging “I uploaded but UI still broken”, curl both URLs and compare
`he.exec` vs `he.save` (or other stale JS). Prefer hard-refresh from the menu.

## Longer cookbook

`project/wui/webpage.md` covers:

- Control mapping (text / select / switch / password / tables)
- Validation helpers (`check.*`)
- Conditional panels
- Full appendices for `he.cmd` object form and uploads
