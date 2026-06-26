## ipsec@client — IPsec Client Instance Management

### Overview

Manage an individual IPsec VPN client connection using strongSwan. Each instance (`ipsec@client`, `ipsec@client2`, …) creates an IPsec tunnel to a remote gateway, and integrates with the network framework as a VPN-type extern interface. Instances are created and managed by [`ipsec@list`](list.md).

- manages IPsec tunnel lifecycle: setup (swanctl --initiate), shutdown (swanctl --terminate)
- generates strongSwan `swanctl.conf` from instance configuration
- supports PSK and certificate authentication (IKEv2)
- configures traffic selectors (local/remote subnets)
- handles IKE/ESP encryption proposals, DPD, rekeying
- handles NAT masquerade, default route, and custom route tables
- monitors connection state via swanctl --list-sas



### Network Architecture

`ipsec@client` is a **VPN extern interface** registered by `ipsec@list` with `network@frame`. It uses a specific extern interface (e.g. `ifname@wan`) or the default gateway as its underlying transport. When the IPsec tunnel comes up, it notifies `network@frame.online`, which triggers VPN routing and multi-link scheduling updates.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).

### Configuration reference ( ipsec@client )

```json
// Attributes introduction 
{
    "status":"client status",                                  // [ "disable", "enable" ]
    "extern":"extern ifname dependency",                       // [ "disable", "default", "ifname@wan", "ifname@lte", ... ]
                                                                    // "disable" for no extern dependency
                                                                    // "default" to use the system default gateway
                                                                    // "ifname@wan", "ifname@lte", ... for a specific extern interface

    "server":"IPsec gateway address",                          // [ string ], IP address or domain name of remote gateway

    // IKE version
    "version":"IKE version",                                   // [ "1", "2" ], default 2 (IKEv2)

    // Authentication
    "auth_method":"authentication method",                     // [ "psk", "pubkey" ]
                                                                    // "psk" for pre-shared key
                                                                    // "pubkey" for certificate-based
    "local_id":"local identifier",                             // [ string ], optional, local IKE ID (IP, FQDN, email)
    "remote_id":"remote identifier",                           // [ string ], optional, remote IKE ID

    // PSK authentication (when auth_method is "psk")
    "psk":"pre-shared key",                                    // [ string ]

    // Certificate authentication (when auth_method is "pubkey")
    // Certificate files are stored in the ipsec project configuration directory,
    // named after the instance object: client.ca / client.cert / client.key for
    // ipsec@client, client2.* for ipsec@client2, etc. Use import_*/clear_* APIs
    // or the web UI to upload, download, and delete these files.

    // Child SA / Traffic selectors
    "local_ts":"local traffic selector",                       // [ string ], local subnet (e.g. "10.1.0.0/24")
    "remote_ts":"remote traffic selector",                     // [ string ], remote subnet (e.g. "10.2.0.0/24")

    // Encryption proposals
    "ike_proposals":"IKE cipher suites",                       // [ string ], optional, e.g. "aes256-sha256-modp2048"
    "esp_proposals":"ESP cipher suites",                       // [ string ], optional, e.g. "aes256-sha256"

    // DPD (Dead Peer Detection)
    "dpd_delay":"DPD check interval",                          // [ string ], optional, e.g. "30s"
    "dpd_action":"DPD action",                                 // [ "restart", "clear", "none" ], default "restart"

    // Connection behavior
    "start_action":"start action",                             // [ "start", "trap", "add" ], default "start"
                                                                    // "start" to initiate connection at startup
                                                                    // "trap" to install trap policies, initiate on matching traffic
                                                                    // "add" to add connection without initiating
    "reauth_time":"reauthentication interval",                 // [ string ], optional, e.g. "0" to disable
    "rekey_time":"rekey interval",                             // [ string ], optional

    // Routing
    "masq":"NAT masquerade",                                   // [ "disable", "enable" ]
    "defaultroute":"set as default route",                     // [ "disable", "enable" ]
    "metric":"route metric",                                   // [ number ], optional
    "route_table":                             // custom route rules, valid when defaultroute is "disable"
    {
        "route rule name":                     // [ string ]
        {
            "target":"destination address",        // [ string ], IP address or network
            "mask":"destination network mask"      // [ string ]
        }
        // "...":{}  How many routes show how many properties
    }
}
```

#### Configuration example

Example, show all IPsec client configuration (PSK site-to-site)
```shell
ipsec@client
{
    "status":"enable",
    "extern":"default",
    "server":"203.0.113.1",
    "version":"2",
    "auth_method":"psk",
    "local_id":"198.51.100.1",
    "remote_id":"203.0.113.1",
    "psk":"MySecretKey123",
    "local_ts":"10.1.0.0/24",
    "remote_ts":"10.2.0.0/24",
    "ike_proposals":"aes256-sha256-modp2048",
    "esp_proposals":"aes256-sha256",
    "dpd_delay":"30s",
    "dpd_action":"restart",
    "start_action":"start",
    "masq":"enable",
    "defaultroute":"disable"
}
```

Certificate authentication uses files in the ipsec project configuration directory, named after the instance object: **`client.cert`**, **`client.key`**, and **`client.ca`** for `ipsec@client`; **`client2.cert`**, etc. for `ipsec@client2`. The runtime resolves them via `config_path()`; a file that exists is treated as configured.

#### Configuration settings example

Example, enable the IPsec client
```shell
ipsec@client:status=enable
ttrue
```

Example, change the IPsec gateway
```shell
ipsec@client:server=new-gateway.example.com
ttrue
```

Example, merge set IPsec configure( include "server" "psk" "local_ts" "remote_ts" )
```shell
ipsec@client|{"server":"203.0.113.1","psk":"MyKey","local_ts":"10.1.0.0/24","remote_ts":"10.2.0.0/24"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **start the IPsec client**
    - succeed return ttrue
    - only starts if status is "enable"
    - generates swanctl.conf, loads secrets, initiates connection via swanctl

+ `shut[]` **shut down the IPsec client**
    - succeed return ttrue
    - terminates the IPsec connection via swanctl --terminate, notifies `network@frame.offline`


#### Query APIs

+ `status[]` **get IPsec client status**
    - failed return NULL
    - succeed return [ json ], connection status and statistics
    ```json
    {
        "status":"Current state",        // [ "disable", "connecting", "established", "down" ]
                                             // "disable" client is disabled
                                             // "connecting" IKE/SA negotiation in progress
                                             // "established" IPsec tunnel is up
                                             // "down" tunnel is down
        "serverip":"server IP",          // [ ip address ], resolved gateway IP
        "local_ts":"local subnet",       // [ string ], local traffic selector
        "remote_ts":"remote subnet",     // [ string ], remote traffic selector
        "livetime":"online time",        // [ string ], format hour:minute:second:day
        "rx_bytes":"received bytes",     // [ string ]
        "tx_bytes":"sent bytes"          // [ string ]
    }
    ```

    Example, get the first IPsec client status
    ```shell
    ipsec@client.status
    {
        "status":"established",
        "serverip":"203.0.113.1",
        "local_ts":"10.1.0.0/24",
        "remote_ts":"10.2.0.0/24",
        "livetime":"02:30:15:0",
        "rx_bytes":"123456",
        "tx_bytes":"654321"
    }
    ```


#### Control APIs

+ `reset[]` **restart the IPsec client**
    - succeed return ttrue
    - terminates current connection and re-initiates
    - behavior depends on the `extern` setting:
        - "default": restarts immediately
        - specific ifname: restarts only when the specified extern interface comes online

+ `import_ca[ file ]` **import a CA certificate**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.ca` in the ipsec project configuration directory

+ `import_cert[ file ]` **import a client certificate**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.cert`

+ `import_key[ file ]` **import a client private key**
    - file ------------ [ string ], uploaded temporary file path
    - succeed return ttrue
    - copies to `{instance}.key`

+ `clear_ca[]` **remove the CA certificate file**
    - succeed return ttrue

+ `clear_cert[]` **remove the client certificate file**
    - succeed return ttrue

+ `clear_key[]` **remove the client private key file**
    - succeed return ttrue

+ `key[]` **list existing certificate files for this instance**
    - succeed return [ json ]
    ```json
    {
        "ca":"path to client.ca",
        "cert":"path to client.cert",
        "key":"path to client.key"
    }
    ```
    Only keys for files that exist are included.
