## center@ctrl — Admin control APIs

### Overview

Admin-only HE APIs for managing cloud usernames (create, list, modify profile, reset password, delete) and privileged gateway diagnostics.

- Intended for device admin WUI (`user.html`) and operator `he` / eline
- Must **not** be listed in `center@userwui` `helist` / `publist` so cloud user pages cannot call these methods
- Cloud login and self-service stay on `center@api` (see `api.md`)
- Account files live under heport `device_path`: `{device_path}/<username>/config`


### Dependencies

- Requires `center@heport` running so `device_path` is registered


### API Reference

#### Management APIs

**User**

+ `user_add[ user, key, [vcode], [lang], [comment] ]` **create a user (create only)**
    - user ------- [ string ], required; only `A-Z` `a-z` `0-9` `_` `-` (reject `/` `.` space `;` etc.)
    - key -------- [ string ], required plaintext password (stored via `simple_encode`)
    - vcode ------ [ string ], optional device register code
    - lang ------- [ "en", "cn", … ], optional; empty follows system
    - comment ---- [ string ], optional
    - fails if user already exists, key missing, or username has illegal characters
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_add[ ashyelf,Cfw1234BE,,en, TestUser ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```

+ `user_modify[ user, [vcode], [lang], [comment] ]` **change non-password fields**
    - user ------- [ string ], required
    - omitted parameters leave that field unchanged
    - explicit empty string clears `vcode` / `comment`; empty `lang` follows system
    - does not change password
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_modify[ ashyelf,sssss,en, TestUser ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```

+ `user_list[ [user] ]` **list all users or one user**
    - user ------ [ string ], optional; when set, return that user config only
    - when omitted, return all users keyed by name
    - password `key` is stripped
    - failed return NULL
    - succeed return json

    Example, list all
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_list
    {
        "ashyelf":
        {
            "lang":"en",
            "comment":"TestUser"
        }
    }
    dimmalex@CLS:~/snake8$
    ```

+ `user_delete[ user ]` **delete a user tree**
    - user ------ [ string ], required
    - removes `{device_path}/<user>/` entirely
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_delete[ ashyelf ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```

+ `user_reset[ user, newkey ]` **admin reset password**
    - user ------- [ string ], required
    - newkey ----- [ string ], required new plaintext password
    - no old password and no admin password check; access is gated by not exposing this component on userwui helist
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_reset[ ashyelf,NewPass123 ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```

+ `user_match[ , user, proof ]` **login credential check (httpd /auth)**
    - (param1) --- ignored; object name comes from `this`
    - user ------- [ string ], check this user
    - proof ------ [ string ], `Base64(PBKDF2-HMAC-SHA256(plaintext, salt=user:rand, iter=10000, dkLen=32))`
      - `rand` is `reg.int[rand]` / `land@machine.status` `rand` (same value the login page uses)
      - on-disk password remains `simple_encode` reversible storage; proof is only for the wire
    - used by `center@userwui` via `auth_object`/`auth_api` (`scalls`, not helist)
    - keep this component **out of** userwui `helist` so cloud pages cannot call it as an oracle
    - lockout --- after **5** failed attempts for the same username, reject with `errno=EAGAIN` for **120** seconds. State is kept in `center@ctrl` **register** (`user_match_lock`, mmap file) so it survives httpd `scall`/`dlclose` of the ctrl `.so`; cleared on success, `user_reset`, or register wipe / reboot. httpd `/auth` may include `"reason":"locked"` so the login page can show a distinct message
    - wrong return tfalse
    - correct return ttrue

    Example (compute proof with OpenSSL / Python, then match). Assume plaintext `67334ertFAS`, user `sam`, rand `664848655`:
    ```shell
    dimmalex@CLS:~/snake8$ he reg.int[ rand ]
    664848655
    # proof = Base64(PBKDF2-HMAC-SHA256("67334ertFAS", "sam:664848655", 10000, 32))
    dimmalex@CLS:~/snake8$ he center@ctrl.user_match[ ,sam,<proof> ]
    ttrue
    ```


**Gateway**

+ `dump[ user, macid ]` **dump heport online memory for one gateway**
    - user ------ [ string ], username (reserved; admin may dump any online mac)
    - macid ----- [ string ], mac identify of gateway
    - error return NULL
    - succeed return json from `center@heport` dump control
    - not for cloud self-service; keep on `center@ctrl` (out of userwui helist)

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.dump[ ashyelf,00037f120000 ]
    ```


### Other

- Related self-service APIs: `center@api.user_profile`, `user_modify`, `user_passwd`
- `center@userwui` config: `auth_object=center@ctrl`, `auth_api=user_match`
- On-disk layout: `userdir/README.md`, `userdir/config.md`
