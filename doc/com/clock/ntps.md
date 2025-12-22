***
## NTP Server management
The management NTP Server

#### Configuration( clock@ntps )
```json
// attribute introduction
{
    "status":"NTP server status"       // [ "disable", "enable"]
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

