# Device full-test — reference

## HE pitfalls that wipe or mislead

| Input | What happens |
|-------|----------------|
| `com\|foo="bar"` | Parsed as OR with **string** value → whole-object non-JSON set → `config_set` may `string2file("", file)` → **empty config** |
| `com\|{"a":"1"}` on already-empty cfg | Merge onto empty → only `a` remains (looks like “merge deleted keys”) |
| Prompt match on `$` | Breaks on keys like `"$L-S$"` mid-JSON |
| Cached `/api/util.js?v=…` | File on device fixed, live `page.*` still old until cache-disabled reload |

## Minimal API matrix shape

For each `project@comp`:

```text
.project@comp
?project@comp
?project@comp.<each api from . list>
project@comp.<api>          # or .api[args] per docs
```

Compare `.` list vs docs vs source `*_api` registration. Digits in API names require land `api_list` rules (leading a-z; then a-z/_/digit).

## Minimal config matrix shape

```text
# snapshot
com
# per key
com:key=<v1> ; com:key ; …
com:key=<vN> ; com:key
# restore
com={…defaults from *.cfg…}   # or per-key sets
com
```

## Minimal WUI step shape

```text
for control in inventory:
  for value in all_options_or_samples:
    set UI
    Apply
    he.load / eline get → assert
restore defaults via UI or HE
```

## Severity guide

| Level | Use |
|-------|-----|
| high | Data loss, crash, cannot configure, wrong persist |
| medium | API probe/list mismatch, broken control with workaround |
| low | UX-only (eye icon), doc mismatch |
| env | Missing peer device/SIM/network — cannot E2E |

## Example projects already exercised this way

| Project | Notes |
|---------|-------|
| `agent` | Full eline + WUI variation matrix |
| `camera` | `osd` / `osd2he` / `gsuni`; mapping keys contain `$…$`; password-eye; gb28281 naming |
