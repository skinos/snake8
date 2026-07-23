## camera@osd — Camera OSD Overlay Management

### Overview

Push live device data to IP camera on-screen display (OSD) text overlays. The component reads overlay XML from a Hikvision camera (ISAPI), keeps a persistent placeholder template on the gateway, replaces `$keyword$` tokens with HE command results, and writes the result back when content changes.

- supports Hikvision ISAPI overlay GET/PUT on video input channel 1
- maps OSD placeholders to device HE commands via **`camera@osd2he`** (see [`osd2he.md`](osd2he.md))
- stores the camera placeholder template under project config path **`hikvision`** so PUT results do not erase `$keyword$` markers
- skips redundant PUT when resolved overlay XML is unchanged since the last successful update
- on gateway shutdown, writes a localized shutdown message to overlay slot 1 and clears other text slots


### Concepts

**Placeholder template**

Configure overlay text on the camera web UI with tokens such as `$L-S$` or `$T-O2$`. On GET, when the camera returns placeholders and the XML differs from the saved template, the gateway stores it at **`config/camera/hikvision`**. All replace operations read this file, not the live camera text (which already shows resolved values after PUT).

**Update loop**

When **`status`** is **`enable`**, the background **`service`** repeatedly: GET from camera → refresh template if placeholders changed → replace from template using **`camera@osd2he`** → PUT only when output differs from the last successful PUT. Failures wait 10 seconds and retry.

**HE value requirement**

Each mapping in **`camera@osd2he`** must resolve to a plain string (or empty string). If the HE command returns a JSON object, that keyword is not replaced and a warning is logged.


### Configuration reference ( camera@osd )

```json
// Attributes introduction 
{
    "status":"start OSD update service",                     // [ "disable","enable" ]
                                                                    // "enable" for run GET/replace/PUT loop
                                                                    // "disable" for stop service
    "camera":"camera driver type",                           // [ string ], currently supports "hikvision"
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



### API Reference

#### Management APIs

+ `setup[]` **initialize the OSD component and start the service when enabled**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - When **`status`** is **`enable`**, starts the **`service`** child process
    - Not intended for manual invocation

+ `shut[]` **stop the OSD service and write shutdown text to the camera when enabled**
    - failed return tfalse
    - succeed return ttrue
    - Stops the supervised **`service`** process
    - When configuration **`status`** is **`enable`**, calls the camera driver shutdown handler (slot 1 shows localized **Router Shutdown**, other text slots cleared)
    - Invoked on configuration change and system **`uninit`**

+ `service` **background GET, replace, and conditional PUT loop**
    - failed return tfalse
    - succeed return tfalse
    - Long-running worker started by **`setup`** when **`status`** is **`enable`**
    - Not intended for manual invocation
