## center@api — Cloud management APIs

### Overview

Business HE APIs for the center cloud: cloud-user self-service account fields, gateway list/control, TCP/UDP port-map orchestration, mesh network endpoints, and firmware library.

- Admin account CRUD lives in **`center@ctrl`** (not in `userwui` helist); cloud login `/auth` also uses `center@ctrl.user_match` via httpd `scalls`
- Device SSL sessions and online memory stay in `center@heport`
- User/device files live under heport `device_path` (also reachable as `center@heport/<user>/...`)
- Port forwarding runtime is `center@pport`; this component persists map rules and calls pport


### Dependencies

- Requires `center@heport` running so `device_path` is registered and Unix control is available
- Port-map helpers call `center@pport`


### API Reference

#### Management APIs

**User**

Cloud-user self-service profile/password. Admin account CRUD and login `user_match`: see `center@ctrl` (`ctrl.md`).

Writable self-service fields are listed in `user_self_attrs[]` in `api/user.c` (currently `vcode` / `lang` / `comment`). Password changes use `user_passwd` (old proof required). Admin-only fields (`relay_max`, `idle_pond`, `log_file_size`, `log_file_max`, `nport`/`pport`/`log`, …): `center@ctrl.user_set` / `user_orset`. See `userdir/config.md`.

+ `user_get[ user, [attr] ]` **get one user profile (password stripped)**
    - user ------ [ string ], required; return that user config only (no list-all)
    - attr ------ [ string ], optional field path; `key` refused
    - missing user dir or config file: NULL (`ENOENT`)
    - failed return NULL
    - succeed return json for that user (or one field)
    ```json
    // Note: password "key" is stripped from the return
    {
        "lang":"language type",                 // [ string ]
        "vcode":"device register vcode",        // [ string ], optional
        "comment":"comment string",             // [ string ]
        "relay_max":"live mesh relay UDP cap",  // [ number ], optional; omitted = unlimited; 0 = none (read-only here)
        "idle_pond":"per-user idle pond",       // [ number ], optional; 0..10000; omit follows center@pport (read-only here)
        "log_file_size":"per-user log file KB", // [ number ], optional; 1..1048576; omit follows center@log (read-only here)
        "log_file_max":"per-user log file count" // [ number ], optional; 1..100000; omit follows center@log (read-only here)
    }
    ```

    Example, get user "sam"
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.user_get[ sam ]
    {
        "comment":"RealOne"
    }
    dimmalex@CLS:~/snake8$
    ```

+ `user_set[ user, value, [attr] ]` **self-service assign (`=`)**
    - user ------ [ string ], required
    - value ----- [ string | object ], required
    - attr ------ [ string ], optional; with attr must be `vcode`/`lang`/`comment`; without attr, `value` must be object whose **every** key is on that allow list (any other key → whole call fails `EPERM`)
    - empty string value deletes that allow-listed field (omit)
    - never writes `key` / `relay_max` / `idle_pond` / `log_file_size` / `log_file_max` / feature gates
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he 'center@api.user_set[ sam, en, lang ]'
    ttrue
    dimmalex@CLS:~/snake8$ he 'center@api.user_set[ sam, {"lang":"en","comment":"RealOne"} ]'
    ttrue
    ```

+ `user_orset[ user, value, [attr] ]` **self-service merge (`|`)**
    - same args and allow list as `user_set`
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~/snake8$ he 'center@api.user_orset[ sam, {"vcode":"sssss","lang":"en"} ]'
    ttrue
    ```

+ `user_passwd[ user, old_proof, new_wrap ]` **self-service change password (no plaintext on wire)**
    - user -------- [ string ], required
    - old_proof --- [ string ], same login proof: `Base64(PBKDF2-HMAC-SHA256(old, salt=user:rand, iter=10000, dkLen=32))`
    - new_wrap ---- [ string ], wrapped new password:
      - `wrap_key = PBKDF2-HMAC-SHA256(old, salt=user:rand:wrap, iter=10000, dkLen=32)`
      - `new_wrap = Base64( IV(16 random) || AES-256-CBC-PKCS7(new_password) )`
    - wrong old proof or missing user return tfalse
    - succeed return ttrue
    - WUI settings page builds proof/wrap via `authProof` / `authWrapNewPassword` in `userwui/js/auth.js`

    Example (values depend on current `rand` and passwords; prefer the settings UI)
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.user_passwd[ sam,<old_proof>,<new_wrap> ]
    ttrue
    dimmalex@CLS:~/snake8$
    ```





**Gateway**

the API can manage gateway   


+ `list[ user, [page number], [page size], [custom] ]` **get all gateway of the user**
    - user --------------- [ string ], the username of gateway
    - [page number] ------ [ number ], page number id ( use of pagination )
    - [page size]   ------ [ number ], page size ( use of pagination )
    - [custom]   --------- [ string ], specify a custom of gateway
    - failed return NULL
    - succeed return json to describes the list
    ```json
    // Attributes introduction of talk by the API return
    {
        "gateway mac identify":                // [ string ]: {}
        {
            "comment":"gateway comment",                   // [ string ], you can comment it to show
            "type":"gateway type",                         // [ "router", "switch", "camera", "senser", ... ], default is "router"
            "online":"online time",                        // [ string ]; format HH:MM:SS:Day, no exist when not online

            "mode":"gateway operator mode",                   // [ "ap", "wisp", "nwisp", "gateway", "dgateway", "misp", "nmisp", "dmisp", "mwm", "mix" ]
                                                              // "ap": access point
                                                              // "wisp": 2.4G Wireless Internet Service Provider connection
                                                              // "nwisp": 5.8G Wireless Internet Service Provider connection( need the board support 5.8G wirless baseband)
                                                              // "gateway": wire WAN gateway
                                                              // "dgateway": Dual wire WAN gateway
                                                              // "misp": LTE Mobile Internet Service Provider connection( need the board support LTE baseband)
                                                              // "nmisp": Next Mobile(NR/LTE) Internet Service Provider connection( need the board support NR/LTE baseband)
                                                              // "dmisp": Dual Mobile(LTE/NR) Internet Service Provider connection( need the board support two LTE/NR baseband)
                                                              // "mwm": custom mix connection base on wisp and mobile internet connection                                                               
                                                              // "mix": custom mix connection base on multiple internet connection 

            "name":"gateway name",                         // [ string ], The name cannot contain spaces
            "version":"gateway version",                   // [ string ]
            "macid":"gateway MAC identify or serial id",   // [ string ]
            "mac":"gateway MAC address",                   // [ string ]
            "model":"gateway model",                       // [ string ]
            "cmodel":"gateway custom model",               // [ string ]
            "datecode":"gateway data code",                // [ string ]

            "wui_port":"gateway wui server port",          // [ number ]
            "telnet_port":"gateway telnet server port",    // [ number ]
            "ssh_port":"gateway ssh server port",          // [ number ]
            "local_ip":"gateway local ip address",         // [ ip address ]

            "ifname":"gateway internet interface name",    // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
                                                                // "ifname@wan" for Gateway WAN
                                                                // "ifname@wan2" for Gateway WAN2
                                                                // "ifname@wan3" for Gateway WAN3
                                                                // "ifname@wan4" for Gateway WAN4
                                                                // "ifname@lte" for first 4G Gateway LTE
                                                                // "ifname@lte2 for second 4G/5G Gateway LTE
                                                                // "ifname@lte3 for third 4G/5G Gateway LTE
                                                                // "ifname@lte4 for fourth 4G/5G Gateway LTE
                                                                // "ifname@wisp" for WISP 2.4G
                                                                // "ifname@wisp2" for WISP 5.8G

            /////////////////////////////////////
            //    Internet interface(IPV4)     // 
            /////////////////////////////////////
            "ip":"gateway internet interface ip address",  // [ ip address ]
            "delay":"8",
            "rx_bytes":"send bytes",                       // [ number ]
            "tx_bytes":"recvie bytes",                     // [ number ]

            /////////////////////////////////////
            //    Internet interface(IPV6)     // 
            /////////////////////////////////////
            "method":"IPv6 address mode",   // [ "manual", "automatic", "slaac" ], Optional, exist when IPV6 enable
                                                // "manual" for manual setting
                                                // "automatic" for DHCPv6
                                                // "slaac" for Stateless address autoconfiguration
            "addr":"IPv6 address",          // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr2":"IPv6 address2",        // [ ipv6 address ], Optional, exist when IPV6 enable
            "addr3":"IPv6 address3",        // [ ipv6 address ], Optional, exist when IPV6 enable

                ////////////////////////////////////////////////////////////////////////////////////////////
                // show this attr when "ifname" be "ifname@lte" "ifname@lte2" "ifname@lte3" "ifname@lte4" // 
                ////////////////////////////////////////////////////////////////////////////////////////////
                "imei":"IMEI numer",            // [ string ]
                "imsi":"IMSI number",           // [ string ]
                "iccid":"ICCID number",         // [ number, "nosim", "pin", "puk" ]
                                                        // number for iccid
                                                        // "nosim" for cannot found the simcard
                                                        // "pin" for the simcard need PIN code
                                                        // "puk" for the simcard pin error
                "plmn":"MCC and MNC",           // [ number, "noreg", "dereg" ]
                                                        // number for MCC and MNC
                                                        // "noreg" for cannot register to opeartor
                                                        // "unreg" for cannot register to opeartor
                                                        // "dereg" for register to operator be refused
                "name":"modem name",             // [ string ], lte modem model or name
                "operator":"operator name",      // [ string ]
                "nettype":"network type",        // The format varies depending on the module
                                                // 2G usually shows GSM, GPRS, EDGE, CDMA
                                                // 3G usually shows WCDMA, EVDO, TDSCDMA, HSPA, HSDPA, HSUPA
                                                // 4G usually shows LTE, FDD, TDD
                "signal":"signal level",         // [ "0", "1", "2", "3", "4" ], "0" for no signal, "1" for weakest signal , "4" for strongest signal
                "rssi":"signal intensity",       // [ number ], the unit is dBm
                "csq":"CSQ number",              // [ number ], Optional
                "rsrp":"RSRP value",             // [ string ], Optional, The format varies depending on the module
                "rsrq":"RSRQ value",             // [ string ], Optional, The format varies depending on the module
                "sinr":"sinr value",             // [ string ], Optional, The format varies depending on the module  
                "band":"current band",           // [ string ], Optional, The format varies depending on the module
                "ci":"cell identity",            // [ string ], Optional
                "lac":"location area code",      // [ string ], Optional
                "channel":"location area code",  // [ string ], Optional    

                //////////////////////////////////////////////////////////////////
                // show this attr when "ifname" be "ifname@wisp" "ifname@wisp2" //
                //////////////////////////////////////////////////////////////////
                "peer":"Peer SSID",              // [ string ]
                "peermac":"Peer BSSID",          // [ MAC address ]
                "channel":"Peer channel",        // [ 1- 165 ]
                "signal":"signal level",         // [ 0, 1, 2, 3 4 ], 0 for no signal, 1 for weakest signal , 4 for strongest signal
                "rate":"connect rate",           // [ number ], Optional, the unit is M
                "rssi":"Peer RSSI",              // [ number ], Optional, the unit is dBm
                "rssp":"Peer signal percentage"  // [ number ], Optional, the unit is %


        }
        // ... more gateway
    }    
    ```   

    Example, list all gateway for user "ashyelf"
    ```shell
    dimmalex@CLS:~$ he center@api.list[ashyelf]
    {
        "303D510049B0":
        {
            "online":"22:40:24:1",             # online 1day and 22h and 40min and 24sec
            "mode":"mwm",                      # custom mix connection base on wisp and mobile internet connection
            "broken":"disable",
            "name":"WL-R320-0049B0",           # gateway name
            "version":"v8.5.1109",             # gateway software version
            "macid":"303D510049B0",            # gateway mac identify
            "model":"3218",                    # gateway hardware model
            "cmodel":"WL-R320",                # gateway custom model
            "datecode":"20231213",             # gateway date in produced
            "wui_port":"80",
            "telnet_port":"23",
            "ssh_port":"22",
            "local_ip":"192.168.8.1",
            "ifname":"ifname@lte",             # current internet connection is LTE
            "delay":"32",
            "ip":"10.51.128.182",
            "rx_bytes":"5426",
            "tx_bytes":"5473",
            "imei":"868186042111714",
            "ci":"4A37D82",
            "lac":"25E3",
            "plmn":"46001",
            "csq":"23",
            "nettype":"FDD LTE",
            "rsrp":"-97",
            "rssi":"-67",
            "rsrq":"-10",
            "sinr":"-18",
            "band":"LTE BAND 3",
            "channel":"1650",
            "signal":"4",
            "operator":"China Unicom",
            "imsi":"460018708133639",
            "iccid":"8986012580155265717"
        },
        "00037F12BB30":
        {
            "online":"22:40:58:1",             # online 1day and 22h and 40min and 58sec
            "mode":"mwm",
            "broken":"disable",
            "name":"R607-12BB30",
            "version":"v8.5.1109",
            "macid":"00037F12BB30",
            "model":"R607",
            "wui_port":"80",
            "telnet_port":"23",
            "ssh_port":"22",
            "local_ip":"192.168.8.1",
            "ifname":"ifname@wisp2",           # current internet connection is WISP(5.8G)
            "ip":"192.168.31.222",
            "rx_bytes":"220945",
            "tx_bytes":"555836",
            "peer":"dimmalex-home-5g",
            "peermac":"8A:12:4E:70:05:80",
            "channel":"36",
            "rate":"2.401",
            "rssp":"29",
            "signal":"2"
        },
        "88124E600580":
        {
            "online":"23:57:52:1",             # online 1day and 23h and 57min and 52sec
            "mode":"mix",
            "broken":"disable",
            "name":"8228-600580",
            "version":"v8.5.1109",
            "macid":"88124E600580",
            "model":"8228",
            "wui_port":"80",
            "telnet_port":"23",
            "ssh_port":"22",
            "local_ip":"192.168.31.254",
            "ifname":"ifname@wan",             # current internet connection is WAN
            "delay":"8",
            "ip":"192.168.1.5",
            "rx_bytes":"7436614",
            "tx_bytes":"15149805"
        },
        "303D51FE49B8":
        {                                      # offline, becuase no online attr
            "mode":"mwm",
            "name":"WL-R322-FE49B8",
            "version":"v8.5.1109",
            "macid":"303D51FE49B8",
            "model":"3218",
            "cmodel":"WL-R322",
            "datecode":"20231213",
            "wui_port":"80",
            "telnet_port":"23",
            "ssh_port":"22",
            "local_ip":"192.168.8.1",
            "ifname":"ifname@lte2",            # last internet connect is LTE2
            "delay":"32",
            "ip":"10.45.68.32",
            "rx_bytes":"4325465",
            "tx_bytes":"3423223",
            "imei":"868186042111773",
            "ci":"4A37D82",
            "lac":"25E3",
            "plmn":"46001",
            "csq":"23",
            "nettype":"FDD LTE",
            "rsrp":"-92",
            "rssi":"-65",
            "rsrq":"-10",
            "sinr":"-18",
            "band":"LTE BAND 3",
            "channel":"1650",
            "signal":"4",
            "operator":"China Unicom",
            "imsi":"460018708133657",
            "iccid":"8986012580155265732"
        }
    }
    ```  

+ `knock[ user, macid ]` **disconnect one online gateway session**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway (required)
    - failed return tfalse
    - succeed return talk from heport knock control (usually ttrue)

    Example, knock 303D510049B0 gateway of username ashyelf
    ```shell
    dimmalex@CLS:~$ he center@api.knock[ashyelf,303D510049B0]        # disconnect the 303D510049B0
    ttrue
    dimmalex@CLS:~$ he center@api.list[ashyelf]:303D510049B0         # show the 303D510049B0 status
    {
        "online":"00:00:05:0",                                          # the online time of 303D510049B0 is reset, because reconnection
        "mode":"mwm",
        "broken":"disable",
        "name":"WL-R320-0049B0",
        "version":"v8.5.1109",
        "macid":"303D510049B0",
        "model":"3218",
        "cmodel":"WL-R320",
        "datecode":"20231213",
        "wui_port":"80",
        "telnet_port":"23",
        "ssh_port":"22",
        "local_ip":"192.168.8.1",
        "ifname":"ifname@lte",
        "delay":"35",
        "ip":"10.51.128.182",
        "rx_bytes":"114284495",
        "tx_bytes":"36942807",
        "imei":"868186042111714",
        "ci":"086CBF1",
        "lac":"756B",
        "plmn":"46001",
        "csq":"28",
        "nettype":"FDD LTE",
        "rsrp":"-93",
        "rssi":"-58",
        "rsrq":"-17",
        "sinr":"-19",
        "band":"LTE BAND 8",
        "channel":"3740",
        "signal":"4",
        "operator":"China Unicom",
        "imsi":"460018708133639",
        "iccid":"8986012580155265717"
    }
    ```  

+ `comment[ user, macid, comment ]` **set gateway operator comment**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway; invalid format fails
    - comment ------------ [ string ], free text; may be empty to clear
    - writes `{device_path}/<user>/dev/<macid>/config`
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~$ he center@api.comment[ashyelf,303D510049B0, Office gateway]
    ttrue
    ```

+ `delete[ user, macid ]` **delete one gateway of the user**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway
    - removes this mac from the user's TCP/UDP maps and from each mesh network `endpoint`
    - knocks online session when possible, then removes `dev/<macid>/` tree
    - map/mesh cleanup is best-effort; missing records do not fail the delete
    - failed return tfalse
    - succeed return ttrue

    Example
    ```shell
    dimmalex@CLS:~$ he center@api.delete[ashyelf,303D510049B0]
    ttrue
    ```

+ `update[ user, macid, [timeout] ]` **update the gateway of the user**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - timeout ------------ [ number ], wait timeout
    - failed return tfalse
    - succeed return ttrue

    Example, update 303D510049B0 gateway of username ashyelf
    ```shell
    dimmalex@CLS:~$ he center@api.update[ashyelf,303D510049B0]        # tell the 303D510049B0 update the infomation of register
    ttrue
    ```  

+ `reboot[ user, macid, [timeout] ]` **reboot the gateway of the user**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - timeout ------------ [ number ], wait timeout
    - failed return tfalse
    - succeed return ttrue

    Example, reboot 303D510049B0 gateway of username ashyelf
    ```shell
    dimmalex@CLS:~$ he center@api.reboot[ashyelf,303D510049B0]        # tell the 303D510049B0 reboot
    ttrue
    ```  

+ `default[ user, macid, [timeout] ]` **default all configure of gateway of the user**
    - user --------------- [ string ], the username of gateway
    - macid -------------- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - timeout ------------ [ number ], optional HE timeout seconds, default 10
    - failed return tfalse
    - succeed return ttrue

    Example, default 303D510049B0 gateway of username ashyelf
    ```shell
    dimmalex@CLS:~$ he center@api.default[ashyelf,303D510049B0]        # tell the 303D510049B0 default
    ttrue
    ```  




**Port map**

Internal helpers `ttyd_hline` / `ttyd_telnet` / `ttyd_ssh` exist for `cruns` (not for normal WUI/operators).

the API can manage port proxy 


+ `tcpmap_add[ user, mac identify, ip, port, [protocol], [lock options] ]` **add a tcp map rule**
    - user --------------- [ string ], username
    - mac identify ------- [ string ], mac identify for gateway
    - ip ----------------- [ ip address ], local ip of gateway
    - port   ------------- [ port ], port of local ip of gateway
    - [ protocol ] ------- [ "tcp", "udp", "dev" ], "tcp" for TCP, "udp" for UDP, "dev" for UART, Not specified will be TCP
    - [ lock options ] --- [ number ], > 0 for lock the first connect client source address    
    - failed return tfalse
    - succeed return ttrue

    Example, add a tcp proxy map to a host that under the 88124E600580 of ashyelf, the host ip is 192.168.8.122 and port is 8000
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_add[ashyelf,88124E600580,192.168.8.122,8000]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  
    Example, add a tcp proxy map to a host that under the 303D510049B0 of ashyelf, the host ip is 192.168.8.1 and port is 80
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_add[ashyelf,303D510049B0,192.168.8.1,80]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  
    Example, add a tcp proxy map to a host that under the 303D510049B0 of ashyelf, the host ip is 192.168.8.1 and port is 23
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_add[ashyelf,303D510049B0,192.168.8.1,23]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  

+ `tcpmap_delete[ user, [mac identify], [port] ]` **delete a tcp map rule**
    - user ---------- [ string ], username
    - mac identify -- [ string ], optional; delete all maps of this gateway when port omitted
    - port   -------- [ port ], optional; required if mac identify omitted
    - at least one of mac identify / port must be set
    - failed return tfalse
    - succeed return ttrue

    Example, delete a tcp proxy that under the 303D510049B0 of ashyelf, the proxy port is 25002
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_list[ashyelf]                           # list all tcp map first 
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"8000",
            "hand_proto":"tcp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"80",
            "hand_proto":"tcp"
        },
        "25002":
        {
            "port":"25002",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"23",
            "hand_proto":"tcp"
        }
    }
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_delete[ashyelf,303D510049B0,25002]    # delete a tcp proxy second
    ttrue
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_list[ashyelf]                          # list again
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"8000",
            "hand_proto":"tcp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"80",
            "hand_proto":"tcp"
        }
    }
    dimmalex@CLS:~/snake8$
    ```  

+ `tcpmap_list[ user, [mac identify] ]` **list all tcp map rule**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway
    - missing map file or empty store returns `{}`
    - failed return NULL
    - succeed return json to describes the list
    ```json
    // Attributes introduction of talk by the API return
    {
        "proxy port":                            // [ port ]: {}
        {
            "macid":"current map device",                 [ string ]
            "hand_ip":"current map local ip",             [ ip address ]
            "hand_port":"current map local port",         [ port ]
            "hand_proto":"current map local protocol"     [ "tcp", "udp", "dev" ]
        }
        // ... more map rule
    }    
    ```
    Example, list all tcp proxy of ashyelf
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.tcpmap_list[ashyelf]
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"8000",
            "hand_proto":"tcp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"80",
            "hand_proto":"tcp"
        }
    }
    dimmalex@CLS:~/snake8$
    ```


+ `udpmap_add[ user, mac identify, ip, port, [protocol], [lock options] ]` **add a udp map rule**
    - user --------------- [ string ], username
    - mac identify ------- [ string ], mac identify for gateway
    - ip ----------------- [ ip address ], local ip of gateway
    - port   ------------- [ port ], port of local ip of gateway
    - [ protocol ] ------- [ "tcp", "udp", "dev" ], "tcp" for TCP, "udp" for UDP, "dev" for UART, Not specified will be UDP
    - [ lock options ] --- [ number ], > 0 for lock the first connect client source address    
    - failed return tfalse
    - succeed return ttrue

    Example, add a udp proxy map to a host that under the 88124E600580 of ashyelf, the host ip is 192.168.8.122 and port is 8000
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.udpmap_add[ashyelf,88124E600580,192.168.8.122,5000]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  
    Example, add a udp proxy map to a host that under the 303D510049B0 of ashyelf, the host ip is 192.168.8.1 and port is 80
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.udpmap_add[ashyelf,303D510049B0,192.168.8.1,5001]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  
    Example, add a udp proxy map to a host that under the 303D510049B0 of ashyelf, the host ip is 192.168.8.1 and port is 23
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.udpmap_add[ashyelf,303D510049B0,192.168.8.1,5002]
    ttrue
    dimmalex@CLS:~/snake8$
    ```  

+ `udpmap_delete[ user, [mac identify], [port] ]` **delete a udp map rule**
    - user ---------- [ string ], username
    - mac identify -- [ string ], optional; delete all maps of this gateway when port omitted
    - port   -------- [ port ], optional; required if mac identify omitted
    - at least one of mac identify / port must be set
    - failed return tfalse
    - succeed return ttrue

    Example, delete a udp proxy that under the 303D510049B0 of ashyelf, the proxy port is 25002
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.udpmap_list[ashyelf]                          # list all the udp map rule first
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"5000",
            "hand_proto":"udp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"5001",
            "hand_proto":"udp"
        },
        "25002":
        {
            "port":"25002",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"5002",
            "hand_proto":"udp"
        }
    }
    dimmalex@CLS:~/snake8$
    dimmalex@CLS:~/snake8$ he center@api.udpmap_delete[ashyelf,303D510049B0,25002]   # delete a udp map second
    ttrue
    dimmalex@CLS:~/snake8$ he center@api.udpmap_list[ashyelf]                        # list again
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"5000",
            "hand_proto":"udp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"5001",
            "hand_proto":"udp"
        }
    }
    dimmalex@CLS:~/snake8$
    ```  

+ `udpmap_list[ user, [mac identify] ]` **list all udp map rule**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway
    - missing map file or empty store returns `{}`
    - failed return NULL
    - succeed return json to describes the list
    ```json
    // Attributes introduction of talk by the API return
    {
        "proxy port":                            // [ port ]: {}
        {
            "macid":"current map device",               //  [ string ]
            "hand_ip":"current map local ip",           //  [ ip address ]
            "hand_port":"current map local port",       //  [ port ]
            "hand_proto":"current map local protocol"   //  [ "tcp", "udp", "dev" ]
        }
        // ... more map rule
    }    
    ```

    Example, list all udp proxy of ashyelf
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.udpmap_list[ashyelf]
    {
        "25000":
        {
            "port":"25000",
            "macid":"88124E600580",
            "hand_ip":"192.168.8.122",
            "hand_port":"5000",
            "hand_proto":"udp"
        },
        "25001":
        {
            "port":"25001",
            "macid":"303D510049B0",
            "hand_ip":"192.168.8.1",
            "hand_port":"50001",
            "hand_proto":"udp"
        }
    }
    dimmalex@CLS:~/snake8$
    ```


+ `gateway_hline[ user, mac identify ]` **create a http port for access the gateway he terminal**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - failed return NULL
    - succeed return string to describe the port

    Example, create a http port to access the he terminal for 88124E600580
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.gateway_hline[ashyelf,88124E600580]
    20006
    dimmalex@CLS:~/snake8$
    ```
    then you can use IE access the 20006 to exeucte he command on 88124E600580

+ `gateway_http[ user, mac identify, [ip], [port] ]` **create a http port for access the web server of gateway**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - ip ------------ [ string ], ip address in gateway side, none for gateway self
    - port ---------- [ string ], port of ip address, none be 80 when ip not none, none be gateway web server when ip be none
    - failed return NULL
    - succeed return string to describe the port

    Example, create a http port to access the he terminal for 88124E600580
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.gateway_http[ashyelf,88124E600580]
    20007
    dimmalex@CLS:~/snake8$
    ```
    then you can use IE access the 20007 for he command on 88124E600580

+ `gateway_telnet[ user, mac identify, [ip], [port] ]` **create a http port for access the telnet of gateway**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - ip ------------ [ string ], ip address in gateway side, none for gateway self
    - port ---------- [ string ], port of ip address, none be 23 when ip not none, none be gateway telnet server when ip be none
    - failed return NULL
    - succeed return string to describe the port

    Example, create a http port to access the telnet terminal for 88124E600580
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.gateway_telnet[ashyelf,88124E600580]
    20008
    dimmalex@CLS:~/snake8$
    ```
    then you can use IE access the 20008 for telnet on 88124E600580

+ `gateway_ssh[ user, mac identify, [ip], [port] ]` **create a http for access the ssh of gateway**
    - user ---------- [ string ], username
    - mac identify -- [ string ], mac identify of gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - ip ------------ [ string ], ip address in gateway side, none for gateway self
    - port ---------- [ string ], port of ip address, none be 22 when ip not none, none be gateway ssh server when ip be none
    - failed return NULL
    - succeed return string to describe the port

    Example, create a http port to access the ssh terminal for 88124E600580
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.gateway_ssh[ashyelf,88124E600580]
    20009
    dimmalex@CLS:~/snake8$
    ```
    then you can use IE access the 20009 for ssh on 88124E600580







**Mesh network**

Durable files under `{device_path}/<user>/net/<netid>` (see `userdir/net/mynet.md`). Runtime UDP / push is **`center@nport`**. After add/delete/knock, api writes **disk** `seq` then calls nport knock (**live** `seq` may `++`). Add one member: `endpoint_add` → `endpoint_knock` → HE `register`. Online peers get `k;seq` (they `s;` when it does not match the applied register). WaitMesh then pushes `endpoint` / `branch` / `leaf`. There is no HE `leave`. `seq` is an alignment token (disk / live / device each hold a copy); see `center@nport` Concepts.

**`relay` makes a NAT member a hub** (same role as FREE). Ordinary spokes stay `disable`. New members default to **`disable`**. The port is not saved in the net file. Status `ip`/`port` stay the hole (or static override); the borrowed UDP is `relay_port`. This is not a user `udpmap`. Offline / hole reset **keep** the remembered port. Delete / disable / no longer want unmaps **that remembered port + mac** only.

How many of those listens a username may hold at once is `<user>/config` **`relay_max`** (admin `center@ctrl` only). Omit the key = unlimited. `0` = none (even `enable` stays a leaf). A number `N` = at most N remembered borrows across that user's meshes (nport `nport_relay_limit`). Over the cap, nport does not `relay_map`. Shrinking the cap does not drop listens already up.


+ `network_add[ user, netid, [network], [keeplive interval], [keeplive failed], [keeplive timeout], [status] ]` **add a network**
    - user --------------- [ string ], username
    - netid -------------- [ string ], network identify; basename only (no `/`); reserved names rejected: gtog, cmd, net, agent, local, portc, heclient
    - network------------- [ string ], VPN CIDR, default 172.16.0.0/24
    - keeplive interval--- [ number ], endpoint keeplive to server interval, default 15, the unit is second
    - keeplive failed----- [ number ], endpoint keeplive failed times, default 4
    - keeplive timeout---- [ number ], endpoint keeplive timeout, default 15, the unit is second
    - status ------------- [ string ], optional, `enable` / `disable`; default `enable` when omitted
    - create only; existing netid (this user or another) returns tfalse (`EEXIST`)
    - to change CIDR or keepalive after create, use `network_modify`
    - writes disk `seq` = `1` then knocks `center@nport` (live loads that number)
    - failed return tfalse
    - succeed return ttrue

    Example, add a network that address is 172.16.0.0/24
    ```shell
    center@api.network_add[ ashyelf, mynet, 172.16.0.0/24 ]
    ttrue
    ```

+ `network_modify[ user, netid, [network], [keeplive interval], [keeplive failed], [keeplive timeout], [status] ]` **change an existing network**
    - user --------------- [ string ], username
    - netid -------------- [ string ], network identify; basename only (no `/`)
    - omitted parameters leave that field unchanged
    - explicit empty string also leaves that field unchanged (CIDR / keepalive / status are not cleared)
    - status ------------- [ string ], optional, `enable` / `disable`
    - does not change `endpoint` membership or reallocate `point`
    - missing network returns tfalse (`ENOENT`)
    - when at least one field changes: disk `seq` `+1` then knocks `center@nport` (live `+1` if CIDR / keepalive / disable / members changed)
    - failed return tfalse
    - succeed return ttrue

    Example, change CIDR only
    ```shell
    center@api.network_modify[ ashyelf, mynet, 10.8.0.0/24 ]
    ttrue
    ```
    Example, change keepalive interval only
    ```shell
    center@api.network_modify[ ashyelf, mynet,, 20 ]
    ttrue
    ```

+ `network_delete[ user, netid ]` **delete a network**
    - user ---------- [ string ], username
    - netid --------- [ string ], network identify; basename only (no `/`)
    - failed return tfalse
    - succeed return ttrue
    - Unmaps each member's **remembered** relay port (port+mac). After nport restart the port is forgotten until WaitMesh; delete in that window does not unmap

+ `network_list[ user, [netid] ]` **list durable network configure from disk**
    - user ---------- [ string ], required
    - netid --------- [ string ], optional; basename only (no `/`)
    - error return NULL
    - succeed return json
    - No user: `EINVAL`
    - No netid: every net file under that user, keyed by netid (`endpoint` omitted)
    - With netid: that net file without `endpoint`. Missing file: NULL (`ENOENT`)
    - Members: `endpoint_list`. Live net / hole: `network_status` / `endpoint_status`
    ```json
    {
        "network identify":
        {
            "status": "enable or disable",
            "seq": "disk seq (api write; nport live may differ until reload)",
            "network":"network address",
            "keepintval":"endpoint keeplive to server interval",
            "keepfailed":"endpoint keeplive failed times",
            "keeptimeout":"endpoint keeplive timeout"
        }
    }
    ```

    Example, list all nets of ashyelf
    ```shell
    center@api.network_list[ ashyelf ]
    ```

    Example, one net file
    ```shell
    center@api.network_list[ ashyelf, mynet ]
    ```

+ `network_status[ user, [netid] ]` **live network status from center@nport.status**
    - user ---------- [ string ], required
    - netid --------- [ string ], optional; basename only (no `/`)
    - error return NULL
    - succeed return json
    - Same parameter rules as `network_list`
    - No netid: that user’s loaded nets only (`user` / live `seq` / `status` per netid)
    - With netid: that net’s `{ user, seq, status }` only (`seq` is nport memory). Wrong user or not loaded: NULL (`ENOENT`)
    - Member run state: `endpoint_status`

    Example, live nets of ashyelf
    ```shell
    center@api.network_status[ ashyelf ]
    ```

    Example, one net live status
    ```shell
    center@api.network_status[ ashyelf, mynet ]
    ```

+ `network_knock[ user, netid ]` **reload network into center@nport and sync online members one by one**
    - user ---------- [ string ], username
    - netid --------- [ string ], network identify
    - failed return tfalse
    - succeed return ttrue
    - Does not blast full endpoint to every device at once

    Example, knock the network mynet
    ```shell
    center@api.network_knock[ ashyelf, mynet ]
    ttrue
    ```



+ `endpoint_add[ user, netid, macid, [point], [extend], [pref], [ip], [port], [listen_port], [relay] ]` **add a endpoint to network**
    - user --------------- [ string ], username
    - netid -------------- [ string ], network identify
    - mac identify ------- [ string ], mac identify for gateway    
    - point -------------- [ ip address ], optional VPN address; auto-allocated in network CIDR when omitted
    - extend ------------- [ network address ], optional, comma-separated CIDRs behind this endpoint
    - pref --------------- [ number ], optional, branch priority among reachable hubs
    - ip ----------------- [ ip address ], optional static public IP override
    - port --------------- [ number ], optional static public port override
    - listen_port -------- [ number ], optional device local WireGuard/raw listen; pushed via `agent@gtog.register`
    - relay -------------- [ string ], optional, `auto` / `enable` / `disable`; default `disable` when omitted
    - failed return tfalse
    - succeed return ttrue
    - `relay` is for a NAT hub; ordinary spokes stay `disable`
    - Does not pick a public port. `disable` never opens a center UDP; `auto` opens one when the device is online and behind NAT; `enable` always does
    - Opening still needs a free slot under that user's `relay_max` (see Mesh intro)
    - Live hole / `relay_port` come from `endpoint_status`. Offline keeps a remembered relay until delete / disable / no longer want
    - Static `ip`/`port` overrides stay admin hole overrides; they are not overwritten with the borrowed port
    - Same mac already in the net is an overwrite (upsert), then `endpoint_knock`

    Example, add endpoint with explicit point
    ```shell
    center@api.endpoint_add[ ashyelf, mynet, 00037f120000, 172.16.0.1 ]
    ttrue
    ```
    Example, add endpoint and auto-allocate point, with LAN extend
    ```shell
    center@api.endpoint_add[ ashyelf, mynet, 00037f120001, , 192.168.8.0/24 ]
    ttrue
    ```
    Example, set pref=100 and listen_port=10005 (no static hole override; relay stays disable)
    ```shell
    center@api.endpoint_add[ ashyelf, mynet, 00037f120000, 172.16.0.1, , 100, , , 10005 ]
    ttrue
    ```
    Example, always use a center UDP for this member
    ```shell
    center@api.endpoint_add[ ashyelf, mynet, 00037f120000, 172.16.0.1, , 100, , , 10005, enable ]
    ttrue
    ```

+ `endpoint_delete[ user, netid, macid ]` **delete a endpoint from network**
    - user ---------- [ string ], username
    - netid --------- [ string ], network identify
    - mac identify -- [ string ], mac identify for gateway    
    - failed return tfalse
    - succeed return ttrue
    - Removes durable membership, unmaps the remembered relay port (port+mac), HE `unregister`, then `k;seq` so online peers `s;` when it does not match

    Example
    ```shell
    center@api.endpoint_delete[ ashyelf, mynet, 00037f120001 ]
    ttrue
    ```  

+ `endpoint_list[ user, netid ]` **list durable endpoints for a network**
    - user ---------- [ string ], username
    - netid ---------- [ string ], network identify
    - error return NULL   
    - succeed return json (durable file fields; may enrich `comment`/`name`/`type` from device files)
    - Live hole / online: `endpoint_status`. Full memory: `center@nport.endpoint_dump`
    ```json
    {
        "mac identify":
        {
            "point":"endpoint address",
            "extend":"local network of endpoint",
            "pref":"branch priority",
            "ip":"optional static public ip override",
            "port":"optional static public udp port override",
            "listen_port":"optional device WireGuard listen port",
            "relay":"optional auto / enable / disable; omitted means disable",
            "comment":"from device config when present",
            "name":"from device reg when present",
            "type":"from device reg when present"
        }
    }    
    ```

    Example
    ```shell
    center@api.endpoint_list[ ashyelf, mynet ]
    {
        "00037f120000": { "point":"172.16.0.1", "extend":"192.168.8.0/24", "pref":"100", "listen_port":"10005", "relay":"enable" },
        "00037f120001": { "point":"172.16.0.2" }
    }
    ```  

+ `endpoint_status[ user, netid, [macid] ]` **live endpoint status from center@nport.endpoint_status**
    - user ---------- [ string ], required
    - netid --------- [ string ], required; basename only (no `/`)
    - macid --------- [ string ], optional, 12-hex or `xx:xx:xx:xx:xx:xx`
    - error return NULL
    - succeed return json
    - No macid: that net’s members, keyed by mac (`online` / `stale` / hole or static `ip`·`port` / `listen_port` / `nattype` 1=FREE 2=LIMIT / `relay`·`relay_port`)
    - With macid: that member’s slim object. Wrong user, net not loaded, or mac missing: NULL (`ENOENT`)

    Example, all members of mynet
    ```shell
    center@api.endpoint_status[ ashyelf, mynet ]
    ```

    Example, one member
    ```shell
    center@api.endpoint_status[ ashyelf, mynet, 00037f120000 ]
    ```

+ `network_endpoint[ user, netid ]` **same neighbor table nport pushes as `agent@gtog.endpoint`**
    - user ---------- [ string ], required
    - netid --------- [ string ], required; basename only (no `/`)
    - error return NULL
    - succeed return json (`seq` + online, not-stale peers: `point` / `extend` / `pubkey` / `nattype` / `ip`·`port` / `pref` / `relay_port`)
    - Stats the user’s net file then `nport_call` `network_endpoint`. Wrong user or file missing: NULL (`ENOENT`). Net not loaded: NULL (`ENOENT`)
    - Every device that receives HE `endpoint` at this moment gets this same object

    Example
    ```shell
    center@api.network_endpoint[ ashyelf, mynet ]
    ```

+ `endpoint_knock[ user, netid, macid ]` **reload one endpoint into center@nport and sync that device**
    - user ---------- [ string ], username
    - netid --------- [ string ], network identify
    - mac identify -- [ string ], mac identify for gateway    
    - failed return tfalse
    - succeed return ttrue
    - Missing on disk: `unregister` if it was in memory. Else `register` (CIDR / keep / listen if set) and `k;seq` to online peers. Returns a held relay if the wish no longer wants one

    Example
    ```shell
    center@api.endpoint_knock[ ashyelf, mynet, 00037f120000 ]
    ttrue
    ```  

**Firmware**


+ `firmware_add[ username, pathname ]` **add a firmware file**
    - user ------------- [ string ], username
    - pathname --------- [ string ], local path to a `.zz` firmware file
    - failed return tfalse
    - succeed return ttrue

    Example, add three firmware file to server
    ```shell
    dimmalex@CLS:~$ ls                                                                   # list firmware file
    mt7621_d228_std_v8.5.1109.zz  mt7628_r600_std_v8.5.1109.zz  mt7981_r607_std_v8.5.1109.zz
    dimmalex@CLS:~$ he center@api.firmware_add[ashyelf, ~/mt7621_d228_std_v8.5.1109.zz ]     # add the mt7621_d228_std_v8.5.1109.zz
    ttrue
    dimmalex@CLS:~$ he center@api.firmware_add[ashyelf, ~/mt7628_r600_std_v8.5.1109.zz ]     # add the mt7628_r600_std_v8.5.1109.zz
    ttrue
    dimmalex@CLS:~$ he center@api.firmware_add[ashyelf, ~/mt7981_r607_std_v8.5.1109.zz ]     # add the mt7981_r607_std_v8.5.1109.zz
    ttrue
    dimmalex@CLS:~$ he center@api.firmware_list[ashyelf]                                     # list all current firmware file
    {
        "mt7628_r600_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r600",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7628_r600_std_v8.5.1109.zz"
        },
        "mt7621_d228_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"d228",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7621_d228_std_v8.5.1109.zz"
        },
        "mt7981_r607_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r607",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7981_r607_std_v8.5.1109.zz"
        }
    }
    dimmalex@CLS:~$
    ```

+ `firmware_delete[ username, filename ]` **delete a firmware**
    - user ------------- [ string ], username
    - filename --------- [ string ], firmware basename under the user firmware dir (no `/`)
    - failed return tfalse
    - succeed return ttrue

    Example, delete a firmware
    ```shell
    dimmalex@CLS:~$ he center@api.firmware_list[ashyelf]                                  # list all current firmware file
    {
        "mt7628_r600_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r600",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7628_r600_std_v8.5.1109.zz"
        },
        "mt7621_d228_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"d228",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7621_d228_std_v8.5.1109.zz"
        },
        "mt7981_r607_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r607",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7981_r607_std_v8.5.1109.zz"
        }
    }
    dimmalex@CLS:~$ he center@api.firmware_delete[ashyelf,mt7981_r607_std_v8.5.1109.zz]   # delete firmware file mt7981_r607_std_v8.5.1109.zz
    ttrue
    dimmalex@CLS:~$ he center@api.firmware_list[ashyelf]                                  # list again
    {
        "mt7628_r600_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r600",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7628_r600_std_v8.5.1109.zz"
        },
        "mt7621_d228_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"d228",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7621_d228_std_v8.5.1109.zz"
        }
    }
    dimmalex@CLS:~$
    ```

+ `firmware_list[ user ]` **get the device firmware list of username**
    - user ------------- [ string ], username
    - missing or empty firmware dir returns `{}`
    - error return NULL
    - succeed return json to describes the list
    ```json
    // Attributes introduction of talk by the method return, that file save at $user/firmware
    {
        "firmware file name":                        // [ string ]:{}
        {
            "dir":"firmware directory",                   // [ string ]
            "custom":"firmware custom identify",          // [ string ]
            "scope":"firmware scope identify",            // [ string ]
            "version":"firmware version",                 // [ string ]
            "oem":"firmware oem",                         // [ string ]
            "zz":"firmware file name"                     // [ string ]
        }
        // ... more firmware
    }
    ```   

    Example, list all current firmware    
    ```shell
    dimmalex@CLS:~$ he center@api.firmware_list[ashyelf]
    {
        "mt7628_r600_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"r600",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7628_r600_std_v8.5.1109.zz"
        },
        "mt7621_d228_std_v8.5.1109.zz":
        {
            "dir":"/mnt/skinos/dbs/center@heport/ashyelf/firmware",
            "custom":"d228",
            "scope":"std",
            "version":"v8.5.1109",
            "zz":"mt7621_d228_std_v8.5.1109.zz"
        }
    }
    dimmalex@CLS:~$
    ```

+ `firmware_path[ username, filename ]` **get a firmware file absolute path**
    - user ------------- [ string ], username
    - filename --------- [ string ], firmware basename under the user firmware dir (no `/`)
    - failed return NULL
    - succeed return string of absolute path (composed even if the file is missing)

    Example, get the mt7628_r600_std_v8.5.1109.zz path
    ```shell
    dimmalex@CLS:~/snake8$ he center@api.firmware_path[ashyelf,mt7628_r600_std_v8.5.1109.zz]
    /mnt/skinos/dbs/center@heport/ashyelf/firmware/mt7628_r600_std_v8.5.1109.zz
    dimmalex@CLS:~/snake8$
    ```

+ `firmware_push[ username, url, mac identify, [timeout] ]` **push a firmware to gateway to upgrade**
    - user ------------- [ string ], username
    - url -------------- [ string ], url for download the firmware; no comma, brackets, whitespace, or control characters
    - mac identify ----- [ string ], mac identify for gateway; must exist under `{device_path}/<user>/dev/<macid>`
    - timeout ---------- [ number ], timeout for wait, the unit is second       
    - failed return tfalse
    - succeed return ttrue

