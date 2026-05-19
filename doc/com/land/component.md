## land@component — Component Registration

### Overview

Manage runtime component registration and unregistration. Every object name in the system must be backed by a component implementation. This component provides the API to bind or unbind an object name to a component at runtime.
- register an object name to a component implementation
- unregister an object name from the system



### API Reference

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
