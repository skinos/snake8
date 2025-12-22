***

## Management of System Service   
Manage gateway system service task

#### The following describes service concepts   
* A service is a Linux background process
* The service is usually an endless loop that runs and never exits
* If the service exits unexpectedly, the system will restart the service
* All the API in the object can run in service mode. However, this API cannot be exited, Otherwise, the system will rerun the API frequently  


#### **API( land@service )**

+ `run[ [delay], service name, object, API, [parameter list,,,] ]` **add service**, register and start a service, update and reset it when already exists
    - delay ------------------ [ number ], delay some microsecond to start 
    - service name ----------- [ string ], service name, you can custom the name   
    - object ----------------- [ string ], component name 
    - API -------------------- [ string ], API of component  
    - parameter list --------- [ string ], parameter list  
    - failed return tfalse
    - succeed return ttrue

    Example, add wui@admin.service to run, named this service wuiserver
    ```shell
    land@service.run[ , wuiserver, wui@admin, service ]
    ttrue
    ```

    Example, add tui@ssh.service delay 5 second to run, named this service sshserver, pass the first argument is 192.168.8.1 and second argument is 23
    ```
    land@service.run[ 5000000, sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `delete[ service name ]` **stop and delete service**   
    - service name ----------- [ string ], service name, you can custom the name   
    - failed return tfalse
    - succeed return ttrue

    Example, stop and delete the service wuiserver
    ```shell
    land@service.delete[ wuiserver ]
    ttrue   
    ```

+ `start[ service name, [object], [API], [parameter list,,,] ]` **add service**, register and start a service, start the service when not running   
    - service name ----------- [ string ], service name, you can custom the name   
    - object ----------------- [ string ], component name 
    - API -------------------- [ string ], API of component  
    - parameter list --------- [ string ], parameter list  
    - failed return tfalse
    - succeed return ttrue

    Example, add wui@admin.service to run, named this service wuiserver
    ```shell
    land@service.start[ wuiserver, wui@admin, service ]
    ttrue
    ```

    Example, add tui@ssh.service to run, named this service sshserver, pass the first argument is 192.168.8.1 and second argument is 23
    ```
    land@service.start[ sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `stop[ service name ]` **stop service**   
    - service name ----------- [ string ], service name   
    - failed return tfalse
    - succeed return ttrue

    Example, stop the service wuiserver
    ```shell
    land@service.stop[ wuiserver ]
    ttrue   
    ```

+ `reset[ service name, [object], [API], [parameter list,,,] ]` **reset service**, register and start a service, reset it when the service already exists   
    - service name ----------- [ string ], service name, you can custom the name   
    - object ----------------- [ string ], component name 
    - API -------------------- [ string ], API of component  
    - parameter list --------- [ string ], parameter list  
    - failed return tfalse
    - succeed return ttrue

    Example, add wui@admin.service to run, named this service wuiserver
    ```shell
    land@service.reset[ wuiserver, wui@admin, service ]
    ttrue
    ```

    Example, add tui@ssh.service to run, named this service sshserver, pass the first argument is 192.168.8.1 and second argument is 23
    ```
    land@service.reset[ sshserver, tui@ssh, service, 192.168.8.1, 23 ]
    ttrue    
    ```

+ `off[ service name ]` **off control service**, then the system don't restart the service when the service exitd any way    
    - service name ----------- [ string ], service name   
    - failed return tfalse
    - succeed return ttrue

    Example, off control the service wuiserver
    ```shell
    land@service.off[ wuiserver ]
    ttrue   
    ```

+ `pid[ service name ]` **get the service pid**   
    - service name ----------- [ string ], service name   
    - failed retrun NULL
    - return string to describes the pid   

    Example, get service wuiserver pid
    ```shell
    land@service.pid[ wuiserver ]
    2041   
    ```

+ `info[ service name ]` **get the service infomation**   
    - service name ----------- [ string ], service name   
    - return json to describes the service infomation  
    
    ```json
    // Attributes introduction of json by the API return
    {
        "name":"service name",                                         // [ string ]
        "status":"service state",                                      // [ "start", "reset", "off", "stop", "unregister", "finish" ]
        "delay":"delay to run",                                        // [ number ], the unit in microsecond
        "obj":"object name",                                           // [ string ]
        "op":"API name",                                               // [ string ]
        "1":"first parameter",                                         // [ string or talk ]
        "2":"second parameter",                                        // [ string or talk ]
        "3":"third parameter",                                         // [ string or talk ]
        "pid":"pid number",                                            // [ number ]
        "last_start":"the system uptime when the service last start"   // [ number ]
    }    
    ```

    Example, get service wuiserver infomation
    ```shell
    land@service.info[ wuiserver ]
    {
        "name":"wuiserver",         # service name is wuiserver
        "delay":"0",                # no delay to run
        "obj":"wui@admin",          # object is wui@admin
        "op":"service",             # component API is service
        "1":"/tmp/webpage",         # the first parameter is /tmp/webpage
        "pid":"2041",               # service process id is 2041
        "last_start":"130"          # last start when system uptime 130 second
    }
    ```

+ `list[]` **list all the service**,   
    - return json to describes all the service infomation  

    ```json
    // Attributes introduction of json by the API return
    {
        "service name":
        {
            "name":"service name",                                         // [ string ]
            "status":"service state",                                      // [ "start", "reset", "off", "stop", "unregister", "finish" ]
            "delay":"delay to run",                                        // [ number ], the unit in microsecond
            "obj":"object name",                                           // [ string ]
            "op":"API name",                                               // [ string ]
            "pid":"pid number",                                            // [ number ]
            "last_start":"the system uptime when the service last start"   // [ number ]
        }
        // "...":{ service infomation }  // How many service show how many properties
    }    
    ```

    Example, list all service
    ```shell
    land@service.list
    {
        "clock@restart":
        {
            "name":"clock@restart",
            "delay":"0",
            "obj":"clock@restart",
            "op":"service",
            "pid":"1745",
            "last_start":"40"
        },
        "tui@telnet":
        {
            "name":"tui@telnet",
            "delay":"0",
            "obj":"tui@telnet",
            "op":"service",
            "pid":"1758",
            "last_start":"40"
        },
        "tui@ssh":
        {
            "name":"tui@ssh",
            "delay":"0",
            "obj":"tui@ssh",
            "op":"service",
            "pid":"1763",
            "last_start":"40"
        },
        "wui@admin":
        {
            "name":"wui@admin",
            "delay":"0",
            "obj":"wui@admin",
            "op":"service",
            "1":"/tmp/webpage",
            "pid":"2041",
            "last_start":"130"
        }
    }
    ```
