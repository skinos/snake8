
***

## Management of Boot startup component
Administration of equipment initialize task
Each project can register its own startup task (either through the project production tool or directly by modifying the prj.json file), which requires the system to execute the task at specified boot level 

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

*It is not recommended for common application developers to register extern and previous launchers*


#### **Configuration( land@init )**

```json
// Attributes introduction 
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
                                                                // "delay"
                                                                // "delay2"
                                                                // "delay3"
                                                                // "delay4"
                                                                // "delay5"
        "call":"component API or program"           // [ string ]
    }
    // "...":{ ... }     How many startup task show how many properties
}
// Examples
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
}
```  



#### **API( land@init )**

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
    ```
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

+ `list[ [boot levels] ]` **list startup task**  
    - boot level ----------- [ string ], default list all registered startup task
    - return json to describes the boot task   
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
    ```
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


