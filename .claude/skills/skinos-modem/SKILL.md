---
name: skinos-modem
description: |
  Add or extend cellular USB LTE/NR module drivers under project/modem/.
  Mandatory order: check/fix config/.../kernel/option.c for ttyUSB → build
  full .zz and upgrade → tip-prove AT (if telnet/SSH) → map framework ATs →
  tip-test all ATs + dial + udhcpc online → ONLY THEN write usbdrv@ driver.
  Stop and report if AT/dial repeatedly fails, or if tip shows no SIM /
  no signal (wait for user to fit SIM/antenna) before continuing.
  Use when the user says "加模组驱动", "新加4G/5G模块", "Quectel", "Fibocom",
  "TD Tech", "MT5710", "MT5700M", "usbdrv@", "modem@lte", "AT驱动", "option.c",
  "内核支不支持模组", or asks how cellular modules plug into Skinos.
  Do NOT use for generic UART apps (skinos-uart) or Wi-Fi alone. Kernel
  option.c changes need full .zz via skinos-sdk + device-upgrade.
---

# Skinos modem — cellular module drivers

Ground truth: **`project/modem/`**. Portal: [`projects.md`](../../projects.md).  
Kernel overlays / `gBOARDID`: **skinos-sdk**. Deploy: **device-upgrade**.  
Project authoring: **skinos-project**.

## When to use

| Task | Here? |
|------|-------|
| New **USB cellular module** driver (VID/PID, AT dialect, netdev) | **Yes** |
| Kernel does not create `ttyUSB*` for this VID:PID | **Yes first** — `option.c` |
| LTE dial / APN / uplink policy | Mostly **`ifname@lte`** |
| SMS app logic | **`smsd`** / `modem@sms` — not a new USB driver |
| Serial DTU / non-modem UART | **skinos-uart** |

---

## 添加 LTE/NR 模块驱动的正常过程（必须遵守）

这是加模组驱动的**主指导方向**。未走完「能 AT → tip 验证全套 AT → AT 拨号 + udhcpc 上线」之前，**不要写** `project/modem/<drv>/` 驱动代码（可先查资料、对照模板，但不要当已验证去实现）。

```text
① option.c 能否生成 ttyUSB
      ↓ 不能 → 改 option.c → 编固件 .zz → 升级
② 有 telnet/SSH？
      ↓ 有 → tip 确认能 AT
      ↓ 不能 AT → 回到 option.c / 端口，反复修到能 AT
      ↓ 反复仍不能 → 停止，报告用户
③ 对照 modem 框架 + 同类驱动，列出需要的 AT
④ tip 测全套 AT → AT 拨号 → udhcpc 拿地址（可再 ping）
      ↓ 无 SIM / 无信号（环境问题）→ 停止，报告用户，等装好卡/天线再继续
      ↓ AT 方言/拨号命令不对 → 查资料换正确 AT，反复直到上线
      ↓ 反复仍不上线（非环境问题）→ 停止，报告用户
⑤ 上线成功 = 信息够了 → 再写模块驱动 → make obj=modem → FPK 热更验证
```

### 环境门禁：无 SIM / 无信号 → 停止并等用户（tip 阶段必判）

tip 测 AT / 拨号时，若现象是**现场环境**而不是 option/AT 方言错误，**不要继续猜 AT、不要硬写驱动、不要反复空转拨号**。应 **立刻停止当前自动化流程，把证据报告给用户**，等用户把 **SIM 卡、天线**（及必要射频）装好后再说继续。

| 典型 tip / AT 现象 | 判断 | 动作 |
|--------------------|------|------|
| `AT+CPIN?` → `+CME ERROR: 10` / `SIM not inserted` / 长期非 `READY` | **无卡或卡未就位** | 停止；报告用户装/插好 SIM |
| `AT+CIMI` / ICCID 类命令 CME 失败，且 CPIN 也不正常 | 同上 | 同上 |
| 信号查询为空/极差且 `C*REG?` 长期未注册（如 `,0`），拨号 CME（如 30）/ 上不了网 | **无信号 / 天线未接好**（在 AT 口已通、命令本身曾验证过的前提下） | 停止；报告用户检查天线与覆盖 |
| `AT` 本身不通、`ttyUSB` 缺失 | **不是**环境问题 → 走 option.c / 阶段 2 | 继续修内核/端口 |

报告里至少写清：已确认能/不能 `AT`、`CPIN?` / 信号 / 注册相关 tip 原文、因此暂停等待用户处理硬件。用户确认装好后，从 **阶段 4 tip 复测**（或至少从 CPIN + 信号 + 拨号）再继续，不要跳过上线闸门直接写驱动。

### 阶段 1 — 内核 `option.c`：能否生成 `ttyUSB`

1. 设备上确认模组：`lsusb` / `he 'arch@usb.devlist'` → 记下 **VID:PID**、产品名。
2. 读当前 **`gBOARDID`**，找到**真正生效**的 `config/.../kernel/option.c`（见下方「如何定位 option.c」）。
3. 查该文件 `option_ids[]`（或等价表）是否已集成此 VID:PID，以及板子是否具备 `option` / 对应 netdev（`cdc_ncm` / `qmi_wwan` / …）。
4. **若不能生成 `ttyUSB*`**：在**那份** `option.c` 中添加支持（署名 `/* add by <name> for … */`）→ **`./mkdel` → `make`** 出完整固件 **`.zz`** → **device-upgrade** 升级上去。  
   - `make obj=modem` **不能**替代内核/`option.c` 修改。
5. 升级后再查：`ls /dev/ttyUSB*`、`dmesg | grep -iE 'option|ttyUSB|…'`。

临时探测（**不能**代替改 `option.c`）：

```bash
echo <vid> <pid> > /sys/bus/usb-serial/drivers/option/new_id
```

### 阶段 2 — tip 确认能 AT（有 telnet/SSH 时）

用户给出了 **telnet 或 SSH** 时：

1. 进入 shell（eline → `ashy`），确认 AT 口（常见 `ttyUSB1`，以实际为准）。
2. **tip 之前先关掉 modem 服务**，避免 atd 占串口干扰 tip：
   ```bash
   he 'modem@lte.shut'    # 有 lte2 等实例时同样 he 'modem@lte2.shut'
   ```
3. 用 **`tip -A`**（不要优先用 `modem@lte.at`）发 `AT` 等冒烟命令。
4. **若不能 AT**：回头查 `option.c` / 接口绑定 / 波特率 / 是否仍被占用，改内核则再出 `.zz` 升级，**反复直到能 AT**。
5. **若反复尝试仍不能 AT**：**停止自动化猜测**，把已做步骤、`lsusb`/`ttyUSB`/`dmesg`/tip 现象 **报告给用户**，等用户决策。

用 tip 路径：

```bash
# FPK 热更后优先用 overlay（squashfs 的 /usr/bin/tip 可能偏旧）
TIP=/mnt/internal/skinos/modem/bin/tip
# 若尚未 FPK、仅固件内置 tip：TIP=tip 或 /usr/bin/tip

he 'modem@lte.shut'                 # 关闭原 modem 驱动/atd，防止抢 tip 串口
$TIP -s 115200 -A 'AT' /dev/ttyUSB1 # exit 0=OK, 1=ERROR/+CME, 2=timeout
```

用户**没有**给 telnet/SSH：说明你打算用的 AT，请其提供 shell 或实验室结果；**不要假装方言已验证**。

### 阶段 3 — 列出需求 AT（框架 + 同类驱动）

在能 AT 之后、写驱动之前：

1. 对照 Skinos **`modem@atd` 回调**（下表）和相近模板驱动（`mt5710` / `rm500u` / `fm650` / …）列出候选 AT。
2. 记下：哪个 `ttyUSB*` 是 AT；哪个 netdev 是数据口（`cdc_ncm`→常 `eth*`，`qmi_wwan`→`wwan*`，…）。

| Callback | 用途 | AT 族示例（按芯片选） |
|----------|------|------------------------|
| `modem_setup` | 初始化 / 注册 URC | `ATE1`, `AT+C*REG=2`, `AT+COPS=3,2` |
| `modem_sim` / `pin` | SIM | `AT+CPIN?` |
| `modem_imei` / `imsi` / `iccid` | 身份 | `AT+CGSN`, `AT+CIMI`, `AT+CCID` / `AT^ICCID?` |
| `modem_watch` | 信号 / 注册 / 运营商 | `AT+CSQ` / `AT^HCSQ?` / `AT+CESQ`, `AT+COPS?`, `AT+C*REG?` |
| `modem_profile` | PDP | `AT+CGDCONT?` / set |
| `modem_attach` | **拨号** | `AT^NDISDUP=…` / `AT+QNETDEVCTL=…` / `AT+QIACT=…` |
| `modem_connected` | **只查状态** | `AT^NDISSTATQRY?` / `AT+QNETDEVCTL?` |
| `modem_detach` | 挂断 | 对应去激活 |

### 阶段 4 — tip 测全套 AT → 拨号 → udhcpc 上线

1. **先** `he 'modem@lte.shut'`（及需要的 `lte2` 等），再开 tip；否则 atd 会抢 AT 口。
2. 用 **tip** 逐条测阶段 3 的候选 AT，记录真实 `OK` / `ERROR` / `+CME` 回包；**不要把未验证指令写进驱动**。
3. **先判环境（见上方「环境门禁」）**：无 SIM / 无信号导致无法上线 → **停止并报告用户**，等装好卡/天线后再继续；**不要**当成「换一条拨号 AT」去穷举。
4. 环境正常后，用已验证的拨号 AT 拨号，确认数据口载波/链路起来。
5. **`udhcpc` 取地址**（必要时先保证网卡不在错误 bridge 里），再建议 `ping`：

```bash
ip link set eth1 up          # 或实际 netdev
udhcpc -i eth1 -n -q -t 5 -T 3 -s /usr/share/skinos/network/udhcpc.sh
ip addr show eth1
ping -c 3 -I eth1 223.5.5.5
```

6. **若拨号或 DHCP 失败，且已排除无卡/无信号**：查厂商资料 / 同类驱动，换正确 AT，**tip 再测**，反复直到 **AT 拨号 + udhcpc 拿到地址（上线成功）**。
7. **若反复仍不能上线**（非环境问题）：**停止**，把已试 AT、回包、网卡/`udhcpc` 日志 **报告给用户**。

**闸门：** 只有 tip 路径 **拨号 + udhcpc 上线成功** 后，才进入写驱动。

### 阶段 5 — 写模块驱动（信息够了才写）

上线成功后，你已有：VID:PID、`option` 绑定、AT 口、数据 netdev、全套可用 AT、拨号/挂断/状态查询语义。

1. 复制相近模板 → 实现 `_usb_match` / callbacks，AT **必须与 tip 验证一致**。
2. 非标准回包用**专用 parse**（如 `hcsq_parse`）；**禁止**滥用无关的 `PARSE_*`。
3. `make obj=modem` → FPK 热更（**device-upgrade**；无需重启）→ 测 `modem@lte` / `ifname@lte`。
4. 若自动拨号仍挂，先回 **阶段 4** 用 tip 复测，不要在 C 里瞎改。

---

## 如何定位 `option.c`

**不同 `gBOARDID` → 不同 `config/…/kernel/`。** `src2kernel` 取**最深存在**的 `option.c`：

```text
${gSCOPE_DIR}/kernel/     e.g. …/mt7981/r607/wrt/kernel/
${gCUSTOM_DIR}/kernel/    e.g. …/mt7981/r607/kernel/
${gHARDWARE_DIR}/kernel/  e.g. …/mt7981/kernel/
${gPLATFORM_DIR}/kernel/  e.g. config/swrt5/kernel/   ← 常见 option.c
```

| Piece | Role |
|-------|------|
| 生效的 `…/kernel/option.c` | Linux `option.c` 产品副本 |
| `config/<platform>/kernel/custom.sh` | `src2kernel option.c …/usb/serial` |

**不要**长期改 `swrt5/build_dir/.../linux-*/drivers/usb/serial/option.c`。细则见 **skinos-sdk**。

示例条目：

```c
/* add by Auto for TD Tech MT5710-CN / MT5700M-CN */
{ USB_DEVICE_INTERFACE_CLASS(0x3466, 0x3301, 0xff) },
```

内核相关 kconfig（`CONFIG_USB_SERIAL_OPTION`、`CONFIG_USB_NET_CDC_NCM` 等）缺失时改板级 **`kernel.config`**。

---

## Architecture（写驱动时）

```text
arch@usb  --usb_match-->  usbdrv@<drv>  (= modem@<drv> via prj.json obj)
                              |
                              +--> modem_alloc → modem@lte / lte2 …
                              +--> com_register(…, modem@atd)
                              v
                         ifname@lte  ← network@frame
```

| Piece | Role |
|-------|------|
| **`arch@usb`** | USB 枚举；调各 `usbdrv@*` `.usb_match` |
| **Driver `com`** | VID/PID、TTY/netdev、注册 `modem@lte*` |
| **`modem@atd`** | 通用 FSM；`com_symbol` 加载驱动回调 |
| **`skinmodem`** | `ATD_*`、parsers、`modem_alloc` |
| **`tip`** | 串口 AT 调试（阶段 2–4 **主工具**） |
| **`ifname@lte`** | 上行拨号面 |

Docs: [`doc/com/modem/lte.md`](../../doc/com/modem/lte.md), [`sms.md`](../../doc/com/modem/sms.md), [`doc/com/ifname/lte.md`](../../doc/com/ifname/lte.md), [`doc/com/arch/usb.md`](../../doc/com/arch/usb.md).

## Existing drivers

| `com` | Hardware | `obj` |
|-------|----------|-------|
| `ec2x` / `ec200x` | Quectel 4G | `usbdrv@ec2x` / `@ec200x` |
| `rm500u` / `rm520n` | Quectel 5G | `usbdrv@rm500u` / `@rm520n` |
| `fm610` / `fm650` / `fm160` | Fibocom | `usbdrv@fm610` … |
| `mt5710` | TD Tech MT5710 / MT5700M (`3466:3301`, NCM + `AT^NDISDUP`) | `usbdrv@mt5710` |
| `atd` / `smsd` | Shared | (not usbdrv) |

## Userspace driver checklist（仅阶段 5）

### 模板

| Scenario | Copy |
|----------|------|
| Quectel 5G (`QNETDEVCTL`) | **`rm500u/`** |
| Quectel 4G + `quectel-CM` | **`ec2x/`** |
| QIACT vs QNETDEVCTL | **`ec200x/`** |
| Fibocom | **`fm650/`** / **`fm160/`** |
| TD Tech / Huawei NCM + `NDISDUP` | **`mt5710/`** |

### 必做表面

- `_usb_match` / `_usb_disappear` / `_usb_shutdown`
- atd 回调：`modem_init`, `modem_cfun`, `modem_setup`, `modem_watch`, `modem_profile`；常用 `sim`/`pin`/`imei`/`imsi`/`iccid`、`attach`/`detach`/`connected`、`urc`
- **`modem_attach` = 只拨号**；**`modem_connected` = 只查状态**（勿在 connected 里狂拨）
- `prj.json`：`"com"` + `"obj": { "usbdrv@mymod": "mymod" }`
- 板级 `usb.cfg` match → `modem@lte` / `lte2`

### 部署

| 改动 | 产物 |
|------|------|
| `config/…/kernel/option.c` 等 | **`.zz`** + 重启升级 |
| 仅 `project/modem/` | **`make obj=modem` → FPK**，不重启，立刻调 |

```text
he 'arch@usb.devlist'
he 'modem@lte.status'
he 'modem@lte.netdev'
he 'ifname@lte.status'
```

HE/config 面变更时用 **skinos-component-doc** 更新 `doc/com/modem/*.md`。

## Gotchas

- **顺序不可跳**：option/ttyUSB → tip 能 AT → tip 全套 AT + 拨号 + udhcpc 上线 → 再写驱动。
- **反复失败要停并报告用户**（AT 不通，或拨号/DHCP 反复失败），不要无限猜。
- **无 SIM / 无信号**：tip 测出后 **停止并报告用户**，等装好 SIM/天线再继续；勿当 AT 方言问题穷举。
- 有 shell 时用 **tip** 证明 AT；不要未验证就写进 C。
- **tip 前先** `he 'modem@lte.shut'`（多实例则一并 shut），避免原 modem/atd 抢串口。
- 专用回包 → 专用 parse；勿滥用 `PARSE_CSQ` 等。
- 驱动放 `project/modem/`，不要放 `land` / `uart`。
- `obj` 值 = **com 目录名**；AT/数据口 index 因模组而异。
- 同 VID:PID 多 SKU（如 MT5710 与 MT5700M）可共用 `option.c` + 一驱动（接口一致时）。
- `new_id` 只是临时；长期支持在 **`config/.../kernel/option.c`**。
- 内核覆盖修改必须带 **`/* add by <name> for … */`**。
- netdev 名用 `modem_netlist` 原样（如 `eth1`）；无 tip 时优先 **`/mnt/internal/skinos/modem/bin/tip`**（FPK 后）。
