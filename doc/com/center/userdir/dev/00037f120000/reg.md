## <username>/dev/<macid>/reg — Device register snapshot

### Overview

JSON file that stores the last register / status-update payload from a gateway.

- Path: `{device_path}/<username>/dev/<macid>/reg` (`00037f120000` here is an example macid)
- HE/dbs path: `center@heport/<username>/dev/<macid>/reg`
- Written by `center@heport` as raw JSON on first SSL register and on every `0+r` update
- Built on the device by `agent@heclient` (`he_client_register`); center does **not** reshape the object before save
- Used by `center@api.device_list`, gateway port helpers, and mesh endpoint enrichment
- List APIs may strip nested fields in the **response** only; the on-disk file stays verbatim


### Concepts

Wire sources that update this file:

- First TLS line: `{macid}+{user}|{pubkey}|{json}\n` — the `{json}` body is saved to `reg`
- Later notify: `0+r{json}\n` — overwrites `reg`

`vcode` inside the JSON must match `<username>/config:vcode` (when set) or register is rejected.


### Configuration reference ( <username>/dev/<macid>/reg )

```json
// Attributes introduction 
{
    "type": "device type",                      // [ string ], from agent@heclient config; default "router"
    "vcode": "verify code",                     // [ string ], must match user account vcode when that vcode is set

    "land@machine.status":                      // [ json ], from land@machine.status after client-side strip
    {
        "mode": "operator mode",                // [ string ], e.g. "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix"
        "name": "gateway name",                 // [ string ]
        "hardware": "hardware identify",        // [ string ]
        "custom": "custom identify",            // [ string ]
        "scope": "scope identify",              // [ string ]
        "version": "firmware version",          // [ string ]
        "cfgversion": "config version",         // [ string ]
        "gpversion": "group config version",    // [ string ]
        "mac": "WAN/LAN mac",                   // [ string ]
        "macid": "12-char mac identify",        // [ string ]
        "model": "model name",                  // [ string ]
        "cmodel": "custom model name",          // [ string ]
        "wui_port": "device WUI port",          // [ string ], may be absent on some platforms
        "telnet_port": "device telnet port",    // [ string ], may be absent on some platforms
        "ssh_port": "device ssh port"           // [ string ], may be absent on some platforms
        // client strips: platform, rand, magic, livetime, current (and host-build local_ip/wui/telnet/ssh)
    },

    "network@frame.gateway":                    // [ json ], from network@frame.gateway after client-side strip
    {
        "ifname": "ifname object",              // [ string ]
        "ip": "IPv4 address",                   // [ string ]
        "mask": "netmask",                      // [ string ]
        "gw": "default gateway",                // [ string ]
        "dns": "primary DNS",                   // [ string ]
        "dns2": "secondary DNS",                // [ string ]
        "name": "link name",                    // [ string ]
        "rx_bytes": "receive bytes",            // [ string ]
        "tx_bytes": "transmit bytes"            // [ string ]
        // client strips: mode, ontime, ifdev, netdev, mac, metric, status, tid, livetime,
        // rx_errs, rx_drops, rx_packets, tx_errs, tx_drops, tx_packets
    },

    "agent@io":                                 // [ json ], optional; from sget(agent@io)
    {
        // IO component register mirror; shape follows agent@io
    },

    "gnss@nmea.info":                           // [ json ], optional; present when GNSS is available
    {
        "step": "locate step",                  // [ "setup", "search", "located" ]
        "utc": "UTC time",                      // [ string ], hour:minute:second:month:day:year
        "lon": "longitude (NMEA)",              // [ string ]
        "lat": "latitude (NMEA)",               // [ string ]
        "longitude": "longitude",               // [ string ]
        "latitude": "latitude",                 // [ string ]
        "speed": "speed",                       // [ string ]
        "direction": "direction",               // [ string ]
        "declination": "declination",           // [ string ]
        "elv": "altitude meters",               // [ string ]
        "inview": "satellites in view",         // [ number ]
        "inuse": "satellites in use"            // [ number ]
    },

    "sensor@xxx.status":                        // [ string ]: { json }, one entry per sensor@* component on the device
    {
        // shape follows that sensor's status API
    }
    // "...":{ ... }  How many sensors show how many "<com>.status" properties
}
```

#### Configuration example

Example, show register snapshot for macid 00037f120000

```shell
center@heport/ashyelf/dev/00037f120000/reg
{                                               # return this
    "type": "router",
    "vcode": "sssss",
    "land@machine.status":
    {
        "mode": "gateway",
        "name": "OfficeGW",
        "hardware": "mt7628",
        "custom": "r600",
        "scope": "std",
        "version": "v8.0.0",
        "macid": "00037f120000",
        "model": "R600",
        "wui_port": "80",
        "telnet_port": "23",
        "ssh_port": "22"
    },
    "network@frame.gateway":
    {
        "ifname": "ifname@wan",
        "ip": "10.0.0.12",
        "mask": "255.255.255.0",
        "gw": "10.0.0.1",
        "dns": "8.8.8.8",
        "rx_bytes": "102400",
        "tx_bytes": "204800"
    },
    "gnss@nmea.info":
    {
        "step": "located",
        "longitude": "113.9",
        "latitude": "22.5",
        "inview": "12",
        "inuse": "9"
    }
}
```



### Other

- `center@api.device_list` may delete nested keys such as `land@machine.status` → `hardware` / `custom` / `scope` / `mac` / `cfgversion`, and `network@frame.gateway` → `name` / `mask` / `gw` / `dns` / `dns2`, in the API reply only
- Gateway quick maps read `wui_port` / `telnet_port` / `ssh_port` from this file
- Heartbeat (`0+h`) and proactive send (`0+s`) do **not** update `reg`
