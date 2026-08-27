## script@fibocom-log — Fibocom Modem Log Capture

### Overview

Shell component (flat `exe` script under `project/modem/fibocom-log`) that starts
background **fibocom-log** from the `modem-log` FPK on `lte` / `lte2`.

Before capture it stops `network@connect`, shuts `ifname@lte` / `modem@lte` (or `lte2`),
reads `modem@….status:name` (before shut), sends module-specific diag-enable AT on
`stty` (`AT+GTLOGEN=1` for FM650/FG650/FG652, `AT+GTTESTDIAG=1` for FM160), then
`AT+CFUN=0`, starts fibocom-log on `dtty`, then `AT+CFUN=1`, waits 2 seconds, then
`modem@….setup` and `ifname@….setup` so the attach and dial sequence is in the log.

Capture files go under **`FIBOCOM_LOG_DIR`** (default `/tmp/file`, usable via `wui@file`).
Requires the `modem-log` package and `tip` (modem cmd).


### Configuration reference ( script@fibocom-log )

Edit macros at the top of the script:

| Macro | Default | Meaning |
|-------|---------|---------|
| `FIBOCOM_LOG_SIZE_MB` | `50` | Max single file size in MB (`-m`) |
| `FIBOCOM_LOG_FILE_NUM` | `1` | Max rotated file count (`-n`); when `1`, stop at `-m` limit without deleting |
| `FIBOCOM_LOG_DIR` | `/tmp/file` | Capture log directory only |
| `FIBOCOM_LOG_PID_DIR` | `/tmp` | Pid file directory (`fibocom-log-lte.pid` …) |
| `FIBOCOM_LOG_FILTER` | `qxdm_default.cfg` | Qualcomm path only (`-f`); not passed for UNISOC FM650 |
| `FIBOCOM_LOG_FLAT` | `1` | On `stop`, move latest `fibolog_*/*.qmdl2` to `FIBOCOM_LOG_DIR` and remove subdirs |
| `FIBOCOM_LOG_BASENAME` | `""` | Flat output name when `FIBOCOM_LOG_FLAT=1`; empty uses `lte` / `lte2` |
| `FIBOCOM_LOG_AT_BAUD` | `115200` | Baud for `tip` AT oneshot |
| `FIBOCOM_LOG_AT_PREP_FM650` | `AT+GTLOGEN=1` | Prep AT when `status:name` matches FM650/FG650/FG652 |
| `FIBOCOM_LOG_AT_PREP_FM160` | `AT+GTTESTDIAG=1` | Prep AT when `status:name` matches FM160 |


### API Reference

#### -

##### stop

- Description: Stop all background `fibocom-log` processes started by this component.
- Returns: `ttrue`

#### +

##### lte / lte2

- Description: Capture on `modem@lte` / `modem@lte2`. Port from `modem@….tty:dtty`; AT on `….tty:stty`. Reads `modem@….status:name` before shut to pick diag AT. Sequence: `service.stop[ network@connect ]` → `ifname@….shut` → `modem@….shut` → diag-enable AT → `AT+CFUN=0` → start fibocom-log → `AT+CFUN=1` → wait 2s → `modem@….setup` → `ifname@….setup`. Re-call replaces the previous capture.
- Returns: `ttrue` / `tfalse`

```shell
$ script@fibocom-log.lte
ttrue
$ script@fibocom-log.lte2
ttrue
$ script@fibocom-log.stop
ttrue
```
