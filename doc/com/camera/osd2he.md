## camera@osd2he — OSD Placeholder to HE Mapping

### Overview

Configuration object that maps camera OSD placeholder tokens to HE commands. The **`camera@osd`** component loads this mapping at service start and uses it during overlay text replacement.

- keys are placeholder strings as configured on the camera (for example **`$L-S$`**, **`$T-O2$`**)
- values are HE command strings executed on the gateway (for example **`ifname@lte.status:signal`**)
- use **`component.method:field`** syntax when only one JSON field from the method result is needed
- each command must return a plain string suitable for a single-line OSD field; JSON objects are rejected


### Concepts

**Placeholder format**

Place the key literally in a Hikvision **`<displayText>`** field on the camera web UI, for example **`$L-S$`**. The gateway detects **`$...$`** in fetched overlay XML and refreshes the saved template when the camera configuration changes.

**HE command form**

| Form | Example | Use when |
|------|---------|----------|
| Method field | `ifname@lte.status:signal` | One field from method JSON is enough |
| Plain method | `land@machine.status:uptime` | Same **`method:field`** pattern |

If the command returns a JSON **object** instead of a scalar string, **`camera@osd`** logs a warning and leaves that placeholder unchanged in the output overlay.


### Configuration reference ( camera@osd2he )

```json
// Attributes introduction 
{
    "$L-S$":"LTE signal strength",                              // [ string ], HE for LTE signal field
    "$L-P$":"LTE IP address",                                  // [ string ], HE for LTE IP field
    "$L-G$":"LTE gateway",                                     // [ string ], HE for LTE gateway field
    "$L-O$":"LTE operator name",                               // [ string ], HE for LTE operator field
    "$T-O2$":"UART sensor O2",                                // [ string ], HE for sensor channel
    "$T-CO$":"UART sensor CO",                                 // [ string ], HE for sensor channel
    "$T-H2S$":"UART sensor H2S",                              // [ string ], HE for sensor channel
    "$T-CH4$":"UART sensor CH4",                              // [ string ], HE for sensor channel
    "$T-C6H6$":"UART sensor C6H6",                             // [ string ], HE for sensor channel
    "$T-H2$":"UART sensor H2",                                 // [ string ], HE for sensor channel
    // "...":"..."  How many placeholders show how many properties
}
```


#### Configuration example

Example, show all the OSD placeholder mapping
```shell
camera@osd2he
{
    "$L-S$":"ifname@lte.status:signal",       # LTE signal → $L-S$
    "$L-P$":"ifname@lte.status:ip",           # LTE IP → $L-P$
    "$L-G$":"ifname@lte.status:gateway",      # LTE gateway → $L-G$
    "$L-O$":"ifname@lte.status:operator",      # LTE operator → $L-O$
    "$T-O2$":"uart@tty.status:02",             # O2 sensor → $T-O2$
    "$T-CO$":"uart@tty.status:CO",             # CO sensor → $T-CO$
    "$T-H2S$":"uart@tty.status:H2S",           # H2S sensor → $T-H2S$
    "$T-CH4$":"uart@tty.status:CH4",           # CH4 sensor → $T-CH4$
    "$T-C6H6$":"uart@tty.status:C6H6",         # C6H6 sensor → $T-C6H6$
    "$T-H2$":"uart@tty.status:H2"              # H2 sensor → $T-H2$
}
```

#### Configuration settings example

Example, map LTE signal placeholder to HE command
```shell
camera@osd2he:$L-S$=ifname@lte.status:signal
ttrue
```

Example, map O2 sensor placeholder to HE command
```shell
camera@osd2he:$T-O2$=uart@tty.status:02
ttrue
```

Example, merge set several placeholder mappings( include "$L-S$" "$L-P$" "$T-O2$" )
```shell
camera@osd2he|{"$L-S$":"ifname@lte.status:signal","$L-P$":"ifname@lte.status:ip","$T-O2$":"uart@tty.status:02"}
ttrue
```
