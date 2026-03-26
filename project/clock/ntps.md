## NTP Server management
Manage the NTP Server

### **Configuration( `clock@ntps` )**

```json
// attribute introduction
{
    "status":"NTP server status",      // [ "disable", "enable"]
    "local":"local network interface"  // [ "ifname@lan", "ifname@lan2", ... ], network ifname name for NTP server to bind, auto-detect if not set
}
```

Example, show the configure
```shell
clock@ntps
{
    "status":"enable"           # NTP server enable
}
```
Example, disable the NTP server
```shell
clock@ntps:status=disable
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
clock@ntps|{"status":"enable","local":"ifname@lan"}
ttrue
```

### **Lifecycle API**

+ `setup[]` **start or skip the NTP server from saved configuration**, *succeed return ttrue*
    - After install, **`init`** usually runs **`clock@ntps.setup`** at the **`general`** stage. If **`status`** is **`enable`**, starts the **`service`** child that runs **`ntpd`** bound to **`local`** (or auto-detected **`local_netdev`**).

+ `shut[]` **stop NTP server supervision for this component**, *succeed return ttrue*
    - **`sdelete( COM_IDPATH )`**. **Not** run automatically on **`uninit`** in the default integration; call explicitly if you need it on shutdown.


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_clock_ntps(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@ntps", "status") == NULL)
        return -1;
    ok = ssets_string("clock@ntps", "enable", "status");
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

/* Example: scall("clock@ntps", "setup", NULL); */
```
