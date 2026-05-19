## Use eline on the terminal to interact with HE commands

**Eline** is an interactive command-line front-end on the gateway. It uses GNU **readline** (line editing, command history) and passes most input to the same HE interpreter as the classic shell (`line_he_command`). The prompt is **`$ `** instead of **`# `**. HE syntax—querying and changing component configuration, calling methods, JSON rules—is the same as in **[he.md](he.md)**; this document repeats the essentials and describes what **eline adds** (built-in commands, **`set`** mode, and passthrough OS lines).

> **Start here**
> - **You see `$ `** — type HE **exactly as in [he.md](he.md)** (no `he` prefix).  
> - **Need BusyBox `ash` or normal shell tools?** Type **`ashy`**. After that you are at **`~ #`** and must run **`he '…'`**; see **he.md** → *How the `he` program joins arguments* (why one quoted argument matters).  
> - **Leave eline without entering shell:** **`exit`** or Ctrl+D at **`$ `**.

---

## Ways to open an eline session

You can use eline over the same kinds of access as the classic HE terminal:

- **Telnet** — enable **Telnet Server** under **&lt;System&gt;** on the web UI, then connect with a client.  
- **SSH** — enable **SSH Server** under **&lt;System&gt;**, then connect with a client.  
- **TTL / RS232 / RS485 UART** — set **Serial#TTL** / **Serial#RS232** / **Serial#485** (or **UART** / **UART2** / **UART3**) to **Command Line** under **&lt;Application&gt;**, then use a serial terminal (typical: 57600, 8N1, no flow control).

After authentication, whether you see **`$ `** (eline) or **`# `** (classic HE loop) depends on the user’s **login shell**. If the shell is set to **eline** (for example **`/usr/bin/eline`**), you get the **`$ `** prompt. You can also run **`eline`** from an existing shell when it is on **`PATH`**.

If **`/etc/banner.he`** exists, eline runs **`cat /etc/banner.he`** once before the first prompt (same style of welcome text as the classic terminal; the prompt that follows is **`$ `**).

---

## Eline vs classic HE terminal

| Aspect | Classic HE terminal (`# `) | Eline (`$ `) |
|---|---|---|
| Line editing | Depends on channel; often minimal | Readline: cursor keys, history, usual editing keys |
| Prompt | `# ` | `$ ` |
| HE commands | Yes | Yes (same formats as **he.md**) |
| Interactive **`set`** session | No | **`set <object>`** (see below) |
| Selected OS commands | Varies by product | Fixed **passthrough** list via **`shell()`** |

---

## HE command format — quick start (30 seconds)

```shell
$ land@machine
$ land@machine:name
$ land@machine:name=DemoGateway
$ land@machine.status
$ land@machine.status:version
```

| Usage | Format | Brief description | Example |
|---|---|---|---|
| Query all component configuration | `component` | Returns the full configuration object of the component. | `land@machine` |
| Query one configuration attribute | `component:attr/path` | Returns a string or JSON value at the path. | `land@machine:name` |
| Set full component configuration | `component={json}` | Replaces the component configuration with the JSON object. | `forward@alg={"ftp":"enable"}` |
| Set one configuration attribute | `component:attr/path=value` | Sets one attribute (string or JSON). | `gnss@nmea:client/server=192.168.8.251` |
| Merge multiple attributes | `component:attr/path\|{json}` or `component\|{json}` | Updates only listed fields. | `gnss@nmea:client\|{"status":"disable","proto":"tcp"}` |
| Clear one configuration attribute | `component:attr/path=` | Clears the attribute. | `gnss@nmea:client=` |
| Call method without parameters | `component.method` | Calls an API method with no parameters. | `client@station.list` |
| Call method with parameters | `component.method[param1,param2,...]` | Calls a method with parameters. | `clock@date.ntpsync[ntp1.aliyun.com]` |
| One field from method JSON | `component.method:attr/path` | Calls method and returns one field. | `ifname@lte.status:ip` |

### How to choose a format quickly

| Goal | Recommended format |
|---|---|
| Read all settings | `component` |
| Read one setting | `component:attr/path` |
| Replace full config object | `component={json}` |
| Update several fields only | `component\|{json}` or `component:attr/path\|{json}` |
| Set one field | `component:attr/path=value` |
| Clear one field | `component:attr/path=` |
| Call an API | `component.method[...]` |
| Call API and return only one field | `component.method:attr/path` |

### HE command categories and return types

+ Each HE line is one command, ended by Enter. Commands fall into: **query configuration**, **modify configuration**, and **call component method**.  
+ Typical returns: plain **string**, **`ttrue` / `tfalse`**, **JSON object** `{...}`, or empty when there is no data—same semantics as in **he.md**. After output, eline shows the next **`$ `** prompt.

For full grammar, nested examples, and troubleshooting, use **[he.md](he.md)**.

---

## Worked examples — HE commands at the `$` prompt

The samples below use **`$ `** as the eline prompt. Lines you type have **`$ `** in front; other lines are printed output.

### List all components

```shell
$ @
{ ... }                                           # component index (shape depends on firmware)
$ 
```

### Query full configuration of one component

```shell
$ land@machine
{
    "mode":"nmisp",
    "name":"ASHYELF-12AAD0",
    "mac":"00:03:7F:12:AA:D0",
    "macid":"00037F12AAD0",
    "language":"cn",
    "cfgversion":"1"
}
$ 
```

### Query one attribute and nested paths

```shell
$ land@machine:name
ASHYELF-12AAD0
$ 
$ ifname@lan:static
{
    "ip":"192.168.8.1",
    "mask":"255.255.255.0"
}
$ ifname@lan:static/ip
192.168.8.1
$ 
```

### Set one attribute (`ttrue` / `tfalse`)

```shell
$ land@machine:name=DemoGateway
ttrue
$ land@machine:name
DemoGateway
$ 
```

### Set a nested field

```shell
$ gnss@nmea:client/server=192.168.8.251
ttrue
$ gnss@nmea:client/server
192.168.8.251
$ 
```

### Replace a whole subtree with JSON

```shell
$ gnss@nmea:client={"status":"enable","server":"192.168.8.250","port":"8000"}
ttrue
$ gnss@nmea:client
{
    "status":"enable",
    "server":"192.168.8.250",
    "port":"8000",
    ...
}
$ 
```

### Merge only some fields (keep the rest)

```shell
$ gnss@nmea:client|{"status":"disable","proto":"tcp"}
ttrue
$ 
```

### Clear one attribute

```shell
$ gnss@nmea:client/server=
ttrue
$ 
```

### Call a method, then read one field from the result

```shell
$ land@machine.status
{
    "version":"1.2.3",
    ...
}
$ land@machine.status:version
1.2.3
$ 
```

### Method with parameters

```shell
$ clock@date.ntpsync[ntp1.aliyun.com]
ttrue
$ 
```

---

## Worked examples — interactive `set <object>` mode

Inside **`set`**, prompts look like **`land@machine:`**. Attribute names are **relative to that object** (same paths you would write after **`:`** in HE, e.g. **`name`**, **`static/ip`**).

### Edit and save

```shell
$ set land@machine
{
    "name":"ASHYELF-12AAD0",
    "language":"cn",
    ...
}
land@machine: name
ASHYELF-12AAD0
land@machine: name=DemoGateway
land@machine: g
{
    "name":"DemoGateway",
    "language":"cn",
    ...
}
land@machine: s
ttrue
$ land@machine:name
DemoGateway
$ 
```

### Query a nested field inside `set`

```shell
$ set ifname@lan
{ ... }
ifname@lan: static/ip
192.168.8.1
ifname@lan: e
$ 
```

### Abandon edits without saving

```shell
$ set land@machine
{ ... }
land@machine: name=WrongName
land@machine: e
$ land@machine:name
ASHYELF-12AAD0
$ 
```

### Leave `set` with Ctrl+D (same as `e` for dropping the session)

Pressing **Ctrl+D** at the **`object:`** prompt releases the in-memory config and returns to **`$ `** without **`sset`**.

---

## Worked examples — built-ins, passthrough, and session control

### Exit eline

```shell
$ exit
```

(End of session. **Ctrl+D** at **`$ `** also exits.)

### Full login shell (`ash`)

```shell
$ ashy
```

Eline is replaced by **`/bin/ash --login`**. When you leave that shell, the remote session often ends because the original eline process is gone—plan accordingly (e.g. open a second session for tests).

### Network / system commands (passthrough)

These run via the device shell, not HE:

```shell
$ ping -c 2 192.168.8.1
$ ip link show
$ ifconfig
$ route -n
$ ls /tmp
$ cd /tmp
$ curl -s http://127.0.0.1/
$ 
```

Use a **space** where the prefix requires it (e.g. **`ping `**, **`ip `**, **`ls `**). **`router`** is not treated as **`route`**.

### Readline history

```shell
$ land@machine:name
$ land@machine:language
$ 
```

Press **Up** twice to recall **`land@machine:language`**, edit, and press **Enter**.

---

## Built-in eline commands (not sent to HE)

Handled by eline before HE parsing:

| Input | Action |
|---|---|
| **`exit`** | Quit eline (exact match; e.g. `exita` is not treated as exit). |
| **`ashy`** | Replace this process with **`/bin/ash --login`**. On failure, an error is printed and the process exits. |
| **`set <object>`** | Enter **interactive configuration mode** for the HE object name (e.g. **`set land@machine`**). See next section. |

**Ctrl+D (EOF):** at the top-level **`$ `** prompt, eline exits. Inside **`set`** mode, EOF leaves **set** mode and returns to **`$ `** (in-memory config is released).

---

## Interactive `set <object>` mode

1. Enter **`set <object>`** with the same object string you would use in HE (e.g. **`set land@machine`**).  
2. Eline loads the configuration with **`sget`**, prints it, then prompts **`object:`** (e.g. **`land@machine:`**).  
3. Further lines:

| Line pattern | Meaning |
|---|---|
| **`attr=value`** | Set **`attr`** on the **in-memory** config to **`value`**. Empty value after **`=`** clears the field. Attribute paths follow HE-style naming (including **`/`** for nested paths where supported). |
| **`g`** | **Get** — print the in-memory configuration again. |
| **`s`** | **Save** — write back with **`sset`**. Prints **`ttrue`** or **`tfalse`**, then leaves **set** mode. |
| **`e`** | **Exit** — leave **set** mode without **`sset`**; in-memory object is freed. |
| No **`=`**, and not **`g`** / **`s`** / **`e`** | Treat the whole line as an attribute path for **`attr_gets`** on the in-memory config; print the value if present. |

Input lines are stored in readline **history**. Until you type **`s`**, you are editing a **working copy** only; **`e`** or EOF abandons it without **`sset`**.

---

## Passthrough lines (system shell)

If a line matches one of the rules below, eline runs it with **`shell()`** instead of HE. Matching is by **string prefix** on the whole line (no extra trimming).

| Prefix / rule | Notes |
|---|---|
| `arp ` (4 chars + space) | e.g. `arp -a` |
| `ping ` | |
| `traceroute` | First 10 characters: matches **`traceroute`**, **`traceroute6`**, … |
| `ifconfig` | First 8 characters |
| `route` | **`route`** alone or **`route `**… — **`router`** does **not** match |
| `netstat` | First 7 characters |
| `iperf` | First 5 characters (`iperf`, `iperf3`, …) |
| `tcpdump` | First 7 characters |
| `mkdir ` | |
| `telnet ` | |
| `wg ` | |
| `ip ` | |
| `tip ` | |
| `cd ` | **`cd` alone** (no space) is **not** matched → treated as HE |
| `ls ` | **`ls` alone** is **not** matched → treated as HE; use e.g. **`ls /`** |
| `tftp ` | |
| `curl ` | |
| **`reboot`** | Any line whose first 6 characters are **`reboot`** (e.g. `reboot`, `reboot -f`) |

All **other** lines go to **`line_he_command`** (same entry point as **`he`** without the shell wrapper).

---

## Tips

- Use **Up/Down** to recall previous lines (readline **history**).  
- **Blank line** or a line whose first byte is not printable is skipped (no HE call, no passthrough); it does not go into history.  
- Full HE grammar, **`he` argv joining**, return types, and **`+` / `=` / `-`** modes are in **[he.md](he.md)**.

---

## Use component documentation to manage any module (same idea as **he.md**)

You manage the gateway through **component names**, **configuration paths**, and **methods** exactly as described in each component’s markdown. Eline only changes the **prompt** (`$ `) and adds **`set`** / passthrough; the mapping from documentation to input lines is unchanged.

### Where to find component docs

- Browse **[online component documentation](../com/)** when your product ships that tree (same layout as in **he.md**). It is updated as features grow.
- In this **land** repository, many topics are also available as local files—for example **[syslog.md](syslog.md)** for syslog.
- Or contact technical support for the correct document bundle for your firmware.

### How to read a component document

- Open the project, then the component page. It usually starts with a **functional overview**.  
- **Configuration** shows the JSON shape: field names, nesting, and meanings. Those names are the **`component:attr/path`** segments you type in HE (and the attribute names you use after **`set component`**).  
- **Methods** lists callable APIs. Each becomes **`component.method`** or **`component.method[args]`** on the terminal, as documented.

Once you know the **component name** (for example **`land@syslog`** from **Configuration**), you can query, change, and invoke methods from eline using the same rules as **[he.md](he.md)**.

### Query configuration from the doc

- **Full object** — type the component name alone:

```shell
$ land@syslog
{
    "status":"enable",
    "location":"",
    "level":"info",
    "trace":"disable",
    "size":"100",
    "remote":"192.168.8.250",
    "port":"514"
}
$ 
```

- **One field** — use **`component:path`** as in the doc:

```shell
$ land@syslog:level
info
$ 
```

### Change configuration from the doc

- **Single attribute** — **`component:attr=value`**:

```shell
$ land@syslog:remote=192.168.8.250
ttrue
$ 
```

- **Merge several keys** — JSON after **`|`** (only listed keys change):

```shell
$ land@syslog|{"remote":"192.168.8.251","port":"500"}
ttrue
$ 
```

- **Replace whole configuration** — **`component={...}`** with the full JSON from the doc:

```shell
$ land@syslog={"status":"enable","location":"","level":"info","trace":"disable","size":"100"}
ttrue
$ 
```

### Call methods from the doc

Method names in the document map to **`component.method`** (and parameters to **`[...]`** if documented).

```shell
$ land@syslog.show
Dec 15 15:47:20 V520-12CC70 user.warn syslog: modem@lte check simcard failed 102 times
Dec 15 15:47:25 V520-12CC70 user.warn syslog: modem@lte check simcard failed 103 times
...                                         # more lines as returned by the device
$ land@syslog.clear
ttrue
$ 
```

### Same workflow with **`set land@syslog`**

After you read **Configuration**, you can edit that object interactively: attribute lines use **only the path part** (no repeated **`land@syslog:`** prefix on each key).

```shell
$ set land@syslog
{ ... }                                     # current JSON printed once
land@syslog: level
info
land@syslog: remote=192.168.8.250
land@syslog: g
{ ... "remote":"192.168.8.250", ... }
land@syslog: s
ttrue
$ 
```

Apply the same pattern for **any** other component: open its doc → note **component name** and field paths → use **`$ component`**, **`$ component:path`**, **`$ component:path=value`**, **`$ component.method`**, or **`$ set component`** as needed.

---

## See also

- **[he.md](he.md)** — complete HE command reference.
