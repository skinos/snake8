## SSH Server Management
Manage the gateway **SSH** service using **Dropbear** (`dropbear` in `PATH`), not OpenSSH `sshd`.

#### Platform notes
- On **`scope=wrt`** or **`platform=slave`**, SSH is **disabled by design**: `_setup` returns success without starting the service; `_set` refuses configuration changes.
- `_setup` fails (`tfalse`) if `which dropbear` does not find an executable.
- If there is **no stored configuration** for this component (`config_get` returns empty), `_setup` returns success and does not apply settings from a config object.

#### Host keys
- Ensures `/etc/dropbear` exists.
- If project config provides **`dsskey`** / **`rsakey`** files, they are copied to `dropbear_dss_host_key` / `dropbear_rsa_host_key` under `/etc/dropbear/`.
- If **`dropbearkey.sh`** is installed for this package, it is executed to generate missing key types (e.g. ed25519, ecdsa, rsa) as supported by the image.

### **Configuration( `tui@ssh` )**

```json
// Attributes introduction 
{
    "status":"enable or disable the service", // [ "disable", "enable" ]
    "port":"service port",                    // string; empty uses default 22 in code
    "manager":                                // optional; same rules as telnet
    // (1) JSON object — values are IPv4 or MAC strings
    // (2) Single string — semicolon-separated list (WUI textarea)
}
```

**`manager` behavior (when `status` is `enable`):** Same iptables pattern as `tui@telnet`, but the filter chain name is **`tui_ssh`**. Non-empty `manager` → jump from `INPUT` on the listen port → per-address **ACCEPT** → final **DROP**. Empty/absent `manager` → no rules from this component for that port.


Example, show all SSH server configure
```shell
tui@ssh
{
    "status":"enable",             # start this service at system startup
    "port":"22",                   # service port 22
    "manager":                     # only the 192.168.8.111 and 00:03:7F:12:AA:B0 can access ssh server
    {
        "pc1":"192.168.8.111",
        "pc2":"00:03:7F:12:AA:B0"
    }
}
```  

Example, modify the port of SSH server
```shell
tui@ssh:port=2222
ttrue
```  

Example, disable the SSH server
```shell
tui@ssh:status=disable
ttrue
```  

Example, set the ip 192.168.8.250 can access the SSH server
```shell
tui@ssh:manager/pc3=192.168.8.250
ttrue
```  

Example, clear the manager allow all ip can access the SSH server
```shell
tui@ssh:manager=
ttrue
```  

Example, manager as a semicolon-separated string (same style as the web UI)
```shell
tui@ssh:manager="192.168.8.111;00:03:7F:12:AA:B0"
ttrue
```  

Examples, change several attributes at once (**merge**)
```shell
tui@ssh|{"status":"enable","port":"2222"}
ttrue
```

### **Component API**

+ `setup[]` **apply saved SSH (Dropbear) configuration and start or skip the service**, *succeed return ttrue*
    - Normally called during boot as **`tui@ssh.setup`** (via the installed package **init** schedule). Verifies **`dropbear`** exists; on **`scope=wrt`** or **`platform=slave`** returns success without starting. If configuration is missing, returns success without applying stored settings. If **`status`** is not **`enable`**, returns success without starting **Dropbear**. Otherwise prepares **`/etc/dropbear`** host keys (from project **`dsskey`** / **`rsakey`** if present, then **`dropbearkey.sh`** when installed), applies optional **`manager`** iptables rules (chain **`tui_ssh`**), and starts the **`service`** child (**`dropbear -F -p <port> -K 300`**).
    - *failed return tfalse* if **`dropbear`** is not found in **`PATH`**.

    Example, run setup manually
    ```shell
    tui@ssh.setup
    ttrue
    ```

+ `shut[]` **stop the Dropbear service and remove this component’s iptables chain**, *succeed return ttrue*
    - Flushes and deletes filter chain **`tui_ssh`**, removes the **`INPUT`** jump to that chain, and deletes the supervised service entry (**`sdelete`** on **`COM_IDPATH`**). Does not remove saved configuration.

    Example, shut down SSH
    ```shell
    tui@ssh.shut
    ttrue
    ```

### **Lifecycle API**

+ `setup[]` — runs during **`init` → `app`** as **`tui@ssh.setup`** (often together with **`tui@telnet.setup`**). There is **no** default **`uninit`** hook; call **`shut[]`** explicitly if you need ordered shutdown.

+ `shut[]` — **not** invoked automatically on **`uninit`** in the default package; see **Component API**.


### **C Code Example**

**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_tui_ssh(void)
{
    char buf[128];
    boole ok;
    if (sgets_string(buf, sizeof(buf), "tui@ssh", "status") == NULL)
        return -1;
    ok = ssets_string("tui@ssh", "value", "status");
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

/* Example: scall("tui@ssh", "status", NULL); then talk_free if JSON */
```
