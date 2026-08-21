## script@fibocom-log — Fibocom Modem Log Capture

### Overview

Shell component (flat `exe` script under `project/modem/fibocom-log`) that starts
background **fibocom-log** from the `modem-log` FPK on `lte` / `lte2`.

Before capture it stops `network@connect`, shuts `ifname@lte` / `modem@lte` (or `lte2`),
sends `AT+CFUN=0` on the AT port (`stty`), starts fibocom-log on `dtty`, then sends `AT+CFUN=1`.

Capture files go under **`FIBOCOM_LOG_DIR`** (default `/tmp/file`, usable via `wui@file`).
Requires the `modem-log` package and `tip` (modem cmd).


### Configuration reference ( script@fibocom-log )

Edit macros at the top of the script:

| Macro | Default | Meaning |
|-------|---------|---------|
| `FIBOCOM_LOG_SIZE_MB` | `20` | Max single file size in MB (`-m`) |
| `FIBOCOM_LOG_FILE_NUM` | `3` | Max rotated file count (`-n`) |
| `FIBOCOM_LOG_DIR` | `/tmp/file` | Capture log directory only |
| `FIBOCOM_LOG_PID_DIR` | `/tmp` | Pid file directory (`fibocom-log-lte.pid` …) |
| `FIBOCOM_LOG_FILTER` | `qxdm_default.cfg` | `-f` filter; relative names resolve under `land@fpk.path[ modem-log ]` |
| `FIBOCOM_LOG_AT_BAUD` | `115200` | Baud for `tip` AT+CFUN oneshot |


### API Reference

#### -

##### stop

- Description: Stop all background `fibocom-log` processes started by this component.
- Returns: `ttrue`

#### +

##### lte / lte2

- Description: Capture on `modem@lte` / `modem@lte2`. Port from `modem@….tty:dtty`; AT on `….tty:stty`. Sequence: `service.stop[ network@connect ]` → `ifname@….shut` → `modem@….shut` → `AT+CFUN=0` → start fibocom-log → `AT+CFUN=1`. Re-call replaces the previous capture.
- Returns: `ttrue` / `tfalse`

```shell
$ script@fibocom-log.lte
ttrue
$ script@fibocom-log.lte2
ttrue
$ script@fibocom-log.stop
ttrue
```
