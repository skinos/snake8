## camera@gsuni — Univision camera GB28181 helpers

### Overview

Shell component (flat `exe` script) that queries Univision camera GB28181 registration
status over `UNIV_API`, and stores/returns a string via the component register.
Camera address and credentials come from **`camera@osd`**
(`address` / `username` / `password`).


### Dependencies

- **`camera@osd`** — camera IP and login credentials
- **`land@register`** — string register used by **`put`** / **`val`**
- Device tools: `curl`, `md5sum` (login uses `encryptType:0` MD5 hashing)


### API Reference

#### Control

##### + camera@gsuni.put[ text ]

###### Description

Write `text` into the component register key **`value`**
(`land@register` under `camera@gsuni`). Empty `text` clears the key.

###### Returns

```json
ttrue
```

###### Example

```shell
camera@gsuni.put[ hello ]
ttrue
```


#### Query

##### + camera@gsuni.val

###### Description

Return the string previously stored by **`put`** (register key **`value`**).
If the key is missing or empty, return **NULL**.

###### Returns

```json
"hello"
```

or

```json
NULL
```

###### Example

```shell
camera@gsuni.val
hello
```


##### + camera@gsuni.gb28281

###### Description

Login to the Univision camera (or reuse an existing session from `camera@osd`), call `GB28181.getConfig`, map `params.regStatus`
to a UTF-8 Chinese status string, and return that string.

| `regStatus` | Returned |
|-------------|----------|
| `online` | `在线` |
| `offline` | `离线` |

On login, query failure, or unknown `regStatus` the call returns **NULL**.

###### Returns

```json
"在线"
```

or

```json
"离线"
```

or

```json
NULL
```

###### Example

```shell
camera@gsuni.gb28281
离线
```
