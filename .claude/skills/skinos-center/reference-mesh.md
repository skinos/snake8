# Mesh — `center@api` network / endpoint + `center@nport`

Sources: `config/slave/center/api.md` (Mesh), `nport.md`, `userdir/net/mynet.md`.

Durable: `{device_path}/<user>/net/<netid>`. Runtime UDP/push: **`center@nport`**. Operators use **api** knock/dump, not nport unix internals.

## Semantics (important)

| Word | Meaning |
|------|---------|
| **master** | Not a separate API. Higher **`pref`** among reachable hubs wins election |
| **relay** | Wish for a **center public UDP** so peers can reach a NAT hub: `auto` / `enable` / `disable` |
| Default new member | **`relay=disable`** (omit / empty = disable) |
| When to enable relay | **Only** for the mesh master that has no public-address hub. Ordinary endpoints stay `disable` |
| `relay_port` | Live borrowed UDP (dump/push only while online). Not stored in net file; not a user `udpmap` |
| `relay_max` | Per-user cap on live listens (`center@ctrl` / account `config`). Over cap → stays leaf |

`auto`: open center UDP when online and behind NAT. `enable`: always. `disable`: never.

Reserved netids (rejected): `gtog`, `cmd`, `net`, `agent`, `local`, `portc`, `heclient`.

## Playbook: ashyelf / device xxx / net `ddd` / xxx relay+master

```bash
# 1. Device present
he 'center@api.list[ ashyelf ]'

# 2. Create network if missing (default CIDR)
he 'center@api.network_list[ ashyelf ]'
he 'center@api.network_add[ ashyelf, ddd, 172.16.0.0/24 ]'

# 3. Add xxx as hub: high pref + relay enable; point auto if omitted
he 'center@api.endpoint_add[ ashyelf, ddd, 00037f123450, , , 100, , , , enable ]'
# optional: explicit point + listen_port
# he 'center@api.endpoint_add[ ashyelf, ddd, 00037f123450, 172.16.0.1, 192.168.8.0/24, 100, , , 10005, enable ]'

# 4. Peer with lower pref (relay stays disable)
he 'center@api.endpoint_add[ ashyelf, ddd, 00037f120001, , 192.168.9.0/24, 0 ]'

# 5. Cap check if relay fails to appear
he 'center@ctrl.user_list[ ashyelf ]'          # relay_max
# he 'center@ctrl.user_modify[ ashyelf,,,,2 ]'

# 6. Converge + verify
he 'center@api.network_knock[ ashyelf, ddd ]'
he 'center@api.network_dump[ ashyelf, ddd ]'
he 'center@api.endpoint_dump[ ashyelf, ddd, 00037f123450 ]'
```

CIDR if user omitted: default **`172.16.0.0/24`**, or ask once.

To change `relay` later: edit net file or delete+add endpoint, then `endpoint_knock`.

## Network CRUD

```bash
he 'center@api.network_add[ ashyelf, ddd, 172.16.0.0/24 ]'
he 'center@api.network_modify[ ashyelf, ddd, 10.8.0.0/24 ]'
he 'center@api.network_list[ ashyelf ]'
he 'center@api.network_knock[ ashyelf, ddd ]'
he 'center@api.network_delete[ ashyelf, ddd ]'   # returns member relay UDP
```

Create-only for add (`EEXIST` if netid taken). Modify does not reallocate endpoint `point`s.

## Endpoint CRUD

```bash
he 'center@api.endpoint_add[ ashyelf, ddd, mac, [point], [extend], [pref], [ip], [port], [listen_port], [relay] ]'
he 'center@api.endpoint_list[ ashyelf, ddd ]'
he 'center@api.endpoint_knock[ ashyelf, ddd, mac ]'
he 'center@api.endpoint_delete[ ashyelf, ddd, mac ]'
```

- `point` omitted → next free host in network CIDR
- `extend` = LAN CIDRs behind gateway
- Static `ip`/`port` = hole overrides (not the borrowed relay port)

## Dump (live)

`network_dump` / `endpoint_dump` merge durable topology with live `ip`/`port`/`pubkey`/`nattype`/`online`/`relay_port`.

## Device / service deps

- Device: `agent@heclient` + `agent@gtog` (+ net*)
- Center: heport (hh push) + nport (UDP) + pport (`relay_map`)
- nport config only: `status` / `port` / `nettest_port` / `timeout` + `setup`/`shut` — mesh CRUD stays on **api**
