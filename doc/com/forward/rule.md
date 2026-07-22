## forward@rule — Policy Based Routing

### Overview

Manage policy based routing (`ip rule`). Prefer the **Component API** below instead of editing raw configuration when possible.

- select packets by source address, source interface, and/or mark id, then look up a route table (`tid`)
- default rule priority (`pref`) is **40000** when omitted
    > mark packets with `forward@mark`, then match them here with `markid`



### Configuration reference ( forward@rule )

```json
// Attributes introduction 
{
    "rule name":                                               // [ string ]: { json }, user can custom the rule name
    {
        "pref":"rule's priority",                                      // [ number ], range of 0-4294967295, The smaller, the higher
                                                                                // 0 for local table priority
                                                                                // 32766 for main table priority
                                                                                // 50000 for default table priority
                                                                                // default is 40000

        "markid":"select the packet use markid",                       // [ number ], range of 1-4294967295
                                                                                // Less than 100 is reserved for the system
                                                                                // You are advised to use more than 100 for user-defined rules
        "srcifname":"select the packet use source interface",          // [ "ifname@lan", "ifname@lan2", ... ], interface name
        "src":"select the packet use source ip address",               // [ ip address, network ]
        "srcmask":"select the packet use source mask of ip address",   // [ netmask ], necessary when "src" be network

        "tid":"which route table to go to"                             // [ number ], range of 0-255
                                                                                // 0 for local table
                                                                                // 253 for default table
                                                                                // 254 for main table
                                                                                // 255 for local table
                                                                                // Less than 100 is reserved for the system
                                                                                // You are advised to use more than 100 for user-defined rules(And avoid 253,254,255)
    }
    // "...":{ ... }  How many rule show how many properties
}
```

#### Configuration example

Example, show all the policy rule configure

```shell
forward@rule
{
    "myCustom1":                       # rule name is myCustom1
    {                                  # packet from ifname@lan and source address is 1.1.1.1 route to route table 101, priority is 38000
        "pref":"38000",
        "srcifname":"ifname@lan",
        "src":"1.1.1.1",
        "srcmask":"255.255.255.255",
        "tid":"101"
    },
    "youCustom":                       # rule name is youCustom
    {                                  # packet from ifname@lan and markid is 300 route to route table 102, default priority is 40000
        "srcifname":"ifname@lan",
        "markid":"300",
        "tid":"102"
    }
}
```

#### Configuration settings example

Example, merge set one policy rule

```shell
forward@rule|{"myCustom1":{"pref":"38000","srcifname":"ifname@lan","src":"1.1.1.1","srcmask":"255.255.255.255","tid":"101"}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **apply saved policy rules**
    - succeed return ttrue
    - normally scheduled as **`init` → `app` → `forward@rule.setup`**

+ `shut[]` **remove saved policy rules from the kernel**
    - succeed return ttrue

+ `add[ name, [src], [srcmask], [srcifname], [markid], tid, [pref] ]` **add policy rule**
    - name ----------- [ string ], rule name
    - src ------------ [ ip address, network ], optional
    - srcmask -------- [ netmask ], optional
    - srcifname ------ [ "ifname@lan", ... ], optional
    - markid --------- [ number ], optional
    - tid ------------ [ number ], route table id
    - pref ----------- [ number ], optional, default 40000
    - succeed return ttrue
    - failed return tfalse

    Example, add a rule named senser, make that source address 192.168.2.12 route to route table 1, priority is 33000

    ```shell
    forward@rule.add[ senser, 192.168.2.12, , , , 1, 33000 ]
    ttrue
    ```

    Example, add a rule named video, make all other acess route to route table 2, priority is 33300

    ```shell
    forward@rule.add[ video, , , , , 2, 33000 ]
    ttrue
    ```

+ `delete[ name ]` **delete policy rule**
    - name ----------- [ string ], rule name
    - succeed return ttrue
    - failed return tfalse

    Example, delete the policy rule named video

    ```shell
    forward@rule.delete[ video ]
    ttrue
    ```

+ `delete[ , [src], [srcmask], [srcifname], [markid], tid, [pref] ]` **delete kernel rule by fields**
    - used for system rules named `~autoN` from `status`
    - succeed return ttrue
    - failed return tfalse


#### Query APIs

+ `status[]` **get current policy rules (kernel + configure)**
    - failed return NULL
    - succeed return [ json ]
    - rules present in the kernel are listed; names come from configure when matched, otherwise auto names like `~auto0`
    - remaining configure-only rules (not in kernel yet) are also included
    ```json
    {
        "rule name":                                                 // [ string ]: { json }, configure name or ~autoN
        {
            "pref":"rule's priority",                                      // [ number ], range of 0-4294967295, The smaller, the higher
                                                                                    // default is 40000
            "markid":"select the packet use markid",                       // [ number ], range of 1-4294967295
            "srcifname":"select the packet use source interface",          // [ "ifname@lan", "ifname@lan2", ... ], ifname
            "src":"select the packet use source ip address",               // [ ip address, network ]
            "srcmask":"select the packet use source mask of ip address",   // [ netmask ], necessary when "src" be network
            "tid":"which route table to go to"                             // [ number ], range of 0-255
        }
        // "...":{ ... }  How many rule show how many properties
    }
    ```

    Example, get the current policy rule

    ```shell
    forward@rule.status
    {
        "~auto0":
        {
            "pref":"0",
            "tid":"255"
        },
        "~auto1":
        {
            "pref":"32766",
            "tid":"254"
        },
        "~auto2":
        {
            "pref":"32767",
            "tid":"253"
        },
        "myCustom1":
        {
            "srcifname":"ifname@lan",
            "src":"1.1.1.1",
            "srcmask":"255.255.255.255",
            "tid":"101",
            "pref":"38000"
        },
        "youCustom":
        {
            "srcifname":"ifname@lan",
            "markid":"300",
            "tid":"102",
            "pref":"40000"
        }
    }
    ```

+ `tidlist[]` **list known route tables**
    - failed return NULL
    - succeed return [ json ], table id to table name
    ```json
    {
        "table id":"table name"                             // [ string ]: [ string ]
        // "...":"..."  How many tid show how many properties
    }
    ```
    - merges `/etc/iproute2/rt_tables` with `forward@routes.list`

    Example, get the current route table list

    ```shell
    forward@rule.tidlist
    {
        "1":"1",
        "2":"2",
        "253":"default",
        "254":"main",
        "255":"local"
    }
    ```
