## Management of component

A component can register an object name so that HE traffic for that object is handled by the chosen component implementation.

### **Configuration( `land@component` )**

The **saved configuration object** for `land@component` (query/set via `land@component`, `land@component:path`, merge `|{json}`, etc.).



`land@component` has **no** JSON configuration object. Mapping is done only through **`register`** / **`unregister`** in the **Component API**.

### **Component API**


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

### **C Code Example**

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

### **Lifecycle API**

+ `setup[]` — invoked from platform init to register built-in components.


### **Joint handlers**

**None** by default for this object (product builds may add more).


### **Published joint events**

**None** beyond what is documented above in the reference package.

