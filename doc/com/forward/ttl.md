***
## TTL settings
Management of out IP packet TTL

#### Configuration( forward@ttl )
```json
// Attributes introduction 
{
    "mode":"mode fo ttl settings",     // [ "none", "fix", "inc", "dec" ], ttl modify mode
    "fix":"output packet TTL",         // [ number ], vaild when "mode" be "fix"
    "inc":"increase the TTL",          // [ number ], vaild when "mode" be "inc"
    "dec":"reduce the TTL"             // [ number ], vaild when "mode" be "dec"
}
```   

Example, show current all of ttl configure
```shell
forward@ttl
{
    "mode":"fix",     # fix the output packet ttl
    "fix":"99",       # ttl number is 99
}
```  

Example, modify the ttl 70
```shell
forward@ttl:fix=70
ttrue
```  

