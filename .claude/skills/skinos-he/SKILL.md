---
name: skinos-he
description: |
  Operate a running landos/skinos device via HE grammar on eline ($), BusyBox ash
  (he '…'), or the classic HE loop (#). Covers login, prompt rules, config get/set,
  method calls, set mode, ashy, discovery (@ / .com / ?com), and return codes.
  Use when the user says "he 命令", "eline", "ashy", "在设备上查一下", "telnet/SSH
  操作", "调一下组件", "land@machine.status", or asks how to query/change config
  or call APIs on a live gateway.
  Do NOT use for firmware/FPK upload (device-upgrade) or writing new components
  (skinos-project / skinos-component-doc). Full grammar samples: doc/com/land/he.md
  and eline.md.
---

# Skinos Device HE / Eline

Operate the **running** gateway with HE. Source of truth for examples:  
`doc/com/land/he.md` · `doc/com/land/eline.md` (copies under `project/land/`).

Upload/auth/web API: **device-upgrade**. This skill is **terminal HE** (and when to prefer it).

## Session inputs (do not invent)

| Need | Rule |
|------|------|
| SSH / telnet `IP:port` | User gives **once per chat**; remember for the session |
| Username / password | Same as web unless user says otherwise; default only if they say so |
| Web `BASE` | Optional; for HTTP `/he` use **device-upgrade** |

Never reuse lab addresses or passwords from docs/examples.

## Prompt → how to type HE

| Prompt | Mode | How to run HE |
|--------|------|----------------|
| `$ ` | **eline** | Type HE **directly** — no `he` prefix |
| `# ` | Classic HE loop (some images) | HE **directly**; `elf` → BusyBox if banner says so |
| `~ #` / ash `#` | Linux shell after **`ashy`** | **`he '<full line>'`** — **one** single-quoted argument |

```text
# eline
$ land@machine.status

# enter BusyBox (replaces eline — exit usually ends the login)
$ ashy
$ shell

# ash
admin@host:~# he 'land@machine.status'
~ # he 'land@machine:name=DemoGateway'
~ # exit          # often disconnects; reconnect for a new eline
```

**Critical:** In ash, `he` concatenates argv **with no spaces**. Always prefer  
`he 'one complete HE line'`. Never `he land@machine name`.

## Connect

Telnet / SSH / serial (Command Line UART, typically 57600 8N1). After login, use the prompt you actually see.

Leave eline: type **`exit`**. Ctrl+D at the top-level prompt **stays** in eline.  
`ashy` / `shell` → `/bin/ash -i`. Leaving ash usually **kills the whole session** (eline was replaced). Open a second session for risky tests.

## HE grammar (payload)

Same string in eline or inside `he '…'`:

| Goal | Format | Example payload |
|------|--------|-----------------|
| Full config | `component` | `land@machine` |
| One attr | `component:attr/path` | `land@machine:name` |
| Nested path | `…:a/b/c` | `ifname@lan:static/ip` |
| Set one | `component:path=value` | `land@machine:name=DemoGateway` |
| Clear one | `component:path=` | `gnss@nmea:client=` |
| Replace all | `component={json}` | `forward@alg={"ftp":"enable"}` |
| Merge | `component\|{json}` or `component:path\|{json}` | `land@syslog\|{"level":"info"}` |
| Call API | `component.method` | `land@machine.status` |
| Call + args | `component.method[p1,p2]` | `clock@date.ntpsync[ntp1.aliyun.com]` |
| Empty leading arg | keep comma | `land@auth.add[,user,pass]` |
| One field from API JSON | `component.method:path` | `land@machine.status:version` |

Discovery (from banner / practice):

| Line | Meaning |
|------|---------|
| `@` | List all components |
| `*prefix` | Filter by **object-name** prefix (`*uartdrv` → names `uartdrv@…`) |
| `@project` | Filter by **project install dir** (`@uart` → path `…/uart/…`; not the same as name prefix) |
| `.component` | List methods/interfaces for that object (**leading** `.`, e.g. `.land@machine`) |
| `?component` | Probe whether the component exists (`ttrue` / `tfalse`) |
| `?component.method` | Probe whether that API exists (`ttrue` / `tfalse`) |

`*` and `@` are different filters — do not swap them (e.g. `@uartdrv` is empty if there is no project dir `uartdrv`). Do not use a leading **`.`** as an existence probe. In ash, quote `?` / `*` (`he '?wifi@n'`). WUI `POST /he` sends the HE string in JSON, so `?` is not a URL query. Tab completion uses **`hetab`** (not `he '?…'`).

## Returns

| Output | Meaning |
|--------|---------|
| JSON `{…}` | Structured data |
| Plain string | Scalar value |
| `ttrue` / `tfalse` | Success / logical failure (probes use these; `?com.badapi` → `tfalse, Function not implemented`) |
| Empty | No printable payload (`NULL`, or `.` on a missing object) |
| `tpanic` | Dispatch failure (missing API often **`tpanic, Function not implemented`**) |
| `terror` | Parse / peer error (bare `?` → **`terror, Invalid argument`**) |

Next prompt returns after each command.

## Eline extras (`$ ` only)

### `set <object>` — edit config interactively

```text
$ set land@machine
land@machine: name=DemoGateway
land@machine: g          # reprint in-memory JSON
land@machine: s          # save (sset) → ttrue/tfalse, leave set
land@machine: e          # abandon without save
```

Paths after `object:` are **relative** (no repeated `land@machine:`). Ctrl+D in set = abandon like `e`.

### OS lines vs HE

First token: leading `* @ ? : .` → HE (not `/` `./` `../` `..`; ash keeps bare `.` / `:`). Else only `/tmp/he.shell` (exact or name + `. : = | [`). **`he …`** is the PATH binary. **`cd`** is in-process `chdir`. Bare **`set`** prints `set <object>`.

### Built-ins

| Input | Action |
|-------|--------|
| `exit` | Quit eline |
| `ashy` / `shell` | Replace with `/bin/ash -i` |
| `cd` / `cd …` | In-process directory change |
| `set <object>` | Interactive config mode |

## How to operate from a component doc

1. Open `doc/com/land/<name>.md` (or project doc) → note **component id** and **Configuration** / **API Reference**.
2. Query: `$ land@syslog` / `$ land@syslog:level`
3. Change: `$ land@syslog:remote=…` or `$ land@syslog\|{…}`
4. Call: `$ land@syslog.show` / `$ land@machine.restart[3,upgrade]`
5. Or `set land@syslog` for multi-field edits, then `s`.

## On-device project files (`land@fpk.path`)

When you need a project’s install directory, binaries (`bin/`), `prj.json`, or package assets on the device:

```text
$ land@fpk.path[ gnss ]
~ # he 'land@fpk.path[ gnss ]'
```

**Do not** assume `/usr/share/skinos/…` (or any fixed path). Location varies by board/`gBOARDID` and FPK vs image install. Prefer `land@fpk.path[ <project> ]` or `land@fpk.list`. Details: **device-upgrade** + `doc/com/land/fpk.md`.

## Agent workflow checklist

```
- [ ] Have SSH/telnet (or user said use web /he → device-upgrade)
- [ ] Detect prompt: $ / # / ~ #
- [ ] Prefer eline HE for interactive checks; ash + he '…' for scripts/OS tools
- [ ] Single-quote every he payload in ash
- [ ] Project files/binaries: land@fpk.path[ name ] — never hardcode /usr/share/skinos
- [ ] Do not treat first ashy exit as “back to eline” — reconnect if needed
- [ ] Map failures via return table; re-read component .md for API shape
```

## Related

| Skill / doc | Use for |
|-------------|---------|
| [reference.md](reference.md) | Routing, `he` `+/=/-` modes, troubleshooting |
| **device-upgrade** | Web `/auth` `/he` `/upload`, firmware/FPK |
| `doc/com/land/he.md` | Full HE examples |
| `doc/com/land/eline.md` | Full eline / set / HE vs shell routing |
| `doc/com/land/README.md` | Land overview + component index |
