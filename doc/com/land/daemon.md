
## Syslog Management   
Manage system daemon

#### **Configuration( land@daemon )**   
```json
// Attributes introduction 
{
    "service_check":"check the service interval",                   // [ number ], the unit is second

    "watchdog_file":"watchdog file",                                // [ file path ]
    "watchdog_interval":"watchdog interval",                        // [ number ], the unit is second

    "memory_check":"check memory interval",                           // [ number ], the unit is second
    "memory_warn":"warn when memory less than this",                  // [ number ]
    "memory_reboot":"reboot when memory less then this",              // [ number ]

    "local_check":"check local interval",                             // [ number ], the unit is second
    "local_disbuild":"check local failed to restart when setup",      // [ number ], the unit is second
    "local_disappear":"check local failed to restsrt when disappear", // [ number ], the unit is second
    "local_ifname":"check the local netdev of ifname"                 // [ string ]

}
```

