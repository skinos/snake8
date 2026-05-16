## land@service — Service Process Management

### Overview

Manage service processes through the daemon supervisor. Services are component methods or programs that run as child processes under the daemon, with automatic restart on unexpected exit.
- run, start, stop, reset, and delete services
- disable services temporarily (off) or permanently (offdel)
- query service PID, status info, and detailed dump
- list all registered services and their states

### Concepts
* A service is a Linux background process
* The service is usually an endless loop that runs and never exits
* If the service exits unexpectedly, the system will restart the service
* All APIs in the object can run in service mode. However, the API should not exit; otherwise, the system will rerun it frequently.


### API Reference

#### Control APIs

+ `run[ delay, name, com, op, ... ]` **register and start a service with optional delay**
    - delay ----------- [ string ], optional, delay in milliseconds before starting the service
    - name ------------ [ string ], the service name for identification
    - com ------------- [ string ], the component object name (e.g. ifname@wan3)
    - op -------------- [ string ], the method name to call on the component
    - ... ------------- [ string, ... ], additional parameters passed to the method
    - failed return tfalse
    - succeed return ttrue

    Example, run a service named mywan calling ifname@wan3.setup
    ```shell
    land@service.run[ mywan, ifname@wan3, setup ]
    ttrue
    ```

    Example, run a service with 5 second delay
    ```shell
    land@service.run[ 5000, mywan, ifname@wan3, setup ]
    ttrue
    ```

+ `start[ name, com, op, ... ]` **start a service (register if not exists)**
    - name ------------ [ string ], the service name
    - com ------------- [ string ], the component object name
    - op -------------- [ string ], the method name to call
    - ... ------------- [ string, ... ], additional parameters passed to the method
    - failed return tfalse
    - succeed return ttrue

    Example, start a service named mywan
    ```shell
    land@service.start[ mywan, ifname@wan3, setup ]
    ttrue
    ```

+ `stop[ name ]` **stop a running service**
    - name ------------ [ string ], the service name to stop
    - failed return tfalse, service not found
    - succeed return ttrue

    Example, stop the service named mywan
    ```shell
    land@service.stop[ mywan ]
    ttrue
    ```

+ `reset[ name, com, op, ... ]` **reset a service (restart with new parameters)**
    - name ------------ [ string ], the service name
    - com ------------- [ string ], the component object name
    - op -------------- [ string ], the method name to call
    - ... ------------- [ string, ... ], additional parameters passed to the method
    - failed return tfalse
    - succeed return ttrue

    Example, reset the service named mywan
    ```shell
    land@service.reset[ mywan, ifname@wan3, setup ]
    ttrue
    ```

+ `delete[ name ]` **delete a service (stop if running, then remove)**
    - name ------------ [ string ], the service name to delete
    - failed return tfalse, service not found
    - succeed return ttrue

    Example, delete the service named mywan
    ```shell
    land@service.delete[ mywan ]
    ttrue
    ```

+ `off[ name ]` **disable a service (stop and do not restart)**
    - name ------------ [ string ], the service name to disable
    - failed return tfalse, service not found
    - succeed return ttrue

    Example, disable the service named mywan
    ```shell
    land@service.off[ mywan ]
    ttrue
    ```

+ `offdel[ name ]` **disable and delete a service after it exits**
    - name ------------ [ string ], the service name
    - failed return tfalse, service not found
    - succeed return ttrue

    Example, disable and delete the service named mywan
    ```shell
    land@service.offdel[ mywan ]
    ttrue
    ```

#### Query APIs

+ `pid[ name ]` **get the PID of a running service**
    - name ------------ [ string ], the service name
    - failed return NULL, service not found or not running
    - succeed return [ number ], the process ID

    Example, get the PID of service mywan
    ```shell
    land@service.pid[ mywan ]
    1234
    ```

+ `info[ name ]` **get the status info of a service**
    - name ------------ [ string ], the service name
    - failed return NULL, service not found
    - succeed return [ json ], service status information
    ```json
    {
        "name": "service name",                // [ string ], the service name
        "pid": "process ID",                   // [ number ], present only when running
        "status": "service status",            // [ string ], current status
                                                   // "start": waiting to start
                                                   // "reset": waiting to restart
                                                   // "stop": waiting to stop
                                                   // "delete": waiting to be deleted
                                                   // "off": disabled
                                                   // "offdel": disabled, delete after exit
                                                   // "done": finished successfully
                                                   // "error": exited with error
        "delay": "startup delay in ms",        // [ number ], present only when delay is set
        "obj": "component object name",        // [ string ], the component object
        "op": "method name",                   // [ string ], present only when op is set
        "last_start": "last start timestamp",  // [ number ], present only when started before
        "fast_exited": "fast exit count",      // [ number ], present only when fast-exited before
        "last_kill": "last kill timestamp",    // [ number ], present only when killed before
        "force_killed": "force kill count",    // [ number ], present only when force-killed before
        "exit_code": "exit code",              // [ number or string ], present only when exited
        "exit_signal": "exit signal"           // [ number ], present only when killed by signal
    }
    ```

    Example, get info of service mywan
    ```shell
    land@service.info[ mywan ]
    {
        "name":"mywan",
        "pid":1234,
        "status":"start",
        "obj":"ifname@wan3",
        "op":"setup",
        "last_start":1705000000
    }
    ```

+ `dump[ name ]` **get detailed dump of a service including parameters**
    - name ------------ [ string ], the service name
    - failed return NULL, service not found
    - succeed return [ json ], detailed service information including all parameters

    Example, dump service mywan
    ```shell
    land@service.dump[ mywan ]
    {
        "name":"mywan",
        "pid":1234,
        "status":"start",
        "obj":"ifname@wan3",
        "op":"setup",
        "1":"param1",
        "2":"param2",
        "last_start":1705000000
    }
    ```

+ `list[]` **list all registered services and their states**
    - failed return NULL
    - succeed return [ json ], a map of service name to service state
    ```json
    {
        "service name":                        // [ string ]: { json }, service name
        {                                          // same structure as info return
            "name": "service name",            // [ string ]
            "pid": "process ID",               // [ number ], present only when running
            "status": "service status",        // [ string ]
            "obj": "component object name",    // [ string ]
            "op": "method name"                // [ string ], present only when op is set
        }
        // "...":{...}  How many services show how many properties
    }
    ```

    Example, list all services
    ```shell
    land@service.list
    {
        "mywan":
        {
            "name":"mywan",
            "pid":1234,
            "status":"start",
            "obj":"ifname@wan3",
            "op":"setup"
        },
        "myvpn":
        {
            "name":"myvpn",
            "pid":5678,
            "status":"start",
            "obj":"ifname@vpn",
            "op":"setup"
        }
    }
    ```
