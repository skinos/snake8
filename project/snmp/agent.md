## snmp@agent — SNMP Agent Management

### Overview

Manage the gateway **SNMP agent** using **net-snmp** `snmpd` (OpenWrt package **`snmpd`** from the swrt5 feeds). Prefer the **Component API** below instead of editing generated `snmpd.conf` when possible.
- When **`status`** is **`enable`**, a supervised child builds `snmpd.conf` and runs **`/usr/sbin/snmpd`** in the foreground
- Supports SNMPv1/v2c communities, optional SNMPv3 users, system identity fields, raw `snmpd.conf` lines, and custom OID shell extensions via net-snmp **`exec`**
- On **`platform=slave`**, the agent is skipped by design; `_setup` still returns success
    > `_setup` fails (`tfalse`) if **`/usr/sbin/snmpd`** is missing



### Configuration reference ( snmp@agent )

```json
// Attributes introduction 
{
    "status":"enable or disable the SNMP agent",     // [ "disable","enable" ]
    "port":"UDP listen port",                        // [ string ], empty uses 161

    "rocommunity":"read-only community",             // [ string ]
    "rwcommunity":"read-write community",            // [ string ]

    "v3":"SNMPv3 security level",                    // [ "disable","noauth","auth","privacy" ]
    "username":"SNMPv3 username",                    // [ string ], required when v3 is not disable
    "auth":"authentication digest",                  // [ "MD5","SHA" ], used when v3 is auth or privacy
    "password":"authentication password",            // [ string ], used when v3 is auth or privacy
    "privacy":"privacy cipher",                      // [ "DES","AES" ], used when v3 is privacy
    "key":"privacy password",                        // [ string ], used when v3 is privacy

    "name":"sysName",                                // [ string ], empty uses land@machine.status name
    "contact":"sysContact",                          // [ string ], empty uses default contact
    "location":"sysLocation",                        // [ string ], empty uses default location

    "options":"raw snmpd.conf lines",                // [ string ], multiple lines separated by ";"

    "custom":                                        // [ json ], custom OID shell extensions
    {
        "oid name":                                  // [ string ]: { json }, rule name
        {
            "oid":"OID pathname",                    // [ string ]
            "cmd":"shell command to execute"         // [ string ]
        }
        // "...":{ ... }  How many custom OID show how many properties
    }
}
```

#### Configuration example

Example, show all the SNMP agent configure

```shell
snmp@agent
{
    "status":"disable",                  # SNMP agent disabled at factory default
    "port":"161",                        # UDP listen port 161
    "rocommunity":"rocommunity",         # read-only community
    "rwcommunity":"rwcommunity",         # read-write community
    "v3":"disable",                      # SNMPv3 disabled
    "custom":                            # built-in custom OID shell extensions
    {
        "machine_model":
        {
            "oid":".1.3.6.1.4.1.2022.501",
            "cmd":"/bin/he land@machine.status:cmodel"
        },
        "firmware_version":
        {
            "oid":".1.3.6.1.4.1.2022.503",
            "cmd":"/bin/he land@machine.status:version"
        }
    }
}
```

#### Configuration settings example

Example, enable the SNMP agent

```shell
snmp@agent:status=enable
ttrue
```

Example, modify the read-only community to public

```shell
snmp@agent:rocommunity=public
ttrue
```

Example, merge set community and port( include "status" "port" "rocommunity" )

```shell
snmp@agent|{"status":"enable","port":"161","rocommunity":"public"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **apply saved SNMP configuration and start or skip the agent**
    - Normally called during boot as **`snmp@agent.setup`** (via the installed package **init** schedule)
    - On **`platform=slave`**, returns success without starting
    - If **`/usr/sbin/snmpd`** is absent, returns **`tfalse`**
    - If **`status`** is **`enable`**, stops OpenWrt procd **`snmpd`** when present, then starts the supervised **`service`** child (`cstart` → `_service` → generate config → **`execlp`** snmpd)
    - failed return tfalse
    - succeed return ttrue

    Example, run setup manually
    ```shell
    snmp@agent.setup
    ttrue
    ```

+ `shut[]` **stop the supervised SNMP agent for this component**
    - Calls **`sdelete( COM_IDPATH )`** to remove the service registration
    - Does not remove saved configuration
    - failed return tfalse
    - succeed return ttrue

    Example, shut down the SNMP agent
    ```shell
    snmp@agent.shut
    ttrue
    ```
