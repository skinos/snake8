## land@joint — Joint Event Management

### Overview

Manage joint event tasks. Joint events are system-wide signals published by components when certain conditions occur. Each FPK can register handlers for joint events through its configuration; the system dispatches them when matching events occur.
- register and unregister joint event handlers at runtime (lost on reboot)
- add and delete persisted joint event handlers in configuration
- list registered joint event handlers by event name
- execute all handlers for a specified joint event



### Configuration reference ( land@joint )

```json
// Attributes introduction 
{
    "list":                                // [ json ], persisted joint event task list
    {
        "task name":                       // [ string ]: { json }, custom task name
        {                                      // task definition
            "level": "joint event name",       // [ string ], the joint event name to subscribe to
                                                   // e.g. "network/online", "network/offline",
                                                   // "machine/status", "network/onextern",
                                                   // "network/on", "storage/insert", etc.
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

Example, show all the joint configure
```shell
land@joint
{
    "list":
    {
        "online_talk_to_me":
        {
            "level":"network/online",          # trigger on network/online event
            "call":"tui@telnet.reset"          # call tui@telnet.reset
        },
        "offline_talk_to_me":
        {
            "level":"network/offline",         # trigger on network/offline event
            "call":"tui@ssh.shut"              # call tui@ssh.shut
        }
    },
    "remote":
    {
        "ip":"192.168.8.222",                  # remote notification IP
        "port":2230                            # remote notification port
    }
}
```

#### Configuration settings example

Example, set the remote notification IP
```shell
land@joint:remote/ip=192.168.1.50
ttrue
```

Example, merge set the remote notification configure( include "ip" "port" )
```shell
land@joint|{"remote":{"ip":"192.168.1.50","port":"2230"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the joint component**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Registers all tasks from configuration list into the runtime cache

#### Control APIs

+ `register[ event, call ]` **register a joint event handler at runtime, lost on reboot**
    - event ----------- [ string ], the joint event name to subscribe to
    - call ------------ [ string ], the component API or program to call
    - failed return tfalse
    - succeed return ttrue

    Example, register calling wui@admin.reset on network/online event
    ```shell
    land@joint.register[ network/online, wui@admin.reset ]
    ttrue
    ```

    Example, register calling tui@ssh.reset on network/up event
    ```shell
    land@joint.register[ network/up, tui@ssh.reset ]
    ttrue
    ```

+ `unregister[ event, call ]` **unregister a joint event handler from runtime**
    - event ----------- [ string ], the joint event name
    - call ------------ [ string ], the component API or program to remove
    - failed return tfalse
    - succeed return ttrue

    Example, unregister tui@ssh.reset from network/up event
    ```shell
    land@joint.unregister[ network/up, tui@ssh.reset ]
    ttrue
    ```

+ `add[ name, call, event ]` **add a persisted joint event handler to configuration**
    - name ------------ [ string ], custom task name
    - call ------------ [ string ], the component API or program to call
    - event ----------- [ string ], the joint event name to subscribe to
    - failed return tfalse
    - succeed return ttrue

    Example, add a task named webreset for network/online event
    ```shell
    land@joint.add[ webreset, wui@admin.reset, network/online ]
    ttrue
    ```

    Example, add a task named sshreset for network/up event
    ```shell
    land@joint.add[ sshreset, tui@ssh.reset, network/up ]
    ttrue
    ```

+ `delete[ name ]` **delete a persisted joint event handler from configuration**
    - name ------------ [ string ], the task name to delete
    - failed return tfalse
    - succeed return ttrue

    Example, delete task named webreset
    ```shell
    land@joint.delete[ webreset ]
    ttrue
    ```

#### Query APIs

+ `list[ [event] ]` **list registered joint event handlers**
    - event ----------- [ string ], optional, the joint event name to filter by; omit or empty to list all events
    - failed return NULL
    - succeed return [ json ], a map of joint event name to handler list
    ```json
    {
        "joint event name":                    // [ string ]: { json }, joint event name
        {                                          // handlers registered for this event
            "component API or program": "",    // [ string ]: [ string ], the call string, value is empty
            // "...":"..."  How many handlers at this event show how many properties
        }
        // "...":{...}  How many events show how many properties
    }
    ```

    Example, list all joint event handlers
    ```shell
    land@joint.list
    {
        "storage/insert":
        {
            "land@syslog.setup":""
        },
        "network/online":
        {
            "agent@heclient.setup":"",
            "client@dhcps.reset":""
        },
        "machine/status":
        {
            "agent@heclient.update":""
        },
        "network/onextern":
        {
            "arch@ethernet.hwnat":"",
            "forward@rule.on":"",
            "forward@nat.on":"",
            "forward@firewall.on":""
        }
    }
    ```

    Example, list handlers for network/online event
    ```shell
    land@joint.list[ network/online ]
    {
        "agent@heclient.setup":"",
        "client@dhcps.reset":""
    }
    ```
