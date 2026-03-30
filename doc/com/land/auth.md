## land@auth — User & Permission Management

Manage usernames and permissions. Prefer the **Component API** below instead of editing raw configuration when possible.
The configuration structure is divided into three layers
- username can belong to multiple groups
- group can have permissions of domains
- because the three-layer structure is relatively complex, the group and domain permissions are temporarily one-to-one
    > that is, there are only username and groups, and each group has a separate domain permission
- groups are pre-built in the system (built-in based on functions), and corresponding username can be created and deleted.
    > Examples storage functions are added, "nas" group are built in the storage. you can add or delete username belonging to "nas" group


### Configuration ( `land@auth` )

The **saved configuration object** for `land@auth` (query/set via `land@auth`, paths under `user/` and `group/`, merge `|{json}`, etc.). Direct edits are easy to get wrong; use APIs for routine account work.

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
        }
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
        }
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

Examples, merge a small subset of fields (illustrative; prefer APIs for real account work)
```shell
land@auth|{"user":{"admin":{"key_check":"enable"}}}
ttrue
```

### Component API

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
    land@auth.match[ ,admin, MjEyMzJmMjk3YTU3YTVhNzQzODk0YTBlNGE4MDFmYzM= ]
    ttrue
    ```

    Example, check the username admin correct with wrong password
    ```
    land@auth.match[ ,admin, admin ]
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
            "key":"eYgJU9Koun1yPYJ78JeH2Q==",
            "domain":
            {
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="
                },
                "wui":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="
                }
            }
        }
    }
    ```

    Example, list all username
    ```
    land@auth.list
    {
        "admin":
        {
            "key":"eYgJU9Koun1yPYJ78JeH2Q==",
            "domain":
            {
                "nas":
                {
                    "key":"CL088bD9dcJUgNzhCKBnfg=="
                },
                "wui":
                {
                    "key":"pTxxKkPm+ezb9w/wowVxSg=="
                }
            }
        }
        "eason":
        {
            "key":"CL088bD9dcJUgNzhCKBnfg=="
        }
    }
    ```


+ `md5[ string ]` **calculate MD5 hash of string**
    - string ----------- [ string ], the string to hash
    - failed return NULL
    - return the MD5 hash string (32 characters hex)

    Example, calculate MD5 of "admin"
    ```shell
    land@auth.md5[ admin ]
    21232f297a57a5a743894a0e4a801fc3
    ```

+ `b64_encode[ string ]` **Base64 encode a string**
    - string ----------- [ string ], the string to encode
    - failed return NULL
    - return the Base64 encoded string

    Example, Base64 encode "admin"
    ```shell
    land@auth.b64_encode[ admin ]
    YWRtaW4=
    ```

+ `b64_decode[ string ]` **Base64 decode a string**
    - string ----------- [ string ], the Base64 string to decode
    - failed return NULL
    - return the decoded string

    Example, Base64 decode "YWRtaW4="
    ```shell
    land@auth.b64_decode[ YWRtaW4= ]
    admin
    ```

+ `encode[ string, [key] ]` **simple encode a string**
    - string ----------- [ string ], the string to encode
    - key -------------- [ string ], optional encryption key; if omitted, uses the system default key
    - failed return NULL
    - return the encoded string

    Example, encode "admin"
    ```shell
    land@auth.encode[ admin ]
    eYgJU9Koun1yPYJ78JeH2Q==
    ```

+ `decode[ string, [key] ]` **simple decode a string**
    - string ----------- [ string ], the encoded string to decode
    - key -------------- [ string ], optional decryption key; if omitted, uses the system default key
    - failed return NULL
    - return the decoded string

    Example, decode "eYgJU9Koun1yPYJ78JeH2Q=="
    ```shell
    land@auth.decode[ eYgJU9Koun1yPYJ78JeH2Q== ]
    admin
    ```

### Lifecycle API


+ `setup[]` **initialize the auth component**, *succeed return ttrue, failed return tfalse, error return terror*
    - This is a lifecycle method called automatically by the system during startup
    - It refreshes the authentication configuration and initializes user/group settings
    - Not intended for manual invocation


### Published Joint Events

The following joint events are published when authentication data changes. Other components can subscribe at runtime (joint registration / **`land@joint`**).

| Event | Description |
|-------|-------------|
| `auth/modify` | Sent when authentication configuration or user accounts are modified. Triggered after `set` (config change), `add` (new account), `delete` (account removal), `modify` (password/username change), or `change` (password change via ciphertext). The event parameter is the affected account name, or `NULL` when the entire config was changed. |


### C Code Example

**Read and update configuration**

```c
#include "skin/skin.h"

static int auth_config_get_and_set(void)
{
    char key_check[32];
    talk_t user_node;
    talk_t domain_key;
    boole ok;

    /* 1) Read one string config value */
    if (sgets_string(key_check, sizeof(key_check), "land@auth", "user/admin/key_check") == NULL)
    {
        return -1;
    }
    printf("admin key_check=%s\n", key_check);

    /* 2) Read one object node */
    user_node = sgets("land@auth", "user/admin");
    if (user_node == NULL || user_node <= tpanic)
    {
        return -1;
    }
    printf("admin default key=%s\n", json_string(user_node, "key"));
    talk_free(user_node);

    /* 3) Update string config values */
    ok = ssets_string("land@auth", "enable", "user/admin/key_check");
    if (ok == false)
    {
        return -1;
    }
    ok = ssets_string("land@auth", "disable", "user/admin/group/vpn");
    if (ok == false)
    {
        return -1;
    }

    /* 4) Update one domain password value with talk/json interface */
    domain_key = string2x("new_encoded_key_here");
    ok = ssets("land@auth", domain_key, "user/admin/domain/wui/key");
    talk_free(domain_key);
    if (ok == false)
    {
        return -1;
    }

    return 0;
}
```

Notes:
- Use `sgets_string()` / `sgets()` to read auth configuration paths.
- Use `ssets_string()` / `ssets()` to update auth configuration paths.
- Any returned `talk_t` that holds JSON (or other heap-owned graph nodes) must be released with `talk_free()` when you are done; `he`-only operators normally do not call these C APIs directly.

**Call component methods**

```c
#include "skin/skin.h"

static void print_auth_call_error(const char *api, talk_t ret)
{
    if (ret == tfalse || ret == terror || ret == tpanic)
    {
        printf("%s failed, errno=%d\n", api, errno);
    }
}
```

##### `domain[ username ]`

```c
talk_t ret = scalls("land@auth", "domain", "admin");
if (ret > tpanic)
{
    printf("admin domain list ready\n");
    talk_free(ret);
}
else print_auth_call_error("domain", ret);
```

##### `check[ [domain], username, password ]`

```c
talk_t ret = scalls("land@auth", "check", "wui,admin,admin_password");
if (ret != ttrue) print_auth_call_error("check", ret);
```

##### `match[ [domain], username, ciphertext ]`

```c
const char *ciphertext = "CIPHER_KEY_TEXT";
talk_t ret = scalls("land@auth", "match", "wui,admin,%s", ciphertext);
if (ret != ttrue) print_auth_call_error("match", ret);
```

##### `modify[ [domain], username, password, [new password], [new username] ]`

```c
talk_t ret = scalls("land@auth", "modify", "wui,admin,oldpass,newpass,admin2");
if (ret != ttrue) print_auth_call_error("modify", ret);
```

##### `change[ [domain], username, ciphertext, [new password], [new username] ]`

```c
const char *ciphertext = "CIPHER_KEY_TEXT";
talk_t ret = scalls("land@auth", "change", "wui,admin,%s,newpass,admin2", ciphertext);
if (ret != ttrue) print_auth_call_error("change", ret);
```

##### `add[ [domain], username, password ]`

```c
talk_t ret = scalls("land@auth", "add", "wui,alice,alice_password");
if (ret != ttrue) print_auth_call_error("add", ret);
```

##### `delete[ username[, ...] ]`

```c
talk_t ret = scalls("land@auth", "delete", "alice,bob");
if (ret != ttrue) print_auth_call_error("delete", ret);
```

##### `list[ [group] ]`

```c
talk_t ret = scalls("land@auth", "list", "admin");
if (ret > tpanic)
{
    printf("admin users json ready\n");
    talk_free(ret);
}
else print_auth_call_error("list", ret);
```

