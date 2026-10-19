# Gateway lifecycle — `center@api`

Source: `config/slave/center/api.md` (Gateway). Device files under `{device_path}/<user>/dev/<macid>/` (`userdir/dev/…`).

## List

```bash
he 'center@api.list[ ashyelf ]'
he 'center@api.list[ ashyelf,1,20 ]'           # page, size
he 'center@api.list[ ashyelf,,,r600 ]'         # filter by machine.status.custom
```

- Fail → NULL; success → JSON keyed by macid
- **Online**: `online` present (`HH:MM:SS:Day`). Offline: no `online`
- Useful fields: `version`, `macid`, `model`, `cmodel`, `wui_port` / `telnet_port` / `ssh_port`, `local_ip`, WAN/`ifname` stats
- List **filters** by `land@machine.status` `custom` but **strips** `hardware`/`custom` from the returned JSON — for model match use `hh` or `reg` (see firmware ref)

Example mac: `00037f123450` under user `ashyelf`:

```bash
he 'center@api.list[ ashyelf ]:00037f123450'
```

## Comment / knock / delete

```bash
he 'center@api.comment[ ashyelf,00037f123450, Lab DUT ]'
he 'center@api.knock[ ashyelf,00037f123450 ]'     # drop TLS session (device may reconnect)
he 'center@api.delete[ ashyelf,00037f123450 ]'    # maps + mesh membership + dev/<mac>/
```

Delete is best-effort on maps/mesh; missing records do not fail the call.

## Refresh / reboot / factory (via hh to device)

Device must exist under `dev/<macid>` (and usually be online).

```bash
he 'center@api.update[ ashyelf,00037f123450 ]'    # agent@heclient.update / refresh register
he 'center@api.reboot[ ashyelf,00037f123450 ]'    # land@machine.restart path
he 'center@api.default[ ashyelf,00037f123450 ]'   # factory default configure
```

Optional 4th arg: wait timeout seconds. Fail → `tfalse`.

## Find which user owns a mac

If user unknown: `center@ctrl.user_list` then `list` each user, or search under heport `device_path/*/dev/<mac>/`. Prefer asking for the cloud username when possible.

## Prerequisites

- Gateway registered under that username (`vcode` match if account has `vcode`)
- Online needed for update/reboot/default/hh; knock/delete work with caveats when offline
