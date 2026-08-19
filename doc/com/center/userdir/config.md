## <username>/config — User account configuration

### Overview

JSON file that stores account settings for one cloud username.

- Path: `{device_path}/<username>/config`
- HE/dbs path: `center@heport/<username>/config`
- Created by `center@ctrl.user_add`; removed with the user tree by `center@ctrl.user_delete`
- Password `key` written by `center@ctrl.user_add`, `center@ctrl.user_reset`, or `center@api.user_passwd`
- Non-password fields updated by `center@ctrl.user_modify` or `center@api.user_modify`
- `center@heport` reads `vcode` on device SSL register to authorize the gateway
- List/self APIs strip `key` from returns


### Configuration reference ( <username>/config )

```json
// Attributes introduction 
{
    "key": "encoded account password",          // [ string ], written via simple_encode; required on create
    "vcode": "device verify code",              // [ string ], compared with register JSON "vcode" on SSL connect
    "lang": "UI language",                      // [ string ], e.g. "en", "cn"; empty follows system default
    "comment": "operator comment"               // [ string ], free text for the account
}
```

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



### Other

Related HE APIs:

- Admin (`center@ctrl`, not in userwui helist): `user_add`, `user_list`, `user_modify`, `user_delete`, `user_reset`, `user_match` (httpd `/auth`)
- Cloud self-service (`center@api`): `user_profile[user]`, `user_modify`, `user_passwd`
