# Joint events

A **joint** is a system-wide event string (for example `network/online`). Publishers call `land@joint.call`. Each FPK can subscribe by listing HE methods under `"joint"` in `prj.json`. When the event fires, those methods run.

This page is how to **subscribe** from a project. Runtime APIs: [`../com/land/joint.md`](../com/land/joint.md). Implement the handler on a comexe ([comexe.md](comexe.md)) or a shell component ([shell.md](shell.md)). Boot-time work that is not event-driven belongs in [init.md](init.md).

---

## Generate with `prj`

From the SDK root:

```bash
prj add_joint myproj network/online myproj@sensor.online
```

| Argument | Meaning |
|----------|---------|
| project | Directory under `project/` |
| event | Joint name (`network/online`, `storage/insert`, …) |
| API | HE method to call |

Result in `prj.json`:

```json
"joint": {
    "network/online": {
        "myproj@sensor.online": ""
    }
}
```

Add more events with more `prj add_joint` lines (or edit JSON). The method name does not have to match the event; `_online` for `network/online` is only a convention. The C symbol is `_` + HE suffix (`_online`, `_on`, `_onextern`).

---

## Common events

| Event | When (typical) |
|-------|----------------|
| `network/on` | A local interface came up |
| `network/onextern` | An external / WAN-side interface came up |
| `network/online` | Internet / default route is usable |
| `network/offline` | That connectivity went away |
| `storage/insert` | Removable storage appeared |
| `storage/remove` | Removable storage disappeared |

Products may publish others (`machine/status`, …). Subscribe only to events you handle; an empty handler still costs a process hop.

Payload is JSON. Network events usually include `ifname`. Always check pointers before use.

---

## Implement the handler

The comexe template already has `_online`:

```c
boole_t _online( obj_t this, param_t param )
{
    talk_t ms;
    const char *event;
    const char *object;
    char *payload_txt;

    object = obj_name( this );
    event = param_string( param, 1 );
    ms = param_talk( param, 2 );
    payload_txt = json2string( ms );
    app_info( "%s: _online event=%s payload=%s", object,
            event != NULL ? event : "(null)",
            payload_txt != NULL ? payload_txt : "(null)" );
    if ( payload_txt != NULL )
    {
        free( payload_txt );
    }
    return ttrue;
}
```

Rules:

1. **Finish quickly.** Joint dispatch waits. Do not `sleep` a long time or block on a daemon loop here. Start or reset a service (`cstart` / `creset`) and return.
2. `param_string( param, 1 )` is the event name; `param_talk( param, 2 )` is the payload. Index is **1-based**.
3. `json_string( ms, "ifname" )` only after `ms != NULL`.
4. Put `_online` (or `_on`, …) in `exe_api_table` or the call fails.

A WAN-up restart (from [`testcom.c`](../../project/tmptools/testcom/testcom.c)):

```c
creset( this, "service", NULL, object );
return ttrue;
```

Separate methods if local vs external vs internet need different work:

```bash
prj add_joint myproj network/on myproj@sensor.on
prj add_joint myproj network/onextern myproj@sensor.onextern
prj add_joint myproj network/online myproj@sensor.online
```

### Shell

Add a function with the same HE name and `creturn ttrue`. Keep it short; call `he` for further work if needed.

---

## Publishing (optional)

If **your** component should notify others, call `land@joint.call` with the event name and payload (see [`../com/land/joint.md`](../com/land/joint.md)). Document published names in the component interface `.md` ([component_doc_guide.md](component_doc_guide.md)). Most application projects only **subscribe**.

---

## Check

```bash
he 'land@joint.list'
he 'myproj@sensor.online'
```

Triggering a real `network/online` depends on the device having a WAN. On a host build you can still invoke the method by hand with dummy parameters to see the log line.

Related: [`../com/land/he.md`](../com/land/he.md), [`../com/land/skin.md`](../com/land/skin.md) (`param_string` / `param_talk`).
