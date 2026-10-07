## ifname@lan — Local/LAN Network Management

### Overview

Manage local (LAN) networks. This component depends on a local network interface or switch (SoC), typically via **`arch`** (`ethernet`, bridge/VLAN wiring), and the **network** project (`network@frame` registration — see [`../network/frame.md`](../network/frame.md)).
Usually `ifname@lan` is the first local network. If there are multiple local networks, `ifname@lan2` is the second local network, and numbering increases sequentially.

- manages LAN interface lifecycle: setup, shutdown, status query
- supports static IPv4 and follow/static/ULA IPv6 addressing (`mode6` selects prefix source; `client@dhcps` `_odhcpd` installs follow/ULA/PD and advertises via odhcpd)
- when the kernel has no IPv6 support, reading this ifname’s config omits IPv6 keys (`mode6`, `static6`, `ula`, `pdid`, `pdlen`, `dhcps6`, …); IPv6 runtime stays off
- provides DHCPv4 server (`dhcps`) and LAN IPv6 host advertisement via odhcpd (`dhcps6`: slaac / hybrid / dhcp)
- no NAT masquerade (it IS the LAN, not NATted)



### Network Architecture

`ifname@lan` is a **local interface** registered by `network@frame` during boot. It uses `ifname@ethcon` as concom and `bridge@lan` as ifdev. Unlike extern interfaces, it does NOT apply NAT masquerade and is NOT subject to multi-uplink scheduling. It can run a DHCP server to assign IPs to local clients. When the LAN comes up or down, it publishes `network/on` / `network/off` joint events.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### Configuration reference ( ifname@lan )

```json
// Attributes introduction 
{
    "status":"start at system startup",                          // [ "enable", "disable" ], enable means auto-setup after boot

    // IPv4
    "mode":"IPV4 address mode",                                  // [ "dhcpc", "static" ]
                                                                      // "dhcpc" for DHCP client mode
                                                                      // "static" for manual IPv4 setting
    "static":                                 // detail configuration for "mode" is "static"
    {
        "ip":"IPv4 address",                        // < ipv4 address >
        "mask":"IPv4 netmask",                      // < ipv4 netmask >
        "ip2":"IPv4 address 2",                     // < ipv4 address >
        "mask2":"IPv4 netmask 2",                   // < ipv4 netmask >
        "ip3":"IPv4 address 3",                     // < ipv4 address >
        "mask3":"IPv4 netmask 3",                   // < ipv4 netmask >
        "gw":"IPv4 gateway",                        // [ ipv4 address ]
        "dns":"IPv4 DNS",                           // [ ipv4 address ]
        "dns2":"IPv4 DNS"                           // [ ipv4 address ]
    },
    "dhcpc":                                  // detail configuration for "mode" is "dhcpc"
    {
        "static":"Set an IP address before obtaining IP via DHCP", // [ "disable", "enable" ], temporary fallback address
        "routeopt":"dhcp option static route",                     // [ "disable", "enable" ], accept classless static routes
        "custom_dns":"Custom DNS",                                 // [ "disable", "enable" ]
        "dns":"Custom DNS1",                                       // [ ip address ], valid when "custom_dns" is "enable"
        "dns2":"Custom DNS2"                                       // [ ip address ], valid when "custom_dns" is "enable"
    },
    "dhcps":                                               // detail configuration for DHCP server settings
    {
        "status":"Whether to start the DHCP service",                      // [ "disable", "enable" ]
        "startip":"The start address within the IPv4 allocation pool",     // [ ipv4 address ]
        "endip":"IPv4 assigns the end address within the pool",            // [ ipv4 address ]
        "mask":"IPv4 assigns a subnet mask within a pool",                 // [ ipv4 netmask ]
        "lease":"lease time for assigned addresses",                       // [ number ], unit is seconds
        "gw":"Specifies the IPv4 gateway",                                 // [ ipv4 address ], default is local network IP address
        "dns":"Specifies the IPv4 DNS",                                    // [ ipv4 address ], default is local network IP address
        "dns2":"Specifies the IPv4 backup dns",                            // [ ipv4 address ]
        "options":"dnsmasq original options"                               // [ string ], multiple options are separated by semicolons
    },

    // IPv6 — two layers:
    //   1) mode6 / static6 / pdid / pdlen / ula  → put a prefix (and this LAN host address) on the LAN netdev
    //   2) dhcps6                                 → how odhcpd tells hosts about that prefix (RA / DHCPv6)
    // Hosts never get a GUA/ULA from dhcps6 alone: the prefix must already be on the interface.
    // Naming: addr = this LAN host address (/64 on the netdev); prefix = CIDR in status (owned slice);
    //         pdlen/pdid = how to carve uplink IA_PD (not the on-link /64 mask); pd = downstream IA_PD on/off

    "mode6":"IPv6 prefix source for this LAN", // [ "disable", "static6", "follow", "ifname@wan", "ifname@lte", "ifname@lte2", "ifname@wisp", "..." ]
                                                    // default "follow"; missing / empty treated as "disable" until factory sets it
                                                    // legacy "auto" is mapped to "follow" by client@dhcps (WAN "auto" is unrelated)
                                                    // "disable" — no IPv6 on this LAN (no follow, no ULA, no odhcpd stanza)
                                                    // "static6" — use static6{}; this LAN owns that prefix (no uplink follow)
                                                    // "follow" — best extern (PD preferred, else relay-capable IPv6, else ULA-only); pick in client@dhcps
                                                    // "ifname@..." — pin that extern only (PD → server; IPv6 no PD → relay candidate)
    "static6":                                // detail when "mode6" is "static6"
    {
        "addr":"LAN IPv6 address",                  // < ipv6 address >, typically ::1
        "mask":"prefix length of addr",             // [ number ], default 64; applied as addr/mask on the LAN netdev
        "dns":"IPv6 DNS",                           // [ ipv6 address ], optional hint for this LAN; RA DNS still follows dhcps6.dns_mode
        "dns2":"IPv6 DNS2"                          // [ ipv6 address ]
    },
    "pdid":"PD block index",                  // [ "auto", "0", "1", "2", "..." ], default "auto" (LAN ordinal)
                                                    // only when following and the uplink has a delegated prefix (IA_PD)
                                                    // which block to take from the uplink PD (OpenWrt ip6hint)
    "pdlen":"PD slice length for this LAN",   // [ number ], default 64
                                                    // only when following and the uplink has a delegated prefix
                                                    // size of the block carved from uplink IA_PD (OpenWrt ip6assign); not the on-link host mask
                                                    // hosts still SLAAC on a /64 PIO; if pdlen < 64, put ::1/64 on the LAN for RA and keep the rest for nested PD
    "ula":"automatic ULA /64",                // [ "enable", "disable" ], default "enable"
                                                    // stable per-LAN fd00::/8 /64; may coexist with GUA from follow/static6
                                                    // when uplink has no IPv6 and no PD, ULA (or static6) is the only way LAN hosts get non-link-local IPv6
    "dhcps6":                                 // how hosts are configured; does NOT create the LAN prefix
    {
        "mode":"how hosts get addresses",            // [ "disable", "slaac", "hybrid", "dhcp" ], default "slaac"
                                                          // applies only when this LAN is odhcpd server (own prefix: PD slice / ULA / static6)
                                                          // on relay path (uplink has IPv6 but no IA_PD), odhcpd relays RA/NDP/DHCPv6; these flags are ignored
                                                          // "slaac" — Unmanaged: A=1 M=0 O=0, hosts use RA only for addresses
                                                          // "hybrid" — Assisted: A=1 M=0 O=1, SLAAC address + DHCPv6 for DNS/other
                                                          // "dhcp" — Managed: A=0 M=1, hosts get IA_NA from the LAN /64 (odhcpd hostid; no start/end pool)
                                                          // "disable" — do not advertise (server: no RA/DHCPv6; relay: do not proxy this LAN)
        "pd":"downstream IA_PD to nested routers",   // [ "auto", "disable" ], default "auto"
                                                          // independent of mode: slaac may still run dhcpv6=server only for IA_PD
                                                          // "auto" — after this LAN takes "pdlen", leftover uplink PD => offer; only /64 left, ULA-only, or relay => off
                                                          // "disable" — never offer IA_PD
        "dns_mode":"DNS in RA/DHCPv6",               // [ "auto", "custom", "disable" ], default "auto"
                                                          // "auto" — advertise this LAN IPv6 (or odhcpd default) when server; relay uses upstream DNS
                                                          // "custom" — use dns / dns2 below
                                                          // "disable" — do not advertise DNS
        "dns":"custom DNS1",                         // [ ipv6 address ], valid when dns_mode is "custom"
        "dns2":"custom DNS2",                        // [ ipv6 address ], valid when dns_mode is "custom"
        "lifetime":"RA router lifetime",             // [ number ], seconds, default 1800; server path only
        "leasetime":"DHCPv6 lease",                  // [ number ], seconds, default 86400; used when mode is dhcp (and hybrid when DHCPv6 is on)
    }
}
```

#### Configuration example

Example, show all configuration of the first local network (factory-shaped)
```shell
ifname@lan
{
    "mode":"static",
    "static":
    {
        "ip":"192.168.1.1",
        "mask":"255.255.255.0"
    },
    "dhcps":
    {
        "status":"enable",
        "startip":"192.168.1.2",
        "endip":"192.168.1.100",
        "mask":"255.255.255.0",
        "lease":"86400",
        "gw":"",
        "dns":""
    },
    "mode6":"follow",
    "pdid":"auto",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto",
        "dns_mode":"auto"
    }
}
```

Example, IPv4 static and DHCP server
```shell
ifname@lan
{
    "mode":"static",
    "static":
    {
        "ip":"192.168.1.1",
        "mask":"255.255.255.0"
    },
    "dhcps":
    {
        "status":"enable",
        "startip":"192.168.1.2",
        "endip":"192.168.1.100"
    }
}
```

Example, IPv4 DHCP client on LAN
```shell
ifname@lan
{
    "mode":"dhcpc",
    "dhcps":
    {
        "status":"disable"
    }
}
```

Example, IPv6 disable on this LAN
```shell
ifname@lan
{
    "mode6":"disable"
}
```

Example, IPv6 factory (minimal): follow uplink; omitted dhcps6 keys use defaults (slaac / pd=auto / dns_mode=auto)
```shell
ifname@lan
{
    "mode6":"follow"
}
```

Example, IPv6 factory fully written
```shell
ifname@lan
{
    "mode6":"follow",
    "pdid":"auto",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto",
        "dns_mode":"auto"
    }
}
```

Example, IPv6 follow WAN only
```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto"
    }
}
```

Example, IPv6 follow LTE only
```shell
ifname@lan
{
    "mode6":"ifname@lte",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto"
    }
}
```

Example, IPv6 follow WAN, pick the second /64 from a large PD (`pdid`)
```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"1",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto"
    }
}
```

Example, IPv6 follow, take a /60 slice from PD for this LAN (`pdlen=60`; hosts still need a /64 PIO on the LAN for SLAAC)
```shell
ifname@lan
{
    "mode6":"follow",
    "pdid":"auto",
    "pdlen":"60",
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"auto"
    }
}
```

Example, IPv6 host mode Unmanaged SLAAC (default; A=1 M=0 O=0)
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"slaac"
    }
}
```

Example, IPv6 host mode Assisted hybrid (SLAAC address + DHCPv6 DNS)
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"hybrid",
        "dns_mode":"auto"
    }
}
```

Example, IPv6 host mode Managed DHCPv6 (IA_NA from the LAN /64; no start/end pool keys)
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"dhcp",
        "leasetime":"86400"
    }
}
```

Example, IPv6 do not advertise to hosts on this LAN
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"disable"
    }
}
```

Example, IPv6 SLAAC hosts, never offer downstream IA_PD
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"disable"
    }
}
```

Example, IPv6 follow without automatic ULA
```shell
ifname@lan
{
    "mode6":"follow",
    "ula":"disable",
    "dhcps6":
    {
        "mode":"slaac"
    }
}
```

Example, IPv6 custom DNS in RA / DHCPv6
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"slaac",
        "dns_mode":"custom",
        "dns":"2001:4860:4860::8888",
        "dns2":"2606:4700:4700::1111"
    }
}
```

Example, IPv6 advertise no DNS
```shell
ifname@lan
{
    "mode6":"follow",
    "dhcps6":
    {
        "mode":"slaac",
        "dns_mode":"disable"
    }
}
```

Example, IPv6 static prefix (this LAN owns the prefix; uplink IPv6 optional)
```shell
ifname@lan
{
    "mode6":"static6",
    "static6":
    {
        "addr":"2001:db8:1::1"
    },
    "ula":"enable",
    "dhcps6":
    {
        "mode":"slaac",
        "pd":"disable"
    }
}
```

Example, IPv6 local-only ULA via static6 when you want a fixed fd prefix (uplink may have no IPv6)
```shell
ifname@lan
{
    "mode6":"static6",
    "static6":
    {
        "addr":"fd12:3456:789a::1"
    },
    "ula":"disable",
    "dhcps6":
    {
        "mode":"slaac"
    }
}
```

Example, IPv6 static prefix with Managed DHCPv6 for hosts
```shell
ifname@lan
{
    "mode6":"static6",
    "static6":
    {
        "addr":"fd00:1::1"
    },
    "dhcps6":
    {
        "mode":"dhcp"
    }
}
```

##### IPv6 by uplink case (mode6=follow or mode6=ifname@…, ula=enable unless noted)

Uplink case A — uplink has IPv6 but no IA_PD (SLAAC-only or IA_NA /128 + on-link /64, empty `prefix` in uplink status)

Effect: this LAN cannot own a separate GUA /64 from PD. Path is **odhcpd relay** (RA + NDP + DHCPv6, WAN `master=1`). `dhcps6.mode` / `pd` / `dns_mode` do **not** change upstream RA flags; hosts share the uplink on-link /64 if the ISP allows ND proxy.

```shell
# A1 — factory; same for slaac / hybrid / dhcp (relay ignores host mode)
ifname@lan
{
    "mode6":"follow",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
# LAN netdev: fe80:: + ULA ::1/64 when ula=enable (default); hosts: fe80:: + GUA from relayed RA (same /64 as WAN) when ISP allows
# status.prefix shows the ULA /64 when there is no owned PD/static6 prefix
# Internet IPv6: yes if ISP permits multiple addresses / ND proxy; else often broken beyond one CPE address

# A2 — do not proxy this LAN
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"disable" }
}
# hosts: no IPv6 advertisement via this LAN odhcpd stanza
```

Uplink case B — uplink IA_PD length is exactly /64 (`prefix` e.g. `2001:db8:1::/64`)

Effect: **server** path. Whole /64 is assigned to this LAN (`::1/64`). No leftover → `pd=auto` means **no** downstream IA_PD. `dhcps6.mode` applies.

```shell
# B1 — slaac (default): hosts get GUA by RA only; no DHCPv6 for host addresses
ifname@lan
{
    "mode6":"follow",
    "pdid":"auto",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}
# LAN: GUA ::1/64 (+ ULA); hosts: GUA SLAAC + fe80:: (+ ULA if advertised); nested routers: no IA_PD

# B2 — hybrid: SLAAC address + DHCPv6 DNS
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"hybrid", "pd":"auto", "dns_mode":"auto" }
}
# hosts: GUA via SLAAC; DNS via DHCPv6 (O=1)

# B3 — dhcp: Managed IA_NA from the LAN /64
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"dhcp", "pd":"disable", "leasetime":"86400" }
}
# hosts: GUA via DHCPv6 IA_NA (odhcpd hostid); not SLAAC for global addresses

# B4 — slaac but force no downstream PD (same as auto when PD is only /64)
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"slaac", "pd":"disable" }
}
```

Uplink case C — uplink IA_PD longer than /64 (e.g. /62, /56; `prefix` e.g. `2001:db8::/56`)

Effect: **server** path. Slice one block with `pdid` + `pdlen` onto the LAN. Leftover → `pd=auto` may offer IA_PD to nested routers. Hosts still need a /64 PIO on the LAN for SLAAC.

```shell
# C1 — slaac + downstream PD if leftover
ifname@lan
{
    "mode6":"follow",
    "pdid":"auto",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}
# LAN: one /64 from PD (+ ULA); hosts: GUA by RA only; nested routers: may get IA_PD from leftover

# C2 — second LAN takes pdid=1
ifname@lan2
{
    "mode6":"ifname@wan",
    "pdid":"1",
    "pdlen":"64",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}

# C3 — hybrid on a large PD
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"hybrid", "pd":"auto", "dns_mode":"auto" }
}
# hosts: SLAAC + DHCPv6 DNS; nested: IA_PD if leftover

# C4 — dhcp for hosts, still allow nested PD
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"dhcp", "pd":"auto" }
}
# hosts: IA_NA; nested: IA_PD if leftover

# C5 — large PD but never delegate downstream
ifname@lan
{
    "mode6":"follow",
    "dhcps6": { "mode":"slaac", "pd":"disable" }
}
```

##### Worked address plan (fixed numbers; every field concrete)

Use one invented topology for all rows below. Only `mode6` / `pdid` / `pdlen` / `ula` / `dhcps6` change; hardware IDs stay fixed.

**Fixed facts**

| Role | Value |
|------|--------|
| WAN netdev | `wan` |
| LAN bridge | `lan` |
| LAN2 bridge | `lan2` |
| Router LAN MAC | `02:50:f4:00:00:01` → link-local `fe80::50:f4ff:fe00:1` |
| PC on LAN MAC | `aa:bb:cc:dd:ee:ff` → IID `a8bb:ccff:fedd:eeff` (EUI-64; privacy addrs omitted) |
| Nested CPE behind LAN | wants IA_PD |
| Stable ULA for `ifname@lan` (when `ula=enable`) | `fd12:3456:789a::/64`, router `fd12:3456:789a::1/64` |
| WAN default IPv6 next-hop | `fe80::1` on `wan` |

**Uplink inventory used in the scenarios**

| Uplink scenario | WAN `addr` | WAN `prefix` (IA_PD) |
|-----------------|------------|----------------------|
| PD `/56` | `2001:db8:0:ff::1/128` | `2001:db8::/56` |
| PD `/64` | `2001:db8:0:ff::1/128` | `2001:db8:abcd::/64` |
| IPv6, no PD (SLAAC/IA_NA only) | `2001:db8:aaaa::5/64` (on-link) | *(empty)* |
| no IPv6 | *(none)* | *(empty)* |

How `/56` is indexed here: `2001:db8::/56` = 256 × `/64` blocks  
`2001:db8:0:00::/64`, `2001:db8:0:01::/64`, …, `2001:db8:0:0f::/64` (first `/60`), …, `2001:db8:0:ff::/64`.

---

**Case P56-M64-A0** — PD `/56`, `pdlen=64`, `pdid=0`, `dhcps6.mode=slaac`, `ula=enable`, `pd=auto`

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"0",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto", "lifetime":"1800" }
}
```

| Item | Concrete value |
|------|----------------|
| LAN owns (status `prefix`) | `2001:db8:0:0::/64` |
| Router on `lan` | `2001:db8:0:0::1/64`, `fd12:3456:789a::1/64`, `fe80::50:f4ff:fe00:1/64` |
| PC GUA (SLAAC) | `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` |
| PC ULA (SLAAC) | `fd12:3456:789a:a8bb:ccff:fedd:eeff/64` |
| PC link-local | `fe80::a8bb:ccff:fedd:eeff/64` |
| Routes on router | `2001:db8:0:0::/64` dev `lan`; `fd12:3456:789a::/64` dev `lan`; `::/0` via `fe80::1` dev `wan`; unreachable/from `2001:db8::/56` (delegated) as installed by uplink |
| Nested IA_PD example | may offer e.g. `2001:db8:0:1::/64` (or larger leftover policy) — **not** `2001:db8:0:0::/64` |
| `lan2` free to take | `pdid=1` → `2001:db8:0:1::/64` |

**RA from odhcpd on `lan` (server, slaac)** — source `fe80::50:f4ff:fe00:1`:

```text
Router Lifetime: 1800
Flags: M=0 O=0
PIO #1: prefix=2001:db8:0:0::/64  L=1 A=1  (hosts form GUA)
PIO #2: prefix=fd12:3456:789a::/64  L=1 A=1  (hosts form ULA)
RDNSS: 2001:db8:0:0::1   (dns_mode=auto → this LAN)
Route Info: (optional) none required beyond default via this router
```

Hosts set: default via `fe80::50:f4ff:fe00:1` on LAN; on-link `2001:db8:0:0::/64` and `fd12:3456:789a::/64`.

---

**Case P56-M64-A1** — same PD `/56`, `pdlen=64`, `pdid=1` (second /64)

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"1",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| LAN owns | `2001:db8:0:1::/64` |
| Router on `lan` | `2001:db8:0:1::1/64`, `fd12:3456:789a::1/64`, `fe80::50:f4ff:fe00:1/64` |
| PC GUA | `2001:db8:0:1:a8bb:ccff:fedd:eeff/64` |
| PC ULA | `fd12:3456:789a:a8bb:ccff:fedd:eeff/64` (same ULA /64 as A0; ULA is per-LAN stable, not from PD) |
| Routes | `2001:db8:0:1::/64` + ULA /64 on `lan`; `::/0` via WAN |

**RA on `lan`:**

```text
Router Lifetime: 1800
M=0 O=0
PIO #1: 2001:db8:0:1::/64  L=1 A=1
PIO #2: fd12:3456:789a::/64  L=1 A=1
RDNSS: 2001:db8:0:1::1
```

Difference vs A0 (`pdid=0`): only the **GUA /64 nibble** (`:0:` → `:1:`). RA shape identical.

---

**Case P56-M60-A0** — PD `/56`, `pdlen=60`, `pdid=0`

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"0",
    "pdlen":"60",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| LAN owns (status `prefix`) | `2001:db8:0:0::/60` (= `2001:db8:0:0::/64` … `2001:db8:0:f::/64`) |
| Router on `lan` (on-link for SLAAC) | `2001:db8:0:0::1/64` (+ ULA + fe80) — **not** `…::1/60` as the RA PIO |
| PC GUA | still `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` (only first /64 advertised) |
| Nested IA_PD inside owned /60 | e.g. `2001:db8:0:1::/64` … `2001:db8:0:f::/64` |
| Still free outside /60 for `lan2` | e.g. `pdid` of next /60 → `2001:db8:0:10::/60` |
| Routes | connected `2001:db8:0:0::/64` on `lan`; owned `/60` reserved for PD policy; `::/0` via WAN |

**RA on `lan` (same PIO as M64 for hosts):**

```text
Router Lifetime: 1800
M=0 O=0
PIO #1: 2001:db8:0:0::/64  L=1 A=1     ← hosts only see /64, never /60 in PIO
PIO #2: fd12:3456:789a::/64  L=1 A=1
RDNSS: 2001:db8:0:0::1
```

What changed vs M64: **reservation / nested PD pool size** (`/60` vs `/64`), not the host address or RA PIO prefix length.

---

**Case P56-M56-A0** — PD `/56`, `pdlen=56` (whole PD to this LAN)

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"0",
    "pdlen":"56",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| LAN owns | `2001:db8::/56` |
| Router / PC GUA | same as M64-A0: router `2001:db8:0:0::1/64`, PC `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` |
| `lan2` from same PD | should not take another pdid (entire /56 reserved here) |
| Nested IA_PD | any leftover `/64` inside `/56` except on-link `2001:db8:0:0::/64`, e.g. `2001:db8:0:2::/64` |

**RA on `lan`:** identical host-visible RA to M64-A0 (PIO still `/64`).

```text
M=0 O=0
PIO: 2001:db8:0:0::/64  L=1 A=1
PIO: fd12:3456:789a::/64  L=1 A=1
RDNSS: 2001:db8:0:0::1
```

---

**Case P56-M64 + `dhcps6.mode=hybrid`** — addresses same as P56-M64-A0; RA/DHCPv6 flags differ

```shell
"dhcps6": { "mode":"hybrid", "pd":"auto", "dns_mode":"auto" }
```

| Item | Concrete value |
|------|----------------|
| Router / PC GUA / ULA | **same addresses** as P56-M64-A0 |
| RA | `M=0 O=1`; PIO same `/64`s; RDNSS may still be present |
| DHCPv6 to PC | DNS (and other options), **not** a different GUA; e.g. DNS `2001:db8:0:0::1` |

```text
RA: Router Lifetime 1800, M=0 O=1
PIO: 2001:db8:0:0::/64 L=1 A=1
PIO: fd12:3456:789a::/64 L=1 A=1
(+ DHCPv6 Information-Request → DNS 2001:db8:0:0::1)
```

---

**Case P56-M64 + `dhcps6.mode=dhcp`** — Managed; no A-flag SLAAC for GUA

```shell
"dhcps6": { "mode":"dhcp", "pd":"disable", "dns_mode":"auto", "leasetime":"86400" }
```

| Item | Concrete value |
|------|----------------|
| Router on `lan` | still `2001:db8:0:0::1/64` (router keeps ::1) |
| PC GUA | DHCPv6 IA_NA example `2001:db8:0:0::a8bb` or odhcpd hostid form inside `2001:db8:0:0::/64` — **not** EUI-64 SLAAC |
| PC ULA | if ULA PIO still A=1, may still SLAAC ULA; or only GUA via DHCP depending on implementation choice (document intent: GUA managed; ULA optional) |
| RA | `M=1 O=0` (or O=1 if DNS via DHCP); **A=0** on GUA PIO so hosts do not SLAAC GUA |

```text
RA: Router Lifetime 1800, M=1 O=0
PIO: 2001:db8:0:0::/64  L=1 A=0
RDNSS: optional; DNS often via DHCPv6
DHCPv6 IA_NA: e.g. 2001:db8:0:0::1000 (hostid), valid/preferred per leasetime
```

---

**Case P64** — uplink PD is only `/64` (`2001:db8:abcd::/64`); `pdlen` cannot enlarge

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "pdid":"0",
    "pdlen":"64",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| LAN owns | `2001:db8:abcd::/64` (entire PD) |
| Router | `2001:db8:abcd::1/64`, `fd12:3456:789a::1/64`, fe80… |
| PC GUA | `2001:db8:abcd:a8bb:ccff:fedd:eeff/64` |
| Nested PD (`pd=auto`) | **off** — no leftover |
| `pdlen=60` or `56` | **invalid / no-op** — cannot carve bigger than `/64` |

**RA:**

```text
M=0 O=0
PIO: 2001:db8:abcd::/64  L=1 A=1
PIO: fd12:3456:789a::/64  L=1 A=1
RDNSS: 2001:db8:abcd::1
```

---

**Case RELAY** — uplink IPv6, **no** IA_PD; WAN on-link `2001:db8:aaaa::5/64`

```shell
ifname@lan
{
    "mode6":"ifname@wan",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| Path | odhcpd **relay** (not server); `dhcps6.mode` ignored for GUA |
| WAN | `2001:db8:aaaa::5/64`, fe80 on `wan` |
| Router on `lan` | `fe80::50:f4ff:fe00:1`, optional ULA `fd12:3456:789a::1/64` (local); **no** new GUA /64 from PD |
| PC GUA | same on-link as WAN if ISP allows ND proxy, e.g. `2001:db8:aaaa:a8bb:ccff:fedd:eeff/64` |
| RA on `lan` | **relayed** upstream RA (ISP flags/prefixes), not locally authored M/O for GUA |

Example **upstream RA** (as seen after relay; values from ISP):

```text
(from ISP, relayed)
Router Lifetime: (ISP)
M/O: (ISP)
PIO: 2001:db8:aaaa::/64  L=1 A=1
RDNSS: (ISP or empty)
```

If ULA is still server-advertised separately, hosts may also see local ULA PIO `fd12:3456:789a::/64` (implementation detail when mixing relay GUA + local ULA).

---

**Case NO6** — uplink has no IPv6; local ULA only

```shell
ifname@lan
{
    "mode6":"follow",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
```

| Item | Concrete value |
|------|----------------|
| WAN IPv6 | none |
| Router on `lan` | `fd12:3456:789a::1/64`, `fe80::50:f4ff:fe00:1/64` |
| PC | `fd12:3456:789a:a8bb:ccff:fedd:eeff/64`, fe80…; **no** `2001:…` |
| Routes | `fd12:3456:789a::/64` dev `lan`; **no** `::/0` IPv6 default |
| Nested PD | off |

**RA (local server):**

```text
Router Lifetime: 1800
M=0 O=0
PIO: fd12:3456:789a::/64  L=1 A=1
RDNSS: fd12:3456:789a::1
(no GUA PIO)
```

---

**Side-by-side (PD `/56` only; same PC MAC; `ula=enable`; slaac unless noted)**

| Case | status `prefix` | Router GUA | PC GUA | RA PIO (GUA) | RA M/O | Nested PD example |
|------|-----------------|------------|--------|--------------|--------|-------------------|
| pdlen=64 pdid=0 | `2001:db8:0:0::/64` | `2001:db8:0:0::1/64` | `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` | `/64` that | 0/0 | `2001:db8:0:1::/64`… |
| pdlen=64 pdid=1 | `2001:db8:0:1::/64` | `2001:db8:0:1::1/64` | `2001:db8:0:1:a8bb:ccff:fedd:eeff/64` | `/64` that | 0/0 | others except `:1:` |
| pdlen=60 pdid=0 | `2001:db8:0:0::/60` | `2001:db8:0:0::1/64` | `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` | **still** `…:0::/64` | 0/0 | `:1:`…`:f:` inside /60 |
| pdlen=56 pdid=0 | `2001:db8::/56` | `2001:db8:0:0::1/64` | `2001:db8:0:0:a8bb:ccff:fedd:eeff/64` | **still** `…:0::/64` | 0/0 | any other /64 in /56 |
| hybrid | same as pdlen=64 pdid=0 | same | same SLAAC GUA | same PIO | 0/1 | same |
| dhcp | same as pdlen=64 pdid=0 | `…::1/64` | IA_NA e.g. `2001:db8:0:0::1000` | PIO A=0 | 1/0 | if `pd=auto` |

Uplink case D — uplink has no IPv6 at all (no GUA, no PD)

Effect: **must not relay** (nothing to relay). Use **server** with a local prefix: automatic `ula=enable`, or `mode6=static6`. No default route to the IPv6 Internet. `dhcps6.mode` applies to the local prefix.

```shell
# D1 — factory follow + ula: local ULA LAN only (mode6=follow, not WAN "auto")
ifname@lan
{
    "mode6":"follow",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto", "dns_mode":"auto" }
}
# LAN: fdxx::1/64 (+ fe80::); hosts: ULA SLAAC + fe80::; no GUA; pd=auto => off (ULA has no leftover public PD)

# D2 — fixed ULA / GUA-looking static prefix you own
ifname@lan
{
    "mode6":"static6",
    "static6": { "addr":"fd12:3456:789a::1" },
    "ula":"disable",
    "dhcps6": { "mode":"slaac" }
}
# LAN and hosts use that /64 only

# D3 — hybrid / dhcp on local-only prefix
ifname@lan
{
    "mode6":"static6",
    "static6": { "addr":"fd00:1::1" },
    "dhcps6": { "mode":"hybrid", "dns_mode":"auto" }
}
ifname@lan
{
    "mode6":"static6",
    "static6": { "addr":"fd00:1::1" },
    "dhcps6": { "mode":"dhcp" }
}

# D4 — ula=disable and no static6: hosts stay link-local only
ifname@lan
{
    "mode6":"follow",
    "ula":"disable",
    "dhcps6": { "mode":"slaac" }
}
# LAN/hosts: essentially fe80:: only (no PIO to advertise)
```

Uplink case E — mix: GUA from PD (or static6) plus ULA

```shell
ifname@lan
{
    "mode6":"follow",
    "ula":"enable",
    "dhcps6": { "mode":"slaac", "pd":"auto" }
}
# When uplink has PD: LAN has GUA slice + ULA; hosts may get both via RA (server stanza)
# When uplink has no PD but has IPv6: LAN stanza is full relay — ULA may be added on netdev but is NOT separately server-advertised
```

#### Configuration settings example

Example, modify the first local network IP address
```shell
ifname@lan:static/ip=192.168.2.1
ttrue
```

Example, disable DHCP server on the first local network
```shell
ifname@lan:dhcps/status=disable
ttrue
```

Example, merge set DHCP pool of the first local network( include "startip" "endip" )
```shell
ifname@lan:dhcps|{"startip":"192.168.2.100","endip":"192.168.2.200"}
ttrue
```

Example, merge set IPv6 follow WAN and Managed host mode
```shell
ifname@lan|{"mode6":"ifname@wan","dhcps6":{"mode":"dhcp","pd":"auto"}}
ttrue
```



### Concepts

#### IPv6 model (LAN)

IPv6 on `ifname@lan` is two steps:

1. **Own a prefix on this LAN netdev** — chosen by `mode6` / `static6` / `pdid`+`pdlen` / `ula`. Follow / pin / ULA / PD slices are installed by `client@dhcps` `_odhcpd`. `mode6=static6` also applies `static6{}` on the LAN netdev via ethcon. Typical host address is `::1` with `static6.mask` (default `/64`). Owned CIDR appears in `ifname@lan.status` as `prefix` (PD slice, static6 network, or ULA-only `/64`). `addr*` lists addresses on the netdev.
2. **Advertise to hosts** (`dhcps6` → `client@dhcps` → odhcpd). odhcpd reads prefixes already present on the interface; it does not invent GUA/ULA from `dhcps6` alone.

**Where status `prefix` comes from:**

| Who | When | Object |
|-----|------|--------|
| WAN/LTE/WISP IPv6 up | uplink gets IA_PD (or tunnel PD) | that uplink `status.prefix` |
| `client@dhcps` `_odhcpd` | installs LAN PD slice / static6 / ULA-only | that LAN `status.prefix` |
| `_odhcpd` / disable | LAN `mode6`/`dhcps6` disable or no owned prefix | LAN `status.prefix` omitted |

Normal home path is **prefix + RA (SLAAC)**. There is no IPv4-style `startip`/`endip` for IPv6; odhcpd IA_NA (when `mode=dhcp`) uses hostids inside the LAN /64.

| Role | Keys | Responsibility |
|------|------|----------------|
| Prefix source | `mode6`, `static6`, `pdid`, `pdlen`, `ula` | `_odhcpd` puts address/prefix on the LAN (PD slice / static6 / ULA) |
| Host config | `dhcps6.mode`, `pd`, `dns_mode`, `dns`, `dns2`, `lifetime`, `leasetime` | `_odhcpd` → odhcpd RA / DHCPv6 / optional downstream IA_PD |

**Server vs relay (by uplink, for `follow` / `ifname@…`):**

| Uplink | LAN path | `dhcps6.mode` |
|--------|----------|---------------|
| IPv6, no IA_PD | relay RA+NDP+DHCPv6 to that uplink | ignored (pass-through) |
| IA_PD /64 or longer | server; put sliced `/64` (`::1`) on LAN | slaac / hybrid / dhcp apply |
| no IPv6 | server + ULA or `static6` only; do not relay | slaac / hybrid / dhcp apply on local prefix |

**Multi-LAN / multi-WAN (`client@dhcps` `_odhcpd` only — ethcon/ltecon stay WAN-client):**

| Intent | Config | Result |
|--------|--------|--------|
| One LAN, auto uplink | `mode6=follow` | follow-pick: PD on default uplink → last successful PD uplink → first PD; else IPv6 on default → first IPv6; then server or relay |
| Pin one WAN/LTE | `mode6=ifname@wan` / `ifname@lte`… | only that uplink when it is IPv6-up; PD→server, v6 no PD→relay candidate, none→ULA server |
| Two LANs, same PD WAN, different slices | both pin (or follow) same uplink; `pdid` 0 / 1 | parallel **servers**; `pdid=auto` = order in `network@frame.list local` (0,1,…) |
| Two LANs, two PD WANs | `lan`→`ifname@wan`, `lan2`→`ifname@lte` (both have IA_PD) | parallel **servers** (different masters OK) |
| Two LANs, two no-PD WANs (both want relay) | both pin different uplinks | **one** relay `master`: pinned preferred; `network@frame.default` breaks ties; else first pin / first follow. Losing **pin** → **server/ULA** (never wrong-master relay). Losing **follow** → share winning master |
| Mix PD server + relay | one LAN PD, another no-PD | PD LAN stays server; relay LAN uses the single master |
| `follow` cannot split WANs | two LANs both `follow` | both get the **same** follow-pick uplink — use pins to assign different WANs |

**odhcpd can:** advertise PIO from addresses already on the LAN; RA flags for slaac/hybrid/dhcp; optional DHCPv6 DNS / IA_NA; turn on `dhcpv6=server` for `pd≠disable` (nested IA_PD best-effort); relay RA/NDP/DHCPv6 with **one** `master`; mix server LANs + one relay group in one process.

**odhcpd / `_odhcpd` cannot (today):** run two different relay masters; carve leftover nested-PD policy beyond “install `::1/64` + optionally enable dhcpv6 server”; advertise local ULA as a **server** PIO while the LAN stanza is full **relay** (ULA address is still added on the netdev when `ula=enable`; ULA-only sets LAN `status.prefix` to that ULA `/64`).

**Implementation split:**

- **ethcon (LAN + WAN/WISP) / ltecon:** WAN/LTE/WISP acquire only (`auto`/`dhcpc6`/`slaac`/`static6`…). On LAN, `mode6=follow` / `ifname@…` does **not** start `odhcp6c`. LAN `mode6=static6` applies `static6{}` on the LAN netdev in ethcon; PD slice / ULA / odhcpd UCI stay in `_odhcpd`.
- **client@dhcps `_odhcpd`:** install LAN PD slice / ULA (and static6 when used as the owned server prefix); decide per-LAN server vs relay + single master; honor `pd` / `dns_mode`; triggered by setup / `network/on|off` reset / `network/upline|downline`.



### API Reference

#### Management APIs

+ `setup[]` **setup the local network**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **shutdown the local network**
    - failed return tfalse
    - succeed return ttrue


#### Query APIs

+ `status[]` **get local network information**
    - failed return NULL
    - succeed return [ json ], local network status information
    ```json
    {
        "status":"Current state",        // [ "nodevice", "uping", "down", "up" ]
                                             // "nodevice" means the underlying device is not present
                                             // "uping" means connecting
                                             // "down" means interface is down
                                             // "up" means connection is established
        "mode":"IPV4 address mode",     // [ "dhcpc", "static" ]
        "netdev":"netdev name",         // [ string ]
        "ifdev":"ifdev name",           // [ string ], Optional
        "gw":"gateway ip address",      // [ ip address ], Optional
        "dns":"dns ip address",         // [ ip address ], Optional
        "dns2":"dns2 ip address",       // [ ip address ], Optional
        "ip":"ip address",              // [ ip address ]
        "mask":"network mask",          // [ ip address ]
        "ontime":"online uptime",       // [ string ], Optional, online system uptime
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"received bytes",    // [ number ]
        "rx_packets":"received packets",// [ number ]
        "tx_bytes":"sent bytes",        // [ number ]
        "tx_packets":"sent packets",    // [ number ]
        "mac":"MAC address",            // [ mac address ]
        "mode6":"IPv6 prefix source",   // [ "disable", "static6", "follow", "ifname@wan", "ifname@lte", "ifname@wisp", "..." ], Optional
        "addr":"IPv6 address/prefixlen",  // [ string ], Optional, CIDR on netdev, e.g. "2001:db8::1/64"
        "addr2":"IPv6 address2/prefixlen", // [ string ], Optional
        "addr3":"IPv6 address3/prefixlen", // [ string ], Optional
        "prefix":"LAN IPv6 prefix"      // [ string ], Optional, owned CIDR (PD slice / static6 / ULA /64); omitted when none
    }
    ```

    Example, get the first local network information
    ```shell
    ifname@lan.status
    {
        "status":"up",                     # connect is succeed
        "mode":"static",                   # IPv4 connect mode is static
        "netdev":"lan",                    # netdev is lan
        "ip":"192.168.1.1",                # ip address is 192.168.1.1
        "mask":"255.255.255.0",            # network mask is 255.255.255.0
        "livetime":"01:15:50:0",           # already online 1 hour and 15 minute and 50 second
        "rx_bytes":"1256",                 # receive 1256 bytes
        "rx_packets":"4",                  # receive 4 packets
        "tx_bytes":"1320",                 # send 1320 bytes
        "tx_packets":"4",                  # send 4 packets
        "mac":"02:50:F4:00:00:00",         # netdev MAC address is 02:50:F4:00:00:00
        "mode6":"follow",                  # IPv6 follows an uplink (or ULA when uplink has no IPv6)
        "addr":"2001:db8:0:0::1/64",       # host address on the bridge
        "prefix":"2001:db8:0:0::/64"       # owned LAN prefix (reg; from _odhcpd)
    }
    ```

+ `netdev[]` **get the netdev**
    - failed return NULL
    - succeed return [ string ], the netdev name

    Example, get the first local network netdev
    ```shell
    ifname@lan.netdev
    lan
    ```

+ `ifdev[]` **get the ifdev**
    - failed return NULL
    - succeed return [ string ], the ifdev component name

    Example, get the first local network ifdev
    ```shell
    ifname@lan.ifdev
    vlan@lan
    ```
