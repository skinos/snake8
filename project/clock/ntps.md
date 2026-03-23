***
## NTP Server management
Manage the NTP Server

#### Configuration( clock@ntps )
```json
// attribute introduction
{
    "status":"NTP server status",      // [ "disable", "enable"]
    "local":"local network interface"  // [ "ifname@lan", "ifname@lan2", ... ], network ifname name for NTP server to bind, auto-detect if not set
}
```

Example, show the configure
```shell
clock@ntps
{
    "status":"enable"           # NTP server enable
}
```
Example, disable the NTP server
```shell
clock@ntps:status=disable
ttrue
```

