# fibocom-log — Fibocom Modem Log Capture Tool

Package source: `config/swrt5/cdriver/modem-log/fibocom-log`  
Binary name on device: `fibocom-log` (from FPK project `modem-log`)  
Version in tree: Fibocom MultiPlatform logtool V1.5.0.6

Captures Qualcomm / Unisoc / MTK / Eigen / ZTE / Samsung platform logs from Fibocom modules over USB DIAG (or related) ports. Runtime configs shipped with the FPK include `devices.ini`, `Channel.ini`, `qxdm_default.cfg`, and ZTE rule files.


## Prerequisites

- Module USB interfaces are enumerated (`/dev/ttyUSB*` or sysfs USB device path).
- Prefer running with **cwd** = `land@fpk.path[ modem-log ]` so relative files such as `devices.ini` resolve correctly.
- Enough free space under the save directory.


## Common options

The top-level process selects the platform handler from the plugged module, then that handler re-parses argv. Shared flags:

| Flag | Meaning |
|------|---------|
| `-d` / `-p` / `-e` | DIAG / log port, e.g. `/dev/ttyUSB0`, or USB syspath |
| `-s` | Directory to save log files |
| `-m` | Max size of a single log file (MB) |
| `-n` | Max number of rotated log files |
| `-f` | Filter / QXDM config file (Qualcomm path; e.g. `qxdm_default.cfg`) |
| `-a` | Unisoc: enable AP log |
| `-i` `-u` `-w` | Remote save: IP / username / password (FTP-style platforms) |
| `-P` | Host / UDX710 port-proxy mode |
| `-h` | Help (platform-specific text after module detect) |

With default USB mode and a single Fibocom module present, the tool can be started with no arguments.


## Platform examples

### Qualcomm modules

```shell
fibocom-log -d /dev/ttyUSB0 -s /tmp/file -f qxdm_default.cfg -m 100 -n 10
```

Or rely on auto-detect and defaults:

```shell
cd "$(he 'land@fpk.path[ modem-log ]')"
./fibocom-log -s /tmp/file -m 100 -n 10
```

### Unisoc modules (e.g. FM650 / FM160 family)

```shell
fibocom-log -p /dev/ttyUSB3 -s /tmp/file -m 100 -n 10
```

Optional AP log:

```shell
fibocom-log -a -p /dev/ttyUSB3 -s /tmp/file -m 100 -n 10
```

### Eigen / ZTE / Samsung / MTK

Same pattern: set DIAG/log port with `-d` or `-p`, save dir with `-s`, rotate with `-m` / `-n`. Run from the `modem-log` install directory so companion `.ini` / `.rule` / `.cfg` files are found.


## Multiple modules

If more than one Fibocom device is present, the tool prints how many were found and requires an explicit port or syspath:

```text
please set portname <-d /dev/XXX> or set syspath <-d /sys/bus/usb/devices/...>
```


## Stop capture

The tool runs until interrupted (SIGINT / SIGTERM). From shell:

```shell
killall fibocom-log
```

Skinos wrapper (see `project/modem`): `script@fibocom-log.stop`


## Notes

- USB port must already be enumerated before starting.
- Log volume grows quickly; keep `-m` / `-n` bounded on flash-limited devices.
- Device table extension: edit `devices.ini` (vid/pid, `ifnum`, `log_main_function`) in the FPK directory.
