## land@auth — User & Permission Management

### Overview

Manage usernames and permissions. Prefer the **Component API** below instead of editing raw configuration when possible.
The configuration structure is divided into three layers
- username can belong to multiple groups
- group can have permissions of domains
- because the three-layer structure is relatively complex, the group and domain permissions are temporarily one-to-one
    > that is, there are only username and groups, and each group has a separate domain permission
- groups are pre-built in the system (built-in based on functions), and corresponding username can be created and deleted.
    > Examples storage functions are added, "nas" group are built in the storage. you can add or delete username belonging to "nas" group



### Configuration reference ( land@auth )

```json
// Attributes introduction 
{
    "user":          // [ json ], username list, all accounts in the system are under this json
    {
        "user name":       // [ string ]: { json }, username in system
        {                      // username configure in this json
            "id":"username identify number",                        // [ number ], linux system user id number
            "key":"username password",                              // [ string ], encrypt password, cannot be space
            "key_check":"key need strength",                        // [ "disable", "enable" ], default be "disable"
            "key_failed_time":"Number of consecutive failures",     // [ number ], default be 0
            "key_failed_wait":"failures to block",                  // [ number ], default be 5, the unit is second

            "group":                     // [ json ], List of groups to which the username belongs
            {
                "group name":"belongs state"  // [ string ]: [ "disable","enable" ], group name the the username belongs
                                                    // "enable" for belongs, "disable" for not
                // "...":"..."  How many groups belongs show how many properties
            },
            "domain":                    // [ json ], List of all domains configure for this username
            {
                "domain name":                // [ string ]: { json }, domain name the the username belongs
                {                                   // Domain-specific properties
                    "key":"In this domain specialized password"        // [ string ], an empty one represents the use of the default password
                }
                // "...":{ ... }  How many domain show how many properties
            }
        }
        // "...":{...}  How many username show how many properties
    },
    "group":     // [ json ], Group list, all groups in the system are under this node
    {
        "group name":      // [ string ]: { json }, group in system
        {                      // group configure in this json
            "id":"group identify",     // [ number ], linux system group id number
            "domain":                    // [ json ], List of all domains configure for this group
            {
                "group name":"belongs state"  // [ string ]: [ "disable","enable" ], group name the the group belongs
                // "...":"..."  How many domain show how many properties
            }
        }
        // "...":{...}  How many group show how many properties
    }
}
```

#### Configuration example

Example, show all the accounts configure
```shell
land@auth                                # enter this
{                                        # return this
    "user":
    {
        "admin":                         # username: admin
        {
            "id":"0",                         # admin username id is 0
            "key":"eYgJU9Koun1yPYJ78JeH2Q==", # admin default domain encode password
            "group":                          # admin belongs groups: admin, nas, vpn
            {
                "admin":"enable",
                "vpn":"enable",
                "nas":"enable"
            },
            "domain":
            {
                "admin":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="       # admin domain encode password
                },
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="       # nas domain encode password
                }
            }
        },
        "eason":                         # username: eason
        {
            "id":"1000",                      # eason username id is 1000
            "key":"CL088bD9dcJUgNzhCKBnfg==", # eason default domain encode password
            "group":                          # eason belongs group: nas
            {
                "nas":"enable"
            }
        }
    },
    "group":
    {
        "admin":                       # admin group
        {
            "id":"0",                  # admin group identify is 0
            "domain":                  # admin group has admin domain permissions
            {
                "admin":"enable"
            }
        },
        "vpn":                         # tui group
        {
            "id":"1001",               # tui group identify is 1001
            "domain":                  # tui group has tui domain permissions
            {
                "vpn":"enable"
            }
        },
        "nas":                         # nas group
        {
            "id":"1002",               # nas group identify is 1002
            "domain":                  # nas group has nas domain permissions
            {
                "nas":"enable"
            }
        }
    }
}
```

#### Configuration settings example

Example, enable the password check for admin
```shell
land@auth:user/admin/key_check=enable
ttrue
```

Example, set admin's number of consecutive failures be 3
```shell
land@auth:user/admin/key_failed_time=3
ttrue
```

Example, merge set the password check for admin( include "key_check" "key_failed_time" "key_failed_wait" )
```shell
land@auth|{"user":{"admin":{"key_check":"enable","key_failed_time":"4","key_failed_wait":"10"}}}
ttrue
```



### API Reference

#### Management APIs

+ `setup[]` **initialize the auth component**, *succeed return ttrue, failed return tfalse, error return terror*
    - failed return tfalse
    - succeed return ttrue
    - This is a lifecycle method called automatically by the system during startup
    - Not intended for manual invocation


#### Query APIs

+ `domain[ username ]` **list all domain for the user's enabled groups**   
    - username ----------- [ string ]
    - failed return NULL
    - succeed return [ json ], all domain "username" belong in the json
    ```json
    {
        "domain name": "enable or disable"  // [ string ]: [ "enable", "disable" ], domain name in system
                                                    // "enable": have this permissions
                                                    // "disable": no this permissions
        // "...":"..."  How many domain belongs show how many properties
    }
    ```

    Example, list all admin's domain
    ```shell
    land@auth.domain[ admin ]
    {
        "admin":"enable",    # admin have the admin domain permissions
        "nas":"enable",      # admin have the nas domain permissions
        "vpn":"enable"       # admin have the vpn domain permissions
    }
    ```

+ `check[ domain, username, password ]` **check the username and password correct**   
    - domain ------------- [ string ], optional, specify a specific domain, default is common
    - username ----------- [ string ] 
    - password ----------- [ string ], text password
    - failed return tfalse
    - succeed return ttrue

    Example, check the username admin correct
    ```shell
    land@auth.check[ ,admin, admin ]
    ttrue
    ```

    Example, check the username admin correct with wrong password
    ```shell
    land@auth.check[ ,admin, passwrong ]
    tfalse
    ```

+ `match[ domain, username, ciphertext ]` **check the username and cryptographic password correct**   
    - domain ----------------- [ string ], optional, specify a specific domain, default is common
    - username --------------- [ string ] 
    - ciphertext ------------- [ string ], cryptographic password
    - failed return tfalse
    - succeed return ttrue

    Example, check the username admin correct
    ```shell
    land@auth.match[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM= ]
    ttrue
    ```

    Example, check the username admin correct with wrong password
    ```shell
    land@auth.match[ ,admin, admin ]
    tfalse
    ```

+ `list[ group ]` **list users**   
    - group ----------- [ string ], optional, when set, only user in this group
    - failed return NULL
    - succeed return [ json ], all user with its key in the json
    ```json
    {
        "user name":     // [ string ]: { json }, system username
        {                     // username's key in this json
            "key":"password string",    // [ string ]
        }
        // "...":{}  How many user show how many properties        
    }
    ```

    Example, list all username
    ```shell
    land@auth.list
    {
        "admin":
        {
            "key":"eYgJU9Koun1yPYJ78JeH2Q=="   # admin's key
        },
        "eason":
        {
            "key":"CL088bD9dcJUgNzhCKBnfg=="   # eason's key
        }    
    }
    ```

    Example, list all admin group username
    ```shell
    land@auth.list[ admin ]
    {
        "admin":
        {
            "key":"eYgJU9Koun1yPYJ78JeH2Q=="   # admin's key
        }
    }
    ```

#### Control APIs

+ `modify[ domain, username, password, [new password], [new username] ]` **modify the username or password with original password**   
    - domain ----------------- [ string ], optional, specify a specific domain, default is common
    - username --------------- [ string ] 
    - password --------------- [ string ], text password
    - new password ----------- [ string ], optional, but "new password" and "new username" must have one of them
    - new username ----------- [ string ], optional, but "new password" and "new username" must have one of them
    - failed return tfalse
    - succeed return ttrue

    Example, modify the password of admin to 12345
    ```shell
    land@auth.modify[ ,admin, admin, 12345 ]
    ttrue
    ```

    Example, modify the username of admin to Bob
    ```shell
    land@auth.modify[ ,admin, 12345, , Bob ]
    ttrue
    ```

+ `change[ domain, username, ciphertext, [new password], [new username] ]` **modify the username or password with original cryptographic password**   
    - domain ----------------- [ string ], optional, specify a specific domain, default is common
    - username --------------- [ string ] 
    - ciphertext ------------- [ string ], cryptographic password 
    - new password ----------- [ string ], optional, but "new password" and "new username" must have one of them
    - new username ----------- [ string ], optional, but "new password" and "new username" must have one of them
    - failed return tfalse
    - succeed return ttrue

    Example, change the password of admin to 12345
    ```shell
    land@auth.change[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM=, 12345 ]
    ttrue
    ```

    Example, change the username of admin to Bob
    ```shell
    land@auth.change[ ,admin, ODI3Y2NiMGVlYThhNzA2YzRjMzRhMTY4OTFmODRlN2I=, , Bob ]
    ttrue
    ```

+ `add[ domain, username, password, group, ... ]` **add a new username**   
    - domain -------------- [ string ], optional slot used when storing a domain-specific key (`user/<name>/domain/<domain>/key`)
    - username ------------ [ string ]
    - password ------------ [ string ]
    - group, ... ---------- [ string, ... ], optional 4th, 5th, … parameters: each non-NULL name gets **`"enable"`** in the new user's **`group`** map
    - failed return tfalse
    - succeed return ttrue

    Example, add xiaomi username, the password is xiaomin123
    ```shell
    land@auth.add[ ,xiaomi, xiaomin123 ]
    ttrue
    ```

+ `delete[ username, ... ]` **delete username**   
    - username, ... ----------- [ string ], you can delete a many username
    - failed return tfalse
    - succeed return ttrue

    Example, delete the username xiaomi
    ```shell
    land@auth.delete[ xiaomi ]
    ttrue
    ```
    
    Example, delete the username Alice and Bob 
    ```shell
    land@auth.delete[ Alice, Bob ]
    ttrue
    ```



### Published Joint Events

The following joint events are published when authentication data changes. Other components can subscribe at runtime (joint registration / **land@joint**).

| Event | Description |
|-------|-------------|
| `auth/modify` | Sent when authentication configuration or user accounts are modified. Triggered after `set` (config change), `add` (new account), `delete` (account removal), `modify` (password/username change), or `change` (password change via ciphertext). The event parameter is the affected account name, or `NULL` when the entire config was changed. |



### Other

Some helpful APIs

+ `md5[ string ]` **calculate MD5 hash of string**
    - string ----------- [ string ], the string to hash
    - failed return NULL
    - succeed return [ string ], the MD5 hash string (32 characters hex)

    Example, calculate MD5 of "admin"
    ```shell
    land@auth.md5[ admin ]
    21232f297a57a5a743894a0e4a801fc3
    ```

+ `b64_encode[ string ]` **Base64 encode a string**
    - string ----------- [ string ], the string to encode
    - failed return NULL
    - succeed return [ string ], the Base64 encoded string

    Example, Base64 encode "admin"
    ```shell
    land@auth.b64_encode[ admin ]
    YWRtaW4=
    ```

+ `b64_decode[ string ]` **Base64 decode a string**
    - string ----------- [ string ], the Base64 string to decode
    - failed return NULL
    - succeed return [ string ], the decoded string

    Example, Base64 decode "YWRtaW4="
    ```shell
    land@auth.b64_decode[ YWRtaW4= ]
    admin
    ```

+ `encode[ string, [key] ]` **simple encode a string**
    - string ----------- [ string ], the string to encode
    - key -------------- [ string ], optional token passed; omitted / NULL uses the implementation default
    - failed return NULL
    - succeed return [ string ], the encoded string

    Example, encode "admin"
    ```shell
    land@auth.encode[ admin ]
    eYgJU9Koun1yPYJ78JeH2Q==
    ```

+ `decode[ string, [key] ]` **simple decode a string**
    - string ----------- [ string ], the encoded string to decode
    - key -------------- [ string ], optional token (must match **`encode`**)
    - failed return NULL
    - succeed return [ string ], the decoded string

    Example, decode "eYgJU9Koun1yPYJ78JeH2Q=="
    ```shell
    land@auth.decode[ eYgJU9Koun1yPYJ78JeH2Q== ]
    admin
    ```
