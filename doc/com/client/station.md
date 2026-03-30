## client@station — Management of all local client
Management of all local client  
Shows **who is on the LAN**: which devices are online, names from DHCP where available, and your saved per-MAC settings (nickname, fixed IP, etc.). **`list`** returns this combined view. Notifications when a client appears or leaves are under **Joint Events** below.

### Configuration ( `client@station` )
```json
// Attributes introduction 
{
    "client MAC address":                                // [ MAC address ]
    {
        "ifname":"specify logical ifname",                    // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], default "ifname@lan"; call "network@frame.list[local]" for the list
        "name":"specify hostname",                            // [ string ]
        "bindip":"specify ip address on dhcp assignment",     // [ ip address ] fixed address for this MAC when using DHCP
        "arpbind":"bind ip set the arp table",                // [ "disable", "enable" ] keep a fixed IP↔MAC binding on the LAN when enable and bindip is set
        "lease":"specify lease on dhcp assignment"            // [ number ] the unit is second
    }
    // more client MAC address rule
}
```   

Examples, show all the station configure
```shell
client@station
{
    "00:03:7F:22:43:2B":                       # first client MAC address
    {
        "ifname":"ifname@lan",                     # belongs to ifname@lan network
        "name":"Office-Printer",                   # custom hostname: Office-Printer
        "bindip":"192.168.31.100",                 # fixed IP 192.168.31.100 for DHCP assignment
        "arpbind":"enable",                        # keep a fixed IP-MAC binding on the LAN
        "lease":"0"                                # use the default lease time
    },
    "F6:F7:73:82:0A:FC":                       # second client MAC address
    {
        "ifname":"ifname@lan",                     # belongs to ifname@lan network
        "name":"Xiaomi-Phone",                     # custom hostname: Xiaomi-Phone
        "bindip":"192.168.31.222",                 # fixed IP 192.168.31.222 for DHCP assignment
        "arpbind":"disable"                        # no IP-MAC binding
    }
}
```

Examples, bind ip 192.168.31.222 for 00:51:45:CB:78:80
```shell
client@station:00:51:45:CB:78:80/bindip=192.168.31.222
ttrue
```

Examples, clear the bind ip for 00:51:45:CB:78:89
```shell
client@station:00:51:45:CB:78:89/bindip=
ttrue
```

Examples, change several attributes at once (**merge**)
```shell
client@station|{"00:51:45:CB:78:80":{"bindip":"192.168.31.222","name":"Phone1"}}
ttrue
```

### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `add[ mac, name, ]` **add a client with name**
    - mac -------------- [ mac address ], the format can be AA:BB:CC:DD:EE:FF or AABBCCDDEEFF (invalid format returns **tfalse**)
    - name ------------- [ string ], optional display name saved for that MAC
    - succeed return ttrue
    - failed return tfalse (invalid MAC or save failure)

    Example, add a client, custom it name "NewPhone"
    ```shell
    client@station.add[ 00:03:7F:22:43:2B, NewPhone ]
    ttrue
    ```   

    Example, add a client, custom it name "OldPhone"
    ```shell
    client@station.add[ 345212EDFE10, OldPhone ]
    ttrue
    ```   

+ `delete[ mac ]` **delete a client**
    - mac -------------- [ mac address ], the format can be AA:BB:CC:DD:EE:FF or AABBCCDDEEFF
    - succeed return ttrue
    - failed return tfalse (invalid MAC format, or that MAC is not present in saved config)
    
    Example, delete a client, mac is 00:03:7F:22:43:2B
    ```shell
    client@station.delete[ 00:03:7F:22:43:2B ]
    ttrue
    ```   

    Example, delete a client, mac is 34:52:12:ED:FE:10
    ```shell
    client@station.delete[ 345212EDFE10 ]
    ttrue
    ```   


+ `list[]` **list current all client infomation**   
    - Combines **currently seen LAN clients**, **DHCP lease names/addresses** (**`client@dhcps.list`**), and **saved settings** for each MAC. Not every field is present for every client (e.g. offline devices may lack fresh online info).
    - Returns **JSON** keyed by MAC. Calls may fail like other component APIs if the service is unavailable; otherwise the result is a JSON object (possibly empty).
    - Typical fields include **`ip`**, **`name`**, **`ifname`**, **`netdev`**, **`uptime`**, **`livetime`** (human-readable online time when the client is seen).
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "name":"client name",                 // [ string ]
            "ip":"ip address",                    // [ IP address ]
            "ifname":"connected ifname",          // [ "ifname@lan", "ifname@lan2", ... ]
            "netdev":"kernel netdev",             // [ string ], e.g. br-lan
            "uptime": 0,                          // [ number ]
            "livetime":"connected time"           // [ string ], format is day:hour:minute:second (e.g., "14:39:34:1" means 1 day 14 hours 39 minutes 34 seconds), offline when no such property
            // ... more client attribute
        }
        // ... more client
    }
    ```   

    Example, list current all client
    ```shell
    client@station.list
    {
        "04:CF:8C:39:91:7A":            # first client
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",    # hostname is xiaomi-aircondition-ma2_mibt917A
            "ip":"192.168.31.140",                        # ip is 192.168.31.140
            "ifname":"ifname@lan"                         # the client from the ifname@lan
        },
        "40:31:3C:B5:6D:4C":            # second client
        {
            "ip":"192.168.31.61",
            "ifname":"ifname@lan",                        # the client from the ifname@lan
            "name":"minij-washer-v5_mibt6D4C",
            "livetime":"14:39:34:1"                       # livetime is 1 day 14 hour 39 minute 34 second
        },
        "14:13:46:C9:97:C7":            # third client
        {
            "ip":"192.168.31.9",
            "ifname":"ifname@lan",                        # the client from the ifname@lan
            "livetime":"14:39:26:1"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "ifname":"ifname@lan2",                       # the client from the ifname@lan2
            "name":"Xiaomi-14-Ultra",
            "livetime":"14:39:27:1"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "ifname":"ifname@lan2",                        # the client from the ifname@lan2
            "name":"Xiaomi-13-Ultra",
            "livetime":"00:00:36:0"
        }
    }
    ```   

+ `ip2mac[ ip ]` **resolve IPv4 to MAC**
    - **ip** — IPv4 address. Returns the **MAC** currently associated on the LAN, or **NULL** if unknown or not found.
    - Read-only. For a full client list, use **`list`**.

### Lifecycle API
+ `setup[]` **start LAN client monitoring**, *succeed return ttrue* — **`init` → `general` → `client@station.setup`** in the default package. On **slave** builds, monitoring is not started; otherwise applies **`bindip`** / **`arpbind`** bindings and starts background monitoring.

+ `shut[]` **stop LAN client monitoring**, *succeed return ttrue* — clears fixed bindings and stops monitoring (**not** listed in stock **`uninit`**; add per product).


### Published Joint Events
Other components can **subscribe** (see **`joint_register`**). Raised when a LAN client **appears**, **disappears**, or **changes IP**.

| Event | Description |
|-------|-------------|
| `station/appear` | New client or **new IP** after the old address was reported gone. Payload: **`ip`**, **`mac`**, **`ifname`**, **`netdev`**. |
| `station/disappear` | Client offline or **IP** about to change. Payload: **`ip`**, **`mac`**, **`ifname`**, **`netdev`**. |

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_client_station(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@station", "status") == NULL)
        return -1;
    return ssets_string("client@station", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@station", "list", NULL); talk_free if JSON */
```
