## ai@agent — LLM Agent

### Overview

Connect to a large-model HTTP API, answer chat, run diagnose, and optionally operate other HE objects under a whitelist.

- Reach the model using `provider` / `base_url` / `model` / `api_key`
- Bind outbound path with `extern` before the HTTP request
- `chat` one user turn; the model may issue HE calls limited by `he_read` / `he_write` / `max_he`
- `diagnose` gather + summarize (read only)
- `operate` + `confirm` + `apply` control whether writes run immediately or wait for confirm
- Standing `task`: when `task_run` is enable, run that prompt every `task_interval` seconds until stopped
- `service[]` listens on a UNIX domain stream socket; `chat` (cmd) is the eline-style client

### Configuration reference ( ai@agent )

```json
// Attributes introduction
{
    "status":"service master switch",              // [ "disable","enable" ]
                                                      // "disable": background not started, chat/diagnose fail
                                                      // "enable": accept chat/diagnose when setup has run

    "provider":"API dialect",                      // [ "openai","ollama","custom" ]
                                                      // "openai": OpenAI-compatible /v1/chat/completions
                                                      // "ollama": Ollama /api/chat
                                                      // "custom": OpenAI dialect, base_url required

    "base_url":"model server URL",                 // [ string ], empty uses provider default
                                                      // openai default: https://api.openai.com/v1
                                                      // ollama default: http://127.0.0.1:11434

    "model":"model id",                            // [ string ], required when status is enable

    "api_key":"API secret",                        // [ string ], empty if the server needs none

    "timeout":"HTTP timeout in seconds",           // [ number ], default 30

    "extern":"outbound interface before connect",  // [ string ]: [ "disable","default","ifname@wan",... ]
                                                      // empty is treated as "default"
                                                      // "disable": no outbound bind
                                                      // "default": bind default gateway
                                                      // "ifname@wan", "ifname@lte", ...: bind that interface

    "lang":"reply language",                       // [ "cn","en" ], default "cn"

    "role":"system role text",                     // [ string ], prepended to the model system prompt

    "chat":"enable chat API",                      // [ "disable","enable" ], default "enable"

    "diagnose":"enable diagnose API",              // [ "disable","enable" ], default "enable"

    "operate":"allow HE write from chat",          // [ "disable","enable" ], default "disable"

    "confirm":"hold writes until apply",           // [ "disable","enable" ], default "enable"
                                                      // used only when operate is enable

    "he_read":"HE objects allowed to query",       // [ string ], "*" or semicolon-separated names
                                                      // default "*"

    "he_write":"HE objects allowed to set",        // [ string ], empty or semicolon-separated names
                                                      // default empty; ignored when operate is disable

    "max_tokens":"max completion tokens",          // [ number ], default 2048

    "max_he":"max HE calls per chat turn",         // [ number ], default 8

    "task":"standing task prompt",                 // [ string ], empty = no standing task
    "task_interval":"seconds between task turns",  // [ number ], default 60
    "task_run":"standing task loop"                // [ "disable","enable" ], default "disable"
                                                      // "enable": after setup, run `task` every task_interval
                                                      // each tick is one chat turn (same he_read/he_write/operate/confirm)
}
```

#### Configuration example

Example, show all the agent configure
```shell
ai@agent
{
    "status":"disable",
    "provider":"openai",
    "base_url":"",
    "model":"",
    "api_key":"",
    "timeout":"30",
    "extern":"default",
    "lang":"cn",
    "role":"",
    "chat":"enable",
    "diagnose":"enable",
    "operate":"disable",
    "confirm":"enable",
    "he_read":"*",
    "he_write":"",
    "max_tokens":"2048",
    "max_he":"8",
    "task":"",
    "task_interval":"60",
    "task_run":"disable"
}
```

#### Configuration settings example

Example, enable and point at a compatible-mode server
```shell
ai@agent={"status":"enable","provider":"openai","base_url":"https://dashscope.aliyuncs.com/compatible-mode/v1","model":"qwen-plus","api_key":"sk-xxx"}
ttrue
```

Example, use local Ollama
```shell
ai@agent={"status":"enable","provider":"ollama","base_url":"http://127.0.0.1:11434","model":"qwen2.5:7b"}
ttrue
```

Example, bind outbound to ifname@lte
```shell
ai@agent:extern=ifname@lte
ttrue
```

Example, set role and language
```shell
ai@agent|{"lang":"cn","role":"现场网关助手，先查再改，改配置前先说明"}
ttrue
```

Example, allow write to ifname@lte and require confirm
```shell
ai@agent|{"operate":"enable","confirm":"enable","he_write":"ifname@lte"}
ttrue
```

Example, assign a standing task and start the loop
```shell
ai@agent|{"task":"盯着链路和定位，有异常就说明","task_interval":"60","task_run":"enable"}
ttrue
```

Example, disable the agent
```shell
ai@agent:status=disable
ttrue
```

### Concepts

**Outbound**
* When `extern` is not `disable`, the client waits for that path, then sends the model HTTP request on it.

**Chat tools**
* During `chat`, the model may request HE get on names in `he_read`, up to `max_he`.
* HE set is issued only when `operate` is `enable` and the name is in `he_write`.
* If `confirm` is `enable`, sets are returned as a proposal; `apply` executes them.

**Diagnose**
* Reads objects in `he_read`, asks the model for a short summary. No set.

**Standing task**
* When `status` is enable, `task` is not empty, and `task_run` is enable, a timer fires every `task_interval` seconds.
* Each fire is one `chat` turn with `task` as the prompt. Same `he_read` / `he_write` / `operate` / `confirm` / `max_he` rules.
* `task.stop` or `task_run=disable` ends the loop. `shut` also stops it.

**Service**
* Backend. `setup` does `sstarts("ai@agent", "ai@agent", "service")`. `service[]` must not return.
* Listen UNIX stream at `project_var_path("%s.unix", "ai@agent")`.
* One accept = one session (history kept on this connection).
* Frontend is the `chat` command; see [`chat.md`](chat.md).

### API Reference

#### Management APIs

+ `setup[]` **initialize the agent**
    - failed return tfalse
    - succeed return ttrue
    - Lifecycle method called during startup
    - When status is enable: `sstarts` `ai@agent.service`
    - If task_run is enable and task is not empty, start the standing loop
    - Not intended for manual invocation

+ `shut[]` **stop service, in-flight HTTP, standing task, and pending confirm**
    - `sdelete` / `sstop` the `ai@agent.service` child
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    ai@agent.shut
    ttrue
    ```

#### Query APIs

+ `status[]` **get client and feature state**
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "status":"run state",                 // [ "down","ready","error" ]
        "provider":"configured provider",     // [ string ]
        "base_url":"configured url",          // [ string ]
        "model":"configured model",           // [ string ]
        "extern":"configured extern",         // [ string ]
        "chat":"chat switch",                 // [ "disable","enable" ]
        "diagnose":"diagnose switch",         // [ "disable","enable" ]
        "operate":"operate switch",           // [ "disable","enable" ]
        "task_run":"standing task loop",      // [ "disable","enable" ]
        "task":"standing task prompt",        // [ string ]
        "llm":"last probe"                    // [ "unknown","ok","fail" ]
    }
    ```

    Example
    ```shell
    ai@agent.status
    {
        "status":"ready",
        "provider":"openai",
        "base_url":"https://dashscope.aliyuncs.com/compatible-mode/v1",
        "model":"qwen-plus",
        "extern":"default",
        "chat":"enable",
        "diagnose":"enable",
        "operate":"disable",
        "task_run":"disable",
        "task":"",
        "llm":"ok"
    }
    ```

#### Feature APIs

+ `service[]` **UNIX stream server (run under land@service, do not return)**
    - Bind and listen at `project_var_path("%s.unix", "ai@agent")`
    - Accept one session per connection; keep prompt history for that connection
    - Each incoming line is one chat turn; reply is one JSON object then newline
    - Line `.` or EOF closes the session; the listen loop continues
    - Requires `status=enable`
    - Started by `setup` via `sstarts("ai@agent", "ai@agent", "service")`
    - Not intended for manual invocation

    Session line protocol (UTF-8, one JSON per line from server):

    | Client line | Server |
    |-------------|--------|
    | text | `{"reply":"...","he":[...]}`  (same shape as `chat[]`) |
    | `/status` | `status[]` JSON |
    | `/diagnose` | `diagnose[]` JSON |
    | `/apply` | `{"reply":"ttrue"}` or fail |
    | `.` | close session |

    Frontend: [`chat.md`](chat.md).


+ `chat[ prompt ]` **one user turn**
    - prompt ----------------------- [ string ]
    - Requires `status=enable` and `chat=enable`
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "reply":"model text",                 // [ string ]
        "he":[ "object names used this turn" ],
        "pending":[                           // present when operate+confirm held writes
            { "object":"name", "set":{} }
        ]
    }
    ```

    Example
    ```shell
    ai@agent.chat[现在链路和定位怎么样]
    {
        "reply":"...",
        "he":["ifname@lte","gnss@nmea"]
    }
    ```

+ `diagnose[]` **read whitelist objects and summarize**
    - Requires `status=enable` and `diagnose=enable`
    - failed return NULL
    - succeed return [ json ]
    ```json
    {
        "reply":"model text",
        "peek":{}
    }
    ```

    Example
    ```shell
    ai@agent.diagnose
    {
        "reply":"...",
        "peek":{}
    }
    ```

+ `apply[]` **execute the last pending write list**
    - Requires `operate=enable`; every object must be in `he_write`
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    ai@agent.apply
    ttrue
    ```

+ `task.start[]` **start the standing task loop**
    - Requires `status=enable` and non-empty `task`
    - Sets `task_run` to enable and starts the timer
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    ai@agent:task=盯着链路和定位，有异常就说明
    ttrue
    ai@agent.task.start
    ttrue
    ```

+ `task.stop[]` **stop the standing task loop**
    - Sets `task_run` to disable
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    ai@agent.task.stop
    ttrue
    ```
