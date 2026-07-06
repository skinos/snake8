## ipsec@list — IPsec Client Infrastructure Management

### Overview

Manage the lifecycle of all IPsec VPN client instances using strongSwan. This component scans for configured instances (`ipsec@client`, `ipsec@client2`, … up to `ipsec@client10`) and registers each enabled one with `network@frame` as a VPN interface. It also provides instance creation and deletion APIs.

- registers/unregisters all configured IPsec instances with `network@frame` at boot/shutdown
- lists all IPsec client configurations and statuses
- creates new IPsec instances with sensible defaults
- deletes existing IPsec instances and unregisters them from the network frame
- generates strongSwan `swanctl.conf` for each instance



### Network Architecture

`ipsec@list` is the **infrastructure manager** for IPsec VPN clients. At boot, it iterates all configured `ipsec@client*` instances and registers each one with `network@frame` as a VPN interface. Each instance then manages its own IPsec tunnel independently via strongSwan's `swanctl`. `ipsec@list` does not hold its own configuration — all per-instance settings live in `ipsec@client`, `ipsec@client2`, etc.

For the full network architecture, see [`../network/frame.md`](../network/frame.md).



### API Reference

#### Management APIs

+ `setup[]` **initialize IPsec infrastructure**
    - succeed return ttrue
    - iterates ipsec@client through ipsec@client10, registers each configured instance with `network@frame` and schedules its setup

+ `shut[]` **shut down all IPsec clients**
    - succeed return ttrue
    - calls shut on each instance, then unregisters from `network@frame`


#### Query APIs

+ `list[]` **list all IPsec client configurations**
    - failed return NULL
    - succeed return [ json ], all configured instances with their full configuration
    ```json
    {
        "ipsec@client":                     // [ string ]: { json }, instance name
        {
            "status":"client status",       // [ string ]
            "server":"remote gateway",      // [ string ]
            // ... full instance config
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all IPsec clients
    ```shell
    ipsec@list.list
    {
        "ipsec@client":
        {
            "status":"enable",
            "extern":"default",
            "server":"203.0.113.1",
            "auth_method":"psk",
            "local_ts":"10.1.0.0/24",
            "remote_ts":"10.2.0.0/24"
        }
    }
    ```

+ `status[]` **list all IPsec client statuses**
    - failed return NULL
    - succeed return [ json ], all configured instances with their runtime status
    ```json
    {
        "ipsec@client":                     // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "connecting", "established", "down" ]
            "serverip":"server IP",         // [ ip address ], resolved server IP
            // ... other status fields when established
        }
        // "...":{}  How many instances show how many properties
    }
    ```

    Example, list all IPsec client statuses
    ```shell
    ipsec@list.status
    {
        "ipsec@client":
        {
            "status":"established",
            "serverip":"203.0.113.1",
            "local_ts":"10.1.0.0/24",
            "remote_ts":"10.2.0.0/24",
            "livetime":"02:30:15:0",
            "rx_bytes":"123456",
            "tx_bytes":"654321"
        }
    }
    ```


#### Control APIs

+ `add[ server ]` **add a new IPsec client**
    - server ------------ [ string ], IPsec gateway address (IP or domain)
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "ipsec@client3")
    - creates with defaults: status=disable, extern=default, version=2, auth_method=psk, start_action=start, dpd_action=restart
    - registers the new instance with `network@frame`

    Example, add an IPsec client
    ```shell
    ipsec@list.add[ 203.0.113.1 ]
    ipsec@client3
    ```

+ `delete[ object ]` **delete an IPsec client**
    - object ------------ [ string ], the instance name to delete (e.g. "ipsec@client3")
    - failed return tfalse (object not found or not a valid IPsec instance)
    - succeed return ttrue
    - shuts down the instance (swanctl --terminate), unregisters from `network@frame`, and removes configuration

    Example, delete an IPsec client
    ```shell
    ipsec@list.delete[ ipsec@client3 ]
    ttrue
    ```
