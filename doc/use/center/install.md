# Center Platform — Installation and Usage Guide

Build and run the **center** cloud management platform on an Ubuntu host so **embedded
gateways** can connect through the **agent** project on each gateway.

Use board identity **`slave-x86-ubuntu2404`** (Ubuntu 24.04) on the host. Embedded
gateways keep their own product firmware; point the **agent** project on each embedded
gateway at this center host.

---

## 1. What the platform provides

| Component | Role | Default port |
|-----------|------|--------------|
| **center@userwui** | Cloud-user Web (embedded gateway list, remote management) | TCP **20000** |
| **webs@httpd + wui** | Admin Web | TCP **20001** |
| **center@heport** | Remote management of embedded gateways (`agent@heclient` on embedded gateway) | TCP **20002** |
| **center@nport** | Mesh coordinator (`agent@gtog` / `agent@net` on embedded gateway) | UDP **20002** |
| **center@api** | Platform API control | TCP **20003** |
| **center@pport** | Port proxy / tunnel (`agent@portc` on embedded gateway) | TCP **20005** |

The slave platform includes **center** in `config/slave/project.config`; a slave
build compiles center services together with land, webs, and wui.

Embedded gateway **agent** configuration (components run on the embedded gateway, not on center):

- [`doc/com/agent/heclient.md`](../../com/agent/heclient.md) — remote HE management on the embedded gateway
- [`doc/com/agent/portc.md`](../../com/agent/portc.md) — port proxy client on the embedded gateway
- [`doc/com/agent/gtog.md`](../../com/agent/gtog.md) — gateway mesh on the embedded gateway

---

## 2. Requirements

- **OS:** Ubuntu 24.04 (64-bit)
- **Network:** Host reachable from embedded gateways (same LAN or routed path)
- **Privileges:** `make sdk_install` and `make sdk_start` install under
  `/usr/share/skinos` and need **sudo**

---

## 3. Get source and install build tools

```bash
# Gitee
git clone https://gitee.com/snake8/snake8.git

# GitHub
git clone https://github.com/skinos/snake8.git

cd snake8
make preset
```

`make preset` installs compilers, libraries, and other Ubuntu build dependencies.

---

## 4. Select board identity

Persist the choice (writes `gBOARDID`):

```bash
make pid gBOARDID=slave-x86-ubuntu2404
```

For a one-off build, pass the prefix on the command line:

```bash
make gBOARDID=slave-x86-ubuntu2404
```

> If unset, the Makefile defaults to `slave-x86-ubuntu2004`. On Ubuntu 24.04 use
> **`slave-x86-ubuntu2404`** explicitly.

---

## 5. Update FPK and slave platform packages

center and related projects ship as FPKs with the slave platform. Refresh on first
setup or after changing machines:

```bash
make update
```

This syncs the repo and refreshes packages under `config/slave/`.

---

## 6. Build

```bash
make
```

Or with an explicit board:

```bash
make gBOARDID=slave-x86-ubuntu2404
```

Runtime files are staged under `build/rootfs/usr/share/skinos/`.

---

## 7. Install on the host

```bash
make sdk_install
```

Alias: `make install`.

Install locations:

- `/usr/share/skinos/` — platform runtime (land, center, webs, wui, …)
- `/usr/local/lib/` — shared libraries
- `/usr/local/bin/` — `he`, `daemon`, … (skipped when `/usr/prj` exists; then use
  `/usr/share/skinos/land/bin/`)

After rebuilding, run `make sdk_install` again to refresh the installation.

---

## 8. Start the platform

```bash
make sdk_start
```

Alias: `make start`.

This runs `/usr/share/skinos/setup.sh`: initializes arch/land, starts daemon, and
calls init level **app** for **center** and related components.

Related commands:

```bash
make sdk_stop        # stop (alias: make stop)
make sdk_uninstall   # stop and remove /usr/share/skinos
```

---

## 9. Verify services are running

After start, confirm center components with **HE** and **netstat**, then open the
required ports in the firewall.

### 9.1 Check components with HE

On the host (use `he` when on PATH, or `/usr/share/skinos/land/bin/he`):

```bash
he 'land@machine.status'
he '@'
he 'center@heport'
he 'center@pport'
he 'center@nport'
he 'center@userwui'
```

Expected:

- `land@machine.status` returns JSON with a `version` field
- `@` lists `center@heport`, `center@pport`, `center@nport`, `center@userwui`,
  `center@api`, `center@ctrl`
- Each center component has **`status":"enable"`** and ports matching the table below

| Component | Config key | Default port | Protocol |
|-----------|------------|--------------|----------|
| center@userwui | port | 20000 | TCP (cloud-user Web) |
| wui@admin | port | 20001 | TCP (admin Web) |
| center@heport | port | 20002 | TCP (embedded gateway agent connect) |
| center@heport | api_port | 20003 | TCP (API control) |
| center@nport | port | 20002 | UDP (mesh coordinator) |
| center@nport | nettest_port | 20003 | UDP (NAT probe) |
| center@pport | port | 20005 | TCP (port proxy) |

### 9.2 Check listening ports with netstat

```bash
netstat -lntp | egrep '20000|20001|20002|20003|20005'
netstat -lnup | egrep '20002|20003'
```

You should see **20000, 20001, 20002, 20003, 20005** in **LISTEN** state; UDP
**20002** and **20003** are owned by **center@nport**.

If `netstat` is not installed:

```bash
ss -lntp | egrep '20000|20001|20002|20003|20005'
ss -lnup | egrep '20002|20003'
```

### 9.3 Open firewall ports

If **ufw**, **iptables**, or a cloud security group is enabled, allow **inbound**
**TCP and UDP** on **20000–30000**. Port proxy (`center@pport`) allocates public
ports from this range; opening only the fixed service ports (20000–20005) is not
enough when embedded gateways use port maps.

**Recommended rule:** open the full range **20000–30000** for both **TCP** and **UDP**.

| Port range | Protocol | Purpose |
|------------|----------|---------|
| **20000** | TCP | Cloud-user Web (`center@userwui`) |
| **20001** | TCP | Admin Web (`wui@admin`) |
| **20002** | TCP | `agent@heclient` on embedded gateway → `center@heport` |
| **20002** | UDP | `agent@gtog` on embedded gateway → `center@nport` |
| **20003** | TCP | API control (`center@heport` `api_port`) |
| **20003** | UDP | NAT probe (`center@nport` `nettest_port`) |
| **20004** | TCP/UDP | Reserved in the center port plan |
| **20005** | TCP | `agent@portc` on embedded gateway → `center@pport` |
| **20006–25000** | **TCP and UDP** | **Dynamic port maps** — `center@pport` `dynamic_port` pool (default starts at **20006**); ephemeral public ports for active proxy sessions |
| **25000–30000** | **TCP and UDP** | **Static port maps** — `center@pport` `static_port` base (default **25000**); persistent mapped public ports (`map index = port − static_port`) |

Defaults come from `center@pport` (`dynamic_port` **20006**, `static_port` **25000**).
Both dynamic and static maps may use **TCP or UDP** depending on the rule.

Ubuntu ufw example (full range):

```bash
sudo ufw allow 20000:30000/tcp
sudo ufw allow 20000:30000/udp
```

Minimum for embedded gateway agent connect **without** port maps (Web + heport + mesh + pport control
only — **port forwarding will not work**):

```bash
sudo ufw allow 20000:20005/tcp
sudo ufw allow 20002:20003/udp
```

For production center hosts that serve port maps, use the **20000–30000** rules above.

---

## 10. Cloud users and embedded gateway online status

Each embedded gateway uses **`user`** and **`vcode`** in **`agent@heclient`**. Create the cloud
user on center first, then confirm the embedded gateway is online via Web or HE.

### 10.1 Add a cloud user

Cloud users are **not** the platform admin (`land@auth` **admin**). Each cloud user
owns a set of embedded gateways and has a **`vcode`** used when an embedded gateway registers.

#### Option A — Admin Web

1. Open **`http://<center-host-IP>:20001`**
2. Log in as admin (default **`admin` / `admin`**)
3. Menu **Cloud → User List** (帐号管理)
4. Click **Add** and fill in:
   - **Username** (`A-Z`, `a-z`, `0-9`, `_`, `-` only)
   - **Password**
   - **vcode** (same value required in `agent@heclient` on the embedded gateway)
   - **Language**, **comment** (optional)
5. The new user should appear in the list

#### Option B — HE commands

```bash
# user_add[ user, key, [vcode], [lang], [comment] ]
he 'center@ctrl.user_add[ myuser,MyPass123,123456,en,Test user ]'

he 'center@ctrl.user_list'

he 'center@ctrl.user_modify[ myuser,654321,cn,Updated note ]'

he 'center@ctrl.user_reset[ myuser,NewPass456 ]'
```

See [`config/slave/center/ctrl.md`](../../../config/slave/center/ctrl.md).

### 10.2 Confirm an embedded gateway is online

After an embedded gateway runs **`agent@heclient`** with matching **`user`** / **`vcode`**:

#### Option A — Cloud-user Web

1. Open **`http://<center-host-IP>:20000`**
2. Log in with the **cloud user** account from §10.1
3. Open **Gateway List**
4. Per embedded gateway:
   - **`online`** present (duration like `22:40:24:1`) → **online**
   - **`online`** absent → **offline**
5. Filter with **Online Devices / Offline Devices**; the list refreshes about every 5 s

#### Option B — HE commands

```bash
# Embedded gateways for one cloud user; entries with "online" are connected
he 'center@api.list[ myuser ]'
```

Example (online embedded gateway):

```json
{
    "303D510049B0":
    {
        "online":"22:40:24:1",
        "name":"WL-R320-0049B0",
        "version":"v8.5.1109",
        "macid":"303D510049B0",
        "local_ip":"192.168.1.1"
    }
}
```

Admin session dump for one online embedded gateway:

```bash
he 'center@ctrl.dump[ myuser,303D510049B0 ]'
```

Field reference: [`config/slave/center/api.md`](../../../config/slave/center/api.md).

---

## 11. Connect embedded gateways (agent)

Before configuring an embedded gateway, ensure:

1. **Center is running** (§8) and **ports are open** (§9.3).
2. A **cloud user** exists on center with a known **`vcode`** (§10.1).
3. The embedded gateway firmware includes the **`agent`** project (`agent@heclient`).
4. The embedded gateway can reach the center host on **20002/TCP** (WAN, LTE, or LAN — use the
   address the embedded gateway can actually route to).

After connect, confirm the embedded gateway on center (§10.2).

### 11.1 Option A — Embedded gateway Web UI

1. Open the embedded gateway local Web UI, for example **`http://<embedded-gateway-IP>`** (default port **80**,
   or the value of `land@machine:wui_port`).
2. Log in with the embedded gateway admin account (often **`admin` / `admin`**).
3. Open **System → Agent Control** (远程控制).
4. Switch to the **Agent Control** tab (`agent@heclient` on this embedded gateway).
5. Enable **He Client**, then set:

   | Field | Value |
   |-------|--------|
   | **Server** | Center host IP or hostname (not `127.0.0.1` unless center runs on the same machine) |
   | **Port** | `20002` |
   | **Account** | Cloud username from §10.1 |
   | **Verification Code** | Same **`vcode`** as on center |
   | **Extern Interface** | Usually **Default Gateway** — outbound path used to reach center |

6. Click **Apply**.
7. On the same page, check **He Status**:
   - **`online`** — connected to center
   - **`uping`** — connecting
   - **`usererror`** / **`vcodeerror`** — wrong account or vcode; fix credentials and Apply again
   - **`down`** — service not running or disabled

Then verify the embedded gateway appears on center (§10.2).

### 11.2 Option B — Telnet / eline on the embedded gateway

Use this when the Web UI is unavailable or for scripting.

#### Step 1 — Log in to eline

1. Enable Telnet on the embedded gateway if needed (**System → Telnet Server**, or
   `tui@telnet:status=enable`; default port **23**).
2. From a PC on a network that can reach the embedded gateway:

   ```bash
   telnet <embedded-gateway-IP> 23
   ```

   Use another port if `tui@telnet:port` was changed.

3. At **`login:`**, enter the embedded gateway username (default **`admin`**).
4. Enter the embedded gateway password (default **`admin`**).
5. You should see the **`$ `** eline prompt. Type HE lines **directly** — no `he` prefix.

SSH works the same way when **`tui@ssh`** is enabled (default port **22**).

#### Step 2 — Configure agent@heclient on the embedded gateway

Replace placeholders with your center address and cloud user from §10.1:

```shell
$ agent@heclient={"status":"enable","server":"<center-host-IP>","port":"20002","user":"<user>","vcode":"<vcode>","extern":"default"}
ttrue
```

Or set fields one at a time:

```shell
$ agent@heclient:status=enable
ttrue
$ agent@heclient:server=<center-host-IP>
ttrue
$ agent@heclient:port=20002
ttrue
$ agent@heclient:user=<user>
ttrue
$ agent@heclient:vcode=<vcode>
ttrue
$ agent@heclient:extern=default
ttrue
```

Saving configuration restarts the heclient background service when **`status`** is **`enable`**.

#### Step 3 — Check connection on the embedded gateway

```shell
$ agent@heclient.status
{
    "status":"online",
    "server":"<resolved-center-ip>"
}
```

| `status` | Meaning |
|----------|---------|
| `online` | Connected to center |
| `uping` | Service running, session not ready yet |
| `down` | Service not running |
| `usererror` | Username wrong or missing — fix **`user`** |
| `vcodeerror` | Verification code wrong — fix **`vcode`** |

#### Step 4 — Confirm on center

Use §10.2 (cloud-user Web or `center@api.list[ <user> ]`) and check that the embedded gateway
entry includes an **`online`** field.

From a shell on the center host:

```bash
he 'center@api.list[ <user> ]'
```

Component reference: [`doc/com/agent/heclient.md`](../../com/agent/heclient.md).

### 11.3 Port proxy (optional)

Enable **center@pport** on center and **`agent@portc`** on the embedded gateway:

```shell
agent@portc={"status":"enable","server":"<center-host-IP>","port":"20005","user":"<user>","vcode":"<vcode>"}
```

heclient **`adjust`** can push portc / gtog settings; see
[`heclient.md`](../../com/agent/heclient.md).

### 11.4 Embedded gateway mesh (optional)

Mesh network uses **center@nport** and **`agent@gtog`** on the embedded gateway; see
[`gtog.md`](../../com/agent/gtog.md).

---

## 12. Quick reference

| Step | Command |
|------|---------|
| Install build deps | `make preset` |
| Select Ubuntu 24.04 board | `make pid gBOARDID=slave-x86-ubuntu2404` |
| Update platform FPK | `make update` |
| Build | `make` |
| Install to host | `make sdk_install` |
| Start | `make sdk_start` |
| Stop | `make sdk_stop` |
| Refresh after code change | `make` → `make sdk_install` → `make sdk_stop` → `make sdk_start` |

One-shot rebuild and start from the source tree:

```bash
make rebuild
```

Equivalent to `make; make sdk_install; make sdk_start`.

---

## 13. Auto-start on boot (Ubuntu)

After **`make sdk_install`**, the platform scripts live on the host:

| Script | Role |
|--------|------|
| `/usr/share/skinos/setup.sh` | Start center (same as `make sdk_start`) |
| `/usr/share/skinos/shut.sh` | Stop center (same as `make sdk_stop`) |

Auto-start means running **`setup.sh`** after each reboot. Pick one method below.

> After rebuilding and running **`make sdk_install`** again, restart the service (or re-run
> `setup.sh`) so the running instance matches the installed files.

### 13.1 Option A — systemd (recommended)

Works on Ubuntu 20.04 / 24.04. Supports clean stop via **`shut.sh`** and restart on failure.

Create a unit file:

```bash
sudo tee /etc/systemd/system/skinos.service >/dev/null <<'EOF'
[Unit]
Description=SkinOS Center Cloud Platform
After=network-online.target
Wants=network-online.target

[Service]
Type=forking
ExecStart=/usr/share/skinos/setup.sh
ExecStop=/usr/share/skinos/shut.sh
RemainAfterExit=yes
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF
```

Enable and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable skinos.service
sudo systemctl start skinos.service
```

Check status:

```bash
sudo systemctl status skinos.service
journalctl -u skinos.service -f
```

Disable auto-start:

```bash
sudo systemctl disable skinos.service
sudo systemctl stop skinos.service
```

After a platform upgrade (`make sdk_install`), reload and restart:

```bash
sudo systemctl restart skinos.service
```

### 13.2 Option B — root crontab `@reboot`

Minimal setup when systemd is not used. Stop on shutdown is not automatic — use
`make sdk_stop` or `shut.sh` manually before maintenance.

```bash
sudo crontab -e
```

Add one line:

```cron
@reboot sleep 15 && /usr/share/skinos/setup.sh
```

The **`sleep 15`** waits for network interfaces to come up (adjust if needed).

List / remove:

```bash
sudo crontab -l
sudo crontab -e   # delete the @reboot line
```

### 13.3 Option C — `/etc/rc.local`

Legacy fallback on some Ubuntu images. **`rc-local.service`** must be enabled.

```bash
sudo tee /etc/rc.local >/dev/null <<'EOF'
#!/bin/bash
/usr/share/skinos/setup.sh
exit 0
EOF

sudo chmod +x /etc/rc.local
sudo systemctl enable rc-local.service 2>/dev/null || true
sudo systemctl start rc-local.service 2>/dev/null || true
```

On older Ubuntu you may also need:

```bash
sudo systemctl enable rc-local
```

There is no automatic **`shut.sh`** on shutdown unless you add it separately.

### 13.4 Option D — login profile (manual / dev only)

For a user account that logs in to the desktop or SSH and should start center once per
session — **not** a true boot service (does not run if nobody logs in):

```bash
grep -q 'setup.sh' ~/.bashrc || echo '/usr/share/skinos/setup.sh' >> ~/.bashrc
```

Prefer **§13.1** for production center hosts.

### 13.5 Verify auto-start

After reboot:

```bash
he 'land@machine.status'
he 'center@heport'
netstat -lntp | egrep '20000|20001|20002|20005'
```

Or open **`http://<center-host-IP>:20001`**. See **§9** for full checks.

---

## 14. Notes

- center runs on the **slave host platform**, not on product boards (swrt5, smtk2, …).
  Embedded gateways keep their own `.zz` firmware; only the **agent** project on each
  embedded gateway targets the center address.
- Firewall: open **20000–30000** **TCP and UDP** on the center host (**§9.3**).
  **20006–25000** is the dynamic pport pool; **25000–30000** is the static map range.
- Keep center and embedded gateway **agent** builds from the same SDK generation to avoid heport/pport
  protocol mismatch.
- Broader Ubuntu install steps (including 20.04): [`config/slave/install.md`](../../../config/slave/install.md).
