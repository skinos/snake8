## <username>/config — User account configuration

### Overview

JSON file that stores account settings for one cloud username.

- Path: `{device_path}/<username>/config`
- HE/dbs path: `center@heport/<username>/config`
- Created by `center@ctrl.user_add`; removed with the user tree by `center@ctrl.user_delete`
- Password `key` written by `center@ctrl.user_add`, `center@ctrl.user_reset`, or `center@api.user_passwd`
- Non-password fields by admin `center@ctrl.user_get` / `user_set` / `user_orset`; cloud `center@api.user_get` / `user_set` / `user_orset` may change `vcode` / `lang` / `comment` only
- `center@heport` reads `vcode` on device SSL register to authorize the gateway
- Admin sets `relay_max`, `idle_pond`, `log_file_size`, `log_file_max`, and the feature gates via ctrl `user_set` / `user_orset`. Cloud api cannot change them
- `idle_pond` is pushed in connect adjust as `agent@portc.idle_pond`. Omit, blank, or a value that is not a full integer `0..10000` keeps `center@pport` `idle_pond`
- `log_file_size` / `log_file_max` are not pushed to the gateway. `center@log` uses them when it writes this user's device logs. Omit, blank, or a value outside range keeps the service `log_file_size` / `log_file_max`
- After admin set/orset, ctrl uses `heport_call("user_reload", …)` so online sessions refresh and re-push adjust
- List/self APIs strip `key` from returns


### Configuration reference ( <username>/config )

```json
// Attributes introduction 
{
    "key": "encoded account password",          // [ string ], written via simple_encode; required on create
    "vcode": "device verify code",              // [ string ], compared with register JSON "vcode" on SSL connect
    "lang": "UI language",                      // [ string ], e.g. "en", "cn"; empty follows system default
    "comment": "operator comment",              // [ string ], free text for the account
    "nport": "mesh client gate",                // [ "enable", "disable" ], omit = follow device gtog + center@nport
                                                    // "disable" forces agent@gtog off in connect adjust
                                                    // "enable" = do not force off; same as omit for adjust (cannot force service on)
    "relay_max": "live mesh relay UDP cap",     // [ number ], optional; omit = unlimited; 0 = none; N = at most N
                                                    // nport counts remembered borrows in memory (nport_relay_limit current)
                                                    // enforced when nport would call relay_map; over cap stays a leaf
    "pport": "proxy client gate",               // [ "enable", "disable" ], omit = follow device portc + center@pport
                                                    // "disable" forces agent@portc off; "enable" = do not force off (same as omit for adjust)
    "idle_pond": "idle standby pond for this user", // [ number ], optional; 0..10000
                                                    // pushed as agent@portc idle_pond; omit or unusable follows center@pport
    "log": "remote log gate",                   // [ "enable", "disable" ], omit = follow device log + center@log
                                                    // "disable" forces agent@logc.center off; "enable" = do not force off (same as omit for adjust)
    "log_file_size": "per-user log file size",  // [ number ], kilobytes, optional; 1..1048576
                                                    // omit or unusable follows center@log log_file_size
    "log_file_max": "per-user log file count"   // [ number ], optional; 1..100000
                                                    // omit or unusable follows center@log log_file_max
}
```

Setting a field to an empty string via `user_set` / `user_orset` **deletes** that key (omit).
#### Configuration example

Example, show account file for user ashyelf

```shell
center@heport/ashyelf/config
{                                               # return this
    "key": "5n/KLt5QS0PdKVfg/XH2kQ==",         # encoded password
    "vcode": "sssss",                           # device must send matching vcode
    "lang": "en",
    "comment": "TestUser"
}
```

Example, cap this account at two live mesh relays

```shell
he 'center@ctrl.user_set[ ashyelf, 2, relay_max ]'
ttrue
```

Example, disable mesh for this account (online devices get adjust reload)

```shell
he 'center@ctrl.user_orset[ ashyelf, {"nport":"disable"} ]'
ttrue
```

Example, idle pond and log file caps for this account

```shell
he 'center@ctrl.user_orset[ ashyelf, {"idle_pond":"4","log_file_size":"2048","log_file_max":"20"} ]'
ttrue
```



### Other

Related HE APIs:

- Admin (`center@ctrl`, not in userwui helist): `user_add`, `user_list`, `user_get`, `user_set`, `user_orset`, `user_delete`, `user_reset`, `user_match` (httpd `/auth`)
- Live reload of online adjust: heport unix control cmd `user_reload` (invoked from ctrl set/orset via `heport_call`)
- Cloud self-service (`center@api`): `user_get`, `user_set`, `user_orset`, `user_passwd` (`relay_max` / `idle_pond` / `log_file_size` / `log_file_max` / feature gates are read-only on the cloud APIs)
- Admin page `user.html`: mesh shows `relay_max` unless `nport` is `disable`; proxy shows `idle_pond` unless `pport` is `disable`; remote log shows `log_file_size` and `log_file_max` unless `log` is `disable`
- Mesh relay wish is per endpoint and makes a NAT member a hub (`userdir/net/mynet.md`). This file only caps how many public UDP listens that wish may open
