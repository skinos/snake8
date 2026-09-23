## center@log — Remote log collector

### Overview

TCP server that stores gateway syslog lines under the heport device tree.

- Default listen port **20004**. Optional SSL on the listen socket (`ssl` = `enable` for SSL encryption, `disable` for plain TCP; default `disable`). Device uplink SSL must match
- A device directory must already exist (`{device_path}/<user>/dev/<macid>`). Unknown user/macid closes the connection. This service does not create device directories
- First line on a connection is `{12-hex-macid};{username}` and a newline. On success the server drops any older live session for the same user+macid, then opens that device's current log file and keeps the fd for the life of the socket. Later lines are log text ending in newline. Newlines inside a line are already tabs when they arrive; they are stored as received
- Files are `{device_path}/<user>/dev/<macid>/log/YYYYMMDD-HHMMSS.log`. A line is never split across files. When the next line would pass `size` (kilobytes), a new timestamp file is opened first. Oldest timestamp names are removed until the count is within `max`
- Global caps are `size` (default 1024) and `max` (default 10). A device `config` may override them with `log_file_size` and `log_file_max`
- Read timeouts: `auth` (default 15) seconds after connect with no identity line; `idle` (default 3600) seconds with no data after identity. Set either to `0` to disable that timeout. Bad identity (format / unknown user+macid / open fail) closes the socket immediately
- `config` key `log` = `disable` does not stop this listener. `center@heport` uses it, together with this service's `status` (or `log.cfg` while register is empty), to push `agent@logc` `center` / `center_port` / `center_ssl` so the device turns the uplink on or off
- List, path, and delete are `center@api` (`log_list`, `log_path`, `log_delete`), not methods on this object
- Register fields are published before `status`. Empty `status` means the service is not ready yet; heport then falls back to `log.cfg` (same as nport/pport). `disable` means the uplink must stay off. `enable` means the listener is up
- On device connect, heport pushes `agent@logc` keys `center` / `center_port` / `center_ssl` (not logc `status` / local file settings)



### Dependencies

- Requires `center@heport` so `device_path` is published before the listener starts
- Device uplink is expected from `agent@logc` (TCP client with optional SSL and offline spool)
- Per-device enable and file caps live in `{device_path}/<user>/dev/<macid>/config` (see `userdir/dev/00037f120000/config.md`)



### Configuration reference ( center@log )

```json
// Attributes introduction 
{
    "status":"enable the log collector",     // [ "disable", "enable" ], default be "enable" when unset
    "port":"TCP listen port",                // [ number ], default be 20004
    "ssl":"SSL encrypt the listen socket",   // [ "disable", "enable" ], default be "disable"
                                                // "enable": SSL encryption; "disable": plain TCP
    "size":"max file size",                  // [ number ], kilobytes, default be 1024
    "max":"max file count per device",       // [ number ], default be 10
    "auth":"wait for identity after connect", // [ number ], seconds, default be 15; 0 disables
    "idle":"no data after identity"          // [ number ], seconds, default be 3600; 0 disables
}
```

#### Configuration example

Example, show all the configure

```shell
center@log
{
    "status":"enable",                    # log collector enabled
    "port":"20004",                       # gateway TCP uplink port
    "ssl":"disable",                      # plain TCP; set enable for SSL encryption
    "size":"1024",                        # rotate after this many kilobytes
    "max":"10",                           # keep at most this many files per device
    "auth":"15",                          # drop if no identity within N seconds
    "idle":"3600"                         # drop if no data for N seconds after identity
}
```

#### Configuration settings example

Example, disable the log collector

```shell
center@log:status=disable
ttrue
```

Example, change the listen port

```shell
center@log:port=20004
ttrue
```

Example, merge set file caps( include "size" "max" )

```shell
center@log|{"size":"1024","max":"10"}
ttrue
```



### Concepts

**Wire protocol (device → center)**

| Phase | Line shape | Notes |
|-------|------------|-------|
| First line | `{12-hex-macid};{username}\n` | Ensures one live session per user+macid, then binds a resident append fd under that device's log dir |
| Later lines | `{text}\n` | Written on the bound fd; `\n` / `\r` inside text are tabs before send; stored as received |

Reconnect sends the identity line again (kicks the prior uplink if still up), then continues with text lines.

**On-disk layout**

| Path | Role |
|------|------|
| `{device_path}/<user>/dev/<macid>/log/` | Directory created when identity is accepted |
| `YYYYMMDD-HHMMSS.log` | Lexicographic order matches time; identity reuses newest while under size, else creates a new name |

**Register snapshot (for heport adjust)**

| Key | Meaning |
|-----|---------|
| `status` | Empty = not ready; `disable` = force uplink off; `enable` = listener up |
| `port` | Listen port string pushed as `agent@logc.center_port` |
| `ssl` | `enable` / `disable` pushed as `agent@logc.center_ssl` |
| `size` / `max` | Live global caps (ints) |
| `auth` / `idle` | Live read timeouts in seconds (ints; 0 = off) |



### API Reference

#### Management APIs

+ `setup[]` **start the log collector when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - When `status` is `disable`, publishes `status` = `disable` and returns ttrue without listening
    - Lifecycle method; also used after configuration changes


+ `shut[]` **stop the log collector**
    - failed return tfalse
    - succeed return ttrue



### Other

- Admin page `log.html` is registered under `prj.json` `wui` (Cloud / Remote Log) for `wui@admin`
- File list / download path / delete: `center@api.log_list`, `center@api.log_path`, `center@api.log_delete` (see `api.md`)
- Per-device switch and caps: `userdir/dev/00037f120000/config.md` (`log`, `log_file_size`, `log_file_max`)
- Stored files: `userdir/dev/00037f120000/log.md`
- userwui Device Log table on the gateway detail page lists names only; download uses httpd `/download` with `api=log_path`
