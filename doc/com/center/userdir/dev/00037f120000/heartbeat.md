## <username>/dev/<macid>/heartbeat — Planned heartbeat store

### Overview

Planned directory for periodic device heartbeat / sensor history.

- Intended path: `{device_path}/<username>/dev/<macid>/heartbeat/`
- Intended file shape: sqlite databases named by month, e.g. `YYYYMM`
- Device wire notify already exists: `0+h{json}\n` from `agent@heclient` heart timer
- **Current center status: stub** — `center@heport` accepts `0+h` / `0+s` but does not write files; sqlite helpers return empty/false


### Concepts

Planned flow (not implemented yet):

1. Gateway sends `0+h{ … }` with keys from the heclient `heart` config (often `gnss@nmea`, not `gnss@nmea.info`)
2. Server stores rows under `dev/<macid>/heartbeat/YYYYMM`
3. Query/delete helpers expose history to management APIs

Until storage is implemented, do not rely on this directory existing at runtime.


### Configuration reference ( <username>/dev/<macid>/heartbeat )

```json
// Attributes introduction 
{
    "YYYYMM": "monthly sqlite database"        // [ string ], planned filename under heartbeat/; schema TBD
}
```



### Other

Related stubs in `center@heport`: heartbeat entry handler, `heartbeat2sqlite`, `sqlite2heartbeat`, sensor helpers.  
Proactive device send (`0+s`) is likewise accepted and discarded until a save path is defined.
