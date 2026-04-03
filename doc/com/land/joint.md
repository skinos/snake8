## land@joint — Joint Event Management
Administration of equipment joint event task
Each FPK can register **joint** handlers through its shipped manifest; the system dispatches them when matching events occur

Joint events are string names. **Emitters** publish a short string or a JSON payload; **consumers** are whatever the platform has registered for that name—usually from (1) each installed FPK’s **joint** section in its manifest, (2) runtime registration through **`land@fpk`** / **`joint_register`**, and (3) the **`land@joint`** cache and persisted **`list`** (`register` / `add` / `delete`).

JSON payloads (for example from **`network@frame`**) typically carry interface context such as **`ifname`**, **`netdev`**, **`ifnametype`**, **`ontime`**, … String payloads are used for simpler signals (e.g. **`date/modify`** with `"set"` / `"ntp"`).

### Concepts

The table summarizes **common event names**, what they mean, who usually **publishes** them, and **example consumers** in a typical reference image. Product FPKs almost always add or remap rows; check **`land@joint.list`** on a running device for the live map.

| Joint event | Purpose | Usually emitted by | Example consumers (reference) |
|-------------|---------|-------------------|------------------------------|
| `machine/status` | System / machine state transition (e.g. entering restart). | **`land@machine`** (and other platform code during upgrade/recovery). | Often extended on products (cloud, LED, tunnel helpers via **`land@joint`**). |
| `date/modify` | System time was changed (manual set or NTP). | **`clock@date`**. | None in the minimal reference set. |
| `auth/modify` | Authentication or account data changed. | **`land@auth`**. | None in the minimal reference set. |
| `network/on` | IPv4 **up** on a **local** logical interface (`ifnametype` = local). | **`network@frame`**. | e.g. **`forward@ttl.on`**, test/diagnostic handlers on some images. |
| `network/onvpn` | IPv4 **up** on a **VPN** logical interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/onextern` | IPv4 **up** on an **extern** (WAN-style) logical interface. | **`network@frame`**. | e.g. **`forward@nat.on`**, **`forward@firewall.on`**. |
| `network/online` | Default gateway / active **connection** path **online**; JSON = connection state. | **`network@frame`**, **`network@connect`**. | e.g. **`clock@date.online`**, **`agent@heclient.setup`**. |
| `network/off` | IPv4 **down** on a **local** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/offvpn` | IPv4 **down** on a **VPN** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/offextern` | IPv4 **down** on an **extern** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/offline` | Loss of default IPv4 connectivity / combined offline signalling. | **`network@frame`**. | None in the minimal reference set. |
| `network/up` | IPv6 **up** on a **local** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/upvpn` | IPv6 **up** on a **VPN** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/upextern` | IPv6 **up** on an **extern** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/upline` | IPv6 default path / line up. | **`network@frame`**. | None in the minimal reference set. |
| `network/down` | IPv6 **down** on a **local** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/downvpn` | IPv6 **down** on a **VPN** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/downextern` | IPv6 **down** on an **extern** interface. | **`network@frame`**. | None in the minimal reference set. |
| `network/downline` | IPv6 default path / line down. | **`network@frame`**. | None in the minimal reference set. |
| `station/appear` | A station (LAN client) appeared or got a new IP (JSON with client fields). | **`client@station`**. | None in the minimal reference set. |
| `station/disappear` | A station went away or IP is being released. | **`client@station`**. | None in the minimal reference set. |
| `storage/insert` | Removable / external storage became available (platform-specific). | Platform / hotplug integration. | e.g. **`land@syslog.setup`**. |
| `storage/remove` | Removable storage removed or unmounted. | Platform / hotplug integration. | e.g. **`land@syslog.setup`**. |

**Runtime registration:** components may also subscribe programmatically (e.g. **`agent@gtog`** for `network/online`). Those subscriptions may not appear in a static manifest until persisted under **`land@joint`** or shipped in an FPK.

**Larger products** typically map `network/onextern`, `network/on`, `network/online`, `machine/status`, … to `forward@nat.on`, `forward@firewall.on`, `agent@heclient.*`, `client@dhcps.reset`, and similar. See **`doc/com/land/joint.md`** for a narrative list, and **`land@joint.list`** on device for the effective map.


### Configuration ( `land@joint` )

The **saved configuration object** for `land@joint` (**list** of joint tasks and optional **remote** UDP notify settings).

```json
// Attributes introduction 
{
    "remote":
    {
        "ip":"remote ip address",            // [ string ], empty clears remote
        "port": "udp port"                   // [ number ], default 515 if missing or not positive
    },
    "list":
    {
        "task name":                             // [ string ], you can custom the name
        {
            "level":"joint event name",                 // [ string ], same field naming idea as boot levels in `init` documentation
            "call":"component API or program"           // [ string ]
        }
        // "...":{ ... }     How many joint event task show how many properties
    }
}
// Examples
{
    "remote":
    {
        "ip":"192.168.8.222",                          // remote ip is 192.168.8.222
        "port": "2230"                                 // [ number ], default 515 if missing or not positive
    },
    "list":
    {
        "online_talk_to_me":                               // execute tui@telnet.reset when the network/online joint event happen
        {
            "level":"network/online",
            "call":"tui@telnet.reset"
        },
        "offline_talk_to_me":                              // execute tui@ssh.shut when the network/offline joint event happen
        {
            "level":"network/offline",
            "call":"tui@ssh.shut"
        }
    }
}
```  

Examples, merge **remote** and one **list** task
```shell
land@joint|{"remote":{"ip":"192.168.8.1","port":"2230"}}
ttrue
```

### Component API

+ `register[ joint event name, call ]` **register a joint event task, lost when reboot**  
    - joint event name ----------- [ string ] 
    - call ----------------------- [ string ], component API or program 
    - failed return tfalse
    - succeed return ttrue

    Example, register executing wui@admin.reset when the network/online joint event happen
    ```shell
    land@joint.register[ network/online, wui@admin.reset ]
    ttrue
    ```
    Example, register executing tui@ssh.reset when the network/up joint event happen
    ```
    land@joint.register[ network/up, tui@ssh.reset ]
    ttrue    
    ```   

+ `unregister[ joint event name, call ]` **delete a joint event task**  
    - joint event name ------- [ string ]
    - call ------------------- [ string ], component API or program
    - failed return tfalse
    - succeed return ttrue

    Example, delete executing tui@ssh.reset when network/up joint event happen
    ```shell
    land@joint.unregister[ network/up, tui@ssh.reset ]
    ttrue
    ```

+ `list[ [joint event name] ]` **list joint event task**  
    - joint event name ----------- [ string ], default will list all registered joint event task in the system
    - return json to describes the event task   
    ```json
    // Attributes introduction of json by the method return
    {
        "joint event name":                 // [ string ]:{},  joint event name 
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     How many executing at that joint event show how many properties
        }
        // "...":{ ... }     How many joint event name show how many properties
    }    
    ```  

    Example, show all the joint event task
    ```shell
    land@joint.list
    {
        "storage/insert":
        {
            "land@syslog.setup":""
        },
        "storage/remove":
        {
            "land@syslog.setup":""
        },
        "network/online":
        {
            "agent@heclient.setup":"",
            "client@dhcps.reset":""
        },
        "machine/status":
        {
            "agent@heclient.update":""
        },
        "network/onextern":
        {
            "arch@ethernet.hwnat":"",
            "forward@rule.on":"",
            "forward@nat.on":"",
            "forward@firewall.on":""
        },
        "network/on":
        {
            "client@dhcps.reset":"",
            "forward@rule.on":"",
            "forward@dnat.on":""
        }
        // ... more joint event
    }
    ```  

    Example, show the network/online joint event task 
    ```shell
    land@joint.list[ network/online ]
    {
        "agent@heclient.setup":"",
        "client@dhcps.reset":""
    }   
    ```


+ `add[ task name, call, joint event name ]` **add a joint event task**
    - task name ------------- [ string ], task name, you can custom the name
    - call ------------------ [ string ], component API or program
    - joint event name ------ [ string ]
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named webreset, that executing wui@admin.reset when the network/online joint event happen 
    ```shell
    land@joint.add[ webreset, wui@admin.reset, network/online ]
    ttrue
    ```
    Example, add a task named sshreset, that executing tui@ssh.reset when the network/up joint event happen 
    ```
    land@joint.add[ sshreset, tui@ssh.reset, network/up ]
    ttrue    
    ```

+ `delete[ task name ]` **delete a joint event task**
    - task name ---- [ string ], task name
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named webreset
    ```shell
    land@joint.delete[ webreset ]
    ttrue
    ```


+ `call[ joint event name, [parameter] ]` **execute all joint event tasks at the specified joint event happen**, *succeed return ttrue, failed return tfalse, error return terror*
    - joint event name ----------- [ string ], joint event happen to execute (e.g., "network/online", "network/offline")
    - parameter ------------------ [ talk_t ], optional parameter to pass to the tasks
    - This method is called by the system during joint event happen
    - It executes all registered tasks for the specified joint event happen and sends UDP notification if remote logging is configured
    - Not intended for manual invocation


### Lifecycle API


+ `setup[]` **initialize the joint component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during startup
    - It registers all joint event tasks from configuration and sets up remote logging if configured
    - Not intended for manual invocation

**Note:** The shared **`land@init`** implementation used by **`land@joint`** does not export **`shut[]`**.

### Joint Handlers

This object **defines** which `component.method` runs for each **joint key** (see the **Joint event catalog** section earlier in this file). It is not a subscriber itself; other components **publish** events listed in the catalog.


### C Code Example

**Read and update configuration**

```c
#include "skin/skin.h"

static int joint_config_get_and_set(void)
{
    char remote_ip[64];
    talk_t task;
    boole ok;

    /* 1) Read remote target settings */
    if (sgets_string(remote_ip, sizeof(remote_ip), "land@joint", "remote/ip") == NULL)
    {
        return -1;
    }
    printf("joint remote ip=%s\n", remote_ip);

    /* 2) Read one task object under list */
    task = sgets("land@joint", "list/online_talk_to_me");
    if (task != NULL && task > tpanic)
    {
        printf("task level=%s call=%s\n", json_string(task, "level"), json_string(task, "call"));
        talk_free(task);
    }

    /* 3) Update remote ip/port */
    ok = ssets_string("land@joint", "192.168.8.222", "remote/ip");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@joint", "2230", "remote/port");
    if (ok == false)
    {
        return -1;
    }

    /* 4) Update one list entry */
    ok = ssets_string("land@joint", "network/online", "list/online_talk_to_me/level");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@joint", "tui@telnet.reset", "list/online_talk_to_me/call");
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

Notes:
- Joint configuration is mainly under `remote/*` and `list/*`.
- Use `sgets_string()` / `sgets()` for reads and `ssets_string()` for updates.

**Call component methods**

```c
#include "skin/skin.h"

static void print_joint_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `register[ joint event name, call ]`

```c
talk_t ret = scalls("land@joint", "register", "network/online,wui@admin.reset");
if (ret != ttrue) print_joint_call_error("register", ret);
```

##### `unregister[ joint event name, call ]`

```c
talk_t ret = scalls("land@joint", "unregister", "network/online,wui@admin.reset");
if (ret != ttrue) print_joint_call_error("unregister", ret);
```

##### `list[ [joint event name] ]`

```c
talk_t ret = scalls("land@joint", "list", "network/online");
if (ret > tpanic)
{
    printf("network/online event list ready\n");
    talk_free(ret);
}
else print_joint_call_error("list", ret);
```

##### `add[ task name, call, joint event name ]`

```c
talk_t ret = scalls("land@joint", "add", "webreset,wui@admin.reset,network/online");
if (ret != ttrue) print_joint_call_error("add", ret);
```

##### `delete[ task name ]`

```c
talk_t ret = scalls("land@joint", "delete", "webreset");
if (ret != ttrue) print_joint_call_error("delete", ret);
```

