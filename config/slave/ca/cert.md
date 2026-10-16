## ca@cert — Certificate Authority

### Overview

Issue the files OpenVPN and IPsec import. Subject fields (country, organization, common name, lifetime, key size, PKCS#12 password) are the configuration of `ca@cert`. Generating a file does not change that configuration.

Files are written in the project configuration directory:

- `ca.crt` / `ca.key` — CA certificate and CA private key
- `server.crt` / `server.key` / `server.p12` — server certificate, private key, and PKCS#12. The certificate is `nsCertType=server` and `serverAuth`, for an OpenVPN server or an IPsec local certificate
- `client-<name>.crt` / `client-<name>.key` / `client-<name>.p12` — client certificate. The common name is `<name>`. The certificate is `nsCertType=client` and `clientAuth`, for an OpenVPN client or an IPsec peer
- `server.tlsauth` — OpenVPN static key V1, for `tls-auth` and `tls-crypt`
- `server.tlsv2` — OpenVPN tls-crypt-v2 server key
- `server.static` — OpenVPN `--secret` static key
- `server.dh` — Diffie-Hellman parameters, size follows `bits`

`make_ca` replaces the CA and deletes the server certificate, server key, server PKCS#12, and every client file. TLS, static, and DH files are left in place. A new CA starts the serial at `01`.


### Configuration reference ( ca@cert )

```json
// Attributes introduction 
{
    "country":"country code",                     // [ string ], default "CN"
    "province":"state or province",               // [ string ], default "GD"
    "city":"city",                                // [ string ], default "ShenZhen"
    "org":"organization",                         // [ string ], default "Ashyelf"
    "ou":"organization unit",                     // [ string ], default "SkinOS"
    "email":"email address",                      // [ string ], default "dimmalex@gmail.com"; empty omits the field
    "cn":"CA common name",                        // [ string ], default "ca"
    "server_cn":"server common name",             // [ string ], default "server"
    "days":"certificate lifetime",                // [ string ], days, default "3650"; allowed 1..36500
    "bits":"RSA and DH size",                     // [ "2048","4096" ], default "2048"
    "password":"PKCS#12 password"                 // [ string ], empty exports a PKCS#12 with an empty password
}
```

#### Configuration example

Example, show the subject used when issuing certificates

```shell
ca@cert
{
    "country":"CN",                               # country code
    "province":"GD",                              # province
    "city":"ShenZhen",                            # city
    "org":"Ashyelf",                              # organization
    "ou":"SkinOS",                                # organization unit
    "email":"dimmalex@gmail.com",                 # email
    "cn":"ca",                                    # CA common name
    "server_cn":"server",                         # server common name
    "days":"3650",                                # lifetime days
    "bits":"2048",                                # RSA and DH size
    "password":""                                 # empty PKCS#12 password
}
```

#### Configuration settings example

Example, set the server common name

```shell
ca@cert:server_cn=vpn
ttrue
```

Example, merge the lifetime and key size

```shell
ca@cert|{"days":"3650","bits":"2048"}
ttrue
```


### API Reference

#### Query APIs

+ `key[]` **list CA, server, TLS, static and DH files**   
    - succeed return [ json ], only files that exist are present

    ```json
    {
        "ca":"CA certificate path",               // [ string ]
        "cakey":"CA private key path",            // [ string ]
        "crt":"server certificate path",          // [ string ]
        "key":"server private key path",          // [ string ]
        "p12":"server PKCS#12 path",              // [ string ]
        "tls":"tls-auth / tls-crypt key path",    // [ string ]
        "tlsv2":"tls-crypt-v2 server key path",   // [ string ]
        "static":"OpenVPN static key path",       // [ string ]
        "dh":"Diffie-Hellman parameter path"      // [ string ]
    }
    ```

    Example, list the files
    ```shell
    ca@cert.key
    {
        "ca":"/etc/config/ca/ca.crt",             # CA certificate
        "cakey":"/etc/config/ca/ca.key",          # CA private key
        "crt":"/etc/config/ca/server.crt",        # server certificate
        "key":"/etc/config/ca/server.key"         # server private key
    }
    ```

+ `list_client[]` **list client certificates**   
    - succeed return [ json ], one object per client name; missing key or PKCS#12 is omitted

    ```json
    {
        "client name":                            // [ string ]: { json }
        {
            "crt":"client certificate path",      // [ string ]
            "key":"client private key path",      // [ string ]
            "p12":"client PKCS#12 path"           // [ string ]
        }
        // "...":{ ... }  How many client show how many properties
    }
    ```

    Example, list clients
    ```shell
    ca@cert.list_client
    {
        "phone":                                  # client name, also the certificate CN
        {
            "crt":"/etc/config/ca/client-phone.crt",
            "key":"/etc/config/ca/client-phone.key",
            "p12":"/etc/config/ca/client-phone.p12"
        }
    }
    ```


#### Control APIs

+ `make_ca[]` **generate a CA certificate and CA private key**   
    - uses the saved subject, `days` and `bits`
    - deletes `server.crt`, `server.key`, `server.p12` and every `client-*` file
    - resets the CA serial to 01
    - failed return tfalse
    - succeed return ttrue

    Example, generate the CA
    ```shell
    ca@cert.make_ca
    ttrue
    ```

+ `make_server[]` **generate the server certificate, private key and PKCS#12**   
    - requires `ca.crt` and `ca.key`
    - common name is `server_cn`
    - certificate is an SSL server certificate (`serverAuth`)
    - PKCS#12 password is `password`; an empty password is allowed
    - failed return tfalse
    - succeed return ttrue

    Example, generate the server certificate
    ```shell
    ca@cert.make_server
    ttrue
    ```

+ `make_pkcs12[]` **pack the existing server certificate into PKCS#12**   
    - requires `ca.crt`, `server.crt` and `server.key`
    - failed return tfalse
    - succeed return ttrue

    Example, build server PKCS#12 again
    ```shell
    ca@cert.make_pkcs12
    ttrue
    ```

+ `make_tls[]` **generate an OpenVPN static key for tls-auth and tls-crypt**   
    - writes `server.tlsauth`
    - failed return tfalse
    - succeed return ttrue

    Example, generate the TLS key
    ```shell
    ca@cert.make_tls
    ttrue
    ```

+ `make_tlsv2[]` **generate an OpenVPN tls-crypt-v2 server key**   
    - writes `server.tlsv2`
    - needs the `openvpn` program
    - failed return tfalse
    - succeed return ttrue

    Example, generate the tls-crypt-v2 key
    ```shell
    ca@cert.make_tlsv2
    ttrue
    ```

+ `make_static[]` **generate an OpenVPN --secret static key**   
    - writes `server.static`
    - failed return tfalse
    - succeed return ttrue

    Example, generate the static key
    ```shell
    ca@cert.make_static
    ttrue
    ```

+ `make_dh[]` **generate Diffie-Hellman parameters**   
    - size is `bits` (2048 or 4096)
    - this call blocks until `openssl dhparam` finishes
    - failed return tfalse
    - succeed return ttrue

    Example, generate DH parameters
    ```shell
    ca@cert.make_dh
    ttrue
    ```

+ `add_client[ name ]` **generate a client certificate, private key and PKCS#12**   
    - name --------------- [ string ], 1..32 characters of letters, digits, `_` or `-`; this is the certificate CN
    - requires `ca.crt` and `ca.key`
    - replaces an existing client of the same name
    - failed return tfalse
    - succeed return ttrue

    Example, generate client phone
    ```shell
    ca@cert.add_client[ phone ]
    ttrue
    ```

+ `delete_client[ name ]` **delete one client certificate, key and PKCS#12**   
    - name --------------- [ string ], same character rules as `add_client`
    - failed return tfalse
    - succeed return ttrue

    Example, delete client phone
    ```shell
    ca@cert.delete_client[ phone ]
    ttrue
    ```

+ `clear_ca[]` **delete the CA and every certificate it signed**   
    - also deletes `ca.key`, `ca.srl`, `server.crt`, `server.key`, `server.p12` and every client file
    - does not delete TLS, static or DH files
    - succeed return ttrue

    Example, delete the CA
    ```shell
    ca@cert.clear_ca
    ttrue
    ```

+ `clear_cakey[]` **delete the CA private key**   
    - succeed return ttrue

    Example, delete the CA private key
    ```shell
    ca@cert.clear_cakey
    ttrue
    ```

+ `clear_cert[]` **delete the server certificate**   
    - succeed return ttrue

    Example, delete the server certificate
    ```shell
    ca@cert.clear_cert
    ttrue
    ```

+ `clear_key[]` **delete the server private key**   
    - succeed return ttrue

    Example, delete the server private key
    ```shell
    ca@cert.clear_key
    ttrue
    ```

+ `clear_pkcs12[]` **delete the server PKCS#12**   
    - succeed return ttrue

    Example, delete the server PKCS#12
    ```shell
    ca@cert.clear_pkcs12
    ttrue
    ```

+ `clear_tls[]` **delete the tls-auth / tls-crypt key**   
    - succeed return ttrue

    Example, delete the TLS key
    ```shell
    ca@cert.clear_tls
    ttrue
    ```

+ `clear_tlsv2[]` **delete the tls-crypt-v2 server key**   
    - succeed return ttrue

    Example, delete the tls-crypt-v2 key
    ```shell
    ca@cert.clear_tlsv2
    ttrue
    ```

+ `clear_static[]` **delete the OpenVPN static key**   
    - succeed return ttrue

    Example, delete the static key
    ```shell
    ca@cert.clear_static
    ttrue
    ```

+ `clear_dh[]` **delete the Diffie-Hellman parameters**   
    - succeed return ttrue

    Example, delete DH parameters
    ```shell
    ca@cert.clear_dh
    ttrue
    ```

+ `import_ca[ file ]` **import a CA certificate**   
    - file --------------- [ string ], uploaded file path, copied to `ca.crt`
    - failed return tfalse
    - succeed return ttrue

    Example, import a CA certificate
    ```shell
    ca@cert.import_ca[ /tmp/ca.crt ]
    ttrue
    ```

+ `import_cakey[ file ]` **import a CA private key**   
    - file --------------- [ string ], copied to `ca.key`
    - failed return tfalse
    - succeed return ttrue

    Example, import a CA private key
    ```shell
    ca@cert.import_cakey[ /tmp/ca.key ]
    ttrue
    ```

+ `import_cert[ file ]` **import a server certificate**   
    - file --------------- [ string ], copied to `server.crt`
    - failed return tfalse
    - succeed return ttrue

    Example, import a server certificate
    ```shell
    ca@cert.import_cert[ /tmp/server.crt ]
    ttrue
    ```

+ `import_key[ file ]` **import a server private key**   
    - file --------------- [ string ], copied to `server.key`
    - failed return tfalse
    - succeed return ttrue

    Example, import a server private key
    ```shell
    ca@cert.import_key[ /tmp/server.key ]
    ttrue
    ```

+ `import_pkcs12[ file ]` **import a server PKCS#12**   
    - file --------------- [ string ], copied to `server.p12`
    - failed return tfalse
    - succeed return ttrue

    Example, import a server PKCS#12
    ```shell
    ca@cert.import_pkcs12[ /tmp/server.p12 ]
    ttrue
    ```

+ `import_tls[ file ]` **import a tls-auth / tls-crypt key**   
    - file --------------- [ string ], copied to `server.tlsauth`
    - failed return tfalse
    - succeed return ttrue

    Example, import a TLS key
    ```shell
    ca@cert.import_tls[ /tmp/ta.key ]
    ttrue
    ```

+ `import_tlsv2[ file ]` **import a tls-crypt-v2 server key**   
    - file --------------- [ string ], copied to `server.tlsv2`
    - failed return tfalse
    - succeed return ttrue

    Example, import a tls-crypt-v2 key
    ```shell
    ca@cert.import_tlsv2[ /tmp/tlsv2.key ]
    ttrue
    ```

+ `import_static[ file ]` **import an OpenVPN static key**   
    - file --------------- [ string ], copied to `server.static`
    - failed return tfalse
    - succeed return ttrue

    Example, import a static key
    ```shell
    ca@cert.import_static[ /tmp/static.key ]
    ttrue
    ```

+ `import_dh[ file ]` **import Diffie-Hellman parameters**   
    - file --------------- [ string ], copied to `server.dh`
    - failed return tfalse
    - succeed return ttrue

    Example, import DH parameters
    ```shell
    ca@cert.import_dh[ /tmp/dh.pem ]
    ttrue
    ```
