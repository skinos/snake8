## <username>/dev/<macid>/config — Per-gateway operator configuration

### Overview

JSON file that stores operator settings for one registered gateway.

- Path: `{device_path}/<username>/dev/<macid>/config` (`00037f120000` here is an example macid)
- HE/dbs path: `center@heport/<username>/dev/<macid>/config`
- Loaded by `center@heport` on SSL register; used to build the `agent@heclient` / `agent@portc` / `agent@gtog` / `agent@logc` adjust push
- Merged into `center@api.device_list` / endpoint list responses
- Extra keys may be stored for UI/extensions; center currently consumes `comment`, `portc`, `gtog`, `log`
- User-level gates on `<user>/config` (`nport`/`pport`/`log`) can also force the matching agent off (see `userdir/config.md`)


### Configuration reference ( <username>/dev/<macid>/config )

```json
// Attributes introduction 
{
    "comment": "operator comment",              // [ string ], free text shown in gateway list
    "portc": "port client switch",              // [ "enable", "disable" ], when "disable" the connect adjust sets agent@portc status to disable
                                                    // omitted or other values: follow center@pport (and user pport gate)
    "gtog": "mesh client switch",               // [ "enable", "disable" ], when "disable" forces agent@gtog off
                                                    // omitted: follow center@nport (and user nport gate)
    "log": "remote log switch"                  // [ "enable", "disable" ], when "disable" forces agent@logc.center off
                                                    // omitted: follow center@log (and user log gate)
    // "...":"..."  How many custom properties show how many properties
}
```

#### Configuration example

Example, show device config for macid 00037f120000

```shell
center@heport/ashyelf/dev/00037f120000/config
{                                               # return this
    "comment": "Office gateway",
    "portc": "enable"
}
```

#### Configuration settings example

Example, set comment

```shell
center@heport/ashyelf/dev/00037f120000/config:comment=Office gateway
ttrue
```

Example, disable portc for this gateway on next connect

```shell
center@heport/ashyelf/dev/00037f120000/config:portc=disable
ttrue
```

Example, merge set comment and portc

```shell
center@heport/ashyelf/dev/00037f120000/config|{"comment":"Office gateway","portc":"enable"}
ttrue
```



### Other

Related HE APIs: `center@api.list`, `center@api.comment`, `center@api.delete` (prefer these over direct dbs paths from userwui).

On connect, `center@heport` pushes:

```json
{
    "cmd":
    {
        "obj": "agent@heclient",
        "op": "adjust",
        "1":
        {
            "agent@portc":
            {
                "status": "enable",
                "nomate_timeout": "46",
                "connect_timeout": "14",
                "mate_timeout": "180",
                "active_pond": "6",
                "pond": "6",
                "idle_pond": "1"
            }
        }
    }
}
```

Timeouts and `active_pond` come from the live `center@pport` register. `idle_pond` uses the owning user's `idle_pond` when that value is a full integer `0..10000`; otherwise it uses the `center@pport` register. `status` is forced to `"disable"` when this file has `"portc":"disable"`, when the owning user has `"pport":"disable"`, or when `center@pport` itself is disabled. The same OR logic applies to `gtog`/`nport` → `agent@gtog` and `log` → `agent@logc.center`. Per-user `log_file_size` / `log_file_max` stay on the account file and are applied by `center@log`, not in this adjust.
