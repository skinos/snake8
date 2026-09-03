## camera@osd — Camera OSD Overlay Management

### Overview

Push live device data to IP camera on-screen display (OSD) text overlays. The component keeps a persistent placeholder template on the gateway, replaces `$keyword$` tokens with HE command results, and writes the result back when content changes.

- supports **Hikvision** ISAPI overlay GET/PUT on video input channel 1
- supports **Univision** (`uni` / `univision`) SDK `UNIV_API` login + `getOSDConfig` / `setOSDConfig` (OSD text stored as Base64)
- supports **ViewSheen** (`vst`) HTTP Digest CGI `configManager.cgi` for `VideoWidget[0].CustomTitle[1].Text` (pipe-separated lines)
- supports **Dahua** (`dahua` / `ajhua`) HTTP Digest CGI `configManager.cgi`, auto-selects `CustomTitle` / `UserDefinedTitle` field that contains `$...$`
- maps OSD placeholders to device HE commands via **`camera@osd2he`** (see [`osd2he.md`](osd2he.md))
- stores the camera placeholder template under project config path **`hikvision`**, **`univision`**, **`vst`**, or **`dahua`** so update results do not erase `$keyword$` markers
- skips redundant update when resolved overlay content is unchanged since the last successful update
- on service stop, restores the saved placeholder template to the camera
- refreshes the local template from the camera only when it is missing (for example after configuration apply)


### Configuration reference ( camera@osd )

```json
// Attributes introduction 
{
    "status":"start OSD update service",                     // [ "disable","enable" ]
                                                                    // "enable" for run GET/replace/update loop
                                                                    // "disable" for stop service
    "camera":"camera driver type",                           // [ string ], "hikvision", "univision" (match "uni"), "vst", or "dahua"/"ajhua"
    "address":"camera IP address or hostname",               // [ string ]
    "username":"camera login username",                        // [ string ]
    "password":"camera login password",                        // [ string ]
    "interval":"milliseconds between successful update cycles" // [ number ], default 5000 when empty or invalid
}
```


#### Configuration example

Example, show all the camera OSD configure
```shell
camera@osd
{
    "status":"enable",                  # run OSD update service
    "camera":"hikvision",               # Hikvision ISAPI driver
    "address":"192.168.8.64",           # camera IP address
    "username":"admin",                 # camera username
    "password":"12345",                 # camera password
    "interval":"15000"                  # update every 15000 ms after success
}
```

Example, Univision camera
```shell
camera@osd
{
    "status":"enable",
    "camera":"univision",
    "address":"192.168.8.68",
    "username":"admin",
    "password":"<camera-password>",
    "interval":"5000"
}
```

Example, ViewSheen camera
```shell
camera@osd
{
    "status":"enable",
    "camera":"vst",
    "address":"192.168.8.64",
    "username":"admin",
    "password":"<camera-password>",
    "interval":"5000"
}
```

Example, Dahua camera
```shell
camera@osd
{
    "status":"enable",
    "camera":"dahua",
    "address":"192.168.8.108",
    "username":"admin",
    "password":"<camera-password>",
    "interval":"5000"
}
```


#### Configuration settings example

Example, enable the camera OSD service
```shell
camera@osd:status=enable
ttrue
```

Example, set camera IP address
```shell
camera@osd:address=192.168.1.100
ttrue
```

Example, merge set camera connection and interval( include "address" "username" "password" "interval" )
```shell
camera@osd|{"address":"192.168.8.64","username":"admin","password":"12345","interval":"30000"}
ttrue
```



### Concepts

**Placeholder template**

Configure overlay text on the camera web UI with tokens such as `$L-S$` or `$T-O2$`. The gateway stores the template at **`config/camera/hikvision`** (Hikvision XML), **`config/camera/univision`** (Univision `setOSDConfig` JSON), **`config/camera/vst`** (ViewSheen Text), or **`config/camera/dahua`** (Dahua field path + Text). All replace operations read this file, not the live camera text (which already shows resolved values after update). The template is fetched from the camera only when the local file is missing. Applying **`camera@osd`** configuration stops the service, restores the template to the camera, removes the local copy, and lets the service bootstrap it again when enabled.

**Hikvision notes**

Match **`camera`** values containing **`hikvision`**. Bootstrap GETs `/ISAPI/System/Video/inputs/channels/1/overlays` and keeps the XML when it contains `$...$` in overlay text. Updates PUT the replaced XML to the same path.

**Univision notes**

Univision requires an RPC login (`md5(md5(password)+MZH3XS)`, `encryptType:0`, cookie + session) before OSD calls. `camera@gsuni.gb28281` reuses the same cookie/session under `/tmp/.camera.osd.uni.*` so replace and set share one login per cycle. Overlay custom / channelTitle `data` fields are Base64; the driver decodes them, replaces `$keyword$`, then re-encodes before `setOSDConfig`. Match **`camera`** values containing **`uni`** (for example **`univision`** or **`uni`**).

**ViewSheen notes**

ViewSheen uses HTTP Digest against `/cgi-bin/configManager.cgi`. Bootstrap reads `getConfig&name=VideoWidget` and keeps `CustomTitle[1].Text` when it contains `$...$` placeholders (typically pipe-separated, for example `$L-S$|$G-LAT$|$BATT$|$HUM$`). Updates call `setConfig` with the same field. Match **`camera`** values containing **`vst`**.

**Dahua notes**

Dahua uses the same Digest CGI family as ViewSheen, but the overlay slot varies by firmware (`CustomTitle[0..3]` or `UserDefinedTitle[0..1]`). Bootstrap scans `getConfig&name=VideoWidget` for the first preferred field whose Text contains `$...$`, then stores a two-part template: CGI field path on line 1, placeholder Text on the following lines. Updates call `setConfig` with `EncodeBlend=true` and that field's Text. Match **`camera`** values containing **`dahua`** or **`ajhua`**.

**Update loop**

When **`status`** is **`enable`**, the background **`service`** repeatedly: ensure local template exists (bootstrap from camera when missing) → replace from template using **`camera@osd2he`** → update camera only when output differs from the last successful update. Failures wait 10 seconds and retry.

**HE value requirement**

Each mapping in **`camera@osd2he`** must resolve to a plain string (or empty string). If the HE command returns a JSON object, that keyword is not replaced and a warning is logged. If the HE command fails or returns **NULL**, the placeholder is replaced with an empty string and a warning is logged.



### API Reference

#### Management APIs

+ `setup[]` **initialize the OSD component and start the service when enabled**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - When **`status`** is **`enable`**, starts the **`service`** child process
    - Not intended for manual invocation

+ `set` **save configuration, restore template to camera, refresh local template, restart service**
    - On every configuration apply: **`shut`** (stop service and restore saved template when previously enabled), remove local template file, save new configuration, **`setup`**
    - Does not branch on the new **`status`** value for template removal; removal always runs after **`shut`**

+ `shut[]` **stop the OSD service and restore the saved camera template when enabled**
    - failed return tfalse
    - succeed return ttrue
    - Stops the supervised **`service`** process
    - When configuration **`status`** is **`enable`**, restores the saved template (**`hikvision`** XML, **`univision`** JSON, **`vst`** Text, or **`dahua`** field+Text) to the camera
    - Invoked on configuration change and system **`uninit`**

+ `service` **background get, replace, and conditional update loop**
    - failed return tfalse
    - succeed return tfalse
    - Long-running worker started by **`setup`** when **`status`** is **`enable`**
    - Not intended for manual invocation
