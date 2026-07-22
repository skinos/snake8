---
name: skinos-he
description: |
  Operate a running landos/skinos device via HE grammar on eline ($), BusyBox ash
  (he '…'), or the classic HE loop (#). Covers login, prompt rules, config get/set,
  method calls, set mode, ashy, discovery (@ / com.), and return codes.
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

# ash
~ # he 'land@machine.status'
~ # he 'land@machine:name=DemoGateway'
~ # exit          # often disconnects; reconnect for a new eline
```

**Critical:** In ash, `he` concatenates argv **with no spaces**. Always prefer  
`he 'one complete HE line'`. Never `he land@machine name`.

## Connect

Telnet / SSH / serial (Command Line UART, typically 57600 8N1). After login, use the prompt you actually see.

Leave eline: `exit` or Ctrl+D at `$ `.  
`ashy` → `/bin/ash --login`. Leaving ash usually **kills the whole session** (eline was replaced). Open a second session for risky tests.

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
| `@` | List components |
| `component.` | List methods/interfaces for that object |
| `*project` | Filter components by project (when supported) |

## Returns

| Output | Meaning |
|--------|---------|
| JSON `{…}` | Structured data |
| Plain string | Scalar value |
| `ttrue` / `tfalse` | Success / failure |
| Empty | No data (`NULL`) |
| `terror` / `tpanic` | Error / panic (see component docs / errno hints) |

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

### Passthrough OS lines

Matched by **prefix** → `shell()`, not HE. Common: `ping `, `ip `, `ifconfig`, `route`, `ls /…`, `curl `, `reboot…`.  
`ls` / `cd` alone (no space) are **HE**, not shell — use `ls /` / `cd /tmp`.

Full list: [reference.md](reference.md).

### Built-ins

| Input | Action |
|-------|--------|
| `exit` | Quit eline |
| `ashy` | Replace with BusyBox ash |
| `set <object>` | Interactive config mode |

## How to operate from a component doc

1. Open `doc/com/land/<name>.md` (or project doc) → note **component id** and **Configuration** / **API Reference**.
2. Query: `$ land@syslog` / `$ land@syslog:level`
3. Change: `$ land@syslog:remote=…` or `$ land@syslog\|{…}`
4. Call: `$ land@syslog.show` / `$ land@machine.restart[3,upgrade]`
5. Or `set land@syslog` for multi-field edits, then `s`.

## Agent workflow checklist

```
- [ ] Have SSH/telnet (or user said use web /he → device-upgrade)
- [ ] Detect prompt: $ / # / ~ #
- [ ] Prefer eline HE for interactive checks; ash + he '…' for scripts/OS tools
- [ ] Single-quote every he payload in ash
- [ ] Do not treat first ashy exit as “back to eline” — reconnect if needed
- [ ] Map failures via return table; re-read component .md for API shape
```

## Related

| Skill / doc | Use for |
|-------------|---------|
| [reference.md](reference.md) | Passthrough list, `he` `+/=/-` modes, troubleshooting |
| **device-upgrade** | Web `/auth` `/he` `/upload`, firmware/FPK |
| `doc/com/land/he.md` | Full HE examples |
| `doc/com/land/eline.md` | Full eline / set / passthrough |
| `doc/com/land/README.md` | Land overview + component index |
