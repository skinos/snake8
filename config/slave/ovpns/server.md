## ovpns@server — OpenVPN server

### Overview

One OpenVPN server process, `ovpns@server`. Each client is a runtime object such as `ovpns@branch-a`, registered onto this component. The object name is not the certificate common name. The client field `cn` is the `ccd` file name and must match the certificate CN, or the login name when authentication uses a username.

`prj.json` declares `server` as an **`exe`** (standalone binary, same pattern as `agent@gtog`). On slave host install, `sdk_install` sets the binary **setuid root** so OpenVPN can create the tun/tap device while landos runs as a normal user. The factory configuration is `server.cfg`. `add` writes a default client configuration and `com_register`s it. Saving configuration restarts `ovpns@server`. The service process writes `server.conf`, every `ccd/<cn>`, and the revoke directory, then execs OpenVPN. That restart drops current sessions.

The server does not issue, store, or export client certificates. It imports only its own CA, certificate, and key (or one PKCS#12), an optional DH file, an optional TLS key, and, in static mode, one shared secret.

### Configuration reference ( ovpns@server )

```json
// Attributes introduction 
{
    "status":"enable or disable",                 // [ "disable","enable" ], default "disable"; "enable" starts OpenVPN in setup
    "extern":"listen interface",                  // [ string ], "disable" binds every address; otherwise an ifname such as "ifname@wan"
    "proto":"udp or tcp",                         // [ "udp","tcp" ], default "udp"
    "port":"listen port",                         // [ string ], default "1194"
    "dev":"tun or tap",                           // [ "tun","tap" ], default "tun"
    "topology":"subnet or net30",                 // [ "subnet","net30" ], default "subnet"; tun only
    "tunnelip":"tunnel network",                  // [ string ], network address, default "10.8.0.0"; the server occupies the first host
    "tunnelmask":"tunnel mask",                   // [ string ], default "255.255.255.0"
    "poolstart":"dynamic pool start",             // [ string ], empty with poolend means the whole subnet pool from --server; both set writes "server … nopool" plus ifconfig-pool (OpenVPN 2.6)
    "poolend":"dynamic pool end",                 // [ string ]
    "bridge":"tap local interface",               // [ string ], ifname such as "ifname@lan"; required when dev is tap
    "cipher":"ordered data ciphers",              // [ string ], colon list; allowed AES-256-GCM, AES-128-GCM, CHACHA20-POLY1305, AES-256-CBC, AES-128-CBC
    "authdigest":"HMAC digest",                   // [ "SHA256","SHA384","SHA512" ], default "SHA256"; written only when cipher contains CBC
    "auth":"authentication",                      // [ "certificate","username","certificate+username","static" ], default "certificate"
    "radius":                                     // [ json ], used only when auth includes a username
    {
        "status":"ask RADIUS",                    // [ "disable","enable" ], default "disable"
        "host":"RADIUS address",                  // [ string ]
        "secret":"shared secret",                 // [ string ]
        "port":"auth port",                       // [ string ], default "1812"
        "wait":"reply wait seconds",              // [ string ], default "5"
        "accounting":"session accounting",        // [ "disable","enable" ], default "disable"; ignored unless status is enable
        "acctport":"accounting port"              // [ string ], default "1813"
    },
    "hmac":"TLS key mode",                        // [ "disable","tls-auth","tls-crypt","tls-crypt-v2" ], default "tls-crypt"
    "pkcs12":                                     // [ json ]
    {
        "status":"use one PKCS#12",               // [ "disable","enable" ], default "disable"
        "password":"PKCS#12 password"             // [ string ], empty writes no askpass
    },
    "client2client":"clients see tunnel peers",   // [ "disable","enable" ], default "disable"
    "clientdup":"allow duplicate CN",             // [ "disable","enable" ], default "disable"
    "float":"keep session after public IP change",// [ "disable","enable" ], default "enable"
    "lzo":"comp-lzo",                             // [ "disable","adaptive","yes","no" ], default "disable"
    "listonly":"reject CN not in the client list",// [ "disable","enable" ], default "disable"; writes ccd/DEFAULT
    "redirect":"push default route",              // [ "disable","enable" ], default "disable"; tun only
    "dns":"pushed DNS",                           // [ string ]
    "dns2":"pushed second DNS",                   // [ string ]
    "local_net":                                  // [ json ], routes pushed to every client; tap ignores this
    {
        "name":                                   // [ string ]: { json }
        {
            "target":"network",                   // [ string ]
            "mask":"mask"                         // [ string ]
        }
        // "...":{ ... }  How many networks show how many properties
    },
    "interval":"keepalive ping seconds",          // [ string ], default "10"
    "timeout":"keepalive timeout seconds",        // [ string ], default "120"
    "maxclient":"max clients",                    // [ string ], default "50"
    "options":"extra server.conf lines",          // [ string ], appended after generated lines; split on ';' or newline
    "revoke":                                     // [ json ], decimal certificate serials; not cleared when a client object is deleted
    {
        "note":                                   // [ string ]: { json }
        {
            "serial":"decimal serial"             // [ string ], hex or openssl serial= is stored as decimal without leading zeros
        }
        // "...":{ ... }  How many serials show how many properties
    }
}
```

Client objects use the same component. `cn` empty means the object suffix. `tunnelip` empty takes an address from the pool. In `net30`, `tunnelip` and `tunnelpeer` are both empty or both the two hosts of one /30 outside the pool. `local_net` is pushed only to this client. `remote_net` is `iroute` plus a kernel `route`, and must not overlap another client. `redirect` empty follows the server. `push_reset` `enable` drops server pushes before this client's own routes and DNS. `password` is used only when `auth` includes a username and `radius.status` is `disable`. `status` `disable` writes `disable` in that client's ccd.

#### Configuration example

Example, show the server configure

```shell
ovpns@server
{
    "status":"enable",
    "extern":"disable",
    "proto":"udp",
    "port":"1194",
    "dev":"tun",
    "topology":"subnet",
    "tunnelip":"10.8.0.0",
    "tunnelmask":"255.255.255.0",
    "auth":"certificate",
    "hmac":"tls-crypt",
    "float":"enable",
    "local_net":
    {
        "r0":
        {
            "target":"192.168.1.0",
            "mask":"255.255.255.0"
        }
    }
}
```

Example, show one client

```shell
ovpns@branch-a
{
    "status":"enable",
    "cn":"branch-a.example.com",
    "tunnelip":"10.8.0.10",
    "remote_net":
    {
        "lan":
        {
            "target":"10.5.0.0",
            "mask":"255.255.255.0"
        }
    }
}
```

### API Reference

#### Management

+ `setup[]` **start the server**   
    - no parameter
    - registers every saved client object except `ovpns@server`
    - `status` `disable` stops the process and returns ttrue
    - succeed return ttrue; the service process loads config, checks it, writes runtime files, then execs OpenVPN

    Example, start the server
    ```shell
    ovpns@server.setup
    ttrue
    ```

+ `shut[]` **stop the server**   
    - no parameter
    - client objects and configuration stay
    - succeed return ttrue

    Example, stop the server
    ```shell
    ovpns@server.shut
    ttrue
    ```

+ `add[ name ]` **create a client object**   
    - name ------------- [ string ], object suffix; `server` is reserved
    - refused when `auth` is `static`, the name exists, or the name is empty
    - does not restart OpenVPN
    - failed return tfalse
    - succeed return [ string ], the object name

    Example, add branch-a
    ```shell
    ovpns@server.add[ branch-a ]
    ovpns@branch-a
    ```

+ `delete[ name ]` **remove a client object**   
    - name ------------- [ string ], `branch-a` or `ovpns@branch-a`
    - does not restart OpenVPN and does not clear `revoke`
    - failed return tfalse
    - succeed return ttrue

    Example, delete branch-a
    ```shell
    ovpns@server.delete[ branch-a ]
    ttrue
    ```

+ `import_ca[ file ]` **import the CA**   
    - file ------------- [ string ], uploaded file path
    - also `import_cert`, `import_key`, `import_tlsauth`, `import_dh`, `import_pkcs12`, `import_static`
    - failed return tfalse
    - succeed return ttrue

    Example, import the CA
    ```shell
    ovpns@server.import_ca[ /tmp/ca.crt ]
    ttrue
    ```

+ `clear_ca[]` **delete the CA file**   
    - also `clear_cert`, `clear_key`, `clear_tlsauth`, `clear_dh`, `clear_pkcs12`, `clear_static`
    - succeed return ttrue

    Example, delete the CA
    ```shell
    ovpns@server.clear_ca
    ttrue
    ```

#### Query

+ `clientlist[]` **list client objects**   
    - no parameter
    - does not include `ovpns@server`
    - note: exe reserves bare `list` for the method table; use `clientlist`
    - succeed return [ json ]
    ```json
    {
        "ovpns@branch-a":     // [ string ]: { json }, client object
        {
            "status":"enable",    // [ "enable", "disable" ]
            "cn":"branch-a",      // [ string ]
            "tunnelip":""         // [ string ]
        }
        // "...":{ ... }  How many clients show how many properties
    }
    ```

    Example, list clients
    ```shell
    ovpns@server.clientlist
    {
        "ovpns@branch-a":
        {
            "status":"enable",
            "cn":"branch-a",
            "tunnelip":"10.8.0.10"
        }
    }
    ```

+ `status[]` **server and online sessions**   
    - no parameter
    - `client` is keyed by the common name or username OpenVPN reports
    - `serial` comes from the client-connect note; it can be empty
    - succeed return [ json ]
    ```json
    {
        "status":"up",            // [ "up", "down" ]
        "client":                 // [ json ]
        {
            "branch-a":           // [ string ]: { json }
            {
                "name":"branch-a",    // [ string ]
                "real":"1.2.3.4:1194",// [ string ], public address and port
                "virtual":"10.8.0.10",// [ string ]
                "rx":"0",             // [ string ]
                "tx":"0",             // [ string ]
                "since":"",           // [ string ]
                "serial":"123"        // [ string ]
            }
        }
    }
    ```

    Example, read status
    ```shell
    ovpns@server.status
    {
        "status":"up",
        "client":{}
    }
    ```

+ `key[]` **which server files are present**   
    - keys are `ca`, `crt`, `key`, `tlsauth`, `dh`, `pkcs12`, `static`
    - a missing file is omitted
    - succeed return [ json ]

    Example, list imported files
    ```shell
    ovpns@server.key
    {
        "ca":"/var/config/ovpns/server.ca"
    }
    ```

#### Control

+ `disconnect[ real ]` **drop one session**   
    - real ------------- [ string ], the `real` value from `status`, `IP:port`
    - does not restart the server and does not write `revoke`
    - failed return tfalse
    - succeed return ttrue

    Example, drop one session
    ```shell
    ovpns@server.disconnect[ 1.2.3.4:1194 ]
    ttrue
    ```

+ `verify[ file ]` **check a username and password**   
    - file ------------- [ string ], OpenVPN via-file, username then password
    - local password when `radius.status` is `disable`; RADIUS PAP when `enable`
    - the login name must equal that client's `cn`
    - failed return tfalse
    - succeed return ttrue

    Example, verify a password file
    ```shell
    ovpns@server.verify[ /tmp/ovpns/up ]
    ttrue
    ```

+ `note[ file ]` **record a connect or disconnect**   
    - file ------------- [ string ]
    - first line `connect` stores the certificate serial for `status`
    - first line `disconnect` sends RADIUS Accounting-Stop when accounting is on
    - connect also sends Accounting-Start when accounting is on
    - a failed note still returns ttrue so the client is not refused

    Example, note a connect
    ```shell
    ovpns@server.note[ /tmp/ovpns/note ]
    ttrue
    ```

+ `online[]` **tap port joined the bridge**   
    - called from `network@frame.online` after the tunnel is up
    - `dev` `tap`: `ifname.ifdev` then `bridge.addif`
    - succeed return ttrue

+ `offline[]` **tap port left the bridge**   
    - `dev` `tap`: `ifname.ifdev` then `bridge.delif`
    - succeed return ttrue
