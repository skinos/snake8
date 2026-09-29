# D228 用例覆盖表

> 主文档：[`industrial-router.md`](./industrial-router.md) · 样例：[`sample-NET-LTE-03.md`](./sample-NET-LTE-03.md)

| # | ID | 功能名 | 侧栏路径 |
|---|----|--------|----------|
| 1 | LOGIN-01 | 打开登录页 | 登录页 |
| 2 | LOGIN-02 | 正确账号密码登录 |  |
| 3 | LOGIN-03 | 错误密码拒绝 |  |
| 4 | LOGIN-04 | 登出回到登录页 |  |
| 5 | DASH-01 | 进入仪表盘 | 概要 |
| 6 | DASH-02 | LTE/NR 卡片状态可读 | 概要 |
| 7 | DASH-03 | LTE 卡片 play 连接 | 概要 |
| 8 | DASH-04 | LTE 卡片 pause 断开 | 概要 |
| 9 | DASH-05 | WISP 卡片状态可读 | 概要 |
| 10 | DASH-06 | WISP 卡片 play/pause | 概要 |
| 11 | DASH-07 | LAN 卡片地址可读 | 概要 |
| 12 | DASH-08 | 以太网口链路图标可读 | 概要 |
| 13 | DASH-09 | 本地无线表可读 | 概要 |
| 14 | DASH-10 | LTE 扳手跳转 4G/5G | 概要 |
| 15 | DASH-11 | WISP 扳手跳转 | 概要 |
| 16 | DASH-12 | LAN/无线扳手跳转 | 概要 |
| 17 | UTIL-01 | 进入资源页 | 性能 |
| 18 | UTIL-02 | CPU 饼图与历史可读 | 性能 |
| 19 | UTIL-03 | 内存饼图可读 | 性能 |
| 20 | UTIL-04 | 存储卷空间可读 | 性能 |
| 21 | TRAF-01 | 进入流量页 | 流量 |
| 22 | TRAF-02 | 上行/LAN/Wi-Fi 速率图可见 | 流量 |
| 23 | TRAF-03 | 客户端表列可读 | 流量 |
| 24 | TRAF-04 | 客户端表分页/条数切换 | 流量 |
| 25 | TRAF-05 | 有客户端时与终端列表对照 | 流量 |
| 26 | NET-CONN-01 | 进入连接策略页 | 网络 → 接入设置 |
| 27 | NET-CONN-02 | 上行状态表可读 | 网络 → 接入设置 |
| 28 | NET-CONN-03 | 查看当前 Connection Scheme | 网络 → 接入设置 |
| 29 | NET-CONN-04 | 方案 Use First Online（改后恢复） | 网络 → 接入设置 |
| 30 | NET-CONN-05 | 方案 Backup(Cold)（改后恢复） | 网络 → 接入设置 |
| 31 | NET-CONN-06 | 方案 Backup(Hot)（改后恢复） | 网络 → 接入设置 |
| 32 | NET-CONN-07 | 方案 Balancing 代表值（改后恢复） | 网络 → 接入设置 |
| 33 | NET-CONN-08 | 设置 First/Second 槽位（改后恢复） | 网络 → 接入设置 |
| 34 | NET-CONN-09 | 槽位禁止重复接口校验或恢复 | 网络 → 接入设置 |
| 35 | NET-CONN-10 | Custom DNS 开关与 DNS 填写恢复 | 网络 → 接入设置 |
| 36 | NET-CONN-11 | Interval / Delay 参数查看轻改恢复 | 网络 → 接入设置 |
| 37 | NET-CONN-12 | Apply 后 Dashboard In Use 与策略一致 | 网络 → 接入设置 |
| 38 | NET-CONN-13 | 当前 mwm 槽位与预期可读对照 | 网络 → 接入设置 |
| 39 | NET-LTE-01 | 进入 LTE/NR · Basic Info | 网络 → 4G/5G网络 |
| 40 | NET-LTE-02 | 顶部状态表可读 | 网络 → 4G/5G网络 |
| 41 | NET-LTE-03 | play 拨号连接 | 网络 → 4G/5G网络 |
| 42 | NET-LTE-04 | pause 断开 | 网络 → 4G/5G网络 |
| 43 | NET-LTE-05 | APN Custom 打开并填写后恢复 | 网络 → 4G/5G网络 |
| 44 | NET-LTE-06 | APN Custom 关闭（运营商默认） | 网络 → 4G/5G网络 |
| 45 | NET-LTE-07 | IP Type / Authentication 代表值恢复 | 网络 → 4G/5G网络 |
| 46 | NET-LTE-08 | Network Status 开关 | 网络 → 4G/5G网络 |
| 47 | NET-LTE-09 | IPv4 Mode 代表切换恢复 | 网络 → 4G/5G网络 |
| 48 | NET-LTE-10 | IPv4 Masquerade(NAT) 开关恢复 | 网络 → 4G/5G网络 |
| 49 | NET-LTE-11 | Custom DNS 开关恢复 | 网络 → 4G/5G网络 |
| 50 | NET-LTE-12 | Availability Check 模式代表值恢复 | 网络 → 4G/5G网络 |
| 51 | NET-LTE-13 | Failed TODO 代表值恢复 | 网络 → 4G/5G网络 |
| 52 | NET-LTE-14 | MTU 查看轻改恢复 | 网络 → 4G/5G网络 |
| 53 | NET-LTE-27 | Dashboard 与 LTE 页状态一致 | 网络 → 4G/5G网络 |
| 54 | NET-LTE-28 | Connection 与 LTE In Use 一致 | 网络 → 4G/5G网络 |
| 55 | NET-LTE-15 | Backup SIM 页进入 | 网络 → 4G/5G网络 |
| 56 | NET-LTE-16 | SIM Function / Mode 查看轻改恢复 | 网络 → 4G/5G网络 |
| 57 | NET-LTE-17 | 备份 APN/PIN 字段可读 | 网络 → 4G/5G网络 |
| 58 | NET-LTE-18 | Custom AT 页进入 | 网络 → 4G/5G网络 |
| 59 | NET-LTE-19 | 新增一条 setup AT 后删除 | 网络 → 4G/5G网络 |
| 60 | NET-LTE-20 | 新增一条周期 AT 后删除 | 网络 → 4G/5G网络 |
| 61 | NET-LTE-21 | SMS Settings 进入 | 网络 → 4G/5G网络 |
| 62 | NET-LTE-22 | SMS Function / HE Agent 开关恢复 | 网络 → 4G/5G网络 |
| 63 | NET-LTE-23 | 短信收件箱表可读 | 网络 → 4G/5G网络 |
| 64 | NET-LTE-24 | Modem Settings 进入 | 网络 → 4G/5G网络 |
| 65 | NET-LTE-25 | Attach Mode 代表值后恢复 Auto | 网络 → 4G/5G网络 |
| 66 | NET-LTE-26 | Watch/Need SIM 等健康检查查看 | 网络 → 4G/5G网络 |
| 67 | NET-WISP24-01 | 进入 WISP 2.4G | 网络 →（无 2.4G WISP） |
| 68 | NET-WISP24-02 | 状态与模式可读 | 网络 →（无） |
| 69 | NET-WISP24-03 | 扫描上级 AP | 网络 →（无） |
| 70 | NET-WISP24-04 | 填写 SSID/密码关联 | 网络 →（无） |
| 71 | NET-WISP24-05 | play 连接 / pause 断开 | 网络 →（无） |
| 72 | NET-WISP24-06 | DHCP/PPPoE/Static 模式代表切换恢复 | 网络 →（无） |
| 73 | NET-WISP24-07 | Masquerade / Availability 查看恢复 | 网络 →（无） |
| 74 | NET-WISP58-01 | 进入 WISP 5.8G | 网络 → 无线连网(5.8G) |
| 75 | NET-WISP58-02 | 5.8G 关联代表流程 | 网络 → 无线连网(5.8G) |
| 76 | NET-LAN-01 | 进入 LAN | 网络 → 本地网络 |
| 77 | NET-LAN-02 | IPv4 地址/掩码可读 | 网络 → 本地网络 |
| 78 | NET-LAN-03 | Address2/3 开关查看 | 网络 → 本地网络 |
| 79 | NET-LAN-04 | DHCP Server 开关 | 网络 → 本地网络 |
| 80 | NET-LAN-05 | DHCP 地址池查看轻改恢复 | 网络 → 本地网络 |
| 81 | NET-LAN-06 | Lease / 下发网关 DNS 查看 | 网络 → 本地网络 |
| 82 | NET-LAN-07 | IPv6 Mode 查看 | 网络 → 本地网络 |
| 83 | NET-LAN-08 | DHCPv6 相关字段查看 | 网络 → 本地网络 |
| 84 | NET-WAN-01 | 进入 WAN | 网络 →（无 WAN） |
| 85 | NET-WAN-02 | WAN 模式查看 | 网络 →（无） |
| 86 | NET-WAN-03 | WAN DHCP/Static/PPPoE 代表恢复 | 网络 →（无） |
| 87 | NET-WAN-04 | WAN NAT/可用性查看 | 网络 →（无） |
| 88 | ROUTE-HOSTS-01 | Hosts 进入 | 路由 → Hosts映射 |
| 89 | ROUTE-HOSTS-02 | 新增主机名映射后删除 | 路由 → Hosts映射 |
| 90 | ROUTE-MAIN-01 | 路由表进入 | 路由 → 路由表 |
| 91 | ROUTE-MAIN-02 | 默认路由/接口列可读 | 路由 → 路由表 |
| 92 | ROUTE-MAIN-03 | 改 Connection 后主表默认路由变化 | 路由 → 路由表 |
| 93 | ROUTE-CUSTOM-01 | 其它路由表进入 | 路由 → 其它路由表 |
| 94 | ROUTE-CUSTOM-02 | 新建表后删除 | 路由 → 其它路由表 |
| 95 | ROUTE-CUSTOM-03 | 表内加一条路由后删除 | 路由 → 其它路由表 |
| 96 | ROUTE-RULE-01 | 路由策略进入 | 路由 → 路由策略 |
| 97 | ROUTE-RULE-02 | 新增策略后删除 | 路由 → 路由策略 |
| 98 | ROUTE-MARK-01 | 包标记进入 | 路由 → 包标记 |
| 99 | ROUTE-MARK-02 | 新增标记后删除 | 路由 → 包标记 |
| 100 | ROUTE-FW-01 | 防火墙进入 | 路由 → 防火墙 |
| 101 | ROUTE-FW-02 | 切换 LAN/LTE 等接口 Tab | 路由 → 防火墙 |
| 102 | ROUTE-FW-03 | Status / Default Action 查看 | 路由 → 防火墙 |
| 103 | ROUTE-FW-04 | ICMP/WEB/SSH/Telnet Access 开关恢复 | 路由 → 防火墙 |
| 104 | ROUTE-FW-05 | IPSEC/NAT Through 开关恢复 | 路由 → 防火墙 |
| 105 | ROUTE-FW-06 | 新增一条防火墙规则后删除 | 路由 → 防火墙 |
| 106 | ROUTE-NAT-01 | 端口映射进入 | 路由 → 端口映射 |
| 107 | ROUTE-NAT-02 | 切换上行 Tab | 路由 → 端口映射 |
| 108 | ROUTE-NAT-03 | 新增一条 DNAT 后删除 | 路由 → 端口映射 |
| 109 | ROUTE-NAT-04 | DMZ 字段查看 | 路由 → 端口映射 |
| 110 | ROUTE-PROXY-01 | 端口代理进入 | 路由 → 端口代理 |
| 111 | ROUTE-PROXY-02 | 新增一条代理后删除 | 路由 → 端口代理 |
| 112 | ROUTE-TTL-01 | TTL 进入 | 路由 → 转发次数(TTL) |
| 113 | ROUTE-TTL-02 | TTL Mode 代表值恢复 | 路由 → 转发次数(TTL) |
| 114 | ROUTE-ALG-01 | ALG 进入 | 路由 → 应用层网关 |
| 115 | ROUTE-ALG-02 | FTP ALG 开关恢复 | 路由 → 应用层网关 |
| 116 | ROUTE-ALG-03 | PPTP ALG 开关恢复 | 路由 → 应用层网关 |
| 117 | ROUTE-ALG-04 | GRE ALG 开关恢复 | 路由 → 应用层网关 |
| 118 | ROUTE-ALG-05 | 其余 ALG 逐项开关恢复 | 路由 → 应用层网关 |
| 119 | ROUTE-ALG-06 | ALG Apply 后配置可读 | 路由 → 应用层网关 |
| 120 | VPN-IPSEC-01 | IPsec客户端 列表页进入 | VPN → IPsec客户端 |
| 121 | VPN-IPSEC-02 | IPsec客户端 + 新建实例 | VPN → IPsec客户端 |
| 122 | VPN-IPSEC-03 | IPsec客户端 打开实例编辑页字段可读 | VPN → IPsec客户端 |
| 123 | VPN-IPSEC-04 | IPsec客户端 Status 启用 | VPN → IPsec客户端 |
| 124 | VPN-IPSEC-05 | IPsec客户端 play 连接 | VPN → IPsec客户端 |
| 125 | VPN-IPSEC-06 | IPsec客户端 pause 断开 | VPN → IPsec客户端 |
| 126 | VPN-IPSEC-07 | IPsec客户端 删除实例恢复 | VPN → IPsec客户端 |
| 127 | VPN-WG-01 | WireGuard 列表页进入 | VPN → WireGuard |
| 128 | VPN-WG-02 | WireGuard + 新建实例 | VPN → WireGuard |
| 129 | VPN-WG-03 | WireGuard 打开实例编辑页字段可读 | VPN → WireGuard |
| 130 | VPN-WG-04 | WireGuard Status 启用 | VPN → WireGuard |
| 131 | VPN-WG-05 | WireGuard play 连接 | VPN → WireGuard |
| 132 | VPN-WG-06 | WireGuard pause 断开 | VPN → WireGuard |
| 133 | VPN-WG-07 | WireGuard 删除实例恢复 | VPN → WireGuard |
| 134 | VPN-L2TP-01 | L2TP客户端 列表页进入 | VPN → L2TP客户端 |
| 135 | VPN-L2TP-02 | L2TP客户端 + 新建实例 | VPN → L2TP客户端 |
| 136 | VPN-L2TP-03 | L2TP客户端 打开实例编辑页字段可读 | VPN → L2TP客户端 |
| 137 | VPN-L2TP-04 | L2TP客户端 Status 启用 | VPN → L2TP客户端 |
| 138 | VPN-L2TP-05 | L2TP客户端 play 连接 | VPN → L2TP客户端 |
| 139 | VPN-L2TP-06 | L2TP客户端 pause 断开 | VPN → L2TP客户端 |
| 140 | VPN-L2TP-07 | L2TP客户端 删除实例恢复 | VPN → L2TP客户端 |
| 141 | VPN-PPTP-01 | PPTP客户端 列表页进入 | VPN → PPTP客户端 |
| 142 | VPN-PPTP-02 | PPTP客户端 + 新建实例 | VPN → PPTP客户端 |
| 143 | VPN-PPTP-03 | PPTP客户端 打开实例编辑页字段可读 | VPN → PPTP客户端 |
| 144 | VPN-PPTP-04 | PPTP客户端 Status 启用 | VPN → PPTP客户端 |
| 145 | VPN-PPTP-05 | PPTP客户端 play 连接 | VPN → PPTP客户端 |
| 146 | VPN-PPTP-06 | PPTP客户端 pause 断开 | VPN → PPTP客户端 |
| 147 | VPN-PPTP-07 | PPTP客户端 删除实例恢复 | VPN → PPTP客户端 |
| 148 | VPN-GRE-01 | GRE隧道 列表页进入 | VPN → GRE隧道 |
| 149 | VPN-GRE-02 | GRE隧道 + 新建实例 | VPN → GRE隧道 |
| 150 | VPN-GRE-03 | GRE隧道 打开实例编辑页字段可读 | VPN → GRE隧道 |
| 151 | VPN-GRE-04 | GRE隧道 Status 启用 | VPN → GRE隧道 |
| 152 | VPN-GRE-05 | GRE隧道 play 连接 | VPN → GRE隧道 |
| 153 | VPN-GRE-06 | GRE隧道 pause 断开 | VPN → GRE隧道 |
| 154 | VPN-GRE-07 | GRE隧道 删除实例恢复 | VPN → GRE隧道 |
| 155 | WIFI-N-01 | 进入 2.4G 热点/SSID Setup | 无线 →（无 2.4G热点） |
| 156 | WIFI-N-02 | Status 开启 AP | 无线 →（无 2.4G热点） |
| 157 | WIFI-N-03 | 设置 SSID | 无线 →（无 2.4G热点） |
| 158 | WIFI-N-04 | Security WPA2-PSK + AES + 密码 | 无线 →（无 2.4G热点） |
| 159 | WIFI-N-05 | Security Mixed / None 代表切换恢复 | 无线 →（无 2.4G热点） |
| 160 | WIFI-N-06 | Hide SSID 开关恢复 | 无线 →（无 2.4G热点） |
| 161 | WIFI-N-07 | Isolate Clients 开关恢复 | 无线 →（无 2.4G热点） |
| 162 | WIFI-N-08 | 终端关联 2.4G | 无线 →（无 2.4G热点） |
| 163 | WIFI-N-09 | 关闭 AP 或恢复原 SSID | 无线 →（无 2.4G热点） |
| 164 | WIFI-N-10 | Advanced 进入 | 无线 →（无 2.4G热点） |
| 165 | WIFI-N-11 | Mode / Bandwidth 查看 | 无线 →（无 2.4G热点） |
| 166 | WIFI-N-12 | Channel / Country 轻改恢复 | 无线 →（无 2.4G热点） |
| 167 | WIFI-N-13 | Tx Power 查看 | 无线 →（无 2.4G热点） |
| 168 | WIFI-N-14 | Beacon/DTIM/LDPC 等查看 | 无线 →（无 2.4G热点） |
| 169 | WIFI-N-15 | ACL Setup 进入 | 无线 →（无 2.4G热点） |
| 170 | WIFI-N-16 | Clients ACL 开关 | 无线 →（无 2.4G热点） |
| 171 | WIFI-N-17 | 增删一条 MAC ACL | 无线 →（无 2.4G热点） |
| 172 | WIFI-N-18 | Clients Table 进入 | 无线 →（无 2.4G热点） |
| 173 | WIFI-N-19 | 关联客户端表可读 | 无线 →（无 2.4G热点） |
| 174 | WIFI-N-20 | Knock 踢下线 | 无线 →（无 2.4G热点） |
| 175 | WIFI-N-21 | 2.4G 多SSID 进入并增删一条恢复 | 无线 →（无 2.4G热点） |
| 176 | WIFI-N-STA-01 | 2.4G 网卡(Station)进入 | 无线 →（无） |
| 177 | WIFI-N-STA-02 | Station 扫描关联 | 无线 →（无） |
| 178 | WIFI-A-01 | 进入 5.8G 热点 | 无线 → 5.8G热点 |
| 179 | WIFI-A-02 | Status 开启 AP | 无线 → 5.8G热点 |
| 180 | WIFI-A-03 | 设置 SSID | 无线 → 5.8G热点 |
| 181 | WIFI-A-04 | Security WPA2-PSK + AES + 密码 | 无线 → 5.8G热点 |
| 182 | WIFI-A-05 | Security Mixed / None 代表切换恢复 | 无线 → 5.8G热点 |
| 183 | WIFI-A-06 | Hide SSID 开关恢复 | 无线 → 5.8G热点 |
| 184 | WIFI-A-07 | Isolate Clients 开关恢复 | 无线 → 5.8G热点 |
| 185 | WIFI-A-08 | 终端关联 5.8G | 无线 → 5.8G热点 |
| 186 | WIFI-A-09 | 关闭 AP 或恢复原 SSID | 无线 → 5.8G热点 |
| 187 | WIFI-A-10 | Advanced 进入 | 无线 → 5.8G热点 |
| 188 | WIFI-A-11 | Mode / Bandwidth 查看 | 无线 → 5.8G热点 |
| 189 | WIFI-A-12 | Channel / Country 轻改恢复 | 无线 → 5.8G热点 |
| 190 | WIFI-A-13 | Tx Power 查看 | 无线 → 5.8G热点 |
| 191 | WIFI-A-14 | Beacon/DTIM/LDPC 等查看 | 无线 → 5.8G热点 |
| 192 | WIFI-A-15 | ACL Setup 进入 | 无线 → 5.8G热点 |
| 193 | WIFI-A-16 | Clients ACL 开关 | 无线 → 5.8G热点 |
| 194 | WIFI-A-17 | 增删一条 MAC ACL | 无线 → 5.8G热点 |
| 195 | WIFI-A-18 | Clients / 关联表可读 | 无线 → 5.8G热点 |
| 196 | WIFI-A-19 | 多SSID（若页内有） | 无线 → 5.8G热点 |
| 197 | WIFI-A-STA-01 | 5.8G 网卡进入 | 无线 / 网络 → 无线连网(5.8G) |
| 198 | WIFI-A-STA-02 | 5.8G 网卡关联 | 无线 / 网络 → 无线连网(5.8G) |
| 199 | STA-LIST-01 | 终端列表进入 | 终端 → 终端列表 |
| 200 | STA-LIST-02 | 列字段可读 | 终端 → 终端列表 |
| 201 | STA-LIST-03 | 编辑终端名称恢复 | 终端 → 终端列表 |
| 202 | STA-LIST-04 | Forget/忽略终端 | 终端 → 终端列表 |
| 203 | STA-ACL-01 | 访问控制进入 | 终端 → 访问控制 |
| 204 | STA-ACL-02 | Status 开关恢复 | 终端 → 访问控制 |
| 205 | STA-ACL-03 | 切换接口 Tab | 终端 → 访问控制 |
| 206 | STA-ACL-04 | 新增一条 ACL 后删除 | 终端 → 访问控制 |
| 207 | APP-IO-01 | IO 管理进入 | 应用 → IO管理 |
| 208 | APP-IO-02 | 切换 g1/g2/g3 Tab | 应用 → IO管理 |
| 209 | APP-IO-03 | 查看初始电平映射 | 应用 → IO管理 |
| 210 | APP-IO-04 | 改 g1 输出后恢复 | 应用 → IO管理 |
| 211 | APP-IO-05 | IO Proxy Service 开关查看 | 应用 → IO管理 |
| 212 | APP-IO-06 | MQTT/Client 参数表单可读 | 应用 → IO管理 |
| 213 | APP-IO-07 | 远程平台 IO | 应用 → IO管理 |
| 214 | APP-CAM-01 | 摄像头 OSD 进入 | 应用 → 摄像头OSD |
| 215 | APP-CAM-02 | 开关 Camera OSD | 应用 → 摄像头OSD |
| 216 | APP-CAM-03 | 填写 IP/账号表单 | 应用 → 摄像头OSD |
| 217 | APP-CAM-04 | 增删一条 Mapping 规则 | 应用 → 摄像头OSD |
| 218 | APP-DDNS-01 | DDNS 进入 | 应用 → 动态域名 |
| 219 | APP-DDNS-02 | Client1 开关 | 应用 → 动态域名 |
| 220 | APP-DDNS-03 | 选服务商填域名账号 | 应用 → 动态域名 |
| 221 | APP-DDNS-04 | State 字段可读 | 应用 → 动态域名 |
| 222 | APP-DDNS-05 | Client2/3 槽位进入查看 | 应用 → 动态域名 |
| 223 | APP-GNSS-01 | 定位进入 | 应用 →（无定位） |
| 224 | APP-GNSS-02 | Status 启用 | 应用 →（无） |
| 225 | APP-GNSS-03 | Fix/坐标字段可读 | 应用 →（无） |
| 226 | APP-GNSS-04 | 配置 NMEA 转发 | 应用 →（无） |
| 227 | APP-GNSS-05 | 句子过滤 | 应用 →（无） |
| 228 | APP-GNSS-06 | 定位#2/#3 | 应用 →（无） |
| 229 | APP-UART1-01 | 串口#1 进入 | 应用 → 串口#1 |
| 230 | APP-UART1-02 | Status 打开端口 | 应用 → 串口#1 |
| 231 | APP-UART1-03 | Mode=Command Line 并恢复 | 应用 → 串口#1 |
| 232 | APP-UART1-04 | Mode=Transparent 并恢复 | 应用 → 串口#1 |
| 233 | APP-UART1-05 | Mode=RTK 查看 | 应用 → 串口#1 |
| 234 | APP-UART1-06 | 线参数修改恢复 | 应用 → 串口#1 |
| 235 | APP-UART1-07 | DTU TCP 目标填写恢复 | 应用 → 串口#1 |
| 236 | APP-UART1-08 | MQTT/TLS 块查看 | 应用 → 串口#1 |
| 237 | APP-UART2-01 | 串口#2 进入 | 应用 → 串口#2 |
| 238 | APP-UART2-02 | Status 打开 | 应用 → 串口#2 |
| 239 | APP-UART2-03 | Command Line 模式恢复 | 应用 → 串口#2 |
| 240 | APP-UART2-04 | Transparent 模式恢复 | 应用 → 串口#2 |
| 241 | APP-UART2-05 | 线参数修改恢复 | 应用 → 串口#2 |
| 242 | APP-UART2-06 | DTU 目标填写恢复 | 应用 → 串口#2 |
| 243 | SYS-DEV-01 | 设备管理进入 | 系统 → 设备管理 |
| 244 | SYS-DEV-02 | 设备名修改恢复 | 系统 → 设备管理 |
| 245 | SYS-DEV-03 | MAC/运行时间只读 | 系统 → 设备管理 |
| 246 | SYS-DEV-04 | PC Time Sync | 系统 → 设备管理 |
| 247 | SYS-DEV-05 | 时区设置恢复 | 系统 → 设备管理 |
| 248 | SYS-DEV-06 | NTP 开关与服务器恢复 | 系统 → 设备管理 |
| 249 | SYS-DEV-07 | Sync Now | 系统 → 设备管理 |
| 250 | SYS-DEV-08 | NTP Service（LAN 授时）查看 | 系统 → 设备管理 |
| 251 | SYS-DEV-09 | Operation Mode 只读确认 mwm | 系统 → 设备管理 |
| 252 | SYS-DEV-10 | Sys Reboot 仅写步骤不默认执行 | 系统 → 设备管理 |
| 253 | SYS-CFG-01 | 配置管理进入 | 系统 → 设备管理 → 配置 |
| 254 | SYS-CFG-02 | Backup Configure 导出 | 系统 → 设备管理 → 配置 |
| 255 | SYS-CFG-03 | Configure Version 可读 | 系统 → 设备管理 → 配置 |
| 256 | SYS-CFG-04 | Restore 入口存在 | 系统 → 设备管理 → 配置 |
| 257 | SYS-CFG-05 | Default Configure 仅写不执行 | 系统 → 设备管理 → 配置 |
| 258 | SYS-SW-01 | 软件管理进入 | 系统 → 设备管理 → 软件 |
| 259 | SYS-SW-02 | 固件版本/SDK/标识可读 | 系统 → 设备管理 → 软件 |
| 260 | SYS-SW-03 | 工程 FPK 列表可读 | 系统 → 设备管理 → 软件 |
| 261 | SYS-SW-04 | 升级上传入口存在 | 系统 → 设备管理 → 软件 |
| 262 | SYS-SW-05 | FPK 仓库入口存在 | 系统 → 设备管理 → 软件 |
| 263 | SYS-PWD-01 | 密码管理进入 | 系统 → 设备管理 → 密码 |
| 264 | SYS-PWD-02 | 原密码错误校验 | 系统 → 设备管理 → 密码 |
| 265 | SYS-PWD-03 | 新密码不一致校验 | 系统 → 设备管理 → 密码 |
| 266 | SYS-PWD-04 | 改密成功后再改回 | 系统 → 设备管理 → 密码 |
| 267 | SYS-AGENT-01 | 远程控制进入 | 系统 → 远程控制 |
| 268 | SYS-AGENT-02 | Local Control 广播/端口查看 | 系统 → 远程控制 |
| 269 | SYS-AGENT-03 | Command Mode 代表值恢复 | 系统 → 远程控制 |
| 270 | SYS-AGENT-04 | Agent Control json 端口查看 | 系统 → 远程控制 |
| 271 | SYS-AGENT-05 | Agent 填 center 账号 | 系统 → 远程控制 |
| 272 | SYS-AGENT-06 | MQTT Control 表单查看 | 系统 → 远程控制 |
| 273 | SYS-AGENT-07 | MQTT 开关轻改恢复 | 系统 → 远程控制 |
| 274 | SYS-REBOOT-01 | 自动重启进入 | 系统 → 自动重启 |
| 275 | SYS-REBOOT-02 | Reboot Mode 各代表值查看恢复 | 系统 → 自动重启 |
| 276 | SYS-REBOOT-03 | Point 时间设置恢复 | 系统 → 自动重启 |
| 277 | SYS-WEB-01 | WEB 服务器进入 | 系统 → WEB服务器 |
| 278 | SYS-WEB-02 | HTTP 开关/端口查看 | 系统 → WEB服务器 |
| 279 | SYS-WEB-03 | HTTPS 开关/端口查看 | 系统 → WEB服务器 |
| 280 | SYS-WEB-04 | 访问地址 ACL 查看 | 系统 → WEB服务器 |
| 281 | SYS-TELNET-01 | Telnet 进入 | 系统 → Telnet服务器 |
| 282 | SYS-TELNET-02 | 开关/端口/ACL 查看恢复 | 系统 → Telnet服务器 |
| 283 | SYS-SSH-01 | SSH 进入 | 系统 → SSH服务器 |
| 284 | SYS-SSH-02 | 开关/端口/ACL 查看恢复 | 系统 → SSH服务器 |
| 285 | DBG-LOG-01 | Syslog 进入 | 调试 → 系统日志 |
| 286 | DBG-LOG-02 | Location/Level 设置恢复 | 调试 → 系统日志 |
| 287 | DBG-LOG-03 | 远程日志开关查看 | 调试 → 系统日志 |
| 288 | DBG-LOG-04 | 日志文件下载入口 | 调试 → 系统日志 |
| 289 | DBG-LOG-05 | Debug/Verb Mask 查看 | 调试 → 系统日志 |
| 290 | DBG-TERM-01 | Terminal 进入 | 调试 → 终端命令行 |
| 291 | DBG-TERM-02 | 打开 HE 终端可输入 | 调试 → 终端命令行 |
| 292 | DBG-INIT-01 | Inittab 进入只读 | 调试 → 开机启动 |
| 293 | DBG-INIT-02 | Uninittab 进入只读 | 调试 → 关机执行 |
| 294 | DBG-JOINT-01 | Jointtab 进入只读 | 调试 → 事件处理 |
| 295 | DBG-DAEMON-01 | Daemon 进入只读 | 调试 → 后台服务 |
| 296 | DBG-SCRIPT-01 | 脚本编程进入 | 调试 → 脚本编程 |
| 297 | DBG-SCRIPT-02 | 脚本列表/编辑入口存在 | 调试 → 脚本编程 |
| 298 | DBG-SCRIPT-03 | 运行示例脚本 | 调试 → 脚本编程 |
| 299 | DEV-NET-01 | 网络框架进入 | 定制开发 → 网络框架 |
| 300 | DEV-NET-02 | 当前模式与 status 一致 | 定制开发 → 网络框架 |
| 301 | DEV-NET-03 | 模式列表可读 | 定制开发 → 网络框架 |
| 302 | DEV-NET-04 | 不切换破坏性模式 | 定制开发 → 网络框架 |
| 303 | DEV-NET-05 | offload 等只读字段 | 定制开发 → 网络框架 |
| 304 | DEV-SDK-01 | SDK 下载 | 定制开发 → SDK下载 |

合计：**304** 条
