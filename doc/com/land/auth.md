

***
## Username/Password and Permission Management

Manage username and permissions, modifying this configuration directly is not recommended, It is recommended to manage through the method
The configuration structure is divided into three layers
- username can belong to multiple groups
- group can have permissions of domains
- because the three-layer structure is relatively complex, the group and domain permissions are temporarily one-to-one
    > that is, there are only username and groups, and each group has a separate domain permission
- groups are pre-built in the system (built-in based on functions), and corresponding username can be created and deleted.
    > Examples storage functions are added, "nas" group are built in the storage. you can add or delete username belonging to "nas" group


#### **Configuration( land@auth )**

```json
// Attributes introduction 
{
    "user":        //  username list, all accounts in the system are under this node
    {
        "user name":                  // [ string ], you can custom the username
        {
            "id":"username identify number",                        // [ number ]
            "key":"username password",                              // [ string ]
            "key_check":"key need strength",                        // [ "disable", "enable" ]
            "key_failed_time":"Number of consecutive failures",     // [ number ]
            "key_failed_wait":"failures to block",                  // [ number ], the unit is second

            "group":                     // List of groups to which the username belongs
            {
                "group name":"belongs state"  // [ string ]: [ "disable","enable" ], "enable" for belongs, "disable" for not
                // "...":"..."                // How many groups belongs show how many properties
            },
            "domain":                    // List of all domains configure for this username
            {
                "domain name":
                {
                    "key":"In this domain specialized password"        // [ string ], an empty one represents the use of the default password
                }
                // "...":{ ... }                                  // How many domain show how many properties
            }
        },
        // "...":{...}                  // How many username show how many properties
    },
    "group":     // Group list, all groups in the system are under this node
    {
        "group name":
        {
            "id":"group identify",     // [ number ]
            "domain":                  // List of all domain permissions under the group
            {
                "domain name":"enable state"  // [ string ]: [ "disable","enable" ]
                // "...":"..."             // How many domain show how many properties
            }
        },
        // "...":{...}                  // How many group show how many properties
    }
}
```

Examples, show all the accounts configure
```shell
land@auth
{
    "user":
    {
        "admin":                       # username: admin
        {
            "id":"0",                  # admin username id is 0
            "key":"E@3DLKSLKJWEWWWWW", # admin default password is E@3DLKSLKJWEWWWWW
            "group":                   # admin belongs groups: admin, web, tui, nas
            {
                "admin":"enable",
                "web":"enable",
                "tui":"enable",
                "nas":"enable"
            },
            "domain":
            {
                "nas":                 # admin password is ADSAADFFF at the nas domain
                {
                    "key":"ADSAADFFF"
                },
                "web":                 # admin password is DSDFSDFF at the web domain
                {
                    "key":"DSDFSDFF"
                }
            }
        },
        "nas":                         # username: nas
        {
            "id":"1000",               # nas username id is 1000
            "key":"nas",               # nas default password is nas
            "group":                   # admin belongs group: nas
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
            "domain":                  # admin group belongs admin domain
            {
                "admin":"enable"
            }
        },
        "web":                         # web group
        {
            "id":"1000",               # web group identify is 1000
            "domain":                  # web group belongs web domain
            {
                "web":"enable"
            }
        },
        "tui":                         # tui group
        {
            "id":"1001",               # tui group identify is 0
            "domain":                  # tui group belongs tui domain
            {
                "tui":"enable"
            }
        },
        "nas":                         # nas group
        {
            "id":"1002",               # nas group identify is 0
            "domain":                  # nas group belongs nas domain
            {
                "nas":"enable"
            }
        }
    }
}
```  


#### **API( land@auth )**

+ `domain[ username ]` **list domain belongs of username**   
    - username ----------- [ string ]
    - failed return NULL
    - return json to list the daemon   
    ```json
    // Attributes introduction of json by the method return
    {
        "domain name":"domain state"   // [ string ]:[ "disable", "enable" ]
        // "...":"..."                 // How many domain show how many properties
    }    
    ```

    Example, list all domain of username admin
    ```shell
    land@auth.domain[ admin ]
    {
        "admin":"enable",
        "tui":"enable",
        "nas":"enable"
    }
    ```


+ `check[ [domain], username, password ]` **check the username and password correct**   
    - domain ----------- [ string ],  specify a specific domain, default is common
    - username ----------- [ string ] 
    - password ----------- [ string ] 
    - failed return tfalse
    - succeed return ttrue

    Example, check the username admin correct
    ```shell
    land@auth.check[ ,admin, admin ]
    ttrue
    ```

    Example, check the username admin correct with wrong password
    ```
    land@auth.check[ ,admin, passwrong ]
    tfalse
    ```

+ `match[ [domain], username, ciphertext ]` **check the username and cryptographic password correct**   
    - domain ----------- [ string ],  specify a specific domain, default is common
    - username ----------- [ string ] 
    - ciphertext ----------- [ string ], cryptographic password
    - failed return tfalse
    - succeed return ttrue

    Example, check the username admin correct
    ```shell
    land@auth.check[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM= ]
    ttrue
    ```

    Example, check the username admin correct with wrong password
    ```
    land@auth.check[ ,admin, admin ]
    tfalse
    ```


+ `modify[ [domain], username, password, [new password], [new username] ]` **modify the username or password with original password**   
    - domain ----------- [ string ],  specify a specific domain, default is common
    - username ----------- [ string ] 
    - password ----------- [ string ] 
    - new password ----------- [ string ] 
    - new username ----------- [ string ]
    - failed return tfalse
    - succeed return ttrue

    Example, modify the password of admin to 12345
    ```shell
    land@auth.modify[ ,admin, admin, 12345 ]
    ttrue
    ```
    Example, modify the username of admin to Bob
    ```
    land@auth.modify[ ,admin, 12345, , Bob ]
    ttrue
    ```

+ `change[ [domain], username, ciphertext, [new password], [new username] ]` **modify the username or password with original cryptographic password**   
    - domain ----------- [ string ],  specify a specific domain, default is common
    - username ----------- [ string ] 
    - ciphertext ----------- [ string ], cryptographic password 
    - new password ----------- [ string ] 
    - new username ----------- [ string ]
    - failed return tfalse
    - succeed return ttrue

    Example, change the password of admin to 12345
    ```shell
    land@auth.change[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM=, 12345 ]
    ttrue
    ```
    Example, change the username of admin to Bob
    ```
    land@auth.change[ ,admin, ODI3Y2NiMGVlYThhNzA2YzRjMzRhMTY4OTFmODRlN2I=, , Bob ]
    ttrue
    ```



+ `add[ [domain], username, password ]` **add a new username**   
    - domain ----------- [ string ],  specify a specific domain, default is common
    - username ----------- [ string ] 
    - password ----------- [ string ] 
    - failed return tfalse
    - succeed return ttrue

    Example, add xiaomi username, the password is xiaomin123
    ```shell
    land@auth.add[ ,xiaomi, xiaomin123 ]
    ttrue
    ```

+ `delete[ username[, ...] ]` **delete username**   
    - username ----------- [ string ] 
    - ... ----------- [ string ], you can delete a many username
    - failed return tfalse
    - succeed return ttrue

    Example, delete the username xiaomi
    ```shell
    land@auth.delete[ xiaomi ]
    ttrue
    ```
    
    Example, delete the username Alice and Bob 
    ```
    land@auth.delete[ Alice, Bob ]
    ttrue
    ```

+ `list[ [group] ]` **list system username**   
    - group ----------- [ string ]
    - failed return NULL
    - return json to describes   
    ```json
    // Attributes introduction of json by the method return
    {
        "user name":                 // [ string ]
        {
            "key":"username password"         // [ string ]
        }
        // "...":{...}                  // How many username show how many properties
    }    
    ```

    Example, list all admin group username
    ```shell
    land@auth.list[ admin ]
    {
        "admin":
        {
            "key":"E@3DLKSLKJWEWWWWW"
        }
    }
    ```

    Example, list all username
    ```
    land@auth.list
    {
        "admin":
        {
            "key":"E@3DLKSLKJWEWWWWW"
        },
        "nas":
        {
            "key":"nas"
        },
        "nobody":
        {
        }
    }
    ```

