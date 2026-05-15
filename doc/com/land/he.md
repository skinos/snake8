
## Execute the HE command on the terminal to manage the gateway   
The HE command line is used to query and change gateway configuration, call component methods, and perform operations such as restart or reset. The same HE grammar is also available through other control protocols besides the terminal.

> **Start here**
> - **Prompt is `$ `?** You are in **[eline](eline.md)** — type HE **directly** (no `he`). Use **`ashy`** to open a shell, then run **`he '…'`**.
> - **Prompt is `# `?** Classic HE loop on some images — type HE directly, or use **`elf`** (if your banner shows it) to reach **`~ #`** shell.
> - **Already at `~ #`?** Every HE line must be **`he '<full line>'`** (one quoted argument). Details in [Linux shell and `he`](#enter-the-linux-shell-and-run-he-with-the-he-prefix).
> - **Interactive front-end:** **[eline.md](eline.md)** covers `set`, passthrough commands, `exit`, and Ctrl+D.

### What you see after login (Telnet, SSH, or serial Command Line)

Many products run **eline** as the login shell. You get a **`$ `** prompt with GNU readline (history and line editing). There you type **HE commands directly**—**no `he` prefix**—and may use **`set`**, **`ashy`**, **`exit`**, and passthrough lines as described in **[eline.md](eline.md)**.

To work in a normal **Linux shell** (`ash`), type **`ashy`** at the **`$ `** prompt. Eline is replaced by **`/bin/ash --login`** (the prompt is often **`~ #`** for root). **Inside that shell, every HE command must be run through the `he` utility**, for example **`he 'land@machine'`** or **`he 'land@machine:name=DemoGateway'`**. Wrap the HE text in **single quotes** whenever it contains characters the shell would treat specially (`|`, `{`, `}`, `[`, `]`, `:`, `=`, `,`, spaces inside JSON, and so on).

Some firmware still uses the classic **HE loop** with a **`# `** prompt (type HE directly, same grammar as eline). The welcome banner text is similar in both cases.

You can reach the gateway over:   
- **Telnet**   
- **SSH**   
- **TTL UART** (baud rate is usually 57600 / 8 data bits / 1 stop bit / no flow control / no parity)   
- **RS232 UART** (same typical serial parameters)   
- **RS485 UART** (same typical serial parameters)   

### Telnet client
1. Open **Telnet Server** under **&lt;System&gt;** on the web UI.
2. Log in with your user and password. If you land in **eline** (`$ `), use it as above; use **`ashy`** when you need **`he`** in shell.

### SSH client
1. Open **SSH Server** under **&lt;System&gt;**.
2. Log in; then follow the **eline / `ashy` / `he`** flow above.

### TTL / RS232 / RS485
1. Under **&lt;Application&gt;**, set **Serial#TTL** / **Serial#RS232** / **Serial#485** or **UART** / **UART2** / **UART3** to **Command Line**.
2. Connect with a serial terminal and log in; same **eline / `ashy` / `he`** rules apply.

After login, a short ASCII banner may appear, for example:   
```
               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
               EEEEEEEEE   LL           FFFFFFFFF
               EE          LL           FF
               EE          LL           FF
        Ashy   EEEEEEEEE   LLLLLLLLLL   FF

 -----------------------------------------------------------
 Command Help
 -----------------------------------------------------------
 @ ----------------------- List all the component
 <com> ------------------- Show component configure
 <com>. ------------------ List all component interface
 <com>:<config> ---------- Get component configure attribute
 <com>:<config>=<value> -- Set component configure attribute
 -----------------------------------------------------------
```   
The active prompt is **`$ `** (eline), **`# `** (classic HE loop), or **`~ #`** (Linux shell after **`ashy`**), depending on image and step—use the prompt your session actually shows.

---

## HE Command Format

### Quick start (30 seconds)

The lines below are what you type in **Linux shell** (`~ #` after **`ashy`**). In **eline** (`$ `), type the same **payload** only—omit **`he`** and the surrounding quotes—see **[eline.md](eline.md)**.

```shell
~ # he 'land@machine'                         # 1) query full component configuration
~ # he 'land@machine:name'                    # 2) query one attribute
~ # he 'land@machine:name=DemoGateway'      # 3) modify one attribute
~ # he 'land@machine.status'                  # 4) call a method
~ # he 'land@machine.status:version'          # 5) one field from method JSON
```

### How the `he` program joins arguments

The implementation builds one string by **concatenating every argument after `he` with no space inserted**. Examples:

- `he 'land@machine:name'` → one argument → correct.
- `he land@machine:name` → one token (no spaces) → correct.
- `he land@machine name` → **wrong**: becomes `land@machinename`.

So in the shell, **always** pass the full HE line as **one** quoted string whenever there is a space, or whenever you are unsure. That also stops the shell from interpreting `|`, `=`, `;`, etc.

If the **first character** of that combined string is **`+`**, **`=`**, or **`-`**, it selects a special mode (loop / parse-only / silent), not a normal component command—see **Advanced `he` prefixes** under [Enter the Linux shell and run HE](#enter-the-linux-shell-and-run-he-with-the-he-prefix).

| Usage | Format | Brief description | Example in Linux shell (`he '…'`) |
|---|---|---|---|
| Query all component configuration | `component` | Returns the full configuration object of the component. | `he 'land@machine'` |
| Query one configuration attribute | `component:attr/path` | Returns a string or JSON value at the specified attribute path. | `he 'land@machine:name'` |
| Set full component configuration | `component={json}` | Replaces the component configuration with the provided JSON object. | `he 'forward@alg={"ftp":"enable"}'` |
| Set one configuration attribute | `component:attr/path=value` | Sets one attribute value. Supports string or JSON value. | `he 'gnss@nmea:client/server=192.168.8.251'` |
| Merge multiple attributes | `component:attr/path\|{json}` or `component\|{json}` | Updates only specified fields and keeps other fields unchanged. | `he 'gnss@nmea:client|{"status":"disable","proto":"tcp"}'` |
| Clear one configuration attribute | `component:attr/path=` | Clears/removes the specified attribute value. | `he 'gnss@nmea:client='` |
| Call method without parameters | `component.method` | Calls a component API method with no parameters. | `he 'client@station.list'` |
| Call method with parameters | `component.method[param1,param2,...]` | Calls a component API method with one or more parameters. | `he 'clock@date.ntpsync[ntp1.aliyun.com]'` |
| Return one field from method JSON | `component.method:attr/path` | Calls method and returns only one field from the JSON result. | `he 'ifname@lte.status:ip'` |

### How to choose a format quickly

| Goal | Recommended format |
|---|---|
| Read all settings | `component` |
| Read one setting | `component:attr/path` |
| Replace full config object | `component={json}` |
| Update several fields only | `component\|{json}` or `component:attr/path\|{json}` |
| Set one field | `component:attr/path=value` |
| Clear one field | `component:attr/path=` |
| Call an API | `component.method[...]` |
| Call API and return only one field | `component.method:attr/path` |

### HE command types and return values

Each HE command takes one line and ends with Enter. Commands fall into three types:

| Type | Purpose |
|------|---------|
| **Query configuration** | Read the current configuration of a component |
| **Modify configuration** | Change the configuration of a component |
| **Call component method** | Invoke a component API (query status, start, stop, etc.) |

Return values:

| Return | Meaning |
|--------|---------|
| **String** | A single text value returned by the method |
| **`ttrue`** | Operation succeeded |
| **`tfalse`** | Operation failed |
| **`{JSON}`** | A JSON object with complex information |
| **Empty** | No data available (method returned NULL) |

After each output, the next prompt appears: **`~ #`** in Linux shell, **`$ `** in eline, or **`# `** in the classic HE loop.

### Query configuration

+ When querying all configurations of the components of the gateway, enter **component name** and press Enter. Usually the return value will be a JSON.

    **Example: query full component configuration**
    ```shell
    ~ # he 'land@machine'                    # Query configurations of land@machine (Basic Information of Gateway)
    {                                 # Return a complete JSON object
        "mode":"nmisp",                              # gateway operator mode Next Mobile(LTE/NR) Internet Service Provider connection
        "name":"ASHYELF-12AAD0",                     # gateway name is ASHYELF-12AAD0
        "mac":"00:03:7F:12:AA:D0",                   # gateway MAC is 88:12:4E:20:46:B0
        "macid":"00037F12AAD0",                      # gateway serial id is 88124E2046B0
        "language":"cn",                             # gateway language is chinese
        "cfgversion":"1"                             # gateway configure version is 1(hint modify one times)
    }
    ~ #
    ```   

+ When querying the attributes specified by the component, give the **component name:attribute path** and press Enter. The return value can be a string or a JSON, depending on the value of the corresponding attribute.

    **Example: query one attribute**
    ```shell
    ~ # he 'land@machine:name'                    # Query the value of the name attribute under the land@machine component configuration
    A218-120108                      # Return a string
    ~ #
    ```   
+ When a component has many levels of attributes, **attribute path** separates the multiple layers of **attribute names** with a **/**. The attribute path is used to locate the attributes.

    **Example: query nested attributes with path**
    ```shell
    ~ # he 'ifname@lan:static'                    # Query the value of the static attribute under the ifname@lan(LAN Network Management) component configuration
    {
        "ip":"192.168.8.1",
        "mask":"255.255.255.0"
    }
    ~ # he 'ifname@lan:static/ip'                    # Query the value of the ip attribute under the static attribute of the ifname@lan(LAN Network Management) component configuration
    192.168.8.1
    ~ #
    ```   

    The path format is **attribute/sub-attribute/sub-sub-attribute/...** to locate deeply nested attributes.

### Modify configuration

+ Modifying the component configuration is similar to querying the component configuration, first giving the **component name**, then giving the **attribute path** to modify the attribute, and finally giving the **value**.

    When modifying the value of the entire component:
    ```shell
    component name=value
    ```
    When modifying the value of an entire component, usually the value must be a JSON (starts with `{` and ends with `}`).
    **ttrue** is returned if the modification succeeds, **tfalse** is returned if the modification fails.

    **Example: replace full component configuration**
    ```shell
    ~ # he 'forward@alg'                    # Query the ALG configuration
    {
        "amanda":"disable",
        "ftp":"enable",
        "h323":"disable",
        "irc":"disable",
        "pptp":"enable",
        "gre":"enable",
        "rtsp":"enable",
        "sip":"enable",
        "snmp":"disable",
        "tftp":"disable",
        "udplite":"disable"
    }
    ~ #
    ~ # he 'forward@alg={"amanda":"enable","ftp":"enable","h323":"enable","irc":"enable","pptp":"enable","gre":"enable","rtsp":"enable","sip":"enable","snmp":"enable","tftp":"enable","udplite":"enable"}'                    # Modify the entire ALG configuration 
    ttrue                          # ttrue is returned, the change is successful
    ~ # he 'forward@alg'                    # After the configuration is modified, check the configuration again
    {
        "amanda":"enable",
        "ftp":"enable",
        "h323":"enable",
        "irc":"enable",
        "pptp":"enable",
        "gre":"enable",
        "rtsp":"enable",
        "sip":"enable",
        "snmp":"enable",
        "tftp":"enable",
        "udplite":"enable"
    }
    ~ #
    ```   

+ When modifying a component specified attribute:
    ```shell
    component name:attribute path=value
    ```
    The value can be a JSON (starting with `{` and ending with `}`) or a string.
    **ttrue** is returned if the modification succeeds, **tfalse** is returned if the modification fails.

    **Example: modify an attribute with JSON value**
    ```shell
    ~ # he 'gnss@nmea'                    # Query the GNSS configuration
    {
        "status":"enable",
        "client":
        {
            "status":"disable",
            "proto":"tcp",
            "server":"",
            "port":"9000",
            "id":"",
            "user":"",
            "vcode":"",
            "login":"",
            "login_string":"",
            "frame_start":"disable",
            "frame_string_string":"",
            "frame_end":"disable",
            "frame_end_string":""
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ # he 'gnss@nmea:client'                    # Query the client attribute in GNSS configuration
    {
        "status":"disable",
        "proto":"tcp",
        "server":"",
        "port":"9000",
        "id":"",
        "user":"",
        "vcode":"",
        "login":"",
        "login_string":"",
        "frame_start":"disable",
        "frame_string_string":"",
        "frame_end":"disable",
        "frame_end_string":""
    }
	~ # he 'gnss@nmea:client={"status":"enable","server":"192.168.8.250","port":"8000","interval":"30","id":"2232"}'                    # Modify the client attribute in gnss@nmea(GNSS configuration)
	ttrue
	~ # he 'gnss@nmea:client'                    # Query the result of modifying the client attributes in gnss@nmea(GNSS configuration)
    {
        "status":"enable",
        "server":"192.168.8.250",
        "port":"8000",
        "interval":"30",
        "id":"2232"
    }
	~ #
    ```

    **Example: modify a nested attribute with string value**
    ```shell
	~ # he 'gnss@nmea:client/server=192.168.8.251'                    # Change the value of the server attribute under the gnss@nmea(GPS management)client attribute
	ttrue
	~ # he 'gnss@nmea:client'                    # After the configuration is modified, check the configuration again
	{
	    "status":"enable",
	    "server":"192.168.8.251",
	    "port":"8000",
	    "interval":"30",
	    "id":"2232"
	}
	~ #
    ```

    **Example: verify the change in full configuration**
    ```shell
	~ # he 'gnss@nmea'                    # Query the all of GNSS configuration
    {
        "status":"enable",
        "client":
        {
            "status":"enable",
            "server":"192.168.8.251",
            "port":"8000",
            "interval":"30",
            "id":"2232"
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ #
    ```   

+ When modifying several specified attributes without affecting others:
    ```shell
    component name|{"attribute1":"value1", "attribute2":"value2" , "attribute3":"value3"}
    ```
    value1, value2, value3 is usually a string.
    **ttrue** is returned if the modification succeeds, **tfalse** is returned if the modification fails.

    **Example: merge several attributes**
    ```shell
	~ # he 'gnss@nmea:client'                    # Query the client attribute in GNSS configuration
	{
	    "status":"enable",
	    "server":"192.168.8.251",
	    "port":"8000",
	    "interval":"30",
	    "id":"2232"
	}
	~ # he 'gnss@nmea:client|{"status":"disable","server":"192.168.2.11","proto":"tcp"}'                    # Modify the several value of the client attributes of gnss@nmea(GNSS configuration)
	ttrue
	~ # he 'gnss@nmea:client'                    # After the configuration is modified, check the configuration again
	{
        "status":"disable",
        "server":"192.168.2.11",
        "port":"8000",
        "interval":"30",
        "id":"2232",
        "proto":"tcp"
	}
	~ #
    ```   


+ To clear an attribute of a component configuration, press Enter after the `=`:
    ```shell
    component name:attribute path=
    ```
    **ttrue** is returned if the modification succeeds, **tfalse** is returned if the modification fails.

    **Example: clear an attribute**
    ```shell
    ~ # he 'gnss@nmea'                    # Query the all of GNSS configuration
    {
        "status":"enable",
        "client":
        {
            "status":"disable",
            "server":"192.168.2.11",
            "port":"8000",
            "interval":"30",
            "id":"2232",
            "proto":"tcp"
        },
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ # he 'gnss@nmea:client='                    # Clear the client attribute of gnss@nmea(GNSS configuration)
	ttrue
	~ # he 'gnss@nmea'                    # After the configuration is modified, check the configuration again
    {
        "status":"enable",
        "client2":
        {
            "status":"disable",
            "proto":"mqtt",
            "server":"",
            "port":"1883",
            "mqtt_id":"",
            "mqtt_username":"",
            "mqtt_password":"",
            "mqtt_keepalive":"10",
            "mqtt_interval":"",
            "mqtt_publish":"",
            "mqtt_publish_qos":"",
            "mqtt_subscribe":
            {
            }
        },
        "server":
        {
            "status":"disable",
            "proto":"tcp",
            "port":"",
            "limit":"5"
        }
    }
	~ #
    ```   


### Calling component method

To call a component method, you need to give the component name and method name, parameters can also be given if any
+ When a component method is called without arguments:
    ```shell
    component name.component method
    ```

    **Example: list clients**
    ```shell
    ~ # he 'client@station.list'                    # Call the list method of client@station(Client Access) to get the current list of clients
    {
        "B4:2E:99:3F:ED:12":
        {
            "ip":"192.168.8.250",
            "device":"lan"
        }
    }
    ~ #
    ```   


+ When calling a component method with parameters:
    ```shell
    component name.component method[ parameter ]
    ```

    **Example: sync time via NTP**
    ```shell
    ~ # he 'clock@date.ntpsync[ntp1.aliyun.com]'                    # Call the ntpsync method of clock@date(System Date) to synchronize time by NTP with ntp1.aliyun.com
    ttrue
    ~ #
    ```   

+ When calling a component method with multiple parameters:
    ```shell
    component name.component method[ parameter1, parameter2, parameter3, ... ]
    ```

    **Example: add an account with multiple parameters**
    ```shell
    ~ # he 'land@auth.add[,xiaomi,4431232]'                    # Call the add of land@auth to add an account with the first parameter empty (none), the second parameter xiaomi, and the third 4431232
    ttrue
    ~ # he 'land@auth.list'                    # Call land@auth's list method to get all the current accounts
    {
        "admin":
        {
            "key":"|*|V223KJDFLKJLKJA23232323543453453452323KJKHUI2KJKHKJKJKH22232321334"
        },
        "xiaomi":
        {
            "key":"|*|V24E4555304E4455784E304530524455304E446B33515452454E6A637A52444E45"
        }
    }
    ~ #
    ```   


+ When a component method returns JSON, you can ask that only the attribute values specified in the JSON be returned:
    ```shell
    component name.component method:attribute path
    ```

    **Example: query one field from method JSON**
    ```shell
    ~ # he 'ifname@lte.status'                    # Call the status method of ifname@lte to query the status of the first LTE connection
    {
        "mode":"dhcpc",
        "ifname":"ifname@lte",
        "netdev":"usb1",
        "gw":"10.33.13.104",
        "dns":"120.80.80.80",
        "dns2":"221.5.88.88",
        "ifdev":"modem@lte2",
        "ontime":"754",
        "metric":"10",
        "status":"up",
        "ip":"10.33.13.103",
        "mask":"255.255.255.240",
        "livetime":"05:03:36:0",
        "rx_bytes":"2681665",
        "rx_packets":"27616",
        "tx_bytes":"1433806",
        "tx_packets":"21926",
        "mac":"02:50:F4:00:00:00",
        "tid":"2",
        "devcom":"modem@device",
        "name":"Quectel-EC2X",
        "imei":"864337056708978",
        "mversion":"20CEFHLGR06A05M1G",
        "iccid":"89860121801097564807",
        "imsi":"460015356123463",
        "pcid":"184",
        "lac":"2604",
        "ci":"6DA5A09",
        "netera":"4G",
        "rsrp":"-96",
        "nettype":"FDD LTE",
        "rssi":"-66",
        "rsrq":"-9",
        "sinr":"-19",
        "band":"LTE BAND 3",
        "plmn":"46001",
        "csq":"25",
        "signal":"4",
        "operator":"ChinaUnion",
        "operator_advise":
        {
            "name":"ChinaUnion",
            "dial":"*99#",
            "apn":"3gnet"
        },
        "state":"connected"
    }
    ~ # he 'ifname@lte.status:ip'                    # Call the status method of ifname@lte, only query the ip address in the state of the first LTE connection
    10.33.13.103
    ~ # he 'ifname@lte.status:operator_advise'                    # Call the status method of ifname@lte, only query the operator_advise in the state of the first LTE connection
    {
        "name":"ChinaUnion",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ~ # he 'ifname@lte.status:operator_advise/apn'                    # Call the status method of ifname@lte, only query the APN in the state of the first LTE connection
    3gnet
    ~ #
    ```   


---
## Reference the component documentation to manage the gateway using the HE command

There are two ways to access component documentation. Each component can be managed through its documentation.
- Access [Online component documentation](../com/) View component documentation. This online file is added/updated as new features are developed.
- Contact technical support

### Component documentation points
- In [Online component documentation](../com/) projects in the system are listed in the form of a directory, and each project contains component documents
- Clicking on a project to enter the project will list all the component documents under this project
- Click on the component to open the component document, which begins with a function description
- Then there is **Configuration**. Configuration is in JSON format. The document also includes query/modify examples, and the configuration can be queried or modified via the HE command.
- This is usually followed by an introduction to the **Methods** of this component. Examples of calls are also provided and can be executed in the HE command.
- **Worked examples below** use **`~ # he '…'`** (Linux shell after **`ashy`**) so they can be copied verbatim. In **eline** (`$ `), type only the string inside the quotes—no **`he`** wrapper.

### Reference document query component configuration
The component name is given in the **Configuration**, for example **Syslog** — **[syslog.md](syslog.md)** — component name **land@syslog**

- Enter **component name** to return all configurations for this component. The attributes and examples for each configuration are described in **Configuration** in the component documentation.

    **Example: query all syslog configuration**
    ```shell
    ~ # he 'land@syslog'                    # Enter component name
    {                               # Return a JSON of all the component configuration
        "status":"enable",                # enable the syslog functions
        "location":"",                     # omit/empty uses the default file path policy (that file uses a random prefix under the system var dir)
        "level":"info",                   # log level is normal information
        "trace":"disable",                # disable the code information
        "size":"100",                     # log buffer is 100k
        "remote":"192.168.8.250",         # send the syslog to remote server 192.168.8.250
        "port":"514"                      # send the syslog to remote server port 514
    }
    ~ #
    ```

- Query a specific attribute by providing the attribute with the **attribute path** after the **component name**.

    **Example: query one syslog attribute**
    ```shell
    ~ # he 'land@syslog:level'                    # Query the value of the level attribute
    info                                    # value of level is info
    ~ #
    ```

### Refer to the component documentation to modify the component configuration   
Refer to **Syslog** docs (**[syslog.md](syslog.md)**). The attributes described in **Configuration** can be modified on the terminal by `component name:attribute path=value`.
- Modify the remote attribute of the land@syslog remote log server on the terminal.

    **Example: set one syslog attribute**
    ```shell
    ~ # he 'land@syslog:remote=192.168.8.250'                    # Change the value of remote to 192.168.8.250
    ttrue                                    # Return ttrue successfully
    ~ #
    ```

- Modify multiple attributes at the same time by encapsulating only the target fields in a JSON object (the rest remains unchanged).

    **Example: merge several syslog attributes**
    ```shell
    ~ # he 'land@syslog|{"remote":"192.168.8.251","port":"500"}'                    # Change the value of remote to 192.168.8.251 and value of port to 500
    ttrue                                                    # Return ttrue successfully
    ~ #
    ```

- Set the full syslog configuration on the terminal. To modify all configurations, provide the same JSON object.

    **Example: replace full syslog configuration**
    ```shell
    ~ # he 'land@syslog={"status":"enable","location":"","debug":"arch@usb","level":"info","trace":"disable","size":"100"}'
    ttrue                                    # Return ttrue successfully
    ~ #
    ```

### Call a component method by referring to the component documentation   
Refer to the same **Syslog** documentation. Methods described there can be called on the terminal as `component name.component method`.
- Call the component land@syslog's show method to display the current log.

    **Example: show syslog output**
    ```shell
    ~ # he 'land@syslog.show'
    Dec 15 15:47:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 102 times
    Dec 15 15:47:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 103 times
    Dec 15 15:47:30 V520-12CC70 user.warn syslog: modem@lte check simcard failed 104 times
    Dec 15 15:47:35 V520-12CC70 user.warn syslog: modem@lte check simcard failed 105 times
    Dec 15 15:47:40 V520-12CC70 user.warn syslog: modem@lte check simcard failed 106 times
    Dec 15 15:47:45 V520-12CC70 user.warn syslog: modem@lte check simcard failed 107 times
    Dec 15 15:47:50 V520-12CC70 user.warn syslog: modem@lte check simcard failed 108 times
    Dec 15 15:47:55 V520-12CC70 user.warn syslog: modem@lte check simcard failed 109 times
    Dec 15 15:48:00 V520-12CC70 user.warn syslog: modem@lte check simcard failed 110 times
    Dec 15 15:48:05 V520-12CC70 user.warn syslog: modem@lte check simcard failed 111 times
    Dec 15 15:48:10 V520-12CC70 user.warn syslog: modem@lte check simcard failed 112 times
    Dec 15 15:48:15 V520-12CC70 user.warn syslog: modem@lte check simcard failed 113 times
    Dec 15 15:48:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 114 times
    Dec 15 15:48:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 115 times
    Dec 15 15:48:30 V520-12CC70 user.warn syslog: modem@lte check simcard failed 116 times
    Dec 15 15:48:35 V520-12CC70 user.warn syslog: modem@lte check simcard failed 117 times
    ~ #
    ```
- Call the clear method of component land@syslog to clear all current logs.

    **Example: clear syslog**
    ```shell
    ~ # he 'land@syslog.clear'
    ttrue
    ~ #
    ```

---

## Enter the Linux shell and run HE with the `he` prefix

### Typical path: **eline** → **`ashy`** → **`he`**

1. After Telnet / SSH / serial login you usually see **eline** with **`$ `**. Type HE **without** the `he` wrapper; use **`exit`** or Ctrl+D to leave eline. Details: **[eline.md](eline.md)** (`set`, passthrough OS commands, readline history).  
2. Type **`ashy`**. This process is replaced by **`/bin/ash --login`** (BusyBox `ash`; prompt is often **`~ #`** for root).  
3. From **`~ #`**, run **`he '<one full HE line>'`**. See **How the `he` program joins arguments** earlier in this document—**one shell argument** is safest.  
4. **`exit`** from `ash` usually **ends the whole login session**, because eline was replaced. Open a spare SSH/Telnet session before risky tests.

### Alternate path: classic **`# `** loop → **`elf`**

Some images show **`# `** first. If the banner documents it, **`elf`** (or the keyword your firmware uses) spawns the same BusyBox shell:

```shell
# elf

BusyBox v1.22.1 (2021-04-20 10:35:42 CST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

~ #
```

### Run HE from the shell

```shell
he '<HE command line>'
```

Use **single quotes** whenever the line contains `|`, `{`, `}`, `[`, `]`, `:`, `=`, `,`, or spaces inside JSON.

### Advanced `he` prefixes (first character of the combined string)

After all arguments are concatenated:

| Leading byte | Behavior |
|---|---|
| **`+`** | Loop execute (`loop_he`) |
| **`=`** | Parse and print structure only (`print_he`) |
| **`-`** | Silent mode (`slient_he` in source—minimal output) |

Examples (entire payload must still follow the “one logical line” rules):

```shell
he '+land@machine'
he '=land@machine:name'
he '-land@machine.status'
```

### Troubleshooting

| Symptom | Typical cause | Quick check |
|---|---|---|
| `tfalse` | Invalid path, value, or validation | `he 'component:attr/path'` |
| Empty return | Field missing or API `NULL` | Query parent path with `he '…'` |
| Wrong / merged text | Multiple `argv` glued without spaces | Use **`he 'single quoted line'`** |
| JSON errors | Bad JSON in the HE line | Fix JSON; keep shell quoting |

### Example session (shell)

```shell
# elf

BusyBox v1.22.1 (2021-04-20 10:35:42 CST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

~ # he 'land@machine'
{
    "platform":"smtk",
    "hardware":"mt7621",
    "custom":"v520",
    "scope":"donkey",
    "language":"cn",
    "macid":"88124E200030",
    "mac":"88:12:4E:20:00:30",
    "name":"V520-200030",
    "title":"V520-200030",
    "mode":"donkeycon"
}
~ # he 'land@machine.status'
{
    "custom":"v520",
    "version":"4.3.3w-042021",
    "publish":"042021",
    "macid":"88124E200030",
    "mac":"88:12:4E:20:00:30",
    "magic":"D7441F2CE4605020",
    "model":"V520",
    "name":"V520-200030",
    "mode":"donkeycon",
    "livetime":"00:18:28:0",
    "current":"12:11:16:04:20:2021"
}
~ #
```

---

## See also

- **[eline.md](eline.md)** — `$ ` prompt, `set`, `ashy`, passthrough list, and Ctrl+D behavior.
