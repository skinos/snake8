# Device Management from the Command Line (eline)

This book is the operator guide for managing a Skinos / landos gateway from **eline**. After Telnet, SSH, or serial login you see the **`$ `** prompt. Type HE commands **directly** — the same strings the web UI sends.

A given product only shows the menus whose interfaces and packages are present. This book documents the **full** web menu surface. If a component is missing on your unit, the matching command returns empty.

---

## Preface — HE on eline

### Open a session

- **Telnet** — enable Telnet Server under System, then connect with a client.
- **SSH** — enable SSH Server under System, then connect with a client.
- **UART** — set the serial port to **Command Line** under Application (typical 57600 8N1).

After login the prompt is **`$ `**. If `/etc/banner.he` exists, a short help banner is printed first:

```
 @ ----------------------- List all the component
 <com> ------------------- Show component configure
 <com>. ------------------ List all component interface
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
```

Leave eline with `exit` or Ctrl+D at `$ `.

### Command format

| Goal | Format | Example |
|------|--------|---------|
| Query all configuration | `component` | `land@machine` |
| Query one attribute | `component:attr/path` | `land@machine:name` |
| Replace full configuration | `component={json}` | `forward@alg={"ftp":"enable"}` |
| Set one attribute | `component:attr/path=value` | `land@machine:name=DemoGateway` |
| Merge several attributes | `component\|{json}` or `component:path\|{json}` | `land@syslog\|{"level":"info"}` |
| Clear one attribute | `component:attr/path=` | `gnss@nmea:client=` |
| Call a method | `component.method` | `land@machine.status` |
| Call a method with arguments | `component.method[p1,p2]` | `clock@date.ntpsync[ntp1.aliyun.com]` |
| One field from method JSON | `component.method:path` | `land@machine.status:version` |

```
$ land@machine
$ land@machine:name
$ land@machine:name=DemoGateway
$ land@machine.status
$ land@machine.status:version
```

### Returns

| Output | Meaning |
|--------|---------|
| JSON `{...}` | Structured data |
| Plain string | Scalar value |
| `ttrue` / `tfalse` | Success / failure |
| Empty | No data |
| `terror` / `tpanic` | Error / panic |

### Discovery

```
$ @
{ ... }
$ land@machine.
{ ... }
```

`@` lists components. `component.` lists that object’s methods.

### Interactive `set` mode

```
$ set land@machine
land@machine: name=DemoGateway
land@machine: g
land@machine: s
```

After `set <object>`, paths are relative. `g` reprints in-memory JSON, `s` saves, `e` (or Ctrl+D) abandons.

### How this maps to the web UI

Each web page loads and saves the same HE strings. `he.load(["ifname@lan"])` in the browser is `$ ifname@lan` here. `he.exec(["ifname@lan="+JSON.stringify(cfg)])` is `$ ifname@lan={...}`.

Multi-instance objects (`ifname@lte2`, `wifi@a`, `uart@tty3`, …) use the same keys as the first instance. Type the object name shown in the page URL (`?object=`).

---

## 1 Overview

Web menu: **Dashboard**. The page polls the network frame, Ethernet ports, and Wi-Fi SSIDs, and offers Connect / Disconnect on uplinks.

### Query the network frame

```
$ network@frame
{
    "type": "hot",
    "1": "ifname@wan",
    "2": "ifname@lte",
    "3": "ifname@lte2",
    "4": "ifname@wisp",
    "custom_dns": "disable"
}
```

| Field | Meaning |
|-------|---------|
| `type` | Multi-uplink policy: `cold`, `hot`, `hot2`–`hot5`, `lazy`, `lazy2`–`lazy5`, `dbdc`, … |
| `1` … `10` | Priority slots → ifname objects |
| `custom_dns` | `enable` / `disable` — override DNS when enabled |

### Query uplink and LAN live status

The Dashboard charts come from `network@frame.extern` (uplinks) and `network@frame.local` (LAN).

```
$ network@frame.extern
{
    "ifname@wan": {
        "status": "up",
        "ip": "192.168.10.1",
        "gw": "192.168.10.254"
    }
}
```

```
$ network@frame.local
{
    "ifname@lan": {
        "status": "up",
        "ip": "192.168.1.1",
        "mask": "255.255.255.0"
    }
}
```

| Field | Meaning |
|-------|---------|
| `status` | Extern: `nodevice` / `reset` / `setup` / `register` / `uping` / `scanning` / `block` / `up` / `failed` / `down`. Local: `uping` / `down` / `up` |
| `ip` / `mask` / `gw` | Current addressing |
| `rx_bytes` / `tx_bytes` | Counters used by the Overview and Traffic charts |

Which uplink is the default route:

```
$ network@frame.status
{
    "ifname@lte": {
        "status": "up",
        "inuse": "enable"
    },
    "ifname@lte2": {
        "status": "down",
        "inuse": "disable"
    }
}
```

| Field | Meaning |
|-------|---------|
| `status` | Link state of that uplink |
| `inuse` | `enable` = currently used as default (cold/hot/lazy) |
| `balance` | Present in dbdc mode when the uplink is in the ECMP set |

```
$ network@frame.status:ifname@lte/status
up
```

### Ethernet ports

```
$ arch@ethernet.status
```

Per-port link / driver status (shape depends on the switch). The Dashboard uses this together with `arch@ethernet` (port map).

```
$ arch@ethernet
{
    "default": {
        "mode": "switch",
        "ifdev": {
            "lan1": "lan1",
            "lan2": "lan2",
            "lan3": "lan3",
            "lan4": "wan"
        }
    }
}
```

### Wi-Fi SSID status (Overview radios)

```
$ wifi@nssid.status
{
    "status": "up",
    "secure": "wpapskwpa2psk",
    "rx_bytes": "767164641",
    "tx_bytes": "369735875",
    "mac": "00:03:7F:12:88:70",
    "ssid": "dimmalex-home",
    "bssid": "00:03:7F:12:88:70",
    "channel": "11"
}
```

`wifi@assid.status` is the 5.8G SSID (same fields).

| Field | Meaning |
|-------|---------|
| `status` | `nodevice` / `up` / `down` |
| `ssid` / `channel` / `mac` | AP identity |
| `rx_bytes` / `tx_bytes` | Traffic counters |

```
$ wifi@nssid.status:ssid
dimmalex-home
```

### Connect / Disconnect (same as Dashboard buttons)

```
$ ifname@lte.setup
ttrue
$ ifname@lte.shut
ttrue
```

Replace `lte` with `lte2`–`lte4`, `wan` / `wan2`, or `wisp` / `wisp2`. The web UI also calls `wui@admin.ttyd_kill` before LTE setup so a web terminal does not hold the AT port.

---

## 2 Performance

Web menu: **Utilization**. The page polls CPU, memory, and storage.

### Query CPU (`land@machine.cpuinfo`)

Values are jiffies from `/proc/stat`. The page computes usage from two samples: `(Δtotal − Δidle) / Δtotal`.

```
$ land@machine.cpuinfo
{
    "cpu": {
        "user": "12345",
        "nice": "0",
        "system": "6789",
        "idle": "987654",
        "iowait": "123",
        "irq": "45",
        "softirq": "67"
    }
}
```

| Field | Meaning |
|-------|---------|
| `cpu` | Aggregate of all cores; `cpu0` … appear when SMP |
| `user` / `nice` / `system` / `idle` | User, nice, kernel, idle time (jiffies) |
| `iowait` / `irq` / `softirq` | I/O wait and interrupt time |

```
$ land@machine.cpuinfo:cpu/idle
987654
```

### Query memory (`land@machine.meminfo`, kB)

```
$ land@machine.meminfo
{
    "total": "524288",
    "free": "262144",
    "buffers": "16384",
    "cached": "131072",
    "available": "409600"
}
```

| Field | Meaning |
|-------|---------|
| `total` | MemTotal |
| `free` | MemFree |
| `buffers` / `cached` | Buffer / page cache |
| `available` | MemAvailable, or `free+buffers+cached` on old kernels. UI used = `total − available` |

```
$ land@machine.meminfo:available
409600
```

### Query storage (`land@machine.sginfo`)

```
$ land@machine.sginfo
{
    "config": {
        "path": "/skinos/cfg",
        "size": "16M",
        "used": "4M",
        "available": "12M",
        "use": "25%"
    }
}
```

| Field | Meaning |
|-------|---------|
| key (`config`, `sd0`, …) | Volume name |
| `path` | Mount path |
| `size` / `used` / `available` | Capacity with unit |
| `use` | Used percent |

```
$ land@machine.sginfo:config/use
25%
```

---

## 3 Traffic

Web menu: **Traffic**. The page charts rx/tx from the same frame maps as Overview, plus the client list.

### Interface counters

```
$ network@frame.extern
$ network@frame.local
$ wifi@nssid.status
$ wifi@assid.status
```

The charts use `rx_bytes` / `tx_bytes` on each ifname and SSID object (see Overview). Poll twice and subtract to get a rate.

### Connected stations (Traffic client table)

```
$ client@station.list
{
    "04:CF:8C:39:91:7A": {
        "name": "xiaomi-aircondition",
        "ip": "192.168.31.140",
        "ifname": "ifname@lan"
    },
    "40:31:3C:B5:6D:4C": {
        "ip": "192.168.31.61",
        "livetime": "14:39:34:1"
    }
}
```

| Field | Meaning |
|-------|---------|
| key | Client MAC |
| `name` | Hostname or saved display name |
| `ip` | Current IPv4 |
| `ifname` | Logical LAN |
| `livetime` | Online time `hour:minute:second:day` |

```
$ wifi@n.stalist
{
    "78:11:DC:92:D3:9E": {
        "livetime": "14:53:17:2",
        "rssi": "-52"
    }
}
```

`wifi@a.stalist` is the 5.8G radio STA list (same fields: `livetime`, `rssi` in dBm).

```
$ client@station.list:04:CF:8C:39:91:7A/ip
192.168.31.140
```

---

## 4 Network

### 4.1 Connection

Web menu: **Network → Connection**. Page depends on `land@machine:mode` (`mix`, `dwisp`, `dmisp`, `mwm`, …). Component: `network@frame`.

The page edits multi-uplink policy: which interfaces are in the scheduler and how they fail over.

**Configuration**

| Key | Meaning |
|-----|---------|
| `type` | `cold` / `hot` / `hot2`–`hot5` / `lazy` / `lazy2`–`lazy5` / `dbdc` / `dbdc2`–`dbdc6` |
| `1` … `10` | Priority slots → `ifname@wan`, `ifname@lte`, … |
| `custom_dns` / `dns` / `dns2` | Override DNS when `custom_dns` is `enable` |
| `interval` | Scheduler poll interval (seconds) |
| `offload` | `disable` / `enable` / `hw` flow offload |

**Query configuration**

```
$ network@frame
{
    "type": "hot",
    "1": "ifname@wan",
    "2": "ifname@lte",
    "custom_dns": "disable"
}
```

List candidate uplinks (the page combo boxes):

```
$ network@frame.list[extern]
{
    "ifname@lte": "modem@lte",
    "ifname@wan": "ethernet@lan1"
}
```

**Query status** — see Overview (`network@frame.status`, `.extern`).

**Change configuration** (Apply)

```
$ network@frame:type=hot
ttrue
$ network@frame|{"type":"hot","1":"ifname@wan","2":"ifname@lte"}
ttrue
$ network@frame={"type":"hot","1":"ifname@wan","2":"ifname@lte","custom_dns":"disable"}
ttrue
```

---

### 4.2 LTE/NR

Web menu: **Network → LTE/NR** (`ifname@lte`). LTE/NR2–4 use `ifname@lte2` … `ifname@lte4` and `modem@lte2` … with the same keys. Tabs also load `modem@lte` (SIM, SMS, AT, IMEI/IMSI lock).

The page manages the cellular uplink: APN, SIM, addressing, keeplive, and modem extras (SMS/GNSS/AT).

**Configuration** (keys the LTE page reads and writes)

| Key | Meaning |
|-----|---------|
| `status` | Auto-setup at boot |
| `pin` | SIM PIN |
| `profile` / `profile_cfg` | Custom APN: `dial`, `apn`, `user`, `passwd`, `type` (`ipv4`/`ipv6`/`ipv4v6`), `auth` |
| `bsim` / `bsim_cfg` | Backup SIM |
| `mode` | IPv4: `dhcpc` / `static` / `ppp` |
| `masq` | NAT masquerade |
| `keeplive` | Link check: `icmp` / `dns` / `recv` / `auto` |
| `sms` / `gnss` / `atport` | Forwarded to `modem@lte` |
| `lock_nettype` | `auto` / `2g` / `3g` / `4g` / `nsa` / `sa` |
| `lock_imei` / `lock_imsi` | Identity lock |
| `custom_set` / `custom_watch` | AT commands at setup / watch |
| `watch_interval` | Modem watch period (seconds) |

**Query configuration**

```
$ ifname@lte
{
    "profile": "enable",
    "profile_cfg": {
        "dial": "*99#",
        "type": "ipv4v6",
        "apn": "internet",
        "user": "card",
        "passwd": "card"
    },
    "mode": "ppp",
    "ppp": {
        "lcp_echo_interval": "10",
        "lcp_echo_failure": "12"
    },
    "masq": "enable",
    "method": "slaac",
    "keeplive": {
        "type": "recv",
        "recv": {
            "timeout": "20",
            "failed": "30",
            "packets": "1"
        }
    },
    "sms": "enable",
    "gnss": "enable",
    "atport": "enable"
}
```

```
$ ifname@lte:profile_cfg/apn
internet
```

**Query status** (`ifname@lte.status`)

```
$ ifname@lte.status
{
    "mode": "dhcpc",
    "netdev": "usb1",
    "gw": "10.84.136.246",
    "dns": "120.80.80.80",
    "dns2": "221.5.88.88",
    "ifdev": "modem@lte",
    "ontime": "28826",
    "status": "up",
    "delay": "26",
    "ip": "10.84.136.245",
    "mask": "255.255.255.252",
    "livetime": "00:31:58:0",
    "rx_bytes": "4407784",
    "rx_packets": "34234",
    "tx_bytes": "4440236",
    "tx_packets": "47893",
    "mac": "02:50:F4:00:00:00",
    "imei": "868186042111714",
    "ci": "4A37D91",
    "lac": "25E3",
    "plmn": "46001",
    "csq": "23",
    "nettype": "FDD LTE",
    "rsrp": "-97",
    "rssi": "-66",
    "rsrq": "-9",
    "sinr": "-18",
    "band": "LTE BAND 1",
    "channel": "100",
    "signal": "4",
    "operator": "China Unicom",
    "imsi": "460018708133639",
    "iccid": "8986012580155265717",
    "name": "Quectel-EC2X"
}
```

| Field | Meaning |
|-------|---------|
| `status` | `nodevice` / `reset` / `setup` / `register` / `idle` / `noimsi` / `noimei` / `uping` / `block` / `up` / `failed` / `down` |
| `mode` | Current IPv4 mode |
| `ip` / `mask` / `gw` / `dns` | PDP address |
| `imei` / `imsi` / `iccid` | SIM / module identity (`iccid` may be `nosim` / `pin` / `puk`) |
| `plmn` | MCC+MNC, or `noreg` / `unreg` / `dereg` |
| `operator` / `nettype` / `band` | Serving cell |
| `signal` | `0`–`4` bars |
| `rssi` / `rsrp` / `rsrq` / `sinr` / `csq` | Radio metrics (dBm where applicable) |
| `livetime` | Online time `hour:minute:second:day` |
| `rx_bytes` / `tx_bytes` | Traffic |

```
$ ifname@lte.status:ip
10.84.136.245
$ ifname@lte.status:signal
4
$ ifname@lte.ifdev
modem@lte
$ ifname@lte.operator
{
    "operator": "China Unicom",
    "apn": "internet"
}
```

**Change configuration**

```
$ ifname@lte:mode=ppp
ttrue
$ ifname@lte:sms=enable
ttrue
$ ifname@lte|{"profile":"enable","profile_cfg":{"apn":"NewAPN"}}
ttrue
```

Replace the whole object (Apply):

```
$ ifname@lte={"mode":"ppp","masq":"enable","profile":"enable","profile_cfg":{"apn":"internet","dial":"*99#"}}
ttrue
```

**Connect / Disconnect**

```
$ ifname@lte.setup
ttrue
$ ifname@lte.shut
ttrue
```

**IMEI / IMSI lock** (Modem tab)

```
$ ifname@lte.lock_imei[868186042111714]
ttrue
$ ifname@lte.lock_imsi[460018708133639]
ttrue
```

**SMS** (SMS tab; `modem@lte` or `ifname@lte.smslist`)

```
$ ifname@lte.smslist
$ modem@lte.sms_list
$ modem@lte.smsdel[sms2.X54gbI]
```

---

### 4.3 WAN

Web menu: **Network → WAN**. Component: `ifname@wan` (WAN2–4: `ifname@wan2` …). Ethernet uplink: static / DHCP / PPPoE, NAT, keeplive.

**Configuration**

| Key | Meaning |
|-----|---------|
| `status` | Auto-setup at boot |
| `mac` | Clone MAC |
| `mode` | `dhcpc` / `static` / `pppoec` |
| `static/ip` `static/mask` `static/gw` `static/dns` | Static IPv4 |
| `pppoec/username` `pppoec/password` | PPPoE |
| `masq` | NAT masquerade |
| `mtu` | MTU (bytes) |
| `method` | IPv6: `disable` / `manual` / `automatic` / `slaac` |
| `keeplive` | `icmp` / `dns` / `recv` / `auto` |

**Query configuration**

```
$ ifname@wan
{
    "mac": "88:12:4E:23:43:12",
    "mode": "pppoec",
    "pppoec": {
        "username": "1923221@gd.com",
        "password": "FDAED13E"
    },
    "masq": "enable",
    "method": "slaac"
}
```

**Query status**

```
$ ifname@wan.status
{
    "status": "up",
    "mode": "static",
    "netdev": "wan",
    "gw": "192.168.10.254",
    "dns": "114.114.114.114",
    "ip": "192.168.10.1",
    "mask": "255.255.255.0",
    "livetime": "01:15:50:0",
    "rx_bytes": "1256",
    "tx_bytes": "1320",
    "mac": "02:50:F4:00:00:00"
}
```

| Field | Meaning |
|-------|---------|
| `status` | `nodevice` / `uping` / `down` / `up` / `failed` / `block` |
| `mode` | Current IPv4 mode |
| `ip` / `mask` / `gw` / `dns` | Current address |
| `livetime` | Online time `hour:minute:second:day` |
| `rx_bytes` / `tx_bytes` | Traffic |

```
$ ifname@wan.status:ip
192.168.10.1
```

**Change configuration**

```
$ ifname@wan:mode=dhcpc
ttrue
$ ifname@wan:static/ip=192.168.10.2
ttrue
$ ifname@wan={"mode":"static","static":{"ip":"192.168.10.1","mask":"255.255.255.0","gw":"192.168.10.254"},"masq":"enable"}
ttrue
```

**Connect / Disconnect**

```
$ ifname@wan.setup
ttrue
$ ifname@wan.shut
ttrue
```

---

### 4.4 WISP

Web menu: **Network → WISP(2.4G)** / **WISP(5.8G)**. Components: `ifname@wisp` / `ifname@wisp2`. Wireless station uplink plus the same IP stack as WAN.

**Configuration**

| Key | Meaning |
|-----|---------|
| `peer` / `peermac` | SSID / BSSID to join |
| `channel` | Channel; `0` = auto |
| `secure` | `disable` / `wpapsk` / `wpa2psk` / `wpapskwpa2psk` |
| `wpa_encrypt` / `wpa_key` | AES/TKIP and PSK |
| `mode` / `static` / `dhcpc` / `pppoec` / `masq` / `keeplive` | Same as WAN |

**Query configuration**

```
$ ifname@wisp
{
    "peer": "V520-D21D20",
    "secure": "wpapsk",
    "wpa_encrypt": "aes",
    "wpa_key": "87654321",
    "mode": "dhcpc",
    "masq": "enable"
}
```

Channel and security lists (page combo boxes):

```
$ ifname@wisp.chlist
$ ifname@wisp.securelist
```

**Query status**

```
$ ifname@wisp.status
```

Same IP fields as WAN, plus:

| Field | Meaning |
|-------|---------|
| `peer` / `peermac` | Associated AP |
| `channel` / `rate` | RF |
| `rssi` / `signal` | Signal (dBm / bars) |

**Scan APs**

```
$ ifname@wisp.aplist
```

**Change configuration / Connect**

```
$ ifname@wisp:peer=Office-AP
ttrue
$ ifname@wisp:wpa_key=87654321
ttrue
$ ifname@wisp.setup
ttrue
$ ifname@wisp.shut
ttrue
```

---

### 4.5 LAN

Web menu: **Network → LAN**. Component: `ifname@lan` (LAN2–LAN4: `ifname@lan2` …). On parasite (non-router) products the same object is bound to the **lwan** page.

The LAN page manages the local interface: IPv4/IPv6 addressing and the DHCP server that assigns addresses to clients. It does not apply NAT.

**Configuration** (keys the LAN page reads and writes)

| Key | Meaning |
|-----|---------|
| `status` | `enable` / `disable` — start the LAN at boot |
| `mode` | IPv4 mode: `static` or `dhcpc` |
| `static/ip`, `static/mask` | LAN IPv4 address and netmask |
| `dhcps/status` | DHCP server on/off |
| `dhcps/startip`, `dhcps/endip` | IPv4 pool |
| `dhcps/lease` | Lease time in seconds |
| `dhcps/gw`, `dhcps/dns`, `dhcps/dns2` | Gateway and DNS given to clients (empty = use LAN IP) |
| `method` | IPv6: `disable` / `manual` / `automatic` |
| `dhcpsv6` / `addrpool` | DHCPv6 pool (when the page shows IPv6 server fields) |

**Query configuration**

```
$ ifname@lan
{
    "mode": "static",
    "static": {
        "ip": "192.168.1.1",
        "mask": "255.255.255.0"
    },
    "method": "automatic",
    "dhcps": {
        "status": "enable",
        "startip": "192.168.1.2",
        "endip": "192.168.1.100",
        "mask": "255.255.255.0",
        "lease": "86400",
        "gw": "",
        "dns": ""
    }
}
```

| Field | What the page shows |
|-------|---------------------|
| `mode` | IPv4 addressing mode |
| `static.ip` / `static.mask` | LAN address |
| `dhcps.status` | DHCP server switch |
| `dhcps.startip`–`endip` | Address pool |
| empty `gw` / `dns` | Clients receive the LAN IP as gateway/DNS |

```
$ ifname@lan:static/ip
192.168.1.1
```

**Query status** (`ifname@lan.status`)

```
$ ifname@lan.status
{
    "status": "up",
    "mode": "static",
    "netdev": "lan",
    "ifdev": "vlan@lan",
    "ip": "192.168.1.1",
    "mask": "255.255.255.0",
    "gw": "192.168.1.1",
    "dns": "192.168.1.1",
    "livetime": "01:15:50:0",
    "ontime": "3600",
    "rx_bytes": "1256",
    "rx_packets": "4",
    "tx_bytes": "1320",
    "tx_packets": "4",
    "mac": "02:50:F4:00:00:00",
    "method": "slaac",
    "addr": "fe80::50:f4ff:fe00:0"
}
```

| Field | Meaning |
|-------|---------|
| `status` | Link: `nodevice` (no device), `uping` (connecting), `down`, `up` |
| `mode` | Current IPv4 mode: `static` or `dhcpc` |
| `netdev` | Kernel netdev name (e.g. `lan`) |
| `ifdev` | Underlying ifdev component (optional) |
| `ip` / `mask` | Current IPv4 address and netmask |
| `gw` / `dns` / `dns2` | Current gateway and DNS (optional) |
| `livetime` | Online time as `hour:minute:second:day` |
| `ontime` | Online uptime string (optional) |
| `rx_bytes` / `rx_packets` | Received traffic |
| `tx_bytes` / `tx_packets` | Sent traffic |
| `mac` | Interface MAC |
| `method` | IPv6 mode when enabled: `manual` / `automatic` / `slaac` |
| `addr` / `addr2` / `addr3` | IPv6 addresses (optional) |

```
$ ifname@lan.status:status
up
$ ifname@lan.status:ip
192.168.1.1
```

**Change configuration**

```
$ ifname@lan:static/ip=192.168.2.1
ttrue
$ ifname@lan:dhcps/status=disable
ttrue
$ ifname@lan:dhcps|{"startip":"192.168.2.100","endip":"192.168.2.200"}
ttrue
```

Replace the whole object (Apply):

```
$ ifname@lan={"mode":"static","static":{"ip":"192.168.8.1","mask":"255.255.255.0"},"dhcps":{"status":"enable","startip":"192.168.8.2","endip":"192.168.8.100","lease":"86400"}}
ttrue
```

Changing the LAN IP typically requires a restart (`land@machine.restart`).

---

### 4.6 Hosts

On non-router products this page sits under **Network**; on routers it sits under **Route**. Component: `network@hosts`. Map of hostname → IPv4.

**Configuration** — object keys are hostnames, values are IP strings.

```
$ network@hosts
{
    "printer": "192.168.8.50",
    "nvr": "192.168.8.60"
}
```

| Field | Meaning |
|-------|---------|
| key | Hostname |
| value | IPv4 address |

```
$ network@hosts:printer
192.168.8.50
```

There is no `.status` API. Apply replaces the whole map:

```
$ network@hosts={"printer":"192.168.8.50","nvr":"192.168.8.60"}
ttrue
$ network@hosts:printer=192.168.8.51
ttrue
```


---

## 5 Route

### 5.1 Hosts

See [4.6 Hosts](#46-hosts). On router products the same `network@hosts` page is under **Route**.

### 5.2 Route Table

Web menu: **Route → Route Table**. Component: `forward@main`. Prefer `.add` / `.delete` over editing raw config.

**Configuration** — named routes (`target`, `mask`, `gw`, `metric`, `ifname`).

```
$ forward@main
{
    "myCustomRule1": {
        "target": "192.168.1.0",
        "mask": "255.255.255.0",
        "gw": "192.168.8.22",
        "metric": "2",
        "ifname": "ifname@wan"
    }
}
```

**Query status** (live kernel table; names starting with `~` are system routes)

```
$ forward@main.status
{
    "myCustomRoute1": {
        "target": "192.168.0.0",
        "mask": "255.255.255.0",
        "gw": "192.168.8.2",
        "ifname": "ifname@wan",
        "netdev": "eth0.2",
        "flags": 1,
        "metric": 2,
        "ref": 0,
        "use": 0,
        "status": "up"
    },
    "~auto1": {
        "target": "127.0.0.1",
        "mask": "255.255.255.0",
        "gw": "0.0.0.0",
        "netdev": "lo",
        "status": "up"
    }
}
```

| Field | Meaning |
|-------|---------|
| `target` / `mask` / `gw` | Destination and next hop |
| `ifname` | Logical output interface |
| `netdev` | Kernel device |
| `metric` | Hop cost |
| `status` | `up` / `down` |

```
$ forward@main.status:myCustomRoute1/status
up
```

**Add / delete**

```
$ forward@main.add[office1,192.168.2.12,255.255.255.0,192.168.9.40,ifname@lan]
ttrue
$ forward@main.delete[office1]
ttrue
```

### 5.3 Custom Route

Web menu: **Route → Custom Route**. List object: `forward@routes`. Each table is `routes@<tid>`.

```
$ forward@routes.list
$ forward@routes.add[123]
ttrue
$ routes@123.status
$ routes@123.add[office,192.168.2.0,255.255.255.0,192.168.8.1,ifname@wan,10]
ttrue
$ forward@routes.delete[123]
ttrue
```

Per-table status fields match `forward@main.status` (plus optional `src` / `type` / `onlink` / `mtu` on custom tables).

### 5.4 Routing Rule

Web menu: **Route → Routing Rule**. Component: `forward@rule`. Policy routing (`ip rule`).

**Configuration**

| Key | Meaning |
|-----|---------|
| `pref` | Priority (default 40000; smaller is higher) |
| `markid` | Match packet mark (use 100+) |
| `srcifname` | Source logical ifname |
| `src` / `srcmask` | Source network |
| `tid` | Route table id (use 100+; avoid 253–255) |

```
$ forward@rule.status
$ forward@rule.tidlist
$ forward@rule.add[lan2wan,192.168.8.0,255.255.255.0,ifname@lan,100,120,40000]
ttrue
$ forward@rule.delete[lan2wan]
ttrue
```

### 5.5 Packet Mark

Web menu: **Route → Packet Mark**. Component: `forward@mark`. Mark id &lt; 100 is reserved.

```
$ forward@mark.status
$ forward@mark.add[myCustom1,300,192.168.8.250,202.94.22.38,tcp,,]
ttrue
$ forward@mark.delete[myCustom1]
ttrue
```

Status JSON: per-rule `markid`, `src`, `dest`, `protocol`, `srcport`, `destport`, `status`.

### 5.6 Firewall

Web menu: **Route → Firewall**. Component: `forward@firewall`. Per-uplink filter. The page also loads `network@frame.list[outer]`.

**Configuration**

| Key | Meaning |
|-----|---------|
| `status` | Firewall on/off for that ifname |
| `default` | `drop` / `accept` |
| `icmp_access` / `telnet_access` / `ssh_access` / `wui_access` | Service access from WAN |
| `nat_through` | Honor `forward@nat` mappings |
| `rule.<name>` | Custom `action` / `src` / `protocol` / `dest` / `destport` |

```
$ forward@firewall
{
    "ifname@lte": {
        "status": "enable",
        "default": "drop",
        "rule": {
            "pcweb": {
                "action": "accept",
                "dest": "192.168.8.222",
                "protocol": "tcp",
                "destport": "80"
            }
        }
    }
}
```

No `.status` API — live policy is the saved config. Apply:

```
$ forward@firewall:ifname@lte/status=enable
ttrue
$ forward@firewall={"ifname@lte":{"status":"enable","default":"drop","wui_access":"enable"}}
ttrue
```

### 5.7 Port Map

Web menu: **Route → Port Map**. Component: `forward@nat`. WAN/LTE port forward + DMZ. Page lists `network@frame.list[outer]`.

```
$ forward@nat
{
    "ifname@wan": {
        "mode": "dnat",
        "rule": {
            "rdesktop": {
                "targetport": "3389",
                "protocol": "tcp",
                "destip": "192.168.31.250",
                "destport": ""
            }
        }
    }
}
```

| Field | Meaning |
|-------|---------|
| `mode` | `forward` (bi-directional) / `dnat` |
| `dmzhost` | All remaining ports to this LAN IP |
| `targetport` | External port |
| `destip` / `destport` | Internal host / port |

```
$ forward@nat={"ifname@wan":{"mode":"dnat","rule":{"rdesktop":{"targetport":"3389","protocol":"tcp","destip":"192.168.31.250"}}}}
ttrue
```

### 5.8 Port Proxy

Web menu: **Route → Port Proxy**. Component: `forward@dnat`. LAN-side proxy to an Internet host. Page lists `network@frame.list[local]`.

```
$ forward@dnat
{
    "ifname@lan": {
        "proxy1": {
            "destip": "29.23.11.35",
            "destport": "28-90",
            "protocol": "tcpudp",
            "targetport": "100"
        }
    }
}
```

```
$ forward@dnat={"ifname@lan":{"proxy1":{"destip":"29.23.11.35","destport":"80","protocol":"tcp","targetport":"8080"}}}
ttrue
```

### 5.9 Time to Live

Web menu: **Route → Time to Live(TTL)**. Component: `forward@ttl`.

```
$ forward@ttl
{
    "ifname@lan": {
        "mode": "fix",
        "fix": "99"
    }
}
```

| Field | Meaning |
|-------|---------|
| `mode` | `disable` / `fix` / `inc` / `dec` |
| `fix` / `inc` / `dec` | TTL value or delta |

```
$ forward@ttl:ifname@lan|{"mode":"fix","fix":"64"}
ttrue
```

### 5.10 ALG

Web menu: **Route → ALG**. Component: `forward@alg`. Each key is a helper (`ftp`, `sip`, `pptp`, `gre`, …) set to `enable` / `disable`.

```
$ forward@alg
{
    "ftp": "enable",
    "pptp": "enable",
    "gre": "enable",
    "sip": "enable",
    "tftp": "disable"
}
```

```
$ forward@alg:ftp=disable
ttrue
$ forward@alg={"ftp":"enable","sip":"enable"}
ttrue
```

---

## 6 Wireless

SSID pages bind `wifi@n` (2.4G radio) + `wifi@nssid` (AP). 5.8G uses `wifi@a` / `wifi@assid` with the same keys. MSSID uses `wifi@nssid2` / `wifi@assid2`. ApClient uses `wifi@nsta` / `wifi@asta` (hidden in WISP modes).

### 6.1 2.4G SSID

Web menu: **Wireless → 2.4G SSID**. Objects: `wifi@n` (radio) and `wifi@nssid` (AP).

**Radio configuration (`wifi@n`)**

| Key | Meaning |
|-----|---------|
| `status` | Radio on/off |
| `mode` | `bg` / `n` / `2g` (AX) |
| `bandwidth` | `20` / `40` |
| `channel` | 0–14; `0` = auto |
| `country` | Country code (`cn`, `us`, …) |
| `txpower` | Transmit power |
| `ldpc` / `shortgi` / `stbc` | PHY features |

```
$ wifi@n
{
    "mode": "n",
    "bandwidth": "40",
    "channel": "11",
    "beacon": "100",
    "ldpc": "enable",
    "shortgi": "enable",
    "stbc": "enable"
}
```

Lists used by the page:

```
$ wifi@n.modelist
$ wifi@n.powerlist
$ wifi@n.securelist
$ wifi@n.acs
$ wifi@n.chlist[n,cn]
```

**SSID configuration (`wifi@nssid`)**

| Key | Meaning |
|-----|---------|
| `status` | AP on/off |
| `ssid` / `broadcast` / `isolated` | Name, beacon, client isolation |
| `secure` | `disable` / `wpapsk` / `wpa2psk` / `wpa3psk` / mix |
| `wpa_encrypt` / `wpa_key` | Cipher and PSK |
| `acl` | `disable` / `accept` / `drop` |
| `maxsta` / `wmm` | Capacity / WMM |

```
$ wifi@nssid
{
    "status": "enable",
    "ssid": "5228-test-2.4g",
    "secure": "wpapskwpa2psk",
    "wpa_key": "22222222",
    "acl": "accept",
    "maxsta": "64",
    "wmm": "enable"
}
```

**Query SSID status**

```
$ wifi@nssid.status
{
    "status": "up",
    "secure": "wpapskwpa2psk",
    "rx_bytes": "767164641",
    "tx_bytes": "369735875",
    "mac": "00:03:7F:12:88:70",
    "ssid": "dimmalex-home",
    "bssid": "00:03:7F:12:88:70",
    "channel": "11"
}
```

| Field | Meaning |
|-------|---------|
| `status` | `nodevice` / `up` / `down` |
| `ssid` / `bssid` / `channel` | AP identity |
| `rx_bytes` / `tx_bytes` | Traffic |

```
$ wifi@nssid.stalist
{
    "78:11:DC:92:D3:9E": {
        "livetime": "14:53:17:2",
        "rssi": "-52"
    }
}
```

Kick a client (Wake / stabeat on the radio):

```
$ wifi@n.stabeat[78:11:DC:92:D3:9E]
ttrue
```

**Change configuration** (Apply writes radio + SSID)

```
$ wifi@n:channel=6
ttrue
$ wifi@nssid:ssid=Office-2G
ttrue
$ wifi@nssid:wpa_key=87654321
ttrue
```

### 6.2 2.4G MSSID

Web menu: **Wireless → 2.4G MSSID**. Objects: `wifi@nssid2`, `wifi@nssid3` (same keys as `wifi@nssid`). Loads `wifi@n.securelist`.

```
$ wifi@nssid2
$ wifi@nssid2={"status":"enable","ssid":"Guest","secure":"wpa2psk","wpa_key":"guestpass"}
ttrue
```

### 6.3 2.4G ApClient

Web menu: **Wireless → 2.4G ApClient**. Component: `wifi@nsta`. Hidden when mode is `wisp` / `dwisp` / `mwm` (WISP page owns the station).

```
$ wifi@nsta
{
    "peer": "V520-D21D20",
    "secure": "wpapsk",
    "wpa_encrypt": "aes",
    "wpa_key": "87654321"
}
```

```
$ wifi@nsta.status
{
    "status": "up",
    "peer": "TP-link-2231",
    "peermac": "70:3A:D8:54:BC:90",
    "channel": "10",
    "rate": "270",
    "rssi": "-41",
    "signal": "3"
}
```

| Field | Meaning |
|-------|---------|
| `status` | Association state |
| `peer` / `peermac` / `channel` | AP |
| `rssi` / `signal` / `rate` | RF |

```
$ wifi@nsta.chlist
$ wifi@nsta.securelist
$ wifi@nsta.aplist
$ wifi@nsta.up
ttrue
$ wifi@nsta.down
ttrue
```

### 6.4 5.8G SSID / MSSID / ApClient

Same pages as 6.1–6.3 with objects `wifi@a`, `wifi@assid`, `wifi@assid2`, `wifi@asta`. ApClient is hidden in `nwisp` / `dwisp` / `mwm` modes.

```
$ wifi@a
$ wifi@assid.status
$ wifi@asta.status
```

---

## 7 Station

### 7.1 Station List

Web menu: **Station → Station List**. Component: `client@station`. Also polls `wifi@n.stalist` / `wifi@a.stalist`.

**Configuration** — MAC keys with optional `name`, `bindip`, `arpbind`, `lease`, `ifname`.

```
$ client@station
{
    "00:03:7F:22:43:2B": {
        "ifname": "ifname@lan",
        "name": "Office-Printer",
        "bindip": "192.168.31.100",
        "arpbind": "enable"
    }
}
```

**Query list** (live ARP + DHCP + saved names)

```
$ client@station.list
{
    "04:CF:8C:39:91:7A": {
        "name": "xiaomi-aircondition",
        "ip": "192.168.31.140",
        "ifname": "ifname@lan"
    },
    "40:31:3C:B5:6D:4C": {
        "ip": "192.168.31.61",
        "livetime": "14:39:34:1"
    }
}
```

| Field | Meaning |
|-------|---------|
| key | MAC |
| `name` | Display name |
| `ip` | Current IPv4 |
| `ifname` | LAN object |
| `livetime` | `hour:minute:second:day` |

```
$ client@station.add[00:03:7F:22:43:2B,Printer]
ttrue
$ client@station.delete[00:03:7F:22:43:2B]
ttrue
```

### 7.2 Access Control List

Web menu: **Station → Access Control List**. Component: `client@acl`. Page lists `network@frame.list[local]`.

```
$ client@acl
{
    "ifname@lan": {
        "status": "enable",
        "rule": {
            "dis163": {
                "proto": "domain",
                "action": "drop",
                "dest": "www.163.com",
                "timer": "enable",
                "timer_cfg": {
                    "timestart": "00:00:00",
                    "timestop": "23:59:59",
                    "weekdays": "1,2,3,4,5,6,7"
                }
            }
        }
    }
}
```

| Field | Meaning |
|-------|---------|
| `status` | ACL on/off per LAN |
| `action` | `drop` / `accept` / `return` |
| `proto` | `domain` / `tcp` / `udp` / `all` / `layer7` |
| `src` / `dest` / `destport` | Match |
| `timer_cfg` | Optional time window |

No `.status` API. Apply:

```
$ client@acl={"ifname@lan":{"status":"enable","rule":{"dis163":{"proto":"domain","action":"drop","dest":"www.163.com"}}}}
ttrue
```

---

## 8 System

### 8.1 Device

Web menu: **System → Device**. Components: `land@machine`, `clock@date`, `clock@ntps`. The header also preloads `land@machine.status`.

**Configuration (`land@machine`)**

| Key | Meaning |
|-----|---------|
| `name` | Hostname (Device page rename) |
| `mode` | Working mode (`ap`, `wisp`, `nwisp`, `gateway`, `dgateway`, `tgateway`, `qgateway`, `misp`, `nmisp`, `dmisp`, `mwm`, `mix`; `default` / `mbridge` disables LAN DHCP) |
| `sn` / `mac` / `macid` / `model` / `features` / `cmodel` / `oem` / `magic` / `datecode` | EEPROM, read-only on this page |
| `language` | UI language (`cn` / `en`) |
| `gpversion` / `cfgversion` | Version strings on Configure |

```
$ land@machine
{
    "name": "SkinOS",
    "mode": "default",
    "sn": "20240001",
    "mac": "AA:BB:CC:DD:EE:FF",
    "model": "R2000",
    "language": "cn",
    "gpversion": "1.0.0",
    "cfgversion": "1.0.0"
}
```

```
$ land@machine:name
SkinOS
```

**Query status (`land@machine.status`)**

```
$ land@machine.status
{
    "name": "SkinOS",
    "mode": "default",
    "platform": "rk3568",
    "hardware": "R2000",
    "custom": "default",
    "scope": "product",
    "version": "8.0.0",
    "livetime": "3d 2h 15m",
    "current": "2025-01-15 10:30:00",
    "rand": 12345,
    "mac": "AA:BB:CC:DD:EE:FF",
    "model": "R2000",
    "features": "lte,wifi",
    "wui_port": "80",
    "telnet_port": "23",
    "ssh_port": "22",
    "local_ip": "192.168.1.1"
}
```

| Field | Meaning |
|-------|---------|
| `name` / `mode` | Same as config (runtime copy) |
| `platform` / `hardware` / `custom` | Board identity |
| `scope` | `std` shows Development menu; `product` hides it |
| `version` | Firmware version |
| `ill` | Present only when non-zero; web forces factory page |
| `livetime` | System uptime |
| `current` | Current date/time |
| `rand` | Session random (auth) |
| `mac` / `model` / `features` | EEPROM cache |
| `wui_port` / `telnet_port` / `ssh_port` | Service ports |
| `local_ip` | LAN IP |

```
$ land@machine.status:version
8.0.0
```

**Date / NTP (`clock@date`, `clock@ntps`)**

```
$ clock@date
{
    "timezone": "8",
    "ntpclient": "enable",
    "ntpserver": "ntp1.aliyun.com",
    "ntpserver2": "ntp2.aliyun.com",
    "ntpserver3": "ntp3.aliyun.com",
    "ntpinterval": "86400"
}
```

```
$ clock@date.status
{
    "current": "12:29:41:05:10:2022",
    "livetime": "00:01:58:0",
    "uptime": "118"
}
```

| Field | Meaning |
|-------|---------|
| `source` | `ntp` / `set` / `lte` / `gps` (optional) |
| `current` | `hour:minute:second:month:day:year` |
| `livetime` | `hour:minute:second:day` |
| `uptime` | Seconds |

```
$ clock@ntps
{
    "status": "enable"
}
```

Set clock / sync:

```
$ clock@date.current[12:30:00:1:15:2025]
ttrue
$ clock@date.ntpsync
ttrue
$ clock@date.ntpsync[ntp1.aliyun.com]
ttrue
$ clock@date:timezone=8
ttrue
$ clock@date|{"ntpclient":"enable","ntpserver":"pool.ntp.org"}
ttrue
$ clock@ntps={"status":"enable"}
ttrue
```

**Change hostname / mode**

```
$ land@machine:name=MyRouter
ttrue
$ land@machine:mode=misp
ttrue
```

Changing `mode` restarts the device.

**Restart** (Device Sys Reboot; web sends `machine.restart[0,wui]`)

```
$ land@machine.restart
ttrue
$ land@machine.restart[0,wui]
ttrue
```

`tfalse` means restart is locked.

### 8.2 Configure

Web tab **Configure Manage**. Backup/restore/default use `arch@data` (web also uses HTTP `/upload` / `/download`).

```
$ arch@data.backup
$ arch@data.default
ttrue
```

Factory UI (when `ill` is set) also calls `arch@data.factory` and EEPROM fields on `arch@data`.

### 8.3 Software

Web tab **Software Manage**.

```
$ land@fpk.list
{
    "agent": {
        "path": "/skinos/agent/",
        "size": 155997,
        "intro": "agent for remote or cloud control",
        "version": "7.0.0",
        "author": "dimmalex@gmail.com"
    }
}
```

| Field | Meaning |
|-------|---------|
| `path` | Install directory (board-dependent; do not hardcode) |
| `size` | Bytes |
| `intro` / `version` / `author` | From `prj.json` |

```
$ land@fpk.delete[agent]
ttrue
$ arch@firmware.online_check
{
    "version": "2.0.2",
    "url": "ftp://repo.example/pub/...",
    "changelog": {
        "2.0.2": "Bug fixes"
    }
}
$ arch@firmware.online_upgrade[ftp://repo.example/pub/fw.zz]
```

### 8.4 Password

Web tab **Password Manage**.

```
$ land@auth.change[,admin,OLD,NEW,admin]
ttrue
```

Arguments: domain (empty = default), username, old password, new password, new username. The web UI sends plaintext as typed.

### 8.5 Web Server

Web menu: **System → Web Server**. Component: `wui@admin`.

| Key | Meaning |
|-----|---------|
| `status` | HTTP server on/off |
| `port` / `sslport` / `ttydport` | HTTP / HTTPS / ttyd |
| `session_timeout` / `talk_timeout` / `key_lifetime` | Seconds |
| `manager` | IP/MAC allow-list |

```
$ wui@admin
{
    "status": "enable",
    "port": "80",
    "sslport": "443",
    "manager": {
        "pc1": "192.168.8.111"
    }
}
```

No `.status` API. Apply:

```
$ wui@admin:port=8080
ttrue
$ wui@admin={"status":"enable","port":"80","sslport":"443"}
ttrue
```

### 8.6 Auto Reboot

Web menu: **System → Auto Reboot**. Component: `clock@restart`. No status API.

| Key | Meaning |
|-----|---------|
| `mode` | `disable` / `age` / `point` / `idle` |
| `age` | Max uptime (seconds) |
| `point_hour` / `point_minute` | Clock time |

```
$ clock@restart
{
    "mode": "age",
    "age": "2880"
}
```

```
$ clock@restart:mode=disable
ttrue
$ clock@restart|{"mode":"point","point_hour":"03","point_minute":"30"}
ttrue
```

### 8.7 Telnet Server

Web menu: **System → Telnet Server**. Component: `tui@telnet`. No status API.

```
$ tui@telnet
{
    "status": "enable",
    "port": "23",
    "manager": {
        "pc1": "192.168.8.111",
        "pc2": "00:03:7F:12:AA:B0"
    }
}
```

```
$ tui@telnet:port=2323
ttrue
$ tui@telnet:status=disable
ttrue
$ tui@telnet|{"status":"enable","port":"23"}
ttrue
```

### 8.8 SSH Server

Web menu: **System → SSH Server**. Component: `tui@ssh`. Same keys as Telnet (default port 22).

```
$ tui@ssh
$ tui@ssh:status=enable
ttrue
$ tui@ssh:port=22
ttrue
```

### 8.9 File Server

Web menu: **System → File Server**. Components: `storage@ftp`, `storage@samba`, NAS users via `land@auth.list[nas]`.

```
$ storage@ftp
{
    "status": "enable",
    "mode": "user",
    "root": "/mnt",
    "anonymous": {
        "path": "/mnt",
        "permission": "read"
    }
}
```

```
$ storage@ftp:status=enable
ttrue
$ land@auth.list[nas]
$ land@auth.add[,shareuser,secret,nas]
ttrue
$ land@auth.delete[shareuser]
ttrue
```

Samba uses `storage@samba` with the same get/set pattern as FTP.

### 8.10 Agent Control

Web menu: **System → Agent Control**. Component: `agent@local`. No status API.

```
$ agent@local
{
    "broadcast": "enable",
    "broadcast_port": "22222",
    "broadcast_group": "default",
    "broadcast_command": "query",
    "json": "disable",
    "json_port": "22220"
}
```

| Key | Meaning |
|-----|---------|
| `json` / `json_port` | TCP HE bridge |
| `json_command` | `query` / `all` |
| `broadcast` / `broadcast_port` | UDP discovery |

```
$ agent@local:json=enable
ttrue
$ agent@local={"json":"enable","json_port":"22220","broadcast":"enable"}
ttrue
```

---

## 9 Debug

### 9.1 Syslog

Web menu: **Debug → Syslog**. Component: `land@syslog`.

**Configuration**

| Key | Meaning |
|-----|---------|
| `status` | `disable` / `enable` / `tui` / `both` / `file` |
| `level` | `verb` / `debug` / `info` / `warn` / `fault` |
| `remote` / `port` | Remote syslog |
| `klog` | Kernel log |
| `location` / `size` | File store and size (KB) |

```
$ land@syslog
{
    "status": "enable",
    "level": "info",
    "remote": "192.168.1.100",
    "port": "514",
    "klog": "enable",
    "critical": "enable",
    "critical_size": "50",
    "location": "storage",
    "size": "100"
}
```

**Query log files**

```
$ land@syslog.list
{
    "12345-syslog.log": "/var/log/12345-syslog.log",
    "critical.txt": "/var/internal/critical.txt"
}
```

| Field | Meaning |
|-------|---------|
| key | File name |
| value | Absolute path |

```
$ land@syslog:status=enable
ttrue
$ land@syslog={"status":"enable","level":"info"}
ttrue
$ land@syslog.delete[12345-syslog.log]
ttrue
```

### 9.2 Terminal

Web menu: **Debug → Terminal**. Starts the web ttyd helper.

```
$ wui@ttyd.setup
$ wui@ttyd.port
```

The page embeds an iframe to that port. This is a browser console, not eline itself.

---

## 10 VPN

List pages create instances (`vpn@l2tp`, `vpn@l2tp2`, …). Instance pages edit one object. `network@frame.list[local]` fills LAN selectors.

### 10.1 L2TP Client

Web menu: **VPN → L2TP Client**. List: `vpn@l2tplist`. Instance: `vpn@l2tp`.

```
$ vpn@l2tplist.list
$ vpn@l2tplist.status
{
    "vpn@l2tp": {
        "status": "up",
        "netdev": "ppp0",
        "ip": "10.0.0.2",
        "dstip": "10.0.0.1",
        "mask": "255.255.255.255",
        "serverip": "198.51.100.1",
        "dns": "8.8.8.8",
        "livetime": "02:30:15:0",
        "rx_bytes": "123456",
        "tx_bytes": "654321"
    }
}
```

| Field | Meaning |
|-------|---------|
| `status` | Tunnel state |
| `ip` / `dstip` / `serverip` | PPP and server addresses |
| `livetime` | `hour:minute:second:day` |

```
$ vpn@l2tplist.add[203.0.113.1,1701]
ttrue
$ vpn@l2tplist.delete[vpn@l2tp]
ttrue
```

**Instance**

| Key | Meaning |
|-----|---------|
| `status` | Enable tunnel |
| `extern` | `default` or `ifname@wan` / `ifname@lte` |
| `server` / `port` | L2TP peer |
| `username` / `password` / `authmode` | PPP auth |
| `masq` / `defaultroute` | NAT and default route |

```
$ vpn@l2tp
$ vpn@l2tp.status
$ vpn@l2tp:server=203.0.113.1
ttrue
$ vpn@l2tp.setup
ttrue
$ vpn@l2tp.shut
ttrue
```

### 10.2 PPTP Client

Same pattern: `vpn@pptplist` / `vpn@pptp`.

```
$ vpn@pptplist.list
$ vpn@pptplist.status
$ vpn@pptplist.add[203.0.113.1]
ttrue
$ vpn@pptp.setup
ttrue
$ vpn@pptp.shut
ttrue
```

### 10.3 GRE Tunnel

Same pattern: `vpn@grelist` / `vpn@gre`.

```
$ vpn@grelist.list
$ vpn@grelist.status
$ vpn@grelist.add[203.0.113.8]
ttrue
$ vpn@gre.setup
ttrue
```

### 10.4 IPsec Client

Web menu: **VPN → IPsec Client**. List: `ipsec@list`. Instance: `ipsec@client`.

```
$ ipsec@list.list
$ ipsec@list.status
$ ipsec@list.add[203.0.113.1]
ttrue
$ ipsec@list.delete[ipsec@client]
ttrue
```

**Instance configuration**

| Key | Meaning |
|-----|---------|
| `status` / `extern` / `server` | Enable, uplink, gateway |
| `version` | IKE `1` / `2` |
| `auth_method` | `psk` or certificates |
| `psk` | Pre-shared key |
| `local_ts` / `remote_ts` | Traffic selectors |

```
$ ipsec@client
{
    "status": "enable",
    "extern": "default",
    "server": "203.0.113.1",
    "version": "2",
    "auth_method": "psk",
    "psk": "MySecretKey123",
    "local_ts": "10.1.0.0/24",
    "remote_ts": "10.2.0.0/24"
}
```

```
$ ipsec@client.status
$ ipsec@client.key
$ ipsec@client.setup
ttrue
$ ipsec@client.shut
ttrue
```

Certificate helpers: `import_ca` / `import_cert` / `import_key` / `clear_ca` / `clear_cert` / `clear_key` (web upload uses HTTP; eline can call the clear/import methods with a path).

### 10.5 WireGuard

Web menu: **VPN → WireGuard**. List: `nvpn@wglist`. Instance: `nvpn@wg`.

```
$ nvpn@wglist.list
$ nvpn@wglist.status
$ nvpn@wglist.add[10.0.0.2,255.255.255.0]
ttrue
$ nvpn@wg
$ nvpn@wg.status
$ nvpn@wg.rekey
ttrue
$ nvpn@wg.setup
ttrue
$ nvpn@wg.shut
ttrue
```

Instance keys: `private_key` / `public_key`, `peer.<pubkey>.endpoint` / `ips` / `keepalive`, `local.ip` / `local.mask`, `port`, `mtu`, `masq`, `defaultroute`.

---

## 11 Application

Pages without `"menu"` in `prj.json` land under **Application**. UART titles on some products are UART#1(RS485) / UART#2(RS232) — same objects.

### 11.1 IO Control

Web menu: **Application → IO Control**. Page config binds `arch@gpio` (attr `g1`); runtime object `agent@io`.

```
$ agent@io
$ arch@gpio
$ agent@io.status
{
    "g1": "01",
    "g2": "11",
    "g3": "10"
}
```

| Field | Meaning |
|-------|---------|
| `gN` | `00` in low, `01` in high, `10` out low, `11` out high |

```
$ agent@io.list
$ agent@io={"status":"enable","init":{"g1":"01"}}
ttrue
```

### 11.2 Camera OSD

```
$ camera@osd
{
    "status": "enable",
    "camera": "hikvision",
    "address": "192.168.8.64",
    "username": "admin",
    "password": "12345",
    "interval": "15000"
}
```

```
$ camera@osd2he
$ camera@osd:status=enable
ttrue
```

No `.status` API on `camera@osd`.

### 11.3 DDNS

```
$ ddns@scripts
{
    "client": {
        "status": "enable",
        "isp": "oray.com",
        "domain": "example.oray.net",
        "username": "user",
        "password": "secret"
    }
}
```

```
$ ddns@scripts.isplist
$ ddns@scripts.status
{
    "client": {
        "state": "succeed",
        "domain_ip": "1.2.3.4",
        "extern_ip": "1.2.3.4"
    }
}
```

| Field | Meaning |
|-------|---------|
| `state` | `register` / `succeed` / `already` / `deny` / `noneed` |
| `domain_ip` / `extern_ip` | Registered vs local IP |

```
$ ddns@scripts={"client":{"status":"enable","isp":"oray.com","domain":"example.oray.net","username":"user","password":"secret"}}
ttrue
```

### 11.4 GNSS

Objects: `gnss@nmea`, `gnss@nmea2`, `gnss@nmea3`. Status is queried as `gnssdrv@nmea.status[gnss@nmea]`.

```
$ gnss@nmea
{
    "status": "enable",
    "ttydev": "/dev/ttyUSB0",
    "drvcom": "gnssdrv@nmea",
    "speed": "9600",
    "nmea": {
        "client": {
            "status": "enable",
            "proto": "tcp",
            "server": "192.168.8.100",
            "port": "9000",
            "filter": "GGA;RMC"
        }
    }
}
```

```
$ gnssdrv@nmea.status[gnss@nmea]
$ gnss@frame.list
$ gnss@nmea:nmea/client/server=192.168.8.251
ttrue
```

### 11.5 SNMP Agent

```
$ snmp@agent
{
    "status": "disable",
    "port": "161",
    "rocommunity": "rocommunity"
}
```

No `.status` API.

```
$ snmp@agent:status=enable
ttrue
$ snmp@agent={"status":"enable","port":"161","rocommunity":"public"}
ttrue
```

### 11.6 UART

Web menu: **Application → UART#1** … **UART#8**. Objects: `uart@tty` … `uart@tty8`.

**Configuration**

| Key | Meaning |
|-----|---------|
| `status` | Port on/off |
| `ttydev` / `drvcom` | Device path and driver (`uartdrv@dtu`, `uartdrv@tui`, …) |
| `speed` / `parity` / `databit` / `stopbit` / `flow` | Line settings |
| `extern` | Reset when an uplink comes online |
| `dtu` (and other driver subtrees) | Driver-specific |

```
$ uart@tty
{
    "status": "enable",
    "ttydev": "/dev/ttyS1",
    "drvcom": "uartdrv@dtu",
    "speed": "57600",
    "dtu": {
        "client": {
            "status": "enable",
            "proto": "tcp",
            "server": "192.168.8.250",
            "port": "800"
        }
    }
}
```

**Query status** (delegated to the driver)

```
$ uart@tty.status
{
    "tty": {
        "rx": 1024,
        "tx": 2048,
        "connect": "ok"
    },
    "client": {
        "rx": 512,
        "tx": 256,
        "connect": "ok",
        "ip": "192.168.8.250"
    }
}
```

| Field | Meaning |
|-------|---------|
| `tty.rx` / `tty.tx` | Serial bytes |
| `connect` | Link state |
| `client.ip` | Peer (DTU) |

```
$ uart@tty.reset
ttrue
$ uart@tty={"status":"enable","ttydev":"/dev/ttyS1","drvcom":"uartdrv@tui","speed":"57600"}
ttrue
```

`uartdrv@tui` is Command Line (eline on that UART). TLS helpers: `uart@tty.key`, `clear_ca` / `clear_cert` / `clear_key`.

### 11.7 Test components

Sample page from `tmptools`. Object: `tmptools@testcom`.

```
$ tmptools@testcom
$ tmptools@testcom.status
$ tmptools@testcom.start
$ tmptools@testcom.shut
```

This entry is a development sample, not a production service.
