## land@machine — Gateway identity, runtime snapshot, and host control

### Overview

The **machine** component (`land@machine`, `MACHINE_COM`) holds persisted gateway identity and operating-mode fields, exposes a consolidated runtime and product snapshot, provides Linux-oriented host resource queries where the platform supports them, and implements guarded reboot and factory-related operations. It is registered in the **land** project (`prj.json`: system basic information management).

### Architecture

- **Startup (`setup`)**: Scans the system project list, registers the **land** project with **fpk**, registers other projects unless **`custom@project`** marks a project as `disable`, applies **`name`** as the host name (length limited by `HOST_NAME_MAX` when non-empty), copies **Web UI**, **telnet**, **SSH**, and **LAN static IP** values from peer components into the global **register** store, and logs completion.
- **Configuration view**: Persisted **`land@machine`** keys are merged with **`arch@data`** (`DATA_COM`) fields for serial identity and language or version metadata when the configuration is read. If **`mode`** is missing or empty in storage, the effective value for display is **`default`**. Writes cannot change **`sn`**, **`mac`**, or **`macid`** through this component; **`language`**, **`gpversion`**, and **`cfgversion`** are applied to **`arch@data`**; changing **`mode`** toggles **LAN** **`dhcps/status`** (`disable` when **`mode`** is **`mbridge`** or **`default`**, otherwise **`enable`**). Other writable keys persist under the normal **`land@machine`** path.
- **Status (`status`)**: Starts from the persisted **`land@machine`** object, then overwrites or adds register-backed and **`arch@data`** fields: platform identifiers, optional health or random numeric fields, uptime and clock strings, product metadata, and service endpoint strings published at **setup**.
- **Restart path**: Honors **`lock`** and **`block_restart`**; sets register **`machine_state`** to **`restarting`**, publishes **`machine/status`**, then schedules or performs reboot (**`daemon stop15exit`**, **`reboot`**, or **`reboot(RB_AUTOBOOT)`** with fallbacks).
- **Resource helpers (`machine_misc.c`)**: Parse **`/proc/stat`**, **`/proc/meminfo`**, **`/proc/*/status`**, and **`df`** output (Linux-oriented); **`sginfo`** and **`esinfo`** reshape **`fsinfo`**-like data using **`PROJECT_CFG_DIR`**, **`PROJECT_MNT_DIR`**, and **`PROJECT_INT_DIR`**.

### Dependencies

- **Skin runtime** (`skin/skin.h`): configuration, register, scall helpers, joint calls, shell helpers.
- **`arch@data` (`DATA_COM`)**: Identity and product fields merged into the configuration view and into **`status`**; writable for **`language`**, **`gpversion`**, **`cfgversion`** through the configuration write path described under **Configuration Reference**.
- **`fpk` / project scan**: Project registration at boot; **`custom@project`** may disable individual projects.
- **Peer configuration sources**: **`wui`**, **`telnetd`**, **`sshd`**, **`lan`** (read during **setup** for register publishing and **`mode`**-related LAN DHCP policy).
- **`lock` (`LOCK_COM`)**: Gates **restart** and **default** versus **release** / **factory** per policy flags.
- **OS**: `sethostname`, `reboot`, `fork` / `sleep`, **`/proc`**, **`df`** (resource helpers).

---

### Configuration Reference ( land@machine )

#### Configuration attributes

```json
{
    "mode": "deployment or operator mode",                         // [ "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix", "mwm", "mbridge", "default", ... ], <product-specific semantics for each literal>
                                                                      // "mbridge": LAN DHCP server (**`lan`** **`dhcps/status`**) is set to **disable** on write when this value is applied
                                                                      // "default": same LAN DHCP side effect as **mbridge** on write
                                                                      // <other values>: LAN **`dhcps/status`** set to **enable** on write when **mode** changes
                                                                      // Default when absent or empty on read: effective display value **default**

    "name": "host name for the device",                             // [ string ], applied at **setup** via `sethostname` when non-empty; length clamped to **HOST_NAME_MAX**

    "mac": "MAC address (identity)",                                // [ mac address ], merged from **`arch@data`** for display; not writable through this component

    "macid": "MAC-derived or serial-related identifier",            // [ string ], merged from **`arch@data`**; not writable through this component

    "sn": "serial number",                                          // [ string ], merged from **`arch@data`**; not writable through this component

    "language": "UI or configuration language tag",                 // [ "cn", "en", "jp", ... ], stored in **`arch@data`** when written

    "cfgversion": "configuration version string",                   // [ string ], stored in **`arch@data`** when written

    "gpversion": "group configuration version string",              // [ string ], stored in **`arch@data`** when written

    "broken": "optional health or quality flag for other subsystems" // [ string ], <producer-defined literals such as **ill** or **disable** if used>; not assigned in **machine.c**
}
```

#### Configuration Example

```shell
land@machine
{
    "mode":"nmisp",                    # example deployment mode
    "name":"ASHYELF-12AAD0",
    "mac":"00:03:7F:12:AA:D0",
    "macid":"00037F12AAD0",
    "language":"cn",
    "cfgversion":"1"
}
```

#### Configuration write example (mandatory)

Writing **`land@machine`** updates persisted configuration under the framework’s storage rules. **`name`** takes effect on the next **setup** (or reboot path that runs **setup**). **`mode`** immediately adjusts **LAN** **`dhcps/status`** as described for **`mbridge`** / **`default`** versus other values. **`language`**, **`cfgversion`**, and **`gpversion`** are written to **`arch@data`**, not only to **`land@machine`**. Attempts to change **`sn`**, **`mac`**, or **`macid`** through this component are rejected (the write fails for those paths). Use the host environment’s usual attribute or merge syntax for **`land@machine`**.

```shell
land@machine|{"name":"DemoGW","language":"en","mode":"gateway"}
```

---

### API Reference

#### Management APIs

##### `setup[]` — Project registration, hostname, and register publish

- Called by the platform during initialization (not a routine operator API).
- failed return `tfalse`
- succeed return `ttrue`
- error return `terror` (if raised by the platform)

#### Query APIs

##### `status[]` — Runtime gateway, product, and service snapshot

- failed return `NULL`
- error return `terror`
- succeed return json
    {
        "mode": "effective deployment mode string",                 // [ string ], from persisted **`land@machine`**; if missing in storage, readers of configuration see **default**; this snapshot includes the stored object first

        "name": "configured host name",                             // [ string ], from persisted **`land@machine`** when present

        "broken": "optional health flag",                           // [ string ], present only if stored under **`land@machine`**; not produced by **machine.c** itself

        "platform": "platform label",                               // [ string ], from register **platform**; **error** if unset

        "hardware": "hardware label",                               // [ string ], from register **hardware**; **error** if unset

        "custom": "custom product label",                           // [ string ], from register **custom**; **error** if unset

        "scope": "scope or region label",                           // [ string ], from register **scope**; **error** if unset

        "version": "firmware or transient state string",            // [ string ], register **machine_state** when non-empty, otherwise register **version**; **error** if neither available

        "ill": "non-zero health indicator",                         // [ number ], present only when register **ill** is non-zero

        "rand": "optional numeric token from register",             // [ number ], present only when register **rand** is defined

        "livetime": "uptime description",                           // [ string ], from **`uptime_desc`**

        "current": "current time description",                        // [ string ], from **`date_desc`**

        "mac": "MAC address",                                         // [ mac address ], from **`arch@data`**

        "macid": "MAC-derived or related id",                       // [ string ], from **`arch@data`**

        "model": "product model",                                     // [ string ], from **`arch@data`**

        "features": "feature string",                               // [ string ], from **`arch@data`**

        "cmodel": "customer model string",                           // [ string ], from **`arch@data`**

        "oem": "OEM identifier",                                      // [ string ], from **`arch@data`**

        "magic": "magic or product key string",                     // [ string ], from **`arch@data`**

        "datecode": "manufacturing or date code",                   // [ string ], from **`arch@data`**

        "gpversion": "group version",                               // [ string ], from **`arch@data`**

        "cfgversion": "configuration version",                      // [ string ], from **`arch@data`**

        "wui_port": "Web UI listen port",                           // [ string ], from register when **setup** stored it

        "telnet_port": "telnet listen port",                        // [ string ], from register when present

        "ssh_port": "SSH listen port",                              // [ string ], from register when present

        "local_ip": "LAN static IP published at setup",             // [ string ], from register when **lan** **static/ip** was set at **setup**
    }

##### `cpuinfo[]` — CPU time counters from `/proc/stat` (Linux)

- failed return `NULL` (for example cannot open **`/proc/stat`**)
- error return `terror`
- succeed return json
    {
        "<cpuid>": {                                                // [ "cpu", "cpu0", "cpu1", ... ]: [ json ], one nested object per **`/proc/stat`** line whose first token starts with **cpu**, until the first line that does not
                                                                       // "cpu": aggregate line when present
                                                                       // "cpuN": per-CPU line when present
            "user": "user jiffies string",                          // [ string ], **`/proc/stat`** column
            "nice": "nice jiffies string",                          // [ string ]
            "system": "system jiffies string",                      // [ string ]
            "idle": "idle jiffies string",                          // [ string ]
            "iowait": "iowait jiffies string",                      // [ string ]
            "irq": "irq jiffies string",                            // [ string ]
            "softirq": "softirq jiffies string"                     // [ string ]
        }
    }

##### `meminfo[]` — Selected fields from `/proc/meminfo` (Linux)

- failed return `NULL`
- succeed return json
    {
        "total": "MemTotal token (kB)",                             // [ string ], first field after label from **`MemTotal:`**
        "free": "MemFree token (kB)",                               // [ string ], from **`MemFree:`**
        "buffers": "Buffers token (kB)",                            // [ string ], from **`Buffers:`**
        "cached": "Cached token (kB)"                               // [ string ], from **`Cached:`**; parsing stops after this line is handled
    }

##### `psinfo[]` — Process table summary from `/proc/*/status` (Linux)

- failed return `NULL`
- succeed return json
    {
        "<pid>": {                                                  // [ string ]: [ json ], **pid** is a decimal directory name under **`/proc`**
            "name": "process name token",                           // [ string ], from **Name:**
            "state": "process state token",                         // [ string ], from **State:**
            "vmsize": "VmSize token",                               // [ string ], from **VmSize:** when present
            "fdsize": "FDSize token",                               // [ string ], from **FDSize:** when present
            "ppid": "parent PID token"                              // [ string ], from **PPid:** when present
        }
    }

##### `psnumber[]` — Count of processes with readable `/proc/<pid>/status` (Linux)

- failed return `NULL`
- succeed return json
    {
        // [ number ], non-negative integer count; one increment per **`/proc/<pid>`** directory for which **`/proc/<pid>/status`** opens successfully
    }

##### `fsinfo[]` — Mounted filesystem usage from `df` (Linux)

- failed return `NULL`
- succeed return json
    {
        "<mountpoint>": {                                            // [ string ]: [ json ], mount path as key; only **`df`** rows whose line starts with **`/`** or **`ubi`** are included
            "filesystem": "source device or pseudo-fs",            // [ string ], first column from **`df`**
            "size": "total size token",                              // [ string ]
            "used": "used token",                                    // [ string ]
            "available": "available token",                          // [ string ]
            "use": "percentage or use token"                         // [ string ], fifth column from parsed **`df`** line
        }
    }

##### `sginfo[]` — Config partition plus extension mounts

- failed return `NULL`
- succeed return json
    {
        "config": {                                                  // [ json ], same inner keys as one **`fsinfo`** value for **`PROJECT_CFG_DIR`**
            "filesystem": "source device or pseudo-fs",            // [ string ]
            "size": "total size token",                              // [ string ]
            "used": "used token",                                    // [ string ]
            "available": "available token",                          // [ string ]
            "use": "percentage or use token"                         // [ string ]
        },

        "<mntname>": {                                               // [ string ]: [ json ], **mntname** is a subdirectory of **`PROJECT_MNT_DIR`** (excluding empty and **.**) with a matching **`df`** mount path substring
            "path": "PROJECT_MNT_DIR/<mntname>",                    // [ string ]
            "size": "total size token",                              // [ string ], from matching **`fsinfo`** row
            "used": "used token",                                    // [ string ]
            "available": "available token",                          // [ string ]
            "use": "percentage or use token"                         // [ string ]
        }
    }

##### `esinfo[]` — Extension storage only (excludes built-in config and internal volume names)

- failed return `NULL`
- succeed return json
    {
        "<mntname>": {                                               // [ string ]: [ json ], same inner keys as **`sginfo`** extension entries; basenames of **`PROJECT_CFG_DIR`** and **`PROJECT_INT_DIR`** are omitted as keys
            "path": "PROJECT_MNT_DIR/<mntname>",                    // [ string ]
            "size": "total size token",                              // [ string ]
            "used": "used token",                                    // [ string ]
            "available": "available token",                          // [ string ]
            "use": "percentage or use token"                         // [ string ]
        }
    }

#### Control APIs

##### `restart[ delay, key ]` — Reboot or schedule reboot

- `delay` --------- [ string ], optional decimal seconds; when strictly positive, forks a child that sleeps then invokes the same operation with no delay
- `key` ----------- [ string ], optional label included in warning logs
- failed return `tfalse` (**`lock.default=enable`**, **`block_restart`**, or **`fork`** failure)
- succeed return `ttrue`
- error return `terror` (for example **`EPERM`** when locked)

##### `reboot[ delay, key ]` — Same behavior as **`restart`**

- `delay` --------- [ string ], optional, same as **`restart`**
- `key` ----------- [ string ], optional, same as **`restart`**
- failed return `tfalse`
- succeed return `ttrue`
- error return `terror`

##### `restart_block[]` / `reboot_block[]` — Set register flag to block restart

- succeed return `ttrue`
- Sets register **`block_restart`** to true.

##### `restart_unblock[]` / `reboot_unblock[]` — Clear restart block

- succeed return `ttrue`
- Sets register **`block_restart`** to false.

##### `default[ delay, key ]` — Restore **`arch@data`** defaults then restart

- Honors **`lock.default`** and register **`block_default`**.
- `delay` --------- [ string ], passed through to **restart** after **`arch@data` `default`** succeeds
- `key` ----------- [ string ], passed through to **restart**
- failed return `tfalse`
- succeed return `ttrue`
- error return `terror` when locked or blocked

##### `default_block[]` — Block **default**, **release**, and **factory** paths that consult **`block_default`**

- succeed return `ttrue`

##### `default_unblock[]` — Clear **`block_default`**

- succeed return `ttrue`

##### `release[ delay, key ]` — Product **release** on **`arch@data`** then restart

- Honors **`lock.factory`** and **`block_default`**.
- `delay` --------- [ string ], forwarded to **restart** on success
- `key` ----------- [ string ], forwarded to **restart**
- failed return `tfalse`
- succeed return `ttrue`
- error return `terror`

##### `factory[ delay, key ]` — Factory reset on **`arch@data`** then restart

- Same locking and **`block_default`** behavior as **`release`**.
- `delay` --------- [ string ], forwarded to **restart** on success
- `key` ----------- [ string ], forwarded to **restart**
- failed return `tfalse`
- succeed return `ttrue`
- error return `terror`

**Policy summary**

- **`lock.default=enable`**: blocks **`restart`** / **`reboot`** and **`default`**.
- **`lock.factory=enable`**: blocks **`release`** and **`factory`**.
- Register **`block_restart`** / **`block_default`**: used with the block and unblock APIs above.

---

### Published Joint Events

| Event | When | Argument | Argument2 | Argument3 |
|-------|------|----------|-----------|-----------|
| `machine/status` | After **`machine_state`** is set to **`restarting`** during the restart sequence | | | |

---

### Other

- **`cpuinfo`**, **`meminfo`**, **`psinfo`**, **`psnumber`**, **`fsinfo`**, **`sginfo`**, and **`esinfo`** expect Linux-style **`/proc`** and **`df`**; on other targets they may return **`NULL`**.
- C usage patterns follow **`skin/skin.h`** (`sgets`, `scall`, `talk_free`, etc.); see **land** tree samples for embedding.
