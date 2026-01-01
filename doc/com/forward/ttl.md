***
## TTL settings
Management of out IP packet TTL of extern inteface

#### Configuration( forward@ttl )
```json
// Attributes introduction 
{
    "local interface name":       // [ "ifname@lan", "ifname@lan2", "ifname@lan3", ... ], above rules set at this interface name
    {
        "mode":"mode fo ttl settings",     // [ "none", "fix", "inc", "dec" ], ttl modify mode
        "fix":"output packet TTL",         // [ number ], vaild when "mode" be "fix"
        "inc":"increase the TTL",          // [ number ], vaild when "mode" be "inc"
        "dec":"reduce the TTL"             // [ number ], vaild when "mode" be "dec"
    }
    // ... more extern interface name

}
```   

Example, show current all of ttl configure
```shell
forward@ttl
{
    "ifname@lan":
    {
        "mode":"fix",     # fix the output packet ttl
        "fix":"99",       # ttl number is 99
    }
}
```  

Example, modify the ttl 70
```shell
forward@ttl:ifname@lan/fix=70
ttrue
```  

