## Management of Boot startup component
Administration of equipment initialize task
Each FPK can register startup tasks through its shipped manifest; the system runs them at the configured **init** level 

There are multiple boot levels at system startup:
    - `arch`       skinos arch setup
    - `land`       skinos land setup
    - `bus`        skinos bus setup
    - `device`     skinos device setup
    - `network`    skinos network setup
    - `manage`     skinos manage frame setup
    - `local`      local interface setup
    - `extern`     internet connect setup
    - `app`        app setup
    - `app2`       app2 setup
    - `general`    general app setup 
    - `delay`      app delay 1 second setup
    - `delay2`     app delay 2 second setup
    - `delay3`     app delay 3 second setup
    - `delay4`     app delay 4 second setup
    - `delay5`     app delay 5 second setup

*It is not recommended for common application developers to register extern and earlier boot levels*


### **Configuration( `land@init` )**

The **saved configuration object** for `land@init` (boot task **list** and optional **remote** notify target). `register` / `unregister` / `list` also use a per-object **cache file**; see the note below.

```json
// Attributes introduction 
{
    "list":
    {
        "task name":                             // [ string ], you can custom the name
        {
            "level":"boot levels",                          // [ string ], boot levels 
                                                                    // "arch"
                                                                    // "land"
                                                                    // "bus"
                                                                    // "device"
                                                                    // "network"
                                                                    // "manage"
                                                                    // "local"
                                                                    // "extern"
                                                                    // "app"
                                                                    // "app2"
                                                                    // "general"
                                                                    // "delay"
                                                                    // "delay2"
                                                                    // "delay3"
                                                                    // "delay4"
                                                                    // "delay5"
            "call":"component API or program"               // [ string ]
        }
        // "...":{ ... }     How many startup task show how many properties
    },
    "remote":                                          // optional; read at setup for UDP notify target used by call
    {
        "ip":"remote ip address",                      // [ string ], empty clears remote
        "port": "udp port"                             // [ number ], default 515 if missing or not positive
    }
}
// Examples
{
    "list":
    {
        "tuisetup":                               // call tui@telnet.setup at the app boot levels
        {
            "level":"app",
            "call":"tui@telnet.setup"
        },
        "webreset":                               // call wui@admin.reset at the general boot levels
        {
            "level":"general",
            "call":"wui@admin.reset"
        }
    },
    "remote":
    {
        "ip":"192.168.1.100",
        "port": "515"
    }
}
```  

Examples, merge **remote** and one **list** entry (only listed keys change)
```shell
land@init|{"remote":{"ip":"192.168.1.50","port":"515"}}
ttrue
```

`register` / `unregister` / `list` use the per-object **cache file** (same layout as the level map above). `add` / `delete` change the persisted **`list`** in configuration; those entries are applied into the cache when **setup** runs (normally at boot). Until then, `list` still reflects only what is already in the cache.


### **Component API**

+ `register[ [boot level], call ]` **register a startup task, lost when reboot**  
    - boot level ----------- [ string ], default be "general" 
    - call ----------------- [ string ], component API or program 
    - failed return tfalse
    - succeed return ttrue

    Example, register calling wui@admin.reset at the app boot level
    ```shell
    land@init.register[ app, wui@admin.reset ]
    ttrue
    ```
    Example, register calling tui@ssh.setup at the default(general) boot level
    ```shell
    land@init.register[ ,tui@ssh.setup ]
    ttrue    
    ```   

+ `unregister[ [boot level], call ]` **delete a startup task**  
    - boot level ----------- [ string ], default be "general"
    - call ----------------- [ string ], component API or program
    - failed return tfalse
    - succeed return ttrue

    Example, delete calling tui@ssh.setup at the app boot level
    ```shell
    land@init.unregister[ app, tui@ssh.setup ]
    ttrue
    ```

+ `list[ [boot level] ]` **list startup task**  
    - boot level ----------- [ string ], omit or empty to list all levels; otherwise return only that level
    - returns json describing the boot task(s)
    ```json
    // Attributes introduction of json by the method return
    {
        "boot level":                  // [ string ]:{},  boot levels 
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     How many calling at that level show how many properties
        }
        // "...":{ ... }     How many boot levels show how many properties
    }    
    ```  

    Example, show all the boot task
    ```shell
    land@init.list
    {
        "app2":                         // call agent@local.setup at boot level app2
        {
            "agent@local.setup":""
        },
        "app":                          // call client@station.setup/clock@restart.setup/forward@alg.setup at boot level app
        {
            "client@station.setup":"",  
            "clock@restart.setup":"",   
            "forward@alg.setup":""      
        },
        "manage":                       // call tui@telnet.setup and tui@ssh.setup at boot level manage
        {
            "tui@telnet.setup":"",      
            "tui@ssh.setup":""          
        }
        // ... more boot levels
    }
    ```

    Example, show the land boot level task
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

+ `add[ task name, call, [boot level] ]` **add a boot startup task**
    - task name ------------ [ string ], task name, you can custom the name
    - call ----------------- [ string ], component API or program
    - boot level ----------- [ string ], default be "general"
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named websetup, that calling wui@admin.setup at the app boot level
    ```shell
    land@init.add[ websetup, wui@admin.setup, app ]
    ttrue
    ```

    Example, add a task named sshsetup, that calling tui@ssh.setup at the general boot level
    ```shell
    land@init.add[ sshsetup, tui@ssh.setup ]
    ttrue    
    ```

+ `delete[ task name ]` **delete a startup task**
    - task name ---- [ string ], task name
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named sshsetup
    ```shell
    land@init.delete[ sshsetup ]
    ttrue
    ```

+ `call[ level, [parameter] ]` **execute all startup tasks at the specified boot level**, *succeed return ttrue, failed return tfalse, error return terror*
    - level ----------- [ string ], boot level to execute (e.g., "app", "general", "network")
    - parameter ------- [ talk_t ], optional parameter to pass to the tasks
    - This method is called by the system during boot process
    - It executes all registered tasks for the specified level and sends UDP notification if remote logging is configured
    - Not intended for manual invocation



### **Lifecycle API**


+ `setup[]` **initialize the init component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during startup
    - It registers all startup tasks from configuration and sets up remote logging if configured
    - Not intended for manual invocation

+ `shut[]` **shutdown the init component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during shutdown
    - Not intended for manual invocation

### **Joint handlers**

**None** by default for this object (product builds may add more).


### **Published joint events**

**None** beyond what is documented above in the reference package.


### **C Code Example**

```c
#include "skin/skin.h"

static void example_land_init(void)
{
    talk_t ret = scall("land@init", "setup", NULL);
    (void)ret;
}
```

