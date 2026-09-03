## camera@univision — Univision camera GB28181 helpers

### Overview

Shell component (flat `exe` script) that queries Univision camera GB28181 registration
status over `UNIV_API`. Camera address and credentials come from **`camera@osd`**
(`address` / `username` / `password`).


### Dependencies

- **`camera@osd`** — camera IP and login credentials
- Device tools: `curl`, `md5sum` (login uses `encryptType:0` MD5 hashing)


### API Reference

#### Query

##### + camera@univision.gb28281

###### Description

Login to the Univision camera, call `GB28181.getConfig`, map `params.regStatus`
to a UTF-8 Chinese status string, and return that string.

| `regStatus` | Returned |
|-------------|----------|
| `online` | `在线` |
| `offline` | `离线` |

On login, query failure, or unknown `regStatus` the call returns **`terror`**.

###### Returns

```json
"在线"
```

or

```json
"离线"
```

###### Example

```shell
camera@univision.gb28281
离线
```
