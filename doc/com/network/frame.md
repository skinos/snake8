## network@frame — Network Frame Management

### Overview

The **network frame** component (`network@frame`) is the hub for LAN/WAN/VPN registration, routing policy, and firewall hooks. When several **uplinks** exist, it works with the **multi-link scheduler service** to pick the active default route using numbered **priority slots** (`"1"`..`"10"`, slot 1 first). A link is available when it is up, has an IPv4 address, and is past the first probe. A link still waiting for that probe (shown as not yet communicating) is left out, the same as a link still coming up. Receive Count mode counts as available. Supported policies are **cold backup**, **hot backup**, **lazy hot backup**, and **load balancing**.

- manages network infrastructure lifecycle: iptables NAT/masq, flow offload, IP rules
- registers local/extern/VPN interfaces and coordinates VLAN and bridge sub-modules
- publishes joint events on IPv4/IPv6 interface state changes
- works with the **connect** service for multi-uplink scheduling
- detects whether the kernel has IPv6 (`/proc/sys/net/ipv6`); when it does not, ifname/client IPv6 paths stay inactive (no DHCPv6 client, no odhcpd, no IPv6 upline joints)
- IPv6 default route (`::/0`) follows the same slot preference as IPv4 `type`; `type6` only decides whether a dead PD prefix may move to another uplink



### Network Architecture

The network subsystem uses a layered architecture that separates configuration, infrastructure management, logical interfaces, and hardware devices.

```
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                              Configuration Layer (arch@net)                                 │
│                                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────────────┐    │
│  │  NET_CONFIG (arch@net)                                                              │    │
│  │                                                                                     │    │
│  │  "local": {                           "extern": {                                   │    │
│  │    "ifname@lan": {                      "ifname@wan": {                             │    │
│  │      "concom":"ifname@ethcon",            "concom":"ifname@ethcon",                 │    │
│  │      "ifdev":"bridge@lan"                 "ifdev":"ethernet@lan1"                   │    │
│  │    },                                    },                                         │    │
│  │    "ifname@lan2": { ... }                "ifname@lte": {                            │    │
│  │  }                                        "concom":"ifname@ltecon",                 │    │
│  │                                           "ifdev":"modem@lte"                       │    │
│  │  "vlan": { ... }                        },                                          │    │
│  │  "bridge": { ... }                      "ifname@wisp": {                            │    │
│  │                                           "concom":"ifname@ethcon",                 │    │
│  │  "connect": {                             "ifdev":"wifi@nsta"                       │    │
│  │    "type":"hot4",                       }                                           │    │
│  │    "1":"ifname@wan",                  }                                             │    │
│  │    "2":"ifname@lte",                                                                │    │
│  │    ...                                                                              │    │
│  │  }                                                                                  │    │
│  └─────────────────────────────────────────────────────────────────────────────────────┘    │
│                                                                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
                                                │ reads at setup
                                                ▼
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                            Infrastructure Layer (network@frame)                             │
│                                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────────────────────┐    │
│  │  network@frame                                                                      │    │
│  │                                                                                     │    │
│  │  • reads arch@net for local/extern lists                                            │    │
│  │  • registers ifname instances: register(ifname, concom, ifdev, type)                │    │
│  │  • manages iptables NAT/masq, flow offload, IP rules                                │    │
│  │  • publishes joint events: network/on, network/off, network/online, ...             │    │
│  │  • coordinates VLAN (network@vlan) and Bridge (network@bridge)                      │    │
│  │  • starts connect service for multi-uplink scheduling                               │    │
│  └─────────────────────────────────────────────────────────────────────────────────────┘    │
│                                                                                             │
│  ┌──────────────────────────────┐  ┌──────────────────────────────-┐                        │
│  │  network@vlan                │  │  network@bridge               │                        │
│  │  creates vlan@<id> instances │  │  creates bridge@<id> instances│                        │
│  └──────────────────────────────┘  └─────────────────────────────-─┘                        │
│                                                                                             │
│  ┌──────────────────────────────┐                                                           │
│  │  network@connect             │  multi-link scheduler service                             │
│  │  cold / hot / lazy / balance │  selects the default, or shares new sessions              │
│  └──────────────────────────────┘                                                           │
│                                                                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
                                                │ registers & manages
                                                ▼
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                           Logical Interface Layer (ifname@*)                                │
│                                                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐         │
│  │   ifname@lan    │  │   ifname@wan    │  │   ifname@lte    │  │   ifname@wisp   │         │
│  │                 │  │                 │  │                 │  │                 │         │
│  │  concom:        │  │  concom:        │  │  concom:        │  │  concom:        │         │
│  │  ifname@ethcon  │  │  ifname@ethcon  │  │  ifname@ltecon  │  │  ifname@ethcon  │         │
│  │                 │  │                 │  │                 │  │                 │         │
│  │  ifdev:         │  │  ifdev:         │  │  ifdev:         │  │  ifdev:         │         │
│  │  bridge@lan     │  │  ethernet@lan1  │  │  modem@lte      │  │  wifi@nsta      │         │
│  │                 │  │                 │  │                 │  │                 │         │
│  │  mode: static   │  │  mode: dhcpc    │  │  mode: ppp      │  │  mode: dhcpc    │         │
│  │  dhcps: enable  │  │  ppoec/static   │  │  masq: enable   │  │  masq: enable   │         │
│  │  masq: disable  │  │  masq: enable   │  │  keeplive: ...  │  │  keeplive: ...  │         │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘         │
│           │                    │                    │                    │                  │
│           │ setup/shut         │ setup/shut         │ setup/shut         │ setup/shut       │
│           │ status/netdev      │ status/netdev      │ status/netdev      │ status/netdev    │
│           │ keepon/keepoff     │ keepon/keepoff     │ keepon/keepoff     │ keepon/keepoff   │
│           ▼                    ▼                    ▼                    ▼                  │
│  ┌─────────────────────────────────────────────────────────────────────────────────────┐    │
│  │  ethcon / ltecon (connection handlers)                                              │    │
│  │  • manages connection lifecycle (setup → connect → online → keepalive)              │    │
│  │  • calls network@frame online/offline/upline/downline when state changes            │    │
│  │  • handles keeplive (ICMP, DNS, recv) and failover actions                          │    │
│  └─────────────────────────────────────────────────────────────────────────────────────┘    │
│                                                                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
                                                │ uses as ifdev
                                                ▼
┌────────────────────────────────────────────────────────────────────────────────────────────┐
│                              Device Layer (ifdev components)                               │
│                                                                                            │
│  ┌───────────────────────┐  ┌───────────────────────┐  ┌───────────────────────┐           │
│  │   Ethernet Devices    │  │    Modem Devices      │  │    WiFi Devices       │           │
│  │                       │  │                       │  │                       │           │
│  │  ethernet@lan         │  │  modem@lte            │  │  wifi@nsta (2.4G STA) │           │
│  │  ethernet@lan1        │  │  modem@lte2           │  │  wifi@asta (5.8G STA) │           │
│  │  ethernet@lan2        │  │                       │  │                       │           │
│  │                       │  │  atd (AT daemon)      │  │  wifi@nssid (2.4G AP) │           │
│  │  APIs:                │  │  smsd (SMS daemon)    │  │  wifi@assid (5.8G AP) │           │
│  │  • setup/shut         │  │                       │  │                       │           │
│  │  • up/connect/down    │  │  APIs:                │  │  wifi@n (2.4G radio)  │           │
│  │  • connected/reset    │  │  • setup/shut         │  │  wifi@a (5.8G radio)  │           │
│  │  • status/netdev      │  │  • up/connect/down    │  │                       │           │
│  │  • online/offline     │  │  • connected/reset    │  │  APIs:                │           │
│  │  • keeplive/setmac    │  │  • status/netdev/tty  │  │  • setup/shut         │           │
│  │  • hwnat              │  │  • sim/pin/plmn/signal│  │  • up/connect/down    │           │
│  │                       │  │  • operator/imei/imsi │  │  • connected/reset    │           │
│  └───────────┬───────────┘  │  • custom_set/watch   │  │  • status/netdev      │           │
│              │              │  • lock_imei/lock_imsi│  │  • aplist/chlist      │           │
│              │              │  • bsim_back/main     │  │  • securelist/stalist │           │
│              │              │                       │  │  • hostapd/wpa        │           │
│              │              └───────────┬───────────┘  └───────────┬───────────┘           │
│              │                          │                          │                       │
└──────────────┼──────────────────────────┼──────────────────────────┼───────────────────────┘
               │                          │                          │
               │ network@frame.add        │ network@frame.add        │ network@frame.add
               ▼                          ▼                          ▼
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                              Hardware Layer (Linux netdev)                                  │
│                                                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐         │
│  │   eth0/eth1     │  │   usb0/usb1     │  │   wlan0/wlan1   │  │     lan         │         │
│  │   (SoC ETH)     │  │   (USB modem)   │  │   (WiFi radio)  │  │   (Bridge)      │         │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  └─────────────────┘         │
│                                                                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
```

**Key Relationships:**

| From | To | Relationship | Mechanism |
|------|-----|--------------|-----------|
| `arch@net` | `network@frame` | provides topology | `sget(NET_CONFIG)` reads local/extern/vlan/bridge/connect |
| `network@frame` | `ifname@*` | registers | `register(ifname, concom, ifdev, type)` |
| `network@frame` | `network@vlan` | coordinates | `scall(VLAN_COM, "setup")` |
| `network@frame` | `network@bridge` | coordinates | `scall(BRIDGE_COM, "setup")` |
| `network@frame` | `network@connect` | starts | `scall(CONNECT_COM, "setup")` |
| `ifname@*` | `concom` (ethcon/ltecon) | delegates | `sstarts(object, drvcom, "service", ...)` |
| `ifname@*` | `ifdev` (ethernet/modem/wifi) | references | ifname holds the bound ifdev name for status / connect |
| `ifname@*` | `network@frame` | notifies | `scall(NETWORK_COM, "online/offline/upline/downline")` |
| `ethernet@*` | `network@frame` | registers devices | `scalls(NETWORK_COM, "add", "%s,%s", object, netdev)` |
| `modem@*` | `network@frame` | registers devices | `scalls(NETWORK_COM, "add", "%s,%s", object, netdev)` |
| `wifi@*` | `network@frame` | registers devices | `scalls(NETWORK_COM, "add", "%s,%s", object, netdev)` |
| `arch@ethernet` | `arch@net` | configures per mode | `config_sgets(COM_IDPATH, mode)` |

**Data Flow for Interface Lifecycle:**

```
1. Boot:  arch@net → network@frame.setup()
              │
              ├─ reads local/extern lists
              ├─ for each: register(ifname, concom, ifdev, type)
              ├─ setup VLAN/bridge
              └─ start connect service (if multi-link)

2. Interface Up:  ifname@wan.setup()
              │
              ├─ ethcon: gets ifdev (ethernet@lan1)
              ├─ calls ifdev "up" → ifconfig eth0 up
              ├─ calls ifdev "connect" → link check
              ├─ acquires IP (DHCP/Static/PPPoE)
              └─ calls network@frame "online" → publishes network/onextern, network/online

3. Interface Down:  ifname@wan.shut()
              │
              ├─ ethcon: calls network@frame "offline" → publishes network/offextern, network/offline
              ├─ releases IP
              └─ calls ifdev "down" → ifconfig eth0 down

4. Multi-link scheduling:  network@connect (scheduler)
              │
              ├─ each round, sees which extern links are up and have an address
              ├─ chooses the uplink from the scheduling type, and from recent delay when delay statistics are on
              └─ installs that default route and DNS; load balancing also shares new LAN sessions across the chosen uplinks
```

**Multi-link Scheduling Types:**

| Type | Behavior | Slots |
|------|----------|-------|
| `cold` | One uplink stays connected. The smallest slot that is up and has an address carries traffic; higher slots are shut down. When that link loses its address, the next available slot is brought up. | 1-10 |
| `hot` | Participating uplinks stay connected. Delay statistics off: the default is the smallest slot that is up, and a smaller slot takes over when it comes up. Delay statistics on: the default prefers the smallest slot whose recent average delay is within the boundary. | 1-10 |
| `hot2..hot5` | Same as hot, using only slots 1..N | 2-5 |
| `lazy` | Participating uplinks stay connected. Delay statistics off: the default stays on the current link while that link is up, and moves when it goes down. Delay statistics on: it also moves when the current link's delay is past the boundary and the difference; while the current link is still acceptable it stays, even if another link is faster. | 1-10 |
| `lazy2..lazy5` | Same as lazy, using only slots 1..N | 2-5 |
| `dbdc` | Among uplinks that are up, the earliest slot numbers share new sessions, at most 6. A higher slot stays out while six earlier uplinks are already in. Delay statistics can leave a slow link up and out of the share. | 1-10 |
| `dbdc2..dbdc6` | Same as dbdc, using only slots 1..N. The share is still at most 6 uplinks. | 2-6 |



### Configuration reference ( network@frame )

```json
// Attributes introduction 
{
    "type":"Multiple link connect type",                           // [ "cold", "hot", "hot2", "hot3", "hot4", "hot5", "lazy", "lazy2", "lazy3", "lazy4", "lazy5", "dbdc", "dbdc2", "dbdc3", "dbdc4", "dbdc5", "dbdc6" ]
                                                                   // "cold": one uplink connected. Smallest slot that is up and has an address; higher slots shut down
                                                                   // "hot": uplinks stay connected. Delay off: smallest slot that is up, and a smaller slot takes over when it comes up. Delay on: smallest slot whose average is within the boundary
                                                                   // "hotN" (N=2..5): same as hot, only slots "1".."N"
                                                                   // "lazy": uplinks stay connected. Delay off: stay on the current link while it is up. Delay on: also move when its delay is past the boundary and the difference; stay while it is still acceptable
                                                                   // "lazyN" (N=2..5): same as lazy, only slots "1".."N"
                                                                   // "dbdc": among uplinks that are up, the earliest slot numbers share new sessions, at most 6. A higher slot stays out while six earlier uplinks are already in
                                                                   // "dbdcN" (N=2..6): same as dbdc, only slots "1".."N"; the share is still at most 6 uplinks
    "concom":"Multiple link connection management components",     // [ string ], optional, custom scheduling component

    "1":"ifname object of extern",                                 // [ string ], priority slot 1, e.g. "ifname@wan", "ifname@lte", "ifname@wisp"
    "2":"ifname object of extern",                                 // [ string ], priority slot 2
    "3":"ifname object of extern",                                 // [ string ], priority slot 3
    "4":"ifname object of extern",                                 // [ string ], priority slot 4
    "5":"ifname object of extern",                                 // [ string ], priority slot 5
    "6":"ifname object of extern",                                 // [ string ], priority slot 6
    "7":"ifname object of extern",                                 // [ string ], priority slot 7
    "8":"ifname object of extern",                                 // [ string ], priority slot 8
    "9":"ifname object of extern",                                 // [ string ], priority slot 9
    "10":"ifname object of extern",                                // [ string ], priority slot 10

    "delay_count":"How many recent delay samples to average",      // [ number ], 0 turns the delay rules off
                                                                   // Compared links are up, have an address, and are past the first probe. Still waiting (not yet communicating) stays out. Receive Count stays in. A failed probe still counts as up
                                                                   // hot / lazy: the averages decide whether the default route moves. lazy also looks at the current link's latest sample before averaging the others; see delay_divide
                                                                   // dbdc: the averages decide which of the chosen links stay in the share
    "delay_divide":"Delay boundary, milliseconds",                 // [ number ]
                                                                   // hot: links whose average is at or under this value are preferred; the smallest such slot becomes the default
                                                                   //      current default is also in that group but has a larger slot: move to the smaller slot this round
                                                                   //      current default is over the boundary: move when it is slower than that smaller slot by more than delay_diff
                                                                   //      every measured link is over the boundary: use the smallest slot that is up
                                                                   //      boundary 0: measured delays sit over the line, so the smallest slot that is up is used. Receive Count is scored as this boundary when that score is left empty, so it still sits on the line. A link still waiting for its first probe is not in this round
                                                                   // lazy, current link still up: look at its latest sample before comparing averages
                                                                   //      boundary above 0 and the latest sample is still at or under the boundary: keep this link this round
                                                                   //      boundary 0 and the latest sample is still at or under delay_diff: keep this link this round
                                                                   //      latest sample past that line, no sample yet, or a failed probe: compare averages using the delay_diff cases
                                                                   //      current link down: move this round to the link with the lowest average. With delay statistics off, the target is the smallest slot that is up
                                                                   // dbdc: boundary 0 trims the share only by how far a link sits behind the fastest chosen link
                                                                   //      boundary above 0: trim when some chosen links are at or under the boundary and some are over it. Otherwise every chosen link stays in the share
    "delay_diff":"How much slower is still acceptable, milliseconds", // [ number ]
                                                                   // The same comparison in hot, lazy, and dbdc: at or under the boundary counts as within it, and over the boundary is the other side. A gap equal to this value does not move; the link must be slower by more than this. 0 means any excess is enough
                                                                   // hot: used when the current default is over the boundary. Move when its average exceeds the preferred link by more than this
                                                                   // lazy, once averages are compared. The target is the link with the lowest average
                                                                   //      boundary 0: move when the current average exceeds the fastest link by more than this
                                                                   //      this value 0 and boundary above 0: move when some link is at or under the boundary and the current average is over it
                                                                   //      both above 0: move when some link is at or under the boundary, the current average is over it, and the current average exceeds the fastest link by more than this
                                                                   // dbdc, among the links already chosen (earliest uplinks by slot number, at most 6). A link on the boundary is on the within side. Together with a link over the boundary, that starts the trim, and the link on the boundary stays
                                                                   //      boundary 0: drop a link when its average exceeds the fastest by more than this
                                                                   //      this value 0 and boundary above 0: when the set has both sides, drop links that are over the boundary
                                                                   //      both above 0: when the set has both sides, drop links that are over the boundary and exceed the fastest by more than this. A link over the boundary that exceeds the fastest by no more than this stays
    "delay_nodelay":"Score when availability check is off",         // [ number ], milliseconds. Empty uses the boundary. Used when that link has no probe samples
    "delay_failed":"Score for one failed probe",                   // [ number ], milliseconds. Empty uses (boundary + difference) × 2. That probe is replaced by this score before the average
    "delay_recv":"Score when probing has fallen back to Receive Count", // [ number ], milliseconds. Empty uses the boundary. The link is scored as this value; earlier samples are left out

    "sticky":"Keep open LAN sessions on the uplink they started on", // [ "disable", "enable" ], empty means disable. Applies to hot and lazy
                                                                   // "enable": when the default moves between links that are still up, sessions already open stay on the uplink they started on; new sessions follow the new default. A link that goes down releases the sessions that were on it

    "interval":"Connect scheduler poll interval",                  // [ number ], seconds; empty or 0 defaults to 3

    // IPv6
    "type6":"IPv6 default-route and PD-sticky policy",             // [ "type", "sticky", "disable" ], default "sticky"
                                                                        // "::/0" for both "type" and "sticky" follows the IPv4 "type" preferred uplink (skip if that uplink has no IPv6)
                                                                        // difference is only whether LAN may take another uplink PD when the PD source dies
                                                                        // "type" — allow the LAN follow-chain to take another uplink PD / relay / ULA
                                                                        // "sticky" — keep the original PD source and prefix; if that uplink dies, withdraw GUA, keep ULA, egress via preferred uplink masq6; do not steal backup PD
                                                                        // "disable" — do not install IPv6 "::/0"; do not change LAN prefix policy

    // DNS (IPv4 and IPv6; no dns6 / lan6)
    "custom_dns":"Custom DNS",                                     // [ "disable", "enable" ]
                                                                        // "disable" — learn DNS from the preferred uplink (IPv4 and IPv6)
                                                                        // "enable" — use dns / dns2 below
    "dns":"Custom DNS1",                                           // [ ipv4 address, ipv6 address ], valid when "custom_dns" is "enable"
    "dns2":"Custom DNS2",                                          // [ ipv4 address, ipv6 address ], valid when "custom_dns" is "enable"

    "offload":"flow offload mode"                                  // [ "disable", "enable", "hw" ]
                                                                        // "disable" for no offload
                                                                        // "enable" for software flow offload
                                                                        // "hw" for hardware flow offload
}
```

#### Configuration example

Example, show all the multi-link configure
```shell
network@frame
{
    "type":"hot",                              # multi-link scheduling mode: hot backup
    "1":"ifname@wan",                          # priority slot 1: WAN (Ethernet)
    "2":"ifname@lte",                          # priority slot 2: LTE modem
    "3":"ifname@lte2",                         # priority slot 3: second LTE modem
    "4":"ifname@wisp",                         # priority slot 4: WISP (wireless relay)
    "type6":"sticky",                          # IPv6 ::/0 follows IPv4 slots; do not hop PD when the PD source dies
    "custom_dns":"disable"                     # use DNS from the active uplink (IPv4 and IPv6)
}
```

Example, IPv4 cold backup (WAN then LTE)
```shell
network@frame
{
    "type":"cold",
    "1":"ifname@wan",
    "2":"ifname@lte"
}
```

Example, IPv4 hot backup
```shell
network@frame
{
    "type":"hot",
    "1":"ifname@wan",
    "2":"ifname@lte"
}
```

Example, IPv4 lazy hot backup (stay on the current uplink while it remains acceptable)
```shell
network@frame
{
    "type":"lazy",
    "1":"ifname@wan",
    "2":"ifname@lte"
}
```

Example, IPv6 factory PD sticky (recommended)
```shell
network@frame
{
    "type6":"sticky"
}
```

Example, IPv6 follow IPv4 slots and allow PD to move when the source dies
```shell
network@frame
{
    "type6":"type"
}
```

Example, IPv6 no default route from the scheduler
```shell
network@frame
{
    "type6":"disable"
}
```

Example, custom DNS IPv4
```shell
network@frame
{
    "custom_dns":"enable",
    "dns":"8.8.8.8",
    "dns2":"1.1.1.1"
}
```

Example, custom DNS IPv6
```shell
network@frame
{
    "custom_dns":"enable",
    "dns":"2001:4860:4860::8888",
    "dns2":"2606:4700:4700::1111"
}
```

Example, custom DNS mixed IPv4 and IPv6
```shell
network@frame
{
    "custom_dns":"enable",
    "dns":"8.8.8.8",
    "dns2":"2001:4860:4860::8888"
}
```

#### Configuration settings example

Example, set multi-link scheduling mode to hot4
```shell
network@frame:type=hot4
ttrue
```

Example, set priority slot 1 to WAN and slot 2 to LTE
```shell
network@frame:1=ifname@wan
ttrue
```

Example, merge set multi-link configure( include "type" "1" "2" "3" "4" )
```shell
network@frame|{"type":"hot4","1":"ifname@lte","2":"ifname@lte2","3":"ifname@wan","4":"ifname@wisp"}
ttrue
```

Example, merge set IPv6 PD policy and custom DNS
```shell
network@frame|{"type6":"sticky","custom_dns":"disable"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the network infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Sets up iptables NAT/masq chain, flow offload, IP rules, registers local/extern interfaces, sets up VLAN and bridge, starts connect service

+ `shut[]` **shutdown the network infrastructure**
    - failed return tfalse
    - succeed return ttrue
    - Stops the multi-link scheduler, unregisters interfaces, clears firewall/routing hooks


#### Query APIs

+ `status[]` **show external connections status**
    - failed return NULL
    - succeed return [ json ], status of all external connections
    ```json
    {
        "ifname object":           // [ string ], e.g. "ifname@wan", "ifname@lte", "ifname@lte2", "ifname@wisp"
        {
            "status":"Whether online",     // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
                                               // "nodevice" means the corresponding module could not be found
                                               // "reset" means modem is being reset
                                               // "setup" means modem is being initialized
                                               // "register" means modem is registering to network
                                               // "uping" for connecting
                                               // "scanning" for scanning the peer
                                               // "block" means waiting for keeplive checks to recover
                                               // "up" means ready for Internet access
                                               // "failed" for keeplive failed
                                               // "down" for the ifname is down
            "inuse":"Whether used",        // [ "disable", "enable" ], cold/hot/lazy: current default; dbdc balance mode: enable when online
            "balance":"Whether load-balanced" // [ "enable" ], dbdc only: enable when this uplink is in the current ECMP/shunts set
        }
        // "...":{}  How many extern show how many properties
    }
    ```

    Example, get all extern connection status
    ```shell
    network@frame.status
    {
        "ifname@lte":
        {
            "status":"up",
            "inuse":"enable"
        },
        "ifname@lte2":
        {
            "status":"down",
            "inuse":"disable"
        }
    }
    ```

+ `list[ type ]` **list all registered connections**
    - type ----------- [ string ], optional, filter by type: "local", "extern", "outer", "vpn", or omit for all
    - failed return NULL
    - succeed return [ json ], all registered connections with their ifdev
    ```json
    {
        "ifname object": "The corresponding ifdev object"    // [ string ]: [ string ]
        // "...":"..."  How many connections show how many properties
    }
    ```

    Example, list all connections
    ```shell
    network@frame.list
    {
        "ifname@lan":"bridge@lan",
        "ifname@lte":"modem@lte",
        "ifname@lte2":"modem@lte2"
    }
    ```

    Example, list only extern connections
    ```shell
    network@frame.list[ extern ]
    {
        "ifname@lte":"modem@lte",
        "ifname@lte2":"modem@lte2"
    }
    ```

+ `mode_list[]` **list available network modes**
    - failed return NULL
    - succeed return [ json ], available network mode names
    ```json
    {
        "mode name": ""     // [ string ]: [ string ]
        // "...":"..."  How many modes show how many properties
    }
    ```

    Example, list available network modes
    ```shell
    network@frame.mode_list
    {
        "default":"",
        "gateway":""
    }
    ```

+ `ifname[ device ]` **look up an ifname by device name**
    - device ----------- [ string ], the device name to look up
    - failed return NULL
    - succeed return [ string ], the ifname object name

    Example, look up ifname by device
    ```shell
    network@frame.ifname[ modem@lte ]
    ifname@lte
    ```

+ `local[ ifname ]` **list or check local interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ "uping", "down", "up" ]
            "mode":"IPV4 address mode",    // [ "dhcpc", "static" ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ], Optional
            "gw":"gateway ip address",     // [ ip address ], Optional
            "dns":"dns ip address",        // [ ip address ], Optional
            "dns2":"dns2 ip address",      // [ ip address ], Optional
            "ip":"ip address",             // [ ip address ]
            "mask":"network mask",         // [ ip address ]
            "mac":"MAC address",           // [ mac address ]
            "method":"IPv6 address mode",  // [ "manual", "automatic", "slaac" ], Optional
            "addr":"IPv6 address"          // [ ipv6 address ], Optional
        }
        // "...":{}  How many local interfaces show how many properties
    }
    ```

    Example, list all local interfaces
    ```shell
    network@frame.local
    {
        "ifname@lan":
        {
            "status":"up",
            "ip":"192.168.1.1",
            "mask":"255.255.255.0"
        }
    }
    ```

    Example, check if ifname@lan is a local interface
    ```shell
    network@frame.local[ ifname@lan ]
    ttrue
    ```

+ `extern[ ifname ]` **list or check extern interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
            "mode":"IPV4 address mode",    // [ "dhcpc", "static", "pppoec" ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ]
            "gw":"gateway ip address",     // [ ip address ]
            "dns":"dns ip address",        // [ ip address ]
            "dns2":"dns2 ip address",      // [ ip address ]
            "ip":"ip address",             // [ ip address ]
            "mask":"network mask",         // [ ip address ]
            "delay":"delay time",          // [ "failed", "block", number ], Optional
            "mac":"MAC address",           // [ mac address ]
            "method":"IPv6 address mode",  // [ "manual", "automatic", "slaac" ], Optional
            "addr":"IPv6 address"          // [ ipv6 address ], Optional
        }
        // "...":{}  How many extern interfaces show how many properties
    }
    ```

    Example, list all extern interfaces
    ```shell
    network@frame.extern
    {
        "ifname@wan":
        {
            "status":"up",
            "ip":"192.168.10.1",
            "gw":"192.168.10.254"
        }
    }
    ```

+ `vpn[ ifname ]` **list or check VPN interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "mode":"current mode",         // [ string ]
            "status":"current state",      // [ string ]
            "ifdev":"ifdev object",        // [ string ]
            "netdev":"netdev name",        // [ string ]
            "ip":"IP address",             // [ ip address ]
            "rx_bytes":"received bytes",   // [ number ]
            "rx_packets":"received packets",// [ number ]
            "tx_bytes":"sent bytes",       // [ number ]
            "tx_packets":"sent packets",   // [ number ]
            "mac":"MAC address"            // [ mac address ]
        }
        // "...":{}  How many VPN interfaces show how many properties
    }
    ```

+ `outer[ ifname ]` **list or check extern and VPN interfaces**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse, if absent returns JSON map
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse
    ```json
    {
        "ifname object":               // [ string ]
        {
            "status":"Current state",      // [ string ]
            "mode":"IPV4 address mode",    // [ string ]
            "netdev":"netdev name",        // [ string ]
            "ifdev":"ifdev name",          // [ string ]
            "ip":"ip address",             // [ ip address ]
            "mac":"MAC address"            // [ mac address ]
        }
        // "...":{}  How many outer interfaces show how many properties
    }
    ```

+ `default[ ifname ]` **get or check the current default connection**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse, the interface that owns the default route
    - Returns the **full status** of the interface that owns the default route (same as calling `status` on that interface). The exact fields depend on the interface type: WAN returns Ethernet fields, LTE returns modem fields, WISP returns WiFi fields.
    ```json
    {
        "status":"Current state",        // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
        "mode":"IPV4 address mode",     // [ "dhcpc", "static", "pppoec", "ppp" ], Optional
        "netdev":"netdev name",         // [ string ]
        "ifdev":"ifdev name",           // [ string ], Optional
        "ifname":"ifname name",         // [ string ], Optional
        "gw":"gateway ip address",      // [ ip address ], Optional
        "dns":"dns ip address",         // [ ip address ], Optional
        "dns2":"dns2 ip address",       // [ ip address ], Optional
        "ip":"ip address",              // [ ip address ]
        "dstip":"destination ip",       // [ ip address ], Optional, for PPP links
        "mask":"network mask",          // [ ip address ]
        "mac":"MAC address",            // [ mac address ]
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"received bytes",    // [ number ]
        "rx_packets":"received packets",// [ number ]
        "tx_bytes":"sent bytes",        // [ number ]
        "tx_packets":"sent packets",    // [ number ]
        "delay":"delay time",           // [ "failed", "block", number ], Optional, for extern interfaces

        // LTE/NR modem fields (only present when default is ifname@lte)
        "imei":"IMEI number",           // [ string ], Optional
        "imsi":"IMSI number",           // [ string ], Optional
        "iccid":"ICCID number",         // [ string ], Optional
        "plmn":"MCC and MNC",           // [ string ], Optional
        "operator":"operator name",     // [ string ], Optional
        "nettype":"network type",       // [ string ], Optional
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], Optional
        "rssi":"signal intensity",      // [ number ], Optional, the unit is dBm
        "csq":"CSQ number",             // [ number ], Optional

        // WiFi station fields (only present when default is ifname@wisp)
        "peer":"Peer SSID",             // [ string ], Optional
        "peermac":"Peer BSSID",         // [ mac address ], Optional
        "channel":"Peer channel",       // [ number ], Optional
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], Optional
        "rssi":"Peer RSSI",             // [ number ], Optional, the unit is dBm
        "rate":"connect rate"           // [ number ], Optional, the unit is M
    }
    ```

    Example, get current default connection when it is WAN (Ethernet)
    ```shell
    network@frame.default
    {
        "status":"up",
        "mode":"dhcpc",
        "netdev":"eth0",
        "ifdev":"ethernet@lan1",
        "ifname":"ifname@wan",
        "ip":"192.168.10.1",
        "mask":"255.255.255.0",
        "gw":"192.168.10.254",
        "dns":"8.8.8.8",
        "dns2":"8.8.4.4",
        "mac":"00:11:22:33:44:55",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

    Example, get current default connection when it is LTE modem
    ```shell
    network@frame.default
    {
        "status":"up",
        "mode":"ppp",
        "netdev":"ppp0",
        "ifdev":"modem@lte",
        "ifname":"ifname@lte",
        "ip":"10.0.0.2",
        "dstip":"10.0.0.1",
        "mask":"255.255.255.255",
        "dns":"8.8.8.8",
        "dns2":"8.8.4.4",
        "mac":"02:50:F4:00:00:00",
        "livetime":"01:15:30:0",
        "rx_bytes":"5678",
        "rx_packets":"42",
        "tx_bytes":"1234",
        "tx_packets":"18",
        "imei":"867160040494084",
        "imsi":"460015356123463",
        "iccid":"89860121801097564807",
        "plmn":"46001",
        "operator":"ChinaMobile",
        "nettype":"WCDMA",
        "signal":"3",
        "rssi":"-107",
        "csq":"23"
    }
    ```

    Example, get current default connection when it is WISP (WiFi station)
    ```shell
    network@frame.default
    {
        "status":"up",
        "mode":"dhcpc",
        "netdev":"ath11",
        "ifdev":"wifi@nsta",
        "ifname":"ifname@wisp",
        "ip":"192.168.1.100",
        "mask":"255.255.255.0",
        "gw":"192.168.1.1",
        "dns":"8.8.8.8",
        "mac":"02:50:F4:00:00:01",
        "livetime":"00:45:20:0",
        "rx_bytes":"98765",
        "rx_packets":"543",
        "tx_bytes":"12345",
        "tx_packets":"67",
        "peer":"MyWiFi-AP",
        "peermac":"AA:BB:CC:DD:EE:FF",
        "channel":"6",
        "signal":"3",
        "rssi":"-52",
        "rate":"72"
    }
    ```

    Example, check if ifname@lte is the current default connection
    ```shell
    network@frame.default[ ifname@lte ]
    ttrue
    ```

+ `gateway[ ifname ]` **get or check the current gateway connection**
    - ifname ----------- [ string ], optional, if given returns ttrue/tfalse
    - failed return NULL
    - succeed return [ json ] or ttrue/tfalse, the interface that owns the gateway route
    - Returns the **full status** of the interface that owns the gateway route (same as calling `status` on that interface). The exact fields depend on the interface type: WAN returns Ethernet fields, LTE returns modem fields, WISP returns WiFi fields.
    ```json
    {
        "status":"Current state",        // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ]
        "mode":"IPV4 address mode",     // [ "dhcpc", "static", "pppoec", "ppp" ], Optional
        "netdev":"netdev name",         // [ string ]
        "ifdev":"ifdev name",           // [ string ], Optional
        "ifname":"ifname name",         // [ string ], Optional
        "gw":"gateway ip address",      // [ ip address ], Optional
        "dns":"dns ip address",         // [ ip address ], Optional
        "dns2":"dns2 ip address",       // [ ip address ], Optional
        "ip":"ip address",              // [ ip address ]
        "dstip":"destination ip",       // [ ip address ], Optional, for PPP links
        "mask":"network mask",          // [ ip address ]
        "mac":"MAC address",            // [ mac address ]
        "livetime":"online time",       // [ string ], format is hour:minute:second:day
        "rx_bytes":"received bytes",    // [ number ]
        "rx_packets":"received packets",// [ number ]
        "tx_bytes":"sent bytes",        // [ number ]
        "tx_packets":"sent packets",    // [ number ]
        "delay":"delay time",           // [ "failed", "block", number ], Optional, for extern interfaces

        // LTE/NR modem fields (only present when gateway is ifname@lte)
        "imei":"IMEI number",           // [ string ], Optional
        "imsi":"IMSI number",           // [ string ], Optional
        "iccid":"ICCID number",         // [ string ], Optional
        "plmn":"MCC and MNC",           // [ string ], Optional
        "operator":"operator name",     // [ string ], Optional
        "nettype":"network type",       // [ string ], Optional
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], Optional
        "rssi":"signal intensity",      // [ number ], Optional, the unit is dBm
        "csq":"CSQ number",             // [ number ], Optional

        // WiFi station fields (only present when gateway is ifname@wisp)
        "peer":"Peer SSID",             // [ string ], Optional
        "peermac":"Peer BSSID",         // [ mac address ], Optional
        "channel":"Peer channel",       // [ number ], Optional
        "signal":"signal level",        // [ "0", "1", "2", "3", "4" ], Optional
        "rssi":"Peer RSSI",             // [ number ], Optional, the unit is dBm
        "rate":"connect rate"           // [ number ], Optional, the unit is M
    }
    ```

    Example, get current gateway connection when it is WAN (Ethernet)
    ```shell
    network@frame.gateway
    {
        "status":"up",
        "mode":"dhcpc",
        "netdev":"eth0",
        "ifdev":"ethernet@lan1",
        "ifname":"ifname@wan",
        "ip":"192.168.10.1",
        "mask":"255.255.255.0",
        "gw":"192.168.10.254",
        "dns":"8.8.8.8",
        "dns2":"8.8.4.4",
        "mac":"00:11:22:33:44:55",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

    Example, get current gateway connection when it is LTE modem
    ```shell
    network@frame.gateway
    {
        "status":"up",
        "mode":"ppp",
        "netdev":"ppp0",
        "ifdev":"modem@lte",
        "ifname":"ifname@lte",
        "ip":"10.0.0.2",
        "dstip":"10.0.0.1",
        "mask":"255.255.255.255",
        "dns":"8.8.8.8",
        "mac":"02:50:F4:00:00:00",
        "livetime":"01:15:30:0",
        "imei":"867160040494084",
        "plmn":"46001",
        "operator":"ChinaMobile",
        "signal":"3"
    }
    ```

    Example, get current gateway connection when it is WISP (WiFi station)
    ```shell
    network@frame.gateway
    {
        "status":"up",
        "mode":"dhcpc",
        "netdev":"ath11",
        "ifdev":"wifi@nsta",
        "ifname":"ifname@wisp",
        "ip":"192.168.1.100",
        "mask":"255.255.255.0",
        "gw":"192.168.1.1",
        "dns":"8.8.8.8",
        "mac":"02:50:F4:00:00:01",
        "livetime":"00:45:20:0",
        "peer":"MyWiFi-AP",
        "peermac":"AA:BB:CC:DD:EE:FF",
        "channel":"6",
        "signal":"3",
        "rssi":"-52"
    }
    ```

    Example, check if ifname@wan is the current gateway connection
    ```shell
    network@frame.gateway[ ifname@wan ]
    ttrue
    ```

+ `extup[]` **get the first extern interface that is currently up**
    - failed return NULL
    - succeed return [ json ], status information of the first extern interface that is up
    ```json
    {
        "ifname":"interface name",           // [ string ]
        "ifdev":"device name",               // [ string ]
        "netdev":"network device",           // [ string ]
        "ip":"IP address",                   // [ ip address ]
        "mask":"network mask",               // [ ip address ]
        "gw":"gateway",                      // [ ip address ]
        "dns":"DNS server",                  // [ ip address ]
        "dns2":"backup DNS"                  // [ ip address ]
    }
    ```


#### Control APIs

+ `register[ ifname, concom, ifdev, type ]` **register a network interface with the frame**
    - ifname ----------- [ string ], interface name, e.g. "ifname@lte"
    - concom ----------- [ string ], connection component name, e.g. "ifname@ethcon"
    - ifdev ------------ [ string ], optional, interface device name, e.g. "modem@lte"
    - type ------------- [ string ], optional, interface type: "local", "extern", "vpn", defaults to "vpn"
    - failed return tfalse
    - succeed return ttrue

    Example, register an extern LTE interface
    ```shell
    network@frame.register[ ifname@lte, ifname@ltecon, modem@lte, extern ]
    ttrue
    ```

+ `unregister[ ifname, type ]` **unregister a network interface from the frame**
    - ifname ----------- [ string ], interface name
    - type ------------- [ string ], optional, interface type, defaults to "vpn"
    - failed return tfalse
    - succeed return ttrue

+ `add[ ifdev, netdev ]` **add a network device**
    - ifdev ------------ [ string ], interface device name
    - netdev ----------- [ string ], optional, network device name
    - failed return tfalse
    - succeed return ttrue
    - Tries VLAN add, then bridge add, then matches against local/extern interface lists and calls their setup

+ `delete[ ifdev ]` **delete a network device**
    - ifdev ------------ [ string ], interface device name
    - failed return tfalse
    - succeed return ttrue
    - Searches local then extern lists, calls shut on match; falls back to bridge/vlan delete

+ `online[ info ]` **IPv4 online notification**
    - info ------------- [ json ], connection information
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface comes online (IPv4)
    ```json
    {
        "ifname":"interface name",         // [ string ]
        "ifdev":"device name",             // [ string ]
        "netdev":"network device",          // [ string ]
        "ip":"IP address",                 // [ ip address ]
        "mask":"network mask",             // [ ip address ]
        "gw":"gateway",                    // [ ip address ]
        "dns":"DNS server",                // [ ip address ]
        "dns2":"backup DNS"                // [ ip address ]
    }
    ```

+ `offline[ ifname ]` **IPv4 offline notification**
    - ifname ----------- [ string ], interface name
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface goes offline (IPv4)

+ `upline[ info ]` **IPv6 online notification**
    - info ------------- [ json ], connection information (`mode6`, `ifname`, `netdev`, `gw6`, `dns6`, `dns62`, `prefix`, …)
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname / odhcp6c when an uplink has usable IPv6 (GUA and/or IA_PD)
    - unchanged payload is ignored (no joint re-fire); IPv4 `online` still records `ontime` / `livetime`, IPv6 upline does not

+ `downline[ ifname ]` **IPv6 offline notification**
    - ifname ----------- [ string ], interface name
    - failed return tfalse
    - succeed return ttrue
    - Called by ifname components when an interface loses IPv6 (clears uplink IPv6 state and publishes downline joints)



### Published Joint Events

The following joint events are published when network interface state changes. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `network/on` | Sent when a local interface (LAN) comes up with IPv4 connectivity. Triggered after the interface obtains an IP address and is ready for local network communication. |
| `network/off` | Sent when a local interface (LAN) goes down or loses IPv4 connectivity. Triggered when the interface is disabled or the connection is lost. |
| `network/up` | Sent when a local interface (LAN) comes up with IPv6 connectivity. Triggered after the interface obtains an IPv6 address. |
| `network/down` | Sent when a local interface (LAN) goes down or loses IPv6 connectivity. |
| `network/onextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv4 connectivity. Triggered after successful connection establishment to the internet service provider. This event precedes `network/online` for external connections. |
| `network/offextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv4 connectivity. Triggered when the ISP connection is lost. |
| `network/upextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv6 connectivity. |
| `network/downextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv6 connectivity. |
| `network/onvpn` | Sent when a VPN interface comes up with IPv4 connectivity. Triggered after the VPN tunnel is successfully established. |
| `network/offvpn` | Sent when a VPN interface goes down or loses IPv4 connectivity. Triggered when the VPN tunnel is closed or interrupted. |
| `network/upvpn` | Sent when a VPN interface comes up with IPv6 connectivity. |
| `network/downvpn` | Sent when a VPN interface goes down or loses IPv6 connectivity. |
| `network/online` | Sent when the system establishes a default route to the internet (IPv4). Triggered after an external interface comes up and the routing table is updated. This indicates the device has full internet access. |
| `network/offline` | Sent when the system loses its default route to the internet (IPv4). Triggered when all external interfaces are down or when the primary connection fails. |
| `network/upline` | Sent when the system establishes IPv6 internet connectivity via an external interface or VPN. |
| `network/downline` | Sent when the system loses IPv6 internet connectivity. |
