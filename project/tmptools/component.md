## 此文件为说明如何编写组件接口文件的格式, 章节, 风格指导

1. 通常组件接口文档存放于项目目录下, 与组件源代码目录存放于相同的目录, 用于专门描述此组件的使用逻辑及向外提供的配置及接口
2. 组件接口文档名称通常以 <组件名称>.md 命名
3. 所有组件接口文档的风格及章节必须与此指导文档一致, 对应的章节内的内容跟据组件的实际实现会不同
4. 以下文档描述的章节如对应的组件不提供则不应包括此章节
5. <>内的内容需要跟据组件的实际信息填充
6. //'' 内的内容是给编写接口文档的人或AI介绍如何编写此段内容, 不应出现在接口文档中
---



## <组件全名> — <简单介绍>

### Overview

<此处介绍此组件实现什么功能, 有什么特点, 等等>


### Architecture

<此处可以详细描述此组件在系统的的使用或是设计逻辑以及向上或向下层的关系, 或者会提供什么概念>


### Dependencies

<介绍依赖关系>


---



### Configuration ( <组件全名> )

#### Configuration attributes

```json
// 
{

    //'组件配置中会有很多属性, 每个属性都各不相同, 在编写此属性时首先要确认属性属于以下哪种类型, 然后确定用哪种格式描述, 注意当属性值是一个JSON时,  这个JSON内又同样可以有很多属性, 可以一层层嵌套'


    //'1, 属性名固定的属性, 即属性名由组件代码约定不可变

        //'a. 可选值属性: 当属性值只有几个可选值时使用此方式描述, 属性名由组件代码约定不可变, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",             // [ "<value>", "<value2>", "value3>", ... ]
                                                                      // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                      // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                      // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                      // <介绍默认值>

        //'b. 字符串值属性: 当属性值是字符串时使用此方式描述, 属性名由组件代码约定不可变, 属性值为任意字符串'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ string ], <attribute value的附加介绍, 如默认值等>

        //'c. 数字值属性: 当属性值是数字时使用此方式描述, 属性名由组件代码约定不可变, 属性值为数字'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>

        //'d. IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性名由组件代码约定不可变, 属性值为IP地址'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ ip address ], <attribute value的附加介绍, 如默认值等>

        //'e. MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性名由组件代码约定不可变, 属性值为MAC地址'
        //'格式如下:'
        "<attribute>": "<attribute value introduce>",                   // [ mac address ], <attribute value的附加介绍, 如默认值等>

        //'f. JSON值属性: 当属性值是JSON时使用此方式描述, 属性名由组件代码约定不可变, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute>": { JSON },                                        // [ json ], <这个JSON的概要性介绍>


    //'2, 可选名属性, 即属性名只有几个可选项, 通常这种属性存在于一个列表中, 最常用于接口列表的属性介绍, 这种情况下接口名称会作为属性的可选名

        //'a. 可选名可选值属性: 当属性值只有几个可选值是使用此方式描述, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ "<name>", "<name2>", "name3>", ... ]:  [ "<value>", "<value2>", "value3>", ... ]
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                            // <...>
                                                                            // <介绍默认值或其它附加介绍>

        //'b. 可选名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ "<name>", "<name2>", "name3>", ... ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>

        //'c. 可选名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ "<name>", "<name2>", "name3>", ... ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>

        //'d. 可选名IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性值为IP地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ "<name>", "<name2>", "name3>", ... ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>

        //'e. 可选名MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性值为MAC地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ "<name>", "<name2>", "name3>", ... ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>

        //'f. 可选名JSON值属性: 当属性值是JSON时使用此方式描述, 属性名由组件代码约定不可变, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ "<name>", "<name2>", "name3>", ... ]: [ json ], <这个JSON的概要性介绍>
                                                                       // "<name>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name2>": <关于当attribute name为此选项时作用及介绍>
                                                                       // "<name3>": <关于当attribute name为此选项时作用及介绍
                                                                       // <...>


    //'3, 字符串名属性, 即属性名是任意字符串时使用此方式描述, 通常这种属性存在于一个列表中, 最常用于规则名, 这种情况下规则名可以是任意的字符串

        //'a. 字符串名可选值属性: 当属性值只有几个可选值是使用此方式描述, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ string ]: [ "<value>", "<value2>", "value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                            // <...>
                                                                            // <介绍默认值或其它附加介绍>

        //'b. 字符串名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ string ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. 字符串名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ string ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. 字符串名IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性值为IP地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ string ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. 字符串名MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性值为MAC地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ string ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. 字符串名JSON值属性: 当属性值是JSON时使用此方式描述, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ string ]: [ json ], <这个JSON的概要性介绍>
                                                                       // <attribute name的附加介绍>


    //'4, 数字名属性, 即属性名是数字时使用此方式描述, 通常这种属性存在于一个列表中, 最常用于有顺序的规则名, 这种情况下规则名可以是数字

        //'a. 数字名可选值属性: 当属性值只有几个可选值是使用此方式描述, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ number ]: [ "<value>", "<value2>", "value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                            // <...>
                                                                            // <介绍默认值或其它附加介绍>

        //'b. 数字名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ number ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. 数字名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ number ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. 数字名IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性值为IP地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ number ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. 数字名MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性值为MAC地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ number ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. 数字名JSON值属性: 当属性值是JSON时使用此方式描述, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ number ]: [ json ], <这个JSON的概要性介绍>
                                                                       // <attribute name的附加介绍>


    //'5, IP地址属性, 即属性名是IP地址时使用此方式描述, 通常这种属性存在于一个列表中, 列表内都是指定的IP地址为规则名组成的规则定义等

        //'a. IP地址名可选值属性: 当属性值只有几个可选值是使用此方式描述, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ "<value>", "<value2>", "value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                            // <...>
                                                                            // <介绍默认值或其它附加介绍>

        //'b. IP地址名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. IP地址名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. IP地址名IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性值为IP地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. IP地址名MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性值为MAC地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. IP地址名JSON值属性: 当属性值是JSON时使用此方式描述, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {},                              // [ ip address ]: [ json ], <这个JSON的概要性介绍>
                                                                       // <attribute name的附加介绍>



    //'6, MAC地址属性, 即属性名是MAC地址时使用此方式描述, 通常这种属性存在于一个列表中, 列表内都是指定的MAC地址为规则名组成的规则定义等

        //'a. MAC地址名可选值属性: 当属性值只有几个可选值是使用此方式描述, 属性值约定的为几个可选值'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ "<value>", "<value2>", "value3>", ... ]
                                                                       // <attribute name的附加介绍>
                                                                            // "<value>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value2>": <关于当attribute value为此选项时作用及介绍>
                                                                            // "<value3>": <关于当attribute value为此选项时作用及介绍
                                                                            // <...>
                                                                            // <介绍默认值或其它附加介绍>

        //'b. MAC地址名字符串值属性: 当属性值是字符串时使用此方式描述, 属性值为任意字符串'
        //'格式如下:'
        "<attribute name introduce>": "<attribute value introduce>",   // [ ip address ]: [ string ], <attribute value的附加介绍, 如默认值>
                                                                       // <attribute name的附加介绍>

        //'c. MAC地址名数字值属性: 当属性值是数字时使用此方式描述, 属性值为数字'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ number ], <attribute value的附加介绍, 如默认值等, 如有取值范围也需介绍>
                                                                       // <attribute name的附加介绍>

        //'d. MAC地址名IP地址值属性: 当属性值是IP地址时使用此方式描述, 属性值为IP地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ ip address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'e. MAC地址名MAC地址值属性: 当属性值是MAC地址时使用此方式描述, 属性值为MAC地址'
        //'格式如下:'
        "<attribute name introduce>": "<attribute introduce>",         // [ ip address ]: [ mac address ], <attribute value的附加介绍, 如默认值等>
                                                                       // <attribute name的附加介绍>

        //'f. MAC地址名JSON值属性: 当属性值是JSON时使用此方式描述, 属性值为JSON, JSON为下层性的集合, 其又可包含各种类型的属性'
        //'格式如下:'
        "<attribute name introduce>": {}                              // [ ip address ]: [ json ], <这个JSON的概要性介绍>
                                                                       // <attribute name的附加介绍>

}
```

Example, show all the configure
```shell
<组件全名>
{
    //'典型的尽可能多的属性的示例'
    "<attribute>":"<value>",                    # <当前属性当前值时的介绍>
    <...>
}

```


---

## API Reference

### Management APIs

#### `setup[]` - Setup network infrastructure
- Prepares the network framework (routing policy, firewall hooks, registration of local and external logical interfaces).
- When multi-link settings exist for the current device mode, starts the **multi-link scheduler service** in the background.

#### `shut[]` - Shutdown network infrastructure
- Stops the multi-link scheduler (if running), tears down registrations, and clears framework-managed firewall state as appropriate.

#### `service[]` - Start multi-link scheduler
- Used internally to launch the scheduler executable that performs uplink selection; ordinary administration goes through `network@frame` and configuration.

### Query APIs

#### `status[]` - Show external connections status

Returns the status of external connections when multiple external connections coexist.

**Returns:**
- `NULL` - Failed
- `terror` - Error (e.g., wrong mode of operation, only works in multi-connection mode)
- JSON object describing network information

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {           // [ "ifname@wan", "ifname@wan2", "ifname@wan3", "ifname@wan4", "ifname@lte", "ifname@lte2", "ifname@lte3", "ifname@lte4", "ifname@wisp", "ifname@wisp2" ]
        "status": "Whether online",    // [ "nodevice", "reset", "setup", "register", "uping", "scanning", "block", "up", "failed", "down" ], "up" for online
        "inuse": "Whether used"        // [ "disable", "enable" ], enable for in used, disable for not used
    }
}
```

**Status Values:**
- `nodevice` - Corresponding module could not be found
- `reset` - Reset the device
- `setup` - Setup the connection
- `register` - Register to peer
- `uping` - Connecting
- `scanning` - Scanning the peer
- `block` - Wait keeplive succeed
- `up` - Ready to connect to internet (signal/network/simcard all ok)
- `failed` - Keeplive failed
- `down` - The ifname is down

**Example:**
```shell
network@frame.status
{
    "ifname@lte": {
        "status": "up",
        "inuse": "enable"
    },
    "ifname@lte2": {
        "status": "down",
        "inuse": "disable"
    }
}
```

#### `list[]` - List all connections

Returns a list of all registered connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": "The corresponding ifdev object"    // [ "ifname@wan", ... ]: [ string ]
}
```

**Example:**
```shell
network@frame.list
{
    "ifname@lan": "bridge@lan",
    "ifname@lte": "modem@lte",
    "ifname@lte2": "modem@lte2"
}
```

#### `local[]` - List all local connections and information

Returns detailed information about local (LAN) connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {
        "status": "Current state",        // [ "uping", "down", "up" ]
        "mode": "IPV4 address mode",      // [ "dhcpc" ] for DHCP, [ "static" ] for manual setting
        "netdev": "netdev name",          // [ string ]
        "ifdev": "ifdev name",            // [ string ], Optional
        "gw": "gateway ip address",       // [ ip address ], Optional
        "dns": "dns ip address",          // [ ip address ], Optional
        "dns2": "dns2 ip address",        // [ ip address ], Optional
        "ip": "ip address",               // [ ip address ]
        "mask": "network mask",           // [ ip address ]
        "ontime": "online uptime",        // [ string ], Optional, online system uptime
        "livetime": "online time",        // [ string ], format is hour:minute:second:day
        "rx_bytes": "received bytes",     // [ number ]
        "rx_packets": "received packets", // [ number ]
        "tx_bytes": "transmitted bytes",  // [ number ]
        "tx_packets": "transmitted packets",// [ number ]
        "mac": "MAC address",             // [ mac address ]
        "method": "IPv6 address mode",    // [ "manual", "automatic", "slaac" ], Optional
        "addr": "IPv6 address",           // [ ipv6 address ], Optional
        "addr2": "IPv6 address2",         // [ ipv6 address ], Optional
        "addr3": "IPv6 address3"          // [ ipv6 address ], Optional
    }
}
```

#### `extern[]` - List all extern connections and information

Returns detailed information about external (WAN) connections.

```json
// Attributes introduction of talk by the method return
{
    "ifname object": {
        "status": "Current state",
        "mode": "IPV4 address mode",      // [ "dhcpc", "static", "pppoe" ]
        "netdev": "netdev name",
        "ifdev": "ifdev name",
        "gw": "gateway ip address",
        "dns": "dns ip address",
        "dns2": "dns2 ip address",
        "ip": "ip address",
        "mask": "network mask",
        "delay": "delay time",            // [ "failed", "block", number ]
        "ontime": "online uptime",
        "livetime": "online time",
        "rx_bytes": "received bytes",
        "rx_packets": "received packets",
        "tx_bytes": "transmitted bytes",
        "tx_packets": "transmitted packets",
        "mac": "MAC address",
        "method": "IPv6 address mode",
        "addr": "IPv6 address",
        
        // LTE specific attributes (for ifname@lte, ifname@lte2, etc.)
        "imei": "IMEI number",
        "imsi": "IMSI number",
        "iccid": "ICCID number",          // [ number, "nosim", "pin", "puk" ]
        "plmn": "MCC and MNC",            // [ number, "noreg", "dereg" ]
        "name": "modem name",
        "operator": "operator name",
        "nettype": "network type",
        "signal": "signal level",         // [ "0", "1", "2", "3", "4" ]
        "rssi": "signal intensity",
        "csq": "CSQ number",
        "rsrp": "RSRP value",
        "rsrq": "RSRQ value",
        "sinr": "sinr value",
        "band": "current band",
        "ci": "cell identity",
        "lac": "location area code",
        "channel": "channel",
        
        // WISP specific attributes (for ifname@wisp, ifname@wisp2)
        "peer": "Peer SSID",
        "peermac": "Peer BSSID",
        "rate": "connect rate",
        "rssp": "Peer signal percentage"
    }
}
```

#### `vpn[]` - List all VPN connections and information

Returns information about VPN connections.

```json
{
    "ifname object": {
        "mode": "current mode",
        "status": "current state",
        "ifdev": "corresponding ifdev object",
        "netdev": "netdev name of linux",
        "ip": "IP address",
        "rx_bytes": "received bytes",
        "rx_packets": "received packets",
        "tx_bytes": "transmitted bytes",
        "tx_packets": "transmitted packets",
        "mac": "MAC address"
    }
}
```

#### `outer[]` - List all extern and VPN connections

Combines extern and VPN connection information.

#### `default[]` - Get current default connection

Returns the current default gateway connection information.

- Call without arguments: return JSON with connection info
- Call with one argument (ifname): return `ttrue` when matched, otherwise `tfalse`

#### `gateway[]` - Get current gateway connection

Returns the current gateway connection information (similar to `default[]`).

### Interface Management APIs

#### `register[ifname,concom,ifdev,type]` - Register an interface

Registers a network interface with the frame.

**Parameters:**
- `ifname` - Interface name (e.g., "ifname@lte")
- `concom` - Connection component name
- `ifdev` - Interface device name
- `type` - Interface type ("local", "extern", "vpn")

#### `unregister[ifname,type]` - Unregister an interface

Unregisters a network interface from the frame.

#### `add[ifdev,netdev]` - Add a device

Adds a network device to the frame (VLAN or Bridge).

#### `delete[ifdev]` - Delete a device

Removes a network device from the frame.

### Online/Offline APIs

#### `online[info]` - IPv4 online notification

Called when an interface comes online (IPv4).

**Info JSON structure:**
```json
{
    "ifname": "interface name",
    "ifdev": "device name",
    "netdev": "network device",
    "ip": "IP address",
    "mask": "network mask",
    "gw": "gateway",
    "dns": "DNS server",
    "dns2": "backup DNS"
}
```

#### `offline[ifname]` - IPv4 offline notification

Called when an interface goes offline (IPv4).

#### `upline[info]` - IPv6 online notification

Called when an interface comes online (IPv6).

#### `downline[ifname]` - IPv6 offline notification

Called when an interface goes offline (IPv6).

---

## Connect Service

The connect service is a separate process that manages multi-link connections. It is started by the frame component when multi-link mode is configured.

### Features

1. **Cold backup**
   - Only one uplink is active at a time; lower-priority links are taken down when a higher-priority one is used.
   - When the default route changes, existing connection tracking may be cleared so sessions are not stuck on the old path.

2. **Hot backup**
   - Multiple uplinks may remain up; the **default route** tracks the best available slot (smallest index that is online).
   - If the preferred uplink fails, traffic moves to the next eligible slot automatically.

3. **Lazy backup**
   - Like hot backup, but after failing over it **does not move back** to a higher-priority uplink when that uplink recovers, until the current uplink fails again — reducing flip-flop between links.

For `hot2`…`hot5` and `lazy2`…`lazy5`, only the first *N* numbered slots participate in **scheduler decisions**; additional slots may still appear in configuration but are outside that policy’s scan (see the table above).

### Signals

The connect service responds to the following signals:

- `SIGHUP` - Refresh connections
- `SIGTERM` / `SIGINT` - Graceful shutdown
- `SIGPIPE` - Ignored (prevents crash on broken pipe)

### Control Interface

On systems that ship the maintenance CLI, you can signal the scheduler with:

```shell
# Exit the connect service
connect exit

# Refresh connections
connect flush
```

### Status Query

```shell
# Query connect service status
network@frame.status
```

---

## Deployment notes

### Paths and storage

Install layout (where libraries, the scheduler binary, runtime sockets, and registration files live) is decided by the **firmware image** and project packaging. Operators should use **`he`**, the Web UI, or product-specific tools — not hard-coded paths.

### Firewall and policy routing

The framework creates the **iptables/nftables structures** needed for outbound NAT, policy routing marks, and shunting. Exact chain names visible on the device may vary by product line.

### Routing Tables

- Default table preference: 100
- Default table name: "default"

---

#### Joint Events
The following joint events are published (JSON on the joint bus) when network interface state changes. Other components can subscribe at runtime (joint registration / **`land@joint`**).

| Event | Description |
|-------|-------------|
| `network/on` | Sent when a local interface (LAN) comes up with IPv4 connectivity. Triggered after the interface obtains an IP address and is ready for local network communication. |
| `network/off` | Sent when a local interface (LAN) goes down or loses IPv4 connectivity. Triggered when the interface is disabled or the connection is lost. |
| `network/up` | Sent when a local interface (LAN) comes up with IPv6 connectivity. Triggered after the interface obtains an IPv6 address. |
| `network/down` | Sent when a local interface (LAN) goes down or loses IPv6 connectivity. |
| `network/onextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv4 connectivity. Triggered after successful connection establishment to the internet service provider. This event precedes `network/online` for external connections. |
| `network/offextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv4 connectivity. Triggered when the ISP connection is lost. |
| `network/upextern` | Sent when an external interface (WAN/LTE/WiFi ISP) comes up with IPv6 connectivity. |
| `network/downextern` | Sent when an external interface (WAN/LTE/WiFi ISP) goes down or loses IPv6 connectivity. |
| `network/onvpn` | Sent when a VPN interface comes up with IPv4 connectivity. Triggered after the VPN tunnel is successfully established. |
| `network/offvpn` | Sent when a VPN interface goes down or loses IPv4 connectivity. Triggered when the VPN tunnel is closed or interrupted. |
| `network/upvpn` | Sent when a VPN interface comes up with IPv6 connectivity. |
| `network/downvpn` | Sent when a VPN interface goes down or loses IPv6 connectivity. |
| `network/online` | Sent when the system establishes a default route to the internet (IPv4). Triggered after an external interface comes up and the routing table is updated. This indicates the device has full internet access. |
| `network/offline` | Sent when the system loses its default route to the internet (IPv4). Triggered when all external interfaces are down or when the primary connection fails. |
| `network/upline` | Sent when the system establishes IPv6 internet connectivity via an external interface or VPN. |
| `network/downline` | Sent when the system loses IPv6 internet connectivity. |

---

## Examples

### Basic Multi-WAN Setup

```shell
# Configure 4 external connections in hot backup mode
config network@frame hot4
set type=hot4
set 1=ifname@wan
set 2=ifname@lte
set 3=ifname@wisp
set 4=ifname@lte2
commit
```

### Custom DNS Configuration

```shell
# Configure custom DNS for multi-WAN
config network@frame hot4
set type=hot4
set custom_dns=enable
set dns=8.8.8.8
set dns2=8.8.4.4
commit
```

### Query Connection Status

```shell
# Get all extern connection status
network@frame.extern

# Get current default gateway
network@frame.default

# Check if specific interface is default
network@frame.default[ifname@lte]
```

### Manual Connection Control

```shell
# Refresh connections (trigger re-evaluation)
connect flush

# Stop connect service
connect exit
```

---

## Notes

1. The **multi-link scheduler** starts only when the **merged network profile** for the current device **network mode** includes a **`connect`** section describing multi-uplink behavior (for example `type` and numbered slots). If that section is absent, only per-interface bring-up runs and no scheduler is needed.
2. Up to **10** priority slots exist; configuration keys **`"1"` … `"10"`** refer to them in order (smaller index = higher priority).
3. Which logical interfaces appear as **local**, **external**, or **VPN** is driven by the **network profile** for the mode, plus any dynamic registration used for disabled-link handling in multi-WAN products.
4. DNS follows the **default uplink** when it changes, **unless** `custom_dns` is `enable` or a per-interface DNS override applies.
5. In **cold** backup, clearing IPv4 connection tracking on default-route changes avoids stale sessions tied to the old uplink.

---

### Lifecycle API
+ `setup[]` / `shut[]` — bring up or tear down the frame service and multi-link **`connect`** integration. **`network@hosts.setup`** is scheduled under **`init` → `land`**; **`network@frame`** itself is usually started from the **`connect`** executable / platform sequence when a profile contains a **`connect`** section.


### Published Joint Events
**IPv4 / IPv6 uplink and scheduling notifications** emitted as JSON on the joint bus, including (non-exhaustive):

| Event (examples) | When |
|------------------|------|
| `network/on`, `network/off` | Logical extern link up/down |
| `network/onextern`, `network/offextern` | External ifname scope |
| `network/onvpn`, `network/offvpn` | VPN scope |
| `network/online`, `network/offline` | Post-connect / DHCP-style online |
| `network/upline`, `network/downline`, `network/upextern`, … | IPv6 / dual-stack analogues |

Payload structure follows the `talk_t` JSON built in **`_online` / `_offline` / `_upline` / `_downline`** (typically includes **`ifname`** and status fields).

### C Code Example
```c
#include "skin/skin.h"

static void example_frame_list(void)
{
    talk_t ret = scall("network@frame", "list", NULL);
    if (ret > tpanic)
        talk_free(ret);
}
```

