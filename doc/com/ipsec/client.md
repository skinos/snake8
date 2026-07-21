## ipsec@client — IPsec Client Instance Management

### Overview

Manage an individual IPsec VPN client connection using strongSwan. Each instance (`ipsec@client`, `ipsec@client2`, …) creates a policy-based IPsec tunnel to a remote gateway (no dedicated tunnel netdev such as `ipsec0`). Instances are created and managed by [`ipsec@list`](list.md).

- manages IPsec tunnel lifecycle: setup (swanctl --initiate), shutdown (swanctl --terminate)
- generates strongSwan `swanctl.conf` from instance configuration (CHILD `start_action` is always `none`, `dpd_action` is always `restart`; landos `_service` initiates after load-all)
- supports PSK and certificate authentication (IKEv1 Main/Aggressive Mode, IKEv2)
- configures traffic selectors (empty local_ts => ifname@lan subnet; empty remote_ts omitted)
- handles IKE/ESP encryption proposals, DPD, rekeying
- monitors connection state via swanctl --list-sas
- resolves server domain to IP and routes via the specified extern interface
- restarts via `reset[]` when the depend extern / default gateway comes online again



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
    "aggressive":"IKEv1 exchange mode",                        // [ "enable", "disable" ], default "disable"
                                                                    // enable: Aggressive Mode (IKEv1 only; swanctl aggressive=yes)
                                                                    // disable: Main Mode (IKEv1 default); ignored for IKEv2
    "natt":"NAT traversal",                                    // [ "auto", "force" ], default "auto"
                                                                    // auto: strongSwan detects NAT and uses UDP-encap when needed
                                                                    // force: always UDP-encap ESP (swanctl encap=yes)

    // Authentication
    "auth_method":"authentication method",                     // [ "psk", "pubkey" ]
                                                                    // "psk" for pre-shared key
                                                                    // "pubkey" for certificate-based
    "local_id":"local identifier",                             // [ string ], optional, local IKE ID (IP, FQDN, email)
                                                                    // empty: use extern / default-gateway interface IP
    "remote_id":"remote identifier",                           // [ string ], optional, remote IKE ID
                                                                    // empty: omit (swanctl remote.id defaults to %any)

    // PSK authentication (when auth_method is "psk")
    "psk":"pre-shared key",                                    // [ string ]

    // Certificate authentication (when auth_method is "pubkey")
    // Certificate files are stored in the ipsec project configuration directory,
    // named after the instance object: client.ca / client.crt / client.key for
    // ipsec@client, client2.* for ipsec@client2, etc. Use import_*/clear_* APIs
    // or the web UI to upload, download, and delete these files.

    // Assign IP / Hybrid Mode Config (UI: "Assign IP (Hybrid)")
    "vip":"request virtual IP from server",                    // [ "enable", "disable" ], default "disable"
                                                                    // enable: swanctl vips = 0.0.0.0 (Hybrid / roadwarrior pools)
                                                                    // disable: site-to-site (no VIP request); not needed just because TS is set

    // Child SA / Traffic selectors
    "local_ts":"local traffic selector",                       // [ string ], optional; empty => ifname@lan subnet (e.g. 192.168.1.0/24)
                                                                    // comma-separated for multiple selectors (IKEv2)
    "remote_ts":"remote traffic selector",                     // [ string ], optional; empty => omit (not 0.0.0.0/0)
                                                                    // comma-separated for multiple selectors (IKEv2)

    // Encryption proposals
    "ike_proposals":"IKE cipher suites",                       // [ string ], optional, e.g. "aes256-sha256-modp2048"
                                                                    // (Phase 1; DH: 2=modp1024, 5=modp1536, 14=modp2048, 15=3072, 19=ecp256, ...)
    "esp_proposals":"ESP cipher suites",                       // [ string ], optional, e.g. "aes256-sha256"
                                                                    // with PFS (Phase 2): append DH group, e.g. "aes256-sha256-modp2048"

    // Lifetime (Phase 1 / Phase 2). Values are seconds if no unit; s/m/h/d also accepted.
    // Web UI exposes rekey_time and esp_rekey only (seconds placeholders); reauth_time / esp_life are HE/advanced.
    "rekey_time":"IKE rekey interval",                         // [ string ], optional, e.g. "10800" or "3h" (Phase 1)
    "reauth_time":"IKE reauthentication interval",             // [ string ], optional, e.g. "0" to disable (not in web UI)
    "esp_rekey":"ESP rekey interval",                          // [ string ], optional, e.g. "3600" or "1h" (Phase 2)
    "esp_life":"ESP hard lifetime",                            // [ string ], optional (not in web UI); empty => strongSwan default from esp_rekey

    // DPD (Dead Peer Detection)
    "dpd_delay":"DPD check interval",                          // [ string ], optional, e.g. "30" or "30s"
    "dpd_timeout":"DPD timeout",                               // [ string ], optional, e.g. "150" or "150s"

    // Advanced swanctl snippets (optional)
    "connections_opt":"extra connection options",              // [ string ], optional, raw swanctl.conf fragment
                                                                    // written to /etc/swanctl/conf.d/<object>.conn
                                                                    // and included inside the connection block
                                                                    // (after children), e.g. custom marks / includes
    "secret_opt":"extra secret options",                       // [ string ], optional, raw swanctl.conf fragment
                                                                    // written to /etc/swanctl/conf.d/<object>.sec
                                                                    // and included inside the ike-/private- secret block
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
    "dpd_delay":"30s"
}
```

Certificate authentication uses files in the ipsec project configuration directory, named after the instance object: **`client.crt`**, **`client.key`**, and **`client.ca`** for `ipsec@client`; **`client2.crt`**, etc. for `ipsec@client2`. The runtime resolves them via `config_path()`; a file that exists is treated as configured.

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
    - terminates the IPsec connection via swanctl --terminate


#### Query APIs

+ `status[]` **get IPsec client status**
    - instance does not exist: return NULL
    - instance exists but config `status` is not `"enable"`: return `{ "status":"disable" }`
    - enabled: return [ json ], parsed from `swanctl --list-sas --ike <object> --pretty`
    ```json
    {
        "status":"Current state",        // [ "disable", "connecting", "established", "down" ]
                                         // disable: config status is not enable
                                         // established: ike_state=ESTABLISHED and child_state=INSTALLED
                                         // connecting: IKE up but child not INSTALLED, or IKE still negotiating
                                         // down: no usable IKE SA
        "serverip":"server IP",          // [ ip ], resolved gateway or remote-host
        "ip":"virtual IP",               // [ ip ], local-vips (when vip/Hybrid assigned)
        "local_id":"local IKE ID",
        "remote_id":"remote IKE ID",
        "local_host":"local outer IP",
        "remote_host":"remote outer IP",
        "ike_state":"IKE SA state",      // Phase 1, e.g. ESTABLISHED
        "child_state":"CHILD SA state",  // Phase 2 / ESP, e.g. INSTALLED
        "ike_version":"2",
        "ike_proposal":"IKE proposal",
        "esp_proposal":"ESP proposal",
        "local_ts":"local traffic selector",
        "remote_ts":"remote traffic selector",
        "rx_bytes":"bytes in",
        "tx_bytes":"bytes out",
        "rx_packets":"packets in",
        "tx_packets":"packets out",
        "established":"IKE age seconds",
        "livetime":"online time",        // hour:minute:second:day
        "install_time":"CHILD age seconds",
        "spi_in":"inbound SPI",
        "spi_out":"outbound SPI",
        "mode":"TUNNEL",
        "protocol":"ESP",
        "reqid":"reqid"
    }
    ```

    Example, get the first IPsec client status
    ```shell
    ipsec@client.status
    {
        "status":"established",
        "serverip":"203.0.113.1",
        "ip":"10.10.10.2",
        "local_id":"ipsec-client",
        "remote_id":"ipsec-server",
        "ike_state":"ESTABLISHED",
        "child_state":"INSTALLED",
        "rx_bytes":"924",
        "tx_bytes":"0",
        "livetime":"00:03:27:0"
    }
    ```


#### Control APIs

+ `reset[]` **restart the IPsec client**
    - succeed return ttrue
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
    - copies to `{instance}.crt`

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
        "crt":"path to client.crt",
        "key":"path to client.key"
    }
    ```
    Only keys for files that exist are included.
