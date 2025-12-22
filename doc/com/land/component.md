***

## Management of component

A component can register an object, and the component can take over all operations on that object


#### **Methods( land@component )**


+ `register[ object, component name ]` **register a object**
    - object ------------------ [ string ]
    - component name ----------- [ string ]
    - failed return tfalse
    - succeed return ttrue

    Examples, register a object use ifname@ethcon
    ```shell
    land@component.register[ ifname@wan3, ifname@ethcon ]
    ttrue
    ```

+ `unregister[ object ]` **delete a object**
    - object ------------------ [ string ]
    - failed return tfalse
    - succeed return ttrue

    Examples, delete a object
    ```shell
    land@component.unregister[ ifname@wan3 ]
    ttrue
    ```


