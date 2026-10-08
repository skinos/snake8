## client@station — LAN Client Management

### Overview

Manage all local network clients. This component monitors the ARP table (and IPv6 neighbour table when `reg ipv6==1`) to track which devices are on the LAN, combines this with DHCP lease information and saved per-MAC settings, and provides a unified view of all connected clients. It also supports MAC–IPv4 binding via ARP table manipulation and publishes joint events when clients appear or disappear.

- doorbell: netlink `RTM_NEWNEIGH/DELNEIGH` (+ timer) → `station_update`
- IPv4 table: `/proc/net/arp`; IPv6 table: `RTM_GETNEIGH` dump (same source as `ip -6 neigh`)
- presence is **MAC-based**: IPv6 address churn does not emit appear/disappear; only new/gone MAC (or IPv4 address change) does
- IPv6 addresses exposed as `addr`, `addr2`, `addr3` (at most 3; skips incomplete / no-lladdr; hides `fe80::` link-local and multicast in `list`)
- combines ARP/ND data, DHCPv4 lease data, and saved per-MAC configuration into unified client list
- supports MAC–IPv4 binding (`bindip` / `arpbind`) only — no IPv6 bind yet
- provides `ip2mac` (IPv4 ARP) and `addr2mac` (IPv6 `RTM_GETNEIGH` lookup)



### Configuration reference ( client@station )

```json
// Attributes introduction 
{
    "client MAC address":                             // [ string ], MAC address as key (e.g. "00:03:7F:22:43:2B")
    {
        "ifname":"specify logical ifname",            // [ "ifname@lan", "ifname@lan2", ... ], default "ifname@lan"
        "name":"specify hostname",                    // [ string ], custom display name
        "bindip":"fixed IP for DHCP",                 // [ ip address ], fixed address for this MAC when using DHCP
        "arpbind":"ARP binding",                      // [ "disable", "enable" ], keep fixed IP-MAC binding on LAN
        "lease":"DHCP lease time"                     // [ number ], the unit is second
    }
    // "...":{}  How many clients show how many properties
}
```

#### Configuration example

Example, show all station configuration
```shell
client@station
{
    "00:03:7F:22:43:2B":
    {
        "ifname":"ifname@lan",
        "name":"Office-Printer",
        "bindip":"192.168.31.100",
        "arpbind":"enable",
        "lease":"0"
    },
    "F6:F7:73:82:0A:FC":
    {
        "ifname":"ifname@lan",
        "name":"Xiaomi-Phone",
        "bindip":"192.168.31.222",
        "arpbind":"disable"
    }
}
```

#### Configuration settings example

Example, bind IP 192.168.31.222 for a client
```shell
client@station:00:51:45:CB:78:80/bindip=192.168.31.222
ttrue
```

Example, clear the bind IP for a client
```shell
client@station:00:51:45:CB:78:89/bindip=
ttrue
```

Example, merge set client configure
```shell
client@station|{"00:51:45:CB:78:80":{"bindip":"192.168.31.222","name":"Phone1"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start LAN client monitoring**
    - failed return tfalse
    - succeed return ttrue
    - on slave platforms, monitoring is not started
    - applies bindip/arpbind ARP bindings and starts background monitoring service

+ `shut[]` **stop LAN client monitoring**
    - succeed return ttrue
    - clears fixed ARP bindings and stops monitoring service


#### Query APIs

+ `list[]` **list all current client information**
    - succeed return [ json ], combined view of ARP/ND data, DHCPv4 leases, Wi-Fi stalist (`wifi@n` / `wifi@a`), and saved settings
    - each key is a MAC address; each value contains ip, optional addr/addr2/…, name, ifname, netdev, ifdev, rssi, signal, uptime, livetime
    ```json
    {
        "client MAC address":                 // [ string ]
        {
            "name":"client name",             // [ string ], hostname from DHCP or saved config
            "ip":"ipv4 address",              // [ ipv4 ], from ARP / DHCPv4
            "addr":"ipv6 address",            // [ ipv6 ], first non-link-local ND address (when ipv6 on)
            "addr2":"ipv6 address",           // [ ipv6 ], optional
            "addr3":"ipv6 address",           // [ ipv6 ], optional (max 3)
            "ifname":"connected ifname",      // [ string ], e.g. "ifname@lan", "ifname@lan2"
            "netdev":"kernel netdev",         // [ string ], e.g. "br-lan"
            "ifdev":"wifi ssid/radio object", // [ string ], from wifi@n / wifi@a stalist when on Wi-Fi
            "rssi":"signal dBm",              // [ number ], from radio stalist
            "signal":"formatted rssi",        // [ string ], e.g. "-52dBm"
            "uptime":"uptime seconds",        // [ number ], seconds since appearance
            "livetime":"connected time"       // [ string ], format hour:minute:second:day
        }
        // "...":{}  How many clients show how many properties
    }
    ```

    Example, list all current clients
    ```shell
    client@station.list
    {
        "04:CF:8C:39:91:7A":
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",
            "ip":"192.168.31.140",
            "addr":"2409:8a55:10a0:ab94::1234",
            "ifname":"ifname@lan"
        },
        "40:31:3C:B5:6D:4C":
        {
            "ip":"192.168.31.61",
            "ifname":"ifname@lan",
            "name":"minij-washer-v5_mibt6D4C",
            "livetime":"14:39:34:1"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "addr":"2409:8a55:10a0:ab94::abcd",
            "addr2":"fd00:1234::1",
            "ifname":"ifname@lan2",
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        }
    }
    ```

+ `ip2mac[ ip ]` **resolve IPv4 address to MAC address**
    - ip --------------- [ ipv4 address ]
    - failed return NULL
    - succeed return [ string ], the MAC address associated with the IP

    Example, resolve IP to MAC
    ```shell
    client@station.ip2mac[ 192.168.31.140 ]
    04:CF:8C:39:91:7A
    ```

+ `addr2mac[ addr ]` **resolve IPv6 address to MAC address**
    - addr ------------- [ ipv6 address ]
    - failed return NULL (also when `reg ipv6!=1`)
    - succeed return [ string ], MAC from IPv6 neighbour dump (`RTM_GETNEIGH`)
    - skips incomplete / no-lladdr; can resolve `fe80::` even though `list` hides link-local

    Example, resolve IPv6 to MAC
    ```shell
    client@station.addr2mac[ 2409:8a55:10a0:ab94::1234 ]
    04:CF:8C:39:91:7A
    ```


#### Control APIs

+ `add[ mac, name ]` **add a client with optional name**
    - mac -------------- [ string ], MAC address (AA:BB:CC:DD:EE:FF or AABBCCDDEEFF format)
    - name ------------- [ string ], optional, display name for this MAC
    - failed return tfalse
    - succeed return ttrue

    Example, add a client with name
    ```shell
    client@station.add[ 00:03:7F:22:43:2B, NewPhone ]
    ttrue
    ```

    Example, add a client using short MAC format
    ```shell
    client@station.add[ 345212EDFE10, OldPhone ]
    ttrue
    ```

+ `delete[ mac ]` **delete a saved client**
    - mac -------------- [ string ], MAC address (AA:BB:CC:DD:EE:FF or AABBCCDDEEFF format)
    - failed return tfalse
    - succeed return ttrue

    Example, delete a client
    ```shell
    client@station.delete[ 00:03:7F:22:43:2B ]
    ttrue
    ```



### Published Joint Events

The following joint events are published when LAN clients appear, disappear, or change IP. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `station/appear` | Sent when a new MAC appears on the LAN, or an existing client gets a **new IPv4** address. Payload includes `mac`, `ifname`, `netdev`, and `ip` (IPv4) and/or `addr` (first IPv6 when the MAC was first seen via ND only). IPv6 address changes alone do **not** emit this event. |
| `station/disappear` | Sent when a MAC leaves both ARP and IPv6 ND tables, or when its **IPv4** address is about to change. Payload includes `ip`, `mac`, `ifname`, `netdev`. |
