## arch@firmware — Platform Firmware Upgrade Management

### Overview

**`arch@firmware`** handles **local and remote firmware upgrades** for embedded Linux products: FPK install, release upgrade archives, TFTP download, and OTA over HTTP/HTTPS/FTP. Callers pass a local file path or use network APIs; the component validates the device, writes flash, and updates system state.

- **`fpk`** installs a single local FPK package; **`zz`** applies a vendor **`.tar.gz`** upgrade archive built for this product
- **`tftp_upgrade`** and **`online_upgrade`** download firmware then delegate to **`fpk`** or **`zz`** as appropriate
- upgrade is blocked when **`arch@lock`** **`upgrade`** is **`enable`**, when another upgrade is in progress, or when device identity register keys are missing
    > **`fpk`**, **`zz`**, and upgrade paths that call them return **`Function locked`**; **`online_check`** is not affected by the upgrade lock
    > only one upgrade runs at a time; concurrent calls receive **`Update busy`**
- OTA URL, download credentials, and post-upgrade restart hint are **not** stored on **`arch@firmware`**; **`online_check`**, **`online_upgrade`**, and successful **`zz`** responses read them from **`arch@custom`** (see **Configuration reference ( arch@custom )** below)
- upgrade lock is stored on **`arch@lock`** (see **Configuration reference ( arch@lock )** below)
- during upgrade the component updates **`machine_state`** in the land register, invokes **`machine/status`**, and drives **`gpio@action`** for upgrade indication


### Configuration reference ( arch@custom )

The following **`arch@custom`** attributes are consumed by **`arch@firmware`**. Query and change them on **`arch@custom`**, not on **`arch@firmware`**.

```json
// Attributes introduction 
{
    "firmware_store": "OTA firmware repository base URL",       // [ string ], FTP, HTTP, or HTTPS; empty uses ftp://repo.ashyelf.com
    "firmware_store_user": "curl -u authentication string",   // [ string ], user:password form; empty uses dl:dl@ashyelf.com
    "upgrade_time": "seconds before suggested restart"          // [ string ], returned as zz "wait" on success; empty uses 90
}
```

#### Configuration example

Example, show arch@custom attributes used by arch@firmware
```shell
arch@custom
{
    "firmware_store":"ftp://repo.ashyelf.com",                # OTA repository base URL
    "firmware_store_user":"dl:dl@ashyelf.com",                  # credentials for online_check and online_upgrade
    "upgrade_time":"90"                                         # restart hint delay in seconds after zz flash success
}
```

#### Configuration settings example

Example, set the OTA firmware repository URL
```shell
arch@custom:firmware_store=ftp://firmware.example.com/releases
ttrue
```

Example, set the post-upgrade restart hint to 120 seconds
```shell
arch@custom:upgrade_time=120
ttrue
```

Example, merge set OTA and upgrade timing settings( include "firmware_store" "firmware_store_user" "upgrade_time" )
```shell
arch@custom|{"firmware_store":"ftp://firmware.example.com/releases","firmware_store_user":"dl:password@example.com","upgrade_time":"120"}
ttrue
```


### Configuration reference ( arch@lock )

**`arch@firmware`** consults the **`upgrade`** attribute on **`arch@lock`**. Query and change it on **`arch@lock`**, not on **`arch@firmware`**.

```json
// Attributes introduction 
{
    "upgrade": "disable or enable upgrade lock"                 // [ "disable","enable" ], blocks fpk, zz, and upgrade paths that apply firmware
                                       // "disable": upgrades allowed (default when unset)
                                       // "enable": upgrades rejected with Function locked
}
```

#### Configuration example

Example, show the upgrade lock used by arch@firmware
```shell
arch@lock:upgrade
"disable"
```

#### Configuration settings example

Example, enable the upgrade lock to block all firmware upgrades
```shell
arch@lock:upgrade=enable
ttrue
```

Example, disable the upgrade lock to allow firmware upgrades
```shell
arch@lock:upgrade=disable
ttrue
```



### API Reference

#### Management APIs


#### Query APIs

+ `online_check` **query the firmware store for version and changelog**   
    - failed return NULL
    - succeed return [ json ], version and download URL when a newer build exists; changelog for current and available releases
    ```json
    {
        "version": "newer release version",         // [ string ], present only when remote version differs from device register "version"
        "url": "firmware download URL",             // [ string ], present when a newer release is available
        "changelog":                               // { json }, release notes from the store manifest
        {
            "release key": "change text"            // [ string ]: { json }, nested notes from the manifest
            // "...":"..."  How many changelog entries show how many properties
        }
    }
    ```
    - Uses **`firmware_store`** and **`firmware_store_user`** from **`arch@custom`** (or built-in defaults when unset).
    - On failure **`errno`** may be **`ESRCH`** (missing device register keys) or **`EAGAIN`** (download or parse failed).

    Example, check for online updates
    ```shell
    arch@firmware.online_check
    {
        "version":"2.0.2",
        "url":"ftp://repo.ashyelf.com/pub/ashyelf/device_2.0.2.tar.gz",
        "changelog":
        {
            "2.0.2":"Bug fixes and improvements"
        }
    }
    ```


#### Control APIs

+ `fpk[ filename ]` **install an FPK package from a local file**
    - filename ----------- [ string ], path to the FPK file
    - failed return [ json ], **`status`** is **`failed`** with **`msg`** describing the error
    - succeed return [ json ], **`status`** is **`success`**
    ```json
    {
        "status": "success or failed",              // [ string ]: [ "success", "failed" ]
                                                    // "success": FPK installed
                                                    // "failed": see "msg"
        "msg": "error description"                  // [ string ], present when status is "failed"
    }
    ```

    Example, download an FPK package with curl then install it locally
    ```shell
    $ curl -u dl:dl@ashyelf.com -k -o /tmp/app.fpk "ftp://repo.ashyelf.com/pub/ashyelf/app.fpk"
      % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                     Dload  Upload   Total   Spent    Left  Speed
    100 2048k  100 2048k    0     0   512k      0  0:00:04  0:00:04 --:--:--  518k
    $ arch@firmware.fpk[ /tmp/app.fpk ]
    {
        "status":"success"
    }
    $ 
    ```

+ `zz[ filename ]` **upgrade from a local tar.gz release archive**
    - filename ----------- [ string ], path to the upgrade **`.tar.gz`** file
    - failed return [ json ], **`status`** is **`failed`** with **`msg`**; optional per-member summary in **`msgs`**
    - succeed return [ json ], **`status`** is **`success`**; may include **`restart`** and **`wait`** when reboot is recommended
    ```json
    {
        "status": "success or failed",              // [ string ]: [ "success", "failed" ]
        "msg": "overall error description",         // [ string ], present on failure
        "msgs":                                     // { json }, optional per-member outcome summary
        {
            "member name": "result text"            // [ string ], success or error text for that member
            // "...":"..."  How many members show how many properties
        },
        "restart": "true",                          // [ string ], present when caller should reboot the device
        "wait": "seconds before restart"            // [ string ], from arch@custom upgrade_time (default 90)
    }
    ```

    Example, download a release archive with curl then apply it locally
    ```shell
    $ curl -u dl:dl@ashyelf.com -k -o /tmp/mt7621_h721_std_v8.6.0402.zz "ftp://firmware.ashyelf.com/pub/h721/mt7621_h721_std_v8.6.0402.zz"
      % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                     Dload  Upload   Total   Spent    Left  Speed
    100 18.2M  100 18.2M    0     0  1120k      0  0:00:16  0:00:16 --:--:-- 1189k
    $ arch@firmware.zz[ /tmp/mt7621_h721_std_v8.6.0402.zz ]
    {
        "status":"success",
        "restart":"true",
        "wait":"90"
    }
    $ 
    ```

+ `tftp_upgrade[ server, filename, localip ]` **download firmware via TFTP and apply it**
    - server ------------- [ string ], TFTP server IP address
    - filename ----------- [ string ], remote file name on the TFTP server
    - localip ------------ [ string ], optional, temporary IPv4 address to assign on the LAN netdev before download
    - failed return tfalse
    - succeed return ttrue
    - Downloads to a temporary file, then applies it through **`fpk`** or **`zz`** as appropriate.
    - On failure **`errno`** reflects the download or upgrade result; check system logs when **`tfalse`**.

    Example, TFTP upgrade from a LAN server
    ```shell
    arch@firmware.tftp_upgrade[ 192.168.1.100, firmware.tar.gz, 192.168.1.2 ]
    ttrue
    ```

+ `online_upgrade[ url, user, cmd ]` **download firmware from a URL and apply it**
    - url ---------------- [ string ], optional, full download URL; default from **`arch@custom`** **`firmware_store`**
    - user --------------- [ string ], optional, curl **`-u`** credentials; default from **`arch@custom`** **`firmware_store_user`**
    - cmd ---------------- [ string ], optional, post-success action: **`restart`** for restart after 5 seconds, or a positive integer string for that many seconds
    - failed return tfalse
    - succeed return ttrue
    - Downloads with curl (up to 3 attempts, 300 s timeout), then runs **`zz`** on the downloaded file; sets **`machine_state`** to **`downloading`** during transfer.
    - On failure **`errno`** reflects the download or upgrade result; check system logs when **`tfalse`**.

    Example, OTA upgrade with automatic restart
    ```shell
    arch@firmware.online_upgrade[ , , restart ]
    ttrue
    ```

    Example, OTA upgrade from a specific URL
    ```shell
    arch@firmware.online_upgrade[ ftp://firmware.example.com/releases/device.tar.gz, dl:password@example.com, 10 ]
    ttrue
    ```

