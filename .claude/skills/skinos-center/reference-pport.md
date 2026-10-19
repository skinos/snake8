# Port penetrate — maps (`tcpmap`/`udpmap`) and temporary `gateway_*`

Sources: `config/slave/center/api.md` (Port map), `pport.md`, `userdir/tcpmap.md` / `udpmap.md`.

Runtime: **`center@pport`**. Device side: **`agent@portc`** pond to pport **:20005**.

## Port ranges

| Range | Default | Use |
|-------|---------|-----|
| Dynamic | 20006–24999 | `gateway_*`, ttyd, mesh `relay_map` UDP |
| Static | ≥25000 | Persisted user `tcpmap` / `udpmap` |

Idle mated sessions typically `mate_timeout=180`. Ephemeral maps often `lock=1`.

## Persistent TCP / UDP maps

Stored under `{device_path}/<user>/tcpmap` and `udpmap`; applied via `pport.tcp_map` / `udp_map`; reloaded on pport start.

```bash
he 'center@api.tcpmap_add[ ashyelf,00037f123450,192.168.8.1,80 ]'
he 'center@api.tcpmap_list[ ashyelf ]'
he 'center@api.tcpmap_delete[ ashyelf,00037f123450,25002 ]'

he 'center@api.udpmap_add[ ashyelf,00037f123450,192.168.8.22,5000 ]'
he 'center@api.udpmap_list[ ashyelf ]'
he 'center@api.udpmap_delete[ ashyelf,00037f123450 ]'
```

- `tcpmap_add` / `udpmap_add`: user, mac, LAN ip, port; optional protocol / lock
- Delete: by mac and/or public port (see `api.md` examples — list first to see allocated public port)
- Fail → `tfalse` / NULL

**Not** the same as mesh `relay_map` (nport-owned UDP; not written to `udpmap`).

## Temporary remote access (no durable tcpmap write)

Returns a **public TCP port** string (browser or client → center host:port).

```bash
he 'center@api.gateway_http[ ashyelf,00037f123450 ]'      # device WUI (wui_port or 80)
he 'center@api.gateway_telnet[ ashyelf,00037f123450 ]'    # telnet + ttyd-style browser term
he 'center@api.gateway_ssh[ ashyelf,00037f123450 ]'
he 'center@api.gateway_hline[ ashyelf,00037f123450 ]'     # ttyd + hline <mac> (HE terminal)
```

Optional `[ip],[port]` on http/telnet/ssh: LAN target behind the gateway; omit = gateway itself / default service port.

Example:

```bash
he 'center@api.gateway_http[ ashyelf,00037f123450 ]'
# → e.g. 20007
# open http://<center-host>:20007  → device WUI
```

Mac must exist under `{device_path}/<user>/dev/<macid>`. Device needs portc pond up. Fail → NULL.

## After penetrate

| Goal | Next skill |
|------|------------|
| Browse / config / diagnose on WUI | browser or curl on returned port |
| HE in terminal | **skinos-he** (via telnet/hline session) |
| Upload `.zz` / FPK to device URL | **device-upgrade** — **model match first** (same HARD RULE) |

## Low-level pport (ops; prefer api wrappers)

```bash
he 'center@pport.list'
he 'center@pport.tcp_map[ ,00037f123450,127.0.0.1,80,tcp,0,0 ]'
he 'center@pport.dynamic_port'
he 'center@pport.relay_list'    # mesh only; do not confuse with udpmap
```

Operators managing cloud users should stick to `center@api.*map_*` and `gateway_*`.

## Prerequisites

- `center@pport` running; device `agent@portc` connected
- Online gateway for reliable mating
- Dynamic pool not exhausted
