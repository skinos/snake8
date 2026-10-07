## client@dhcps — DHCP Server Management

### Overview

Manage DHCP server (dnsmasq) and IPv6 RA/DHCPv6 (odhcpd) for local network interfaces. DHCP server settings are configured per logical **`ifname`** (e.g. `ifname@lan`). The component generates dnsmasq configuration from per-interface `dhcps`, and odhcpd UCI from each LAN `mode6`/`dhcps6` (`slaac`/`hybrid`/`dhcp`, optional downstream PD).

- manages dnsmasq and odhcpd lifecycle: setup, shutdown, reset, reload
- generates per-interface DHCP configuration from ifname settings
- supports static IP assignment, gateway, DNS, and classless static routes
- integrates with client@station for MAC-IP binding (dhcp-host entries)
- supports DNS proxy configuration
- provides DHCP lease listing from dnsmasq lease file
- IPv6: `_odhcpd` installs LAN prefixes (follow PD / static6 / ULA) and builds odhcpd UCI with per-LAN pin/`follow` and a single relay master (ethcon/ltecon do not do this)



### Configuration reference ( client@dhcps )

DHCP server configuration is stored per-interface in each **`ifname@lan`** component's `dhcps` subtree. `client@dhcps` reads and writes these settings.

```json
// Attributes introduction 
{
    "interface name":                             // [ string ], e.g. "ifname@lan", "ifname@lan2"
    {
        "status":"DHCP server status",            // [ "disable", "enable" ]
        "startip":"pool start IP",                // [ ip address ], first IP of the DHCP range
        "endip":"pool end IP",                    // [ ip address ], last IP of the DHCP range
        "mask":"subnet mask",                     // [ ip address ], optional, defaults to interface static mask
        "lease":"lease time",                     // [ number ], seconds, values below 120 are raised to 120
        "gw":"gateway address",                   // [ ip address ], optional, router option sent to clients
        "dns":"primary DNS server",               // [ ip address ], optional, DNS server option sent to clients
        "dns2":"secondary DNS server",            // [ ip address ], optional, secondary DNS server
        "routeopt_table":"static routes",         // [ json ], optional, RFC 3442 classless static route option
        {
            "route name":                         // [ string ]
            {
                "target":"network address",       // [ ip address ]
                "mask":"CIDR bits",               // [ string ]
                "gw":"gateway"                    // [ ip address ]
            }
            // "...":{}  How many routes show how many properties
        },
        "options":"extra dnsmasq options",        // [ string ], optional, semicolon-separated dnsmasq config lines
        "dnsproxy":"DNS proxy settings"           // [ json ], optional, DNS proxy/redirect configuration
        {
            "dns":"DNS server to redirect to"     // [ ip address ]
        }
    }
    // "...":{}  How many interfaces show how many properties
}
```

#### Configuration example

Example, show all DHCP server configuration
```shell
client@dhcps
{
    "ifname@lan":
    {
        "status":"enable",
        "startip":"192.168.31.100",
        "endip":"192.168.31.254",
        "mask":"255.255.255.0",
        "lease":"86400",
        "gw":"192.168.31.1",
        "dns":"8.8.8.8",
        "dns2":"114.114.114.114"
    }
}
```

#### Configuration settings example

Example, modify DHCP pool for ifname@lan
```shell
client@dhcps:ifname@lan/startip=192.168.31.200
ttrue
```

Example, set DNS servers for ifname@lan
```shell
client@dhcps:ifname@lan/dns=8.8.8.8
ttrue
```

Example, merge set DHCP configure for ifname@lan
```shell
client@dhcps|{"ifname@lan":{"status":"enable","startip":"192.168.31.100","endip":"192.168.31.254","lease":"86400"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start DHCP services**
    - succeed return ttrue
    - on slave platforms, no DHCP service is started
    - starts dnsmasq; starts odhcpd only when the kernel has IPv6 support

+ `shut[]` **stop DHCP services**
    - succeed return ttrue
    - stops dnsmasq; stops odhcpd when it was started for IPv6


#### Query APIs

+ `list[]` **list current DHCP lease information**
    - succeed return [ json ], DHCP lease entries from dnsmasq lease file
    - returns empty JSON object `{}` if no lease information
    - each key is a client MAC; each value has ip and name
    ```json
    {
        "client MAC address":                 // [ string ]
        {
            "ip":"ip address",                // [ ip address ]
            "name":"client name"              // [ string ], hostname from DHCP
        }
        // "...":{}  How many clients show how many properties
    }
    ```

    Example, list all DHCP clients
    ```shell
    client@dhcps.list
    {
        "04:CF:8C:39:91:7A":
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",
            "ip":"192.168.31.140"
        },
        "40:31:3C:B5:6D:4C":
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C"
        }
    }
    ```


#### Control APIs

+ `reset[]` **restart DHCP services**
    - succeed return ttrue
    - on slave platforms, does nothing
    - backs up lease file, restarts dnsmasq (and odhcpd when the kernel has IPv6), restores lease file
    - used by `network/on` and `network/off` (LAN up/down) and by the user to reset all DHCP services

+ `online[]` **reload IPv4 DHCP (dnsmasq)**
    - succeed return ttrue
    - on slave platforms, does nothing
    - SIGHUP dnsmasq only (resolv/DNS after WAN IPv4 online/offline)

+ `upline[]` **reload IPv6 DHCP (odhcpd)**
    - succeed return ttrue
    - on slave platforms, does nothing
    - no-op when the kernel has no IPv6 support
    - regenerates `/etc/config/dhcp` (and re-applies LAN prefixes in `_odhcpd`) then **restarts** odhcpd
    - SIGHUP is not used: odhcpd does not reload UCI on signal
    - used by `network/upline` and `network/downline`

#### IPv6 pin / follow (`_odhcpd`)

LAN `mode6`: `disable` | `static6` | `follow` | `ifname@…` (legacy `auto`→`follow`). Implemented only in `client@dhcps` (not ethcon/ltecon WAN-client paths). When the kernel has no IPv6, every LAN is treated as `mode6=disable` (no odhcpd LAN work).

`_odhcpd` pipeline (one function, no helpers):

1. **A** open `/etc/config/dhcp`
2. **B** scan default + all WAN uplinks that are IPv6-up → follow-pick (PD: default → last successful PD uplink → first; else v6: default → first)
3. **C** scan each LAN → provisional uplink + want (`server` / `relay`)
4. **D** decide plan: `server-only` (multi-PD OK) vs `single-relay` (one master)
5. **E** finalize LANs: pin≠master → server/ULA; `follow` RELAY shares master
6. **F** install prefixes + write UCI per LAN (+ one master WAN stanza if relay)
7. **G** `execlp(odhcpd -u)` — `-u` disables netifd/ubus ifname lookup (landos has no netifd; otherwise RA never goes out)

| Want | When | Action |
|------|------|--------|
| server | `static6`, or uplink has IA_PD, or no IPv6 | install prefix (static6 / PD slice / ULA) + `ra/dhcpv6=server` |
| relay | uplink has IPv6 but no PD | `ra/ndp/dhcpv6=relay`; one process-wide `master` |

Relay master: among RELAY LANs prefer `ifname@` pins; `network@frame.default` breaks ties; else first. Pin≠master → server/ULA + warn. `follow` RELAY shares the winning master. Multiple PD servers (different WANs) OK in parallel — pin each LAN; two `follow` LANs share one follow-pick uplink.

Owned LAN CIDR (PD slice / static6 / ULA-only) appears in `ifname@lan*.status` as `prefix`. Uplink IA_PD appears in that WAN/LTE `status` as `prefix`. Limits: one relay master only; `pd=auto` enables dhcpv6 server but does not compute leftover slices; relay stanza does not also serve local ULA PIO.


### Published Joint Events

The following joint events trigger DHCP service actions. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `network/on` | Local interface up. Triggers `client@dhcps.reset` (IPv4+IPv6 restart). |
| `network/off` | Local interface down. Triggers `client@dhcps.reset` (IPv4+IPv6 restart). |
| `network/online` | IPv4 internet up. Triggers `client@dhcps.online` (dnsmasq SIGHUP). |
| `network/offline` | IPv4 internet down. Triggers `client@dhcps.online` (dnsmasq SIGHUP). |
| `network/upline` | IPv6 uplink up. Triggers `client@dhcps.upline` (odhcpd restart). |
| `network/downline` | IPv6 uplink down. Triggers `client@dhcps.upline` (odhcpd restart). |
