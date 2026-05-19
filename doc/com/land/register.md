## land@register — Register Variable Management

### Overview

Manage system register variables. Register variables are a system-wide key-value store shared across all components, used for runtime state, configuration snapshots, and inter-component data exchange.
- list all register variables for a component or globally
- read register values as integer, boolean, or string
- write register values as integer, boolean, or string
- dump binary register data in hexadecimal format



### API Reference

#### Query APIs

+ `list[ id ]` **list all register variables**
    - id -------------- [ string ], optional, the component object name; omit to list global register variables
    - failed return NULL
    - succeed return [ json ], a map of register variable name to value
    ```json
    {
        "variable name": "variable value",  // [ string ]: [ string ], register variable name and its value
        // "...":"..."  How many variables show how many properties
    }
    ```

    Example, list all global register variables
    ```shell
    land@register.list
    {
        "platform":"rk3568",
        "hardware":"R2000",
        "version":"8.0.0"
    }
    ```

    Example, list register variables for land@machine
    ```shell
    land@register.list[ land@machine ]
    {
        "wui_port":"80",
        "telnet_port":"23",
        "ssh_port":"22"
    }
    ```

+ `int[ id, name ]` **get an integer register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - failed return NULL
    - succeed return [ number ], the integer value

    Example, get the global rand value
    ```shell
    land@register.int[ rand ]
    12345
    ```

    Example, get the ill value from global register
    ```shell
    land@register.int[ ill ]
    0
    ```

+ `boole[ id, name ]` **get a boolean register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - failed return NULL
    - succeed return [ string ], "true" or "false"

    Example, get the block_restart value
    ```shell
    land@register.boole[ block_restart ]
    false
    ```

+ `string[ id, name ]` **get a string register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - failed return NULL
    - succeed return [ string ], the string value

    Example, get the platform value
    ```shell
    land@register.string[ platform ]
    rk3568
    ```

    Example, get the log_file value from global register
    ```shell
    land@register.string[ log_file ]
    /var/log/12345-syslog.log
    ```

+ `dump10[ id, name ]` **dump binary register data in hex, 10 bytes per line**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - failed return tfalse, variable not found
    - succeed return NULL, data printed to stdout

    Example, dump the log_mask register
    ```shell
    land@register.dump10[ log_mask ]
    ff ff ff ff ff ff ff ff ff ff
    ff ff ff ff ff ff ff ff ff ff
    ```

+ `dump100[ id, name ]` **dump binary register data in hex, 100 bytes per line**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - failed return tfalse, variable not found
    - succeed return NULL, data printed to stdout

    Example, dump the log_mask register with 100 bytes per line
    ```shell
    land@register.dump100[ log_mask ]
    ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ...
    ```

#### Control APIs

+ `set_int[ id, name, value ]` **set an integer register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - value ----------- [ string ], the integer value to set
    - failed return tfalse
    - succeed return ttrue

    Example, set the global rand value
    ```shell
    land@register.set_int[ rand, 99999 ]
    ttrue
    ```

+ `set_boole[ id, name, value ]` **set a boolean register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - value ----------- [ string ], "true" or "false"
    - failed return tfalse
    - succeed return ttrue

    Example, set block_restart to true
    ```shell
    land@register.set_boole[ block_restart, true ]
    ttrue
    ```

+ `set_string[ id, name, value ]` **set a string register value**
    - id -------------- [ string ], optional, the component object name; omit for global register
    - name ------------ [ string ], the register variable name
    - value ----------- [ string ], optional, the string value to set; omit or empty to clear
    - failed return tfalse
    - succeed return ttrue

    Example, set a string value
    ```shell
    land@register.set_string[ machine_state, restarting ]
    ttrue
    ```

    Example, clear a string value
    ```shell
    land@register.set_string[ machine_state ]
    ttrue
    ```
