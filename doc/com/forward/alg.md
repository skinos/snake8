## forward@alg — Application Layer Gateway Management

### Overview

Load and unload ALG (Application Layer Gateway) kernel helpers. Prefer the **Component API** below instead of editing raw configuration when possible.

- each ALG protocol (FTP, SIP, H.323, …) is controlled by an enable/disable flag
- skipped in **default** / **parasite** network modes



### Configuration reference ( forward@alg )

```json
// Attributes introduction 
{
    "amanda":"amanda ALG function",            // [ "disable", "enable" ]
    "ftp":"ftp ALG function",                  // [ "disable", "enable" ]
    "h323":"h323 ALG function",                // [ "disable", "enable" ]
    "irc":"irc ALG function",                  // [ "disable", "enable" ]
    "pptp":"pptp ALG function",                // [ "disable", "enable" ]
    "gre":"gre ALG function",                  // [ "disable", "enable" ]
    "sip":"sip ALG function",                  // [ "disable", "enable" ]
    "rtsp":"rtsp ALG function",                // [ "disable", "enable" ]
    "snmp":"snmp ALG function",                // [ "disable", "enable" ]
    "tftp":"tftp ALG function",                // [ "disable", "enable" ]
    "udplite":"udplite ALG function"           // [ "disable", "enable" ]
}
```

#### Configuration example

Example, show current all ALG settings

```shell
forward@alg
{
    "amanda":"disable",
    "ftp":"enable",
    "h323":"disable",
    "irc":"disable",
    "pptp":"enable",
    "gre":"enable",
    "rtsp":"enable",
    "sip":"enable",
    "snmp":"disable",
    "tftp":"disable"
}
```

#### Configuration settings example

Example, disable the FTP ALG

```shell
forward@alg:ftp=disable
ttrue
```

Example, merge several ALG settings at once

```shell
forward@alg|{"ftp":"enable","sip":"disable","h323":"enable"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **load ALG kernel helpers from configuration**
    - succeed return ttrue
    - skipped in **default** / **parasite** network modes
    - normally scheduled as **`init` → `app` → `forward@alg.setup`**

+ `shut[]` **unload all ALG kernel helpers**
    - succeed return ttrue
    - removes all ALG-related kernel modules regardless of configuration


#### Query APIs

+ `status[]` **get the current ALG configuration**
    - failed return NULL
    - succeed return [ json ], same shape as configuration

    Example, get ALG settings

    ```shell
    forward@alg.status
    {
        "ftp":"enable",
        "sip":"disable"
    }
    ```
