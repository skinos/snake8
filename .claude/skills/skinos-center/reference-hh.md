# Remote HE — `hh` / `hline` / heport HTTP `/hh`

Sources: `config/slave/center/heport.md`, `hh/hh.c`, `hline/hline.c`, `skincenter/skincenter.h`.

## Path

```text
Operator ──unix heport.unix──► center@heport ──TLS :20002──► device agent@heclient
```

Device must be **online** (SSL session). Macid is 12 hex chars (no separators).

## CLI `hh`

Argv are concatenated **with no spaces** (same pitfall as ash `he`).

```bash
hh 00037f123450-land@machine.status
hh 00037f123450-land@machine.status:version
hh 00037f123450-land@machine.restart[ 3, heport ]
```

Format: `<12-char-macid>-<HE-line>`. Byte at index 12 must be `-` (`HEPORT_CMD_GAPC`); HE starts at index 13. Argv are concatenated with **no spaces**.

Modes (prefix on the concatenated command, no mac):

| Prefix | Behavior |
|--------|----------|
| (default with mac) | `line_hh_command` once, print result |
| `=` | print mode (`print_hh`) |
| `+` | loop (`loop_hh`) |
| `-` | silent (`slient_hh`) |

Timeout default for line path: **60** s.

Library: `string_hh_execute` / `talk_hh_execute` / `line_hh_command` in `skincenter`.

## CLI `hline`

Interactive `$ ` prompt forwarding every line to that mac:

```bash
hline 00037f123450
$ land@machine.status
$ exit
```

## HTTP `/hh` (heport `api_port`, default 20003)

1. Authenticate on heport HTTP (key lifetime from `center@heport:key_lifetime`)
2. `POST /hh` with username, key, macid, HE body
3. Device must be online and owned by that user

Prefer local `hh` on slave when operating the SDK host. HTTP details follow heport httpd; cloud user pages normally use **userwui** + `center@api`, not raw heport `/hh`.

## Center APIs that already call hh

| API | Device HE (approx.) |
|-----|---------------------|
| `center@api.update` | register refresh via heclient |
| `center@api.reboot` | `land@machine.restart[ … ]` |
| `center@api.default` | `land@machine.default[ … ]` |
| `center@api.firmware_push` | `land@service.start[ heclient-upgrade, arch@firmware, online_upgrade, <url>,,2 ]` |
| nport topology push | `agent@gtog.endpoint` / `branch` / `leaf` |

## After hh: device grammar

Use **skinos-he** for full HE syntax (`component.method`, `set`, `ashy`, returns). This file only covers **reaching** the device through center.

## Failures

- Offline / unknown mac → hh fails (`errno` / empty / error talk)
- Wrong mac format → `EINVAL`
- heport not running → unix control missing
