## chat — terminal frontend for ai@agent.service

### Overview

A command-line frontend, same role as **eline**: sit in a terminal, readline, `$ ` prompt, talk to a user.

It does not implement the model, diagnose, or HE. Every line goes to **`ai@agent.service`** over the UNIX stream socket.

```
  user  <-->  chat (frontend)  <-->  UNIX  <-->  ai@agent.service
```

- Binary: `chat` (`cmd` in `project/ai`)
- Socket: `project_var_path("%s.unix", "ai@agent")`
- Service must be running (`ai@agent.setup` has started `ai@agent.service`)

### Use

```shell
$ chat
$ 现在链路和定位怎么样
{
    "reply":"...",
    "he":["ifname@lte","gnss@nmea"]
}
$ /status
{ "status":"ready", "model":"qwen-plus" }
$ exit
```

One line without entering the prompt:

```shell
chat 现在链路怎么样
```

### Built-in (handled in the frontend)

| Input | Action |
|-------|--------|
| `exit` | Leave `chat` (Ctrl+D same) |
| `/status` | send to service, print reply |
| `/diagnose` | send to service, print reply |
| `/apply` | send to service, print reply |
| other text | send as one chat turn, print reply |

Blank line is ignored. Up/Down is readline history.

If the socket is not there, print an error and exit.
