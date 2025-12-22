***
## Management of the DHCP server  
Management of DHCP server

#### Configuration( client@dhcps )   
```json
// Attributes introduction 
{
    "interface name":                        // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        "status":"dhcp server status",         // [ "disable", "enable" ]
        "startip":"dhcp pool start ip",        // [ ip address ]
        "endip":"dhcp pool end ip",            // [ ip address ]
        "lease":"lease time"                   // [ number ]
    }
    // more interface name
}
```   


#### **Methods**

+ `list[]` **list current dhcp client infomation**   
    - return NULL when failed
    - return terror when error
    - return json to describes all client infomation when succeed
    ```json
    // Attributes introduction of json by the API return
    {
        "client mac address":               // [ MAC address ]
        {
            "ip":"ip address"                     // [ IP address ]
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
            "ip":"192.168.31.140"                         # ip is 192.168.31.140
        },
        "40:31:3C:B5:6D:4C":            # second client
        {
            "ip":"192.168.31.61",
            "name":"minij-washer-v5_mibt6D4C"
        },
        "14:13:46:C9:97:C7":            # third client
        {
            "ip":"192.168.31.9",
            "livetime":"14:39:26:1"
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


