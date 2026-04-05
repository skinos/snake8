## UART infrastructure

`uart@frame` is the **infrastructure** component for this package.
It registers UART logical objects (`uart@tty`, `uart@tty2`, …) with the Skin runtime, resolves `ttydev` / `devcom` / `drvcom`, and starts the selected driver executable(`uartdrv@dtu`, `uartdrv@tui`, …) for each enabled port.

It does **not** hold a large JSON configuration blob of its own, port settings live on each **`uart@tty*`** object.
Use the **Component API** below for registration helpers and DNS utilities.


### Component API

All calls below are issued on **`uart@frame`**

+ `register[ uart_object, [ttydev], [devcom], [drvcom] ]` **register a UART object and optional bindings**
    - uart_object ------------ [ string ], e.g. `uart@tty3`
    - ttydev ----------------- [ string ], optional, e.g. `/dev/ttyUSB8`
    - devcom ----------------- [ string ], optional UART device component, e.g. `usb@tty-2-32`
    - drvcom ----------------- [ string ], optional driver key, e.g. `uartdrv@dtu`
    - failed return **tfalse** (`errno` set, often `EINVAL`)
    - succeed return **ttrue**
    - Also calls `com_register( object, "uart@frame", 0 )`.

    Examples:
    ```shell
    uart@frame.register[ uart@tty3, /dev/ttyUSB8 ]
    ttrue
    ```
    ```shell
    uart@frame.register[ uart@tty-2-32, /dev/ttyUSB8, usb@tty-2-32, uartdrv@dtu ]
    ttrue
    ```

+ `unregister[ uart_object ]` **unregister a UART object**
    - uart_object ------------ [ string ]
    - failed return **tfalse**
    - succeed return **ttrue**

    Example:
    ```shell
    uart@frame.unregister[ uart@tty3 ]
    ttrue
    ```

+ `list[]` **list registered UART objects**
    - failed return **NULL**
    - succeed return **JSON**: per object, `ttydev`, `devcom`, `drvcom` (strings; may be inexistence in JSON)

    Example (shape):
    ```json
    {
        "uart@tty": 
        { 
            "ttydev":"/dev/ttyS0",
            "drvcom":"uartdrv@dtu"
        },
        "uart@2-3": 
        { 
            "ttydev":"/dev/ttyUSB8", 
            "devcom":"usb@tty-2-3",
            "drvcom":"uartdrv@dtu"
        }
    }
    ```

+ `add[ devcom ]` **start the UART port** whose saved `devcom` matches
    - devcom ----------------- [ string ], must match that port’s `devcom` register
    - finds the object and calls its **`setup`**
    - failed return **tfalse** if no match or setup fails
    - succeed return **ttrue**

+ `delete[ devcom ]` **stop the UART port** whose `devcom` matches
    - devcom ----------------- [ string ]
    - calls that object’s **`shut`**
    - failed return **tfalse** if no match or shut fails
    - succeed return **ttrue**

+ `domain2ip[ domain, [timeout_seconds] ]` **resolve a hostname to IPv4**
    - domain ----------------- [ string ]
    - timeout ---------------- [ string or number ], optional; default **10** seconds if omitted
    - succeed return **ttrue** and store result in register: `reg_set_string(this, domain, ip)`
    - failed return **terror** and `reg_set_string(this, domain, NULL)`

    Example:
    ```shell
    uart@frame.domain2ip[ www.example.com, 20 ]
    ttrue
    ```

### Lifecycle API

+ `setup[]`
    - **`uart@frame`**: walks all UART objects in this project’s config, `com_register`s each,
      calls each **`uart@tty*.setup`**. Intended for **`prj.json` `init`** (e.g. `uart@frame.setup`).
    - **`uart@tty*`** (per port): reads config; if `status` is `disable`, returns **tfalse**;
      resolves `ttydev` (direct or via `devcom`); resolves `drvcom`; calls into the driver
      **`service`** entry with the port name and config. GPIO `convert` may be applied.

+ `shut[]`
    - **`uart@frame`**: calls **`shut`** on every registered UART object, then `com_unregister`.
    - **`uart@tty*`**: tears down that port (`sdelete`).

### C code example

```c
#include "skin/skin.h"

static void print_frame_call_error(const char *api, talk_t ret)
{
	if (ret == tfalse || ret == terror || ret == tpanic)
	{
		printf("%s failed, errno=%d\n", api, errno);
	}
}
```

##### `register[ uart_object, ttydev, devcom, drvcom ]`

```c
talk_t ret = scalls("uart@frame", "register", "uart@tty3,/dev/ttyUSB8,usb@tty-2-3,uartdrv@dtu");
	if (ret != ttrue) print_frame_call_error("register", ret);
```

##### `list[]`

```c
talk_t ret = scall("uart@frame", "list", NULL);
if (ret > tpanic)
{
	/* inspect JSON */
	talk_free(ret);
}
else
	print_frame_call_error("list", ret);
```
