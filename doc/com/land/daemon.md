## Daemon Management
Manage the `daemon` executable and its watchdog behavior.

### **Configuration( `land@daemon` )**

The **saved configuration object** for `land@daemon` (watchdog, memory, local link checks, service scan interval). Consumed by the **`daemon`** executable at runtime.

```json
// Attributes introduction
{
    "service_check":"service scan interval",                           // [ number ], second

    "watchdog_file":"watchdog device file path",                       // [ string ], file path (for example /dev/watchdog)
    "watchdog_interval":"watchdog feed interval",                      // [ number ], microsecond

    "memory_check":"memory check interval",                            // [ number ], second
    "memory_warn":"warn threshold of free memory",                     // [ number ], kB
    "memory_reboot":"reboot threshold of free memory",                 // [ number ], kB

    "local_check":"network local check interval",                      // [ number ], second
    "local_disbuild":"reboot threshold when local is missing at setup",// [ number ], count
    "local_disappear":"reboot threshold when local disappears",        // [ number ], count
    "local_ifname":"object name that provides netdev API"              // [ string ], for example "ifname@local"
}
```

Examples, merge several tuning fields at once
```shell
land@daemon|{"service_check":"5","watchdog_interval":"1000000"}
ttrue
```

#### **Notes**
- `watchdog_interval` is parsed as microseconds, then converted into `tv_sec/tv_usec`.
- `memory_warn` and `memory_reboot` are compared with free memory in kB.
- `local_disbuild` and `local_disappear` are retry counts, not time duration.
- If `watchdog_file` is not set in config, daemon tries register variable `watchdog_file`.

### **Executable commands**

The **`daemon`** program (not HE methods on a long-running session) accepts these control lines:

#### **Shell Command Examples**
Stop daemon immediately:
```shell
daemon exit
```

Stop daemon after 15 seconds:
```shell
daemon stop15exit
```

Stop daemon after 600 seconds:
```shell
daemon delay600exit
```

Show flash id:
```shell
daemon flashid
```

#### **Configuration Examples**
Minimal service check only:
```json
{
    "service_check":"5"
}
```

Enable watchdog feed every 1 second:
```json
{
    "watchdog_file":"/dev/watchdog",
    "watchdog_interval":"1000000"
}
```

Enable memory and local watchdog policies:
```json
{
    "memory_check":"10",
    "memory_warn":"3000",
    "memory_reboot":"800",
    "local_check":"30",
    "local_disbuild":"20",
    "local_disappear":"10",
    "local_ifname":"ifname@local"
}
```

### **Lifecycle API**

+ **No** HE `setup[]` on `land@daemon` — configuration is read by the standalone **`daemon`** executable.
+ Treat **`land@daemon`** as persisted settings only.


### **Joint handlers**

**None** (not a joint subscriber).


### **Published joint events**

**None**.


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_land_daemon(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "land@daemon", "status") == NULL)
        return -1;
    return ssets_string("land@daemon", "enable", "status") ? 0 : -1;
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

/* e.g. scall("land@daemon", "list", NULL); talk_free if JSON */
```

