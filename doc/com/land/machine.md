## Management of Machine Basic Information
Manage gateway basic information

**Who should read what:** JSON and `he` / shell examples are for operators and integrators using the command line. Optional **C-language** fragments at the end match the real headers (`#include "skin/skin.h"`, `talk_free`, `scall`, `json_*`, …); skip them if you do not embed the platform SDK.

**Note:** `cpuinfo`, `meminfo`, `psinfo`, `psnumber`, and `fsinfo` (and APIs built on the same data) are intended for **Linux-based** firmware images. On startup, the machine component completes its usual registration, applies the **configured hostname** (within system limits), and publishes **web UI, telnet, SSH, and LAN IP** settings so other parts of the system can read them.

**Policy locks:** In **`lock`** configuration, **`default=enable`** disallows **`restart`** and **`default`** (restore defaults). **`factory=enable`** disallows **`release`** and **`factory`**. Separately, **`restart_block`** / **`restart_unblock`** toggles whether **`restart`** is allowed; **`default_block`** / **`default_unblock`** toggles whether **`default`**, **`release`**, and **`factory`** are allowed.

### **Configuration( `land@machine` )**

The **saved configuration object** for `land@machine` (query/set via `land@machine`, `land@machine:path`, merge `|{json}`, etc.).

```json
// Attributes introduction 
{
    // main attributes
    "mode":"gateway operator mode",                 // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                       // "ap": access point
                                                       // "wisp": 2.4G Wireless Internet Service Provider connection
                                                       // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wireless baseband)
                                                       // "gateway": wire WAN gateway
                                                       // "dgateway": Dual wire WAN gateway
                                                       // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                       // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                       // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                       // "mwm": multiple LTE and Wireless gateway
                                                       // "mix": custom mix connection from multiple internet connection
                                                       // "mbridge" or "default": LAN dhcps status set disable
    "name":"gateway name",                          // < string >, The name cannot contain spaces
    "mac":"gateway MAC address",                    // < mac address >
    "macid":"gateway MAC identify or serial id",    // < string >
    "sn":"board serial number",                     // present when exposed by the product; read-only here
    "language":"gateway language",                  // [ "cn", "en", "jp", ... ], language code in two letter, "cn" for Chinese, "en" for English
    // other attributes
    "cfgversion":"gateway configure version",       // [ string ]
    "gpversion":"gateway group configure version",  // [ string ]
    "broken":"gateway system state"                 // [ "ill", "disable" ]
}
```

Example, show all basic configure
```shell
land@machine
{
    "mode":"nmisp",                              # gateway operator mode Next Mobile(LTE/NR) Internet Service Provider connection
    "name":"ASHYELF-12AAD0",                     # gateway name is ASHYELF-12AAD0
    "mac":"00:03:7F:12:AA:D0",                   # gateway MAC is 00:03:7F:12:AA:D0
    "macid":"00037F12AAD0",                      # gateway serial id is 88124E2046B0
    "language":"cn",                             # gateway language is chinese
    "cfgversion":"1"                             # gateway configure version is 1(hint modify one times)
}
```  

Example, modify the gateway name
```shell
land@machine:name=NewMachineName
ttrue
```  

Example, show the gateway name after modified
```shell
land@machine:name
NewMachineName
```  

Example, modify the operation mode to "mix"
```shell
land@machine:mode=mix
ttrue
```  

Example, modify the gateway language to English
```shell
land@machine:language=en
ttrue
```  

Examples, change several attributes at once (merge; only listed fields change)
```shell
land@machine|{"name":"DemoGW","language":"en"}
ttrue
```

### **Component API**

+ `status[]` **get the gateway basic information**   
    - returns JSON that describes the gateway basic information   

    ```json
    // Attributes introduction of json by the method return
    {
        "mode":"gateway operator mode",              // < "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mix" >
                                                         // "ap": access point
                                                         // "wisp": 2.4G Wireless Internet Service Provider connection
                                                         // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wireless baseband)
                                                         // "gateway": wire WAN gateway
                                                         // "dgateway": Dual wire WAN gateway
                                                         // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                         // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                         // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                         // "mwm": multiple LTE and Wireless gateway                                                         
                                                         // "mix": custom mix connection from multiple internet connection
        "name":"gateway name",
        "platform":"gateway platform identify",
        "hardware":"gateway hardware identify",
        "custom":"gateway custom identify",
        "scope":"gateway scope identify",
        "version":"gateway version",
        "cfgversion":"gateway configure version",        // [ string ]
        "gpversion":"gateway group configure version",   // [ string ]
        "livetime":"system running time",                // hour:minute:second:day
        "current":"current date",                        // hour:minute:second:month:day:year
        "mac":"gateway MAC address",
        "macid":"gateway MAC identify or serial id",
        "model":"gateway model",
        "features":"product feature flags",
        "cmodel":"gateway custom model",
        "oem":"OEM identifier string",
        "magic":"gateway magic identify",
        "datecode":"manufacturing date code",
        "rand":"optional pseudo-random integer when exposed",
        "wui_port":"web UI port from product configuration",
        "telnet_port":"telnet port from product configuration",
        "ssh_port":"SSH port from product configuration",
        "local_ip":"LAN static IP from product configuration"
    }    
    ```

    **`version`** is normally the firmware or software build identifier. While a restart is in progress, **`version`** may briefly show a **status** string instead.

    Example, get the gateway basic information
    ```shell
    land@machine.status
    {
        "mode":"nmisp",                                   # gateway operator mode Next Mobile(NR/LTE) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                          # gateway name is ASHYELF-12AAD0
        "platform":"smtk2",                               # gateway platform identify is smtk2( hint the basic sdk is second release of smtk )
        "hardware":"mt7621",                              # gateway hardware identify is mt7621( hint the chip is MT7621 )
        "custom":"d228",                                  # gateway custom identify is d228( hint the product name is D228 )
        "scope":"std",                                    # gateway scope identify is std( hint this is a standard release for D228 )
        "version":"tiger7-20220218",                      # gateway version is tiger7-20220218( hint the version publish at 2022.02.18 )
        "cfgversion":"6",                                 # gateway cfgversion is 6, configure maybe modified 6 times
        "livetime":"00:06:35:0",                          # gateway It has been running for 6 minutes and 35 seconds
        "current":"14:54:30:05:04:2022",                  # current date is 2022.05.04, It's 14:54 and 30 seconds
        "mac":"00:03:7F:12:AA:D0",                        # gateway MAC is 00:03:7F:12:AA:D0
        "macid":"00037F12AAD0",                           # gateway serial id is 00037F12AAD0
        "model":"5228",                                   # gateway model is 5228
        "magic":"870E2935E4605D02"                        # gateway magic is 870E2935E4605D02
    }
    ```


+ `restart[ [delay] ]` **reboot the gateway**   
    - delay ------- [ number ], delay some second to reboot the system
    - failed return tfalse
    - succeed return ttrue

    Example, reboot the gateway immediately( the call may not return because the system restart now )
    ```shell
    land@machine.restart
    # ---- system restart -----
    ```

    Example, reboot the gateway after 10 second( the call will be return immediately )
    ```
    land@machine.restart[ 10 ]
    ttrue
    ```

+ `restart_block[]` **disable all operator of reboot the gateway**   
    - failed return tfalse
    - succeed return ttrue
    
    Example, disable all operator of reboot the gateway
    ```shell
    land@machine.restart_block
    ttrue
    # then you cannot reboot the gateway after land@machine.restart_block
    land@machine.restart
    tfalse, Operation not permitted
    ```

+ `restart_unblock[]` **enable reboot the gateway**   
    - failed return tfalse
    - succeed return ttrue

+ `reboot[ [delay] ]` **same as `restart`**

+ `reboot_block[]` / `reboot_unblock[]` **same as `restart_block` / `restart_unblock`**.

    Example
    ```shell
    # first, disable all operator of reboot the gateway
    land@machine.restart_block
    ttrue
    # then you cannot reboot the gateway after land@machine.restart_block
    land@machine.restart
    tfalse, Operation not permitted
    # enable reboot the gateway
    land@machine.restart_unblock
    ttrue
    # then you can reboot the gateway
    land@machine.restart
    # ---- system restart ----
    ```

+ `default[ [delay] ]` **default the gateway configure and restart**   
    - delay ------- [ number ], delay some second to default the system
    - failed return tfalse
    - succeed return ttrue

    Example, default the gateway configure and restart immediately( the call may not return because the system restart now )
    ```shell
    land@machine.default
    # ---- system restart -----
    ```

    Example, default the gateway configure and restart after 10 second( the call will be return immediately )
    ```
    land@machine.default[ 10 ]
    ttrue
    ```

+ `default_block[]` **disable all operator of default the gateway configure**   
    - failed return tfalse
    - succeed return ttrue
    
    Example, disable all operator of default the gateway configure
    ```shell
    land@machine.default_block
    ttrue
    # then you cannot default the gateway configure after land@machine.default_block
    land@machine.default
    tfalse, Operation not permitted
    ```

+ `default_unblock[]` **enable all operator of default the gateway configure**   
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    # first, disable all operator of default the gateway configure
    land@machine.default_block
    ttrue
    # then you cannot default the gateway configure after land@machine.default_block
    land@machine.default
    tfalse, Operation not permitted
    # enable default the gateway configure
    land@machine.default_unblock
    ttrue
    # then you can default the gateway configure
    land@machine.default
    # ---- system restart ----
    ```

+ `release[ [delay] ]` **product release reset, then reboot** — same blocking and **`lock`** rules as **`default`** (including **`default_block`** / **`factory=enable`**). Optional **delay** arguments behave like **`restart`**.

+ `factory[ [delay] ]` **factory reset, then reboot** — same blocking and **`lock`** rules as **`release`**.

    ```shell
    land@machine.release
    land@machine.factory
    ```



+ `cpuinfo[]` **get the gateway CPU information**   
    - returns JSON that describes CPU statistics  

    ```json
    // Attributes introduction of json by the method return
    {
        "cpu":
        {
            "user":"normal processes executing in user mode",
            "nice":"niced processes executing in user mode",
            "system":"processes executing in kernel mode",
            "idle":"time spent idle",
            "iowait":"waiting for I/O to complete",
            "irq":"servicing interrupts",
            "softirq":"servicing softirqs"
        }
        //"cpu core identify":{ cpu information }     How many CPU cores show how many properties
    }
    ```

    Example, get the gateway cpu information
    ```shell
    land@machine.cpuinfo
    {
        "cpu":                       # total CPU
        {
            "user":"389",
            "nice":"0",
            "system":"2769",
            "idle":"34014",
            "iowait":"12",
            "irq":"0",
            "softirq":"493"
        },
        "cpu0":                      # CPU core 0
        {
            "user":"142",
            "nice":"0",
            "system":"996",
            "idle":"8383",
            "iowait":"0",
            "irq":"0",
            "softirq":"13"
        },
        "cpu1":                      # CPU core 1
        {
            "user":"58",
            "nice":"0",
            "system":"396",
            "idle":"8851",
            "iowait":"0",
            "irq":"0",
            "softirq":"144"
        },
        "cpu2":                      # CPU core 2
        {
            "user":"131",
            "nice":"0",
            "system":"864",
            "idle":"8194",
            "iowait":"12",
            "irq":"0",
            "softirq":"168"
        },
        "cpu3":                      # CPU core 3
        {
            "user":"56",
            "nice":"0",
            "system":"512",
            "idle":"8584",
            "iowait":"0",
            "irq":"0",
            "softirq":"167"
        }
    }
    ```

+ `psinfo[]` **get the gateway process information**   
    - returns JSON that describes processes  

    ```json
    // Attributes introduction of json by the method return
    {
         // How many process show how many properties
        "process identify":
        {
            "name":"process name",
            "state":"process state",
            "ppid":"parent process identify",
            "fdsize":"Number of file descriptor slots currently allocated",
            "vmsize":"Virtual memory usage of the entire process"
        }
    }
    ```

    Example, get the gateway process information
    ```shell
    land@machine.psinfo
    {
        "1":
        {
            "name":"procd",
            "state":"S",
            "ppid":"0",
            "fdsize":"32",
            "vmsize":"2736"
        },
        "2":
        {
            "name":"kthreadd",
            "state":"S",
            "ppid":"0",
            "fdsize":"32"
        },
        "3":
        {
            "name":"ksoftirqd/0",
            "state":"S",
            "ppid":"2",
            "fdsize":"32"
        },
        "4":
        {
            "name":"kworker/0:0",
            "state":"S",
            "ppid":"2",
            "fdsize":"32"
        },
        # ......
    }
    ```

+ `psnumber[]` **process count** — approximate number of processes; can differ slightly from the number of objects returned by **`psinfo`**.

    ```shell
    land@machine.psnumber
    42
    ```



+ `meminfo[]` **get the gateway memory information**   
    - returns JSON that describes system memory use  

    ```json
    // Attributes introduction of json by the method return
    {
        "total":"total memory size",       // The unit is KB  
        "free":"free memory size",         // The unit is KB  
        "buffers":"buffer cache size",     // The unit is KB  
        "cached":"page cache size"         // The unit is KB  
    }
    ```

    Example, get the gateway memory information
    ```shell
    land@machine.meminfo
    {
        "total":"125584",
        "free":"54076",
        "buffers":"6520",
        "cached":"22024"
    }
    ```



+ `fsinfo[]` **get the gateway filesystem information**   
    - returns JSON describing mounted filesystems

    ```json
    // Attributes introduction of json by the method return
    {
         // How many filesystem show how many properties
        "file system mount path":
        { 
            "filesystem":"device path",
            "size":"total size",               // K ending mean KB, M ending mean MB
            "used":"has been used",            // K ending mean KB, M ending mean MB
            "available":"available size",      // K ending mean KB, M ending mean MB
            "use":"usage rate"                 // percentage
        }
    }
    ```

    Example, get the gateway filesystem information
    ```shell
    land@machine.fsinfo
    {
        "/":                                             # root file system
        {
            "filesystem":"/dev/root",
            "size":"9.5M",
            "used":"9.5M",
            "available":"0",
            "use":"100%"
        },
        "/tmp/mnt/config":                                     # configure file system
        {
            "filesystem":"/dev/mtdblock7",
            "size":"1.0M",
            "used":"200.0K",
            "available":"824.0K",
            "use":"20%"
        },
        "/tmp/mnt/internal":                                  # internal firmware storage
        {
            "filesystem":"/dev/mtdblock6",
            "size":"3.8M",
            "used":"260.0K",
            "available":"3.5M",
            "use":"7%"
        }
    }
    ```

+ `sginfo[]` **get the gateway storage filesystem information**   
    - returns JSON combining **configuration** storage usage with **per-mount** usage for **extension** storage (each entry can include **`path`**, **`size`**, **`used`**, **`available`**, **`use`** when available)  

    ```json
    // Attributes introduction of json by the method return
    {
         // How many storage filesystem show how many properties
        "storage filesystem identify":
        { 
            "filesystem":"device path",
            "size":"total size",               // K ending mean KB, M ending mean MB
            "used":"has been used",            // K ending mean KB, M ending mean MB
            "available":"available size",      // K ending mean KB, M ending mean MB
            "use":"usage rate"                 // percentage
        }
    }
    ```

    Examples, get the gateway storage filesystem information
    ```shell
    land@machine.sginfo
    {
        "config":                                 # configure file system
        {
            "size":"1.0M",
            "used":"200.0K",
            "available":"824.0K",
            "use":"20%"
        },
        "internal":                                    # internal firmware storage
        {
            "path":"/tmp/mnt/int",
            "size":"3.8M",
            "used":"260.0K",
            "available":"3.5M",
            "use":"7%"
        }
    }
    ```

+ `esinfo[]` **get the gateway extension storage filesystem information**   
    - returns JSON for **add-on** storage only (built-in configuration and internal areas are not listed)  

    ```json
    // Attributes introduction of json by the method return
    {
         // How many extension storage filesystem show how many properties
        "storage filesystem identify":
        { 
            "filesystem":"device path",
            "size":"total size",               // K ending mean KB, M ending mean MB
            "used":"has been used",            // K ending mean KB, M ending mean MB
            "available":"available size",      // K ending mean KB, M ending mean MB
            "use":"usage rate"                 // percentage
        }
    }
    ```

    Example, get the gateway extension storage filesystem information
    ```shell
    land@machine.esinfo
    {
        "usb1":                                        # example add-on volume; built-in areas omitted
        {
            "path":"/tmp/mnt/usb1",
            "size":"1.8G",
            "used":"120.0M",
            "available":"1.6G",
            "use":"7%"
        }
    }
    ```


### **Lifecycle API**

+ `setup[]` **initialize the machine component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during startup
    - It scans and registers all system projects, sets the hostname, and initializes network interfaces
    - Not intended for manual invocation




### **Published joint events**

The following joint events are published when the machine state changes. Other components can subscribe at runtime (joint registration / **`land@joint`**).

| Event | Description |
|-------|-------------|
| `machine/status` | Sent when the machine state is updated. Triggered in the following scenarios: 1) At the beginning of `restart`/`reboot`, after the state is marked as `restarting` in the register; 2) During firmware upgrade process when the state changes to `downloading`, `upgrading`, `failed`, or `succeed`; 3) During firmware recovery operations. This event notifies other components about system state transitions so they can prepare accordingly (e.g., save data, close connections, or update LED indicators). |




### **C Code Example**

**Read and update configuration** — `sgets` / `sgets_string`, `ssets_string` / `ssets` (same paths as HE).

```c
#include "skin/skin.h"

static int machine_config_get_and_set(void)
{
    char name[64];
    talk_t mode;
    boole ok;

    /* 1) Get string config: name */
    if (sgets_string(name, sizeof(name), "land@machine", "name") == NULL)
    {
        return -1;
    }
    printf("current machine.name = %s\n", name);

    /* 2) Get generic config value: mode */
    mode = sgets("land@machine", "mode");
    if (mode == NULL || mode <= tpanic)
    {
        return -1;
    }
    printf("current machine.mode = %s\n", x2string(mode));
    talk_free(mode);

    /* 3) Set string config values */
    ok = ssets_string("land@machine", "DemoMachine", "name");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@machine", "mix", "mode");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@machine", "en", "language");
    if (ok == false)
    {
        return -1;
    }

    /* 4) Read back to verify */
    if (sgets_string(name, sizeof(name), "land@machine", "name") == NULL)
    {
        return -1;
    }
    printf("updated machine.name = %s\n", name);

    return 0;
}
```

Notes:
- Use `sgets_string()` / `sgets()` to get configuration values.
- Use `ssets_string()` / `ssets()` to update configuration values.
- Values returned as `talk_t` that reference allocated JSON must be released with `talk_free()` in C examples below (`he`-only users can ignore this).

**Call component methods** — `scall` / `scalls` (`com.h`).

```c
#include "skin/skin.h"

static void print_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `status[]`

```c
talk_t ret = scall("land@machine", "status", NULL);
if (ret > tpanic)
{
    printf("name=%s, mode=%s\n", json_string(ret, "name"), json_string(ret, "mode"));
    talk_free(ret);
}
else print_call_error("status", ret);
```

##### `restart[[delay]]`

```c
talk_t ret = scalls("land@machine", "restart", "10"); /* delay 10s */
if (ret != ttrue) print_call_error("restart", ret);
```

##### `restart_block[]`

```c
talk_t ret = scall("land@machine", "restart_block", NULL);
if (ret != ttrue) print_call_error("restart_block", ret);
```

##### `restart_unblock[]`

```c
talk_t ret = scall("land@machine", "restart_unblock", NULL);
if (ret != ttrue) print_call_error("restart_unblock", ret);
```

##### `reboot[[delay]]`

```c
talk_t ret = scalls("land@machine", "reboot", "5"); /* delay 5s */
if (ret != ttrue) print_call_error("reboot", ret);
```

##### `reboot_block[]`

```c
talk_t ret = scall("land@machine", "reboot_block", NULL);
if (ret != ttrue) print_call_error("reboot_block", ret);
```

##### `reboot_unblock[]`

```c
talk_t ret = scall("land@machine", "reboot_unblock", NULL);
if (ret != ttrue) print_call_error("reboot_unblock", ret);
```

##### `default[[delay]]`

```c
talk_t ret = scalls("land@machine", "default", "15"); /* delay 15s */
if (ret != ttrue) print_call_error("default", ret);
```

##### `default_block[]`

```c
talk_t ret = scall("land@machine", "default_block", NULL);
if (ret != ttrue) print_call_error("default_block", ret);
```

##### `default_unblock[]`

```c
talk_t ret = scall("land@machine", "default_unblock", NULL);
if (ret != ttrue) print_call_error("default_unblock", ret);
```

##### `release[]`

```c
talk_t ret = scall("land@machine", "release", NULL);
if (ret != ttrue) print_call_error("release", ret);
```

##### `factory[]`

```c
talk_t ret = scall("land@machine", "factory", NULL);
if (ret != ttrue) print_call_error("factory", ret);
```

##### `cpuinfo[]`

```c
talk_t ret = scall("land@machine", "cpuinfo", NULL);
if (ret > tpanic)
{
    talk_t cpu = json_json(ret, "cpu");
    printf("cpu idle=%s\n", json_string(cpu, "idle"));
    talk_free(ret);
}
else print_call_error("cpuinfo", ret);
```

##### `psinfo[]`

```c
talk_t ret = scall("land@machine", "psinfo", NULL);
if (ret > tpanic)
{
    talk_t p1 = json_json(ret, "1");
    if (p1 != NULL) printf("pid1 name=%s\n", json_string(p1, "name"));
    talk_free(ret);
}
else print_call_error("psinfo", ret);
```

##### `psnumber[]`

```c
talk_t ret = scall("land@machine", "psnumber", NULL);
if (ret > tpanic)
{
    printf("process count=%s\n", x2string(ret));
    talk_free(ret);
}
else print_call_error("psnumber", ret);
```

##### `meminfo[]`

```c
talk_t ret = scall("land@machine", "meminfo", NULL);
if (ret > tpanic)
{
    printf("mem total=%sKB free=%sKB\n", json_string(ret, "total"), json_string(ret, "free"));
    talk_free(ret);
}
else print_call_error("meminfo", ret);
```

##### `fsinfo[]`

```c
talk_t ret = scall("land@machine", "fsinfo", NULL);
if (ret > tpanic)
{
    talk_t root = json_json(ret, "/");
    if (root != NULL) printf("/ use=%s\n", json_string(root, "use"));
    talk_free(ret);
}
else print_call_error("fsinfo", ret);
```

##### `sginfo[]`

```c
talk_t ret = scall("land@machine", "sginfo", NULL);
if (ret > tpanic)
{
    talk_t internal = json_json(ret, "internal");
    if (internal != NULL) printf("internal path=%s\n", json_string(internal, "path"));
    talk_free(ret);
}
else print_call_error("sginfo", ret);
```

##### `esinfo[]`

```c
talk_t ret = scall("land@machine", "esinfo", NULL);
if (ret > tpanic)
{
    talk_t usb1 = json_json(ret, "usb1");
    if (usb1 != NULL) printf("usb1 available=%s\n", json_string(usb1, "available"));
    talk_free(ret);
}
else print_call_error("esinfo", ret);
```

### **Joint handlers**

**None** by default for this object (product builds may add more).

