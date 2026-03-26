## FTP Server Management (ProFTPD)

Manage the FTP server using **`storage@ftp`**. Persistent defaults are merged from [`ftp.cfg`](ftp.cfg). Modifying configuration directly on disk is not recommended—prefer the HE API or Web UI.

### **Configuration( `storage@ftp` )**

```json
// Attributes introduction (see README.md for full field semantics)
{
    "status":"enable or disable the FTP service",     // [ "enable", "disable" ]
    "mode":"login mode",                              // [ "anonymous", "user" ]
    "root":"optional chroot base",                      // [ string ], default PROJECT_MNT_DIR
    "anonymous": { "path":"...", "permission":"..." },
    "user": { "share name": { "path":"...", "permission":"...", "user": { } } }
}
```

Example, show current FTP configuration
```shell
storage@ftp
```

Example, enable the service
```shell
storage@ftp:status=enable
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
storage@ftp|{"status":"enable","mode":"anonymous"}
ttrue
```

### **Component API**

+ `setup[]` **bring up FTP from saved configuration when enabled**, *succeed return ttrue*
    - Normally called during boot as **`storage@ftp.setup`** (via the installed package **init** schedule). If register **`platform`** is **`slave`**, FTP is intentionally skipped (debug log only) and the call still returns **`ttrue`**. If **`/usr/sbin/proftpd`** is absent, returns **`tfalse`**. When **`status`** is **`enable`**, registers and starts the **`service`** child (**`cstart`** → **`_service`**, which writes **`proftpd.conf`** and **`execlp`** **`proftpd`**).

+ `shut[]` **tear down the FTP service registration for this component**, *succeed return ttrue*
    - Calls **`sdelete( COM_IDPATH )`** to remove the component’s service registration. **`_set`** invokes **`_shut`** before **`config_set`** so a running **`proftpd`** instance is cleared before new JSON is applied; you may also call **`shut`** explicitly when your platform’s shutdown path runs component **`shut`** methods.


### **Lifecycle API**

+ `setup[]` — runs during **`init` → `app`** as **`storage@ftp.setup`** only. **No** default **`uninit`** entry for **`storage@ftp.shut`**.

+ `shut[]` — invoke manually, from **`_set`**, or add **`uninit`** in a product manifest.


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_storage_ftp(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "storage@ftp", "status") == NULL)
        return -1;
    ok = ssets_string("storage@ftp", "enable", "status");
    return ok ? 0 : -1;
}
```

**Call component methods**

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
        printf("%s failed, errno=%d\n", api, errno);
}

/* Example: scall("storage@ftp", "setup", NULL); */
```
