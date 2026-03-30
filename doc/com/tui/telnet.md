## tui@telnet — Telnet Server Management
Manage the gateway **telnetd** service (BusyBox or system `telnetd` in `PATH`).

#### Platform notes
- On **`scope=wrt`** or **`platform=slave`**, telnet is **disabled by design**: `_setup` returns success without starting the service; `_set` refuses configuration changes.
- `_setup` fails (`tfalse`) if `which telnetd` does not find an executable (path length check in code).
- If there is **no stored configuration** for this component (`config_get` returns empty), `_setup` returns success and does not apply settings from a config object.

### Configuration ( `tui@telnet` )
```json
// Attributes introduction 
{
    "status":"enable or disable the service", // [ "disable", "enable" ]
    "port":"service port",                    // string; empty uses default 23 in code
    "manager":                                // optional; access control via iptables
    // Either:
    // (1) JSON object — keys are labels, values are IPv4 or MAC strings:
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
    // (2) Single string — semicolon-separated IPv4 or MAC (same as WUI textarea):
    // "192.168.8.111;00:03:7F:12:AA:B0"
}
```

**`manager` behavior (when `status` is `enable`):**
- If `manager` is **absent or empty**, no extra iptables rules are added by this component; who can connect depends on the rest of the firewall.
- If `manager` has **one or more** addresses: traffic to the listen port is sent to chain `tui_telnet`; only listed **IPv4** (`inet_pton`) or **MAC** (otherwise treated as `--mac-source`) match **ACCEPT**; the chain ends with **DROP** for others.


Example, show all telnet server configure
```shell
tui@telnet
{
    "status":"enable",             # start this service at system startup
    "port":"23",                   # service port 23
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

Example, modify the port of telnet server
```shell
tui@telnet:port=2323
ttrue
```  

Example, disable the telnet server
```shell
tui@telnet:status=disable
ttrue
```  

Example, set the ip 192.168.8.250 can access the telnet server
```shell
tui@telnet:manager/pc3=192.168.8.250
ttrue
```  

Example, clear the manager allow all ip can access the telnet server
```shell
tui@telnet:manager=
ttrue
```  

Example, manager as a semicolon-separated string (same style as the web UI)
```shell
tui@telnet:manager="192.168.8.111;00:03:7F:12:AA:B0"
ttrue
```  

Examples, change several attributes at once (**merge**)
```shell
tui@telnet|{"status":"enable","port":"2323"}
ttrue
```

### Component API
+ `setup[]` **apply saved telnet configuration and start or skip the service**, *succeed return ttrue*
    - Normally called during boot as **`tui@telnet.setup`** (via the installed package **init** schedule). Verifies **`telnetd`** exists; on **`scope=wrt`** or **`platform=slave`** returns success without starting. If configuration is missing, returns success without applying stored settings. If **`status`** is not **`enable`**, returns success without starting **`telnetd`**. Otherwise applies optional **`manager`** iptables rules (chain **`tui_telnet`**) and starts the **`service`** child (**`telnetd -F -p <port>`**).
    - *failed return tfalse* if **`telnetd`** is not found in **`PATH`**.

    Example, run setup manually
    ```shell
    tui@telnet.setup
    ttrue
    ```

+ `shut[]` **stop the telnet service and remove this component’s iptables chain**, *succeed return ttrue*
    - Flushes and deletes filter chain **`tui_telnet`**, removes the **`INPUT`** jump to that chain, and deletes the supervised service entry (**`sdelete`** on **`COM_IDPATH`**). Does not remove saved configuration.

    Example, shut down telnet
    ```shell
    tui@telnet.shut
    ttrue
    ```

### Lifecycle API
+ `setup[]` — runs during **`init` → `app`** as **`tui@telnet.setup`**. **No** default **`uninit`**; use **`shut[]`** or extend the package manifest if needed.

+ `shut[]` — **not** wired on **`uninit`** in the default package; see **Component API**.


### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_tui_telnet(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "tui@telnet", "status") == NULL)
        return -1;
    ok = ssets_string("tui@telnet", "value", "status");
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

/* Example: scall("tui@telnet", "status", NULL); then talk_free if JSON */
```
