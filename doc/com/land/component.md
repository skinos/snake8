## land@component — Component Registration

### Overview

Manage runtime component registration, unregistration, and discovery. Every object name in the system must be backed by a component implementation. This component provides the API to bind or unbind an object name to a component at runtime, and to query what is registered.
- register an object name to a component implementation
- unregister an object name from the system
- list registered components (optional prefix / project filters)
- list object-alias placeholders and whether their targets resolve



### API Reference

#### Query APIs

+ `list[ prefix, project ]` **list registered components**
    - prefix ------------ [ string ], optional, object-name prefix filter (e.g. `"usbdrv"` matches `usbdrv@…`); omit or empty for all
    - project ----------- [ string ], optional, keep only entries whose implementation path belongs to this project id (e.g. `"land"`); omit or empty for all projects
    - failed return NULL
    - succeed return [ json ], map of object name to implementation path (unresolved aliases are hidden)
    ```json
    {
        "object name": "implementation path"  // [ string ]: [ string ], registered object → shared library or executable path
        // "...":"..."  How many components show how many properties
    }
    ```

    Example, list all registered components
    ```shell
    land@component.list
    {
        "land@machine":"/usr/share/skinos/land/machine/libmachine.so",
        "land@auth":"/usr/share/skinos/land/auth/libauth.so"
    }
    ```

    Example, list components under project land
    ```shell
    land@component.list[ ,land ]
    {
        "land@machine":"/usr/share/skinos/land/machine/libmachine.so",
        "land@auth":"/usr/share/skinos/land/auth/libauth.so"
    }
    ```

+ `alias[]` **list object-alias placeholders and resolve status**
    - failed return NULL
    - succeed return [ json ], each alias maps to its target component and whether that target currently resolves
    ```json
    {
        "alias name":                    // [ string ]: { json }, short object alias (e.g. "com", "log")
        {
            "com": "target object",      // [ string ], real component object the alias points to
            "exist": "resolve state"     // [ string ]: [ "true", "false" ], "true" if target is a LIB/EXE, "false" if unresolved
        }
        // "...":{ ... }  How many aliases show how many properties
    }
    ```

    Example, list all aliases
    ```shell
    land@component.alias
    {
        "com":
        {
            "com":"land@component",
            "exist":"true"
        },
        "log":
        {
            "com":"land@syslog",
            "exist":"true"
        }
    }
    ```


#### Control APIs

+ `register[ object, component ]` **register an object name to a component**
    - object ----------- [ string ], the object name to register in the system (e.g. ifname@wan3)
    - component -------- [ string ], the component name or path to bind to the object
    - failed return tfalse
    - succeed return ttrue

    Example, register object ifname@wan3 to component ifname@ethcon
    ```shell
    land@component.register[ ifname@wan3, ifname@ethcon ]
    ttrue
    ```

+ `unregister[ object ]` **unregister an object name from the system**
    - object ----------- [ string ], the object name to unregister
    - failed return tfalse
    - succeed return ttrue

    Example, unregister object ifname@wan3
    ```shell
    land@component.unregister[ ifname@wan3 ]
    ttrue
    ```
