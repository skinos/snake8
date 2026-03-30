## forward@alg — Management of Application Layer Gateway

### Configuration ( `forward@alg` )
```json
// Attributes introduction 
{
    "amanda":"amanda ALG function",            // [ "disable", "enable" ]
    "ftp":"ftp ALG function",                  // [ "disable", "enable" ]
    "h323":"h323 ALG function",                // [ "disable", "enable" ]
    "irc":"irc ALG function",                  // [ "disable", "enable" ]
    "pptp":"pptp ALG function",                // [ "disable", "enable" ]
    "gre":"gre ALG function",                  // [ "disable", "enable" ]
    "sip":"sip ALG function",                  // [ "disable", "enable" ]
    "rtsp":"rtsp ALG function",                // [ "disable", "enable" ]
    "snmp":"snmp ALG function",                // [ "disable", "enable" ]
    "tftp":"tftp ALG function",                // [ "disable", "enable" ]
    "udplite":"udplite ALG function"           // [ "disable", "enable" ]
}
```   

Example, show current all ALG settings
```shell
forward@alg
{
    "amanda":"disable",
    "ftp":"enable",
    "h323":"disable",
    "irc":"disable",
    "pptp":"enable",
    "gre":"enable",
    "rtsp":"enable",
    "sip":"enable",
    "snmp":"disable",
    "tftp":"disable"
}
```   

Example, disable the FTP ALG
```shell
forward@alg:ftp=disable
ttrue
```   

Example, disable the sip ALG
```shell
forward@alg:sip=disable
ttrue
```   

Example, enable the h323 ALG
```shell
forward@alg:h323=enable
ttrue
```   

Example, show the FTP ALG settings
```shell
forward@alg:ftp
disable
```   

Examples, change several attributes at once (**merge**)
```shell
forward@alg|{"ftp":"enable","sip":"disable"}
ttrue
```

### Component API
**Directly callable** APIs: standard configuration get/set/merge on **`forward@alg`** (see **Configuration**). No separate operator methods beyond **`setup[]` / `shut[]`** below.

### Lifecycle API
+ `setup[]` **load ALG kernel helpers from configuration**, *succeed return ttrue* — **`init` → `app` → `forward@alg.setup`** in the default package. In **default** / **parasite** network modes, ALG is skipped; otherwise enables/disables helpers (FTP, SIP, …) per saved flags.

+ `shut[]` **unload ALG helpers**, *succeed return ttrue* — called from platform shutdown.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_forward_alg(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "forward@alg", "status") == NULL)
        return -1;
    return ssets_string("forward@alg", "enable", "status") ? 0 : -1;
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

/* e.g. scall("forward@alg", "list", NULL); talk_free if JSON */
```
