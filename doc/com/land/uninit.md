
***

## Management of shutdown component
Administration of equipment shutdown task
Each project can register its own shutdown task (either through the project production tool or directly by modifying the prj.json file), which requires the system to execute the task at specified shutdown level 

There are multiple shutdown levels at system shutdown process:
    - `delay5`     delay shutdown
    - `delay4`     delay shutdown
    - `delay3`     delay shutdown
    - `delay2`     delay shutdown
    - `delay`      delay shutdown
    - `general`    general app shutdown 
    - `app2`       app2 shutdown
    - `app`        app shutdown
    - `extern`     internet connect shutdown
    - `local`      local interface shutdown
    - `manage`     skinos manage frame shutdown
    - `network`    skinos network shutdown
    - `device`     skinos device shutdown
    - `bus`        skinos bus shutdown
    - `land`       skinos land shutdown
    - `arch`       skinos arch shutdown

#### **Configuration( land@uninit )**

```json
// Attributes introduction 
{
    "task name":                             // [ string ], you can custom the name
    {
        "level":"shutdown levels",                          // [ string ], shutdown levels 
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
        "call":"specify component API or program"           // [ string ]
    }
    // "...":{ ... }     How many shutdown task show how many properties
}
// Examples
{
    "tuishutdown":                               // call tui@telnet.shut at the app shutdown levels
    {
        "level":"app",
        "call":"tui@telnet.shut"
    },
    "sshshutdown":                               // call tui@ssh.shut at the general shutdown levels
    {
        "level":"general",
        "call":"tui@ssh.shut"
    }
}
```  

#### **API( land@uninit )** 

+ `register[ [shutdown level], call ]` **register a shutdown task, lost when reboot**  
    - shutdown level ------- [ string ], default be "general" 
    - call ----------------- [ string ], component API or program  
    - failed return tfalse
    - succeed return ttrue

    Example, register calling wui@admin.shut at the app shutdown level
    ```shell
    land@uninit.register[ app, wui@admin.shut ]
    ttrue
    ```
    Example, register calling tui@ssh.shut at the default(general) shutdown level
    ```
    land@uninit.register[ ,tui@ssh.shut ]
    ttrue    
    ```   

+ `unregister[ [shutdown level], call ]` **delete a shutdown task**  
    - shutdown level ------- [ string ], default be "general"
    - call ----------------- [ string ], component API or program
    - failed return tfalse
    - succeed return ttrue

    Example, delete calling tui@ssh.shut at the app shutdown level
    ```shell
    land@uninit.unregister[ app, tui@ssh.shut ]
    ttrue
    ```

+ `list[ [shutdown levels] ]` **list shutdown task**  
    - shutdown level ----------- [ string ], default list all registered shutdown task
    - return json to describes the shutdown task   
    ```json
    // Attributes introduction of json by the method return
    {
        "shutdown level":                 // [ string ]:{},  shutdown levels 
        {
            "component API or program":"",
            "component API or program":""
            // "...":"..."     How many call at that level show how many properties
        }
        // "...":{ ... }     How many shutdown levels show how many properties
    }    
    ```  

    Example, show all the shutdown task
    ```shell
    land@uninit.list
    {
        "app2":                         // call agent@local.shut at shutdown level app2
        {
            "agent@local.shut":""
        },
        "app":                          // call client@station.shut/clock@restart.shut/forward@alg.shut at shutdown level app
        {
            "client@station.shut":"",  
            "clock@restart.shut":"",   
            "forward@alg.shut":""      
        },
        "manage":                       // call tui@telnet.shut and tui@ssh.shut at shutdown level manage
        {
            "tui@telnet.shut":"",      
            "tui@ssh.shut":""          
        }
        // ... more level
    }
    ```

    Example, show the land shutdown level task
    ```shell
    land@uninit.list[ land ]
    {
        "land@auth.shut":"",
        "network@hosts.shut":""
    }   
    ```



+ `add[ stask name, call, [shutdown level] ]` **add a shutdown task**
    - task name ------------ [ string ], task name, you can custom the name
    - call ------------------ [ string ], component API or program 
    - shutdown level -------- [ string ], default be "general"
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named webshut, that calling wui@admin.shut at the app shutdown level
    ```shell
    land@uninit.add[ webshut, wui@admin.shut, app ]
    ttrue
    ```

    Example, add a task named sshshut, that calling tui@ssh.shut at the general shutdown level
    ```
    land@uninit.add[ sshshut, tui@ssh.shut ]
    ttrue    
    ```

+ `delete[ task name ]` **delete a shutdown task**
    - task name ---- [ string ], task name
    - failed return tfalse
    - succeed return ttrue

    Example, delete webshut
    ```shell
    land@uninit.delete[ webshut ]
    ttrue
    ```


