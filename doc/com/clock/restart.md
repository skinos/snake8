## clock@restart — Scheduled system restart

Manages automatic system restarts by uptime, clock time, or idle/client conditions (`clock@restart`).

### Configuration ( `clock@restart` )
```json
// attribute introduction
{
    "mode":"Restart plan mode",                        // [ "disable", "age", "point", "idle" ]
                                                         // "disable": disable restart auto
                                                         // "age": maximum runtime to restart
                                                         // "point": fixed-point to restart
                                                         // "idle": idle specified period to restart
    "delay":"delay some second to work",               // [ number ], The unit is seconds, valid for all mode

    "age":"The maximum runtime",                               // [ number ], The unit is seconds, valid for "mode" is "age"

    "point_age":"The maximum runtime",                         // [ number ], valid for "mode" is "point"
    "point_hour":"Specifies when hour to restart",             // [ number ], valid for "mode" is "point"
    "point_minute":"Specifies when minute of hour to restart", // [ number ], valid for "mode" is "point"

    "idle_start":"Start runtime",                              // [ number ], valid for "mode" is "idle"
    "idle_time":"Idle time",                                   // [ number ], valid for "mode" is "idle"
    "idle_hour":"Specifies when hour to restart",              // [ number ], valid for "mode" is "idle"
    "idle_minute":"Specifies when minute of hour to restart",  // [ number ], valid for "mode" is "idle"
    "idle_age":"The maximum runtime"                           // [ number ], valid for "mode" is "idle"
}
```   

Example showing the configuration for age mode, it will automatically restart when system runs for 2880 seconds
```shell
clock@restart
{
    "mode":"age",
    "age":"2880"
}
```   

Example showing the configuration for point mode, it will restart at 23:45
```shell
clock@restart
{
    "mode":"point",
    "point_hour":"23",
    "point_minute":"45",
    "point_age":"2880"
}
```

Example disable the restart function
```shell
clock@restart:mode=disable
ttrue
```   

Example: set to automatically restart when system runs for 3600 seconds
```shell
clock@restart|{"mode":"age","age":"3600"}
ttrue
```   

Example: set to automatically restart at 03:30
```shell
clock@restart|{"mode":"point","point_hour":"03","point_minute":"30"}
ttrue
```


### Lifecycle API
+ `setup[]` **start the restart planner service when mode is active**, *succeed return ttrue*
    - After install, **`init`** usually runs **`clock@restart.setup`** at the **`app`** stage. For **`mode`** in **`age`**, **`point`**, or **`idle`**, starts the **`service`** child; **`disable`** skips the service.

+ `shut[]` **stop restart supervision**, *succeed return ttrue*
    - **`sdelete( COM_IDPATH )`**. **Not** run automatically on **`uninit`** in the default integration.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_clock_restart(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@restart", "mode") == NULL)
        return -1;
    ok = ssets_string("clock@restart", "disable", "mode");
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

/* Example: scall("clock@restart", "setup", NULL); — usually only from init */
```
