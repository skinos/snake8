## client@dhcps — Management of the DHCP server
Management of DHCP server  
DHCP server settings are configured per logical **ifname**. **`client@dhcps.list`** returns clients currently known from the DHCP lease list (what the server has assigned), not a live probe of each device.

### Configuration ( `client@dhcps` )
```json
// Attributes introduction 
{
    "interface name":                              // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], DHCP settings for this logical ifname
    {
        "status":"dhcp server status",         // [ "disable", "enable" ]
        "startip":"dhcp pool start ip",        // [ ip address ] first IP of the DHCP range
        "endip":"dhcp pool end ip",            // [ ip address ] last IP of the DHCP range
        "mask":"subnet mask",                  // [ ip address ] optional, defaults to interface static mask
        "lease":"lease time",                  // [ number ] seconds; values below 120 may be raised to 120 when applied
        "gw":"gateway address",                // [ ip address ] optional, router option sent to clients
        "dns":"primary dns server",            // [ ip address ] optional, DNS server option sent to clients
        "dns2":"secondary dns server",         // [ ip address ] optional, secondary DNS server
        "routeopt_table":"static routes",      // [ array ] optional, static route options (RFC 3442 classless route option)
        // each entry has "target" (network), "mask" (CIDR bits), "gw" (gateway)
        "options":"extra config lines",        // [ string ] optional, extra dnsmasq config lines separated by ';'
        "mode":"dhcp mode",                    // [ "static" ] optional, "static" for static IP assignment mode
        "dnsproxy":"dns proxy settings"        // [ object ] optional, DNS proxy/redirect configuration
        // contains "dns" field for the DNS server to redirect all queries to
    }
    // more ifname
}
```   

Examples, show all the DHCP server configure
```shell
client@dhcps
{
    "ifname@lan":                              # DHCP settings for ifname@lan
    {
        "status":"enable",                         # DHCP server is enabled
        "startip":"192.168.31.100",                # DHCP pool starts at 192.168.31.100
        "endip":"192.168.31.254",                  # DHCP pool ends at 192.168.31.254
        "mask":"255.255.255.0",                    # subnet mask 255.255.255.0
        "lease":"86400",                           # lease time is 86400 seconds (24 hours)
        "gw":"192.168.31.1",                       # gateway address sent to clients
        "dns":"8.8.8.8",                           # primary DNS server for clients
        "dns2":"114.114.114.114"                   # secondary DNS server for clients
    }
}
```

### Component API
**Directly callable** APIs from HE / eline / HTTP `/he`.
+ `list[]` **list current dhcp client infomation**   
    - Returns a **JSON object** (empty **`{}`** if there is no lease information yet). Each key is a client **MAC**; each value has **`ip`** and **`name`** (hostname from DHCP).
    - For online duration and richer client view, use **`client@station.list`**.
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "ip":"ip address",                    // [ IP address ]
            "name":"client name",                 // [ string ]
        }
        // ... more client
    }
    ```   

    Example, list current all client of DHCP server
    ```shell
    client@dhcps.list
    {
        "04:CF:8C:39:91:7A":            # first client
        {
            "name":"xiaomi-aircondition-ma2_mibt917A",    # hostname is xiaomi-aircondition-ma2_mibt917A
            "ip":"192.168.31.140",                        # ip is 192.168.31.140
        },
        "40:31:3C:B5:6D:4C":            # second client
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C"
        },
        "14:13:46:C9:97:C7":            # third client
        {
            "ip":"192.168.31.9",
            "name":"Watch"
        },
        "F6:F7:73:82:0A:FC":
        {
            "ip":"192.168.100.183",
            "name":"Xiaomi-14-Ultra"
        },
        "F6:F7:73:77:1D:3B":
        {
            "ip":"192.168.100.182",
            "name":"Xiaomi-13-Ultra"
        }
    }
    ```   

### Lifecycle API
The following are normally invoked during platform **`init`** / **`uninit`** (after the FPK is loaded) but may be called manually:


+ `setup[]` **start the DHCP server side**, *succeed return ttrue*
    - Called at system startup. On **slave** builds, no DHCP service is started.
    - Otherwise starts the DHCP/IPv6 helper services the image uses (**dnsmasq**, and **odhcpd** when available).

+ `shut[]` **stop the DHCP server side**, *succeed return ttrue*
    - Called at shutdown. Stops the same DHCP-related services that were started.

### Joint Handlers
Typical **joint** wiring (event → handler) on a device with this package:

+ `reset[]` **restart DHCP services after WAN/LAN ifname changes**, *succeed return ttrue*
    - **`network/on`** and **`network/off`** → **`client@dhcps.reset`**.
    - On **slave** builds, does nothing. Otherwise restarts DHCP services so addressing and leases stay consistent with the new network state.

+ `on[]` **reload DHCP configuration without full restart**, *succeed return ttrue*
    - **`network/online`** and **`network/offline`** → **`client@dhcps.on`**.
    - On **slave** builds, does nothing. Otherwise tells the DHCP services to reload their configuration.

### C Code Example
**Read and update configuration**

```c
#include "skin/skin.h"

static int example_config_client_dhcps(void)
{
    char buf[128];
    if (sgets_string(buf, sizeof(buf), "client@dhcps", "status") == NULL)
        return -1;
    return ssets_string("client@dhcps", "enable", "status") ? 0 : -1;
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

/* e.g. scall("client@dhcps", "list", NULL); talk_free if JSON */
```
