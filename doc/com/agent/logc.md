## agent@logc — System Log Management

### Overview

Own `/dev/log` in place of busybox syslogd. Each datagram is one line and is written to the local rotating file, optionally sent as classic UDP syslog, and optionally uploaded to `center@log` over TCP.
- configure log output mode: syslog, tui terminal, both, or file only
- set global log level and per-component log level filtering
- forward logs to a remote syslog server over UDP
- display, list, and delete log files
- write log messages at different severity levels
- critical log to internal storage
- optional TCP uplink to center: first line is `{macid};{user}\n` (same account string as `agent@heclient` `user`); later lines match the local file format (`stamp host fac.prio msg\n`); any `\n`/`\r` inside the message body is replaced with `\t` so each record is one physical line
- `center_ssl` = `enable` uses SSL encryption on the center TCP link; `disable` is plain TCP (both ends must match)
- TCP connect / SSL handshake budget is 15s; after the link is up, write-idle timeout is 120s (aligned with TCP keepalive idle)
- local file optional: **no local file means live TCP only** — if the link is down the line is dropped, there is no offline retransmit
- with local files, disconnect resume uses one cursor on `agent@logc` register (`tcp_ack_file` + `tcp_ack_off`, bound as pointers for direct read/write); if that cursor file is deleted the old cache is lost and the cursor moves to the live `log_file` from offset 0; first run with no cursor starts at live EOF; at-least-once (reconnect may repeat whole lines); rotation prefers not to delete the file still pointed by `tcp_ack_file`
    > Object id is **`agent@logc`**. `center@heport` adjust pushes this object (`center` / `center_port` / `center_ssl`). `status` is the log output mode (`enable`/`both` start `/dev/log`; `file` alone does not); UDP uses `remote_server` / `remote_port`; TCP uses `center` / `center_port` / `center_ssl`.



### Configuration reference ( agent@logc )

```json
// Attributes introduction 
{
    "status":"log output mode",                    // [ "disable", "enable", "tui", "both", "file" ], default be "disable"
                                                      // "disable": logging disabled
                                                      // "enable": output to syslog (/dev/log reader started)
                                                      // "tui": output to terminal
                                                      // "both": output to syslog and terminal
                                                      // "file": output to file only
    "klog":"kernel log",                           // [ "disable", "enable" ], enable kernel log daemon, default be "disable"
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

    "file_location":"log file storage location",   // [ "storage", "internal", "sd*", "mm*", "<path>" ], where to store log files
                                                      // "storage": use the first available storage device
                                                      // "internal": use internal flash storage
                                                      // "sd*": use specific SD card (e.g. "sd0", "sd1")
                                                      // "mm*": use specific MMC storage (e.g. "mm0")
                                                      // "/path": use an absolute directory path
                                                      // empty or unset: use default var directory
    "file_size":"log file size limit in KB",       // [ number ], maximum log file size in kilobytes, default be 5 for internal or 100 for storage
    "file_max":"rotated local file count",         // [ number ], max MMDDHHMM-uptime.log.txt files in the log dir, default be 1

    "remote_server":"remote syslog server address", // [ string ], IP address or hostname of remote syslog server, empty means disabled
    "remote_port":"remote syslog server port",     // [ string ], port number for remote syslog, default be "514"

    "center":"TCP upload to center@log",           // [ "disable", "enable" ], default be "disable"; does not stop local file or UDP
    "center_server":"center host",                 // [ string ], optional; empty means inherit agent@heclient server
    "center_port":"center TCP port",               // [ number ], default be 20004
    "center_ssl":"SSL encrypt the center TCP",     // [ "disable", "enable" ], default be "disable"
                                                      // "enable": SSL encryption; "disable": plain TCP

    "critical":"critical log",                      // [ "disable", "enable" ], enable critical log to internal storage, default be "disable"
    "critical_size":"critical log size limit in KB" // [ number ], maximum critical log file size in kilobytes, default be 100

}
```

#### Configuration example

Example, show all the logc configure
```shell
agent@logc
{
    "status":"enable",                         # output to syslog
    "klog":"enable",                           # kernel log enabled
    "level":"info",                            # global log level is info

    "remote_server":"192.168.1.100",            # forward to remote syslog server
    "remote_port":"514",                       # remote syslog port

    "critical":"enable",                       # critical log enabled
    "critical_size":"50",                      # critical log file size limit 50KB

    "file_location":"storage",                 # log stored on storage device
    "file_size":"100",                         # log file size limit 100KB
    "file_max":"3",                            # keep at most three .log.txt files

    "center":"enable",                         # upload to center@log
    "center_port":"20004",                     # center TCP port
    "center_ssl":"disable"                     # plain TCP; set enable for SSL encryption
}
```

#### Configuration settings example

Example, enable the syslog output
```shell
agent@logc:status=enable
ttrue
```

Example, set global log level to debug
```shell
agent@logc:level=debug
ttrue
```

Example, merge set the syslog configure( include "status" "level" "remote_server" "remote_port" )
```shell
agent@logc|{"status":"enable","level":"debug","remote_server":"192.168.1.100","remote_port":"514"}
ttrue
```

Example, enable the critical log
```shell
agent@logc:critical=enable
ttrue
```

Example, turn on TCP upload only (does not change status or UDP port)
```shell
agent@logc|{"center":"enable","center_port":"20004","center_ssl":"disable"}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the syslog service**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Reads the configuration, sets log options and level mask, binds `/dev/log` (no syslogd), and optionally starts klogd

+ `shut[]` **stop the syslog service**
    - failed return tfalse
    - succeed return ttrue
    - Stops the `/dev/log` service and klogd


#### Query APIs

+ `path[]` **get the current log file path and size limit**
    - failed return NULL
    - succeed return [ json ], log file path and size information
    ```json
    {
        "path": "log file path",           // [ string ], absolute path to the current log file (MMDDHHMM-uptime.log.txt)
        "size": "size limit in KB"         // [ number ], log file size limit in kilobytes
    }
    ```

    Example, get the log file path
    ```shell
    agent@logc.path[]
    {
        "path":"/var/log/09211959-3600.log.txt",   # current active log file
        "size":5                              # size limit in KB
    }
    ```

+ `list[]` **list all log files in the log directory**
    - failed return NULL
    - succeed return [ json ], a map of log filename to full file path, including critical log files if enabled
    - local logs are recognized by the `.log.txt` suffix
    ```json
    {
        "log filename": "full file path",  // [ string ]: [ string ], log filename and its absolute path
        // "...":"..."  How many log files show how many properties
    }
    ```

    Example, list all log files
    ```shell
    agent@logc.list
    {
        "09211959-3600.log.txt":"/var/log/09211959-3600.log.txt",
        "09212010-4200.log.txt":"/var/log/09212010-4200.log.txt",
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
    agent@logc.mask[]
    ttrue
    ```

+ `list_type[]` **list all supported component type tokens in a two-level map**
    - failed return NULL
    - succeed return [ json ], outer key is the major type (e.g. "land"), inner key is subtype short name, value is the full type token for add/del APIs
    ```json
    {
        "major type": {                        // [ string ]: [ json ], major type name (e.g. "land", "modem")
            "subtype": "full type token"       // [ string ], subtype short name -> full token (e.g. "auth" -> "land@auth")
            // "...":"..."  How many subtypes show how many properties
        }
        // "...":{...}  How many major types show how many properties
    }
    ```

    Example, list supported type tokens
    ```shell
    agent@logc.list_type
    {
        "land":{
            "default":"land_default",
            "auth":"land@auth",
            "init":"land@init"
        },
        "modem":{
            "default":"modem_default",
            "lte":"modem@lte"
        }
    }
    ```

+ `list_fault[]` **list component types with fault level enabled**
    - failed return NULL
    - succeed return [ json ], keys are type tokens that currently have fault logging enabled in the log mask
    ```json
    {
        "type token": "",                      // [ string ]: [ string ], enabled type token; value is empty
        // "...":""  How many enabled types show how many properties
    }
    ```

    Example, list types with fault enabled
    ```shell
    agent@logc.list_fault
    {
        "land":"",
        "arch@usb":""
    }
    ```

+ `list_warn[]` **list component types with warn level enabled**
    - failed return NULL
    - succeed return [ json ], keys are type tokens that currently have warn logging enabled in the log mask

    Example, list types with warn enabled
    ```shell
    agent@logc.list_warn
    {
        "network":"",
        "ifname@lte":""
    }
    ```

+ `list_info[]` **list component types with info level enabled**
    - failed return NULL
    - succeed return [ json ], keys are type tokens that currently have info logging enabled in the log mask

    Example, list types with info enabled
    ```shell
    agent@logc.list_info
    {
        "land@auth":"",
        "modem@lte":""
    }
    ```

+ `list_debug[]` **list component types with debug level enabled**
    - failed return NULL
    - succeed return [ json ], keys are type tokens that currently have debug logging enabled in the log mask

    Example, list types with debug enabled
    ```shell
    agent@logc.list_debug
    {
        "uart@tty":""
    }
    ```

+ `list_verb[]` **list component types with verbose level enabled**
    - failed return NULL
    - succeed return [ json ], keys are type tokens that currently have verbose logging enabled in the log mask
    - `list_verbose[]` is the same API

    Example, list types with verbose enabled
    ```shell
    agent@logc.list_verb
    {
        "default_shell":""
    }
    ```

+ `show[ arg ]` **display the contents of the current log file**
    - arg ------------ [ string ], optional, first parameter; when "html", output the entire log file as an HTML table; when a numeric string (e.g. "100"), output the latest N log lines as plain text; when omitted or "0", output the entire log file as plain text
    - failed return tfalse, log file not found
    - succeed return ttrue

    Example, show log file contents
    ```shell
    agent@logc.show
    ttrue
    ```

    Example, show the latest 100 log lines
    ```shell
    agent@logc.show[100]
    ttrue
    ```

    Example, show log file as HTML table
    ```shell
    agent@logc.show[ html ]
    ttrue
    ```

+ `critical_path[]` **get the critical log file path and size limit**
    - failed return NULL
    - succeed return [ json ], critical log path and size information
    ```json
    {
        "path": "critical log file path",  // [ string ], absolute path (typically under internal storage)
        "size": "size limit in KB"         // [ number ], critical log size limit in kilobytes
    }
    ```
    - Also refreshes register variables `critical_file` / `critical_limit` used by the logger

    Example, get the critical log path
    ```shell
    agent@logc.critical_path[]
    {
        "path":"/mnt/internal/critical.txt",   # absolute path to the critical log
        "size":100                             # size limit in KB
    }
    ```

+ `critical_show[ html ]` **display the contents of the critical log file**
    - html ----------- [ string ], optional, when set to "html", output as an HTML table
    - failed return tfalse, critical log file not found
    - succeed return ttrue
    - Shows the rotated backup file (.0) first, then the current critical log file

    Example, show critical log file contents
    ```shell
    agent@logc.critical_show
    ttrue
    ```

    Example, show critical log file as HTML table
    ```shell
    agent@logc.critical_show[ html ]
    ttrue
    ```


#### Control APIs

+ `clear[]` **delete the current log file**
    - failed return tfalse
    - succeed return ttrue

    Example, clear the log file
    ```shell
    agent@logc.clear[]
    ttrue
    ```

+ `delete[ file ]` **delete a specific log file**
    - file ----------- [ string ], the log filename to delete (as returned by list, including "critical.txt" and "critical.0.txt")
    - failed return tfalse
    - succeed return ttrue

    Example, delete a specific log file
    ```shell
    agent@logc.delete[ 09212010-4200.log.txt ]
    ttrue
    ```

    Example, delete the critical log file
    ```shell
    agent@logc.delete[ critical.txt ]
    ttrue
    ```

+ `debug[ message, ... ]` **write a debug level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a debug log message
    ```shell
    agent@logc.debug[ connection established from 192.168.1.1 ]
    ttrue
    ```

+ `info[ message, ... ]` **write an info level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write an info log message
    ```shell
    agent@logc.info[ system startup complete ]
    ttrue
    ```

+ `warn[ message, ... ]` **write a warning level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a warning log message
    ```shell
    agent@logc.warn[ disk space low ]
    ttrue
    ```

+ `fault[ message, ... ]` **write a fault level log message**
    - message, ... ----------- [ string ], the message text, multiple arguments are combined with spaces
    - failed return tfalse
    - succeed return ttrue

    Example, write a fault log message
    ```shell
    agent@logc.fault[ failed to connect to database ]
    ttrue
    ```

+ `add_fault[ type, ... ]` **enable fault level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons (e.g. "land;arch;network@connect")
    - failed return tfalse
    - succeed return ttrue

    Example, enable fault logging for land and arch components
    ```shell
    agent@logc.add_fault[ land;arch ]
    ttrue
    ```

+ `add_warn[ type, ... ]` **enable warn level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable warn logging for network components
    ```shell
    agent@logc.add_warn[ network ]
    ttrue
    ```

+ `add_info[ type, ... ]` **enable info level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable info logging for ifname components
    ```shell
    agent@logc.add_info[ ifname ]
    ttrue
    ```

+ `add_debug[ type, ... ]` **enable debug level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, enable debug logging for modem components
    ```shell
    agent@logc.add_debug[ modem ]
    ttrue
    ```

+ `add_verb[ type, ... ]` **enable verbose level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue
    - `add_verbose[]` is the same API

    Example, enable verbose logging for uart components
    ```shell
    agent@logc.add_verb[ uart ]
    ttrue
    ```

+ `del_fault[ type, ... ]` **disable fault level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable fault logging for land components
    ```shell
    agent@logc.del_fault[ land ]
    ttrue
    ```

+ `del_warn[ type, ... ]` **disable warn level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable warn logging for network components
    ```shell
    agent@logc.del_warn[ network ]
    ttrue
    ```

+ `del_info[ type, ... ]` **disable info level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable info logging for ifname components
    ```shell
    agent@logc.del_info[ ifname ]
    ttrue
    ```

+ `del_debug[ type, ... ]` **disable debug level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue

    Example, disable debug logging for modem components
    ```shell
    agent@logc.del_debug[ modem ]
    ttrue
    ```

+ `del_verb[ type, ... ]` **disable verbose level logging for specified component types**
    - type, ... ----------- [ string ], one or more component type names separated by semicolons
    - failed return tfalse
    - succeed return ttrue
    - `del_verbose[]` is the same API

    Example, disable verbose logging for uart components
    ```shell
    agent@logc.del_verb[ uart ]
    ttrue
    ```



### Other

- Center peer is `center@log` (see `../center/log.md`); heport adjust only sets `center` / `center_port` / `center_ssl`
- Short object alias: `log` → `logc` in agent `prj.json`
