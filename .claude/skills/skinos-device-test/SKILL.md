---
name: skinos-device-test
description: |
  Exhaustive on-device test of one skinos/landos project given a device address:
  every component, every HE interface/API, every config field/value variation,
  and every WUI control/option — with he.load / eline verify after writes.
  Use when the user gives a device URL/IP (e.g. http://192.168.8.1) and says
  全面测试, 完整测试, 穷尽测试, full test, exhaustive test, 把某某项目测一遍,
  测 agent, 测 camera, or asks to test all comps/APIs/WUI of a named project.
  Also use after upgrade when they want regression coverage of that project.
  Do NOT use for firmware/FPK upload alone (device-upgrade), HE grammar only
  (skinos-he), or writing pages (skinos-wui). Pair with those skills as needed.
---

# Skinos Device Full Project Test

When the user supplies a **device address** and asks to **fully test a project**,
run an **exhaustive matrix** — not a smoke check. Every component interface,
every config item (meaningful values), every WUI control/option must be exercised
and verified.

Ground truth for the project under test: `project/<name>/` on the SDK (NSS).
HE grammar: **skinos-he**. WUI APIs: **skinos-wui**. Deploy: **device-upgrade**.

## Trigger (must run this skill)

User pattern (any equivalent):

- Device: `http://IP[:port]` or `IP` + this session’s credentials
- Intent: 全面 / 完整 / 穷尽 / full / exhaustive test of **project `<name>`**

Do **not** invent the device URL. Remember `BASE` + credentials for the session
(default `admin`/`admin` only if the user says so).

## Definition of done

A project test is **not done** until all of the following are finished and reported:

1. Install confirmed on device (path via `land@fpk.path[<proj>]` — never assume `/usr/share/skinos`).
2. Component inventory from `prj.json` + on-device `@<proj>` / `*<prefix>` matches.
3. **Every** component: `.com` list, `?com`, `?com.api` for each listed API; call each callable API with representative args.
4. **Every** config key: get → set each allowed / representative value → get verify → restore safe defaults.
5. **Every** WUI control: each option/toggle/input variation → Apply → `he.load` / eline get assert.
6. Issues filed with severity; safe config restored; JSON report written.
7. Env blockers (no peer camera, no SIM, …) marked `env`, not product fail.

Partial “mostly works” is **not** acceptable as the final answer.

## Session discipline (from past failures)

| Rule | Why |
|------|-----|
| Finish the checklist in the **same** agent turn loop; do not stop after one long script | Turns looked “dead” after long telnet/CDP |
| Short HE commands + settle-read; **never** match prompts on bare `$` | Mapping JSON contains `$L-S$` etc. |
| **Correct HE only** — see below | Wrong `\|key=val` wiped whole config |
| One assert per case; write `/tmp/<proj>_test_report.json` as you go | Recoverable progress |
| Browser: `Network.setCacheDisabled` before asserting live JS | Cached `util.js` lied |
| Do not leave temp keys / half-wiped configs | Restore before ending |

### Forbidden HE (will corrupt config)

```text
WRONG:  camera@osd2he|foo="bar"     # | must be followed by {json}
WRONG:  com|key=value
RIGHT:  com:key=value               # set one attr
RIGHT:  com:key=                    # clear one attr
RIGHT:  com|{"key":"value"}         # merge JSON object
RIGHT:  com={"key":"value"}         # replace entire config object
```

Full grammar: **skinos-he** / `doc/com/land/he.md`.

## Workflow checklist

Copy and tick:

```text
Full project test:
- [ ] 0. Inputs: BASE, user/pass, project name
- [ ] 1. Inventory from SDK sources
- [ ] 2. Confirm install + WUI menu/page on device
- [ ] 3. Eline API matrix (all comps)
- [ ] 4. Eline config matrix (all keys × values)
- [ ] 5. WUI exhaustive matrix + he.load verify
- [ ] 6. Restore safe defaults
- [ ] 7. Report + issues
```

### 0. Inputs

| Input | Source |
|-------|--------|
| `BASE` | User this session |
| Credentials | User / default if allowed |
| Project | e.g. `agent`, `camera` → `project/<name>/` |
| Telnet/SSH | Same creds; prefer eline `$` direct HE |

### 1. Inventory (SDK, before poking device)

From `project/<name>/`:

| Source | Extract |
|--------|---------|
| `prj.json` | `com` / `exe` / `wui` pages |
| `*.cfg` | Default config keys + enums |
| `*.md` / `doc/com/<name>/` | Documented APIs and examples |
| `*.html` | Every input/select/checkbox/button/table row action |
| `cn.json` / `en.json` | Labels (for UI mapping) |

Build three lists: **APIs**, **config keys (+allowed values)**, **WUI steps**.
If docs and code disagree, **code + on-device `.com` / cfg win**; note the doc gap as an issue.

### 2. Install confirm

On device (eline or `he '…'`):

```text
@<project>                 # comps under project dir
land@fpk.path[<project>]   # real install root
?project@comp              # each expected object
```

Open WUI menu path from `prj.json` `wui` / `land@fpk.wui_menu`. Confirm page loads.

### 3. Eline API matrix

For each `project@comp`:

1. `.project@comp` → list APIs; every name must be probeable.
2. `?project@comp` and `?project@comp.api` → expect `ttrue` for registered APIs.
3. Call each API with docs/cfg examples (`method[args]`). Record return.
4. Flag: listed but `?` → `tfalse`; callable but missing from `.` list; `tpanic` when should work.

Do **not** skip APIs that look “internal” unless docs mark them non-callable and `?` is `tfalse`.

### 4. Eline config matrix

For each config key:

1. Read current: `com` or `com:key`.
2. For enums: set **every** enum value; after each, read back.
3. For strings/numbers: several legal values + 1–2 edge (empty, max) if safe.
4. Prefer `com:key=value` / `com:key=` / `com|{…}` / `com={…}` only as appropriate.
5. After the matrix, **restore** the project’s safe default (from `*.cfg` or pre-test snapshot).

Merge check (if object is a free-form map): `com:temp=1` then `com|{"other":"2"}` → both remain + prior keys; clear with `com:temp=` / `com:other=`.

### 5. WUI exhaustive matrix

Drive the project page (browser CDP or equivalent):

1. Enable sections that hide controls (e.g. status checkbox) before testing children.
2. Every control: each select option, each toggle state, representative text values.
3. Dynamic tables: add / edit / delete / restore defaults if the page supports it.
4. After **each** Apply (or equivalent save): `he.load(['project@comp…'])` or eline get; assert UI intent == device config.
5. UX-only widgets (password eye, etc.): still test (programmatic click + live handler); mark severity `low` if config path OK.
6. Hard-reload with cache disabled when verifying shipped JS.

### 6. Restore

Return comps to documented defaults (`*.cfg`). Confirm with a final get. Do not end on a wiped or half-tested config.

### 7. Report

Write `/tmp/<project>_test_report.json` (and short human summary):

```json
{
  "device": "http://…",
  "project": "<name>",
  "install_confirmed": {},
  "eline": { "apis": {}, "cfg": {} },
  "wui": { "cases_passed": [], "cases_failed": [] },
  "issues": [
    { "severity": "high|medium|low|env", "id": "…", "detail": "…" }
  ],
  "verdict": "…"
}
```

Human summary: install OK? matrices complete? open product issues vs env gaps.

## Access tips

| Channel | Use |
|---------|-----|
| Telnet/SSH → eline | Config/API matrix (fast, scriptable) |
| Browser on `BASE` | WUI matrix + live JS |
| HTTP `/he` | Optional; same HE strings (device-upgrade) |

Telnet reader: drain until quiet; strip ANSI; **do not** treat `$` inside JSON as the shell prompt.

## Related skills

| Skill | Role |
|-------|------|
| device-upgrade | Put build on device first |
| skinos-he | HE / eline grammar |
| skinos-wui | Page/`he.js` rules |
| skinos-component-doc | How APIs should be documented |

## Extra detail

Pitfalls and minimal matrices: [reference.md](reference.md)
