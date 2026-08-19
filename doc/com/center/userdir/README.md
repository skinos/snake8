## userdir — Per-user data layout under center@heport

### Overview

`userdir/` documents the **on-disk files** that live under each username in the heport device database.

- Runtime root is `device_path` published by `center@heport` (`dbs_path` of that component)
- Absolute path shape: `{device_path}/<username>/…`
- Same tree is reachable as HE/dbs object path: `center@heport/<username>/…`
- Business HE APIs that create or mutate these files are documented in `center@ctrl` (`ctrl.md`) for admin account ops and `center@api` (`api.md`) for cloud self-service / gateway / map / mesh / firmware
- Sample tree under this directory uses placeholder names (`mynet`, `00037f120000`); replace with the real username / netid / macid


### Concepts

Directory layout created for each user:

```
{device_path}/<username>/
  config                         # account settings (key, vcode, lang, comment)
  tcpmap                         # persisted TCP port-forward rules
  udpmap                         # persisted UDP port-forward rules
  dev/
    <macid>/
      reg                        # last register / 0+r snapshot from the gateway
      config                     # operator settings for this gateway (comment, portc, …)
      heartbeat/                 # planned sqlite store (not written yet)
  net/
    <netid>                      # durable mesh topology (point/extend/pref); runtime in center@nport
  firmware/
    <name>.zz                    # firmware images for OTA push
```

Document map:

| Path under `<username>/` | Doc file |
|--------------------------|----------|
| `config` | [`config.md`](config.md) |
| `tcpmap` | [`tcpmap.md`](tcpmap.md) |
| `udpmap` | [`udpmap.md`](udpmap.md) |
| `net/<netid>` | [`net/mynet.md`](net/mynet.md) (`mynet` is an example netid) |
| `dev/<macid>/reg` | [`dev/00037f120000/reg.md`](dev/00037f120000/reg.md) |
| `dev/<macid>/config` | [`dev/00037f120000/config.md`](dev/00037f120000/config.md) |
| `dev/<macid>/heartbeat/` | [`dev/00037f120000/heartbeat.md`](dev/00037f120000/heartbeat.md) |
| `firmware/*.zz` | [`firmware.md`](firmware.md) |



### Other

- Deleting a user removes the whole `{device_path}/<username>/` tree
- Online SSL sessions and talk forward state stay in `center@heport` memory; they are not files under this tree
- Mesh UDP coordinator is `center@nport`; `net/<netid>` files are written by `center@api` and loaded/knocked by nport
- One network `seq` versions topology; hole ip/port/pubkey stay in nport memory (see `net/mynet.md`, `nport.md`)
