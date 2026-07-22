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

First character of the **combined** string:

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

Not normal component commands — avoid unless you intend these modes.

---

## Eline passthrough prefixes

Whole-line **prefix** match → `shell()`, else HE (`line_he_command`):

| Prefix / rule | Notes |
|---------------|-------|
| `arp ` | space required |
| `ping ` | |
| `traceroute` | first 10 chars (`traceroute`, `traceroute6`, …) |
| `ifconfig` | first 8 chars |
| `route` | `route` or `route …`; **`router` does not match** |
| `netstat` | first 7 |
| `iperf` | first 5 (`iperf3`, …) |
| `tcpdump` | first 7 |
| `mkdir ` | |
| `telnet ` | |
| `wg ` | |
| `ip ` | |
| `tip ` | |
| `cd ` | **`cd` alone → HE** |
| `ls ` | **`ls` alone → HE**; use `ls /` |
| `tftp ` | |
| `curl ` | |
| `reboot` | first 6 chars (`reboot`, `reboot -f`, …) |

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

Ctrl+D in set = abandon (like `e`). Ctrl+D at top-level `$ ` = exit eline.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `tfalse` | Bad path/value/validation | Query parent path; check component `.md` |
| Empty | Missing field / API `NULL` | Query full object or parent |
| Glued / wrong text | Multiple ash argv | Use `he 'one line'` |
| JSON parse errors | Broken JSON / bad quoting | Fix JSON; keep single quotes in ash |
| `ashy` then gone | Expected — eline replaced | Reconnect for `$ ` |
| Passthrough ignored | Wrong prefix (`ls` without path) | Add space + args (`ls /`) |

---

## Banner help (typical)

```
 @ ----------------------- List all the component
 <com> ------------------- Show component configure
 <com>. ------------------ List all component interface
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
```

---

## Prefer web vs terminal

| Situation | Prefer |
|-----------|--------|
| User gave web `BASE`, batch HE / upload | **device-upgrade** (`POST /he`) |
| User gave SSH/telnet, interactive debug | This skill (eline / ash) |
| Need `ashy` tools (`ls`, `swanctl`, …) | SSH/telnet → `ashy` |
