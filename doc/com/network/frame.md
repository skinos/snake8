## network@frame — Network Frame Management

## Overview

The **network frame** component (`network@frame`) is the hub for LAN/WAN/VPN registration, routing policy, and firewall hooks. When several **uplinks** exist, it works with the **multi-link scheduler service** to pick the active default route using numbered **priority slots** (`"1"`…`"10"`). Supported policies include **cold backup**, **hot backup**, and **lazy hot backup** (no automatic fail-back until the current uplink fails).

### Architecture

The frame component consists of two main parts:

1. **frame** - The main network frame component that manages network infrastructure
2. **connect** - A service process that handles multi-link connection management and switching

### Dependencies (conceptual)

- **Land platform** — component model, configuration, and inter-component calls used by every Skinos root.
- **Network helper library** — shared utilities for interface logging, routing, firewall helpers, and dial/DHCP-style bring-up (consumed by both **network@frame** and **ifname@…** docs).

Multi-uplink scheduling operates on **logical interfaces** (`ifname@wan`, `ifname@lte`, …) that have been registered with **network@frame**; see **ifname** documentation for each role’s JSON model.

---

### Configuration ( `network@frame` )
#### Configuration attributes

```json
// Attribute introduction
{
    "type": "Multiple link connect type",                          // [ "cold", "hot", "hot2", "hot3", "hot4", "hot5", "lazy", "lazy2", "lazy3", "lazy4", "lazy5" ]
                                                                   // "cold" / "hot" / "lazy" use priority slots "1".."10" (smaller number = higher priority)
                                                                   // "hotN" / "lazyN" (N=2..5): only slots "1".."N" participate in scheduler decisions; higher slots may still exist but are outside that policy
    "concom": "Multiple link connection management components",    // [ string ], You can customize the data scheduling component to implement more personalized requirements

    "1": "ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ] 
    "2": "ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "3": "ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "4": "ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "5": "ifname object of extern",          // [ "ifname@wan", "ifname@wan2", "ifname@lte", "ifname@lte2", "ifname@wisp", "ifname@wisp2", ... ]
    "6": "ifname object of extern",          // [ "ifname@wan", ... ]
    "7": "ifname object of extern",          // optional; same conventions as "1".."6" (system supports up to slot "10")
    "8": "ifname object of extern",
    "9": "ifname object of extern",
    "10": "ifname object of extern",

    // Configure parameters of the delay switchover function, only use in dbdc mode(reserved), the function can control the data via low delay connection
    "delay_count": "Statistical delay times of last",   // [ number ]
    "delay_divide": "delay divide line",                // [ number ], the unit is ms
    "delay_diff": "Delay differential",                 // [ number ], the unit is ms

    // DNS custom when Multiple DNS
    "custom_dns": "Custom DNS",                       // [ "disable", "enable", "ifname@lte", "ifname@lte2", ... ]
    "dns": "Custom DNS1",                             // [ ip address ], This is valid when "custom_dns" is "enable"
    "dns2": "Custom DNS2"                             // [ ip address ], This is valid when "custom_dns" is "enable"
}
```

Example, show all the configure
```shell
network@frame
{
    "type":"hot",                              # multi-link scheduling mode: hot backup
    "1":"ifname@wan",                          # priority slot 1: WAN (Ethernet)
    "2":"ifname@lte",                          # priority slot 2: LTE modem
    "3":"ifname@lte2",                         # priority slot 3: second LTE modem
    "4":"ifname@wisp",                         # priority slot 4: WISP (wireless relay)
    "custom_dns":"disable"                     # use DNS from the active uplink (not custom)
}
```

### Connection Types

| Type | Description | Priority slots considered by the scheduler |
|------|-------------|-----------------------------------------------|
| `cold` | Cold backup — only one uplink active at a time; others are brought down | `"1"` .. `"10"` |
| `hot` | Hot backup — several uplinks may stay up; default route prefers the smallest-numbered online slot | `"1"` .. `"10"` |
| `hot2` | Hot backup; scheduler only evaluates `"1"` .. `"2"` | 2 |
| `hot3` | Hot backup; scheduler only evaluates `"1"` .. `"3"` | 3 |
| `hot4` | Hot backup; scheduler only evaluates `"1"` .. `"4"` | 4 |
| `hot5` | Hot backup; scheduler only evaluates `"1"` .. `"5"` | 5 |
| `lazy` | Lazy hot backup — after a switch to a backup, it does not move back to a higher-priority link until that backup fails | `"1"` .. `"10"` |
| `lazy2` | Lazy backup; scheduler only evaluates `"1"` .. `"2"` | 2 |
| `lazy3` | Lazy backup; scheduler only evaluates `"1"` .. `"3"` | 3 |
| `lazy4` | Lazy backup; scheduler only evaluates `"1"` .. `"4"` | 4 |
| `lazy5` | Lazy backup; scheduler only evaluates `"1"` .. `"5"` | 5 |

### Configuration Example

```json
{
    "type": "hot4",                  // multi-link scheduling on 4 extern connections
    "1": "ifname@lte",
    "2": "ifname@lte2",
    "3": "ifname@wan",
    "4": "ifname@wisp",
    "delay_count": "10",             // Collect statistics on the latest 10 delays and schedule traffic based on the delay
    "delay_divide": "150", 
    "delay_diff": "100"
}
```

---

## API Reference

### Management APIs

#### `setup[]` - Setup network infrastructure
- Prepares the network framework (routing policy, firewall hooks, registration of local and external logical interfaces).
- When multi-link settings exist for the current device mode, starts the **multi-link scheduler service** in the background.

#### `shut[]` - Shutdown network infrastructure
- Stops the multi-link scheduler (if running), tears down registrations, and clears framework-managed firewall state as appropriate.

#### `service[]` - Start multi-link scheduler
- Used internally to launch the scheduler executable that performs uplink selection; ordinary administration goes through `network@frame` and configuration.

### Query APIs

#### `status[]` - Show external connections status

Returns the status of external connections when multiple external connections coexist.

**Returns:**
- `NULL` - Failed
- `terror` - Error (e.g., wrong mode of operation, only works in multi-connection mode)
- JSON object describing network information

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {           // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
        "status": "Whether online",    // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ], "up" for online
        "inuse": "Whether used"        // [ "disable", "enable" ], enable for in used, disable for not used
    }
}
```

**Status Values:**
- `nodevice` - Corresponding module could not be found
- `reset` - Reset the device
- `setup` - Setup the connection
- `register` - Register to peer
- `uping` - Connecting
- `scanning` - Scanning the peer
- `block` - Wait keeplive succeed
- `up` - Ready to connect to internet (signal/network/simcard all ok)
- `failed` - Keeplive failed
- `down` - The ifname is down

**Example:**
```shell
network@frame.status
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

#### `list[]` - List all connections

Returns a list of all registered connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": "The corresponding ifdev object"    // [ "ifname@wan", ... ]: [ string ]
}
```

**Example:**
```shell
network@frame.list
{
    "ifname@lan": "bridge@lan",
    "ifname@lte": "modem@lte",
    "ifname@lte2": "modem@lte2"
}
```

#### `local[]` - List all local connections and information

Returns detailed information about local (LAN) connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {
        "status": "Current state",        // [ "uping", "down", "up" ]
        "mode": "IPV4 address mode",      // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting
        "netdev": "netdev name",          // [ string ]
        "ifdev": "ifdev name",            // [ string ], Optional
        "gw": "gateway ip address",       // [ ip address ], Optional
        "dns": "dns ip address",          // [ ip address ], Optional
        "dns2": "dns2 ip address",        // [ ip address ], Optional
        "ip": "ip address",               // [ ip address ]
        "mask": "network mask",           // [ ip address ]
        "ontime": "online uptime",        // [ string ], Optional, online system uptime
        "livetime": "online time",        // [ string ], format is hour:minute:second:day
        "rx_bytes": "received bytes",     // [ number ]
        "rx_packets": "received packets", // [ number ]
        "tx_bytes": "transmitted bytes",  // [ number ]
        "tx_packets": "transmitted packets",// [ number ]
        "mac": "MAC address",             // [ mac address ]
        "method": "IPv6 address mode",    // [ "manual", "automatic", "slaac" ], Optional
        "addr": "IPv6 address",           // [ ipv6 address ], Optional
        "addr2": "IPv6 address2",         // [ ipv6 address ], Optional
        "addr3": "IPv6 address3"          // [ ipv6 address ], Optional
    }
}
```

#### `extern[]` - List all extern connections and information

Returns detailed information about external (WAN) connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {
        "status": "Current state",
        "mode": "IPV4 address mode",      // [ "dhcpc", "static", "pppoe" ]
        "netdev": "netdev name",
        "ifdev": "ifdev name",
        "gw": "gateway ip address",
        "dns": "dns ip address",
        "dns2": "dns2 ip address",
        "ip": "ip address",
        "mask": "network mask",
        "delay": "delay time",            // [ "failed", "block", number ]
        "ontime": "online uptime",
        "livetime": "online time",
        "rx_bytes": "received bytes",
        "rx_packets": "received packets",
        "tx_bytes": "transmitted bytes",
        "tx_packets": "transmitted packets",
        "mac": "MAC address",
        "method": "IPv6 address mode",
        "addr": "IPv6 address",
        
        // LTE specific attributes (for ifname@lte, ifname@lte2, etc.)
        "imei": "IMEI number",
        "imsi": "IMSI number",
        "iccid": "ICCID number",          // [ number, "nosim", "pin", "puk" ]
        "plmn": "MCC and MNC",            // [ number, "noreg", "dereg" ]
        "name": "modem name",
        "operator": "operator name",
        "nettype": "network type",
        "signal": "signal level",         // [ "0", "1", "2", "3", "4" ]
        "rssi": "signal intensity",
        "csq": "CSQ number",
        "rsrp": "RSRP value",
        "rsrq": "RSRQ value",
        "sinr": "sinr value",
        "band": "current band",
        "ci": "cell identity",
        "lac": "location area code",
        "channel": "channel",
        
        // WISP specific attributes (for ifname@wisp, ifname@wisp2)
        "peer": "Peer SSID",
        "peermac": "Peer BSSID",
        "rate": "connect rate",
        "rssp": "Peer signal percentage"
    }
}
```

#### `vpn[]` - List all VPN connections and information

Returns information about VPN connections.

```json
{
    "ifname object": {
        "mode": "current mode",
        "status": "current state",
        "ifdev": "corresponding ifdev object",
        "netdev": "netdev name of linux",
        "ip": "IP address",
        "rx_bytes": "received bytes",
        "rx_packets": "received packets",
        "tx_bytes": "transmitted bytes",
        "tx_packets": "transmitted packets",
        "mac": "MAC address"
    }
}
```

#### `outer[]` - List all extern and VPN connections

Combines extern and VPN connection information.

#### `default[]` - Get current default connection

Returns the current default gateway connection information.

- Call without arguments: return JSON with connection info
- Call with one argument (ifname): return `ttrue` when matched, otherwise `tfalse`

#### `gateway[]` - Get current gateway connection

Returns the current gateway connection information (similar to `default[]`).

### Interface Management APIs

#### `register[ifname,concom,ifdev,type]` - Register an interface

Registers a network interface with the frame.

**Parameters:**
- `ifname` - Interface name (e.g., "ifname@lte")
- `concom` - Connection component name
- `ifdev` - Interface device name
- `type` - Interface type ("local", "extern", "vpn")

#### `unregister[ifname,type]` - Unregister an interface

Unregisters a network interface from the frame.

#### `add[ifdev,netdev]` - Add a device

Adds a network device to the frame (VLAN or Bridge).

#### `delete[ifdev]` - Delete a device

Removes a network device from the frame.

### Online/Offline APIs

#### `online[info]` - IPv4 online notification

Called when an interface comes online (IPv4).

**Info JSON structure:**
```json
{
    "ifname": "interface name",
    "ifdev": "device name",
    "netdev": "network device",
    "ip": "IP address",
    "mask": "network mask",
    "gw": "gateway",
    "dns": "DNS server",
    "dns2": "backup DNS"
}
```

#### `offline[ifname]` - IPv4 offline notification

Called when an interface goes offline (IPv4).

#### `upline[info]` - IPv6 online notification

Called when an interface comes online (IPv6).

#### `downline[ifname]` - IPv6 offline notification

Called when an interface goes offline (IPv6).

---

## Connect Service

The connect service is a separate process that manages multi-link connections. It is started by the frame component when multi-link mode is configured.

### Features

1. **Cold backup**
   - Only one uplink is active at a time; lower-priority links are taken down when a higher-priority one is used.
   - When the default route changes, existing connection tracking may be cleared so sessions are not stuck on the old path.

2. **Hot backup**
   - Multiple uplinks may remain up; the **default route** tracks the best available slot (smallest index that is online).
   - If the preferred uplink fails, traffic moves to the next eligible slot automatically.

3. **Lazy backup**
   - Like hot backup, but after failing over it **does not move back** to a higher-priority uplink when that uplink recovers, until the current uplink fails again — reducing flip-flop between links.

For `hot2`…`hot5` and `lazy2`…`lazy5`, only the first *N* numbered slots participate in **scheduler decisions**; additional slots may still appear in configuration but are outside that policy’s scan (see the table above).

### Signals

The connect service responds to the following signals:

- `SIGHUP` - Refresh connections
- `SIGTERM` / `SIGINT` - Graceful shutdown
- `SIGPIPE` - Ignored (prevents crash on broken pipe)

### Control Interface

On systems that ship the maintenance CLI, you can signal the scheduler with:

```shell
# Exit the connect service
connect exit

# Refresh connections
connect flush
```

### Status Query

```shell
# Query connect service status
network@frame.status
```

---

## Deployment notes

### Paths and storage

Install layout (where libraries, the scheduler binary, runtime sockets, and registration files live) is decided by the **firmware image** and project packaging. Operators should use **`he`**, the Web UI, or product-specific tools — not hard-coded paths.

### Firewall and policy routing

The framework creates the **iptables/nftables structures** needed for outbound NAT, policy routing marks, and shunting. Exact chain names visible on the device may vary by product line.

### Routing Tables

- Default table preference: 100
- Default table name: "default"

---

#### Joint Events
The following joint events are published (JSON on the joint bus) when network interface state changes. Other components can subscribe at runtime (joint registration / **`land@joint`**).

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

---

## Examples

### Basic Multi-WAN Setup

```shell
# Configure 4 external connections in hot backup mode
config network@frame hot4
set type=hot4
set 1=ifname@wan
set 2=ifname@lte
set 3=ifname@wisp
set 4=ifname@lte2
commit
```

### Custom DNS Configuration

```shell
# Configure custom DNS for multi-WAN
config network@frame hot4
set type=hot4
set custom_dns=enable
set dns=8.8.8.8
set dns2=8.8.4.4
commit
```

### Query Connection Status

```shell
# Get all extern connection status
network@frame.extern

# Get current default gateway
network@frame.default

# Check if specific interface is default
network@frame.default[ifname@lte]
```

### Manual Connection Control

```shell
# Refresh connections (trigger re-evaluation)
connect flush

# Stop connect service
connect exit
```

---

## Notes

1. The **multi-link scheduler** starts only when the **merged network profile** for the current device **network mode** includes a **`connect`** section describing multi-uplink behavior (for example `type` and numbered slots). If that section is absent, only per-interface bring-up runs and no scheduler is needed.
2. Up to **10** priority slots exist; configuration keys **`"1"` … `"10"`** refer to them in order (smaller index = higher priority).
3. Which logical interfaces appear as **local**, **external**, or **VPN** is driven by the **network profile** for the mode, plus any dynamic registration used for disabled-link handling in multi-WAN products.
4. DNS follows the **default uplink** when it changes, **unless** `custom_dns` is `enable` or a per-interface DNS override applies.
5. In **cold** backup, clearing IPv4 connection tracking on default-route changes avoids stale sessions tied to the old uplink.

---

### Lifecycle API
+ `setup[]` / `shut[]` — bring up or tear down the frame service and multi-link **`connect`** integration. **`network@hosts.setup`** is scheduled under **`init` → `land`**; **`network@frame`** itself is usually started from the **`connect`** executable / platform sequence when a profile contains a **`connect`** section.


### Published Joint Events
**IPv4 / IPv6 uplink and scheduling notifications** emitted as JSON on the joint bus, including (non-exhaustive):

| Event (examples) | When |
|------------------|------|
| `network/on`, `network/off` | Logical extern link up/down |
| `network/onextern`, `network/offextern` | External ifname scope |
| `network/onvpn`, `network/offvpn` | VPN scope |
| `network/online`, `network/offline` | Post-connect / DHCP-style online |
| `network/upline`, `network/downline`, `network/upextern`, … | IPv6 / dual-stack analogues |

Payload structure follows the `talk_t` JSON built in **`_online` / `_offline` / `_upline` / `_downline`** (typically includes **`ifname`** and status fields).

### C Code Example
```c
#include "skin/skin.h"

static void example_frame_list(void)
{
    talk_t ret = scall("network@frame", "list", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```

