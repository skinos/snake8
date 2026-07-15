## ipsec@list — IPsec Client Infrastructure Management

### Overview

Manage the lifecycle of all IPsec VPN client instances using strongSwan. This component scans for configured instances (`ipsec@client`, `ipsec@client2`, … up to `ipsec@client10`) and provides instance creation and deletion APIs.

- schedules setup for all configured IPsec instances at boot/shutdown
- lists all IPsec client configurations and statuses
- creates new IPsec instances with sensible defaults
- deletes existing IPsec instances
- generates strongSwan `swanctl.conf` for each instance



### Architecture

`ipsec@list` is the **infrastructure manager** for IPsec VPN clients. At boot, it iterates all configured `ipsec@client*` instances and schedules each one's setup. Each instance manages its own IPsec tunnel independently via strongSwan's `swanctl` (policy-based IPsec, no dedicated tunnel netdev). `ipsec@list` does not hold its own configuration — all per-instance settings live in `ipsec@client`, `ipsec@client2`, etc.



### API Reference

#### Management APIs

+ `setup[]` **initialize IPsec infrastructure**
    - succeed return ttrue
    - iterates ipsec@client through ipsec@client10, schedules each configured instance setup

+ `shut[]` **shut down all IPsec clients**
    - succeed return ttrue
    - calls shut on each instance, then stops the shared charon service


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
    - succeed return [ json ], all existing instances with their runtime status
    - for each `ipsec@client*` slot, calls that instance's `status[]` and stores the result when non-NULL
    - each instance status is the same payload as [`ipsec@client.status`](client.md)
    ```json
    {
        "ipsec@client":                     // [ string ]: { json }, instance name
        {
            "status":"Current state",       // [ "disable", "connecting", "established", "down" ]
            "serverip":"server IP",         // [ ip ], resolved gateway or remote-host
            "ip":"virtual IP",              // [ ip ], local-vips
            "local_id":"local IKE ID",
            "remote_id":"remote IKE ID",
            "local_host":"local outer IP",
            "remote_host":"remote outer IP",
            "ike_state":"IKE SA state",     // e.g. ESTABLISHED
            "child_state":"CHILD SA state", // e.g. INSTALLED
            "ike_version":"2",
            "ike_proposal":"IKE proposal",
            "esp_proposal":"ESP proposal",
            "local_ts":"local traffic selector",
            "remote_ts":"remote traffic selector",
            "rx_bytes":"bytes in",
            "tx_bytes":"bytes out",
            "rx_packets":"packets in",
            "tx_packets":"packets out",
            "established":"IKE age seconds",
            "livetime":"online time",       // hour:minute:second:day
            "install_time":"CHILD age seconds",
            "spi_in":"inbound SPI",
            "spi_out":"outbound SPI",
            "mode":"TUNNEL",
            "protocol":"ESP",
            "reqid":"reqid"
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
            "ip":"10.10.10.2",
            "local_id":"ipsec-client",
            "remote_id":"ipsec-server",
            "ike_state":"ESTABLISHED",
            "child_state":"INSTALLED",
            "rx_bytes":"924",
            "tx_bytes":"0",
            "livetime":"00:03:27:0"
        }
    }
    ```


#### Control APIs

+ `add[ server ]` **add a new IPsec client**
    - server ------------ [ string ], IPsec gateway address (IP or domain)
    - failed return NULL (all 10 slots full)
    - succeed return [ string ], the new instance name (e.g. "ipsec@client3")
    - creates with defaults: status=disable, extern=default, version=2, auth_method=psk, dpd_delay=30s

    Example, add an IPsec client
    ```shell
    ipsec@list.add[ 203.0.113.1 ]
    ipsec@client3
    ```

+ `delete[ object ]` **delete an IPsec client**
    - object ------------ [ string ], the instance name to delete (e.g. "ipsec@client3")
    - failed return tfalse (object not found or not a valid IPsec instance)
    - succeed return ttrue
    - shuts down the instance (swanctl --terminate) and removes configuration

    Example, delete an IPsec client
    ```shell
    ipsec@list.delete[ ipsec@client3 ]
    ttrue
    ```
