## land@daemon — Daemon & Watchdog

### Overview

The daemon executable is the main supervisor process of the system. It feeds the hardware watchdog, monitors memory usage, checks the local network interface, and periodically scans registered services, restarting any that have exited unexpectedly.
- feed the hardware watchdog at configurable intervals
- monitor free memory and reboot when memory is critically low
- check local network interface presence and reboot on prolonged absence
- scan and manage registered services, restart exited services automatically
- accept control commands via command-line arguments



### Configuration reference ( land@daemon )

```json
// Attributes introduction 
{
    "service_check":"service scan interval",                       // [ number ], interval in seconds to scan registered services, default be 3

    "watchdog_file":"watchdog device file path",                   // [ string ], path to the hardware watchdog device (e.g. /dev/watchdog), empty means disabled
    "watchdog_interval":"watchdog feed interval",                  // [ number ], feed interval in microseconds, default be 1000000 (1 second)

    "memory_check":"memory check interval",                        // [ number ], interval in seconds to check free memory, 0 means disabled, default be 10
    "memory_warn":"warn threshold of free memory",                 // [ number ], warn when free memory falls below this value in kB, 0 means disabled
    "memory_reboot":"reboot threshold of free memory",             // [ number ], reboot when free memory falls below this value in kB, 0 means disabled

    "local_check":"network local check interval",                  // [ number ], interval in seconds to check local network interface, 0 means disabled, default be 30
    "local_disbuild":"reboot threshold when local missing at setup",  // [ number ], number of consecutive checks before reboot when local interface never appears, default be 20
    "local_disappear":"reboot threshold when local disappears",    // [ number ], number of consecutive checks before reboot when local interface disappears, default be 10
    "local_ifname":"local interface object name"                   // [ string ], object name that provides netdev API for local interface check (e.g. ifname@local)
}
```

#### Configuration example

Example, show all the daemon configure
```shell
land@daemon
{
    "service_check":"5",                       # scan registered services every 5 seconds
    "watchdog_file":"/dev/watchdog",           # hardware watchdog device path
    "watchdog_interval":"1000000",             # feed watchdog every 1000000 microseconds (1 second)
    "memory_check":"10",                       # check free memory every 10 seconds
    "memory_warn":"3000",                      # warn when free memory below 3000 kB
    "memory_reboot":"800",                     # reboot when free memory below 800 kB
    "local_check":"30",                        # check local network interface every 30 seconds
    "local_disbuild":"20",                     # reboot after 20 consecutive local-missing checks
    "local_disappear":"10",                    # reboot after 10 consecutive local-disappear checks
    "local_ifname":"ifname@local"              # local network interface object name
}
```

#### Configuration settings example

Example, set the service scan interval to 5 seconds
```shell
land@daemon:service_check=5
ttrue
```

Example, set the watchdog feed interval to 2 seconds
```shell
land@daemon:watchdog_interval=2000000
ttrue
```

Example, merge set the daemon configure( include "service_check" "watchdog_interval" "memory_check" )
```shell
land@daemon|{"service_check":"5","watchdog_interval":"2000000","memory_check":"10"}
ttrue
```



### Other

The daemon executable accepts the following command-line control commands.

+ `daemon exit` **stop the daemon immediately**
    - sends SIGINT to the running daemon process
    - the daemon will stop all services and exit

    Example, stop the daemon immediately
    ```shell
    daemon exit
    ```

+ `daemon stop15exit` **stop the daemon after 15 seconds**
    - sends SIGXFSZ to the running daemon process
    - the daemon will suspend service startup and exit after 15 seconds

    Example, stop the daemon after 15 seconds
    ```shell
    daemon stop15exit
    ```

+ `daemon delay600exit` **stop the daemon after 600 seconds**
    - sends SIGWINCH to the running daemon process
    - the daemon will exit after 600 seconds (10 minutes)

    Example, stop the daemon after 600 seconds
    ```shell
    daemon delay600exit
    ```

+ `daemon flashid` **show the flash ID of the device**
    - reads and prints the flash identification string

    Example, show the flash ID
    ```shell
    daemon flashid
    0123456789ABCDEF
    ```
