## nmeatest — GPS NMEA PTY simulator

### Overview

Host/device shell tool that creates a Linux pseudo-TTY and writes periodic NMEA cycles (GGA / RMC / GSA / GSV) so you can test `gnssdrv@nmea` without real GNSS hardware.
- opens a PTY master/slave pair and optionally symlinks the slave to a stable path
- on start: calls `gnss@frame.register[ nmeatest, <path>, gnssdrv@nmea, gnss@nmea ]`
- every interval (default 500 ms) emits one fix cycle with a slowly moving position
- on stop (Ctrl+C / SIGTERM): calls `gnss@frame.unregister[ nmeatest ]`



### Concepts

Treat the simulator like a hotplugged GNSS TTY: keep `nmeatest` running and it binds `gnss@nmea` for you. The `devcom` string `nmeatest` is only a bind key for later `unregister` (there is no real `usb@tty-*` object). If nothing has the slave open yet, PTY writes may return `EIO` and the tool retries.



### Other

#### Shell usage

```shell
nmeatest [symlink_path] [interval_ms]
```

- `symlink_path` — default `/tmp/gnss-tty`; created as a symlink to the PTY slave; also passed to `register` as `ttydev`
- `interval_ms` — cycle period, default `500`, minimum `100`

Example test flow
```shell
nmeatest /tmp/gnss-tty 500
# auto: gnss@frame.register[ nmeatest, /tmp/gnss-tty, gnssdrv@nmea, gnss@nmea ]
gnss@frame.list
gnssdrv@nmea.status[ gnss@nmea ]
# Ctrl+C stops and auto: gnss@frame.unregister[ nmeatest ]
```
