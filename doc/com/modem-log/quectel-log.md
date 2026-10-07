# quectel-log — Quectel Modem Log Capture Tool (QLog)

Package source: `config/swrt5/cdriver/modem-log/quectel-log`  
Binary name on device: `quectel-log` (built from QLog V1.5.17)  
Upstream name in sources / prebuilt sample: `QLog`

Captures Quectel module DIAG / dump / platform logs (Qualcomm, Unisoc, ASR, MTK, Eigen, Sony, …) over USB or UART. Filter configs (`*.config`) are installed next to the binary in the `modem-log` FPK.


## Prerequisites

- Module USB (or UART) log port is present, typically DIAG such as `/dev/ttyUSB0`.
- Prefer running with **cwd** = `land@fpk.path[ modem-log ]` when using `-f` with a packaged `*.config` file by relative name.
- Enough free space under the save directory.


## Usage

```text
quectel-log -p <log port> -s <log save dir> -f <filter_cfg> -n <max file num> -m <size MB>
```

Show help:

```shell
quectel-log -h
```


## Options

| Flag | Meaning |
|------|---------|
| `-p` | Log / DIAG port. Accepts `/dev/ttyUSB0`, `ttyUSB0`, or `USB0` |
| `-s` | Save directory (default tool-internal name if omitted). Also supports special forms below |
| `-f` | Filter config file (e.g. `default.config`, `5GNR_LTE_CN_V11.config`). Omit to use built-in default. UC200T / EC200T often need no filter |
| `-n` | Max number of log files to keep (`0`–`512`). When exceeded, oldest files are removed |
| `-m` | Max size of a **single** log file in MB (`2`–`512`, default 256) |
| `-D` | Delete existing logs in the save dir before capture (`-D` all, or `-Dqmdl` for suffix) |
| `-c` | Continue after dump capture (default exits after dump) |
| `-q` | Exit after USB disconnect |
| `-x` | Capture udx710 NMEA port log |
| `-t` | UART / COM data mode (Sony BG770A-GL; Unisoc EXX00U AP UART — pair with `-m 10`) |
| `-i` | Ignore Unisoc EXX00U AP log (default captures AP) |
| `-a` | EXX00U blue-screen dump: panic address, e.g. `-a 0x12345678` |
| `-g` | State-grid module model, e.g. `-g EC200T` |
| `-h` | Help |

Note: the printed help line mentions `-b` for size; the implemented flag is **`-m`**.


## `-s` special modes

| Value | Behavior |
|-------|----------|
| directory path | Save files locally (normal mode) |
| `9000` (port ≥ 9000) | TCP server; connect with QPST / QWinLog / CATStudio / Logel |
| `IP:9000` | TCP client; send log to a listener such as `nc -l 9000 > log.bin` |
| `tftp:IP` | TFTP client upload |
| `ftp:IP-user:xxx-pass:xxx` | FTP client upload (user/pass ≤ 32 bytes) |


## Examples

Local capture to `/tmp/file`:

```shell
cd "$(he 'land@fpk.path[ modem-log ]')"
./quectel-log -p /dev/ttyUSB0 -s /tmp/file -m 100 -n 10
```

With an explicit filter from the FPK directory:

```shell
./quectel-log -p /dev/ttyUSB0 -s /tmp/file -f default.config -m 100 -n 10
```

Minimal (tool defaults for port / filter):

```shell
./quectel-log -s /tmp/file
```

Dump catch (module already in dump / Sahara mode):

```shell
./quectel-log -s /tmp/file/dump
```

TCP server for PC tools:

```shell
./quectel-log -p /dev/ttyUSB0 -s 9000
```


## Packaged filter configs

Installed with the FPK (names may vary by release):

- `default.config`
- `defaultNR5G1216.config`
- `5GNR_LTE_CN_V11.config`
- `NR5GRegistration0608.config`
- `T1` … `T7` scenario configs


## Stop capture

Runs until SIGINT / SIGTERM (Ctrl+C), or until dump finishes (unless `-c`).

```shell
killall quectel-log
```

Skinos wrapper (see `project/modem`): `script@quectel-log.stop`


## Notes

- Eigen dump: start the tool within about **16 seconds** after the module enters dump, or capture may time out.
- For Qualcomm crash dump workflows, prepare the module (disable auto-reset / force dump) before starting; see `example_catch_dump.readme.txt` in the source tree.
- Bound `-m` / `-n` on devices with limited `/tmp` or flash.
