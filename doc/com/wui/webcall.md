# HTTP HE bridge (WUI / `webs`)

The device exposes a small JSON-over-HTTP API that runs the same **HE** command lines as the CLI (see [`doc/com/land/eline.md`](../../doc/com/land/eline.md)). The Ace WUI talks to **`POST /he`** after login and uses **`POST /auth`** for the session key. Unauthenticated **`POST /public`** is allowed only for commands that match **`wui@admin.publist`** on the device.

**Test base URL used in examples:** `http://192.168.8.1`  
**Test credentials:** username `admin`, password `admin` (only for that demo host; production devices may differ).

Set a shell variable once so you can paste the rest:

```bash
export BASE='http://192.168.8.1'
```

Examples below use **`curl`** and **`jq`** (`sudo apt install jq` / `brew install jq`). Without `jq`, read the JSON response and copy fields by hand.

---

## Common HTTP behaviour

| Item | Detail |
|------|--------|
| Methods | These flows use **`POST`**. |
| Request body | JSON object. Use **`Content-Type: application/json`**. |
| Success status | **`200 OK`** for normal handling. |
| Response **`Content-Type`** | Often **`text/plain; charset=UTF-8`** even when the body is JSON. Parse by shape (body starts with `{` vs plain text). |
| CORS | Responses include **`Access-Control-Allow-Origin: *`**. |
| Connection | **`Connection: close`**. |

**Plain-text error bodies** (still often `200 OK`):

- **`Data Error`** — empty body, invalid JSON, or other parse issues.
- **`Auth Error`** — missing/invalid session key on `/he`, disallowed command on `/public` or `/he`, or nested JSON where a command string is required.

---

## 1. `POST /public` — no session (read-only subset)

Used before login to read **`land@machine.status`** (field **`rand`** is required to build the login password) and other HE lines permitted by **`publist`**. Any **`username`** / **`key`** fields in the body are ignored.

### Try it with `curl`

**Step 1 — call `/public` and print the response:**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}'
```

You should see one JSON object. The value at **`he`** is the status object; note **`he.rand`** for login.

**Step 2 (optional) — pretty-print with `jq`:**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' | jq .
```

**Step 3 (optional) — two commands in one request** (`he`, `he1`, …):

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status","he1":"wui@admin"}' | jq .
```

If a command is not on **`publist`**, the body may be **`Auth Error`** instead of JSON.

### Raw HTTP (what the exchange looks like on the wire)

**Request:**

```http
POST /public HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 28

{"he":"land@machine.status"}
```

**Response (example from the test host; values differ per device):**

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8
Connection: close

{"he":{"mode":"mix","broken":"disable","name":"720F-1238FE","platform":"srock","hardware":"rk3568","custom":"720f","scope":"std","version":"v8.6.0213","rand":"54309407","livetime":"00:17:36:0","current":"08:15:45:03:26:2026","mac":"00:03:7F:12:38:FE","macid":"00037F1238FE","model":"720F","wui_port":"80","telnet_port":"23","ssh_port":"22","local_ip":"192.168.8.1"}}
```

**JSON shape:** each top-level key you send (`he`, `he1`, …) is echoed in the response. The **value** is the HE result (string, JSON object, or strings like **`ttrue`** / **`tfalse`**).

**Postman:** **POST** → `http://192.168.8.1/public` → Body → **raw** → **JSON** → `{"he":"land@machine.status"}`.

---

## 2. `POST /auth` — login

### Password field (same rules as `project/wui/ace/js/login.js`)

The JSON field **`password`** is **not** the plain text password.

1. Get **`rand`** from **`land@machine.status`** via **`POST /public`** (path **`he.rand`** in the response).
2. Build the UTF-8 string: **`plainPassword + ":" + username + ":" + rand`**.
3. **MD5** that string → **32-character lowercase hexadecimal** string (same as the **blueimp-md5** result in the WUI).
4. **Base64-encode** that **hex string** (ASCII), standard Base64.

Example when **`rand`** is **`54309407`** and the account is **`admin` / `admin`**:

- MD5 input: `admin:admin:54309407`
- MD5 (hex): `7c73e9b92e9dceff9664716f802a324`
- Value for **`password`**: **`N2M3M2U5YjkyZTllZGNlZmY5NjY0NzE2ZjgwMmEzMjQ=`**

If login returns **`{"return":"false"}`**, fetch **`rand`** again from `/public` and recompute — **`rand`** can change.

### Try it with `curl` (step by step)

**Step 1 — get `rand` from `/public`:**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' > /tmp/wui_public.json

jq -r '.he.rand' /tmp/wui_public.json
```

Copy the printed **`rand`**, or capture it in a variable:

```bash
RAND=$(jq -r '.he.rand' /tmp/wui_public.json)
echo "rand=$RAND"
```

**Step 2 — compute `password` for `admin` / `admin`**

On Linux (GNU **`md5sum`** + **`base64`**):

```bash
USER=admin
PLAIN=admin
MD5=$(printf '%s' "$PLAIN:$USER:$RAND" | md5sum | awk '{print $1}')
PW=$(printf '%s' "$MD5" | base64 -w0 2>/dev/null || printf '%s' "$MD5" | base64 | tr -d '\n')
echo "password JSON field: $PW"
```

Portable one-liner with **Python 3** (same result on Linux / macOS):

```bash
export RAND=$(jq -r '.he.rand' /tmp/wui_public.json)
PW=$(python3 -c "import hashlib,base64,os; r=os.environ['RAND']; u='admin'; p='admin'; h=hashlib.md5(f'{p}:{u}:{r}'.encode()).hexdigest(); print(base64.b64encode(h.encode()).decode())")
echo "password JSON field: $PW"
```

**Step 3 — send `/auth` and print the result:**

```bash
curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" | jq .
```

**Step 4 — save the session key for `/he`:**

```bash
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" | jq -r '.key')
echo "key=$KEY"
```

Expect **`return`** **`true`** and a non-empty **`key`**.

### Raw HTTP (login)

**Request:**

```http
POST /auth HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 96

{"username":"admin","password":"N2M3M2U5YjkyZTllZGNlZmY5NjY0NzE2ZjgwMmEzMjQ="}
```

(`Content-Length` depends on the exact JSON; your client sets it automatically.)

**Response on success** (`key` value is unique each time):

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8

{"key":"BApNBS19XeiRH_syJSi8_CNqjNK6c9539jPLIj5GA9A!","username":"admin","return":"true"}
```

**Response on failure:**

```json
{"return":"false"}
```

**Postman:** run **`POST /public`** first → copy **`rand`** → compute **`password`** (script or external tool) → **`POST /auth`** with `username` + `password`.

---

## 3. `POST /he` — authenticated HE batch

You need the latest **`key`** and **`username`** from **`/auth`** (after each **`/he`** response, prefer the new **`key`** in that response for the next call). All other top-level string fields are HE lines: **`he`**, **`he1`**, **`he2`**, …

The device may restrict commands with **`wui@admin.helist`**. A query string such as **`?rand=0.123`** is optional (the WUI uses it to avoid caching).

### Try it with `curl` (step by step)

Assume **`BASE`**, **`KEY`**, and **`USER=admin`** are already set from the login steps. If not, log in again and set:

```bash
USER=admin
# RAND=... PW=... as in section 2, then:
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"$USER\",\"password\":\"$PW\"}" | jq -r '.key')
```

**Step 1 — read full `land@machine` config:**

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\"}" | jq .
```

**Step 2 — refresh `KEY` from the response** (recommended before the next call):

```bash
KEY=$(curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\"}" | jq -r '.key')
echo "updated key=$KEY"
```

**Step 3 — batch: full config + one attribute path:**

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\",\"he1\":\"land@machine:name\"}" | jq .
```

Update **`KEY`** again from this response’s **`.key`** field.

**Step 4 — set one attribute** (example keeps the same name; adjust as needed):

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine:name=720F-1238FE\"}" | jq .
```

Expect **`he`** to be **`ttrue`** on success.

**Step 5 — call a component method** (status JSON):

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine.status\"}" | jq .
```

**Step 6 — one request, multiple HE lines** (`he`, `he1`, `he2`, `he3`, …). Each field runs as a separate command; the response echoes the same keys with results plus a refreshed **`key`**:

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\",\"he1\":\"land@machine:name\",\"he2\":\"land@machine.status\",\"he3\":\"land@machine:language\"}" | jq .
```

Example response shape (values depend on the device):

```json
{
  "he": { "mode": "mix", "name": "720F-1238FE", "...": "..." },
  "he1": "720F-1238FE",
  "he2": { "version": "v8.6.0213", "rand": "54309407", "...": "..." },
  "he3": "en",
  "key": "<refreshed-session-key>"
}
```

If **`helist`** rejects a line, the whole request may fail with **`Auth Error`**. After a successful batch, set **`KEY`** from **`.key`** again before the next **`/he`**.

After every **`/he`** call, read the new **`key`** from the JSON body and use it for the following request.

### Raw HTTP (authenticated batch)

**Request:**

```http
POST /he HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 180

{"key":"<session-key-from-auth>","username":"admin","he":"land@machine","he1":"land@machine:name"}
```

**Response (example shape from the test host):**

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8

{"he":{"mode":"mix","broken":"disable","name":"720F-1238FE","mac":"00:03:7F:12:38:FE","macid":"00037F1238FE","language":"en"},"he1":"720F-1238FE","key":"<refreshed-session-key>"}
```

### Reference: what to put in `he` / `he1` / …

| Intent | HE line in JSON value |
|--------|------------------------|
| Full component | `land@machine` |
| One attribute | `land@machine:name` |
| Set one attribute | `land@machine:name=720F-1238FE` |
| Call method | `land@machine.status` (or `component.method[...]` per grammar) |

**Successful set (response fragment):**

```json
{"he":"ttrue","key":"<new-key>"}
```

**Postman:** store **`key`** after **`/auth`**; after each **`/he`**, copy the new **`key`** from the response into your next request (or use a **Tests** script: `pm.environment.set("talkkey", pm.response.json().key)`).

---

## HE grammar (quick reference)

Full syntax: **[`doc/com/land/eline.md`](../../doc/com/land/eline.md)**.

| Goal | HE line |
|------|---------|
| Query full config | `component` e.g. `land@machine` |
| Query one path | `component:attr` e.g. `land@machine:name` |
| Set one path | `component:attr=value` |
| Call method | `component.method` or `component.method[arg1,arg2]` |
| Method + pick field | `component.method:field/path` |
