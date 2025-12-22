
***
## LTE/NR Modem Management
Management LTE modem baseband

#### Configuration( modem@lte )
**modem@lte** is first LTE modem
**modem@lte2** is second LTE modem

```json
// Attributes introduction 
{
    // Status for baseband
    "status":"start at system startup",    // [ "enable", "disable" ]

    // SMS function for baseband
    "sms":"SMS statue",                        // [ "disable", "enable" ]

    // GNSS function for baseband
    "gnss":"GNSS statue",                      // [ "disable", "enable" ]

    // AT port function for baseband
    "atport":"AT port statue",                 // [ "disable", "enable" ]

    // lock attributes
    "lock_nettype":"network type",             // [ "auto", "2g", "3g", "4g", "nsa", "sa" ]
    "lock_imei":"lock imei function",          // [ "disable", "enable" ]
    "lock_imsi":"lock imsi function",          // [ "disable", "enable" ]

    // custom at command
    "custom_set":                              // custom at setting list at the modem setup
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...more AT command
    },
    "custom_watch":                            // custom at watch list at the modem watch
    {
        "custom name":"AT command"             // [ string ]:[ string ]
        // ...more AT command
    },

    // watch attributes
    "watch_interval":"How often do query the modem",  // [ number ], the unit in second

    // attributes for work alone to use
    "pin":"simcard pin",                       // [ string ]
    "profile":"custom the profile",            // [ "disable", "enable" ]
    "profile_cfg":                             // custom profile save here, the json be used when "profile" value is "enable"
    {
        "dial":"dial number",                     // [ number ]
        "cid":"dial CID",                         // [ number ], default is 1
        "type":"ip address type",                 // [ "ipv4", "ipv6", "ipv4v6" ]
        "auth":"authentication method",           // [ "pap", "chap", "papchap" ]
        "apn":"APN name",                         // [ string ]
        "user":"user name",                       // [ string ]
        "passwd":"user password"                  // [ string ]
    }

}
```

Examples, show all the first LTE modem configure
```shell
modem@lte
{
    "gnss":"enable",                   # enbale the GPS function
 
    "custom_set":                      # first exeucte AT+COPS=3,2, second execute AT+CPIN=1234 at modem setup
    {
        "1":"AT+COPS=3,2",
        "2":"AT+CPIN=1234"
    },
    "custom_watch":                      # first exeucte AT+CPIN, second execute AATI at modem watch
    {
        "2":"ATI"
        "1":"AT+CPIN",
    }
}
```  

Examples, enable the SMS for first LTE modem
```shell
modem@lte:sms=enable
ttrue
```  

#### **API**
**modem@lte** is first LTE modem
**modem@lte2** is second LTE modem

+ `status[]` **get the lte modem infomation**
    - failed return NULL   
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of json by the method return
    {
        "status":"Current state",        // [ "setup", "register", "up", "idle", "reset", "down", "nodevice" ]
                                             // "setup" for setup the modem
                                             // "register" for register the network
                                             // "up" for ready to connect to internet, hint signal/network/simcard all ok
                                             // "idle" for error to idle
                                             // "reset" for reset the modem
                                             // "down" for modem is down
                                             // "nodevice" for modem cannot find
        "imei":"IMEI numer",            // [ string ]
        "imsi":"IMSI number",           // [ string ]
        "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                // number for iccid
                                                // "nosim" for cannot found the simcard
                                                // "pin" for the simcard need PIN code
                                                // "puk" for the simcard pin error
        "mversion":"modem verstion",    // [ string ]
        "name":"modem name",            // [ string ]
        "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                // number for MCC and MNC
                                                // "noreg" for cannot register to opeartor
                                                // "unreg" for cannot register to opeartor
                                                // "dereg" for register to operator be refused
        "nettype":"network type",        // The format varies depending on the module
                                         // 2G usually shows GSM, GPRS, EDGE, CDMA
                                         // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                         // 4G usually shows LTE, FDD, TDD
        "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
        "csq":"CSQ number",              // [ number ]
        "rssi":"signal intensity",       // [ number ], the unit is dBm
        "rsrp":"RSRP value",             // Optional, The format varies depending on the module
        "rsrq":"RSRQ value",             // Optional, The format varies depending on the module
        "sinr":"sinr value",             // Optional, The format varies depending on the module 
        "band":"current band",           // Optional, The format varies depending on the module
        "operator":"operator name"       // [ string ]
    }
    ```

    Example, get the first lte modem infomation
    ```shell
    modem@lte.status
    {
        "imei":"867160040494084",          # imei is 867160040494084
        "imsi":"460015356123463",          # imei is 460015356123463
        "iccid":"89860121801097564807",    # imei is 89860121801097564807
        "csq":"3",                         # CSQ nubmer is 3
        "signal":"3",                      # signal level is 3
        "state":"connect",                 # state is connect to the internet
        "plmn":"46001",                    # plmn is 46001
        "nettype":"WCDMA",                 # nettype is WCDMA
        "rssi":"-107",                     # signal intensity is -107
        "operator":"ChinaMobile"           # operator name is ChinaMobile
    }
    ```

+ `tty[ [type] ]` **get the lte modem tty device**
    - type ----------- [ string ], tty type 
    - failed return NULL   
    - succeed return string that be tty device when given type
    - succeed return json to describes tty device when no type   
    ```json
    // Attributes introduction of json by the method return
    {
        "stty":"status tty device",             // [ string ]
        "mtty":"misc tty device",               // [ string ]
        "gtty":"GNSS tty device"                // [ string ] 
    }
    ```

    Example, get the first lte modem tty list
    ```shell
    modem@lte.tty
    {
        "stty":"/dev/ttyUSB1",
        "mtty":"/dev/ttyUSB2",
        "gtty":"/dev/ttyUSB3"
    }
    ```

    Example, get the first lte modem status tty
    ```shell
    modem@lte.tty[ stty ]
    /dev/ttyUSB1
    ```

    Example, get the first lte modem GNSS tty
    ```shell
    modem@lte.tty[ gtty ]
    /dev/ttyUSB3
    ```

+ `sim[]` **get the simcard state**
    - no simcard return tfalse
    - simcard work well return ttrue
    - simcard have pin code return "pin"
    - simcard need unlock key return "puk"
    
    Example, get the first lte modem simcard state
    ```shell
    modem@lte.sim
    ttrue
    ```

    Example, get the secord lte modem simcard state
    ```shell
    modem@lte2.sim
    pin
    ```

+ `pin[ [pin code] ]` **pin code to unlock the simcard**
    - pin code ----------- [ string ] 
    - failed return tfalse
    - succeed return ttrue
    
    Example, unlock the simcard with pin
    ```shell
    modem@lte.pin[ 123456 ]
    ttrue
    ```

    Example, unlock the simcard without pin( get the pin code from lte configure )
    ```shell
    modem@lte2.pin[]
    ttrue
    ```

+ `imei[]` **get the imei of simcard**
    - failed return NULL
    - succeed return string of simcard imei
    
    Example, get the imei of first lte modem
    ```shell
    modem@lte.imei
    8986032474898527548
    ```

+ `imsi[]` **get the imsi of simcard**
    - failed return NULL
    - succeed return string of simcard imsi
    
    Example, get the imsi of first lte modem
    ```shell
    modem@lte.imsi
    460115664109131
    ```

+ `iccid[]` **get the iccid of simcard**
    - failed return NULL
    - succeed return string of simcard iccid
    
    Example, get the iccid of first lte modem
    ```shell
    modem@lte.iccid
    8986032474898527548
    ```

+ `plmn[]` **get the plmn of modem network**
    - failed return NULL
    - succeed return string of modem plmn
    
    Example, get the plmn of first lte modem
    ```shell
    modem@lte.plmn
    46011
    ```

+ `signal[]` **get the signal of modem network**
    - failed return NULL
    - succeed return string of modem signal, "0,1,2,3,4,5", "0" for no signal, "5" for good signal
    
    Example, get the plmn of first lte modem
    ```shell
    modem@lte.signal
    2
    ```

+ `operator[]` **get the modem current operator profile**
    - failed return NULL   
    - succeed return json to describes infomation   
    ```json
    // Attributes introduction of json by the method return
    {
        "name":"operator name",               // [ string ]
        "dial":"*dial number",                // [ string ]
        "apn":"APN code"                      // [ string ]
    }
    ```

    Example, get the first lte modem profile
    ```shell
    modem@lte.operator
    {
        "name":"中国联通",
        "dial":"*99#",
        "apn":"3gnet"
    }
    ```



+ `at[ at command ]` **exeucte a AT command**
    - at command ----------- [ string ]  
    - failed return NULL
    - succeed return string that be return of AT
    
    Example, talk to first lte modem AT command for simcard state
    ```shell
    modem@lte.talk[at+cpin?]
    +CPIN: READY

    OK
    ```

    Example, talk to second lte modem AT command for network state
    ```shell
    modem@lte2.talk[at+creg?]
    +CREG: 2,1,"A538","1EB3FB7",2

    OK
    ```

+ `reset[]` **reset the modem**
    - failed return NULL
    - succeed return string that be tty device when given type
    - succeed return json to describes tty device when no type  
    
    Example, reset the first lte modem
    ```shell
    modem@lte.reset
    ttrue
    ```

    Example, reset the second lte modem
    ```shell
    modem@lte2.reset
    ttrue
    ```



