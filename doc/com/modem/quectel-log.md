## script@quectel-log — Quectel Modem Log Capture

### Overview

Shell component (flat `exe` script under `project/modem/quectel-log`) that starts
background **quectel-log** (QLog) from the `modem-log` FPK on `lte` / `lte2`.

Before capture it stops `network@connect`, shuts `ifname@lte` / `modem@lte` (or `lte2`),
sends `AT+CFUN=0` on the AT port (`stty`), starts QLog on `dtty`, then sends `AT+CFUN=1`.

Capture files go under **`QUECTEL_LOG_DIR`** (default `/tmp/file`, usable via `wui@file`).
Requires the `modem-log` package and `tip` (modem cmd).


### Configuration reference ( script@quectel-log )

Edit macros at the top of the script:

| Macro | Default | Meaning |
|-------|---------|---------|
| `QUECTEL_LOG_SIZE_MB` | `20` | Max single file size in MB (`-m`) |
| `QUECTEL_LOG_FILE_NUM` | `10` | Max rotated file count (`-n`) |
| `QUECTEL_LOG_DIR` | `/tmp/file` | Capture log directory only |
| `QUECTEL_LOG_PID_DIR` | `/tmp` | Pid file directory (`quectel-log-lte.pid` …) |
| `QUECTEL_LOG_FILTER` | `default.config` | `-f` filter; relative names resolve under `land@fpk.path[ modem-log ]` |
| `QUECTEL_LOG_AT_BAUD` | `115200` | Baud for `tip` AT+CFUN oneshot |


### API Reference

#### -

##### stop

- Description: Stop all background `quectel-log` processes started by this component.
- Returns: `ttrue`

#### +

##### lte / lte2

- Description: Capture on `modem@lte` / `modem@lte2`. Port from `modem@….tty:dtty`; AT on `….tty:stty`. Sequence: `service.stop[ network@connect ]` → `ifname@….shut` → `modem@….shut` → `AT+CFUN=0` → start QLog → `AT+CFUN=1`. Re-call replaces the previous capture.
- Returns: `ttrue` / `tfalse`

```shell
$ script@quectel-log.lte
ttrue
$ script@quectel-log.lte2
ttrue
$ script@quectel-log.stop
ttrue
```
