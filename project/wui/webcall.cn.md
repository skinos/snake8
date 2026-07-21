# HTTP HE 桥接 (WUI / `webs`)

设备提供一个小型的 JSON-over-HTTP API，运行与 CLI 相同的 **HE** 命令行（参见 [`doc/com/land/eline.md`](../../doc/com/land/eline.md)）。Ace WUI 登录后通过 **`POST /he`** 进行通信，并使用 **`POST /auth`** 获取会话密钥。未认证的 **`POST /public`** 仅允许匹配设备上 **`wui@admin.publist`** 的命令。

**示例中使用的测试基础 URL：** `http://192.168.8.1`  
**测试凭据：** 用户名 `admin`，密码 `admin`（仅适用于该演示主机；生产设备可能不同）。

设置一个 shell 变量，以便您可以粘贴后续命令：

```bash
export BASE='http://192.168.8.1'
```

以下示例使用 **`curl`** 和 **`jq`**（`sudo apt install jq` / `brew install jq`）。如果没有 `jq`，可以直接读取 JSON 响应并手动复制字段。

---

## 通用 HTTP 行为

| 项目 | 详情 |
|------|--------|
| 方法 | 这些流程使用 **`POST`**。 |
| 请求体 | JSON 对象。使用 **`Content-Type: application/json`**。 |
| 成功状态 | **`200 OK`** 表示正常处理。 |
| 响应 **`Content-Type`** | 即使响应体是 JSON，通常也返回 **`text/plain; charset=UTF-8`**。根据内容格式判断（响应体以 `{` 开头为 JSON，否则为纯文本）。 |
| CORS | 响应包含 **`Access-Control-Allow-Origin: *`**。 |
| 连接 | **`Connection: close`**。 |

**纯文本错误响应体**（仍然通常返回 `200 OK`）：

- **`Data Error`** -- 空响应体、无效 JSON 或其他解析问题。
- **`Auth Error`** -- `/he` 上缺少/无效会话密钥、`/public` 或 `/he` 上不允许的命令，或在需要命令字符串的地方使用了嵌套 JSON。

---

## 1. `POST /public` -- 无需会话（只读子集）

在登录前用于读取 **`land@machine.status`**（字段 **`rand`** 是构建登录密码所必需的）以及 **`publist`** 允许的其他 HE 命令行。请求体中的任何 **`username`** / **`key`** 字段都会被忽略。

### 使用 `curl` 尝试

**步骤 1 -- 调用 `/public` 并打印响应：**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}'
```

您应该看到一个 JSON 对象。**`he`** 的值是状态对象；记下 **`he.rand`** 用于登录。

**步骤 2（可选）-- 使用 `jq` 格式化输出：**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' | jq .
```

**步骤 3（可选）-- 一个请求中发送两个命令**（`he`、`he1`、...）：

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status","he1":"wui@admin"}' | jq .
```

如果命令不在 **`publist`** 中，响应体可能是 **`Auth Error`** 而不是 JSON。

### 原始 HTTP（传输层的实际内容）

**请求：**

```http
POST /public HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 28

{"he":"land@machine.status"}
```

**响应（来自测试主机的示例；不同设备的值会不同）：**

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8
Connection: close

{"he":{"mode":"mix","broken":"disable","name":"720F-1238FE","platform":"srock","hardware":"rk3568","custom":"720f","scope":"std","version":"v8.6.0213","rand":"54309407","livetime":"00:17:36:0","current":"08:15:45:03:26:2026","mac":"00:03:7F:12:38:FE","macid":"00037F1238FE","model":"720F","wui_port":"80","telnet_port":"23","ssh_port":"22","local_ip":"192.168.8.1"}}
```

**JSON 结构：** 您发送的每个顶层键（`he`、`he1`、...）都会在响应中回显。**值**是 HE 结果（字符串、JSON 对象或 **`ttrue`** / **`tfalse`** 等字符串）。

**Postman：** **POST** -> `http://192.168.8.1/public` -> Body -> **raw** -> **JSON** -> `{"he":"land@machine.status"}`。

---

## 2. `POST /auth` -- 登录

### 密码字段（与 `project/wui/ace/js/login.js` 相同的规则）

JSON 字段 **`password`** **不是**明文密码。

1. 通过 **`POST /public`** 从 **`land@machine.status`** 获取 **`rand`**（响应中的 **`he.rand`** 路径）。
2. 构建 UTF-8 字符串：**`plainPassword + ":" + username + ":" + rand`**。
3. 对该字符串进行 **MD5** 运算 -> **32 位小写十六进制**字符串（与 WUI 中 **blueimp-md5** 的结果相同）。
4. 对该**十六进制字符串**（ASCII）进行**标准 Base64 编码**。

示例，当 **`rand`** 为 **`54309407`** 且账户为 **`admin` / `admin`** 时：

- MD5 输入：`admin:admin:54309407`
- MD5（十六进制）：`7c73e9b92e9dceff9664716f802a324`
- **`password`** 的值：**`N2M3M2U5YjkyZTllZGNlZmY5NjY0NzE2ZjgwMmEzMjQ=`**

如果登录返回 **`{"return":"false"}`**，请重新从 `/public` 获取 **`rand`** 并重新计算 -- **`rand`** 可能会变化。

### 使用 `curl` 尝试（逐步操作）

**步骤 1 -- 从 `/public` 获取 `rand`：**

```bash
curl -sS -X POST "$BASE/public" \
  -H 'Content-Type: application/json' \
  -d '{"he":"land@machine.status"}' > /tmp/wui_public.json

jq -r '.he.rand' /tmp/wui_public.json
```

复制打印出的 **`rand`**，或将其保存到变量中：

```bash
RAND=$(jq -r '.he.rand' /tmp/wui_public.json)
echo "rand=$RAND"
```

**步骤 2 -- 为 `admin` / `admin` 计算 `password`**

在 Linux 上（GNU **`md5sum`** + **`base64`**）：

```bash
USER=admin
PLAIN=admin
MD5=$(printf '%s' "$PLAIN:$USER:$RAND" | md5sum | awk '{print $1}')
PW=$(printf '%s' "$MD5" | base64 -w0 2>/dev/null || printf '%s' "$MD5" | base64 | tr -d '\n')
echo "password JSON field: $PW"
```

使用 **Python 3** 的跨平台单行命令（Linux / macOS 结果相同）：

```bash
export RAND=$(jq -r '.he.rand' /tmp/wui_public.json)
PW=$(python3 -c "import hashlib,base64,os; r=os.environ['RAND']; u='admin'; p='admin'; h=hashlib.md5(f'{p}:{u}:{r}'.encode()).hexdigest(); print(base64.b64encode(h.encode()).decode())")
echo "password JSON field: $PW"
```

**步骤 3 -- 发送 `/auth` 并打印结果：**

```bash
curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" | jq .
```

**步骤 4 -- 保存会话密钥用于 `/he`：**

```bash
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"admin\",\"password\":\"$PW\"}" | jq -r '.key')
echo "key=$KEY"
```

预期 **`return`** 为 **`true`** 且 **`key`** 非空。

### 原始 HTTP（登录）

**请求：**

```http
POST /auth HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 96

{"username":"admin","password":"N2M3M2U5YjkyZTllZGNlZmY5NjY0NzE2ZjgwMmEzMjQ="}
```

（`Content-Length` 取决于具体的 JSON；您的客户端会自动设置它。）

**成功响应**（`key` 值每次都是唯一的）：

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8

{"key":"BApNBS19XeiRH_syJSi8_CNqjNK6c9539jPLIj5GA9A!","username":"admin","return":"true"}
```

**失败响应：**

```json
{"return":"false"}
```

**Postman：** 先运行 **`POST /public`** -> 复制 **`rand`** -> 计算 **`password`**（脚本或外部工具）-> 使用 `username` + `password` 发送 **`POST /auth`**。

---

## 3. `POST /he` -- 已认证的 HE 批量请求

您需要从 **`/auth`** 获取最新的 **`key`** 和 **`username`**（每次 **`/he`** 响应后，优先使用该响应中的新 **`key`** 用于下一次调用）。所有其他顶层字符串字段都是 HE 命令行：**`he`**、**`he1`**、**`he2`**、...

设备可能通过 **`wui@admin.helist`** 限制命令。查询字符串（如 **`?rand=0.123`**）是可选的（WUI 使用它来避免缓存）。

### 使用 `curl` 尝试（逐步操作）

假设 **`BASE`**、**`KEY`** 和 **`USER=admin`** 已在登录步骤中设置。如果没有，请重新登录并设置：

```bash
USER=admin
# RAND=... PW=... 如第 2 节所述，然后：
KEY=$(curl -sS -X POST "$BASE/auth" \
  -H 'Content-Type: application/json' \
  -d "{\"username\":\"$USER\",\"password\":\"$PW\"}" | jq -r '.key')
```

**步骤 1 -- 读取完整的 `land@machine` 配置：**

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\"}" | jq .
```

**步骤 2 -- 从响应中刷新 `KEY`**（建议在下一次调用前执行）：

```bash
KEY=$(curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\"}" | jq -r '.key')
echo "updated key=$KEY"
```

**步骤 3 -- 批量请求：完整配置 + 单个属性路径：**

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\",\"he1\":\"land@machine:name\"}" | jq .
```

再次从此响应的 **`.key`** 字段更新 **`KEY`**。

**步骤 4 -- 设置单个属性**（示例保持相同名称；根据需要调整）：

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine:name=720F-1238FE\"}" | jq .
```

成功时预期 **`he`** 为 **`ttrue`**。

**步骤 5 -- 调用组件方法**（状态 JSON）：

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine.status\"}" | jq .
```

**步骤 6 -- 一个请求，多个 HE 命令行**（`he`、`he1`、`he2`、`he3`、...）。每个字段作为独立命令运行；响应回显相同的键和结果，并附带一个刷新后的 **`key`**：

```bash
curl -sS -X POST "$BASE/he" \
  -H 'Content-Type: application/json' \
  -d "{\"key\":\"$KEY\",\"username\":\"$USER\",\"he\":\"land@machine\",\"he1\":\"land@machine:name\",\"he2\":\"land@machine.status\",\"he3\":\"land@machine:language\"}" | jq .
```

响应结构示例（值取决于设备）：

```json
{
  "he": { "mode": "mix", "name": "720F-1238FE", "...": "..." },
  "he1": "720F-1238FE",
  "he2": { "version": "v8.6.0213", "rand": "54309407", "...": "..." },
  "he3": "en",
  "key": "<refreshed-session-key>"
}
```

如果 **`helist`** 拒绝某个命令行，整个请求可能以 **`Auth Error`** 失败。批量请求成功后，在下一次 **`/he`** 请求前再次从 **`.key`** 更新 **`KEY`**。

每次 **`/he`** 调用后，从 JSON 响应体中读取新的 **`key`** 并用于后续请求。

### 原始 HTTP（已认证的批量请求）

**请求：**

```http
POST /he HTTP/1.1
Host: 192.168.8.1
Content-Type: application/json
Content-Length: 180

{"key":"<session-key-from-auth>","username":"admin","he":"land@machine","he1":"land@machine:name"}
```

**响应（来自测试主机的示例结构）：**

```http
HTTP/1.1 200 OK
Content-Type: text/plain; charset=UTF-8

{"he":{"mode":"mix","broken":"disable","name":"720F-1238FE","mac":"00:03:7F:12:38:FE","macid":"00037F1238FE","language":"en"},"he1":"720F-1238FE","key":"<refreshed-session-key>"}
```

### 参考：`he` / `he1` / ... 中应该放什么

| 意图 | JSON 值中的 HE 命令行 |
|--------|------------------------|
| 完整组件 | `land@machine` |
| 单个属性 | `land@machine:name` |
| 设置单个属性 | `land@machine:name=720F-1238FE` |
| 调用方法 | `land@machine.status`（或按语法规则使用 `component.method[...]`） |

**成功设置（响应片段）：**

```json
{"he":"ttrue","key":"<new-key>"}
```

**Postman：** 在 **`/auth`** 后保存 **`key`**；每次 **`/he`** 后，将响应中的新 **`key`** 复制到下一个请求中（或使用 **Tests** 脚本：`pm.environment.set("talkkey", pm.response.json().key)`）。

---

## HE 语法（快速参考）

完整语法：**[`doc/com/land/eline.md`](../../doc/com/land/eline.md)**。

| 目标 | HE 命令行 |
|------|---------|
| 查询完整配置 | `component` 例如 `land@machine` |
| 查询单个路径 | `component:attr` 例如 `land@machine:name` |
| 设置单个路径 | `component:attr=value` |
| 调用方法 | `component.method` 或 `component.method[arg1,arg2]` |
| 方法 + 选取字段 | `component.method:field/path` |
