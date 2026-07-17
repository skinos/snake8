# SkinOS Cloud Management Platform — Installation Guide (Ubuntu)

This guide installs and runs the **SkinOS cloud management platform** on Ubuntu
using the host/debug board identity `slave-x86-ubuntu2004` (Ubuntu 20.04)
or `slave-x86-ubuntu2404` (Ubuntu 24.04).

**Recommended OS:** Ubuntu 20.04 or Ubuntu 24.04

---

## 1. Prerequisites

Install Git and Make if they are not already available:

```bash
sudo apt-get update
sudo apt-get install -y git make
```

---

## 2. Download the source code

```bash
git clone https://gitee.com/snake8/snake8.git
cd snake8
```

---

## 3. Install build tools

After the source tree is available, install the full build toolchain:

```bash
make preset
```

`make preset` installs compilers, libraries, and related development packages
required to build SkinOS on Ubuntu.

---

## 4. Select board identity

Select the board identity for your Ubuntu version:

```bash
# Ubuntu 20.04
make pid gBOARDID=slave-x86-ubuntu2004

# Ubuntu 24.04
make pid gBOARDID=slave-x86-ubuntu2404
```

> Tip: If `gBOARDID` is not set, the top-level Makefile defaults to
> `slave-x86-ubuntu2004`. On Ubuntu 24.04, set `slave-x86-ubuntu2404`
> explicitly as shown above.

---

## 5. Update FPK packages and SDK

Download / refresh the FPK packages and platform SDK assets for the current board:

```bash
make update
```

What this step does (internally):

- Pulls the latest repository changes
- Updates / downloads platform packages under `config/slave/`
- Refreshes FPK packages used by the SkinOS root filesystem

---

## 6. Build

Compile the SkinOS cloud management platform:

```bash
make
```

This prepares the build directories, compiles the application components for the
selected Ubuntu board identity, and stages the result under `build/rootfs/`.

---

## 7. Install

Install the staged SkinOS runtime onto the host system:

```bash
make sdk_install
```

This copies the platform files to:

- `/usr/share/skinos`
- `/usr/local/lib`
- `/usr/local/bin`

and refreshes the shared library cache with `ldconfig`.

> Note: `make install` is an alias of `make sdk_install`.

---

## 8. Start the cloud platform

Start SkinOS:

```bash
make sdk_start
```

This runs `/usr/share/skinos/setup.sh`, which initializes the SkinOS daemon
and related services.

> Note: `make start` is an alias of `make sdk_start`.

Useful related commands:

```bash
make sdk_stop      # stop the running platform (also: make stop)
make sdk_uninstall # stop and remove /usr/share/skinos
```

---

## 9. Enable auto-start on boot (Ubuntu)

To start the cloud platform automatically after reboot, arrange for
`/usr/share/skinos/setup.sh` to run at boot.

### Option A — systemd (recommended)

Create a systemd unit:

```bash
sudo tee /etc/systemd/system/skinos.service >/dev/null <<'EOF'
[Unit]
Description=SkinOS Cloud Management Platform
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

Enable and start the service:

```bash
sudo systemctl daemon-reload
sudo systemctl enable skinos.service
sudo systemctl start skinos.service
```

Check status / logs:

```bash
sudo systemctl status skinos.service
journalctl -u skinos.service -f
```

Disable auto-start (if needed):

```bash
sudo systemctl disable skinos.service
sudo systemctl stop skinos.service
```

### Option B — rc.local (simple fallback)

If `systemd` unit management is not preferred, use `rc.local`:

```bash
sudo tee /etc/rc.local >/dev/null <<'EOF'
#!/bin/bash
/usr/share/skinos/setup.sh
exit 0
EOF

sudo chmod +x /etc/rc.local
sudo systemctl enable rc-local.service 2>/dev/null || true
```

On some Ubuntu versions you may also need:

```bash
sudo systemctl enable rc-local
sudo systemctl start rc-local
```

---

## Quick reference

| Step | Command |
| --- | --- |
| Clone | `git clone https://gitee.com/snake8/snake8.git` |
| Install tools | `make preset` |
| Select board (20.04) | `make pid gBOARDID=slave-x86-ubuntu2004` |
| Select board (24.04) | `make pid gBOARDID=slave-x86-ubuntu2404` |
| Update packages / SDK | `make update` |
| Build | `make` |
| Install to host | `make sdk_install` |
| Start | `make sdk_start` |
| Stop | `make sdk_stop` |
| Auto-start | systemd unit for `/usr/share/skinos/setup.sh` |

---

## Notes

- Run `make sdk_install` again after rebuilding if you need to refresh the host installation.
- Boot auto-start must target `/usr/share/skinos/setup.sh` on the installed host path, not the source tree.
- The platform layout and board selection are driven by `gBOARDID` in the top-level Makefile.
