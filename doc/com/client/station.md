## client@station — LAN Client Management

### Overview

Manage all local network clients. This component monitors the ARP table to track which devices are on the LAN, combines this with DHCP lease information and saved per-MAC settings, and provides a unified view of all connected clients. It also supports MAC-IP binding via ARP table manipulation and publishes joint events when clients appear or disappear.

- monitors ARP table in real-time via netlink socket for instant client detection
- combines ARP data, DHCP lease data, and saved per-MAC configuration into unified client list
- supports MAC-IP binding with ARP table enforcement
- publishes joint events on client appear/disappear with IP change detection
- provides IP-to-MAC resolution from ARP table



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
    - succeed return [ json ], combined view of ARP data, DHCP leases, and saved settings
    - each key is a MAC address; each value contains ip, name, ifname, netdev, uptime, livetime
    ```json
    {
        "client MAC address":                 // [ string ]
        {
            "name":"client name",             // [ string ], hostname from DHCP or saved config
            "ip":"ip address",                // [ ip address ]
            "ifname":"connected ifname",      // [ string ], e.g. "ifname@lan", "ifname@lan2"
            "netdev":"kernel netdev",         // [ string ], e.g. "br-lan"
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
            "ifname":"ifname@lan2",
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        }
    }
    ```

+ `ip2mac[ ip ]` **resolve IPv4 address to MAC address**
    - ip --------------- [ ip address ]
    - failed return NULL
    - succeed return [ string ], the MAC address associated with the IP

    Example, resolve IP to MAC
    ```shell
    client@station.ip2mac[ 192.168.31.140 ]
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
| `station/appear` | Sent when a new client appears on the LAN or a client gets a new IP address. Payload includes `ip`, `mac`, `ifname`, `netdev`. |
| `station/disappear` | Sent when a client goes offline or its IP address is about to change. Payload includes `ip`, `mac`, `ifname`, `netdev`. |
