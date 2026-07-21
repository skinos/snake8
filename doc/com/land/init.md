## land@init — Boot Startup Tasks

### Overview

Manage boot startup tasks. Each FPK can register startup tasks through its configuration; the system runs them at the configured boot level during startup.
- register and unregister startup tasks at runtime (lost on reboot)
- add and delete persisted startup tasks in configuration
- list registered startup tasks by level
- execute all tasks at a specified boot level

There are multiple boot levels at system startup, executed in order:
- `arch` — platform layer setup
- `land` — core infrastructure setup
- `bus` — bus subsystem setup
- `device` — device setup
- `network` — network subsystem setup
- `manage` — management framework setup
- `local` — local interface setup
- `extern` — internet connection setup
- `app` — application setup
- `app2` — secondary application setup
- `general` — general application setup
- `delay` through `delay5` — delayed setup (1 to 5 seconds)



### Configuration reference ( land@init )

```json
// Attributes introduction 
{
    "list":                                // [ json ], persisted startup task list
    {
        "task name":                       // [ string ]: { json }, custom task name
        {                                      // task definition
            "level": "boot level",             // [ string ], the boot level to run this task at
                                                   // "arch", "land", "bus", "device", "network",
                                                   // "manage", "local", "extern", "app", "app2",
                                                   // "general", "delay", "delay2", "delay3",
                                                   // "delay4", "delay5"
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

Example, show all the init configure
```shell
land@init
{
    "list":
    {
        "tuisetup":
        {
            "level":"app",                     # boot at app level
            "call":"tui@telnet.setup"          # call tui@telnet.setup
        },
        "webreset":
        {
            "level":"general",                 # boot at general level
            "call":"wui@admin.reset"           # call wui@admin.reset
        }
    },
    "remote":
    {
        "ip":"192.168.1.100",                  # remote notification IP
        "port":515                             # remote notification port
    }
}
```

#### Configuration settings example

Example, set the remote notification IP
```shell
land@init:remote/ip=192.168.1.50
ttrue
```

Example, merge set the remote notification configure( include "ip" "port" )
```shell
land@init|{"remote":{"ip":"192.168.1.50","port":"515"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the init component**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Registers all tasks from configuration list into the runtime cache

#### Control APIs

+ `register[ level, call ]` **register a startup task at runtime, lost on reboot**
    - level ----------- [ string ], the boot level, default be "general"
    - call ------------ [ string ], the component API or program to call
    - failed return tfalse
    - succeed return ttrue

    Example, register calling wui@admin.reset at the app boot level
    ```shell
    land@init.register[ app, wui@admin.reset ]
    ttrue
    ```

    Example, register calling tui@ssh.setup at the default general level
    ```shell
    land@init.register[ ,tui@ssh.setup ]
    ttrue
    ```

+ `unregister[ level, call ]` **unregister a startup task from runtime**
    - level ----------- [ string ], the boot level, default be "general"
    - call ------------ [ string ], the component API or program to remove
    - failed return tfalse
    - succeed return ttrue

    Example, unregister tui@ssh.setup at the app boot level
    ```shell
    land@init.unregister[ app, tui@ssh.setup ]
    ttrue
    ```

+ `add[ name, call, [level] ]` **add a persisted startup task to configuration**
    - name ------------ [ string ], custom task name
    - call ------------ [ string ], the component API or program to call
    - level ----------- [ string ], the boot level, default be "general"
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named websetup at the app level
    ```shell
    land@init.add[ websetup, wui@admin.setup, app ]
    ttrue
    ```

    Example, add a task named sshsetup at the general level
    ```shell
    land@init.add[ sshsetup, tui@ssh.setup ]
    ttrue
    ```

+ `delete[ name ]` **delete a persisted startup task from configuration**
    - name ------------ [ string ], the task name to delete
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named sshsetup
    ```shell
    land@init.delete[ sshsetup ]
    ttrue
    ```

+ `call[ level, parameter ]` **execute all startup tasks registered for a boot level**
    - level ----------- [ string ], boot level (e.g. "arch", "land", "app", "general")
    - parameter ------- [ json ], optional, passed as the second param to each task (joint-style payload)
    - failed return tfalse
    - succeed return ttrue
    - Invoked by the boot script (`land.init`) for each level; forks one child per task and runs the HE call string
    - Not intended for routine manual use; for debugging only

    Example, run all tasks at the app boot level
    ```shell
    land@init.call[ app ]
    ttrue
    ```

+ `knock[ project ]` **run a project's prj.json init section (hot install path)**
    - project --------- [ string ], project name (directory under the installed projects root)
    - failed return tfalse
    - succeed return ttrue
    - Reads that project's `prj.json` → `init` and invokes each `project@component.method` entry
    - Used by `land@fpk.install` after placing a package; empty `init` section still returns ttrue

    Example, apply init hooks from project wui after FPK install
    ```shell
    land@init.knock[ wui ]
    ttrue
    ```

#### Query APIs

+ `list[ [level] ]` **list registered startup tasks**
    - level ----------- [ string ], optional, the boot level to filter by; omit or empty to list all levels
    - failed return NULL
    - succeed return [ json ], a map of boot level to task list
    ```json
    {
        "boot level":                          // [ string ]: { json }, boot level name
        {                                          // tasks registered at this level
            "component API or program": "",    // [ string ]: [ string ], the call string, value is empty
            // "...":"..."  How many tasks at this level show how many properties
        }
        // "...":{...}  How many levels show how many properties
    }
    ```

    Example, list all startup tasks
    ```shell
    land@init.list
    {
        "app2":
        {
            "agent@local.setup":""
        },
        "app":
        {
            "client@station.setup":"",
            "clock@restart.setup":"",
            "forward@alg.setup":""
        },
        "manage":
        {
            "tui@telnet.setup":"",
            "tui@ssh.setup":""
        }
    }
    ```

    Example, list tasks at the land level
    ```shell
    land@init.list[ land ]
    {
        "land@auth.setup":"",
        "land@joint.setup":"",
        "land@init.setup":"",
        "land@uninit.setup":"",
        "network@hosts.setup":""
    }
    ```
