# Projects under `project/` — domains, layering, where to add features

Catalog of Skinos **domains**: what each does, what you typically extend, and **where to put new work** (module drivers, serial apps, routing, …).

| Related doc | Role |
|-------------|------|
| [`project.md`](project.md) | What **one** Skinos project is (parts / `prj.json`) |
| [`README.md`](README.md) | SDK portal |
| [`.claude/skills/skinos-modem/`](.claude/skills/skinos-modem/) | Add cellular **module drivers** under `modem` |
| [`.claude/skills/skinos-uart/`](.claude/skills/skinos-uart/) | Add **UART protocol apps / drivers** under `uart` |

Projects are **not** isolated apps: they register components into one runtime (`project@key`, or `obj` aliases). **Not every board ships every project** — product images select a subset via `config/` / `sdk.config`.

---

## Quick decision: where do I add X?

| You want to add… | Put it in | How / notes |
|------------------|-----------|-------------|
| **Cellular USB modem driver** (Quectel/Fibocom/…) | **`modem`** | New `com` + `obj` `usbdrv@…`; copy `rm500u/` or `ec2x/`. Skill: **skinos-modem** |
| **LTE uplink / dial policy** (APN, PPP, failover) | **`ifname`** (+ uses `modem@lte`) | `ifname@lte` / `ltecon`; see `doc/com/ifname/lte.md` |
| **Wi‑Fi radio / AP / STA** | **`wifi`** | Often + kernel/firmware under `config/<platform>/` |
| **Serial protocol / DTU / NMEA / custom UART app** | **`uart`** | New `exe` + `obj` `uartdrv@…`; copy `dtu/` or `gnss/`. Skill: **skinos-uart** |
| **TTY port count / default baud / which driver** | **`uart`** + board **`config/…/uart/tty*.cfg`** | Instances `uart@tty`…`tty8` managed by `uart@frame` |
| **Interactive shell over serial** | **`uart`** (`uartdrv@tui`) | Or use existing `tui` driver |
| **Telnet / SSH into eline** | **`tui`** | Network terminal access (not UART hardware) |
| **Bridge / VLAN / hosts / keep-alive** | **`network`** | L2/L3 plumbing |
| **NAT / firewall / route table** | **`forward`** | Often joint on `network/*` |
| **LAN DHCP / client list / ACL** | **`client`** | |
| **IPsec / WireGuard / L2TP/PPTP/GRE** | **`ipsec` / `nvpn` / `vpn`** | |
| **GNSS as a product feature (NMEA objects)** | **`gnss`** and/or **`uart`** `uartdrv@gnss` | Device on serial → uart driver; system GNSS objects → gnss project |
| **Camera / OSD** | **`camera`** | |
| **Remote/cloud HE / MQTT / port map** | **`agent`** | |
| **Web admin page / menu** | **`wui`** (+ page assets); HTTP server is **`webs`** | |
| **Auth / log / service / FPK / init·joint** | **`land`** | Do **not** put hardware drivers here |
| **Board USB/GPIO/PCI/ethernet glue** | **`arch`** (if present) or **`config/<platform>/`** kernel/rootfs | USB match tables for modems live with `arch@usb` |
| **Brand-new domain with no fit** | **New `project/<name>/`** from `tmptools` | Prefer a new project over stuffing `land` |

---

## Layering (conceptual)

```text
       +---------------------------+       +---------------------------+
       | wui → webs@httpd          |       | tui (telnet/ssh)          |
       +-------------+-------------+       +-------------+-------------+
                     \                               /
                      \   same HE via land          /
                       v                            v
                 +-----------------------------------------+
                 | land: he, eline, daemon, auth, …        |
                 +--------------------+--------------------+
                                      |
            +-------------------------+-------------------------+
            v                         v                         v
      network / ifname / wifi / modem / uart / vpn…
            |
            +---- forward / client (react to network/* joints)
```

- **land** = management base.  
- **modem** drivers expose `usbdrv@*` → runtime `modem@lte*`.  
- **uart** drivers expose `uartdrv@*` → bound per `uart@tty*`.  
- **ifname** sits between hardware (modem/wifi/eth) and **network@frame**.

---

## Domain catalog (what exists + what you extend)

| Project | Purpose | Typical extend points |
|---------|---------|------------------------|
| **land** | Framework, HE/eline, auth, syslog, service, FPK, init/joint/uninit | New core services only; **not** device drivers |
| **network** | hosts, frame, vlan, bridge, keeplive; `connect` | New L2/L3 helpers; frame scheduling of ifnames |
| **ifname** | Connection engines (`ethcon`, `ltecon`) | New link types; LTE policy using `modem@lte` |
| **wifi** | AP / STA | New radio modes; platform Wi‑Fi firmware in `config/` |
| **modem** | ATD/SMSD + **USB module drivers** (`ec2x`, `rm500u`, `mt5710`, …) | **New module driver com** + `usbdrv@` alias → **skinos-modem** |
| **uart** | Port manager (`frame`) + **serial apps** (`dtu`, `tui`, `gnss`) | **New `uartdrv@` protocol exe** → **skinos-uart** |
| **forward** | nat, firewall, dnat, main, alg, ttl | New forward rules / ALG helpers |
| **client** | dhcps, station, acl | LAN-side policy |
| **clock** | ntps, date, restart | Time sources / reboot helpers |
| **storage** | ftp (and related) | File services |
| **tui** | telnet, ssh | Remote shell into eline |
| **wui** | Admin UI registration; `obj` → `webs@httpd` | Menus/pages (often live in feature projects’ `wui`) |
| **webs** | `httpd` | HTTP server implementation |
| **agent** | io, local, heclient, mqtt, portc; `gtog` | Cloud/remote control protocols |
| **vpn** | l2tp / pptp / gre lists | Classic VPN clients |
| **nvpn** | WireGuard list | WG management |
| **ipsec** | IPsec list/client area | IPsec tunnels + certs |
| **gnss** | GNSS frame / nmea objects | System GNSS feature (may use uart GNSS driver) |
| **camera** | OSD / camera | Video-related components |
| **tmptools** | Templates | Copy patterns for **any** new project |

Optional / product-specific domains may appear under `doc/com/` without a checked-out tree here.

---

## Two extension patterns to remember

### A. USB modem module → `modem`

```text
arch@usb  --usb_match-->  usbdrv@mymod (= modem@mymod)
                              |
                              +--> registers modem@lte*  -->  modem@atd
                              +--> optional modem@sms*   -->  modem@smsd
                              |
                              v
                         ifname@lte  (dial / uplink)
```

Details: [`.claude/skills/skinos-modem/SKILL.md`](.claude/skills/skinos-modem/SKILL.md).

### B. Serial protocol app → `uart`

```text
uart@frame.setup  -->  uart@ttyN (config)
                         |
                         +--> sstarts(uartdrv@myproto, service, uart@ttyN, /dev/tty…)
```

Details: [`.claude/skills/skinos-uart/SKILL.md`](.claude/skills/skinos-uart/SKILL.md).

---

## Documentation links (by domain)

| Project | Docs |
|---------|------|
| **land** | [`doc/com/land/`](doc/com/land/) — he, eline, init, joint, machine, auth, skin, fpk, … |
| **forward** | [`doc/com/forward/`](doc/com/forward/) |
| **network** | [`doc/com/network/`](doc/com/network/) |
| **client** | [`doc/com/client/`](doc/com/client/) |
| **modem** | [`doc/com/modem/`](doc/com/modem/) · skill **skinos-modem** |
| **wifi** | [`doc/com/wifi/`](doc/com/wifi/) |
| **ifname** | [`doc/com/ifname/`](doc/com/ifname/) |
| **uart** | [`doc/com/uart/`](doc/com/uart/) · skill **skinos-uart** |
| **clock / storage / tui / wui / agent / …** | Matching `doc/com/<name>/` |

**Rule of thumb:** code in `project/<name>/` · API docs in `doc/com/<topic>/` · shipping rules in that project’s `prj.json` · board defaults in `config/<platform>/…`.
