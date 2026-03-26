## Management of System Service   
Manage gateway system service task

### **Configuration( `land@service` )**

The **saved configuration object** for `land@service` (query/set via `land@service`, `land@service:path`, merge `|{json}`, etc.).



`land@service` has **no** separate JSON configuration tree. All behaviour is driven by the **Component API** below (`run`, `delete`, `list`, etc.).

#### The following describes service concepts   
* A service is a Linux background process
* The service is usually an endless loop that runs and never exits
* If the service exits unexpectedly, the system will restart the service
* All APIs in the object can run in service mode. However, the API should not exit; otherwise, the system will rerun it frequently.  



### **Component API**

+ `run[ [delay], service name, object, API, [parameter list,,,] ]` **add service**, register and start a service, update and reset it when already exists
    - delay ------------------ [ number ], delay before start (microseconds; e.g. 5000000 for 5 seconds)
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

+ `off[ service name ]` **off control service**, then the system don't restart the service when the service exited anyway    
    - service name ----------- [ string ], service name   
    - failed return tfalse
    - succeed return ttrue

    Example, off control the service wuiserver
    ```shell
    land@service.off[ wuiserver ]
    ttrue   
    ```

+ `offdel[ service name ]` **off control service and automatically delete this service after it exits**    
    - service name ----------- [ string ], service name   
    - failed return tfalse
    - succeed return ttrue

    Example, off control and automatically delete service `wuiserver` after it exits
    ```shell
    land@service.offdel[ wuiserver ]
    ttrue
    ```

+ `pid[ service name ]` **get the service pid**   
    - service name ----------- [ string ], service name   
    - failed return NULL
    - return a string that describes the PID   

    Example, get service wuiserver pid
    ```shell
    land@service.pid[ wuiserver ]
    2041   
    ```

+ `info[ service name ]` **get the service information**   
    - service name ----------- [ string ], service name   
    - return JSON that describes the service information  
    
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

    Example, get service `wuiserver` information
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

+ `dump[ service name ]` **get the service debug information**   
    - service name ----------- [ string ], service name   
    - return JSON that describes the internal debug information of the service

    Example, get service wuiserver debug information
    ```shell
    land@service.dump[ wuiserver ]
    {
        ...
    }
    ```

+ `list[]` **list all the service**,   
    - return JSON that describes all service information  

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
        // "...":{ service information }  // Shows one property per service
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

### **C Code Example**

**Call component methods**

```c
#include "skin/skin.h"

static void print_service_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `run[ [delay], service name, object, API, [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "run", "0,wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("run", ret);
```

##### `delete[ service name ]`

```c
talk_t ret = scalls("land@service", "delete", "wuiserver");
if (ret != ttrue) print_service_call_error("delete", ret);
```

##### `start[ service name, [object], [API], [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "start", "wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("start", ret);
```

##### `stop[ service name ]`

```c
talk_t ret = scalls("land@service", "stop", "wuiserver");
if (ret != ttrue) print_service_call_error("stop", ret);
```

##### `reset[ service name, [object], [API], [parameter list,,,] ]`

```c
talk_t ret = scalls("land@service", "reset", "wuiserver,wui@admin,service,/tmp/webpage");
if (ret != ttrue) print_service_call_error("reset", ret);
```

##### `off[ service name ]`

```c
talk_t ret = scalls("land@service", "off", "wuiserver");
if (ret != ttrue) print_service_call_error("off", ret);
```

##### `offdel[ service name ]`

```c
talk_t ret = scalls("land@service", "offdel", "wuiserver");
if (ret != ttrue) print_service_call_error("offdel", ret);
```

##### `pid[ service name ]`

```c
talk_t ret = scalls("land@service", "pid", "wuiserver");
if (ret > tpanic)
{
    printf("pid=%s\n", x2string(ret));
    talk_free(ret);
}
else print_service_call_error("pid", ret);
```

##### `info[ service name ]`

```c
talk_t ret = scalls("land@service", "info", "wuiserver");
if (ret > tpanic)
{
    printf("status=%s obj=%s op=%s\n",
           json_string(ret, "status"),
           json_string(ret, "obj"),
           json_string(ret, "op"));
    talk_free(ret);
}
else print_service_call_error("info", ret);
```

##### `dump[ service name ]`

```c
talk_t ret = scalls("land@service", "dump", "wuiserver");
if (ret > tpanic)
{
    printf("service dump json ready\n");
    talk_free(ret);
}
else print_service_call_error("dump", ret);
```

##### `list[]`

```c
talk_t ret = scall("land@service", "list", NULL);
if (ret > tpanic)
{
    printf("service list json ready\n");
    talk_free(ret);
}
else print_service_call_error("list", ret);
```

### **Lifecycle API**

+ `setup[]` / `shut[]` — **when implemented** for **`land@service`**, start/stop the component service or hooks. Scheduling follows the installed FPK **init** / **uninit** / **joint** manifest.
+


### **Joint handlers**

**None** by default for this object (product builds may add more).


### **Published joint events**

**None** beyond what is documented above in the reference package.

