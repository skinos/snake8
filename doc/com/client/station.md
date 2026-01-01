***
## Management of all local client   
Management of all local client

#### Configuration( client@station )   
```json
// Attributes introduction 
{
    "client MAC address":                                // [ MAC address ]
    {
        "ifname":"specify on interface name",                 // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], default the "ifname@lan", call "network@frame.list[local]" get the list
        "name":"specify hostname",                            // [ string ]
        "bindip":"specify ip address on dhcp assignment",     // [ ip address ]
        "arpbind":"bind ip set the arp table",                // [ "disable", "enable" ]
        "lease":"specify lease on dhcp assignment"            // [ number ] the unit is second
    }
    // more client MAC address rule
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

#### **API( client@station )**

+ `add[ mac, name, ]` **add a client with name**
    - mac -------------- [ mac address ], the format can be AA:BB:CC:DD:EE:FF or AABBCCDDEEFF
    - name ------------- [ string ], client name
    - succeed return ttrue
    - failed return tfalse

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
    - failed return tfalse
    - error return terror   
    
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
    - return NULL when failed
    - return terror when error
    - return json to describes all client infomation when succeed
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "name":"client name",                 // [ string ]
            "ip":"ip address",                    // [ IP address ]
            "ifname":"connected ifname",          // [ "ifname@lan", "ifname@lan2", ... ]
            "livetime":"connected time"           // [ string ], format is hour:minute:second:day, offline when no such property
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
            "name":"xiaomi-aircondition-ma2_mibt917A"     # hostname is xiaomi-aircondition-ma2_mibt917A
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



