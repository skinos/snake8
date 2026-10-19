# Firmware library and OTA — `center@api`

Sources: `config/slave/center/api.md` (Firmware), `userdir/firmware.md`, `api/firmware.c`, `userwui/content/upgrade.html`.

There is **no** `firmware_match` HE API. Matching is operator-side (this skill + WUI filter by `custom`).

## HARD RULE — match before push / FPK

1. Device model (prefer live):

```bash
hh 00037f123450-land@machine.status
# use: platform, hardware, custom, scope, version (and oem if present)
```

Fallback: `he 'center@heport/ashyelf/dev/00037f123450/reg'` → nested `land@machine.status` (`hardware`, `custom`, `scope`, `version`).  
`center@api.list` may filter by `custom` but **strips** `hardware`/`custom` from the JSON you see — do not rely on list alone for match.

2. Package side — stored name:

```text
{hardware}_{custom}_{scope}_{version}[_{oem}].zz
```

`firmware_list` returns parsed `custom` / `scope` / `version` / optional `oem` (and basename in `zz`).

3. Compare **hardware**, **custom**, **scope** (and oem when relevant). Platform: same rule as **device-upgrade** Step 0 (leading-`s` forms compatible, e.g. `wrt`↔`swrt5`).

4. **Mismatch → stop.** Report both sides. Do **not** call `firmware_push` or device `/upload`.

5. FPK / penetrate-then-upload: still this check, then **device-upgrade**.

## Library CRUD

```bash
he 'center@api.firmware_add[ ashyelf, /path/to/mt7628_r600_std_v8.5.1109.zz ]'
he 'center@api.firmware_list[ ashyelf ]'
he 'center@api.firmware_path[ ashyelf, mt7628_r600_std_v8.5.1109.zz ]'
he 'center@api.firmware_delete[ ashyelf, mt7628_r600_std_v8.5.1109.zz ]'
```

- `firmware_add` unpacks the `.zz`, derives the canonical filename, stores under `<user>/firmware/`
- Do not rename `.zz` by hand
- Empty lib → `{}`

## Select image for a device

```bash
# After status shows custom=r600, hardware=mt7628, scope=std:
he 'center@api.firmware_list[ ashyelf ]'
# pick zz where custom/hardware/scope match (hardware is in the filename prefix)
```

WUI upgrade page: `list[ user,,,custom ]` then push only that custom’s gateways.

## Push OTA

```bash
he 'center@api.firmware_push[ ashyelf,<url>,00037f123450 ]'
```

- Device must exist under `dev/<mac>` and be **online** (hh)
- URL must have no comma, brackets, whitespace, or control chars
- Internally: `land@service.start[ heclient-upgrade, arch@firmware, online_upgrade, <url>,,2 ]` via hh

### Build download URL (userwui pattern)

After cloud `/auth` you have `username` + `key`. Same host as userwui (default port **20000**):

```text
{proto}://{host}/download?username={user}&key={key}&object=center@api&api=firmware_path&p={user}&p2={file.zz}
```

Then:

```bash
he 'center@api.firmware_push[ ashyelf,http://…/download?…&p2=mt7628_r600_std_v8.5.1109.zz,00037f123450 ]'
```

Local-only ops still need a URL the **device** can HTTP-get (usually center’s userwui `/download`, not a host filesystem path).

## Verify

```bash
hh 00037f123450-land@machine.status:version
he 'center@api.list[ ashyelf ]:00037f123450'
```

List may show transient version strings during upgrade (`downloading` / `upgrading` / …) per WUI.

## Example mac `00037f123450`

```text
1. he 'center@api.list[ ashyelf ]' — find mac, online?
2. hh 00037f123450-land@machine.status — hardware/custom/scope
3. firmware_list — choose matching zz only
4. If no match → stop (or firmware_add correct image, then rematch)
5. firmware_push with download URL
6. Optional: gateway_http then device-upgrade for FPK — match again first
```
