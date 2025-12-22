***
## Management of Application Layer Gateway  

#### Configuration( forward@alg )   

```json
// Attributes introduction 
{
    "amanda":"amanda ALG function",            // [ "disable", "enable" ]
    "ftp":"ftp ALG function",                  // [ "disable", "enable" ]
    "h323":"h323 ALG function",                // [ "disable", "enable" ]
    "irc":"irc ALG function",                  // [ "disable", "enable" ]
    "pptp":"pptp ALG function",                // [ "disable", "enable" ]
    "gre":"gre ALG function",                  // [ "disable", "enable" ]
    "sip":"sip ALG function",                  // [ "disable", "enable" ]
    "rtsp":"rtsp ALG function",                // [ "disable", "enable" ]
    "snmp":"snmp ALG function",                // [ "disable", "enable" ]
    "tftp":"tftp ALG function"                 // [ "disable", "enable" ]
}
```   

Example, show current all ALG settings
```shell
forward@alg
{
    "amanda":"disable",
    "ftp":"enable",
    "h323":"disable",
    "irc":"disable",
    "pptp":"enable",
    "gre":"enable",
    "rtsp":"enable",
    "sip":"enable",
    "snmp":"disable",
    "tftp":"disable"
}
```   

Example, disable the FTP ALG
```shell
forward@alg:ftp=disable
ttrue
```   

Example, disable the sip ALG
```shell
forward@alg:sip=disable
ttrue
```   

Example, enable the h323 ALG
```shell
forward@alg:h323=enable
ttrue
```   

Example, show the FTP ALG settings
```shell
forward@alg:ftp
disable
```   

