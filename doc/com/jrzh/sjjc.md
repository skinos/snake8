## jrzh@sjjc — ISIOT-401 Driver (SuQian Protocol)

### Overview

Read gas concentration data from ISIOT-401 device via Modbus RTU and report to SuQian cloud platform via TCP socket with AES encryption.
- Bound as uart driver `uartdrv@sjjc` (sibling of `uartdrv@isiot401` Lanzhou protocol in the same project)
- Default gas types: O2, CO, H2S, CH4 (configurable via `modbus_reg0` ~ `modbus_reg3`)
- Optional register via `BALL_POINT_INFO` when `point_report=enable`; report via `BALL_MONITOR_REAL_DATA` every `report_interval`
- Manual read and report via Control APIs
    > Select this driver with uart `"drvcom":"uartdrv@sjjc"` and configure under the `sjjc` section
    > Real-time plaintext follows SuQian 布控球实时数据 (top-level collectTime + datas of deviceCode/monitorCode/value)
    > Data is encrypted with AES-128-CBC and base64 encoded before transmission


### Configuration reference ( jrzh@sjjc )

```json
// Attributes introduction
{
    "modbus_addr": "1",                                        // [ number ], Modbus device address, default 1, range 1-253
    "report_interval": "3000",                                 // [ number ], report interval in milliseconds, default 3000, minimum 100
    "modbus_timeout": "1",                                     // [ number ], Modbus response timeout in seconds, default 1, minimum 1
    "tcp_timeout": "10",                                       // [ number ], TCP connect+send+recv total timeout in seconds, default 10, minimum 1
    "ent_code": "enterprise code from cloud platform",         // [ string ], default "LZ7300300243"
    "device_code": "device identifier",                        // [ string ], default from MACHINE_COM macid
    "device_name": "device display name",                      // [ string ], default from MACHINE_COM name, fallback "ISIOT-401"
    "server": "120.195.56.46:8010",                            // [ string ], cloud platform server, http://host:port or host:port, default "120.195.56.46:8010"
    "aes_key": "f271379419e349ba",                             // [ string ], default "f271379419e349ba"
    "modbus_reg0": "O2",                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 0 gas type, default "O2"
    "modbus_reg1": "CO",                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 1 gas type, default "CO"
    "modbus_reg2": "H2S",                                      // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 2 gas type, default "H2S"
    "modbus_reg3": "CH4"                                       // [ "O2", "CO", "H2S", "CH4", "C6H6", "H2", ... ], register 3 gas type, default "CH4"
}
```

#### Supported gas types

| Gas Name | Factor | Divisor | Unit | Unit Code | hlimit | hhlimit |
|----------|--------|---------|------|-----------|--------|---------|
| O2 | 1 | 100 | %VOL | 3 | 19.5 (low) | 23.5 |
| CO | 2 | 1 | ppm | 2 | 20 | 50 |
| H2S | 3 | 10 | ppm | 2 | 10 | 20 |
| CH4 | 17 | 10 | %LEL | 1 | 20 | 50 |
| C6H6 | 20 | 10 | ppm | 2 | 10 | 15 |
| H2 | 4 | 10 | ppm | 2 | 10 | 15 |

To add new gas types, edit the `gas_defs` table in `sjjc.c`.

#### Configuration example

Example, show uart configure using sjjc section
```shell
uart@tty
{
    "drvcom":"uartdrv@sjjc",
    "sjjc":
    {
        "modbus_addr":"1",                                     # Modbus device address
        "report_interval":"3000",                              # report every 3000 milliseconds
        "modbus_timeout":"1",                                  # Modbus response timeout in seconds
        "tcp_timeout":"10",                                    # TCP timeout in seconds
        "ent_code":"LZ7300300243",                             # enterprise code
        "device_code":"00037F124020",                          # device identifier
        "device_name":"D218-124020",                           # device display name
        "server":"120.195.56.46:8010",                         # cloud platform server
        "aes_key":"f271379419e349ba",                          # AES encryption key
        "modbus_reg0":"O2",                                    # register 0: O2 (default)
        "modbus_reg1":"CO",                                    # register 1: CO (default)
        "modbus_reg2":"H2S",                                   # register 2: H2S (default)
        "modbus_reg3":"CH4"                                    # register 3: CH4 (default)
    }
}
```

Example, change register 2 to benzene (C6H6) and register 3 to hydrogen (H2)
```shell
uart@tty
{
    "drvcom":"uartdrv@sjjc",
    "sjjc":
    {
        "modbus_reg2":"C6H6",                                  # register 2: benzene
        "modbus_reg3":"H2"                                     # register 3: hydrogen
    }
}
```

#### Configuration settings example

Example, change the report interval to 500 milliseconds
```shell
uart@tty:sjjc|{"report_interval":"500"}
ttrue
```

Example, change the TCP timeout to 15 seconds
```shell
uart@tty:sjjc|{"tcp_timeout":"15"}
ttrue
```

Example, change the Modbus timeout to 2 seconds
```shell
uart@tty:sjjc|{"modbus_timeout":"2"}
ttrue
```

Example, change the Modbus address to 2
```shell
uart@tty:sjjc|{"modbus_addr":"2"}
ttrue
```

Example, merge set the device code and device name ( include "device_code" "device_name" )
```shell
uart@tty:sjjc|{"device_code":"GAS-002","device_name":"Warehouse gas detector"}
ttrue
```



### API Reference

#### Management APIs

+ `service` **start the sjjc gas detector service**
    - failed return tfalse
    - succeed return tfalse
    - This is a lifecycle method called automatically by the uart framework during startup
    - Opens Modbus RTU connection, registers device info, and enters event loop


#### Query APIs

+ `status` **query the running status of the sjjc service**
    - failed return NULL
    - succeed return [ json ], current status and gas concentrations
    ```json
    {
        "modbus_addr": "1",              // [ number ], Modbus device address
        "report_interval": "3000",       // [ number ], report interval in milliseconds
        "last_read": "12345",            // [ number ], uptime_int() of last successful read
        "registered": "yes",             // [ string ], device info registered: "yes" or "no"
        "modbus_reg0": "O2",             // [ string ], register 0 gas type
        "modbus_reg1": "CO",             // [ string ], register 1 gas type
        "modbus_reg2": "H2S",            // [ string ], register 2 gas type
        "modbus_reg3": "CH4",            // [ string ], register 3 gas type
        "O2": "20.50",                   // [ number ], oxygen concentration (%VOL)
        "CO": "5",                       // [ number ], carbon monoxide concentration (ppm)
        "H2S": "0.3",                    // [ number ], hydrogen sulfide concentration (ppm)
        "CH4": "1.2"                     // [ number ], methane concentration (%LEL)
    }
    ```

    Example, check the service status
    ```shell
    jrzh@sjjc.status[uart@tty]
    {
        "modbus_addr":"1",
        "report_interval":"3000",
        "last_read":"12345",
        "registered":"yes",
        "modbus_reg0":"O2",
        "modbus_reg1":"CO",
        "modbus_reg2":"C6H6",
        "modbus_reg3":"H2",
        "O2":"20.50",
        "CO":"5",
        "C6H6":"0.3",
        "H2":"1.2"
    }
    ```

+ `read` **read gas data from device immediately**
    - failed return tfalse
    - succeed return [ json ], gas concentrations (gas names depend on modbus_regX configuration)
    ```json
    {
        "O2": "20.50",                   // [ number ], oxygen concentration (%VOL)
        "CO": "5",                       // [ number ], carbon monoxide concentration (ppm)
        "H2S": "0.3",                    // [ number ], hydrogen sulfide concentration (ppm)
        "CH4": "1.2"                     // [ number ], methane concentration (%LEL)
    }
    ```

    Example, read gas data now
    ```shell
    jrzh@sjjc.read[uart@tty]
    {
        "O2":"20.50",
        "CO":"5",
        "H2S":"0.3",
        "CH4":"1.2"
    }
    ```


#### Control APIs

+ `report` **read and report gas data immediately**
    - failed return tfalse
    - succeed return ttrue

    Example, read and report gas data now
    ```shell
    jrzh@sjjc.report[uart@tty]
    ttrue
    ```

+ `register` **register device info to cloud platform**
    - failed return tfalse
    - succeed return ttrue

    Example, register device info now
    ```shell
    jrzh@sjjc.register[uart@tty]
    ttrue
    ```



### Other

**Cloud Platform Data Interaction**

Connect to `server` via TCP. The `data` field is JSON encrypted with AES-128-CBC (IV same as key) then base64 encoded. Each packet ends with `@@`.

**Step 1: Register device (`BALL_POINT_INFO`)**

Send:
```json
{
    "companyCode": "LZ7300300243",
    "serviceId": "BALL_POINT_INFO",
    "dataId": "1717382400000",
    "data": "<AES encrypted then base64 encoded>"
}@@
```

Plaintext inside `data` (before encryption):
```json
{
    "companyCode": "LZ7300300243",
    "datas": [{
        "deviceCode": "00037F124020",
        "deviceName": "D218-124020",
        "deleted": "0",
        "createDate": "20250603120000",
        "createBy": "D218-124020",
        "updateDate": "20250603120000",
        "updateBy": "D218-124020",
        "entCode": "LZ7300300243",
        "list": [
            {"factor": "1",  "monitorCode": "1",  "hlimit": 19.50, "hhlimit": 23.50, "unit": "3"},
            {"factor": "2",  "monitorCode": "2",  "hlimit": 20.00, "hhlimit": 50.00, "unit": "2"},
            {"factor": "3",  "monitorCode": "3",  "hlimit": 10.00, "hhlimit": 20.00, "unit": "2"},
            {"factor": "17", "monitorCode": "17", "hlimit": 20.00, "hhlimit": 50.00, "unit": "1"}
        ]
    }]
}
```

Response success:
```json
{"code":200,"dataId":"1717382400000","message":"数据接收成功"}@@
```

Response failure:
```json
{"code":500,"dataId":"1717382400000","message":"未授权的访问"}@@
```

**Step 2: Report real-time data (`BALL_MONITOR_REAL_DATA`)**

Read 4 gas values via Modbus, then send:
```json
{
    "companyCode": "LZ7300300243",
    "serviceId": "BALL_MONITOR_REAL_DATA",
    "dataId": "1717382430000",
    "data": "<AES encrypted then base64 encoded>"
}@@
```

Plaintext inside `data` (before encryption, SuQian 2.1.5):
```json
{
    "collectTime": "20250603120030",
    "datas": [
        {"deviceCode": "00037F124020", "monitorCode": "1",  "value": 20.50},
        {"deviceCode": "00037F124020", "monitorCode": "2",  "value": 5.00},
        {"deviceCode": "00037F124020", "monitorCode": "3",  "value": 0.30},
        {"deviceCode": "00037F124020", "monitorCode": "17", "value": 1.20}
    ]
}
```

Response success:
```json
{"code":200,"dataId":"1717382400000","message":"数据接收成功"}@@
```

Response failure:
```json
{"code":500,"dataId":"1717382400000","message":"未授权的访问"}@@
```

**Modbus RTU Register Map (default configuration)**

| Register | Address | Default Gas | Type | Formula |
|----------|---------|-------------|------|---------|
| 0x0000 | 0 | O2 | UINT16 | value = raw / 100 |
| 0x0001 | 1 | CO | UINT16 | value = raw / 1 |
| 0x0002 | 2 | H2S | UINT16 | value = raw / 10 |
| 0x0003 | 3 | CH4 | UINT16 | value = raw / 10 |

Register gas types can be changed via `modbus_reg0` ~ `modbus_reg3` configuration. See "Supported gas types" table above.
