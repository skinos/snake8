## agent@mqtt — MQTT Remote Management Client

### Overview

Connect the device to a remote MQTT broker for registration, status reporting, periodic heartbeats, and remote HE command execution. The component mirrors the role of **`agent@heclient`** but uses MQTT publish/subscribe instead of the Heport line protocol.
It does not send Heport receipt packets or application-level keeplive; broker **`mqtt_keepalive`** maintains the session.
Optional TLS uses certificate files under the product configuration directory: **`mqtt.ca`**, and optionally **`mqtt.crt`** / **`mqtt.key`**.
TLS is enabled only when **`mqtt.ca`** exists; with CA alone the client verifies the broker, and with **`mqtt.crt`** + **`mqtt.key`** present it also presents a client certificate.

- Publish device registration, update, heartbeat, and optional proactive reports to configurable topics
- Subscribe to server command topics when configured
- Execute remote JSON HE commands and publish HE command results
    > Empty publish or subscribe topic strings disable the corresponding feature entirely

### Configuration reference ( agent@mqtt )

```json
// Attributes introduction 
{
    "status":"remote management over MQTT",                 // [ "disable","enable" ], service master switch
                                                               // "disable": background service not started
                                                               // "enable": connect to broker when setup runs

    "server":"MQTT broker address",                         // [ string ], domain name or ip address
    "port":"MQTT broker port",                              // [ number ], default 1883
    "mqtt_id":"MQTT client identifier",                     // [ string ], empty uses machine macid
    "mqtt_username":"MQTT broker username",                 // [ string ], optional
    "mqtt_password":"MQTT broker password",                 // [ string ], optional
    "mqtt_keepalive":"MQTT session keepalive in seconds",   // [ number ], default 60
                                                               // also used as the period base for mosquitto_loop_misc timer (keepalive/2)

    "user":"account username for device registration",       // [ string ], required when status is enable; written into registration JSON
    "vcode":"account verification code",                    // [ string ], optional, written into registration JSON
    "type":"device type reported to server",               // [ string ], default router
    "extern":"outbound interface before broker connect",    // [ string ]: [ "disable","default","ifname@wan",... ]
                                                               // empty string is treated as "default"
                                                               // "disable": no outbound bind, no network reset joint
                                                               // "default": bind default gateway, reset on network/online
                                                               // "ifname@wan", "ifname@lte", ...: bind that interface, reset on network/onextern when ifname matches

    "topic_register":"publish topic for device registration",       // [ string ], empty skips register publish
    "topic_update":"publish topic for device snapshot update",      // [ string ], empty skips update publish
    "topic_heart":"publish topic for periodic heartbeat",             // [ string ], empty skips heart publish
    "topic_send":"publish topic for proactive JSON report",         // [ string ], empty disables send API publish
    "topic_ret":"publish topic for HE command result",        // [ string ], empty executes commands without reply
    "topic_he":"subscribe topic for remote HE commands",         // [ string ], empty skips command subscribe

    "topic_register_qos":"QoS for topic_register",            // [ number ], default 1
    "topic_update_qos":"QoS for topic_update",                // [ number ], default 1
    "topic_heart_qos":"QoS for topic_heart",                  // [ number ], default 0
    "topic_send_qos":"QoS for topic_send",                    // [ number ], default 1
    "topic_ret_qos":"QoS for topic_ret",                      // [ number ], default 1
    "topic_he_qos":"QoS for topic_he",              // [ number ], default 1

    "heart":                              // [ string ]: { json }, periodic HE snapshot collection
    {
        "he command":"interval in seconds"  // [ string ]:[ number ], HE command and timer interval
        // "...":"..."  How many heartbeat entries show how many properties
    }
}
```

#### Configuration example

Example, show all the mqtt configure
```shell
agent@mqtt
{
    "status":"enable",                                    # remote management is enabled
    "server":"mqtt.example.com",                          # broker address
    "port":"1883",                                        # broker port
    "mqtt_id":"",                                         # use machine macid as client id
    "mqtt_username":"device",                             # broker login username
    "mqtt_password":"secret",                             # broker login password
    "mqtt_keepalive":"60",                                # MQTT keepalive 60 seconds
    "user":"ashyelf",                                     # account bound to this device
    "vcode":"123456",                                     # account verification code
    "type":"router",                                      # device type is router
    "extern":"default",                                   # wait for default gateway before connect
    "topic_register":"agent/device/register",             # registration publish topic
    "topic_update":"agent/device/update",                 # update publish topic
    "topic_heart":"agent/device/heart",                   # heartbeat publish topic
    "topic_send":"",                                      # proactive send topic disabled
    "topic_he":"agent/device/command",               # remote command subscribe topic
    "topic_ret":"agent/device/ret",                       # HE command result publish topic
    "topic_register_qos":"1",
    "topic_update_qos":"1",
    "topic_heart_qos":"0",
    "topic_send_qos":"1",
    "topic_ret_qos":"1",
    "topic_he_qos":"1",
    "heart":
    {
        "gnss@nmea":"10"                                  # collect gnss every 10 seconds
    }
}
```

#### Configuration settings example

Example, enable the mqtt agent and set broker address
```shell
agent@mqtt={"status":"enable","server":"mqtt.example.com","port":"1883","user":"ashyelf"}
ttrue
```

Example, disable the mqtt agent
```shell
agent@mqtt:status=disable
ttrue
```

Example, set the heartbeat publish topic
```shell
agent@mqtt:topic_heart=agent/device/heart
ttrue
```

Example, merge set broker and topic attributes ( include "server" "port" "topic_register" "topic_he" )
```shell
agent@mqtt|{"server":"broker.example.com","port":"8883","topic_register":"dev/reg","topic_he":"dev/cmd"}
ttrue
```

### Concepts

**Registration and update payloads**
* On connect, when **`topic_register`** is configured, the service publishes one retained JSON object containing **`type`**, **`user`**, **`vcode`**, and machine / gateway / IO / GNSS / sensor snapshots.
* The snapshot fields match the **`{JSON}`** portion of a Heport (**`agent@heclient`**) register packet; MQTT also puts **`user`** inside the JSON (heclient carries **`user`** in the Heport line prefix instead).
* **`update`** and the **`machine/status`** joint event trigger the same JSON on **`topic_update`** when that topic is configured.

**Heartbeat**
* Each entry in **`heart`** starts a timer when **`topic_heart`** is non-empty. When it fires, the service collects the named HE command and publishes the result JSON to **`topic_heart`**.
* Empty **`topic_heart`** disables heart entirely (no timers).
* The **`heart`** API reads or replaces the runtime heart schedule without writing configuration; payload format matches the **`heart`** object in configuration.

**Remote commands**
* JSON received on **`topic_he`** is passed to **`talk_he_command`**. The result JSON is published to **`topic_ret`** when that topic is configured.
* Only JSON command objects are supported (not single HE strings).

**Device identity on the broker**
* **`mqtt_id`** (default machine macid) and optional **`mqtt_username`** / **`mqtt_password`** identify the client to the broker.
* **`user`** and **`vcode`** remain in the registration JSON for the application server.

**Extern interface and reset**
* When **`extern`** is not **`disable`**, the service waits for the selected gateway or interface, adds a host route to the broker, and registers **`agent@mqtt.reset`**.
* **`extern=default`** (or empty): joint on **`network/online`**. Specific **`ifname@…`**: joint on **`network/onextern`**.
* **`reset`** restarts the background service when the outbound path matches so routing and MQTT session are rebuilt.
* Session model matches **`agent@heclient`**: resolve DNS and connect once per process; disconnect or connect failure exits so the platform can restart and re-resolve / re-route.
* When the dependency is not ready, the service exits **`ttrue`** and waits for joint **`reset`**.

### API Reference

#### Management APIs

+ `setup[]` **initialize the mqtt component and start the background service when status is enable**
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation

+ `shut[]` **stop the mqtt background service**
    - Unregisters runtime **`network/online`** and **`network/onextern`** reset handlers
    - failed return tfalse
    - succeed return ttrue

    Example, shutdown the mqtt agent
    ```shell
    agent@mqtt.shut
    ttrue
    ```

+ `reset[ event, event data ]` **restart the mqtt background service when the bound extern changes**
    - event ----------------------- [ string ], joint event name (for example network/online)
    - event data ------------------ [ json ], event payload; must include **`ifname`**
    - Used as a joint handler registered by the service when **`extern`** is not **`disable`**
    - **`extern=default`**: acts only when event is **`network/online`**
    - **specific interface**: acts when event **`ifname`** equals configured **`extern`**
    - failed return tfalse
    - succeed return ttrue

    Example, reset when default gateway comes online (normally invoked by joint)
    ```shell
    agent@mqtt.reset[ network/online, {"ifname":"ifname@wan"} ]
    ttrue
    ```

    Example, reset when bound interface ifname@lte comes online
    ```shell
    agent@mqtt.reset[ network/onextern, {"ifname":"ifname@lte"} ]
    ttrue
    ```

#### Query APIs

+ `status[]` **get current mqtt connection status**   
    - failed return NULL
    - succeed return [ json ], connection state and resolved broker address
    ```json
    {
        "status":"connection state",        // [ string ]: [ "down", "uping", "online", "suberror" ]
                                                // "down": background service is not running
                                                // "uping": service is running but not connected yet
                                                // "online": connected to broker successfully
                                                // "suberror": topic_he subscribe failed; service exited
        "server":"resolved broker ip"       // [ string ], present when status is uping, online, or suberror
                                                // IP resolved once at service start
    }
    ```

    Example, get status when connected
    ```shell
    agent@mqtt.status
    {
        "status":"online",                  # connected to broker successfully
        "server":"114.132.219.158"          # resolved broker ip address
    }
    ```

    Example, get status when service is stopped
    ```shell
    agent@mqtt.status
    {
        "status":"down"                     # service is not running
    }
    ```

#### Control APIs

+ `update[]` **ask the running service to republish device snapshot to topic_update**   
    - failed return tfalse
    - succeed return ttrue

    Example, update device information to the broker
    ```shell
    agent@mqtt.update
    ttrue
    ```

+ `send[ json payload ]` **publish a JSON payload to topic_send from the running service**   
    - json payload ----------------- [ json ], arbitrary JSON object to publish
    - failed return tfalse
    - succeed return ttrue

    Example, publish a proactive report when topic_send is configured
    ```shell
    agent@mqtt.send[{"land@machine.status":{}}]
    ttrue
    ```

+ `heart[ json payload ]` **get or replace the runtime heart schedule in the running service**   
    - json payload ----------------- [ json ], optional heart object; same format as configuration **`heart`**
    - without json payload --------- return the current runtime heart object
    - with json payload ------------ replace runtime heart timers with the given object; failed return tfalse, succeed return ttrue

    Example, get current runtime heart schedule
    ```shell
    agent@mqtt.heart
    {
        "gnss@nmea":"10"
    }
    ```

    Example, replace runtime heart schedule
    ```shell
    agent@mqtt.heart[{"gnss@nmea":"30","land@machine.status":"60"}]
    ttrue
    ```

### Joint Events Hook

The product manifest registers the following platform joint handler for **`agent@mqtt`**:

| Joint key | Method |
|-----------|-------------|
| `machine/status` | `agent@mqtt.update` |

When **`extern`** is not **`disable`**, the background service also registers at runtime:

| Joint key | Method | Condition |
|-----------|-------------|-----------|
| `network/online` | `agent@mqtt.reset` | **`extern=default`** (or empty) |
| `network/onextern` | `agent@mqtt.reset` | **`extern`** is a specific interface name |
