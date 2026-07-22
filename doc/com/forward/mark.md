## forward@mark — Mark the Packet

### Overview

Mark matching packets with a mark id (iptables mangle `PREROUTING`). Prefer the **Component API** below instead of editing raw configuration when possible.

- mark id less than 100 is reserved for the system; use 100+ for user rules
- works with `forward@rule` via `markid` for policy routing



### Configuration reference ( forward@mark )

```json
// Attributes introduction 
{
    "rule name":                    // [ string ]: { json }, user can custom the rule name
    {
        "markid":"mark the packet with this id",                       // [ number ], range of 1-4294967295
                                                                            // Less than 100 is reserved for the system
                                                                            // You are advised to use more than 100 for user-defined rules
        "src":"select the packet use source ip address",               // [ ip address ]
        "dest":"select the packet use destination ip address",         // [ ip address ]
        "protocol":"protocol type",                                    // [ "all", "tcp", "udp", "tcpudp" ], all for all protocol, tcpudp for tcp and udp
        "srcport":"source port",                                       // [ number ], default is all source port
        "destport":"destination port"                                  // [ number ], default is all destination port
    }
    // "...":{ ... }  How many rule show how many properties
}
```

#### Configuration example

Example, show all the mark rule configure

```shell
forward@mark
{
    "myCustom1":                       # rule name is myCustom1
    {                                  # mark that source address is 192.168.8.250 and destination address is 202.94.22.38 packet 300
        "markid":"300",
        "src":"192.168.8.250",
        "dest":"202.94.22.38"
    },
    "youCustom":                       # rule name is youCustom
    {                                  # mark that source address is 192.168.8.251 and tcp protocol source port is 1000-2000 and destination port is 80/8080 packet 301
        "markid":"301",
        "src":"192.168.8.251",
        "protocol":"tcp",
        "srcport":"1000-2000",
        "destport":"80,8080"
    }
}
```

#### Configuration settings example

Example, merge set one mark rule

```shell
forward@mark|{"myCustom1":{"markid":"300","src":"192.168.8.250","dest":"202.94.22.38"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **apply saved mark rules into mangle PREROUTING**
    - succeed return ttrue
    - normally scheduled as **`init` → `app` → `forward@mark.setup`**

+ `add[ name, markid, [src], [dest], [protocol], [srcport], [destport] ]` **add mark rule**
    - name ----------- [ string ], rule name
    - markid --------- [ number ]
    - src ------------ [ ip address ], optional
    - dest ----------- [ ip address ], optional
    - protocol ------- [ "all", "tcp", "udp", "tcpudp" ], optional
    - srcport -------- [ string ], optional
    - destport ------- [ string ], optional
    - succeed return ttrue
    - failed return tfalse

    Example, add a mark rule

    ```shell
    forward@mark.add[ myCustom1, 300, 192.168.8.250, 202.94.22.38 ]
    ttrue
    ```

+ `delete[ name ]` **delete mark rule**
    - name ----------- [ string ], rule name
    - succeed return ttrue
    - failed return tfalse

    Example, delete a mark rule

    ```shell
    forward@mark.delete[ myCustom1 ]
    ttrue
    ```


#### Query APIs

+ `status[]` **get the current mark rule configure**
    - failed return NULL
    - succeed return [ json ], same shape as configuration

    Example, get mark rules

    ```shell
    forward@mark.status
    {
        "myCustom1":
        {
            "markid":"300",
            "src":"192.168.8.250",
            "dest":"202.94.22.38"
        }
    }
    ```
