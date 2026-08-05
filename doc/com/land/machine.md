## land@machine — Machine Information Management

### Overview

Manage machine hardware information, system configuration, and power operations.
- query machine status including platform, hardware, version, uptime, and memory
- manage machine configuration such as name, mode, and network ports
- restart, reboot, and restore factory defaults
- block and unblock restart and default restore operations
- query CPU, memory, process, and filesystem information



### Configuration reference ( land@machine )

```json
// Attributes introduction 
{
    "name":"machine hostname",                     // [ string ], the system hostname
    "mode":"machine working mode",                 // [ "ap", "wisp", "nwisp", "gateway", "dgateway", "tgateway", "qgateway", "misp", "nmisp", "dmisp", "mwm", "mix" ], default be "default"
                                                       // "ap": access point
                                                       // "wisp": 2.4G Wireless Internet Service Provider connection
                                                       // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wireless base
                                                       // "gateway": wire WAN gateway
                                                       // "dgateway": Dual wire WAN gateway
                                                       // "tgateway": Three wire WAN gateway
                                                       // "qgateway": Quartered wire WAN gateway
                                                       // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                       // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseb
                                                       // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR b
                                                       // "mwm": multiple LTE and Wireless gateway
                                                       // "mix": custom mix connection from multiple internet connection
                                                       // "mbridge" or "default": LAN dhcps status set disable

    "sn":"serial number",                          // [ string ], read-only, from EEPROM, cannot be changed
    "mac":"MAC address",                           // [ string ], read-only, from EEPROM, cannot be changed
    "macid":"MAC ID",                              // [ string ], read-only, from EEPROM, cannot be changed
    "model":"product model",                       // [ string ], read-only, from EEPROM
    "features":"product features",                 // [ string ], read-only, from EEPROM
    "cmodel":"custom model",                       // [ string ], read-only, from EEPROM
    "oem":"OEM information",                       // [ string ], read-only, from EEPROM
    "magic":"magic number",                        // [ string ], read-only, from EEPROM
    "datecode":"manufacture date code",            // [ string ], read-only, from EEPROM
    "language":"system language",                  // [ string ], stored in EEPROM, writable
    "gpversion":"general purpose version",         // [ string ], stored in EEPROM, writable
    "cfgversion":"configuration version"           // [ string ], stored in EEPROM, writable
}
```

#### Configuration example

Example, show all the machine configure
```shell
land@machine
{
    "name":"SkinOS",                             # machine hostname
    "mode":"default",                            # working mode
    "sn":"20240001",                             # serial number
    "mac":"AA:BB:CC:DD:EE:FF",                   # MAC address
    "model":"R2000",                             # product model
    "language":"cn",                             # system language
    "gpversion":"1.0.0",                         # general purpose version
    "cfgversion":"1.0.0"                         # configuration version
}
```

#### Configuration settings example

Example, set the machine hostname
```shell
land@machine:name=MyRouter
ttrue
```

Example, set the machine working mode to LTE Mobile Router
```shell
land@machine:mode=misp
ttrue
```

Example, merge set the machine configure( include "name" "mode" "language" )
```shell
land@machine|{"name":"MyRouter","mode":"router","language":"en"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the machine component**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Scans and registers projects, sets hostname, stores port and IP info in register

#### Query APIs

+ `status[]` **get the full machine status**
    - failed return NULL
    - succeed return [ json ], machine status including platform, hardware, version, uptime, EEPROM data and ports
    ```json
    {
        "name": "machine hostname",              // [ string ], from persisted land@machine config
        "mode": "working mode",                  // [ string ], from persisted land@machine config
        "platform": "platform identifier",       // [ string ], from register platform
        "hardware": "hardware identifier",        // [ string ], from register hardware
        "custom": "custom profile name",         // [ string ], from register custom
        "scope": "system scope",                 // [ string ], from register scope
        "version": "firmware version",           // [ string ], register machine_state when non-empty, otherwise register version
        "ill": "health indicator",               // [ number ], present only when register ill is non-zero
        "rand": "random number",                 // [ number ], from register rand
        "livetime": "system uptime",             // [ string ], uptime description
        "current": "current time",               // [ string ], current date and time description
        "mac": "MAC address",                    // [ string ], from arch@data EEPROM
        "macid": "MAC ID",                       // [ string ], from arch@data EEPROM
        "model": "product model",                // [ string ], from arch@data EEPROM
        "features": "product features",          // [ string ], from arch@data EEPROM
        "cmodel": "custom model",                // [ string ], from arch@data EEPROM
        "oem": "OEM information",                // [ string ], from arch@data EEPROM
        "magic": "magic number",                 // [ string ], from arch@data EEPROM
        "datecode": "manufacture date code",     // [ string ], from arch@data EEPROM
        "gpversion": "general purpose version",  // [ string ], from arch@data EEPROM
        "cfgversion": "configuration version",   // [ string ], from arch@data EEPROM
        "wui_port": "web UI port",               // [ string ], from register wui_port
        "telnet_port": "telnet port",            // [ string ], from register telnet_port
        "ssh_port": "SSH port",                  // [ string ], from register ssh_port
        "local_ip": "LAN IP address"             // [ string ], from register local_ip
    }
    ```

    Example, get machine status
    ```shell
    land@machine.status
    {
        "name":"SkinOS",                         # machine hostname
        "mode":"default",                        # working mode
        "platform":"rk3568",                     # platform identifier
        "hardware":"R2000",                      # hardware identifier
        "custom":"default",                      # custom profile name
        "scope":"product",                       # system scope
        "version":"8.0.0",                       # firmware version
        "livetime":"3d 2h 15m",                  # system uptime
        "current":"2025-01-15 10:30:00",         # current date and time
        "rand":12345,                            # random number
        "mac":"AA:BB:CC:DD:EE:FF",               # MAC address
        "model":"R2000",                         # product model
        "features":"lte,wifi",                   # product features
        "wui_port":"80",                         # web UI port
        "telnet_port":"23",                      # telnet port
        "ssh_port":"22",                         # SSH port
        "local_ip":"192.168.1.1"                 # LAN IP address
    }
    ```

+ `cpuinfo[]` **get CPU usage information from /proc/stat**
    - failed return NULL
    - succeed return [ json ], CPU usage statistics per core
    ```json
    {
        "cpu name":                              // [ string ]: { json }, CPU identifier (cpu, cpu0, cpu1, ...)
        {                                             // CPU usage counters
            "user": "user time",                 // [ number ], user mode time in jiffies
            "nice": "nice time",                 // [ number ], nice user mode time in jiffies
            "system": "system time",             // [ number ], kernel mode time in jiffies
            "idle": "idle time",                 // [ number ], idle time in jiffies
            "iowait": "io wait time",            // [ number ], I/O wait time in jiffies
            "irq": "irq time",                   // [ number ], interrupt request time in jiffies
            "softirq": "softirq time"            // [ number ], soft interrupt time in jiffies
        }
        // "...":{...}  How many CPUs show how many properties
    }
    ```

    Example, get CPU information
    ```shell
    land@machine.cpuinfo
    {
        "cpu":
        {
            "user":"12345",                      # user mode time
            "nice":"0",                          # nice user mode time
            "system":"6789",                     # kernel mode time
            "idle":"987654",                     # idle time
            "iowait":"123",                      # I/O wait time
            "irq":"45",                          # interrupt time
            "softirq":"67"                       # soft interrupt time
        }
    }
    ```

+ `meminfo[]` **get memory usage information from /proc/meminfo**
    - failed return NULL
    - succeed return [ json ], memory usage information (all values in kB)
    ```json
    {
        "total": "total memory",                 // [ number ], MemTotal
        "free": "free memory",                   // [ number ], MemFree (completely unused)
        "buffers": "buffer memory",              // [ number ], Buffers
        "cached": "cached memory",               // [ number ], Cached (not SwapCached)
        "available": "available memory"          // [ number ], MemAvailable if present; else free+buffers+cached for old kernels
    }
    ```

    Example, get memory information
    ```shell
    land@machine.meminfo
    {
        "total":"524288",                        # total memory 512MB
        "free":"262144",                         # MemFree 256MB
        "buffers":"16384",                       # buffer memory 16MB
        "cached":"131072",                       # cached memory 128MB
        "available":"409600"                     # usable estimate for UI / new apps
    }
    ```

+ `psinfo[]` **get process information from /proc**
    - failed return NULL
    - succeed return [ json ], a map of PID to process status
    ```json
    {
        "pid":                                   // [ string ]: { json }, process ID
        {                                             // process status information
            "name": "process name",              // [ string ], process name
            "state": "process state",            // [ string ], process state (R, S, D, Z, T, etc.)
            "vmsize": "virtual memory size",     // [ number ], virtual memory size in kB
            "fdsize": "file descriptor size",    // [ number ], number of allocated file descriptors
            "ppid": "parent process ID"          // [ number ], parent process ID
        }
        // "...":{...}  How many processes show how many properties
    }
    ```

    Example, get process information
    ```shell
    land@machine.psinfo
    {
        "1":
        {
            "name":"init",                       # process name
            "state":"S",                         # sleeping state
            "vmsize":"1234",                     # virtual memory size
            "fdsize":"32",                       # file descriptors
            "ppid":"0"                           # parent PID
        }
    }
    ```

+ `psnumber[]` **get the total number of running processes**
    - failed return NULL
    - succeed return [ number ], the count of running processes

    Example, count running processes
    ```shell
    land@machine.psnumber
    45
    ```

+ `fsinfo[]` **get filesystem usage information from df**
    - failed return NULL
    - succeed return [ json ], a map of mount point to filesystem information
    ```json
    {
        "mount point":                           // [ string ]: { json }, filesystem mount point
        {                                             // filesystem usage information
            "filesystem": "device path",         // [ string ], the device or filesystem path
            "size": "total size",                // [ string ], total size
            "used": "used size",                 // [ string ], used size
            "available": "available size",       // [ string ], available size
            "use": "usage percentage"            // [ string ], usage percentage
        }
        // "...":{...}  How many filesystems show how many properties
    }
    ```

    Example, get filesystem information
    ```shell
    land@machine.fsinfo
    {
        "/":
        {
            "filesystem":"/dev/root",            # device path
            "size":"256M",                       # total size
            "used":"128M",                       # used size
            "available":"128M",                  # available size
            "use":"50%"                          # usage percentage
        }
    }
    ```

+ `sginfo[]` **get storage device information**
    - failed return NULL
    - succeed return [ json ], a map of storage device name to path and usage
    ```json
    {
        "device name":                           // [ string ]: { json }, storage device name (config, sd0, etc.)
        {                                             // storage device information
            "path": "mount path",                // [ string ], mount path of the storage device
            "size": "total size",                // [ string ], total size
            "used": "used size",                 // [ string ], used size
            "available": "available size",       // [ string ], available size
            "use": "usage percentage"            // [ string ], usage percentage
        }
        // "...":{...}  How many storage devices show how many properties
    }
    ```

    Example, get storage device information
    ```shell
    land@machine.sginfo
    {
        "config":
        {
            "path":"/skinos/cfg",                # config partition mount path
            "size":"16M",                        # total size
            "used":"4M",                         # used size
            "available":"12M",                   # available size
            "use":"25%"                          # usage percentage
        }
    }
    ```

+ `esinfo[]` **get external storage device information excluding config and internal**
    - failed return NULL
    - succeed return [ json ], a map of external storage device name to path and usage, same structure as sginfo but excluding config and internal partitions
    ```json
    {
        "device name":                           // [ string ]: { json }, external storage device name (sd0, sd1, etc.)
        {                                             // storage device information
            "path": "mount path",                // [ string ], mount path of the storage device
            "size": "total size",                // [ string ], total size with unit
            "used": "used size",                 // [ string ], used size with unit
            "available": "available size",       // [ string ], available size with unit
            "use": "usage percentage"            // [ string ], usage percentage
        }
        // "...":{...}  How many external storage devices show how many properties
    }
    ```

    Example, get external storage information
    ```shell
    land@machine.esinfo
    {
        "sd0":
        {
            "path":"/mnt/sd0",                   # SD card mount path
            "size":"7G",                         # total size
            "used":"2G",                         # used size
            "available":"5G",                    # available size
            "use":"28%"                          # usage percentage
        }
    }
    ```

#### Control APIs

+ `restart[ delay, key ]` **restart the machine**
    - delay ----------- [ string ], optional, delay in seconds before restart
    - key ------------- [ string ], optional, a reason or identifier for the restart
    - failed return tfalse, restart is locked or blocked
    - succeed return ttrue

    Example, restart the machine immediately
    ```shell
    land@machine.restart
    ttrue
    ```

    Example, restart the machine after 10 seconds
    ```shell
    land@machine.restart[ 10, firmware upgrade ]
    ttrue
    ```

+ `reboot[ delay, key ]` **reboot the machine, same as restart**
    - delay ----------- [ string ], optional, delay in seconds before reboot
    - key ------------- [ string ], optional, a reason or identifier for the reboot
    - failed return tfalse
    - succeed return ttrue

    Example, reboot the machine
    ```shell
    land@machine.reboot
    ttrue
    ```

+ `restart_block[]` **block machine restart operations**
    - failed return tfalse
    - succeed return ttrue

    Example, block restart
    ```shell
    land@machine.restart_block
    ttrue
    ```

+ `reboot_block[]` **block machine reboot operations, same as restart_block**
    - failed return tfalse
    - succeed return ttrue

    Example, block reboot
    ```shell
    land@machine.reboot_block
    ttrue
    ```

+ `restart_unblock[]` **unblock machine restart operations**
    - failed return tfalse
    - succeed return ttrue

    Example, unblock restart
    ```shell
    land@machine.restart_unblock
    ttrue
    ```

+ `reboot_unblock[]` **unblock machine reboot operations, same as restart_unblock**
    - failed return tfalse
    - succeed return ttrue

    Example, unblock reboot
    ```shell
    land@machine.reboot_unblock
    ttrue
    ```

+ `default[]` **restore default configuration and restart**
    - failed return tfalse, operation is locked or blocked
    - succeed return ttrue

    Example, restore default configuration
    ```shell
    land@machine.default
    ttrue
    ```

+ `default_block[]` **block default restore operations**
    - failed return tfalse
    - succeed return ttrue

    Example, block default restore
    ```shell
    land@machine.default_block
    ttrue
    ```

+ `default_unblock[]` **unblock default restore operations**
    - failed return tfalse
    - succeed return ttrue

    Example, unblock default restore
    ```shell
    land@machine.default_unblock
    ttrue
    ```

+ `release[]` **run release operation and restart**
    - failed return tfalse, operation is locked or blocked
    - succeed return ttrue

    Example, run release operation
    ```shell
    land@machine.release
    ttrue
    ```

+ `factory[]` **run factory reset and restart**
    - failed return tfalse, operation is locked or blocked
    - succeed return ttrue

    Example, run factory reset
    ```shell
    land@machine.factory
    ttrue
    ```



### Published Joint Events

The following joint events are published when machine state changes. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `machine/status` | Sent when the machine state changes. Triggered by `restart` when the machine_state register is set to "restarting". The event parameter is NULL. |
