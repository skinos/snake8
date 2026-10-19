# Center users — admin (`center@ctrl`) and self-service (`center@api`)

Sources: `config/slave/center/ctrl.md`, `api.md` (User), `userdir/config.md`.

Requires `center@heport` so `device_path` is registered. Account file: `{device_path}/<user>/config`.

## Who can call what

| API | Audience | On userwui helist? |
|-----|----------|--------------------|
| `center@ctrl.user_*` / `dump` | Admin / local `he` / admin WUI | **No** |
| `center@api.user_profile` / `user_modify` / `user_passwd` | Cloud user | Yes (`center@api`) |
| `center@ctrl.user_match` | httpd `/auth` only (`scalls`) | Must stay off helist |

`relay_max` is **admin-only** (`center@ctrl`). Cloud `user_modify` cannot change it.

## Admin — `center@ctrl`

### List users

```bash
he 'center@ctrl.user_list'
he 'center@ctrl.user_list[ ashyelf ]'
```

Returns JSON keyed by username; password `key` stripped. Fail → NULL.

### Create

```bash
he 'center@ctrl.user_add[ ashyelf,Cfw1234BE,,en, TestUser ]'
he 'center@ctrl.user_add[ ashyelf,Pass,,en, MeshUser, 2 ]'   # relay_max=2
```

- Username: `A-Z` `a-z` `0-9` `_` `-`; length < 32; no `/` `.` space
- `key` required plaintext (stored `simple_encode`)
- Optional: `vcode`, `lang`, `comment`, `relay_max` (omit=unlimited, `0`=none, `N`=cap)
- Fail if exists / bad name / encode fail → `tfalse`

### Modify (non-password)

```bash
he 'center@ctrl.user_modify[ ashyelf,sssss,en, TestUser ]'
he 'center@ctrl.user_modify[ ashyelf,,,,2 ]'    # set relay_max
```

Omitted args unchanged; explicit empty clears `vcode` / `comment` / `relay_max` (cleared = unlimited). Shrinking `relay_max` does not drop listens already up.

### Reset password / delete

```bash
he 'center@ctrl.user_reset[ ashyelf,NewPass123 ]'
he 'center@ctrl.user_delete[ ashyelf ]'
```

Delete: each gateway via `center@api.delete`, then networks/maps, then remove `{device_path}/<user>/`. Best-effort cleanup; tree still removed.

### Login match (HTTP auth)

```bash
he 'reg.int[ rand ]'
# proof = Base64(PBKDF2-HMAC-SHA256(plaintext, salt=user:rand, iter=10000, dkLen=32))
he 'center@ctrl.user_match[ ,ashyelf,<proof> ]'
```

5 failures → lock 120s (`EAGAIN` / `"reason":"locked"`). Cleared on success, `user_reset`, or reboot.

### Dump online gateway memory

```bash
he 'center@ctrl.dump[ ashyelf,00037f123450 ]'
```

Admin diagnostic; not for cloud self-service.

## Self-service — `center@api`

```bash
he 'center@api.user_profile[ ashyelf ]'
he 'center@api.user_modify[ ashyelf,,en, RealOne ]'   # vcode/lang/comment only
he 'center@api.user_passwd[ ashyelf,<old_proof>,<new_wrap> ]'
```

- `user_passwd`: old PBKDF2 proof + AES-wrapped new password (see `api.md`; WUI `auth.js`)
- Does **not** change `relay_max`

## Account file fields

| Key | Meaning |
|-----|---------|
| `key` | Encoded password |
| `vcode` | Device register must match when set |
| `lang` / `comment` | UI / note |
| `relay_max` | Live mesh center-UDP listen cap |

Read path: `he 'center@heport/ashyelf/config'` (shows `key`; prefer list/profile APIs when stripping password).
