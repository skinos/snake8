---
name: skinos-center
description: |
  Operate the center cloud platform: admin users, gateways under a cloud user,
  hh/hline HE to a device by mac, pport penetrate (HTTP/telnet/ssh/eline), mesh
  network/endpoint (pref/relay), firmware library match and OTA push.
  Use when the user says "云平台", "center", "ashyelf 下的设备", "hh 发指令",
  "穿透", "gateway_http/telnet", "组 mesh", "firmware_push", "查有多少用户",
  or asks to manage devices through center (not direct device SSH alone).
  HARD: before any .zz OTA or FPK install, match device model; mismatch → stop.
  Do NOT use for device HE grammar (skinos-he), direct device /upload without
  center (device-upgrade), or building/installing center on slave (skinos-slave).
  Sources: config/slave/center/{api,ctrl,heport,pport,nport,userwui}.md and userdir/.
---

# Skinos Center Cloud Management

Operate **center** on the SDK host (or remote userwui) so gateways that dial in
can be listed, commanded, penetrated, meshed, and OTA-upgraded.

Authoritative API docs live under `config/slave/center/`. This skill is the
**operator playbook**; details are in `reference-*.md`.

## Session inputs (do not invent)

| Need | Rule |
|------|------|
| Cloud username | User gives once per chat (e.g. `ashyelf`); remember for the session |
| Center host | Default: this slave SDK host (`he` / `hh` in `~/snake8`). Remote userwui `BASE` only if user gives it |
| Credentials | Admin ops use local `he` (no cloud password). HTTP path needs user/pass once; do not reuse doc examples |
| Target macid | 12-char id (e.g. `00037f123450`); remember when given |

Never invent macids, passwords, or BASE URLs from docs.

## HARD RULE — model match before upgrade / FPK

Before **`firmware_push`** or any penetrate-then-**device-upgrade** `.zz`/FPK upload:

1. Read device: `hh <macid>-land@machine.status` (prefer) or `center@heport/<user>/dev/<mac>/reg` → nested `land@machine.status`
2. Compare to package: firmware `{hardware}_{custom}_{scope}_{version}[_{oem}].zz` / FPK/`gBOARDID` rules from **device-upgrade** Step 0 (platform leading-`s` compatible)
3. **Any mismatch → stop**, report device vs package fields; **do not** push/upload
4. Only then upgrade or install

## Default ports (slave center)

| Service | Default | Role |
|---------|---------|------|
| `center@userwui` | TCP **20000** | Cloud WUI `/auth` `/he` |
| `center@heport` | TCP **20002** SSL / **20003** HTTP | Device TLS + `/auth` `/hh` |
| `center@nport` | UDP **20002** / **20003** | Mesh register / NAT test (same numbers, UDP) |
| `center@pport` | TCP **20005** | Device `agent@portc` pond |
| Dynamic maps | **20006–24999** | `gateway_*`, ttyd, mesh relay UDP |
| Static maps | **≥25000** | User `tcpmap` / `udpmap` |

## How to talk to center

**Primary (local slave):**

```bash
he 'center@ctrl.user_list'
he 'center@api.list[ ashyelf ]'
hh 00037f123450-land@machine.status
hline 00037f123450
```

**Secondary (remote HTTP userwui):** same auth rhythm as **device-upgrade** (`/public` → proof → `/auth` → `/he`), object methods are `center@api.*` (cloud helist). Admin `center@ctrl.*` is **not** on userwui helist — use local `he` or admin WUI.

## Aspects (read when needed)

| ID | Topic | File |
|----|--------|------|
| A2–A3 | Admin + self-service users | [reference-users.md](reference-users.md) |
| A4 | Gateway list / knock / reboot / delete | [reference-gateway.md](reference-gateway.md) |
| A5 | `hh` / `hline` / HTTP `/hh` | [reference-hh.md](reference-hh.md) |
| A6–A7 | Persistent maps + temporary remote access | [reference-pport.md](reference-pport.md) |
| A8 | Mesh network / endpoint / relay / pref | [reference-mesh.md](reference-mesh.md) |
| A9 | Firmware library + OTA + model match | [reference-firmware.md](reference-firmware.md) |
| A10 | userdir layout, dump, ports, deps | [reference-data.md](reference-data.md) |

## Playbook: upgrade by mac (e.g. `00037f123450`)

```text
1. Locate — center@api.list[ user ] (or scan users); confirm online
2. Model match — hh <mac>-land@machine.status vs target .zz/FPK → mismatch STOP
3. OTA — firmware_list → pick matching zz → build download URL → firmware_push
4. Optional penetrate — gateway_http / gateway_telnet / gateway_hline → then
   skinos-he (HE) or device-upgrade (upload) with model check again
5. One-shot HE — hh <mac>-<command> (no penetrate needed)
```

## Playbook: mesh (e.g. ashyelf / mac xxx / net `ddd` / xxx relay+master)

```text
1. list[ ashyelf ] — device exists (online better)
2. network_list — if no ddd → network_add[ ashyelf, ddd, 172.16.0.0/24 ]
3. endpoint_add[ ashyelf, ddd, xxx, , , 100, , , , enable ]
   master = high pref; relay = enable (NAT hub only; ordinary peers stay disable)
4. Other members: endpoint_add with lower pref
5. Check relay_max on center@ctrl; knock / dump for relay_port / online
```

Semantic: **master** is not a separate API — higher reachable **`pref`** wins hub election. **`relay`** = center public UDP wish (`auto`/`enable`/`disable`).

## Cheat sheet

```bash
# Users (admin)
he 'center@ctrl.user_list'
he 'center@ctrl.user_add[ ashyelf,Pass123,,en, Test ]'
he 'center@ctrl.user_delete[ ashyelf ]'

# Gateways
he 'center@api.list[ ashyelf ]'
he 'center@api.reboot[ ashyelf,00037f123450 ]'

# HE to device (mac 12 hex + '-' + HE line; argv glued with no spaces)
hh 00037f123450-land@machine.status
hline 00037f123450

# Penetrate
he 'center@api.gateway_http[ ashyelf,00037f123450 ]'    # → public port
he 'center@api.gateway_telnet[ ashyelf,00037f123450 ]'
he 'center@api.gateway_hline[ ashyelf,00037f123450 ]'

# Mesh
he 'center@api.network_add[ ashyelf, ddd, 172.16.0.0/24 ]'
he 'center@api.endpoint_add[ ashyelf, ddd, 00037f123450, , , 100, , , , enable ]'
he 'center@api.network_dump[ ashyelf, ddd ]'

# Firmware
he 'center@api.firmware_list[ ashyelf ]'
he 'center@api.firmware_push[ ashyelf,<url>,00037f123450 ]'
```

## Related

| Skill | When |
|-------|------|
| **skinos-he** | After penetrate / hh: HE grammar on the device |
| **device-upgrade** | Penetrate then `/upload` `.zz`/FPK; board match Step 0 |
| **skinos-slave** | Build/install/run center on this SDK host |

Sources: `config/slave/center/api.md`, `ctrl.md`, `heport.md`, `pport.md`, `nport.md`, `userwui.md`, `userdir/`.
