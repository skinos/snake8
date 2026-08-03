---
name: device-upgrade
description: |
  Build, deploy, and test landos firmware and FPK packages on remote devices via the web API.
  Use when the user says "帮我升级固件", "upgrade firmware", "上传fpk", "deploy to device",
  "测试一下设备", "remote test", "把编译的包传上去", or asks to push code changes to a running device.
  Also use when the user provides a device URL (e.g. http://x.x.x.x:port) and asks to upgrade or test.
  Choose .zz vs FPK by what changed: config/kernel → full .zz; project-only → FPK.
  After FPK upload success: do not restart — start debugging immediately.
  Do NOT use for local builds only (no device URL given).
---

# Device Upgrade & Remote Testing

Build firmware/FPK packages and deploy them to remote landos devices via the HTTP web API.
Supports full firmware (`.zz`) upgrade (`p2=restart` auto-reboot) and FPK package install (**hot-swap: no restart; debug right after `"status":"success"`**).

## What to deploy: `.zz` vs FPK (read this first)

| What you changed | Deploy with | Why |
|------------------|-------------|-----|
| **`config/…`** (kernel overlays e.g. `option.c`, `kernel.config`, `sdk.config`, `rootfs/`, board `*.cfg`, DTS, …) | **Full firmware `.zz`** | Baked into image / kernel; FPK cannot replace the running kernel |
| **`project/<name>/` only** (C / `prj.json` / WUI assets for one package) | **`make obj=<name>` → FPK** | Hot-install; **no reboot**; install success → debug immediately |
| Both `config/` **and** `project/` | **`.zz` first** (includes rebuilt apps), or `.zz` then extra FPK if you iterate on the app | Kernel/config always need `.zz` |

```text
Changed config/swrt5/kernel/option.c  →  ./mkdel && make  →  upload .zz (restart)
Changed only project/modem/mt5710/    →  make obj=modem   →  upload .fpk → debug now (no restart)
```

Same rule is recorded in **skinos-sdk** / **skinos-modem**. Do **not** assume FPK is enough after a kernel/`option.c` edit.

## Prerequisites

- **Device URL (`BASE`)** — the user gives it **once per chat session** (e.g. `http://IP:PORT`). Remember it for the rest of that session; do **not** ask again unless they change it. Do **not** invent a URL or reuse an address from docs/examples or a previous chat.
- **Credentials** — same rule: use what the user gives in this session; default only if they say so: `admin` / `admin`. Web API, SSH, and telnet use the **same** username/password.
- **SSH or telnet** — only when Linux shell is needed; ask the user once for `IP` + `port` (SSH or telnet), then reuse for the session. Not the same as web `BASE` port.
- For upgrade: a built `.zz` under `build/`, or an FPK under `build/store/`

If this session has **no** device URL yet and the user asks to upgrade/deploy, **ask once**, then keep using that `BASE`.

## Step 1: Build Firmware (.zz)

Do **not** use `make clean` (full rebuild is very slow). Prefer `./mkdel` then a full `make`:

```bash
# Check target board first
make pidinfo   # or: make pidlist

# Incremental clean of skinos build dirs, then full firmware image
./mkdel
make           # dep → kernel → app → install → pack → build/*.zz
```

Output: `build/*.zz` (full firmware image for the current `gBOARDID`).

If only `sdk.config` / platform config changed, same flow: `./mkdel` → `make`.

Skip this step when the `.zz` is already built and you only need to upload.

## Step 2: Authenticate

Get session key from the device. All commands use `curl` with JSON body.

```bash
BASE="http://DEVICE_IP:PORT"   # from user once this session; reuse afterward

# Get rand
RAND=$(curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' \
  --connect-timeout 10 --max-time 15 \
  | python3 -c "import sys,json; print(json.load(sys.stdin)['he']['rand'])")

# Compute password: MD5(plain:username:rand) then base64
MD5=$(printf '%s' "admin:admin:$RAND" | md5sum | awk '{print $1}')
PW=$(printf '%s' "$MD5" | base64 -w0)

# Get session key
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" \
  | python3 -c "import sys,json; print(json.load(sys.stdin).get('key',''))")
```

**Key expires** after each `/he` or `/upload` call — re-authenticate between operations if needed.

## Step 3: Full Firmware Upgrade (.zz)

Upload the built image. Pass **`p2=restart`** so the device restarts automatically after a successful upgrade (same as the web UI checkbox).

### Upload Firmware

```bash
# Find the .zz file
FIRMWARE=$(find build/ -name "*.zz" -type f | head -1)

# Upload firmware (p2=restart → auto reboot after success)
curl -sS -X POST \
  "$BASE/upload?username=admin&key=$KEY&object=arch@firmware&api=zz&p=%5B%5D&p2=restart" \
  -F "filename=@$FIRMWARE" \
  --connect-timeout 30 --max-time 600

# Response: {"status":"success",...} — device restarts by itself via p2=restart
```

### Confirm upgrade completed

When to start confirming depends **only** on whether `/upload` returned a success JSON — **do not** use any `wait` field in the response to override these times:

| Upload outcome | Meaning | Wait before confirm |
|----------------|---------|---------------------|
| Returns JSON with **`"status":"success"`** | Upgrade accepted; device auto-restarts | **30 seconds**, then confirm |
| Never returns / hangs / curl timeout (no usable response body) | Device may already be rebooting hard | **90 seconds**, then still run confirm |

After the wait, poll `land@machine.status` until success.

**Success criteria** (both required):
1. Response is valid JSON
2. Field `version` matches `^v[0-9]` (lowercase `v` + a digit), e.g. `v8.6.0703`

```bash
# If upload returned {"status":"success",...}: sleep 30
# If upload hung / timed out (no return): sleep 90
sleep 30   # or: sleep 90

for i in $(seq 1 20); do
  RESULT=$(curl -sS -X POST "$BASE/public" \
    -H 'Content-Type: application/json' \
    -d '{"he":"land@machine.status"}' \
    --connect-timeout 5 --max-time 10 2>/dev/null || true)
  if echo "$RESULT" | python3 -c '
import sys, json, re
try:
    d = json.load(sys.stdin)
    # status may be under "he" or top-level depending on /public wrapping
    he = d.get("he", d)
    if isinstance(he, str):
        he = json.loads(he)
    ver = he.get("version", "") if isinstance(he, dict) else ""
    sys.exit(0 if re.match(r"^v[0-9]", str(ver)) else 1)
except Exception:
    sys.exit(1)
'; then
    echo "Upgrade OK: $RESULT"
    break
  fi
  echo "Waiting for device... ($i)"
  sleep 10
done
```

### Recover: bad version after upgrade

If `land@machine.status` **does return JSON**, but `version` does **not** match `^v[0-9]`, the device is in an abnormal post-upgrade state. Trigger a software restart and re-confirm.

**Loop (max 3 attempts):**
1. Re-authenticate (key may have expired)
2. Call `land@machine.restart[3,upgrade]`
3. Wait **90 seconds**
4. Run **Confirm upgrade completed** again (same success criteria)
5. If still not `^v[0-9]`, repeat from step 1

If **3 restart cycles** still fail confirm → treat as **device hung**. Stop automating and **tell the user to manually power-cycle / reboot the device**.

```bash
# Re-authenticate first, then:
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"key\":\"$KEY\",\"he\":\"land@machine.restart[3,upgrade]\"}"

sleep 90
# then re-run Confirm upgrade completed (poll land@machine.status)
# repeat up to 3 times; if all fail → prompt user for manual reboot
```

## Step 4: FPK Package Build & Deploy (No Restart)

FPK packages are **hot-swappable**.

**After upload returns `"status":"success"`:**
- **Do not restart** the device (`land@machine.restart`, reboot, power cycle, etc.)
- **Do not** wait for a reboot window (that is only for `.zz` firmware)
- The new code is already active — **start debugging immediately** (HE APIs, telnet/`he`, logs, traffic checks)
- **Resolve on-device project paths with `land@fpk.path`** (see below) — never assume `/usr/share/skinos/…`

Only restart if a later debug step explicitly requires it (rare; not part of FPK install).

### Resolve project install path (`land@fpk.path`) — mandatory

After **FPK install** or **firmware (`.zz`) upgrade**, the live install directory of a project is **dynamic**. It changes with `gBOARDID` / `PROJECT_DIR`, overlay vs image bake, and hot-install vs factory path.

**Do not** hardcode or `find` under `/usr/share/skinos`, `/mnt/internal/skinos`, `/tmp/mnt/…`, etc.

**Always** query first:

```text
# eline
$ land@fpk.path[ gnss ]
/tmp/mnt/internal/skinos/gnss

# ash
~ # he 'land@fpk.path[ gnss ]'
```

| Need | HE |
|------|-----|
| One project root | `land@fpk.path[ <project> ]` → absolute directory string |
| All projects + paths | `land@fpk.list` → each entry has `"path"` |

Then use that root for binaries (`…/bin/<cmd>`), `prj.json`, libs, WUI assets, logs under the package, etc.  
Full API: `doc/com/land/fpk.md` (`path` / `list`).

### Build FPK

```bash
# Build single project as FPK
make dep              # only needed once
make obj=<project>    # e.g. make obj=ipsec
```

Output: `build/store/<project>-<version>-<hardware>.fpk`

### Upload FPK to Device

```bash
FPK_FILE="build/store/ipsec-1.0.0-mt7628.fpk"

curl -sS -X POST \
  "$BASE/upload?username=admin&key=$KEY&object=arch@firmware&api=fpk&p=%5B%5D" \
  -F "filename=@$FPK_FILE" \
  --connect-timeout 30 --max-time 120

# Response: {"status":"success"}
```

On success → go straight to **Step 5: Remote Testing**. No restart step in between.

### Initialize Component After FPK Install

Only when the component is **brand-new** on the device and still needs first-time setup/config (not required for updating an already-running package):

```bash
# Setup infrastructure (if needed)
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"<project>@list.setup"}'

# Create instance config
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"<project>@client|{\"status\":\"disable\",...}"}'
```

## Step 5: Remote Testing

Default to **web HE APIs** (same path the web UI uses). Only open a terminal when you need the raw Linux system.

### Web: call component APIs

Authenticate first (Step 2), then use `POST /he` — same as the web UI talking to components.

```bash
# Read config
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"<project>@<instance>"}'

# Call API
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"<project>@<instance>.<api>"}'

# Set config (pipe syntax)
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"<project>@<instance>|{\"key\":\"value\"}"}'
```

Upload files to a component API (e.g. certificate import):

```bash
curl -sS -X POST \
  "$BASE/upload?username=admin&key=$KEY&object=<project>@<instance>&api=<api_name>&p=%5B%5D" \
  -F "filename=@/path/to/local/file"
```

### Terminal: eline and Linux shell

Use this only when web tests are not enough (filesystem, processes, logs, binary presence, etc.).

**Full HE / eline grammar and prompt rules:** skill **skinos-he** (from `he.md` / `eline.md`). Summary:

1. Ask the user **once this session** for either:
   - **SSH**: `IP` + `port`, or
   - **Telnet**: `IP` + `port`
2. Username/password are the **same** as the web upgrade API for this session (default `admin`/`admin` only if the user said so).
3. After login you are in **eline** (`$ `) — type HE lines **directly** (no `he` prefix).
4. Type `ashy` and Enter to enter **Linux shell** (BusyBox ash, prompt often `~ #`). There you can run normal Linux commands, or run HE via **`he '…'`** (one single-quoted line, e.g. `he 'land@machine.status'`).
5. Leaving the Linux shell (`exit` or Ctrl+D) typically **closes the SSH/telnet session** (does not return to eline). Reconnect if you need eline again.

The address the user gives is often a **NAT-mapped** host port. Do **not** substitute `ssh_port` / `telnet_port` / `wui_port` from `land@machine.status` (those are device-local, e.g. 22/23/80).

```bash
# Password auth example (interactive or via pexpect/sshpass)
ssh -o PreferredAuthentications=password -o PubkeyAuthentication=no \
  -p SSH_PORT USER@DEVICE_IP
# or:
telnet DEVICE_IP TELNET_PORT
```

```text
# --- after login: eline ($), HE directly ---
$ land@machine.status
$ ipsec@client
$ ipsec@client.key

# --- enter Linux shell ---
$ ashy

# --- Linux shell (~ #): OS commands + he ---
~ # uname -a
~ # he 'land@fpk.path[ gnss ]'    # NEVER assume /usr/share/skinos
~ # he 'land@machine.status'
~ # exit          # disconnects the session (reconnect for eline again)
```

| Prompt | Mode | HE |
|--------|------|-----|
| `$ ` | eline | direct: `land@machine.status` |
| `~ #` / `# ` | Linux shell (BusyBox) | `he 'land@machine.status'` |

Do **not** invent SSH/telnet addresses. If shell access is needed and the user has not given one this session, ask for SSH or telnet `IP:port` first, then remember it for the session.

### IPsec remote test (PSK hybrid hub)

When the user asks to test IPsec against a lab strongSwan server:

1. **Session inputs** (ask once if missing, then remember):
   - Device web `BASE` (and telnet/SSH if shell needed)
   - IPsec **server SSH** (host/port/user/password) — used only to **read** server config; do **not** commit secrets into docs
2. On the Ubuntu hub, prefer reading `/etc/ipsec.conf` (often world-readable) and `/etc/ipsec.secrets` (usually needs `sudo`). Comments in `ipsec.conf` may already list landos client fields.
3. Map classic strongSwan **hub** → landos **`ipsec@client`**:

| Server (`ipsec.conf`) | Client (`ipsec@client`) |
|----------------------|-------------------------|
| Public IP / DNS of hub | `server` |
| `leftid=@…` | `remote_id` (keep `@` if present) |
| `leftauth=psk` / secrets PSK | `auth_method=psk`, `psk=…` |
| `leftsubnet=…` | `remote_ts=…` |
| `rightsourceip=…` (VIP pool) | `vip=enable` |
| `keyexchange=ikev2` | `version=2` |
| (optional) | `extern=default` unless user specifies WAN/LTE |

4. Device side workflow:
   - `ipsec@list.setup` then set `ipsec@client|{…,"status":"enable"}`
   - After FPK install or config change, call `ipsec@client.reset` if the tunnel stays down
   - **Poll** `ipsec@client.status` for 15–60s — first replies are often `"status":"down"` while negotiating; success is `"status":"established"` (IKE ESTABLISHED + child INSTALLED). Do **not** fail on the first `down`
5. Cross-check when web status is unclear: telnet/SSH → eline `ipsec@client.status`, or ash `swanctl --list-sas`; on the hub `sudo ipsec statusall` (VIP pool online count / SA up).
6. **Never** hardcode lab PSK, SSH passwords, or device URLs into `SKILL.md` / `AGENTS.md`. Keep them session-only from the user (or read live from the hub).

### Re-auth between HE calls

`key` from `/auth` is often **one-shot** (response may return a new `key`). For scripts: **re-authenticate before every** `/he` or `/upload`, or always use the `key` returned in the previous response.

## Complete Workflow Example: Build, Deploy, Test IPsec

`BASE` below is a placeholder — use the device URL the user gave **in this session** (ask once if missing, then reuse).

```bash
BASE="http://DEVICE_IP:PORT"   # set once from user for this session

# 1. Build IPsec FPK
make obj=ipsec

# 2. Authenticate
RAND=$(curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' | python3 -c "import sys,json; print(json.load(sys.stdin)['he']['rand'])")
MD5=$(printf '%s' "admin:admin:$RAND" | md5sum | awk '{print $1}')
PW=$(printf '%s' "$MD5" | base64 -w0)
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" | python3 -c "import sys,json; print(json.load(sys.stdin).get('key',''))")

# 3. Upload FPK (no restart)
curl -sS -X POST "$BASE/upload?username=admin&key=$KEY&object=arch@firmware&api=fpk&p=%5B%5D" \
  -F "filename=@build/store/ipsec-1.0.0-mt7628.fpk"

# 4. Setup and test
curl -sS -X POST "$BASE/he" -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"ipsec@list.setup"}'

curl -sS -X POST "$BASE/he" -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"ipsec@client|{\"status\":\"disable\",\"server\":\"10.0.0.1\",\"auth_method\":\"pubkey\"}"}'

# 5. Test certificate APIs
curl -sS -X POST "$BASE/upload?username=admin&key=$KEY&object=ipsec@client&api=import_ca&p=%5B%5D" \
  -F "filename=@/tmp/test_ca.pem"

curl -sS -X POST "$BASE/he" -H 'Content-Type: application/json' \
  -d '{"username":"admin","key":"'"$KEY"'","he":"ipsec@client.key"}'
# Expected: {"ca":"/etc/config/ipsec/ipsec@client.ca",...}
```

## Certificate Handling Convention (UART/IPsec)

When implementing certificate management for a new component, follow the UART pattern:

- **File extensions**: `.ca` (CA cert), `.crt` (client cert), `.key` (private key)
- **File naming**: `<instance>.{ca,crt,key}` via `config_path(PROJECT_ID, "%s.crt", object)`
- **HE APIs**: `import_ca[file]`, `import_cert[file]`, `import_key[file]`, `clear_ca[]`, `clear_cert[]`, `clear_key[]`, `key[]`
- **JSON key in key[] response**: `"ca"`, `"crt"`, `"key"` (not `"cert"`)
- **HTML IDs**: `<prefix>_ca`, `<prefix>_crt`, `<prefix>_key` with `_down`/`_remove` suffixes
- **Upload URL**: `/upload?...&object=<project>@<instance>&api=import_ca&p=[]`
- **Delete command**: `<project>@<instance>.clear_ca`

## Troubleshooting

| Error | Cause | Fix |
|-------|-------|-----|
| `Data Error` | Wrong JSON format | Use `{"he":"..."}` not `["cmd"]` |
| `Auth Error` | Command restricted or expired key | Re-authenticate; check helist |
| `tpanic,22` | Component not registered | Call `<project>@list.setup` first |
| `tpanic,89` | Command not in helist | Use authenticated `/he` not `/public` |
| `{"return":"false"}` | Login failed | Fetch new `rand`, recompute password |
| Device not responding after upgrade | Still restarting | Upload returned `"status":"success"` → wait **30s**; upload hung / no return → wait **90s**; then poll `land@machine.status` until `version` matches `^v[0-9]` |
| Upload `/zz` never returns | Likely upgrade/reboot mid-request | Do **not** retry upload immediately; wait **90s**, then run confirm |
| `status` OK but `version` not `^v[0-9]` | Abnormal post-upgrade state | `land@machine.restart[3,upgrade]` → wait 90s → confirm; max **3** cycles; then tell user to **manual reboot** |
| FPK install but APIs return NULL | Instance not created | Set config with pipe syntax first |
| Cannot find FPK files / `cmd` under `/usr/share/skinos` | Install path is dynamic | `land@fpk.path[ <project> ]` (or `land@fpk.list`) — never hardcode share/mnt paths |
| `ipsec@client.status` = `down` right after enable/reset | Still negotiating | Poll 15–60s for `established`; check `swanctl --list-sas` / hub `ipsec statusall` |
| IPsec params unknown | No prior session record | SSH to hub, read `/etc/ipsec.conf` (+ secrets); map left* → `ipsec@client` (see IPsec section) |
