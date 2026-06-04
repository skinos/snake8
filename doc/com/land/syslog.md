## land@syslog — System Log Management

### Overview

Manage the system log service including log output mode, log level filtering, log file location, remote syslog forwarding, and log file operations.
- configure log output mode: syslog, tui terminal, both, or file only
- set global log level and per-component log level filtering
- forward logs to a remote syslog server
- display, list, and delete log files
- write log messages at different severity levels
- critical log to internal storage with automatic rotation



### Configuration reference ( land@syslog )

```json
// Attributes introduction 
{
    "status":"log output mode",                    // [ "disable", "enable", "tui", "both", "file" ], default be "disable"
                                                      // "disable": logging disabled
                                                      // "enable": output to syslog
                                                      // "tui": output to terminal
                                                      // "both": output to syslog and terminal
                                                      // "file": output to file only
    "trace":"trace mode",                          // [ "disable", "enable" ], default be "disable"
    "level":"global log level",                    // [ "verb", "debug", "info", "warn", "fault" ], set the global log level, default be "info"
                                                      // "verb": verbose, debug, info, warn, fault
                                                      // "debug": debug, info, warn, fault
                                                      // "info": info, warn, fault
                                                      // "warn": warn, fault
                                                      // "fault": fault only

    "fault":"fault level component filter",        // [ string ], semicolon-separated component names to enable fault logging, empty means none
    "warn":"warn level component filter",          // [ string ], semicolon-separated component names to enable warn logging, empty means none
    "info":"info level component filter",          // [ string ], semicolon-separated component names to enable info logging, empty means none
    "debug":"debug level component filter",        // [ string ], semicolon-separated component names to enable debug logging, empty means none
    "verb":"verbose level component filter",       // [ string ], semicolon-separated component names to enable verbose logging, empty means none

    "remote":"remote syslog server address",       // [ string ], IP address or hostname of remote syslog server, empty means disabled
    "port":"remote syslog server port",            // [ string ], port number for remote syslog, default be "514"
    "klog":"kernel log",                           // [ "disable", "enable" ], enable kernel log daemon, default be "disable"
    "critical":"critical log",                     // [ "disable", "enable" ], enable critical log to internal storage, default be "disable"
    "critical_size":"critical log size limit in KB", // [ number ], maximum critical log file size in kilobytes, default be 100

    "location":"log file storage location",        // [ "storage", "internal", "sd*", "mm*", "<path>" ], where to store log files
                                                      // "storage": use the first available storage device
                                                      // "internal": use internal flash storage
                                                      // "sd*": use specific SD card (e.g. "sd0", "sd1")
                                                      // "mm*": use specific MMC storage (e.g. "mm0")
                                                      // "/path": use an absolute directory path
                                                      // empty or unset: use default var directory
    "size":"log file size limit in KB"             // [ number ], maximum log file size in kilobytes, default be 5 for internal or 100 for storage
}
```

#### Configuration example

Example, show all the syslog configure
```shell
land@syslog
{
    "status":"enable",                         # output to syslog
    "level":"info",                            # global log level is info
    "remote":"192.168.1.100",                  # forward to remote syslog server
    "port":"514",                              # remote syslog port
    "klog":"enable",                           # kernel log enabled
    "critical":"enable",                       # critical log enabled
    "critical_size":"50",                      # critical log file size limit 50KB
    "location":"storage",                      # log stored on storage device
    "size":"100"                               # log file size limit 100KB
}
```

#### Configuration settings example

Example, enable the syslog output
```shell
land@syslog:status=enable
ttrue
```

Example, set global log level to debug
```shell
land@syslog:level=debug
ttrue
```

Example, merge set the syslog configure( include "status" "level" "remote" "port" )
```shell
land@syslog|{"status":"enable","level":"debug","remote":"192.168.1.100","port":"514"}
ttrue
```

Example, enable the critical log
```shell
land@syslog:critical=enable
ttrue
```

Example, merge set the syslog configure with critical log( include "status" "critical" "critical_size" )
```shell
land@syslog|{"status":"enable","critical":"enable","critical_size":"50"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the syslog service**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Reads the configuration, sets log options and level mask, starts syslogd and optionally klogd

+ `shut[]` **stop the syslog service**
    - failed return tfalse
    - succeed return ttrue
    - Kills syslogd and klogd processes

#### Query APIs

+ `path[]` **get the current log file path and size limit**
    - failed return NULL
    - succeed return [ json ], log file path and size information
    ```json
    {
        "path": "log file path",           // [ string ], absolute path to the current log file
        "size": "size limit in KB"         // [ number ], log file size limit in kilobytes
    }
    ```

    Example, get the log file path
    ```shell
    land@syslog.path[]
    {
        "path":"/var/log/12345-syslog.log",   # absolute path to the log file
        "size":5                              # size limit in KB
    }
    ```

+ `list[]` **list all log files in the log directory**
    - failed return NULL
    - succeed return [ json ], a map of log filename to full file path, including critical log files if enabled
    ```json
    {
        "log filename": "full file path",  // [ string ]: [ string ], log filename and its absolute path
        // "...":"..."  How many log files show how many properties
    }
    ```

    Example, list all log files
    ```shell
    land@syslog.list
    {
        "12345-syslog.log":"/var/log/12345-syslog.log",    # log filename and path
        "critical.txt":"/var/internal/critical.txt",        # critical log file
        "critical.0.txt":"/var/internal/critical.0.txt"    # rotated critical log file
    }
    ```

+ `mask[]` **dump the current log level mask for all components**
    - failed return tfalse
    - succeed return ttrue
    - Outputs the log mask table showing which levels are enabled for each component type and subtype

    Example, dump the log mask
    ```shell
    land@syslog.mask[]
    ttrue
    ```

+ `show[ html ]` **display the contents of the current log file**
    - html ----------- [ string ], optional, when set to "html", output as an HTML table
    - failed return tfalse, log file not found
    - succeed return ttrue

    Example, show log file contents
    ```shell
    land@syslog.show
    ttrue
    ```

    Example, show log file as HTML table
    ```shell
    land@syslog.show[ html ]
    ttrue
    ```

+ `critical_show[ html ]` **display the contents of the critical log file**
    - html ----------- [ string ], optional, when set to "html", output as an HTML table
    - failed return tfalse, critical log file not found
    - succeed return ttrue
    - Shows the rotated backup file (.0) first, then the current critical log file

    Example, show critical log file contents
    ```shell
    land@syslog.critical_show
    ttrue
    ```

    Example, show critical log file as HTML table
    ```shell
    land@syslog.critical_show[ html ]
    ttrue
    ```

#### Control APIs

+ `clear[]` **delete the current log file**
    - failed return tfalse
    - succeed return ttrue

    Example, clear the log file
    ```shell
    land@syslog.clear[]
    ttrue
    ```

+ `delete[ file ]` **delete a specific log file**
    - file ----------- [ string ], the log filename to delete (as returned by list, including "critical.txt" and "critical.0.txt")
    - failed return tfalse
    - succeed return ttrue

    Example, delete a specific log file
    ```shell
    land@syslog.delete[ 12345-syslog.log.0 ]
    ttrue
    ```

    Example, delete the critical log file
    ```shell
    land@syslog.delete[ critical.txt ]
    ttrue
    ```

+ `debug[ message, ... ]` **write a debug level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a debug log message
    ```shell
    land@syslog.debug[ connection established from 192.168.1.1 ]
    ttrue
    ```

+ `info[ message, ... ]` **write an info level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write an info log message
    ```shell
    land@syslog.info[ system startup complete ]
    ttrue
    ```

+ `warn[ message, ... ]` **write a warning level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a warning log message
    ```shell
    land@syslog.warn[ disk space low ]
    ttrue
    ```

+ `fault[ message, ... ]` **write a fault level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a fault log message
    ```shell
    land@syslog.fault[ failed to connect to database ]
    ttrue
    ```

+ `add_fault[ type, ... ]` **enable fault level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons (e.g. "land;arch;network@connect")
    - failed return tfalse
    - succeed return ttrue

    Example, enable fault logging for land and arch components
    ```shell
    land@syslog.add_fault[ land;arch ]
    ttrue
    ```

+ `add_warn[ type, ... ]` **enable warn level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable warn logging for network components
    ```shell
    land@syslog.add_warn[ network ]
    ttrue
    ```

+ `add_info[ type, ... ]` **enable info level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable info logging for ifname components
    ```shell
    land@syslog.add_info[ ifname ]
    ttrue
    ```

+ `add_debug[ type, ... ]` **enable debug level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable debug logging for modem components
    ```shell
    land@syslog.add_debug[ modem ]
    ttrue
    ```

+ `add_verb[ type, ... ]` **enable verbose level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable verbose logging for uart components
    ```shell
    land@syslog.add_verb[ uart ]
    ttrue
    ```

+ `del_fault[ type, ... ]` **disable fault level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable fault logging for land components
    ```shell
    land@syslog.del_fault[ land ]
    ttrue
    ```

+ `del_warn[ type, ... ]` **disable warn level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable warn logging for network components
    ```shell
    land@syslog.del_warn[ network ]
    ttrue
    ```

+ `del_info[ type, ... ]` **disable info level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable info logging for ifname components
    ```shell
    land@syslog.del_info[ ifname ]
    ttrue
    ```

+ `del_debug[ type, ... ]` **disable debug level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable debug logging for modem components
    ```shell
    land@syslog.del_debug[ modem ]
    ttrue
    ```

+ `del_verb[ type, ... ]` **disable verbose level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable verbose logging for uart components
    ```shell
    land@syslog.del_verb[ uart ]
    ttrue
    ```
