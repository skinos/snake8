# HE / eline reference (device operation)

Companion to [SKILL.md](SKILL.md). Canonical long docs:  
`doc/com/land/he.md`, `doc/com/land/eline.md`.

---

## Shell `he` argv joining

Implementation concatenates every argument after `he` **with no inserted space**:

| Command | Result |
|---------|--------|
| `he 'land@machine:name'` | OK |
| `he land@machine:name` | OK (single token) |
| `he land@machine name` | **Wrong** → `land@machinename` |

Always: `he '<entire HE line>'` when unsure, or when the line has `| { } [ ] : = ,` or spaces.

---

## Advanced `he` prefixes (ash only)

First character of the **combined** string — **`he` program modes** (not used in eline):

| Prefix | Behavior |
|--------|----------|
| `+` | Loop execute |
| `=` | Parse / print structure only |
| `-` | Silent / minimal output |

```bash
he '+land@machine'
he '=land@machine:name'
he '-land@machine.status'
```

HE **discovery** prefixes (same in eline or `he '…'`):

| Prefix | Behavior |
|--------|----------|
| `*` / `*land` / `*uartdrv` | List by **object-name** prefix (`prefix@…`) |
| `@` / `@land` / `@uart` | List by **project install directory** (`…/<project>/…`) |
| `:` / `:land` | List configuration objects |
| `.land@machine` | List APIs of that component |
| `?land@machine` / `?land@machine.status` | Existence probe → `ttrue` / `tfalse` |

`*` ≠ `@`: name prefix vs project path segment (aliases like `uartdrv@tui` often live under project `uart`). Do not use a leading `.` as an existence probe. Quote `?` / `*` in ash: `he '?wifi@n'`.

Not normal component commands — avoid `+` / `=` / `-` unless you intend those modes.

---

## Eline routing (HE vs shell)

First token → HE if it starts with `* @ ? : .` (not `/` `./` `../` `..`; ash keeps bare `.` / `:`), or matches `/tmp/he.shell`. Else **`shell()`**. Built-ins first: `exit`, `ashy`/`shell`, `cd`, `set` / `set <object>`.

| First token | Route |
|-------------|-------|
| `*land` / `@` / `?x` / `:land` / `.land@machine` | HE (prefix) |
| `land@machine` / `land@machine.status` | HE (whitelist) |
| `ls` / `pwd` / `he` / `foo.sh` / `./x` / `../bin` | shell |
| unknown word | `sh: … not found` |

---

## Interactive `set` line rules

After `$ set <object>` → prompt `object:`:

| Line | Meaning |
|------|---------|
| `attr=value` | Set in-memory field (`=` empty clears) |
| `g` | Reprint in-memory JSON |
| `s` | Save with `sset`, leave set |
| `e` | Exit set without save |
| path only | Print that attribute from in-memory copy |

Ctrl+D in set = abandon (like `e`). Ctrl+D at top-level prompt = **stay** in eline.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `tfalse` / `tfalse, Invalid argument` | Probe miss or unknown object | `?component` / fix the object name |
| `tfalse, Function not implemented` | Probe of a missing API | `.com` to list APIs |
| `tpanic, Function not implemented` | Missing API on **call** | `?com.api` then `.com` |
| Empty after `.com` | List miss (not a probe) | Use `?com` |
| Glued / wrong / silent `he a b` | argv concatenated | `he 'one line'` |
| `he '+…'` hangs | Loop mode | Do not use `+` interactively |
| `*prefix` listed files | cwd glob matched before HE | `he '*prefix'` |
| `ashy`/`shell` then gone | Expected — eline replaced | Reconnect for `$ ` |

---

## Banner help (typical)

```
 @ ----------------------- List all the component
 <com> ------------------- Show component configure
 .<com> ------------------ List all component interface
 ?<com> ------------------ Probe whether component exists
 ?<com>.<api> ------------ Probe whether API exists
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
```

List APIs: **leading** `.` (`.land@machine`). Existence: **leading** `?`.

---

## Prefer web vs terminal

| Situation | Prefer |
|-----------|--------|
| User gave web `BASE`, batch HE / upload | **device-upgrade** (`POST /he`) |
| User gave SSH/telnet, interactive debug | This skill (eline / ash) |
| Need `ashy` tools (`ls`, `swanctl`, …) | SSH/telnet → `ashy` |
