## land@syslog — System Logging

Manages the system log service. Object name: `land@syslog`.

### Configuration ( `land@syslog` )

The **saved configuration object** for `land@syslog` (query/set via `land@syslog`, `land@syslog:path`, merge `|{json}`, etc.).

```json
// Attribute introduction
{
    "status":"syslog status",          // [ "disable", "enable", "tui", "file", "both" ]
                                            // "enable" for syslog
                                            // "disable" for no log
                                            // "tui" for output to terminal
                                            // "both" for output to terminal and syslog
                                            // "file" for output to file
    "trace":"record code information", // [ "disable", "enable" ]

    "level":"default log level",            // [ "verb", "debug", "info", "warn", "fault" ]
                                            // "verb" for debug business data
                                            // "debug" for debug code
                                            // "info" for normal information
                                            // "warn" for warning information
                                            // "fault" for error information
    "fault":"debug type",              // [ string ], Multiple types separated by ";" (ex: "arch@usb;arch@pci"); refines fault log mask
    "warn":"warn type",                // [ string ], Multiple types separated by ";" (ex: "arch@usb;arch@pci"); refines warn log mask
    "info":"info type",                // [ string ], Multiple types separated by ";" (ex: "arch@usb;arch@pci"); refines info log mask
    "debug":"debug type",              // [ string ], Multiple types separated by ";" (ex: "arch@usb;arch@pci"); refines debug log mask
    "verb":"verb type",                // [ string ], Multiple types separated by ";" (ex: "arch@usb;arch@pci"); refines verbose log mask

    "klog":"kernel log status",         // [ "disable", "enable" ]
    "size":"log size",                  // [ string ], decimal number in KB. Logs will be overwritten when they exceed the size of the recording pool
    "location":"location of logs",      // optional; selects base path for file logging
                                            // if unset or other: default file under the system var directory with random prefix
                                            // "storage": first non-config/internal storage path from machine sginfo
                                            // "internal" or names starting with "sd" or "mm": path from machine sginfo for that key
                                            // absolute path starting with "/": use as log path
    "remote":"Remote Log server",     // [ string ], If this parameter is left blank, the remote log server is disabled
    "port":"Remote Log port"          // [ string ], decimal number, default is 514
}
```

Examples, show all the syslog configure   
```shell
land@syslog
{
    "status":"enable",                # enable the syslog functions
    "location":"",                    # omit or leave unrecognized for default var path (that file uses random prefix under system var dir); or use storage/internal/sd*/mm*/absolute path
    "debug":"arch@usb;arch@pci",      # log the arch@usb and arch@pci debug information
    "level":"info",                   # default log level is normal information
    "trace":"disable",                # disable the code information
    "size":"100",                     # log buffer is 100k
    "remote":"192.168.8.250",         # send the syslog to remote server 192.168.8.250
    "port":"514"                      # send the syslog to remote server port 514
}
```  

Examples, set the remote syslog server be 112.43.230.74   
```shell
land@syslog:remote=112.43.230.74
```  

Examples, clear the remote syslog server
```shell
land@syslog:remote=
```  

Examples, set the syslog level to debug 
```shell
land@syslog:level=debug
```  

Examples, merge several fields at once
```shell
land@syslog|{"level":"info","remote":"192.168.8.1","port":"514"}
ttrue
```

### Component API

+ `path[]` **show log location and max size**, *succeed return talk, failed return NULL, error return terror*   
    ```json
    // Attributes introduction of talk by the method return
    {
        "path":"log file pathname",   // [ string ]
        "size":"log file size limit"  // [ number ], the unit is Kbyte
    }    
    ```

    ```shell
    # examples, show current log file pathname and size limit
    land@syslog.path
    {
        "path":"/var/3618760113-landlog.txt",  # current the syslog save at /var/3618760113-landlog.txt
        "size":"100"                            # file limit is 100k
    }
    ```

+ `clear[]` **clear the log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.clear
    ttrue
    ```

+ `show[]` **show the log**, *succeed return ttrue, failed return tfalse, error return terror*   
    ```shell
    # examples
    land@syslog.show
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>66>> (AT+QNWINFO^M^M +QNWINFO: "FDD LTE","46001","LTE BAND 3",1650^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (at+qeng="servingcell"^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>22>> (at+qeng="servingcell"^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CREG?^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>9>> (AT+CREG?^M)
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>48>> (AT+CREG?^M^M +CREG: 2,1,"2604","6DA5A09",7^M ^M OK^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 <<< (AT+CSQ^M )
    May 12 01:58:16 ASHYELF-12ADB0 daemon.debug modem@lte2: 6 >>28>> (AT+CSQ^M^M +CSQ: 18,99^M ^M OK^M )    
    ttrue
    ```

+ `info[ message ]` **record a level of info log**, *succeed return ttrue, failed return tfalse, error return terror*   
    - message ----------- [ string ], the info message to record
    ```shell
    # examples
    land@syslog.info[ "this is log for info level" ]
    ttrue
    ```

+ `debug[ message ]` **record a level of debug log**, *succeed return ttrue, failed return tfalse, error return terror*   
    - message ----------- [ string ], the debug message to record
    ```shell
    # examples
    land@syslog.debug[ "this is log for debug level" ]
    ttrue
    ```

+ `warn[ message ]` **record a level of warn log**, *succeed return ttrue, failed return tfalse, error return terror*   
    - message ----------- [ string ], the warn message to record
    ```shell
    # examples
    land@syslog.warn[ "this is log for warn level" ]
    ttrue
    ```

+ `fault[ message ]` **record a level of fault log**, *succeed return ttrue, failed return tfalse, error return terror*   
    - message ----------- [ string ], the fault message to record
    ```shell
    # examples
    land@syslog.fault[ "this is log for fault level" ]
    ttrue
    ```

+ `mask[]` **dump the current log mask as hex bytes**
    ```shell
    # examples, dump the current log mask
    land@syslog.mask
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    07 07 07 07 07 07 07 07 07 07
    ```

+ `add_fault[ component, ... ]` **add component(s) to fault level log mask**, *succeed return ttrue, failed return tfalse, error return terror*
    - component ----------- [ string ], component name(s) to add to fault log mask (e.g., "arch@usb", "modem@lte")
    - Multiple components can be specified as separate parameters
    - This allows fine-grained control over which components can log at fault level
    ```shell
    # examples
    land@syslog.add_fault[ modem@lte, modem@lte2 ]
    ttrue
    ```

+ `add_warn[ component, ... ]` **add component(s) to warn level log mask**, *succeed return ttrue, failed return tfalse, error return terror*
    - component ----------- [ string ], component name(s) to add to warn log mask

+ `add_info[ component, ... ]` **add component(s) to info level log mask**, *succeed return ttrue, failed return tfalse, error return terror*
    - component ----------- [ string ], component name(s) to add to info log mask

+ `add_debug[ component, ... ]` **add component(s) to debug level log mask**, *succeed return ttrue, failed return tfalse, error return terror*
    - component ----------- [ string ], component name(s) to add to debug log mask

+ `add_verb[ component, ... ]` **add component(s) to verbose level log mask**, *succeed return ttrue, failed return tfalse, error return terror*
    - component ----------- [ string ], component name(s) to add to verbose log mask

+ `del_fault[ component, ... ]` **remove component(s) from fault level log mask**, *always return ttrue*
    - component ----------- [ string ], component name(s) to remove from fault log mask

+ `del_warn[ component, ... ]` **remove component(s) from warn level log mask**, *always return ttrue*
    - component ----------- [ string ], component name(s) to remove from warn log mask

+ `del_info[ component, ... ]` **remove component(s) from info level log mask**, *always return ttrue*
    - component ----------- [ string ], component name(s) to remove from info log mask

+ `del_debug[ component, ... ]` **remove component(s) from debug level log mask**, *always return ttrue*
    - component ----------- [ string ], component name(s) to remove from debug log mask

+ `del_verb[ component, ... ]` **remove component(s) from verbose level log mask**, *always return ttrue*
    - component ----------- [ string ], component name(s) to remove from verbose log mask

+ `list[]` **list all log files**, *succeed return talk, failed return NULL*
    - returns JSON mapping **basename → full path** for files in the same directory as the active log path whose names contain the platform log basename pattern (**`landlog.txt`** substring, per **`LANDLOG_FILENAME`**)
    ```json
    // Attributes introduction of json by the method return
    {
        "log file name":"log file full path"
        // "...":"..."  How many log files show how many properties
    }
    ```
    ```shell
    # examples
    land@syslog.list
    {
        "3618760113-landlog.txt":"/var/3618760113-landlog.txt"
    }
    ```

+ `delete[ log file name ]` **delete a specified log file**, *succeed return ttrue, failed return tfalse*
    - log file name ----------- [ string ], the log file name to delete
    ```shell
    # examples
    land@syslog.delete[ 3618760113-landlog.txt ]
    ttrue
    ```


### Lifecycle API


+ `setup[]` **initialize the syslog component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during startup
    - It initializes the logging system based on configuration (status, level, remote server, etc.)
    - Not intended for manual invocation

+ `shut[]` **shutdown the syslog component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during shutdown
    - It stops klogd and syslogd processes
    - Not intended for manual invocation


### Joint Handlers

**Joint** handlers may invoke **`land@syslog.setup`** when storage changes so log location logic can run again.

| Joint key | Method |
|-----------|--------|
| `storage/insert` | `land@syslog.setup` |
| `storage/remove` | `land@syslog.setup` |


### C Code Example

**Read and update configuration**

```c
#include "skin/skin.h"

static int syslog_config_get_and_set(void)
{
    char level[32];
    char remote[64];
    char port[32];
    boole ok;

    /* 1) Read one string setting */
    if (sgets_string(level, sizeof(level), "land@syslog", "level") == NULL)
    {
        return -1;
    }
    printf("current syslog level=%s\n", level);

    /* 2) Read remote target */
    if (sgets_string(remote, sizeof(remote), "land@syslog", "remote") == NULL)
    {
        return -1;
    }
    if (sgets_string(port, sizeof(port), "land@syslog", "port") == NULL)
    {
        return -1;
    }
    printf("remote=%s port=%s\n", remote, port);

    /* 3) Update common settings */
    ok = ssets_string("land@syslog", "debug", "level");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "file", "status");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "storage", "location");
    if (ok == false)
    {
        return -1;
    }

    /* 4) Update remote target */
    ok = ssets_string("land@syslog", "192.168.8.250", "remote");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@syslog", "514", "port");
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

Notes:
- Use `sgets_string()` / `sgets()` to fetch syslog configuration values.
- Use `ssets_string()` to update syslog configuration values.

**Call component methods**

```c
#include "skin/skin.h"

static void print_syslog_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `path[]`

```c
talk_t ret = scall("land@syslog", "path", NULL);
if (ret > tpanic)
{
    printf("path=%s size=%s\n", json_string(ret, "path"), json_string(ret, "size"));
    talk_free(ret);
}
else print_syslog_call_error("path", ret);
```

##### `clear[]`

```c
talk_t ret = scall("land@syslog", "clear", NULL);
if (ret != ttrue) print_syslog_call_error("clear", ret);
```

##### `show[]`

```c
talk_t ret = scall("land@syslog", "show", NULL);
if (ret != ttrue) print_syslog_call_error("show", ret);
```

##### `info[]`

```c
talk_t ret = scalls("land@syslog", "info", "this is info log from C");
if (ret != ttrue) print_syslog_call_error("info", ret);
```

##### `warn[]`

```c
talk_t ret = scalls("land@syslog", "warn", "this is warn log from C");
if (ret != ttrue) print_syslog_call_error("warn", ret);
```

##### `fault[]`

```c
talk_t ret = scalls("land@syslog", "fault", "this is fault log from C");
if (ret != ttrue) print_syslog_call_error("fault", ret);
```
