## ovpn@client — OpenVPN Client Instance Management

### Overview

Manage an individual OpenVPN client connection. Each instance (`ovpn@client`, `ovpn@client2`, …) connects to an OpenVPN server and integrates with the network framework as a VPN-type interface. Instances are created and managed by [`ovpn@list`](list.md).

- manages the OpenVPN lifecycle: setup, shutdown, online/offline
- resolves the server domain to an IP and routes via the specified extern interface
- supports certificate, username/password, and static-key authentication
- handles NAT masquerade, default route, and custom route tables
- writes the OpenVPN log to `PROJECT_LOG_DIR/<object>.log`, or to the system log when `log` is `syslog`



### Configuration reference ( ovpn@client )

```json
// Attributes introduction 
{
    "status":"client status",                                  // [ "disable", "enable" ]
    "extern":"extern ifname dependency",                       // [ "disable", "default", "ifname@wan", "ifname@lte", ... ]
                                                                    // empty / omitted is treated as "default"
                                                                    // "disable" for no extern dependency
                                                                    // "default" to use the system default gateway
                                                                    // "ifname@wan", "ifname@lte", ... for a specific extern interface

    "server":"OpenVPN server address",                         // [ string ], IP address or domain name
    "port":"OpenVPN server port",                              // [ number ], default 1194
    "proto":"transport protocol",                              // [ "udp", "tcp" ], default "udp"
    "dev":"tunnel device type",                                // [ "tun", "tap" ], default "tun"
    "bridge":"local interface to join",                        // [ string ], optional, an ifname from network@frame.list[local], e.g. "ifname@lan"
                                                                    // used when dev is "tap"; empty means do not join a bridge
                                                                    // on online, <ifname>.ifdev gives the bridge (e.g. bridge@lan), then that bridge addif
                                                                    // on offline, resolve ifdev again and delif
                                                                    // a joined tap is only a bridge port: no DNS, NAT, or routes on it
    "topology":"tun topology",                                 // [ "subnet", "p2p", "net30" ], default "subnet"
                                                                    // used when dev is "tun" and auth is not "static"
                                                                    // "net30" gives each client its own /30; the interface is local and peer addresses
    "localip":"point-to-point local address",                  // [ ip address ], valid when auth is "static"
    "remoteip":"point-to-point remote address",                // [ ip address ], valid when auth is "static"
                                                                    // peer address when dev is "tun"; netmask when dev is "tap"

    "cipher":"data cipher",                                    // [ string ], default "AES-256-GCM"
    "comp_lzo":"LZO compression",                              // [ "disable", "adaptive", "yes", "no" ], default "disable"
    "auth":"authentication type",                              // [ "certificate", "username", "static" ], default "certificate"
    "username":"auth username",                                // [ string ], valid when auth is "username"
    "password":"auth password",                                // [ string ], valid when auth is "username"
    "hmac":"control-channel protection",                       // [ "disable", "tls-auth", "tls-crypt", "tls-crypt-v2" ], default "disable"
                                                                    // ignored when auth is "static"
                                                                    // key file is {instance}.tlsauth
                                                                    // tls-auth is written as "tls-auth <file> 1"
                                                                    // tls-crypt is written as "tls-crypt <file>"
                                                                    // tls-crypt-v2 is written as "tls-crypt-v2 <file>"

    "interval":"keepalive interval",                           // [ number ], seconds, default 10
    "timeout":"keepalive timeout",                             // [ number ], seconds, default 60
    "options":"extra OpenVPN options",                         // [ string ], optional, semicolon-separated lines
    "log":"log destination",                                   // [ "file", "syslog" ], default "file"
                                                                    // "file" writes PROJECT_LOG_DIR/<object>.log, truncated on each start
                                                                    // "syslog" sends to the system log under the instance name

    "masq":"NAT masquerade",                                   // [ "disable", "enable" ]
    "defaultroute":"set as default route",                     // [ "disable", "enable" ]
    "route_table":                             // custom route rules, valid when defaultroute is "disable"
    {
        "route rule name":                     // [ string ]
        {
            "target":"destination address",        // [ string ], IP address or network
            "mask":"destination network mask"      // [ string ]
        }
        // "...":{}  How many routes show how many properties
    },
    "custom_dns":"custom DNS",                                 // [ "disable", "enable" ]
    "dns":"custom DNS1",                                       // [ ip address ], valid when custom_dns is "enable"
    "dns2":"custom DNS2"                                       // [ ip address ], valid when custom_dns is "enable"
}
```

#### Configuration example

Example, show all OpenVPN client configuration
```shell
ovpn@client
{
    "status":"enable",
    "extern":"default",
    "server":"vpn.example.com",
    "port":"1194",
    "proto":"udp",
    "dev":"tun",
    "topology":"subnet",
    "cipher":"AES-256-GCM",
    "auth":"certificate",
    "comp_lzo":"disable",
    "hmac":"disable",
    "interval":"10",
    "timeout":"60",
    "masq":"enable",
    "defaultroute":"enable",
    "log":"file"
}
```

#### Configuration settings example

Example, enable the OpenVPN client
```shell
ovpn@client:status=enable
ttrue
```

Example, change the OpenVPN server
```shell
ovpn@client:server=new-vpn.example.com
ttrue
```

Example, merge set OpenVPN configure( include "server" "port" "auth" )
```shell
ovpn@client|{"server":"vpn.example.com","port":"1194","auth":"username"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the OpenVPN client**
    - succeed return ttrue
    - only starts if status is "enable"
    - generates the client config and launches openvpn

+ `shut[]` **shut down the OpenVPN client**
    - succeed return ttrue
    - notifies `network@frame.offline`, stops openvpn, and deletes the tunnel device
    - keeps the log file


#### Query APIs

+ `status[]` **get OpenVPN client status**
    - instance does not exist: return NULL
    - instance exists but config `status` is not `"enable"`: return `{ "status":"disable" }`
    - enabled: return [ json ]
    ```json
    {
        "status":"Current state",        // [ "disable", "uping", "down", "up" ]
                                             // "disable" client is disabled
                                             // "uping" OpenVPN is connecting
                                             // "down" OpenVPN is down
                                             // "up" tunnel is established
        "netdev":"netdev name",          // [ string ], e.g. "ovpn0"
        "serverip":"server IP",          // [ ip address ], resolved server IP
        "ip":"IP address",               // [ ip address ], local tunnel IP
        "dstip":"destination IP",        // [ ip address ], remote tunnel IP
        "mask":"network mask",           // [ ip address ]
        "gw":"gateway IP",               // [ ip address ], Optional
        "dns":"DNS server",              // [ ip address ], Optional
        "dns2":"backup DNS",             // [ ip address ], Optional
        "livetime":"online time",        // [ string ], format hour:minute:second:day
        "rx_bytes":"received bytes",     // [ string ]
        "rx_packets":"received packets", // [ string ]
        "tx_bytes":"sent bytes",         // [ string ]
        "tx_packets":"sent packets"      // [ string ]
    }
    ```

    Example, get the first OpenVPN client status
    ```shell
    ovpn@client.status
    {
        "status":"up",
        "netdev":"ovpn0",
        "ip":"10.8.0.2",
        "dstip":"10.8.0.1",
        "mask":"255.255.255.0",
        "gw":"10.8.0.1",
        "serverip":"198.51.100.1",
        "dns":"8.8.8.8",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "rx_packets":"789",
        "tx_bytes":"654321",
        "tx_packets":"987"
    }
    ```

+ `netdev[]` **get the OpenVPN netdev name**
    - failed return NULL
    - succeed return [ string ], the tunnel netdev (ovpn0 for ovpn@client, ovpn1 for ovpn@client2, … ovpn9 for ovpn@client10)

    Example, get the first OpenVPN client netdev
    ```shell
    ovpn@client.netdev
    ovpn0
    ```

+ `key[]` **list existing key files for this instance**
    - list object: return NULL
    - succeed return [ json ]
    ```json
    {
        "ca":"path to client.ca",
        "crt":"path to client.crt",
        "key":"path to client.key",
        "static":"path to client.static",
        "tlsauth":"path to client.tlsauth"
    }
    ```
    Only keys for files that exist are included. `ovpn@client2` uses `client2.ca`, `client2.crt`, and so on.


#### Control APIs

+ `reset[]` **restart the OpenVPN client**
    - succeed return ttrue
    - behavior depends on the `extern` setting:
        - "default": restarts only on `network/online`
        - specific ifname: restarts only when event ifname matches
        - empty / "disable": no restart

+ `import_ca[ file ]` **import a CA certificate**
    - file ------------ [ string ], uploaded temporary file path
    - list object: return tfalse
    - succeed return ttrue
    - copies to `{instance}.ca` in the ovpn project configuration directory

+ `import_cert[ file ]` **import a client certificate**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.crt`

+ `import_key[ file ]` **import a client private key**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.key`

+ `import_static[ file ]` **import a static secret key**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.static`
    - used when auth is "static"

+ `import_tlsauth[ file ]` **import the control-channel key**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.tlsauth`
    - used when hmac is "tls-auth", "tls-crypt", or "tls-crypt-v2"

+ `clear_ca[]` **remove the CA certificate file**
    - succeed return ttrue

+ `clear_cert[]` **remove the client certificate file**
    - succeed return ttrue

+ `clear_key[]` **remove the client private key file**
    - succeed return ttrue

+ `clear_static[]` **remove the static secret key file**
    - succeed return ttrue

+ `clear_tlsauth[]` **remove the control-channel key file**
    - succeed return ttrue
