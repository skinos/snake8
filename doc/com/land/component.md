## land@component — Component Registration

Every object name in the system must be backed by a component implementation.
`land@component` provides the API to bind an object name to a component at
runtime — useful when the mapping is not already established by an installed
FPK package.  It carries no JSON configuration of its own; all work is done
through the `register` / `unregister` calls below.

### Configuration ( `land@component` )

The **saved configuration object** for `land@component` (query/set via `land@component`, `land@component:path`, merge `|{json}`, etc.).


`land@component` has **no** JSON configuration object. Mapping is done only through **`register`** / **`unregister`** in the **Component API**.

### Component API


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

    **Implementation:** this API calls `com_register(object, origin, 0)`. With **`type == 0`**, **`origin`** is **not** treated as a raw filesystem path: it is a **key** resolved through the daemon **`COM_COM`** register map (`register_value_pointer`). The map must already contain a non-empty value (the real `.com` path). If the key is missing, registration fails with **`ENOENT`**. To register from an explicit on-disk file, **`land@fpk`** uses `com_register` with a non-zero **`type`**.

+ `unregister[ object ]` **delete a object**
    - object ------------------ [ string ]
    - failed return tfalse
    - succeed return ttrue

    Examples, delete a object
    ```shell
    land@component.unregister[ ifname@wan3 ]
    ttrue
    ```

### Lifecycle API

+ `setup[]` — invoked from platform init to register built-in components.


### C Code Example

**Call component methods**

```c
#include "skin/skin.h"

static void print_component_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `register[ object, component name ]`

```c
talk_t ret = scalls("land@component", "register", "ifname@wan3,ifname@ethcon");
if (ret != ttrue) print_component_call_error("register", ret);
```

##### `unregister[ object ]`

```c
talk_t ret = scalls("land@component", "unregister", "ifname@wan3");
if (ret != ttrue) print_component_call_error("unregister", ret);
```

