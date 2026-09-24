## center@ctrl — Admin control APIs

### Overview

Admin-only HE APIs for managing cloud usernames (create, list, get/set config, reset password, delete) and privileged gateway diagnostics.

- Intended for device admin WUI (`userlist.html` / `user.html`) and operator `he` / eline
- Must **not** be listed in `center@userwui` `helist` / `publist` so cloud user pages cannot call these methods
- Cloud login and self-service stay on `center@api` (see `api.md`)
- Account files live under heport `device_path`: `{device_path}/<username>/config`


### Dependencies

- Requires `center@heport` running so `device_path` is registered


### API Reference

#### Management APIs

**User**

+ `user_add[ user, key, [vcode] ]` **create a user (create only)**
    - user ------- [ string ], required; only `A-Z` `a-z` `0-9` `_` `-`; length `< 32` bytes (reject `/` `.` space `;` etc.)
    - key -------- [ string ], required plaintext password (stored via `simple_encode`)
    - vcode ------ [ string ], optional device register code
    - other fields (lang/comment/relay_max/nport/pport/log/…) — use `user_set` / `user_orset` after create
    - fails if user already exists, key missing, username has illegal characters, or password encode fails
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_add[ ashyelf,Cfw1234BE,sssss ]
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
            "vcode":"sssss",
            "lang":"en",
            "comment":"TestUser"
        }
    }
    dimmalex@CLS:~/snake8$
    ```

+ `user_get[ user, [attr] ]` **read user config (no password)**
    - user ------ [ string ], required; same charset as `user_add`
    - attr ------ [ string ], optional field path; omit for whole object (password `key` stripped / refused)
    - missing user dir or config file: NULL (`ENOENT`)
    - failed return NULL
    - succeed return json or field value

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_get[ ashyelf ]
    dimmalex@CLS:~/snake8$ he center@ctrl.user_get[ ashyelf, nport ]
    ```

+ `user_set[ user, value, [attr] ]` **assign (`=`)**
    - user ------ [ string ], required
    - value ----- [ string | object ], required
    - attr ------ [ string ], optional; with attr set that field (**empty string deletes** the key); without attr, `value` must be object (keys merged at user root; empty string values delete those keys)
    - never writes `key` (use `user_add` / `user_reset`)
    - after save, ctrl calls heport unix control `user_reload` (via `heport_call`, same path as `dump`) so online gateways re-get adjust
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he 'center@ctrl.user_set[ ashyelf, disable, nport ]'
    ttrue
    dimmalex@CLS:~/snake8$ he 'center@ctrl.user_set[ ashyelf, {"lang":"cn","comment":"lab"} ]'
    ttrue
    ```

+ `user_orset[ user, value, [attr] ]` **merge (`|`)**
    - same args as `user_set`; object values are deep-merged; empty string clears keys the same way
    - with attr + object: merge under that path; with attr + scalar: same as set
    - also triggers heport unix control `user_reload` via `heport_call`
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he 'center@ctrl.user_orset[ ashyelf, {"nport":"disable","log":"enable"} ]'
    ttrue
    ```

+ `user_delete[ user ]` **delete a user tree**
    - user ------ [ string ], required; same charset as `user_add` (reject `/` `.` `..`)
    - first deletes each gateway via `center@api.delete` (maps, mesh membership and any mesh relay UDP, heport knock)
    - then deletes remaining networks via `center@api.network_delete` and leftover TCP/UDP maps
    - then removes `{device_path}/<user>/` entirely
    - cleanup is best-effort; the user tree is still removed
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he center@ctrl.user_delete[ ashyelf ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```

+ `user_reset[ user, newkey ]` **admin reset password**
    - user ------- [ string ], required; same charset as `user_add` (reject `/` `.` `..`)
    - newkey ----- [ string ], required new plaintext password
    - no old password and no admin password check; access is gated by not exposing this component on userwui helist
    - also clears `user_match` login lockout for this username
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
    - user ------- [ string ], required; same charset as `user_add` (reject empty `/` `.` `..`)
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

- Admin WUI: menu `userlist.html` (add / delete / reset password); double-click opens `user.html` for `user_get` / `user_orset` (no password)
- Related self-service APIs: `center@api.user_get`, `user_set`, `user_orset`, `user_passwd`
- `center@userwui` config: `auth_object=center@ctrl`, `auth_api=user_match`
- On-disk layout: `userdir/README.md`, `userdir/config.md`
- Mesh relay listen cap: `<user>/config` `relay_max` (this component). Wish field `relay` stays on the net file and makes a NAT member a hub (`userdir/net/mynet.md`)
- Feature gates on `<user>/config`: `nport` / `pport` / `log` = `disable` force agent adjust off (see `userdir/config.md`); live push via heport control `user_reload`
