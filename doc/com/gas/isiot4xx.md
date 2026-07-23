## gas@isiot4xx — ISIOT-4xx Modbus Gas Reader

### Overview

Read gas concentration data from the ISIOT-4xx four-in-one detector via Modbus RTU.
- Bound as uart driver `uartdrv@isiot4xx` and started by `uart@frame`
- Default gas types: O2, CO, H2S, CH4 (configurable via `modbus_reg0` ~ `modbus_reg3`)
- Periodic Modbus poll every `read_interval` milliseconds
- Query current values with `status` / `read` (uart object as argument)
    > Cloud reporting is handled by `gas@jrzh`, which calls these APIs



### Configuration reference ( gas@isiot4xx )

```json
// Attributes introduction 
{
    "modbus_addr": "1",                                        // [ number ], Modbus device address, default 1, range 1-253
    "read_interval": "100",                                    // [ number ], Modbus poll interval in milliseconds, default 100, minimum 1
    "modbus_timeout": "1",                                     // [ number ], Modbus response timeout in seconds, default 1, minimum 1
    "modbus_reg0": "O2",                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 0 gas type, default "O2"
    "modbus_reg1": "CO",                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 1 gas type, default "CO"
    "modbus_reg2": "H2S",                                      // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 2 gas type, default "H2S"
    "modbus_reg3": "CH4"                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 3 gas type, default "CH4"
}
```

#### Configuration example

Example, show all the isiot4xx configure under uart
```shell
uart@tty
{
    "drvcom":"uartdrv@isiot4xx",
    "isiot4xx":
    {
        "modbus_addr":"1",
        "read_interval":"100",
        "modbus_timeout":"1",
        "modbus_reg0":"O2",
        "modbus_reg1":"CO",
        "modbus_reg2":"H2S",
        "modbus_reg3":"CH4"
    }
}
```

#### Configuration settings example

Example, change the Modbus poll interval to 5 seconds (5000 ms)
```shell
uart@tty:isiot4xx|{"read_interval":"5000"}
ttrue
```

Example, change register 2 to benzene (C6H6)
```shell
uart@tty:isiot4xx|{"modbus_reg2":"C6H6"}
ttrue
```



### API Reference

#### Management APIs

+ `service` **start the isiot4xx Modbus reader**
    - failed return tfalse
    - succeed return tfalse
    - Lifecycle method started by uart framework (`uartdrv@isiot4xx`)
    - Opens Modbus RTU and enters the poll loop



#### Query APIs

+ `status[ <uart object> ]` **query last Modbus sample**
    - failed return NULL
    - succeed return
```json
{
    "last_read":"12345",                                       // [ number ], uptime of last successful read
    "O2":"20.90",                                              // [ string ], last value for that gas (%.2f)
    "CO":"0.00"                                                // [ string ], last value for that gas (%.2f)
    // "...":"..."  How many bound gases show how many value properties
}
```
    - Example
```shell
gas@isiot4xx.status[uart@tty]
{ ... }
```



#### Control APIs

+ `read[ <uart object> ]` **force one Modbus read and return values**
    - failed return tfalse
    - succeed return
```json
{
    "O2":"20.91",                                              // [ string ], gas value (%.2f)
    "CO":"0.00"                                                // [ string ], gas value (%.2f)
    // "...":"..."  How many bound gases show how many value properties
}
```
    - Example
```shell
gas@isiot4xx.read[uart@tty]
{ "O2":"20.91", "CO":"0.00", "H2S":"0.00", "CH4":"0.00" }
```
