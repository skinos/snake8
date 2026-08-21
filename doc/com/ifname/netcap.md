## ifname@netcap — Network Packet Capture

### Overview

Shell component (flat `exe` script under `project/ifname/netcap`) for background
**tcpdump** on logical interfaces (`ifname@lte`, `ifname@wan`, …).

Capture files go under **`NETCAP_DIR`** (default `/tmp/file`, usable via `wui@file`).


### Configuration reference ( ifname@netcap )

Edit macros at the top of the script:

| Macro | Default | Meaning |
|-------|---------|---------|
| `NETCAP_PROTO` | `""` (all) | tcpdump BPF filter |
| `NETCAP_SIZE_MB` | `10` | Max pcap size in MB (`tcpdump -C`); then exit |
| `NETCAP_DIR` | `/tmp/file` | Output directory for `netcap-*.pcap` |


### API Reference

#### -

##### stop

- Description: Stop all `netcap-*` tcpdump processes started by this component.
- Returns: `ttrue`

#### +

##### lte / lte2 / wan / wan2 / wisp / wisp2 / lan

- Description: Start background tcpdump on `ifname@<name>.netdev` → `NETCAP_DIR/netcap-<name>.pcap`. Re-call replaces the previous capture for that name. Stops after `NETCAP_SIZE_MB`.
- Returns: `ttrue` / `tfalse`

```shell
$ ifname@netcap.lte
ttrue
$ ifname@netcap.stop
ttrue
```
