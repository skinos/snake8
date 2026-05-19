## land@uninit — Shutdown Tasks

### Overview

Manage shutdown tasks. Each FPK can register shutdown tasks through its configuration; the system runs them at the configured shutdown level during shutdown.
- register and unregister shutdown tasks at runtime (lost on reboot)
- add and delete persisted shutdown tasks in configuration
- list registered shutdown tasks by level
- execute all tasks at a specified shutdown level

There are multiple shutdown levels at system shutdown, executed in order:
- `delay5` through `delay` — delayed shutdown (5 to 1 seconds)
- `general` — general application shutdown
- `app2` — secondary application shutdown
- `app` — application shutdown
- `extern` — internet connection shutdown
- `local` — local interface shutdown
- `manage` — management framework shutdown
- `network` — network subsystem shutdown
- `device` — device shutdown
- `bus` — bus subsystem shutdown
- `land` — core infrastructure shutdown
- `arch` — platform layer shutdown



### Configuration reference ( land@uninit )

```json
// Attributes introduction 
{
    "list":                                // [ json ], persisted shutdown task list
    {
        "task name":                       // [ string ]: { json }, custom task name
        {                                      // task definition
            "level": "shutdown level",         // [ string ], the shutdown level to run this task at
                                                   // "delay5", "delay4", "delay3", "delay2", "delay",
                                                   // "general", "app2", "app", "extern", "local",
                                                   // "manage", "network", "device", "bus", "land", "arch"
            "call": "component API or program" // [ string ], the component method or program to call
        }
        // "...":{...}  How many tasks show how many properties
    },
    "remote":                              // [ json ], optional UDP notification target
    {
        "ip": "remote IP address",         // [ string ], remote IP for UDP notification, empty means disabled
        "port": "UDP port"                 // [ number ], UDP port number, default be 515
    }
}
```

#### Configuration example

Example, show all the uninit configure
```shell
land@uninit
{
    "list":
    {
        "tuishutdown":
        {
            "level":"app",                     # shutdown at app level
            "call":"tui@telnet.shut"           # call tui@telnet.shut
        },
        "sshshutdown":
        {
            "level":"general",                 # shutdown at general level
            "call":"tui@ssh.shut"              # call tui@ssh.shut
        }
    }
}
```

#### Configuration settings example

Example, set the remote notification IP
```shell
land@uninit:remote/ip=192.168.1.50
ttrue
```

Example, merge set the remote notification configure( include "ip" "port" )
```shell
land@uninit|{"remote":{"ip":"192.168.1.50","port":"515"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the uninit component**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Registers all tasks from configuration list into the runtime cache

#### Control APIs

+ `register[ level, call ]` **register a shutdown task at runtime, lost on reboot**
    - level ----------- [ string ], the shutdown level, default be "general"
    - call ------------ [ string ], the component API or program to call
    - failed return tfalse
    - succeed return ttrue

    Example, register calling wui@admin.shut at the app shutdown level
    ```shell
    land@uninit.register[ app, wui@admin.shut ]
    ttrue
    ```

    Example, register calling tui@ssh.shut at the default general level
    ```shell
    land@uninit.register[ ,tui@ssh.shut ]
    ttrue
    ```

+ `unregister[ level, call ]` **unregister a shutdown task from runtime**
    - level ----------- [ string ], the shutdown level, default be "general"
    - call ------------ [ string ], the component API or program to remove
    - failed return tfalse
    - succeed return ttrue

    Example, unregister tui@ssh.shut at the app shutdown level
    ```shell
    land@uninit.unregister[ app, tui@ssh.shut ]
    ttrue
    ```

+ `add[ name, call, [level] ]` **add a persisted shutdown task to configuration**
    - name ------------ [ string ], custom task name
    - call ------------ [ string ], the component API or program to call
    - level ----------- [ string ], the shutdown level, default be "general"
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named webshut at the app level
    ```shell
    land@uninit.add[ webshut, wui@admin.shut, app ]
    ttrue
    ```

    Example, add a task named sshshut at the general level
    ```shell
    land@uninit.add[ sshshut, tui@ssh.shut ]
    ttrue
    ```

+ `delete[ name ]` **delete a persisted shutdown task from configuration**
    - name ------------ [ string ], the task name to delete
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named sshshut
    ```shell
    land@uninit.delete[ sshshut ]
    ttrue
    ```

#### Query APIs

+ `list[ [level] ]` **list registered shutdown tasks**
    - level ----------- [ string ], optional, the shutdown level to filter by; omit or empty to list all levels
    - failed return NULL
    - succeed return [ json ], a map of shutdown level to task list
    ```json
    {
        "shutdown level":                      // [ string ]: { json }, shutdown level name
        {                                          // tasks registered at this level
            "component API or program": "",    // [ string ]: [ string ], the call string, value is empty
            // "...":"..."  How many tasks at this level show how many properties
        }
        // "...":{...}  How many levels show how many properties
    }
    ```

    Example, list all shutdown tasks
    ```shell
    land@uninit.list
    {
        "app2":
        {
            "agent@local.shut":""
        },
        "app":
        {
            "client@station.shut":"",
            "clock@restart.shut":"",
            "forward@alg.shut":""
        },
        "manage":
        {
            "tui@telnet.shut":"",
            "tui@ssh.shut":""
        }
    }
    ```

    Example, list tasks at the land level
    ```shell
    land@uninit.list[ land ]
    {
        "land@auth.shut":"",
        "network@hosts.shut":""
    }
    ```
