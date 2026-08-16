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
    - delay ----------- [ string ], optional, delay in milliseconds before starting the service; omit with a leading empty slot (`run[,name,com,op]`)
    - name ------------ [ string ], the service name for identification
    - com ------------- [ string ], the component object name (e.g. ifname@wan3)
    - op -------------- [ string ], the method name to call on the component
    - ... ------------- [ string, ... ], additional parameters passed to the method
    - failed return tfalse
    - succeed return ttrue

    Example, run a service named mywan calling ifname@wan3.setup (no delay)
    ```shell
    land@service.run[,mywan,ifname@wan3,setup]
    ttrue
    ```

    Example, run a service with 5 second delay
    ```shell
    land@service.run[5000,mywan,ifname@wan3,setup]
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

+ `off[ name ]` **mark a service off (do not kill; do not restart after exit; keep record)**
    - name ------------ [ string ], the service name to disable
    - failed return tfalse, service not found
    - succeed return ttrue
    - note: the running process is not signaled; if it later exits, daemon will not restart it and keeps the registration with status `off`

    Example, disable the service named mywan
    ```shell
    land@service.off[ mywan ]
    ttrue
    ```

+ `offdel[ name ]` **mark a service offdel (do not kill; do not restart; delete record after exit)**
    - name ------------ [ string ], the service name
    - failed return tfalse, service not found
    - succeed return ttrue
    - note: same as `off` regarding the live process; when the process exits, daemon removes the registration

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
    - not found return NULL
    - map attach / path failure return terror
    - succeed return [ json ], RO `*.service` map node as-is, except `status` converted to a readable string
    - note: there is no top-level `name` field (the queried name is the argument); zero-valued map fields such as `"pid":0` may still appear. For a filtered view with `"name"` and params, use `dump`
    ```json
    {
        "pid": 0,                              // [ number ], map value (0 when not running)
        "status": "service status",            // [ string ], converted from SERVICE_ORDER_* int
                                                   // "start": under run supervision (pending start or already running)
                                                   // "reset": waiting to restart
                                                   // "stop": waiting to stop
                                                   // "delete": waiting to be deleted
                                                   // "off": disabled (keep registration)
                                                   // "offdel": disabled, delete registration after exit
                                                   // "done": finished successfully (do not restart)
                                                   // "error": exited with error (do not restart)
        "delay": 0,                            // [ number ], ms; may be 0
        "obj": "component object name",        // [ string ]
        "op": "method name",                   // [ string ], may be empty
        "last_start": 0,                       // [ number ], uptime; may be 0
        "fast_exited": 0,                      // [ number ], may be 0
        "last_kill": 0,                        // [ number ], may be 0
        "force_killed": 0,                     // [ number ], may be 0
        "exit_code": 0,                        // [ number ], raw map value (not dump's string form)
        "exit_signal": 0                       // [ number ], may be 0
    }
    ```

    Example, get info of service mywan
    ```shell
    land@service.info[ mywan ]
    {
        "pid":1234,
        "status":"start",
        "delay":0,
        "obj":"ifname@wan3",
        "op":"setup",
        "last_start":1705000000,
        "fast_exited":0,
        "last_kill":0,
        "force_killed":0,
        "exit_code":0,
        "exit_signal":0
    }
    ```

+ `dump[ name ]` **get detailed dump of a service including parameters**
    - name ------------ [ string ], the service name
    - not found / empty dump return NULL
    - daemon IPC failure return terror (or tpanic if daemon replies tpanic)
    - succeed return [ json ], filtered detail: includes `"name"`, omits zero/empty noise fields when unused, expands param slots `"1"`/`"2"`/…
    - note: dump keeps map `status` as int (`SERVICE_ORDER_*`); use `info`/`list` for readable status names

    Example, dump service mywan
    ```shell
    land@service.dump[ mywan ]
    {
        "name":"mywan",
        "pid":1234,
        "status":1,
        "obj":"ifname@wan3",
        "op":"setup",
        "1":"param1",
        "2":"param2",
        "last_start":1705000000
    }
    ```

+ `list[]` **list all registered services and their states**
    - map attach / path failure return terror
    - succeed return [ json ], map of **service name (object key)** → same value shape as `info` (no inner `"name"` field)
    ```json
    {
        "mywan":
        {
            "pid":1234,
            "status":"start",
            "obj":"ifname@wan3",
            "op":"setup"
        },
        "myvpn":
        {
            "pid":5678,
            "status":"start",
            "obj":"ifname@vpn",
            "op":"setup"
        }
    }
    ```

    Example, list all services
    ```shell
    land@service.list
    {
        "mywan":
        {
            "pid":1234,
            "status":"start",
            "delay":0,
            "obj":"ifname@wan3",
            "op":"setup",
            "last_start":1705000000,
            "fast_exited":0,
            "last_kill":0,
            "force_killed":0,
            "exit_code":0,
            "exit_signal":0
        }
    }
    ```
