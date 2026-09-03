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

**Signal bars**

Use **`camera@osd2he.signal2flag[ <he> ]`** when a placeholder should show LTE-style bar glyphs instead of a raw number. The inner HE command must return a plain string or number in the range **0–5** (for example **`ifname@lte.status:signal`**). Level **0** or a failed resolve is shown as **`✕`** (no signal); levels **1–5** map to **`▂`**, **`▂▄`**, **`▂▄▆`**, **`▂▄▆█`**.

**Language translation**

Use **`camera@osd2he.lang[ <text> ]`** to translate a literal string, or
**`camera@osd2he.lang[ ,<he> ]`** when the first parameter is empty: the second
parameter is executed as HE, and its string result is translated. Translation uses
the camera project language JSON (`cn.json` / `en.json`), selected by
**`land@machine:language`**. Keys are English source strings; values are the
localized text. Missing keys or a missing language file return the original string
unchanged.


### Configuration reference ( camera@osd2he )

```json
// Attributes introduction 
{
    "$L-S$":"LTE signal strength",                              // [ string ], HE for LTE signal field
    "$L-P$":"LTE IP address",                                  // [ string ], HE for LTE IP field
    "$L-G$":"LTE gateway",                                     // [ string ], HE for LTE gateway field
    "$L-O$":"LTE operator name",                               // [ string ], HE for LTE operator field
    "$G-LAT$":"GNSS latitude",                                  // [ string ], HE for latitude (decimal degrees)
    "$G-LON$":"GNSS longitude",                                 // [ string ], HE for longitude (decimal degrees)
    "$G-ALT$":"GNSS altitude",                                  // [ string ], HE for altitude in metres (GGA)
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
    "$G-LAT$":"gnss@nmea.status:pos/lat",      # latitude → $G-LAT$
    "$G-LON$":"gnss@nmea.status:pos/lon",      # longitude → $G-LON$
    "$G-ALT$":"gnss@nmea.status:pos/alt",      # altitude → $G-ALT$
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
camera@osd2he|{"$L-S$":"camera@osd2he.signal2flag[ifname@lte.status:signal]","$L-P$":"ifname@lte.status:ip","$T-O2$":"uart@tty.status:02"}
ttrue
```


### API Reference

#### Query APIs

+ `signal2flag[ he ]` **map signal level 0–5 to bar glyphs for OSD**
    - he --------------- [ string ], HE command that returns signal level as string or number (e.g. `ifname@lte.status:signal`)
    - failed return NULL
    - succeed return [ string ], `✕` for level 0 or resolve failure; `▂` … `▂▄▆█` for levels 1–5

    Example, LTE signal bars for OSD placeholder mapping
    ```shell
    camera@osd2he.signal2flag[ifname@lte.status:signal]
    ▂▄▆
    ```

    Example, use in osd2he mapping (default in osd2he.cfg)
    ```shell
    camera@osd2he:$L-S$=camera@osd2he.signal2flag[ifname@lte.status:signal]
    ttrue
    ```

+ `lang[ text ]` / `lang[ , he ]` **translate a string with project i18n (cn/en)**
    - text ------------- [ string ], language key (English source text in `cn.json` / `en.json`); when empty, use **he**
    - he --------------- [ string ], HE command whose string result is used as the language key (only when **text** is empty)
    - failed return NULL
    - succeed return [ string ], translated text for the current system language; original text when the key or language file is missing

    Example, translate a literal key
    ```shell
    camera@osd2he.lang[offline]
    离线
    ```

    Example, translate the result of an HE command
    ```shell
    camera@osd2he.lang[,camera@gsuni.gb28281]
    离线
    ```

    Example, use in osd2he mapping
    ```shell
    camera@osd2he:$GB$=camera@osd2he.lang[,camera@gsuni.gb28281]
    ttrue
    ```
