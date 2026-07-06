#!/usr/bin/env python3
"""Generate 720F spec_pro_cn.docx without external dependencies."""

import html
import zipfile
from pathlib import Path
from xml.sax.saxutils import escape

OUT = Path(__file__).with_name('spec_pro_cn.docx')

WNS = 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'


def p(text='', style=None, center=False, bold=False):
    text = escape(text)
    rpr = '<w:rPr><w:b/></w:rPr>' if bold else ''
    text = f'<w:r>{rpr}<w:t xml:space="preserve">{text}</w:t></w:r>'
    ppr = ''
    if style:
        ppr += f'<w:pStyle w:val="{style}"/>'
    if center:
        ppr += '<w:jc w:val="center"/>'
    if ppr:
        ppr = f'<w:pPr>{ppr}</w:pPr>'
    return f'<w:p>{ppr}{text}</w:p>'


def heading(text, level):
    return p(text, style=f'Heading{level}')


def bullet(text):
    text = escape(text)
    return (
        f'<w:p><w:pPr><w:pStyle w:val="ListBullet"/>'
        f'<w:numPr><w:ilvl w:val="0"/><w:numId w:val="1"/></w:numPr></w:pPr>'
        f'<w:r><w:t xml:space="preserve">{text}</w:t></w:r></w:p>'
    )


def table(headers, rows):
    cols = len(headers)
    grid = ''.join(f'<w:gridCol w:w="4500"/>' for _ in range(cols))
    def row(cells, header=False):
        tcs = []
        for c in cells:
            c = escape(c)
            tcpr = '<w:tcPr><w:shd w:val="clear" w:color="auto" w:fill="D9E2F3"/></w:tcPr>' if header else ''
            tcs.append(f'<w:tc>{tcpr}<w:p><w:r><w:t xml:space="preserve">{c}</w:t></w:r></w:p></w:tc>')
        return '<w:tr>' + ''.join(tcs) + '</w:tr>'
    body = row(headers, True) + ''.join(row(r) for r in rows)
    return f'<w:tbl><w:tblPr><w:tblW w:w="0" w:type="auto"/><w:tblBorders><w:top w:val="single" w:sz="4"/><w:left w:val="single" w:sz="4"/><w:bottom w:val="single" w:sz="4"/><w:right w:val="single" w:sz="4"/><w:insideH w:val="single" w:sz="4"/><w:insideV w:val="single" w:sz="4"/></w:tblBorders></w:tblPr><w:tblGrid>{grid}</w:tblGrid>{body}</w:tbl>'


def placeholder(caption):
    return p(f'[图片占位：{caption}]', center=True)


blocks = []
blocks.append(p('720F 产品规格书', center=True, bold=True))
blocks.append(p('Rev. 1.0 | 客户评审版', center=True))
blocks.append(p('720F 是一款面向工业现场、车载移动与复杂网络环境的多链路工业路由器。整机基于 F720 V1.0 硬件平台，集成 RK3568 主控、双 4G LTE 模块、5G NR 模块、双频 Wi-Fi、千兆以太网交换、6 路 RS232 串口及远程管理能力，适用于多蜂窝冗余、视频回传、边缘接入和远程运维等场景。蜂窝模块型号、频段、认证及软件功能范围可根据目标区域、运营商和项目配置确认。'))
blocks.append(p('表 0-1. 文档信息', bold=True))
blocks.append(table(['文档字段', '内容'], [
    ['产品', '720F 工业多链路路由器'],
    ['文档版本', 'Rev. 1.0'],
    ['文档状态', '客户评审版'],
    ['保密级别', '客户保密资料'],
    ['适用范围', '工程评审、样品评估、项目沟通与部署规划'],
    ['文档日期', '2026-06-27'],
]))

blocks.append(heading('1. 产品概述', 1))
blocks.append(p('720F 采用整机盒式形态，基于 Ashyelf V720（RK3568）硬件平台，预装 Talonbox SkinOS 嵌入式系统。设备面向需要将多路蜂窝、有线以太网、Wi-Fi、串口和远程管理能力集成到同一节点的工业与车载项目。整机可直接作为多 WAN 工业网关部署，也可结合客户外壳、天线和供电方案进行项目级集成。'))
blocks.append(heading('1.1 主要能力', 2))
for item in [
    '双 Quectel EG25 4G LTE 模块（板载贴装）+ Quectel RM520N-GL 5G NR 模块（M.2 接口），共 3 路蜂窝接入能力，支持多 SIM 管理。',
    '1×WAN + 3×LAN 千兆以太网交换，通过 M12 8 芯工业连接器引出；端口角色随工作模式变化。',
    'Broadcom AP6398 双频 Wi-Fi（2.4 GHz / 5 GHz），支持 AP、客户端/中继等使用场景。',
    '6 路 RS232 串口，通过 25Pin J30J 连接器引出，支持并行工作与协议转换。',
    '1 路可配置 GPIO，可用于输入/输出或项目级扩展。',
    '支持 PPTP、L2TP、IPsec、GRE、OpenVPN、WireGuard 等常见 VPN 功能。',
    '支持内网穿透、远程组网等专网能力。',
    '支持远程平台管理，包括设备状态、远程访问、Web UI、终端访问、OTA 及项目集成。',
    '基于 Talonbox SkinOS 嵌入式系统平台，支持组件化开发、软件包部署和项目级定制。',
]:
    blocks.append(bullet(item))

blocks.append(heading('1.2 目标应用', 2))
blocks.append(p('表 1-1. 720F 典型应用场景', bold=True))
blocks.append(table(['应用场景', '典型用途'], [
    ['工业多链路网关', '通过双 4G、5G、有线 WAN 和 Wi-Fi 实现链路冗余、备份与负载均衡'],
    ['视频/车载传输', '为摄像头、车载终端提供多蜂窝并发上行与链路连续性保障'],
    ['远程监测与运维', '向远程平台上报设备状态、串口数据、定位信息，并支持远程诊断'],
    ['边缘接入节点', '连接 PLC、传感器、控制器等现场设备并汇聚至云平台'],
    ['定制协议网关', '通过 SDK 和组件开发实现客户专属串口或远程管理协议'],
]))

blocks.append(heading('1.3 系统平台', 2))
blocks.append(p('720F 使用 Talonbox SkinOS 嵌入式系统平台。SkinOS 是 Talonbox 基于嵌入式 Linux 构建的网关系统，面向模块化开发、远程管理、协议转换、软件包部署和现场项目定制。项目用户可根据项目范围申请开发文档、SDK 支持、管理工具、OTA 工具或定制组件开发。'))

blocks.append(heading('2. 外观与结构信息', 1))
blocks.append(heading('2.1 整机视图', 2))
blocks.append(p('下图展示 720F 整机外观形态，用于产品识别、结构评估和安装空间规划。'))
for cap in ['图2-1. 720F 整机正面视图', '图2-2. 720F 整机背面/接口视图', '图2-3. 720F 整机侧面视图']:
    blocks.append(placeholder(cap))

blocks.append(heading('2.2 尺寸与安装', 2))
blocks.append(p('表 2-1. 720F 结构尺寸与安装信息', bold=True))
blocks.append(table(['项目', '规格/说明'], [
    ['整机长度 × 宽度', '约 180 mm × 140 mm'],
    ['整机高度', '以机械图纸为准'],
    ['安装方式', '桌面放置、导轨安装或项目定制安装结构（以最终结构件为准）'],
    ['机械尺寸基准', '以 F720 V1.0 机械图纸及 DXF/STEP 文件为准'],
]))
blocks.append(placeholder('图2-4. 720F 机械尺寸图（待补充）'))

blocks.append(heading('2.3 操作与 ESD 注意事项', 2))
for item in [
    '720F 以整机形式供货。操作时应避免直接接触内部 PCBA 裸露元器件和连接器针脚。',
    '装配、检查和现场维护时，应使用防静电工作台、接地腕带和合适的防静电包装。',
    '除非项目设计明确支持，否则设备上电时不得插拔 SIM 卡。',
    '部署前应评审天线布局、接地、散热、防护等级和安装方式。',
]:
    blocks.append(bullet(item))

blocks.append(heading('3. 硬件规格', 1))
blocks.append(p('720F 的主要硬件规格如下。具体配置可能因模块批次、区域认证、硬件版本和项目需求调整。'))
blocks.append(p('表 3-1. 720F 硬件规格总表', bold=True))
blocks.append(table(['参数', '规格/说明'], [
    ['主控平台', 'Rockchip RK3568，ARM Cortex-A55 四核，Ashyelf V720 硬件平台'],
    ['系统内存/存储', '以最终 BOM 为准；板载 eMMC，支持 Micro SD 扩展存储'],
    ['4G 蜂窝模块', '2 × Quectel EG25（板载贴装）'],
    ['5G 蜂窝模块', '1 × Quectel RM520N-GL（M.2 接口）'],
    ['蜂窝接入', '支持 4G LTE 与 5G NR 多路并发接入；具体制式、频段和运营商适配能力取决于模块型号、固件和项目配置'],
    ['SIM', '3 × SIM 卡槽，分别对应两路 EG25 与一路 RM520N'],
    ['以太网', 'RTL8365MB 千兆交换芯片；内部 1×WAN + 3×LAN；通过 M12 8 芯工业连接器引出'],
    ['Wi-Fi', 'Broadcom AP6398（SDIO），双频 802.11 b/g/n/ac；2T2R 天线设计'],
    ['串口', '6 路 RS232，通过 25Pin J30J 连接器引出'],
    ['GPIO', '1 路 GPIO（丝印 GPIO1），可配置输入/输出'],
    ['定位', '支持蜂窝模块内置定位能力；具体能力取决于 EG25/RM520N 及项目配置'],
    ['天线接口', '2 × 4G（4G1/4G2）；4 × 5G（5G1~5G4）；2 × Wi-Fi（WIFI0/WIFI1）'],
    ['USB', '多路 USB Host 接口（具体数量与位置以接口图为准）'],
    ['电源输入', 'DC 12-48 V 宽压输入；推荐 12 V / 3 A 或以上电源'],
    ['功耗', '满载功耗 < 25 W；瞬时峰值可能随多路无线并发、网络状态和业务负载变化；建议预留不少于 35 W 的电源功率预算'],
    ['按键/复位', '系统复位按键（恢复出厂功能以固件为准）'],
    ['指示灯', '多路系统/网络状态指示灯；具体灯态定义以固件版本和项目配置为准'],
    ['看门狗', '板载硬件看门狗，支持异常恢复'],
    ['工作温度', '支持 -40 °C 冷启动并可长期工作至 +70 °C'],
    ['防护与可靠性', '支持硬件看门狗、链路保活、异常恢复和定时/空闲重启等可靠性机制'],
]))

blocks.append(heading('3.1 蜂窝通信参考', 2))
blocks.append(p('下表蜂窝通信参数以 Quectel EG25 与 RM520N-GL 为参考。不同目标区域的模块选型、频段、认证、天线配置、产品标签和运营商准入要求，应在项目阶段确认。'))
blocks.append(p('表 3-2. 4G 蜂窝通信参考参数（EG25）', bold=True))
blocks.append(table(['项目', '参考规格'], [
    ['参考 LTE 模块', 'Quectel EG25-G / EG25 系列（以实际 BOM 为准）'],
    ['LTE 制式', 'LTE Cat 4'],
    ['LTE FDD 频段', '以所选 EG25 区域版本数据手册为准（如 B1/B3/B5/B7/B8/B20/B28 等）'],
    ['LTE TDD 频段', '以所选 EG25 区域版本数据手册为准（如 B34/B38/B39/B40/B41 等）'],
    ['3G/2G 频段', '以所选 EG25 区域版本数据手册为准'],
    ['LTE 吞吐率', '最大 150 Mbps 下行 / 最大 50 Mbps 上行（Cat 4 参考值）'],
    ['模块数量', '2 路独立 EG25 模块，支持双卡双通/多 WAN 策略'],
]))
blocks.append(p('表 3-3. 5G 蜂窝通信参考参数（RM520N-GL）', bold=True))
blocks.append(table(['项目', '参考规格'], [
    ['参考 5G 模块', 'Quectel RM520N-GL'],
    ['5G 制式', '5G NR（NSA/SA，以模块与固件支持为准）'],
    ['5G 频段', '以 RM520N-GL 全球版数据手册为准'],
    ['LTE 回退', '支持 LTE/3G/2G 回退（以模块能力为准）'],
    ['接口形式', 'M.2 Key B / USB/PCIe（以硬件设计为准）'],
    ['5G 吞吐率', '以 Quectel RM520N-GL 数据手册为准'],
    ['天线', '4 × 5G 天线接口（5G1~5G4）'],
]))

blocks.append(heading('3.2 Wi-Fi 通信参考', 2))
blocks.append(p('表 3-4. Wi-Fi 通信参考参数（AP6398）', bold=True))
blocks.append(table(['项目', '参考规格'], [
    ['Wi-Fi 芯片', 'Broadcom AP6398（SDIO）'],
    ['标准', 'IEEE 802.11 b/g/n/ac，2.4 GHz / 5 GHz 双频'],
    ['MIMO', '2T2R'],
    ['理论 PHY 速率', '2.4 GHz 最高 300 Mbps；5 GHz 最高 867 Mbps（参考值）'],
    ['默认固件配置', '5 GHz AC，80 MHz 带宽（可按项目调整）'],
    ['天线', '2 × Wi-Fi 天线接口（WIFI0/WIFI1）'],
]))

blocks.append(heading('4. 接口资源与电气定义', 1))
blocks.append(p('本章说明 720F 的主要接口位置、连接器形式和电气定义。接口标注图用于快速识别器件和连接器位置，具体 pinout、线序和项目配置要求以对应表格及接口图为准。'))
blocks.append(heading('4.1 前面板/接口布局', 2))
blocks.append(placeholder('图4-1. 720F 接口布局总图（基于 F720 V1.0 丝印，待补充高清图）'))
blocks.append(p('表 4-1. 主要接口定义', bold=True))
blocks.append(table(['接口', '定义/说明'], [
    ['电源输入', 'VCC+ / VCC-，DC 12-48 V 宽压输入；具体连接器型号以 BOM 为准'],
    ['SIM 卡槽 ×3', '分别对应 EG25-1、EG25-2 与 RM520N；上电状态下禁止插拔'],
    ['以太网', 'M12 8 芯工业连接器；内部交换提供 1×WAN + 3×LAN 千兆端口'],
    ['RS232 串口', '25Pin J30J 连接器，引出 6 路 RS232 串口'],
    ['GPIO', 'GPIO1，3.3 V 可编程输入/输出'],
    ['4G 天线', '4G1、4G2（IPEX 或项目指定天线座）'],
    ['5G 天线', '5G1、5G2、5G3、5G4（IPEX 或项目指定天线座）'],
    ['Wi-Fi 天线', 'WIFI0、WIFI1'],
    ['USB', 'USB Host 接口（具体定义以接口图为准）'],
    ['复位按键', '系统启动后长按约 5-8 秒后松开，可恢复出厂设置并重启（以固件为准）'],
]))

blocks.append(heading('4.1.1 M12 以太网接口线序', 3))
blocks.append(p('720F 提供 4 个 M12 8 芯工业以太网座子，丝印标识为 P0、P1、P2、P3（接口区域自左向右依次为 P3、P2、P1、P0）。四个座子线序定义相同，内部经 RTL8365MB 千兆交换芯片分别接入 port0~port3（对应 lan1、lan2、lan3、wan）。进行线束或转接设计时，应结合 M12 线序、以太网变压器、ESD 防护、线缆长度和项目结构要求确认。'))
blocks.append(p('表 4-2. M12 8 芯以太网线序（P0 / P1 / P2 / P3 通用）', bold=True))
blocks.append(p('数据来源：F720_V1.0 丝印说明。TXVP 表示差分正（+），TXVN 表示差分负（-），A/B/C/D 表示千兆以太网四个线对。', bold=False))
blocks.append(table(['M12 Pin', '丝印信号名', '线对', '说明'], [
    ['1', 'TXVN-C', 'C', '线对 C 负'],
    ['2', 'TXVP-D', 'D', '线对 D 正'],
    ['3', 'TXVN-D', 'D', '线对 D 负'],
    ['4', 'TXVN-A', 'A', '线对 A 负'],
    ['5', 'TXVP-B', 'B', '线对 B 正'],
    ['6', 'TXVP-A', 'A', '线对 A 正'],
    ['7', 'TXVP-C', 'C', '线对 C 正'],
    ['8', 'TXVN-B', 'B', '线对 B 负'],
]))
blocks.append(p('表 4-3. M12 接口标识', bold=True))
blocks.append(table(['丝印标识', '说明'], [
    ['P0', 'M12 8 芯以太网座子，线序同表 4-2'],
    ['P1', 'M12 8 芯以太网座子，线序同表 4-2'],
    ['P2', 'M12 8 芯以太网座子，线序同表 4-2'],
    ['P3', 'M12 8 芯以太网座子，线序同表 4-2'],
]))
blocks.append(p('注：四个 M12 座子线序完全一致。接口区域丝印自左向右依次为 P3、P2、P1、P0。各座子与内部 RTL8365MB 交换端口（lan1/lan2/lan3/wan）的对应关系请参照 F720_V1.0 原理图。'))

blocks.append(heading('4.1.2 J30J-25Pin RS232 接口线序', 3))
blocks.append(p('720F 的 6 路 RS232 串口通过 25Pin J30J 连接器集中引出。丝印按双列 25Pin 排列，Pin 1~13 为上排，Pin 14~25 为下排。下表引脚定义来源于 F720_V1.0 丝印说明。'))
blocks.append(p('表 4-4. J30J-25Pin 引脚定义总表', bold=True))
blocks.append(table(['Pin', '信号名', 'Pin', '信号名'], [
    ['1', 'GPIO1', '14', '3.3VD9'],
    ['2', 'GND', '15', 'UART9-RX'],
    ['3', 'UART3-RX', '16', 'UART9-TX'],
    ['4', 'UART3-TX', '17', 'GND9'],
    ['5', 'GND5', '18', '3.3VD4'],
    ['6', 'UART5-RX', '19', 'UART4-RX'],
    ['7', 'UART5-TX', '20', 'UART4-TX'],
    ['8', '3.3VD5', '21', 'GND4'],
    ['9', 'VCC-', '22', '3.3VD7'],
    ['10', 'VCC-', '23', 'UART7-RX'],
    ['11', 'VCC+', '24', 'UART7-TX'],
    ['12', 'VCC+', '25', 'GND7'],
    ['13', 'VCC+', '', ''],
]))
blocks.append(p('表 4-5. J30J RS232 串口信号分组', bold=True))
blocks.append(table(['串口', 'UART', 'RX Pin', 'TX Pin', 'GND Pin', '辅助 Pin', '说明'], [
    ['COM1 / 串口1', 'UART3', '3', '4', '2', '-', 'RS232 收发'],
    ['COM2 / 串口2', 'UART5', '6', '7', '5', '8 (3.3VD5)', 'RS232 收发'],
    ['COM3 / 串口3', 'UART4', '19', '20', '21', '18 (3.3VD4)', 'RS232 收发'],
    ['COM4 / 串口4', 'UART7', '23', '24', '25', '22 (3.3VD7)', 'RS232 收发'],
    ['COM5 / 串口5', 'UART9', '15', '16', '17', '14 (3.3VD9)', 'RS232 收发'],
    ['GPIO', '-', '-', '-', '-', '1 (GPIO1)', '可配置输入/输出，非串口'],
]))
blocks.append(p('表 4-6. J30J 电源引脚说明', bold=True))
blocks.append(table(['Pin', '信号', '说明'], [
    ['9~10', 'VCC-', '电源负极（与整机 DC 输入共地参考）'],
    ['11~13', 'VCC+', '电源正极引出（具体用途以原理图为准）'],
]))
blocks.append(p('注：3.3VDx 为对应 RS232 通道收发器辅助电源/参考脚，接线请以原理图为准。J30J 丝印可见 UART3/4/5/7/9 五组完整 RS232 信号；其余 RS232 资源分配以最新硬件版本与项目配置为准。'))

blocks.append(heading('4.2 指示灯说明', 2))
blocks.append(p('表 4-7. 指示灯状态说明（参考）', bold=True))
blocks.append(table(['指示灯', '状态说明'], [
    ['系统指示灯', '上电早期常亮；启动/自检时慢闪；拨号或升级时快闪；联网成功后常亮（以固件为准）'],
    ['蜂窝/信号指示灯', '用于显示蜂窝注册、拨号及信号强度状态（以固件版本为准）'],
    ['以太网指示灯', '显示链路连接状态（以硬件设计为准）'],
]))

blocks.append(heading('5. 联网与路由能力', 1))
blocks.append(heading('5.1 互联网接入模式', 2))
for item in [
    '4G LTE 蜂窝接入（双 EG25 独立链路）',
    '5G NR 蜂窝接入（RM520N-GL）',
    '蜂窝 Modem 透传模式',
    '有线 WAN 接入互联网',
    '2.4 GHz / 5 GHz 无线接入、桥接、中继、多 SSID 优先连接',
    '混合（Mix）多 WAN 模式：默认工作模式，支持多链路并存与策略路由',
]:
    blocks.append(bullet(item))

blocks.append(heading('5.2 蜂窝网络功能', 2))
for item in [
    '支持多路 SIM 独立拨号与管理',
    '支持自定义 APN、SIM PIN、短信管理、制式锁定、频段锁定、小区锁定、SIM 锁、模块锁及自定义 AT 查询/配置',
    '支持双 4G + 5G 多链路冷备份、热备份与负载均衡',
    '支持按客户端或应用指定出口链路',
]:
    blocks.append(bullet(item))

blocks.append(heading('5.3 网络可靠性', 2))
for item in [
    '多种网络保活机制：ICMP 检测、DNS 检测、接收包计数检测',
    '连续失败自动恢复、链路切换与异常重启',
    '硬件看门狗与软件保活协同',
]:
    blocks.append(bullet(item))

blocks.append(heading('6. 业务协议与应用能力', 1))
blocks.append(heading('6.1 串口协议能力', 2))
for item in [
    '串口透明传输：帧长度、帧间隔、流控、注册包/保活包、包前后缀、激活包、多服务器/客户端、多协议并行、流量统计',
    'Modbus RTU 转 Modbus TCP',
    'MQTT 数据透传',
    'HTTP POST 数据上报',
    'HE 指令模式网关管理',
    '支持 SDK 和组件开发实现自定义串口协议转换',
]:
    blocks.append(bullet(item))

blocks.append(heading('6.2 IO 与工业控制', 2))
for item in [
    'GPIO 输入/输出切换与状态上报',
    '支持 MQTT 订阅发布控制 IO',
    '支持远程协议控制 IO 模式与状态',
    '接入远程管理平台后，可通过平台 API 或界面进行 IO 管理',
]:
    blocks.append(bullet(item))

blocks.append(heading('6.3 定位能力', 2))
blocks.append(p('表 6-1. 定位能力与数据上报方式', bold=True))
blocks.append(table(['能力', '说明'], [
    ['定位来源', '蜂窝模块内置定位（EG25 / RM520N-GL）'],
    ['数据上报', '支持 NMEA、MQTT、HTTP JSON 等上报方式（以固件配置为准）'],
    ['平台管理', '接入远程管理平台后可查看和管理定位数据'],
]))

blocks.append(heading('6.4 网络应用能力', 2))
blocks.append(p('表 6-2. 网络应用功能列表', bold=True))
blocks.append(table(['类别', '功能'], [
    ['防火墙/NAT', '防火墙、NAT/DMZ、端口映射/代理'],
    ['路由', '静态路由、策略路由、源地址/端口路由'],
    ['高级网络', 'DDNS、UPnP、域名重定向、IGMP 代理'],
    ['动态路由', 'RIPv1/RIPv2/RIPng、OSPFv2/OSPFv3（以固件包为准）'],
    ['监控与管理', '终端流量监控、访问控制、WOL、SNMP（支持自定义 OID）'],
    ['时间管理', 'NTP 客户端/服务端'],
]))

blocks.append(heading('6.5 VPN 与专网', 2))
for item in [
    'PPTP、L2TP、IPsec、GRE 隧道',
    'OpenVPN 客户端，支持预共享密钥或证书',
    'WireGuard',
    '多网关 HA（VRRP，以固件支持为准）',
]:
    blocks.append(bullet(item))

blocks.append(heading('6.6 内网穿透与异地组网', 2))
blocks.append(p('表 6-3. 内网穿透与异地组网能力', bold=True))
blocks.append(table(['能力', '说明'], [
    ['内网穿透', '支持将内网服务映射至公网访问'],
    ['异地组网', '支持多设备组成虚拟专网'],
    ['典型用途', '远程访问 Web UI、下游设备管理页面、串口服务或项目指定服务'],
]))

blocks.append(heading('7. 管理、运维与开放集成', 1))
blocks.append(heading('7.1 本地与远程管理', 2))
for item in [
    '本地 Web UI 管理（默认端口 80）',
    'SSH（默认端口 22）与 Telnet（默认端口 23）',
    '本地 TCP 控制、HTTP 状态查询、局域网广播发现',
    '短信管理（重启/重置/查询/配置，以模块与配置为准）',
    '远程 HTTP 控制、MQTT 状态上报与告警',
    '云平台批量管理、远程登录、远程维护',
    '提供本地管理工具包、编程 API 及远程管理服务器 SDK',
]:
    blocks.append(bullet(item))

blocks.append(heading('7.2 远程运维能力', 2))
blocks.append(p('表 7-1. 远程运维能力说明', bold=True))
blocks.append(table(['能力', '说明'], [
    ['设备状态', '在线状态、接口状态、信号强度、流量统计等'],
    ['远程访问', '远程 Web UI、SSH/Telnet、串口 CLI（以项目配置为准）'],
    ['下游设备维护', '可访问网关下游设备的管理页面或服务端口'],
    ['配置管理', '配置备份、下发与批量管理（以项目配置为准）'],
    ['API/SDK', '提供远程管理 API/SDK 用于客户平台集成'],
]))
blocks.append(placeholder('图7-1. Web UI 登录界面（待补充）'))
blocks.append(placeholder('图7-2. Web UI 管理首页（待补充）'))
blocks.append(p('表 7-2. Web UI 默认访问信息', bold=True))
blocks.append(table(['项目', '默认值'], [
    ['默认管理 IP', '192.168.8.1（或项目定义的管理 IP）'],
    ['Web 端口', '80'],
    ['SSH 端口', '22'],
    ['Telnet 端口', '23'],
    ['默认 SSID', '可使用设备型号标识和 MAC 地址后缀；具体前缀可按项目配置'],
]))

blocks.append(heading('7.3 常见工作模式', 2))
blocks.append(p('表 7-3. 常见工作模式', bold=True))
blocks.append(table(['模式', '说明'], [
    ['4G/5G 路由模式', '通过蜂窝网络接入互联网，并为客户端提供网络访问'],
    ['有线路由模式', '通过有线 WAN 接入互联网，并为下游客户端提供访问'],
    ['混合（Mix）模式', '多链路同时可用，支持备份、负载均衡与策略路由（出厂默认）'],
    ['桥接/中继模式', '可作为无线桥接或中继节点（以固件配置为准）'],
]))

blocks.append(heading('7.4 升级、定制与开发支持', 2))
blocks.append(p('表 7-4. 升级、定制与开发支持', bold=True))
blocks.append(table(['类别', '说明'], [
    ['固件升级', '支持本地升级、在线升级和 LAN 批量升级；提供 OTA 工具'],
    ['配置管理', '配置备份、恢复与批量下发'],
    ['Web UI 定制', '可按项目定制界面与权限'],
    ['组件化扩展', '支持 SkinOS 软件包安装、卸载、恢复和部署'],
    ['SDK/API', '提供本地/远程管理 API、开发文档和客户平台集成支持'],
    ['协议定制', '可按项目定制串口、IO、远程管理或私有协议转换逻辑'],
]))

blocks.append(heading('8. 型号配置', 1))
blocks.append(p('720F 当前为标准单一型号，固定包含双 EG25 4G 模块与 RM520N-GL 5G 模块配置。'))
blocks.append(p('表 8-1. 720F 标准配置说明', bold=True))
blocks.append(table(['配置项', '720F 标准版'], [
    ['RK3568 主控平台', 'V'],
    ['双 Quectel EG25 4G LTE', 'V'],
    ['Quectel RM520N-GL 5G NR（M.2）', 'V'],
    ['3 × SIM 卡槽', 'V'],
    ['1×WAN + 3×LAN 千兆交换', 'V'],
    ['AP6398 双频 Wi-Fi', 'V'],
    ['6 路 RS232（J30J-25Pin）', 'V'],
    ['GPIO', 'V'],
    ['蜂窝内置定位', 'V'],
]))

blocks.append(heading('9. 系统与交付说明', 1))
blocks.append(p('表 9-1. 订购、交付与项目资料说明', bold=True))
blocks.append(table(['项目', '说明'], [
    ['系统平台', 'Talonbox SkinOS 嵌入式系统（支持组件化开发与接口扩展）'],
    ['文档配套', '接口图、规格图、丝印说明（F720_V1.0）、机械图纸（待项目确认）'],
    ['工程资料', '可在项目评估阶段提供接口布局、连接器参考、pinout、电气说明及 DXF/STEP 文件'],
    ['交付周期', '最终时间取决于数量、认证要求、器件供应、测试安排和定制范围'],
    ['定制说明', '如需特殊频段、接口、电源、外壳或协议能力，可按项目定制'],
]))

blocks.append(heading('10. 声明与注意事项', 1))
for item in [
    '本文档为 720F 客户评审版资料，图片部分为占位，正式发版前将补充高清产品图与 pinout 图。',
    '最终配置、频段、天线、标签、认证范围、软件功能和交付资料以项目确认版本为准。',
    '模块级器件上显示的认证标识仅适用于模块本体，并不自动代表 720F 整机已取得相应认证。',
    '客户最终产品（包括外壳、天线布置、电源设计、连接器/线束、安装方式、标签和集成方案）可能需要额外认证或工程评审。',
    '蜂窝模块选型、支持频段、天线配置、运营商准入和交付时间可能随目标市场和项目配置变化。',
]:
    blocks.append(bullet(item))

DOCUMENT_XML = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="{WNS}">
  <w:body>
    {''.join(blocks)}
    <w:sectPr><w:pgSz w:w="11906" w:h="16838"/><w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440"/></w:sectPr>
  </w:body>
</w:document>'''

CONTENT_TYPES = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
  <Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
  <Override PartName="/word/numbering.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.numbering+xml"/>
</Types>'''

RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>'''

DOC_RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>
  <Relationship Id="rId2" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/numbering" Target="numbering.xml"/>
</Relationships>'''

STYLES = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles xmlns:w="{WNS}">
  <w:style w:type="paragraph" w:default="1" w:styleId="Normal">
    <w:name w:val="Normal"/>
    <w:rPr><w:rFonts w:ascii="宋体" w:hAnsi="宋体" w:eastAsia="宋体"/><w:sz w:val="22"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/><w:basedOn w:val="Normal"/><w:rPr><w:b/><w:sz w:val="32"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="Heading2"><w:name w:val="heading 2"/><w:basedOn w:val="Normal"/><w:rPr><w:b/><w:sz w:val="28"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="Heading3"><w:name w:val="heading 3"/><w:basedOn w:val="Normal"/><w:rPr><w:b/><w:sz w:val="24"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="ListBullet"><w:name w:val="List Bullet"/><w:basedOn w:val="Normal"/></w:style>
</w:styles>'''

NUMBERING = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:numbering xmlns:w="{WNS}">
  <w:abstractNum w:abstractNumId="0">
    <w:lvl w:ilvl="0"><w:start w:val="1"/><w:numFmt w:val="bullet"/><w:lvlText w:val="•"/><w:lvlJc w:val="left"/></w:lvl>
  </w:abstractNum>
  <w:num w:numId="1"><w:abstractNumId w:val="0"/></w:num>
</w:numbering>'''


def main():
    with zipfile.ZipFile(OUT, 'w', compression=zipfile.ZIP_DEFLATED) as z:
        z.writestr('[Content_Types].xml', CONTENT_TYPES)
        z.writestr('_rels/.rels', RELS)
        z.writestr('word/document.xml', DOCUMENT_XML)
        z.writestr('word/_rels/document.xml.rels', DOC_RELS)
        z.writestr('word/styles.xml', STYLES)
        z.writestr('word/numbering.xml', NUMBERING)
    print(f'Wrote {OUT} ({OUT.stat().st_size} bytes)')


if __name__ == '__main__':
    main()
