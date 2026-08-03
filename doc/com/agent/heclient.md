## agent@heclient — Heport Remote Management Client

### Overview

Connect the device to a remote Heport service over TLS for registration, status reporting, periodic heartbeats, and remote HE command execution.
The service uses a line-oriented Heport protocol (not MQTT). Related agent components such as **`agent@portc`** and **`agent@gtog`** can be started or reconfigured through **`adjust`**.

- Register the device with account **`user`** / **`vcode`** and push machine, gateway, IO, GNSS, and sensor snapshots
- Keep the session alive with application keeplive; republish snapshots on **`update`**
- Execute remote JSON talk commands and string HE lines from the server, then ACK results
- Optional outbound bind via **`extern`** (default gateway or a specific interface) with joint **`reset`** when the path changes
    > Account errors (**`usererror`** / **`vcodeerror`**) stop the service without auto-restart; connect or extern-not-ready failures retry by platform policy

### Configuration reference ( agent@heclient )

```json
// Attributes introduction 
{
    "status":"connect to heport server for remote management",  // [ "disable","enable" ], service master switch
                                                                   // "disable": background service not started
                                                                   // "enable": connect when setup runs

    "server":"heport server address",                           // [ string ], domain name or ip address
    "port":"heport server port",                                // [ number ], default 20002
    "user":"account username for device registration",          // [ string ], required when status is enable
    "vcode":"account verification code",                        // [ string ], optional
    "type":"device type reported to server",                    // [ string ], default router

    "extern":"outbound interface before connect",               // [ string ]: [ "disable","default","ifname@wan",... ]
                                                                   // empty string is treated as "default"
                                                                   // "disable": no outbound bind, no network reset joint
                                                                   // "default": bind default gateway, reset on network/online
                                                                   // "ifname@wan", "ifname@lte", ...: bind that interface, reset on network/onextern when ifname matches

    "connect_timeout":"TCP/TLS connect timeout in seconds",     // [ number ], default 20; covers tcp_connect and TLS handshake
    "keeplive_interval":"keeplive probe interval in seconds",   // [ number ], default 10
    "keeplive_timeout":"idle timeout before session exit",      // [ number ], default 35

    "heart":                              // [ string ]: { json }, periodic HE snapshot collection
    {
        "he command":"interval in seconds"  // [ string ]:[ number ], HE command and timer interval
        // "...":"..."  How many heartbeat entries show how many properties
    }
}
```

#### Configuration example

Example, show all the heclient configure
```shell
agent@heclient
{
    "status":"enable",                                    # remote management is enabled
    "extern":"default",                                   # bind default gateway before connect
    "server":"cls.ashyelf.com",                           # heport server address
    "port":"20002",                                       # heport server port
    "user":"ashyelf",                                     # account bound to this device
    "vcode":"123456",                                     # account verification code
    "type":"router",                                      # device type is router
    "connect_timeout":"20",                               # TCP/TLS connect timeout 20 seconds
    "keeplive_interval":"10",                             # keeplive interval 10 seconds
    "keeplive_timeout":"35",                              # idle timeout 35 seconds
    "heart":
    {
        "gnss@nmea":"10"                                  # collect gnss every 10 seconds
    }
}
```

#### Configuration settings example

Example, enable the he client and bind account
```shell
agent@heclient={"status":"enable","server":"devport.ashyelf.com","port":"20002","user":"dimmalex@gmail.com","vcode":"123456"}
ttrue
```

Example, disable the he client
```shell
agent@heclient:status=disable
ttrue
```

Example, set the extern network interface to ifname@lte
```shell
agent@heclient:extern=ifname@lte
ttrue
```

Example, merge set server and keeplive attributes ( include "server" "keeplive_interval" "keeplive_timeout" )
```shell
agent@heclient|{"server":"heport.ashyelf.com","keeplive_interval":"30","keeplive_timeout":"60"}
ttrue
```

### Concepts

**Registration and update**
* After TLS connect, the service sends one register line: **`{macid}+{user}|{pubkey}|{json}`**. The JSON includes type, vcode, machine status, gateway, IO, GNSS, and sensor snapshots.
* **`update`** (and the **`machine/status`** joint) asks the running service to send **`0+r{json}`** with the same snapshot content.

**Heartbeat and keeplive**
* Each entry in **`heart`** starts a timer. When it fires, the service collects the named HE command and sends **`0+h{json}`**.
* Application keeplive uses **`keeplive_interval`** / **`keeplive_timeout`**. Idle beyond timeout exits the service so the platform can retry.

**Remote commands**
* Server JSON talk objects are executed with **`talk_he_command`**; string HE lines use **`he_execute`**. Results are ACKed on the same talk id.
* Non-zero talk id means a forwarded unix/http request on the server; the device must echo the same tid in the ACK.

**Extern interface and reset**
* When **`extern`** is not **`disable`**, the service waits for the selected gateway or interface, adds a host route to the resolved server IP, and registers **`agent@heclient.reset`**.
* **`extern=default`** (or empty): joint on **`network/online`**. Specific **`ifname@…`**: joint on **`network/onextern`**.
* **`reset`** restarts the background service when the outbound path matches so routing and session are rebuilt.

**Account errors**
* Server **`0-u…`** / **`0-v…`** set **`usererror`** / **`vcodeerror`**, unregister network reset joints, and exit with **`terror`** (no auto-restart until configuration is fixed and setup runs again).

### API Reference

#### Management APIs

+ `setup[]` **initialize the heclient component and start the background service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **stop the heclient background service and tear down managed agent@gtog / agent@portc**
    - Unregisters runtime **`network/online`** and **`network/onextern`** reset handlers
    - failed return tfalse
    - succeed return ttrue

    Example, shutdown the he client
    ```shell
    agent@heclient.shut
    ttrue
    ```

+ `reset[ event, event data ]` **restart the heclient background service when the bound extern changes**
    - event ----------------------- [ string ], joint event name (for example network/online)
    - event data ------------------ [ json ], event payload; must include **`ifname`**
    - Used as a joint handler registered by the service when **`extern`** is not **`disable`**
    - **`extern=default`**: acts only when event is **`network/online`**
    - **specific interface**: acts when event **`ifname`** equals configured **`extern`**
    - failed return tfalse
    - succeed return ttrue

    Example, reset when default gateway comes online (normally invoked by joint)
    ```shell
    agent@heclient.reset[ network/online, {"ifname":"ifname@wan"} ]
    ttrue
    ```

    Example, reset when bound interface ifname@lte comes online
    ```shell
    agent@heclient.reset[ network/onextern, {"ifname":"ifname@lte"} ]
    ttrue
    ```

#### Query APIs

+ `status[]` **get current heclient connection status**
    - failed return NULL
    - succeed return [ json ], connection state and resolved server address
    ```json
    {
        "status":"connection state",        // [ string ]: [ "down", "uping", "online", "usererror", "vcodeerror" ]
                                                // "down": background service is not running
                                                // "uping": service is running but session not ready
                                                // "online": connected and session marked succeed
                                                // "usererror": username wrong or not exist; service stopped
                                                // "vcodeerror": verification code wrong; service stopped
        "server":"resolved heport server ip" // [ string ], present when status is uping or online
    }
    ```

    Example, get status when connected
    ```shell
    agent@heclient.status
    {
        "status":"online",                  # connected to heport successfully
        "server":"114.132.219.158"          # resolved heport server ip
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@heclient.status
    {
        "status":"down"                     # service is not running
    }
    ```

    Example, get status when username is wrong
    ```shell
    agent@heclient.status
    {
        "status":"usererror"                # username rejected; service exited
    }
    ```

#### Control APIs

+ `update[]` **ask the running service to re-send the device snapshot to heport**
    - failed return tfalse
    - succeed return ttrue

    Example, update device information to the server
    ```shell
    agent@heclient.update
    ttrue
    ```

+ `send[ json payload ]` **send a JSON payload to heport from the running service**
    - json payload ----------------- [ json ], arbitrary JSON object sent as **`0+s{...}`**
    - failed return tfalse
    - succeed return ttrue

    Example, send a proactive report
    ```shell
    agent@heclient.send[{"network@frame.gateway":{}}]
    ttrue
    ```

+ `adjust[ adjust configuration ]` **apply configuration to other components via `sset`**
    - adjust configuration --------- [ json ], map of object name → configure object
    - For each entry: enable cache store, then **`sset(object, cfg)`** so the target’s **`_set`** saves config and applies its restart policy (**`shut`/`setup`** or channel **`sreset`**)
    - Prefer **`agent@portc`** and channel **`agent@net*`**; pool **`agent@gtog`** keys are mainly **`net_max`** / **`port_start`**
    - failed return tfalse (any **`sset`** failed)
    - succeed return ttrue
    ```json
    {
        "component name":                  // [ string ]: { json }, full object name (e.g. "agent@portc")
        {
            // configuration for this component
        }
        // "...":{ ... }  How many components show how many properties
    }
    ```

    Example, enable portc
    ```shell
    agent@heclient.adjust[{"agent@portc":{"status":"enable","server":"192.168.1.1","port":"20005"}}]
    ttrue
    ```

    Example, enable a mesh channel
    ```shell
    agent@heclient.adjust[{"agent@net":{"status":"enable","server":"192.168.1.1","port":"20002","netid":"office-vpn"}}]
    ttrue
    ```

### Joint Events Hook

The product manifest registers the following platform joint handler for **`agent@heclient`**:

| Joint key | Method |
|-----------|-------------|
| `machine/status` | `agent@heclient.update` |

When **`extern`** is not **`disable`**, the background service also registers at runtime:

| Joint key | Method | Condition |
|-----------|-------------|-----------|
| `network/online` | `agent@heclient.reset` | **`extern=default`** (or empty) |
| `network/onextern` | `agent@heclient.reset` | **`extern`** is a specific interface name |
