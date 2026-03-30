## clock@date — System date and time

Manage system date and time, including timezone, manual set, NTP client sync, and status reporting (`clock@date`).

### Configuration ( `clock@date` )
```json
// Attributes introduction
{
    "timezone":"time zone",                             // [ number ], -12 to 12, West 12 to East 12, support half time zone like 3:30, -3:30
    "ntpclient":"whether to start the NTP client",      // [ "disable", "enable" ]
    "ntpserver":"NTP Server",                           // [ string ]
    "ntpserver2":"NTP Server 2",                        // [ string ]
    "ntpserver3":"NTP Server 3",                        // [ string ]
    "ntpinterval":"NTP Synchronization interval"        // [ number ], interval (in seconds) for time synchronization with the NTP server
}
```   

Example, show all the configure
```shell
clock@date
{
    "timezone":"8",                   # time zone is East 8, china
    "ntpclient":"enable",             # enable the NTP client to synchronize with NTP server
    "ntpserver":"ntp1.aliyun.com",    # ntp1.aliyun.com, ntp2.aliyun.com, ntp3.aliyun.com, Try in turn until you succeed
    "ntpserver2":"ntp2.aliyun.com",
    "ntpserver3":"ntp3.aliyun.com",
    "ntpinterval":"86400"             # synchronization every 86400 seconds
}
```  
Example, modify the time zone to West 5
```shell
clock@date:timezone=-5
ttrue
```  
Example, disable the NTP client time synchronization
```shell
clock@date:ntpclient=disable
ttrue
```  
Examples, change several attributes at once (**merge** — only listed fields are updated; **`|`** plus JSON object)
```shell
clock@date|{"timezone":"8","ntpclient":"enable","ntpserver":"pool.ntp.org"}
ttrue
```


### Component API
+ `status[]` **get the date information**
    - return NULL when failed
    - return terror when error
    - return json to describe date information when succeed
    ```json
    // Attributes introduction of json by the API return
    {
        "source":"The source of the time",                // [ "ntp", "set", "lte", "gps" ]
                                                             // ntp: indicates that it originated from NTP, which has the highest NTP priority, and NTP synchronization success covers all other times
                                                             // set: indicates time set by manually
                                                             // rtc: indicates source RTC time  
                                                             // lte: indicates source LTE time 
                                                             // gps: indicates source GPS time 
                                                             // An empty or none of this node indicates that it has not been set
        "current":"current date",                         // [ string ], format is hour:minute:second:month:day:year
        "livetime":"system live time",                    // [ string ], format is hour:minute:second:day
        "uptime":"system uptime in second"                // [ number ]
    }    
    ```   
    Example, get the current date   
    ```shell
    clock@date.status
    {
        "current":"12:29:41:05:10:2022",         # current is 12:29:41 on May 10, 2022
        "livetime":"00:01:58:0",                 # system run 1 minute and 58 second
        "uptime":"118"                           # system run 118 second
    }
    ```   

+ `current[ [current date], [time zone] ]` **set current date or get current time**
    - [current date] ------ [ string ], format is hour:minute:second:month:day:year
    - [time zone] --------- [ number ], -12 to 12, West 12 to East 12, support half time zone like 3:30, -3:30
    - return ttrue for succeed when set the current date or time zone
    - return tfalse for failed when set the current date or time zone 
    - return json to describes current time when no argument and succeed
    - return NULL when no argument and failed to get current time
    ```json
    {
        "sec":"The number of seconds since 1970.01.01:00:00:00",  // [ number ]
        "usec":"current microsecond",                             // [ number ] 
        "minuteswest":"Minutes west of Greenwich",                // [ number ]
        "dsttime":"type of DST correction"                        // [ number ]
    }
    ```   

    Example, set current date 11:12:23, On July 8th, in 2019   
    ```shell
    clock@date.current[ 11:12:23:07:08:2019 ]
    ttrue
    ```   
    Example, set time zone to china   
    ```shell
    clock@date.current[ , 8 ]
    ttrue
    ```   
    Example, get current time
    ```shell
    clock@date.current
    {
        "sec":"1747327771",
        "usec":"803417",
        "minuteswest":"-480",
        "dsttime":"0"
    }
    ```   


+ `ntpsync[ [NTP server] ]` **sync the time with NTP server**
    - [NTP server] ------ [ string ], NTP server   
    - return ttrue for succeed
    - return tfalse for failed
    - return terror for error

    Example, sync the time with time.window.com
    ```shell
    clock@date.ntpsync[ time.window.com ]
    ttrue
    ```   
    Example, sync the time with NTP server in the configure
    ```shell
    clock@date.ntpsync
    ttrue
    ```


### Lifecycle API
+ `setup[]` **apply saved timezone and related boot-time state**, *succeed return ttrue*
    - **Not** run automatically during **`init`** in the default clock package; call **`setup[]`** from your integration if the timezone must be applied before other services.

+ `shut[]` **stop this component’s supervised child (NTP client service)**, *succeed return ttrue*
    - **Not** run automatically on **`uninit`** in the default integration; call explicitly if you need it on shutdown.

### Joint Handlers
| Joint key | Invokes |
|-----------|---------|
| `network/online` | `clock@date.online` |

Registered as a **joint** handler in the default clock package. When the stack marks the WAN path **online**, **`online[]`** starts the embedded NTP client **`service`** if configuration has **`ntpclient`** = **`enable`**.


### Published Joint Events
+ **`date/modify`** — emitted when the wall clock is changed. Second argument is a short source tag, e.g. **`set`** (manual `current[…]`) or **`ntp`** (successful NTP sync).

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_clock_date(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "clock@date", "timezone") == NULL)
        return -1;
    ok = ssets_string("clock@date", "8", "timezone");
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

/* Example: scall("clock@date", "status", NULL); then talk_free if JSON */
```
