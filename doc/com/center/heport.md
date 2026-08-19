## center@heport — Gateway HE forwarding service

### Overview

Accept gateway SSL connections, forward HE commands, and host HTTP `/auth` and `/hh`.

- Business management HE APIs moved to **`center@api`** (see `api.md`)
- User/device database directory is still owned here (`device_path`)
- Mesh networking is no longer handled by heport (to be redesigned in a separate program)


### Configuration reference ( center@heport )


```json
// attribute introduction
{
    "status":"enable the function",                       // [ "disable", "enable" ]

    "port":"tcp port for gateway connect in ssl",         // [ number ], 1 to 65535
    "api_port":"http port for connect to control",        // [ number ], 1 to 65535

    "talk_timeout":"timeout to disconnect",               // [ number ], The unit is seconds
    "key_lifetime":"auth key lifetime for http access"    // [ number ], The unit is seconds
}
```   
Example, show all the configure  
```shell
dimmalex@CLS:~/snake8$ he center@heport
{
    "status":"enable",
    "port":"20002",
    "api_port":"20003",
    "talk_timeout":"25",
    "key_lifetime":"300"
}
dimmalex@CLS:~/snake8$ 
```  
Example, modify the he port to 10002   
```shell
dimmalex@CLS:~/snake8$ he center@heport:port=10002
ttrue
dimmalex@CLS:~/snake8$ 
```   
Example, modify the api port to 10003  
```shell
dimmalex@CLS:~/snake8$ he center@heport:api_port=10003
ttrue
dimmalex@CLS:~/snake8$ 
```


### API Reference

#### Control APIs

+ `setup[]` —
    start the heport service when `status` is `enable`

+ `shut[]` —
    stop the heport service


