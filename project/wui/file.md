## wui@file — Temporary File Manager

### Overview

Manage regular files under the temporary directory **`FILE_DIR`** (`/tmp/file`).
`wui@file.setup` creates this directory at boot (`init.app`).

Only list and delete are provided. Files are addressed by **filename** only (no path separators).


### Configuration reference ( wui@file )

No persistent configuration.


### API Reference

#### -

##### delete[ name ]

- Description: Delete one regular file under `FILE_DIR` by filename.
- Parameters:
    - `name` — [ string ], filename only; must not contain `/`
- Returns:
    - `ttrue` — deleted
    - `tfalse` — failed (`EINVAL` for bad name, `EPERM` / `EISDIR` / errno from `unlink` / missing file)

```shell
$ wui@file.delete[ dump.log ]
ttrue
```

#### +

##### setup

- Description: Create `FILE_DIR` if it does not exist. Invoked from `prj.json` `init.app`.
- Parameters: none
- Returns:
    - `ttrue` — directory ready
    - `tfalse` — create failed or path exists but is not a directory

```shell
$ wui@file.setup
ttrue
```

##### list

- Description: List regular files under `FILE_DIR`. Creates the directory if it does not exist.
- Parameters: none
- Returns: JSON map `{ "filename": "fullpath", ... }`, or empty `{}` when the directory has no regular files. `NULL` on hard failure (cannot create/open directory).

```shell
$ wui@file.list
{
    "dump.log":"/tmp/file/dump.log",
    "capture.pcap":"/tmp/file/capture.pcap"
}
```
