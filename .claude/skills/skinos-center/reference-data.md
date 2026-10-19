# Data layout, dumps, ports, dependencies

Sources: `config/slave/center/userdir/README.md`, `ctrl.md`, service `*.md`.

## Per-user tree

Root: `device_path` from `center@heport` (`reg` / dbs). Also `center@heport/<user>/…`.

```text
{device_path}/<username>/
  config                 # account (key, vcode, lang, comment, relay_max)
  tcpmap                 # durable TCP maps
  udpmap                 # durable UDP maps
  dev/<macid>/
    reg                  # last register / 0+r snapshot (includes land@machine.status)
    config               # operator comment, portc, …
    heartbeat/           # planned; not written yet
  net/<netid>            # durable mesh topology
  firmware/*.zz          # OTA library
```

Delete user (`center@ctrl.user_delete`) removes the whole tree after cascading gateways/networks/maps.

Online TLS sessions live in **heport memory**, not as files.

## Useful reads

```bash
he 'center@heport'                              # port / api_port / …
he 'center@heport/ashyelf/config'
he 'center@heport/ashyelf/dev/00037f123450/reg'
he 'center@ctrl.dump[ ashyelf,00037f123450 ]'   # online memory dump
he 'center@api.network_dump[ ashyelf, ddd ]'
```

## Default listen ports

| Component | Ports |
|-----------|-------|
| userwui | TCP 20000 (HTTP), optional ssl/term |
| heport | TCP 20002 (device SSL), 20003 (HTTP api) |
| nport | UDP 20002 / 20003 |
| pport | TCP 20005; dynamic 20006–24999; static ≥25000 |

## Dependency graph

```text
center@heport (device_path, TLS, hh unix, /hh)
  ├── center@ctrl (accounts, auth, dump)
  ├── center@api (business HE)
  │     ├── pport maps / gateway_* / firmware_push(hh)
  │     └── nport knock after mesh CRUD
  ├── center@pport (pond + maps + mesh relay_map)
  ├── center@nport (UDP mesh + hh push via heport)
  └── center@userwui (auth→ctrl.user_match, he→api)

Device: agent@heclient, agent@portc, agent@gtog
```

Lifecycle: package `init`/`uninit` → `app` typically `setup`/`shut` for heport, pport, userwui (and nport). Ops change `status` / ports then `setup` if needed — not the usual per-device management path.

## Cascades

| Action | Side effects |
|--------|----------------|
| `api.delete` gateway | tcp/udp maps for mac, mesh endpoints, knock, remove `dev/<mac>/` |
| `ctrl.user_delete` | all gateways delete, networks, maps, remove user tree |
| `network_delete` | member relay UDP returned |
| `endpoint_delete` | membership + relay UDP + nport knock peers |

## Boundaries

| Need | Skill |
|------|-------|
| Cloud manage / hh / penetrate / mesh / cloud OTA | **skinos-center** |
| HE grammar on device | **skinos-he** |
| Direct device web upload | **device-upgrade** |
| Build/run center on slave | **skinos-slave** |
