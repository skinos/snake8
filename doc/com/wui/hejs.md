# Frontend `he.js`: component configuration and component APIs

This document explains how admin Web UI code uses the **`he` object** from **`ace/api/he.js`** to send HE commands to the device **`/he`** endpoint—**read or write component configuration** and **invoke component methods**. HE command grammar matches the terminal / eline HE line; see **`doc/com/land/he.md`** in the repository for the full grammar.

---

## 1. Server endpoint and policy

Browser code executes HE by POSTing JSON to **`/he`** (path is usually **`window.hepath`** → `"/he"` on authenticated pages). The server validates the session **`key`**. Which HE lines are accepted can be restricted by device configuration (e.g. a per-service allow list such as **`helist`** on **`wui@admin`**); if unset, a built-in default list may apply.

---

## 2. Script include and globals

### 2.1 Include

```html
<script src="/api/he.js"></script>
```

The page must already load **jQuery** (`$.ajax`, `$.Deferred`) and framework helpers such as **`page`** (`load` / `exec` call `page.overlay`, `page.alert`, etc.).

### 2.2 Typical shell setup after login

The main shell (e.g. **`ace/js/index.js`**) on logged-in pages usually sets:

| Variable | Role |
|----------|------|
| `window.hepath` | HE endpoint path, typically `"/he"` (login pages may use `"/public"`) |
| `window.talkkey` | Session key, often restored from `sessionStorage.getItem("talkkey")` |
| `window.username` | Current user name from `sessionStorage` (or equivalent) |

`he.cmd` always adds **`key`** and **`username`** to the POST JSON. If the response contains a new **`key`**, it updates **`window.talkkey`** and **`sessionStorage`**.

---

## 3. HE command strings (what to pass as `a`)

Each entry is **one HE line as a string**, same categories as in the terminal docs:

| Goal | HE form (examples) |
|------|---------------------|
| Read full component config | `"land@machine"` |
| Read one attribute | `"land@machine:name"` |
| Set one attribute | `"land@machine:name=DemoGateway"` |
| Replace full config object | `"modem@lte=" + JSON.stringify(config)` (or any string that is `component={json}`) |
| Merge selected fields | forms with `\|{json}` as in **`he.md`** |
| Clear one attribute | `"component:attr="` |
| Call method, no parameters | `"land@machine.status"` |
| Call method with parameters | `"ifname@lte.lock_imei[123456789012345]"` |
| One field from method JSON | `"land@machine.status:version"` |

In UI code:

- Use an **array** for multiple commands in order; map results with **`v[0]`, `v[1]`, …** (see encoding below).
- Use an **object** to name results: keys are your property names, values are HE strings; read **`v.yourKey`** in the callback.

---

## 4. Core: `he.cmd(a, args, func)`

Behavior in **`ace/api/he.js`**:

1. **Request**: `POST` to **`window.hepath + "?rand=" + Math.random()`**, `Content-Type: application/json`, JSON body.
2. **Command encoding**:
   - If **`a`** is an **array**: index 0 → JSON field **`he`**, index 1 → **`he1`**, index 2 → **`he2`**, etc.
   - If **`a`** is an **object**: same **`he` / `he1` / …** mapping in enumeration order, while **original keys** are remembered to rebuild the result object.
3. **Fixed fields**: always **`key`** (`window.talkkey`) and **`username`** (`window.username`).
4. **`args` (optional)**:
   - **`timeout`**: request timeout in ms; if omitted, `0` (jQuery default applies).
   - **`loading`**: if truthy, **`page.overlay(args.loading)`** for the request, **`page.overlay2hide()`** when done.
5. **Async vs sync**:
   - **`func` provided**: asynchronous; **`func(ret)`** when finished.
   - **`func` is `null`**: synchronous; return value is the parsed result object (or a string on some error paths).
6. **Response handling**:
   - If the body does **not** start with `{`: treated as plain text / error; **`"Auth Error"`** redirects to **`login.html`**; **`"Data Error"`** is logged; async path passes the string to **`func`**.
   - If JSON: read **`he` / `he1` / …** and map back to array indices or object keys.
   - String decoding: **`"ttrue"` / `"true"`** → boolean **`true`**; **`tfalse` / `false`…** → **`false`**; **`terror` / `tpanic`…** → **`false`** (logged); **`"NULL"`** → **`null`**.
7. **Reboot guard**: if **`window.rebooting`** is set, sync **`he.cmd`** returns **`null`**; async **`complete`** returns early so late responses are ignored.

### 4.1 Array example

```javascript
he.bkload(["land@machine.status"]).then(function (v) {
    var machines = v[0];   // result of the first command in the array
});
```

Multiple commands:

```javascript
he.bkload(["wifi@nssid.status", "wifi@assid.status"]).then(function (v) {
    var nssid = v[0];
    var assid = v[1];
});
```

### 4.2 Object example (named results)

To get **`v.machine`** instead of **`v[0]`**, pass an object (keys are yours, values are HE strings):

```javascript
he.cmd({
    machine: "land@machine",
    cpu: "land@machine.cpuinfo"
}, null, function (v) {
    // v.machine and v.cpu hold the two command results
});
```

---

## 5. Promise-style helpers

All wrap **`he.cmd`**. If the outcome is a **string**, **`page.alert`** with **`$.i18n`** is used; on success they resolve an **object**.

| API | Behavior |
|-----|----------|
| **`he.load(a, loading)`** | Full-screen loading overlay (`loading` or default i18n **Loading**), then hide; returns **`$.Deferred().promise()`** |
| **`he.exec(a, loading)`** | Same with default **Running** text |
| **`he.bkload(a)`** | No overlay |
| **`he.bkexec(a)`** | No overlay |

Typical pattern: chain **`then`** and update the DOM.

```javascript
he.load([
    "modem@lte",
    "ifname@lte.status"
], $.i18n("Loading")).then(function (v) {
    var config = v[0];
    var status = v[1];
    // bind to DOM …
});
```

Submit full configuration (same style as **`lte.js`**):

```javascript
he.exec([object + "=" + JSON.stringify(config)]).then(function () {
    page.hint2succeed($.i18n("Modify successfully"));
});
```

ON-OFF style APIs such as **`setup` / `shut`**:

```javascript
he.exec(["ifname@wan.setup"]).then(function (result) { /* … */ });
he.exec(["ifname@wan.shut"]).then(function (result) { /* … */ });
```

---

## 6. Reboot helpers

| API | Role |
|-----|------|
| **`he.reboot(args)`** | Shows reboot progress, sets **`window.rebooting = true`**, runs HE strings in **`args.cmds`** (array), then appends **`machine.restart`** |
| **`he.upgrade_reboot(args)`** | Same pattern with a longer default wait, intended after firmware upgrade |

Optional **`args`**: **`title`**, **`restartTime`** (or **`window.custom.restart_time`** / **`upgrade_wait`**), **`href`**, **`hint`**, **`cmds`**. When the bar finishes, the UI prompts and **reloads** or **navigates**.

---

## 7. Recommended UI patterns

1. **Read-only views**: batch reads with **`he.load`** or **`he.bkload`**, render inside **`then`**; use **`bkload`** on polling pages to avoid flickering overlays.
2. **Forms**: after validation, **`he.exec`** with **`component:attr=value`**, **`component={json}`**, or merge forms from **`he.md`**; then show a hint or **`load`** again.
3. **Start/stop services**: **`he.exec`** on **`.setup` / `.shut`**, then refresh status.
4. **Errors**: **`load` / `exec`** already surface string errors; if you call **`he.cmd` directly**, handle **`typeof ret === "string"`** (including auth redirect side effects from the transport layer).
5. **Denial / whitelist**: if commands fail policy-side, check the device’s Web UI service configuration for allowed HE patterns and match mode (**`sub`**, **`equal`**, prefix).

---

## 8. Quick reference

- **Include**: `<script src="/api/he.js"></script>`
- **Low level**: **`he.cmd(commandArrayOrObject, { timeout, loading }, callback)`**
- **High level**: **`he.load` / `he.exec` / `he.bkload` / `he.bkexec`**
- **Arrays**: **`v[i]`** is the result of the **`i`**-th command in order.

Full HE syntax, merge/pipe rules, and special prefixes are defined in **`doc/com/land/he.md`**. This file documents **`he.js`** request/response mapping for **`project/wui`** page code.
