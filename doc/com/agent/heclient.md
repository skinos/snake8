***
## HE Client -- Heport Remote Management Client
Connect to heport server via SSL/TLS, accept remote administrative control, execute commands, report device status, and manage other components

#### Configuration( agent@heclient )
```json
{
    // basic
    "status":"connect to heport server for remote management",   // [ "disable", "enable" ]
    "server":"heport server address",                            // [ string ], domain name or ip address
    "port":"heport server port",                                 // [ number ], default 20002
    "user":"username for bindding to the account",               // [ string ]
    "vcode":"verification code for the account",                 // [ string ], optional
    "type":"device type",                                        // [ string ], default "router"

    // bindding extern network
    "extern":"bindding extern ifname to connect server",         // [ "ifname@lte", "ifname@wan", "ifname@wisp", ... ], optional
                                                                    // call network@frame.local[extern] get the list
                                                                    // when not set, use default gateway to connect server

    // connection control
    "connect_timeout":"timeout for connection in seconds",       // [ number ], default 20
    "keeplive_interval":"keeplive interval in seconds",          // [ number ], default 10
    "keeplive_timeout":"timeout for keeplive in seconds"         // [ number ], default 35
}
```

Example, show all the configure
```shell
agent@heclient
{
    "status":"enable",                        # remote management is enabled
    "server":"cls.ashyelf.com",               # heport server address
    "port":"20002",                           # heport server port
    "user":"ashyelf",                         # bindding to account ashyelf
    "type":"router",                          # device type is router
    "connect_timeout":"20",                   # connection timeout 20 seconds
    "keeplive_interval":"10",                 # keeplive interval 10 seconds
    "keeplive_timeout":"35"                   # keeplive timeout 35 seconds
}
```

Example, enable the he client and bindding to account dimmalex@gmail.com
```shell
agent@heclient={"status":"enable","server":"devport.ashyelf.com","port":"20002","user":"dimmalex@gmail.com","vcode":"123456"}
ttrue
```

Example, disable the he client
```shell
agent@heclient:status=disable
ttrue
```

Example, modify the heport server to heport.ashyelf.com
```shell
agent@heclient:server=heport.ashyelf.com
ttrue
```

Example, modify the keeplive interval to 30 seconds
```shell
agent@heclient:keeplive_interval=30
ttrue
```

Example, set the extern network interface to ifname@lte
```shell
agent@heclient:extern=ifname@lte
ttrue
```



#### **API**

+ `setup[]` **setup the he client, start the connection service**
    setup will read the configuration, if status is "enable", start the background service process to connect to the heport server
    - succeed return ttrue
    - failed return tfalse

    Example, setup the he client
    ```shell
    agent@heclient.setup
    ttrue
    ```

+ `shut[]` **shutdown the he client and all managed components**
    shut will stop the background service, shutdown the managed gtog(agent@gtog) and portc(agent@portc) components, and remove the adjust file
    - succeed return ttrue
    - failed return tfalse

    Example, shutdown the he client
    ```shell
    agent@heclient.shut
    ttrue
    ```

+ `status[]` **get the he client current status information**
    - failed return NULL
    - succeed return json to describes status information
    ```json
    // Attributes introduction of talk by the API return
    {
        "status":"current status",        // [ "uping", "down", "online", "usererror", "vcodeerror" ]
                                             // "uping" for connecting to server
                                             // "down" for service is not running
                                             // "online" for connected to server successfully
                                             // "usererror" for username is wrong or not exist
                                             // "vcodeerror" for verification code is wrong
        "server":"resolved heport server ip" // [ ip address ], only available when status is "uping" or "online"
    }
    ```

    Example, get the he client status when connected
    ```shell
    agent@heclient.status
    {
        "status":"online",                    # connected to server successfully
        "server":"114.132.219.158"            # heport server resolved ip address
    }
    ```

    Example, get the status when connecting
    ```shell
    agent@heclient.status
    {
        "status":"uping",                     # connecting to server
        "server":"114.132.219.158"
    }
    ```

    Example, get the status when service is stopped
    ```shell
    agent@heclient.status
    {
        "status":"down"                       # service is not running
    }
    ```

    Example, get the status when username is wrong
    ```shell
    agent@heclient.status
    {
        "status":"usererror"                  # username wrong, the service will exit
    }
    ```

    Example, get the status when vcode is wrong
    ```shell
    agent@heclient.status
    {
        "status":"vcodeerror"                 # verification code wrong, the service will exit
    }
    ```

+ `update[]` **notify the service to re-send device information to server**
    send SIGUSR1 signal to the service process, trigger it to re-collect and send device status (machine status, network gateway, GNSS info, sensor status) to the heport server
    - succeed return ttrue, the service process is running and signal sent
    - failed return tfalse, the service process is not running

    Example, update the device information to server
    ```shell
    agent@heclient.update
    ttrue
    ```

+ `adjust[ {adjust configuration} ]` **adjust the configuration and state of other components**
    adjust will modify the configuration of specified components and restart them accordingly. 
    If the new configuration is the same as existing, it will only start or stop the component based on its status field.
    If the configuration is different, it will shut down the component, apply new configuration, then start it
    - {adjust configuration} ------ json
    ```json
    // Attributes introduction of json pass to API
    {
        "component name":                           // [ string ], the full component object name (e.g. "agent@portc", "agent@gtog")
        {
            // the complete configuration for this component
        }
        // ...more other component configurations
    }
    ```
    - succeed return ttrue
    - failed return tfalse

    Example, adjust to enable the port client, disable the network client
    ```shell
    agent@heclient.adjust[{"agent@portc":{"status":"enable"},"agent@gtog":{"status":"disable"}}]
    ttrue
    ```

    Example, adjust to enable gtog with server configuration
    ```shell
    agent@heclient.adjust[{"agent@gtog":{"status":"enable","server":"192.168.1.1","port":"20000"}}]
    ttrue
    ```

+ `service[]` **internal background service (not called directly)**
    this is the main service loop started by setup, it handles:
    1. SSL/TLS connection to heport server
    2. Device registration (send macid, username, pubkey and device info)
    3. Keepalive mechanism
    4. Receiving and executing remote commands (both JSON format and string format)
    5. Forwarding command results back to server

    The service will exit and return different values based on the situation:
    - return terror: username or vcode is wrong, will not auto-restart
    - return tfalse: connection failed or timeout, will auto-retry
    - return ttrue: extern network interface is not ready, will auto-retry

    **Registration information sent to server includes:**
    ```json
    {
        "type":"device type",                              // device type, e.g. "router"
        "vcode":"verification code",                       // account verification code
        "land@machine.status":                             // machine status information
        {
            "mode":"device model",
            "name":"device name",
            "mac":"device mac address",
            "version":"firmware version",
            "cfgversion":"configuration version"
            // ... other machine status fields
        },
        "network@frame.gateway":                           // current network gateway information
        {
            "name":"gateway interface name",
            "ip":"gateway ip address",
            "mask":"netmask",
            "gw":"gateway address",
            "dns":"dns server",
            "dns2":"secondary dns server",
            "rx_bytes":"receive bytes",
            "tx_bytes":"transmit bytes"
        },
        "gnss@nmea.info":                                  // GNSS/GPS information (if available)
        {
            "step":"current step",
            "latitude":"latitude value",
            "longitude":"longitude value",
            "speed":"speed",
            "elv":"elevation"
        },
        "<sensor_name>.status":                            // sensor status (if available, may have multiple)
        {
            // sensor specific status fields
        }
    }
    ```

    **Protocol format:**
    - Register packet: `<macid>+<user>|<pubkey>|<json>\n`
    - Keepalive request: `<tid>+keeplive\n`
    - Keepalive response: `<tid>-k\n`
    - User error response: `<tid>-u...\n`
    - Vcode error response: `<tid>-v...\n`
    - JSON command from server: `<tid>-{ "cmd":"<object>.<method>", "cmd2":"<object2>.<method2>" }\n`
    - JSON command response: `<tid>+{ "cmd":"<result>", "cmd2":<result2> }\n`
    - String command from server: `<tid>-<object>.<method>\n`
    - String command response: `<tid>+<result>\n`
    - Update report: `<tid>+u<json>\n`
