# Snake8 / landos — Embedded Gateway SDK

This repository is an **embedded Linux gateway and device-management SDK**. Instead of exposing raw Linux utilities as the primary control surface, it adds a **component model** on top: each feature area is a **named component** with a **JSON configuration schema** and **callable APIs**. Day-to-day management uses the **same HE grammar** everywhere—whether you type at an interactive terminal, from a full shell, or through remote agents and the Web UI backend.

The in-tree documentation often refers to this stack as **landos** (land-layer operating model) and to shared libraries as **skinos**.

---

## What you get at a glance

| Layer | Role |
|--------|------|
| **Linux** | Kernel, drivers, userspace daemons as usual |
| **Components** | Addressed like `land@machine`, `forward@nat`, `client@dhcps` — each exposes JSON config + APIs |
| **`eline`** | **Default interactive UI** on the device (prompt **`$ `**): readline, history, HE lines **without** the `he` prefix — see [`doc/com/land/eline.md`](doc/com/land/eline.md) |
| **`he`** | Small binary that runs **one HE line** from an **`ash`/shell** session: always use **`he '…'`** (single quoted) so the shell does not eat `=`, `\|`, JSON, etc. — full grammar in [`doc/com/land/he.md`](doc/com/land/he.md) |
| **Projects / FPK** | Shipping unit: a directory under `project/` with `prj.json` builds into an **FPK** (installable package), like an application installer for the device |
| **Lifecycle** | Boot stages, **joint** events (e.g. network up/down), and shutdown hooks run registered component APIs from `prj.json` |

**Documentation entry points** (under [`doc/com/land/`](doc/com/land/)):

- [`README.md`](doc/com/land/README.md) — landos overview and doc index  
- [`eline.md`](doc/com/land/eline.md) — **normal way to manage the device** from Telnet / SSH / serial  
- [`he.md`](doc/com/land/he.md) — **only when you are already in a Linux shell** (`ashy` from eline, scripts, automation)  
- [`fpk.md`](doc/com/land/fpk.md) — FPK install layout and `prj.json` fields  
- [`init.md`](doc/com/land/init.md), [`joint.md`](doc/com/land/joint.md), [`uninit.md`](doc/com/land/uninit.md) — boot levels and event-driven tasks  
- [`component.md`](doc/com/land/component.md), [`machine.md`](doc/com/land/machine.md), [`auth.md`](doc/com/land/auth.md), [`syslog.md`](doc/com/land/syslog.md), [`service.md`](doc/com/land/service.md), [`register.md`](doc/com/land/register.md), [`daemon.md`](doc/com/land/daemon.md), [`skin.md`](doc/com/land/skin.md), [`prj.json.md`](doc/com/land/prj.json.md) — core `land@*` behaviour and tooling  

Other domains (`forward`, `network`, `wifi`, …) have matching folders under [`doc/com/`](doc/com/).

---

## Top-level directory map

| Path | Purpose |
|------|---------|
| **`Makefile`** | Top build orchestration: `dep`, `kernel` / `app`, install into `build/`, refresh `doc/dev/include` from staged headers |
| **`gBOARDID`** | Selects **platform / SoC / board / scope / OEM** (see file comments). Drives `config/<platform>/…` resolution. Excluded from version control; see **`gBOARDID.txt`** for a checked-in reference template |
| **`config/`** | Per-platform trees (**`smtk2`**, **`smtk3`**, **`srock`**, **`sopen`**, **`slave`**, …): toolchains, kernel, rootfs overlays, `makefile.config`, `sdk.makefile`, `fpk.makefile`, etc. |
| **`project/`** | **All implementations and packages**: C components, libraries, daemons, Web UI pages, and **`prj.json`** manifests. Each subdirectory corresponds to a domain (e.g. `project/land`, `project/forward`, `project/network`, `project/webs`) |
| **`doc/`** | Human-facing documentation and generated API headers |
| **`tools/`** | Host-side build helpers (`firmware-encode`, `fpk-install`, `prj` and other FPK packaging utilities) |
| **`var/`** | Local IDE / Si4 project files (not part of the firmware story) |
| **`rice/`** | Optional path for customer-specific overlays (referenced by `Makefile` as `gRICE_DIR`) |

Build outputs (typical):

- **`build/install/`** — staged headers and libs during compile  
- **`build/rootfs/`** — assembled root filesystem; installed projects land under something like **`usr/share/skinos/`** (macro `gosPRJ_DIR` in the `Makefile`)  

Symbolic install paths such as **`⟨PRJ_ROOT⟩`** are documented in [`doc/com/land/fpk.md`](doc/com/land/fpk.md) and map to C macros like `PROJECT_DIR`.

---

## Project layout

Each subdirectory under **`project/`** represents a **domain** — it contains both the **implementation source code** (C components, libraries, daemons) and the **packaging manifest** (`prj.json`).

Examples: `project/land`, `project/forward`, `project/network`, `project/webs`, `project/wifi`, `project/modem`, etc.

Every project **must** include:

1. **`prj.json`** — manifest (see below)  
2. A **Makefile** appropriate to the platform build (often OpenWrt-style under this SDK)

Packaging and install semantics are described in [`doc/com/land/fpk.md`](doc/com/land/fpk.md).

---

## `prj.json` — the project manifest

`prj.json` is the contract between build, runtime registration, and documentation. Common keys:

| Key | Meaning |
|-----|---------|
| **`name`**, **`intro`**, **`desc`**, **`version`**, **`author`**, **`type`** | Identity; `type: "root"` marks root-privileged / system-level packages |
| **`com`** | Subdirectories that are **components** (become names like `projectname@subdir`) |
| **`lib`** | Libraries built with the project |
| **`exe`** | Standalone executables |
| **`osc`** | Bundled third-party / open-source trees |
| **`cmd`** | Commands shipped with the project (e.g. `he`, `eline`, `daemon` under **land**) |
| **`obj`** | **Aliases**: maps a public component name to another implementation (e.g. dynamic or shared object) |
| **`init`** | Map **boot level** → **`project@component.api`** to run at startup. Known levels include **`arch`**, **`land`**, **`app`**, **`general`**, **`manage`**, **`delay`** (see [`init.md`](doc/com/land/init.md)). Value per level can be a **string** (single API, e.g. `"wui@admin.setup"`) or an **object** (multiple APIs, e.g. `{"land@auth.setup":"", "land@init.setup":""}`) |
| **`joint`** | Map **event** (e.g. `network/on`, `network/online`) → APIs to run when that event fires |
| **`uninit`** | Shutdown-stage APIs |
| **`wui`** | Web UI registration: menu group, localized titles, HTML page, optional `config` / `object` binding |

Boot levels and joint events are defined in [`doc/com/land/init.md`](doc/com/land/init.md), [`doc/com/land/joint.md`](doc/com/land/joint.md), and [`doc/com/land/uninit.md`](doc/com/land/uninit.md).

---

## Projects under `project/` — components and how they relate

Projects are **not** isolated apps: they **register components** into one runtime. Component names are usually **`project@directory`** (from **`com`**) unless **`obj`** rewrites the public name (e.g. `land@joint` → `init` implementation). **`init` / `joint` / `uninit`** entries **call APIs on other projects’ components** (e.g. `forward@main.on` on `network/on`), which is how behaviour is chained.

### Dependency / layering (conceptual)

Plain **monospace text** diagram (no Mermaid). Read top → bottom as “depends on / builds on”; side notes are cross-links.

```text
       +---------------------------+       +---------------------------+
       | wui (admin -> webs@httpd) |       | tui (telnet / ssh)        |
       +-------------+-------------+       +-------------+-------------+
                     \                               /
                      \   operators use the same HE  /
                       v   grammar via land tools   v
                 +-----------------------------------------+
                 | land: he, eline, daemon                 |
                 +--------------------+--------------------+
                                      |
                                      v
  +---------------------------------------------------------------------+
  | land:      skin, machine, auth, syslog, service, register, fpk,    |
  |            init / joint / uninit, component registry               |
  +---------------------------------------------------------------------+
                                      |
            +-------------------------+-------------------------+
            v                         v                         v
  +-------------------+     +-------------------+     +-------------------+
  | network           |     | ifname            |     | wifi              |
  | hosts, frame,     |     | ethcon, ltecon    |     | ap, sta, skinwifi |
  | vlan, bridge, …   |     |                   |     |                   |
  +---------+---------+     +-------------------+     +-------------------+
            |                         ^                         ^
            |              +----------+-----------+              |
            |              | modem (atd, smsd,   |              |
            |              | ec2x, rm500u, …)   |              |
            |              +----------+-----------+              |
            |                         |                         |
            +-------------------------+-------------------------+
                                      |
                      +---------------+---------------+
                      v                               v
              +---------------+               +---------------+
              | forward       |               | client        |
              | main, nat, fw |               | dhcps, station|
              | dnat, ttl, alg|               | acl           |
              +---------------+               +---------------+
                      \                               /
                       `---- joint on network/* -----'
                         (on/off, online/offline
                          refreshes rules and DHCP)

  clock ..........> uses network/online (and friends) for NTP / time
  storage ........> land joint on storage insert/remove (e.g. syslog paths)
  agent ..........> follows network + machine status; drives HE remotely
```

**Legend**

- **Vertical flow:** later stages assume **land** and **connectivity** projects are present; **forward** / **client** sit on top of stable **network** / **ifname** behaviour.
- **forward + client:** many **`joint`** entries re-run APIs when **`network/*`** events fire, so those two **both consume and refresh against** link state (not a one-way line only).
- **wifi** / **modem** often pair with **ifname** and **network** per product; not every image ships all four.

- **`land`** — **Always at the bottom of the management stack**: `skin` library, lifecycle **`land@init` / `land@joint` / `land@uninit`**, packaging **`land@fpk`**, identity **`land@machine`**, security **`land@auth`**, logging **`land@syslog`**, process control **`land@service`** / **`daemon`**, **`land@register`**, **`land@component`**, and the **`he`** + **`eline`** commands. Other projects assume these names exist.  
- **`network`** — Shared **L2/L3 plumbing**: **`network@hosts`**, **`network@frame`**, **`network@vlan`**, **`network@bridge`**, **`network@keeplive`**, PPP tree under **`osc`**, and **`connect`** (executable) tied to **`ifname`**-style externals. **`network@hosts.setup`** runs at the **`land`** boot level so name resolution is ready early.  
- **`ifname`** — **Per-interface connection logic** (`ifname@ethcon`, `ifname@ltecon`). Upper layers (Wi‑Fi STA, modem PDP, Web docs) refer to these patterns when talking about “WAN/LTE/WISP” style links.  
- **`wifi`** — **Radio and STA/AP policy** via **`wifi@ap`**, **`wifi@sta`**, plus **`skinwifi`**. Depends on kernel/platform Wi‑Fi from **`config/`**; coordinates with **`network`** and **`ifname`** for bridged or routed topologies.  
- **`modem`** — **Cellular**: **`modem@atd`**, **`modem@smsd`**, vendor drivers **`modem@ec2x`**, **`modem@rm500u`**, **`skinmodem`**, **`tip`**. Feeds **`ifname@ltecon`**-class setups when LTE is the uplink.  
- **`forward`** — **Router features**: **`forward@main`** (routing table), **`forward@nat`**, **`forward@firewall`**, **`forward@dnat`**, **`forward@ttl`**, **`forward@alg`**. **`joint`** hooks on **`network/on`**, **`network/onextern`**, **`network/onvpn`** refresh rules when uplinks change—so **`forward`** sits **above** stable interface management.  
- **`client`** — **LAN side**: **`client@dhcps`**, **`client@station`**, **`client@acl`**. Reacts heavily to **`network/on|off|online|offline`** so DHCP/ACL track plug/unplug and carrier.  
- **`clock`** — **`clock@ntps`**, **`clock@date`**, **`clock@restart`**; uses **`joint`** on **`network/online`** to sync time when WAN appears.  
- **`storage`** — **`storage@ftp`** (and related file-server bits); **`init`** runs setup tasks; **`land`** may react to **`storage/insert|remove`** for syslog paths (see **`land`** `joint` in `prj.json`).  
- **`tui`** — **`tui@telnet`**, **`tui@ssh`** (Dropbear) so operators reach **`eline`** / **`he`** remotely.  
- **`wui`** — Does not reimplement HTTP in this tree: **`wui@admin`** is an **`obj`** alias to **`webs@httpd`** (implemented under **`project/webs`**). **`init` / `uninit`** at **`app`** stage start/stop the admin site.  
- **`agent`** — **Off-box control**: **`agent@io`**, **`agent@local`**, **`agent@heclient`**, **`agent@portc`**, executable **`gtog`**. **`init` / `joint`** tie **`heclient`** to **`network/online`** and **`machine/status`** so cloud tunnels track reality.  
- **`webs`** — **HTTP server**: **`webs@httpd`** — the actual HTTP daemon used by **`wui@admin`** via **`obj`** alias. Implementation in **`project/webs/httpd/`**.  
- **`uart`** — **Serial applications**: **`uart@frame`**, **`uart@dtu`**, **`uart@hetui`**, **`skinuart`**; WUI maps pages to **`uart@tty`** / **`tty2`** / **`tty3`**.  
- **`tmptools`** — **Teaching / templates**: **`tmptools@testcom`**, **`testexe`**, **`prj`** command; safe sandbox to copy patterns from.  

The table maps **`prj.json` → component names** to **clickable docs** (relative to the repo root). If no split Markdown exists yet, we point to the closest doc or to **`project/<domain>/`** source.

| Project | Components — documentation links | Notable extras |
|---------|-----------------------------------|----------------|
| **land** | [`land@fpk`](doc/com/land/fpk.md) · [`land@init`](doc/com/land/init.md) · [`land@joint`](doc/com/land/joint.md) · [`land@uninit`](doc/com/land/uninit.md) · [`land@component`](doc/com/land/component.md) · [`land@register`](doc/com/land/register.md) · [`land@syslog`](doc/com/land/syslog.md) · [`land@service`](doc/com/land/service.md) · [`land@machine`](doc/com/land/machine.md) · [`land@auth`](doc/com/land/auth.md) | **`cmd`**: [`he`](doc/com/land/he.md), [`eline`](doc/com/land/eline.md), [`daemon`](doc/com/land/daemon.md) · **`lib`**: [`skin`](doc/com/land/skin.md) |
| **forward** | [`forward@alg`](doc/com/forward/alg.md) · [`forward@ttl`](doc/com/forward/ttl.md) · [`forward@firewall`](doc/com/forward/firewall.md) · [`forward@nat`](doc/com/forward/nat.md) · [`forward@dnat`](doc/com/forward/dnat.md) · [`forward@main`](doc/com/forward/main.md) · also [`rule`](doc/com/forward/rule.md), [`mark`](doc/com/forward/mark.md) | **`obj`** aliases e.g. `forward@254` → `main` |
| **network** | [`network@frame`](doc/com/network/frame.md) · `network@hosts` / `vlan` / `bridge` / `keeplive` — *no separate `doc/com` pages; see [`frame.md`](doc/com/network/frame.md) and `project/network/`* | **`lib`**: `skinnet`; **`exe`**: `connect`; **`osc`**: `ppp-2.4.5` |
| **client** | [`client@acl`](doc/com/client/acl.md) · [`client@dhcps`](doc/com/client/dhcps.md) · [`client@station`](doc/com/client/station.md) | Heavy **`joint`** on LAN/WAN events |
| **modem** | [`modem@atd`](doc/com/modem/lte.md) (LTE / AT — same feature area as **`atd`**) · [`modem@smsd`](doc/com/modem/sms.md) · **`modem@ec2x` / `modem@rm500u`** — *drivers; see [`lte.md`](doc/com/modem/lte.md) + `project/modem/`* | **`lib`**: `skinmodem`; **`cmd`**: `tip`; **`obj`**: `usbdrv@ec2x` / `rm500u` |
| **wifi** | AP radios: [`wifi@n` \(2.4G\)](doc/com/wifi/n.md), [`wifi@a` \(5.8G\)](doc/com/wifi/a.md); STA: [`wifi@nsta`](doc/com/wifi/nsta.md), [`wifi@asta`](doc/com/wifi/asta.md); multi-SSID: [`assid`](doc/com/wifi/assid.md), [`nssid`](doc/com/wifi/nssid.md) — *maps to `prj.json` **`ap` / `sta`** trees* | **`lib`**: `skinwifi` |
| **ifname** | [`ifname@lan`](doc/com/ifname/lan.md) · [`ifname@wan`](doc/com/ifname/wan.md) · [`ifname@lte`](doc/com/ifname/lte.md) · [`ifname@wisp`](doc/com/ifname/wisp.md) — *wired / uplink docs; aligns with **`ethcon` / `ltecon`** usage* | Building blocks for wired/LTE/WISP uplinks |
| **clock** | [`clock@ntps`](doc/com/clock/ntps.md) · [`clock@date`](doc/com/clock/date.md) · [`clock@restart`](doc/com/clock/restart.md) | **`osc`**: `ntpclient` |
| **storage** | [`storage@ftp`](doc/com/storage/ftp.md) | File / NAS style services |
| **tui** | [`tui@telnet`](doc/com/tui/telnet.md) · [`tui@ssh`](doc/com/tui/ssh.md) | **`obj`**: `telnetd`→`telnet`, `sshd`→`ssh` |
| **wui** | [`wui@admin`](doc/com/wui/admin.md) · [`wui` ACE skin](doc/com/wui/ace.md) · [`webpage guide`](doc/com/wui/webpage.md) · **`webs@httpd`** — *HTTP server code: `project/webs/`* | Web admin shell |
| **agent** | [`agent@io`](doc/com/agent/io.md) · [`agent@local`](doc/com/agent/local.md) · [`agent@heclient`](doc/com/agent/heclient.md) · [`agent@portc`](doc/com/agent/portc.md) · [`gtog`](doc/com/agent/gtog.md) · [`net`](doc/com/agent/net.md) | **`exe`**: `gtog` |
| **webs** | `webs@httpd` — *HTTP daemon implementation in `project/webs/httpd/`* | Used by **wui** via `obj` alias |
| **uart** | *No `doc/com/uart/` yet* — see `project/uart/` | **`lib`**: `skinuart` |
| **tmptools** | [`prj` online tool](project/tmptools/prj.md) · [`prj.json` format](doc/com/land/prj.json.md) · **`testcom`** — *examples in [`prj.json.md`](doc/com/land/prj.json.md) §15 + `project/tmptools/`* | **`cmd`**: `prj`; **`exe`**: `testexe` |

Other directories may exist without a `prj.json`; they may still be pulled in by platform makefiles or as dependencies of another project.

---

## Documentation layout (`doc/`)

| Area | Contents |
|------|-----------|
| **`doc/com/<domain>/`** | **Component and feature docs** (e.g. `land/`, `forward/`, `network/`, …). Each file usually documents **configuration (JSON)** and **APIs** for the matching `project@component` names. |
| **`doc/com/land/`** | **System shell**: [`eline.md`](doc/com/land/eline.md), [`he.md`](doc/com/land/he.md), [`fpk.md`](doc/com/land/fpk.md), lifecycle [`init.md`](doc/com/land/init.md) / [`joint.md`](doc/com/land/joint.md) / [`uninit.md`](doc/com/land/uninit.md), and the core `land@*` references listed at the top of this README. |
| **`doc/dev/include/`** | **C headers** staged from `build/install/include` after a successful build. |
| **`doc/product/`**, **`doc/use/`** | Product-specific notes (optional). |

**Rule of thumb:** implementation in **`project/<name>/`** + contract in **`doc/com/<same topic>/`** + shipping rules in **`project/<name>/prj.json`**.

---

## Managing the operating system

All control uses one **HE grammar** (see [`eline.md`](doc/com/land/eline.md) and [`he.md`](doc/com/land/he.md)). Three operations cover almost everything you do:

| Operation | Role |
|-----------|------|
| **View configuration** | Read the component’s stored JSON model: whole object or a single **attribute path** (`:` + slash-separated path). |
| **Modify configuration** | Change stored settings: one field (`path=value`), replace whole object (`component={...}`), or merge/patch only selected keys (pipe/JSON forms in [`he.md`](doc/com/land/he.md)). |
| **Call a component API** | Run an action implemented by the component (`component.api` or `component.api[args]`); result is often JSON or a sentinel such as **`ttrue` / `tfalse`**. Append **`:path`** to take one field from JSON output. |

Below, **`$ `** means **eline** (type the line as-is). **`~ #`** means a normal shell after **`ashy`** — wrap the **same** HE text in **`he '…'`**.

### 1. View configuration

**Intro:** Inspect what is persisted for a component. **`component`** returns the full configuration object; **`component:attr/path`** returns one value (string or nested JSON fragment).

**Eline (`$ `):**

```text
$ land@machine
$ land@machine:name
$ ifname@lan:static
```

**Shell (`~ #`):** same three lines, each wrapped: `he 'land@machine'`, etc.

| Command | What it does | Typical return |
|---------|----------------|----------------|
| **`land@machine`** | Loads the **entire stored config** for component **`land@machine`** (identity, language, MAC-related fields, …). | **JSON object** printed to the terminal; then the next **`$ `** prompt. If the component is missing or the path is wrong, you may see an error string or sentinel instead of JSON. |
| **`land@machine:name`** | Reads **one attribute** (`name`) via an attribute path. | **Plain text** (the configured name string), or empty line if unset; not wrapped in JSON quotes in the output. |
| **`ifname@lan:static`** | Reads the **`static`** subtree for **`ifname@lan`** (fixed LAN IPv4/mask, etc., when that model exists on the image). | **JSON fragment** (e.g. `ip` / `mask` fields) or minimal output if undefined. |

**Discover components:** at **`$ `**, type **`@`**; or see [`he.md`](doc/com/land/he.md) for the equivalent when not using eline. The JSON **shape** is product-specific—use component docs under **`doc/com/`** for field meanings.

### 2. Modify configuration

**Intro:** Updates the configuration store (subject to validation). Typical patterns: set one attribute, clear with **`path=`**, replace with **`component={...}`**, or merge with **`component|{...}`** / **`component:branch|{...}`** — full rules in [`he.md`](doc/com/land/he.md).

**Eline (`$ `):**

```text
$ land@machine:name=MyGateway
$ land@machine|{"language":"en"}
$ gnss@nmea:client=
```

**Shell (`~ #`):**

```sh
he 'land@machine:name=MyGateway'
he 'land@machine|{"language":"en"}'
he 'gnss@nmea:client='
```

Complex JSON or characters such as **`|`** and **`=`** are why the shell form **must** use a **single-quoted** argument.

| Command | What it does | Typical return |
|---------|----------------|----------------|
| **`land@machine:name=MyGateway`** | **Sets** the **`name`** field to the string **`MyGateway`** (validated by **`land@machine`**). | Often **`ttrue`** / **`tfalse`** (success/failure tokens) or a short status string; on failure, an error hint may be printed. **`he`** also maps this to a **process exit code** in scripts (see [`he.md`](doc/com/land/he.md)). |
| **`land@machine`** + merge JSON (pipe form in [`he.md`](doc/com/land/he.md), e.g. `{"language":"en"}`) | **Merges** into **`land@machine`**: only listed keys change; other keys stay as before. | Same style as a single-field set: **boolean-like** or status text; invalid JSON or forbidden keys → failure path. |
| **`gnss@nmea:client=`** | **Clears** the **`client`** branch under **`gnss@nmea`** (example: remove client block). | Success/failure indicators like other writes; no large JSON body on success. |

*(Example **`gnss@nmea`** only applies if that component is present on your firmware.)*

### 3. Call a component API

**Intro:** Invokes a named API on the component (not the same as reading config). Use **`component.api`** with no args, or **`component.api[arg1,arg2]`** with parameters. Use **`component.api:field/path`** to print only part of the returned JSON.

**Eline (`$ `):**

```text
$ land@machine.status
$ land@machine.status:version
$ clock@date.ntpsync[ntp1.aliyun.com]
$ client@station.list
```

**Shell (`~ #`):**

```sh
he 'land@machine.status'
he 'land@machine.status:version'
he 'clock@date.ntpsync[ntp1.aliyun.com]'
he 'client@station.list'
```

| Command | What it does | Typical return |
|---------|----------------|----------------|
| **`land@machine.status`** | Calls the **`status`** API on **`land@machine`** (runtime / version / capability snapshot—exact fields are implementation-defined). | **JSON object** printed to the terminal; or **`ttrue`/`tfalse`/`terror`/`tpanic`**-style sentinels when there is no JSON body (see [`he.md`](doc/com/land/he.md)). |
| **`land@machine.status:version`** | Same **`status`** call, but only the **`version`** field from the JSON is printed. | **Short string** (e.g. firmware or component version) if present; empty or error if the path does not exist. |
| **`clock@date.ntpsync[ntp1.aliyun.com]`** | Calls **`ntpsync`** on **`clock@date`** with one argument: NTP server hostname/IP. | **`ttrue`** on accepted sync request, **`tfalse`** or error text on failure; some builds may return a small JSON or message instead—check **`doc/com/clock/date.md`**. |
| **`client@station.list`** | Calls **`list`** on **`client@station`** (enumerates learned LAN clients / ARP-style table—semantics per product). | **JSON** (array or object of stations); or empty / error if the service is not ready. |

List available APIs for a component with **`<component>.`** (see [`eline.md`](doc/com/land/eline.md) / [`he.md`](doc/com/land/he.md)).

---

### 4. Default: **`eline`** (prompt **`$ `**)

On a typical image the login shell is **`eline`**. After Telnet, SSH, or serial login you get a **`$ `** prompt with **readline** (history, line editing). In this mode:

- Type **HE commands directly** — **do not** prefix **`he`**.
- The grammar is **identical** to [`he.md`](doc/com/land/he.md); [`eline.md`](doc/com/land/eline.md) adds **eline-only** behaviour:
  - **`set <object>`** — interactive multi-field session for one component
  - **`ashy`** — drop to BusyBox **`ash`** when you need normal shell tools (then use **`he '…'`** as in §1–3)
  - **`exit`** / Ctrl+D — leave eline without entering shell
  - A small **passthrough** list of OS commands implemented inside eline

### 5. Only in a full shell: **`he '…'`**

After **`ashy`**, or in scripts / cron / your own SSH session with `/bin/ash`, the HE interpreter is no longer the foreground loop. Every line must be run as **`he '…'`** as shown in §1–3.

[`he.md`](doc/com/land/he.md) explains **why all arguments are concatenated** and why **one quoted string** is required when the line contains spaces or shell metacharacters.

Some older products still use a classic **HE loop** with **`# `** prompt (same as eline: **no** `he` prefix). Follow the prompt you actually see.

### 6. Discovering components and APIs

- At **`$ `**: **`@`** lists components (shape is firmware-dependent); **`<component>.`** lists APIs — details in [`eline.md`](doc/com/land/eline.md) and [`he.md`](doc/com/land/he.md).
- Return values are usually **JSON**, a plain string, empty output, or sentinel codes such as **`ttrue` / `tfalse` / `terror` / `tpanic`** (see [`he.md`](doc/com/land/he.md) / [`eline.md`](doc/com/land/eline.md)).

### 7. Web UI

Pages declared under each project’s **`wui`** block appear in the admin site. They edit the **same JSON configuration** and end up calling the **same component APIs**—just through HTML/JS instead of HE text.

### 8. Remote / cloud

The **`agent`** project wires **`agent@heclient`** and related pieces to **network online** and **machine status** events so remote sessions track connectivity; see [`doc/com/agent/`](doc/com/agent/) and `project/agent/prj.json`.

---

## Building (overview)

1. **Install host dependencies** (Ubuntu): **`make preset`** installs compilers, libraries, and tools needed by the build.  
2. Set **`gBOARDID`** to match your hardware profile: either edit `gBOARDID` directly, or run **`make pid gBOARDID=<platform-soc-board>`** (e.g. `slave-x86-ubuntu2004`). See **`gBOARDID.txt`** for known identifiers.  
3. **`make update`** — pull the latest SDK, platform config repos, and apply SDK adjustments.  
4. **`make dep`** — prepare `build/` trees and rootfs staging.  
5. **`make`** (or split **`make kernel`** / **`make app`** as your workflow requires — see **`target.makefile`** and platform **`sdk.makefile`**).  
6. After a successful build, public headers are refreshed under **`doc/dev/include/`**.

Additional useful targets (defined in **`misc.makefile`**):

| Target | Purpose |
|--------|---------|
| `make preset` | Install Ubuntu build dependencies |
| `make update` | Pull latest SDK + platform repos |
| `make rebuild` | Recompile, install, and start (host / slave mode) |
| `make menu` / `make menuconfig` | Enter SDK menu configuration (smtk2/smtk3 etc.) |
| `make install` / `make start` / `make stop` | Install / start / stop the system (slave / host mode) |
| `make tftp` / `make ftp` / `make sz` | Deploy firmware to device via different transports |

Exact toolchain and image packaging steps depend on the selected **`config/<platform>`** tree; consult that platform's docs and makefiles under `config/`. For a complete quick-start walkthrough on the **slave** (Ubuntu host) platform, see [`config/slave/readme.md`](config/slave/readme.md).

---

## Repository structure — multi-repo layout

The main repository (`snake8`) does **not** contain platform trees directly. Instead:

- Each subdirectory under **`project/`** (e.g. `project/land`, `project/forward`, `project/agent`, …) is an **independent Git repository**, cloned or pulled separately.
- Each **`config/<platform>`** directory (e.g. `config/smtk2`, `config/srock`) is also a separate Git repository.
- The optional **`rice/`** directory (customer overlays) may be yet another repo.

The top-level **`.gitignore`** excludes these paths so they are not tracked by the main repo.

Helper scripts for working with this multi-repo layout:

| Script | Purpose |
|--------|---------|
| **`gitst`** | Run `git status` across the main repo, all `project/` sub-repos, `rice/`, and each `config/<platform>` |
| **`gitup`** | Run `git pull` across the same set of repositories |
| **`mkdel`** | Clean build artifacts (`skinos_*` temp directories) from all platform SDK build trees |

---

## Version and tracking

- **`gPUBLISH` / `gVERSION`** in the top **`Makefile`** label the SDK release line.  
- **`TPD.txt`** in the repo root is a short internal task note file (not user documentation).

---

## Summary

**Snake8** ships **many coordinated projects** under `project/`, each registering **named components** and **lifecycle hooks** into a single runtime. **`land`** supplies the **`he` / `eline` / `daemon`** tooling and core **`land@*`** services; **`network`**, **`ifname`**, **`wifi`**, and **`modem`** implement connectivity; **`forward`** and **`client`** implement routing and LAN policy; **`webs`** provides the HTTP server; **`wui`**, **`tui`**, **`agent`**, **`uart`**, **`clock`**, **`storage`**, and **`tmptools`** layer management and applications on top.

**Operate the device through [`eline.md`](doc/com/land/eline.md) by default**; use [`he.md`](doc/com/land/he.md) **only when you are in a normal shell** or automating with **`he '…'`**.
