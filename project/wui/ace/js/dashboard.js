var flush_interval = 2;

function adjustBoxLayout() {
    var containers = $('.responsive-flex-container');

    var greenGradient = [
        "rgb(59, 138, 72)",
        "rgb(64, 154, 79)",
        "rgb(67, 169, 84)",
        "rgb(71, 187, 90)",
        "rgb(77, 189, 95)",
        "rgb(86, 197, 104)",
        "rgb(97, 209, 114)",
        "rgb(108, 220, 123)",
        "rgb(118, 224, 134)",
        "rgb(128, 232, 144)"
    ];

    containers.each(function() {
        var container = $(this);
        var containerPadding = 10
        var availableWidth = container.width() - containerPadding;

        // 在线图表排列顺序规则
        var boxes = container.find('.pricing-box').get();
        boxes.sort(function(a, b) {
            var aOnline = $(a).attr('or-online') === 'true' ? 1 : 0;
            var bOnline = $(b).attr('or-online') === 'true' ? 1 : 0;
            return bOnline - aOnline; 
        });
        $.each(boxes, function(i, box) { 
            container.append(box);
            $(box).find('.widget-box, .widget-header').css({
                'border-color': '',
                'background-color': '',
                'border-bottom-color': ''
            }); 
        });

        // 在线可见的图表 重置框架颜色 绿色
        var onlineBoxes = container.find('.pricing-box[or-online="true"]:visible');
        onlineBoxes.each(function(index) {
            var color = (index < greenGradient.length) ? greenGradient[index] : greenGradient[9];

            var $widget = $(this).find('.widget-box');
            var $header = $(this).find('.widget-header');

            // 使用 style.setProperty 来强制覆盖CSS里的 !important
            $widget[0].style.setProperty('border-color', color, 'important');
            $header[0].style.setProperty('background-color', color, 'important');
            $header[0].style.setProperty('border-bottom-color', color, 'important');
        });

        // 获取可见图表
        var visibleBoxes = container.find('.pricing-box').filter(function() {
            return $(this).css('display') !== 'none';
        });
        var count = visibleBoxes.length;
        if (count === 0) return;

        visibleBoxes.removeClass(
            'layout-1 layout-2 layout-3 layout-4-last layout-5-last ' +
            'layout-6-last layout-7-last layout-8-last layout-9 layout-10-last'
        );
        
        // 找到所有可见图表中 内部表格最宽的那一个作为基准
        var maxRequiredWidth = 0;
        visibleBoxes.each(function() {
            var table = $(this).find('table')[0];
            if (table) {
                // scrollWidth 代表内容的真实宽度 即使被溢出隐藏
                if (table.scrollWidth > maxRequiredWidth) {
                    maxRequiredWidth = table.scrollWidth;
                }
            }
        });

        // 设定一个保底的最小宽度 防止没有数据时计算出错
        maxRequiredWidth = Math.max(maxRequiredWidth, 380); 

       // 动态决定排列几行 增强响应性能 不写死window width
        var finalCols = 1;
        if (availableWidth / 3 >= maxRequiredWidth) {
            finalCols = 3;
        } else if (availableWidth / 2 >= maxRequiredWidth) {
            finalCols = 2;
        } else {
            finalCols = 1;
        }

        if (finalCols === 3) {
            apply3ColumnLogic(visibleBoxes, count);
        } else if (finalCols === 2) {
            if (count === 1) {
                visibleBoxes.addClass('layout-1');
            } else if (count % 2 === 0) {
                visibleBoxes.addClass('layout-2');
            } else {
                visibleBoxes.slice(0, count - 1).addClass('layout-2');
                visibleBoxes.slice(count - 1).addClass('layout-1');
            }
        } else {
            visibleBoxes.addClass('layout-1');
        }
    });
}

// 默认图表三列布局
function apply3ColumnLogic(visibleBoxes, count) {
    if (count === 1) {
        visibleBoxes.addClass('layout-1');
    } else if (count === 2) {
        visibleBoxes.addClass('layout-2');
    } else if (count === 3) {
        visibleBoxes.addClass('layout-3');
    } else if (count === 4) {
        visibleBoxes.slice(0, 3).addClass('layout-3');
        visibleBoxes.slice(3).addClass('layout-4-last');
    } else if (count === 5) {
        visibleBoxes.slice(0, 3).addClass('layout-3');
        visibleBoxes.slice(3).addClass('layout-5-last');
    } else if (count === 6) {
        visibleBoxes.addClass('layout-3');
        visibleBoxes.slice(3).addClass('layout-6-last');
    } else if (count === 7) {
        visibleBoxes.slice(0, 6).addClass('layout-3');
        visibleBoxes.slice(6).addClass('layout-7-last');
    } else if (count === 8) {
        visibleBoxes.slice(0, 6).addClass('layout-3');
        visibleBoxes.slice(6).addClass('layout-8-last');
    } else if (count === 9) {
        visibleBoxes.addClass('layout-3');
    } else if (count >= 10) {
        visibleBoxes.slice(0, 9).addClass('layout-3');
        visibleBoxes.slice(9).addClass('layout-10-last');
    }
}

// 监听容器大小 解决图表框架压缩显示
if (window.ResizeObserver) {
    const ro = new ResizeObserver(entries => {
        window.requestAnimationFrame(() => {
            adjustBoxLayout();
        });
    });
    $('.responsive-flex-container').each(function() { ro.observe(this); });
} else {
    $(window).on('resize', adjustBoxLayout);
}

function showChart(id) {
    $(id).show();
    adjustBoxLayout();
}

function lte_show(info, id) {
    if (!info) {
        $(id).hide().attr('or-online', 'false'); // 隐藏并标记非在线;
        return;
    }
    var isOnline = (info.status === "up");
    $(id).attr('or-online', isOnline ? 'true' : 'false');

    // 状态和按钮
    if (info.status) {
        showChart(id);
        $(id + "_btn").html('<i class="ace-icon fa fa-pause"></i>');
        $(id + "_status").text($.i18n(info.status));
        
        if (info.status === "up" || 
            info.status === "uping" || 
            info.status === "connect" || 
            info.status === "connecting") {
            // 状态正常
        } else {
            $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        }
    } else {
        $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        $(id + "_status").text($.i18n("down"));
    }

    // 填充所有数据
    
    // 运营商
    var operatorText = "";
    if (info.operator) {
        operatorText = $.i18n(info.operator);
    } else if (info.plmn) {
        operatorText = $.i18n(info.plmn);
    }
    
    // 网络类型
    $(id + "_nettype").text(info.nettype || "");
    $(id + "_nettype5").text(info.nettype2 || "");
    
    // CSQ
    $(id + "_csq").text(info.csq || "");
    
    // 信号图标 - 4G
    if (info.signal) {
        $(id + "_rssiimg").attr("src", "/assets/css/images/signal_" + info.signal + ".png");
    } else {
        $(id + "_rssiimg").attr("src", "/assets/css/images/signal_0.png");
    }
    
    // 信号图标 - 5G
    if (info.signal2) {
        $(id + "_rssiimg5").attr("src", "/assets/css/images/signal_" + info.signal2 + ".png");
    } else {
        $(id + "_rssiimg5").attr("src", "/assets/css/images/signal_0.png");
    }
    
    // RSSI/RSRP - 4G
    if (info.rssi) {
        $(id + "_rssi").text(info.rssi + "dBm");
    } else {
        $(id + "_rssi").text("");
    }
    
    if (info.rsrp) {
        $(id + "_rsrp").text("RSRP:" + info.rsrp + "dBm");
    } else {
        $(id + "_rsrp").text("");
    }
    
    // RSRP - 5G
    if (info.rsrp2) {
        $(id + "_rsrp5").text(info.rsrp2 + "dBm");
    } else {
        $(id + "_rsrp5").text("");
    }
    
    // RSRQ/SINR - 4G
    if (info.rsrq) {
        $(id + "_rsrq").text("RSRQ:" + info.rsrq + "dB");
    } else {
        $(id + "_rsrq").text("");
    }
    
    if (info.sinr) {
        $(id + "_sinr").text("SINR:" + info.sinr + "dB");
    } else {
        $(id + "_sinr").text("");
    }
    
    // RSRQ/SINR - 5G
    if (info.rsrq2) {
        $(id + "_rsrq5").text("RSRQ:" + info.rsrq2 + "dB");
    } else {
        $(id + "_rsrq5").text("");
    }
    
    if (info.sinr2) {
        $(id + "_sinr5").text("SINR:" + info.sinr2 + "dB");
    } else {
        $(id + "_sinr5").text("");
    }

    // 频段 - 4G
    $(id + "_band").text(info.band || "");
    
    // 频段 - 5G
    $(id + "_band5").text(info.band2 || "");
    
    // 设备信息
    $(id + "_vidpid").text(info.name || "");
    $(id + "_imei").text(info.imei || "");
    $(id + "_imsi").text(info.imsi || "");
    $(id + "_iccid").text(info.iccid || "");
    
    // LAC/CI/ARFCN
    $(id + "_lac").text(info.lac || "");
    $(id + "_ci").text(info.ci || "");
    $(id + "_arfcn").text(info.arfcn || "");

    if( info.lac && info.ci ){
        $(id + "_separator").show();
    }else{
        $(id + "_separator").hide();
    }

    // IPv4
    $(id + "_ip").text(info.ip || ' ');
    $(id + "_mask").text(info.mask || ' ');
    
    if (!info.gw || info.gw === "0.0.0.0") {
        $(id + "_gateway").text(info.dstip || ' ');
    } else {
        $(id + "_gateway").text(info.gw || ' ');
    }
    
    // DNS
    $(id + "_dns").text(info.dns || ' ');
    $(id + "_dns2").text(info.dns2 || ' ');
    
    // 延迟
    if (info.delay) {
        if (info.delay === "failed" || info.delay === "block") {
            $(id + "_delay").text($.i18n(info.delay));
        } else {
            $(id + "_delay").text(info.delay + "ms");
        }
    } else {
        $(id + "_delay").text("");
    }
    
    // IPv6
    if (info.method && info.method !== "disable") {
        $(id + "_method").text($.i18n(info.method));
        
        $(id + "_addr").text(info.addr || "");
        $(id + "_addr2").text(info.addr2 || "");
        $(id + "_addr3").text(info.addr3 || "");
        $(id + "_hop").text(info.hop || "");
        $(id + "_resolve").text(info.resolve || "");
        $(id + "_resolve2").text(info.resolve2 || "");
    } else {
        $(id + "_method").text("");
        $(id + "_addr").text("");
        $(id + "_addr2").text("");
        $(id + "_addr3").text("");
        $(id + "_hop").text("");
        $(id + "_resolve").text("");
        $(id + "_resolve2").text("");
    }
    
    // 流量统计
    $(id + "_rxtx").text(byte2readable(info.rx_bytes || "0") + " / " + byte2readable(info.tx_bytes || "0"));
    $(id + "_livetime").text(info.livetime || ' ');

    // 控制显示/隐藏
    
    // 判断4G和5G是否有内容
    var has4G = info.signal || info.nettype || info.csq;
    var has5G = info.signal2 || info.nettype2;
    
    // 4G头部显示控制
    if (has4G) {
        $(id + "_4g_head").show();
        // 4G有信号时，运营商加到4G后面
        $(id + "_operator").text(operatorText);
    } else {
        $(id + "_4g_head").hide();
        $(id + "_operator").text("");
    }
    
    // 5G头部显示控制
    if (has5G) {
        $(id + "_5g_head").show();
        // 5G有信号且4G无信号时，运营商加到5G后面
        if (!has4G && operatorText) {
            $(id + "_operator5").text(operatorText);
        } else {
            $(id + "_operator5").text("");
        }
    } else {
        $(id + "_5g_head").hide();
        $(id + "_operator5").text("");
    }
    
    // 没有任何信号但运营商存在
    if (!has4G && !has5G && operatorText) {
        $(id + "_4g_head").show();
        $(id + "_operator").text(operatorText);
        $(id + "_nettype").text("");
        $(id + "_csq").text("");
    }
    
    // 美化显示：无5G且无信号时显示默认图标
    if (!info.nettype2 && !info.signal && !info.signal2) {
        $(id + "_4g_head").show();
    }
    
    // RSSI行显示控制 - 4G
    var hasRssi4 = info.rssi || info.rsrp;
    if (hasRssi4) {
        $(id + "_rssi4_head").show();
    } else {
        $(id + "_rssi4_head").hide();
    }
    
    // RSSI行显示控制 - 5G
    var hasRssi5 = info.rsrp2;
    if (hasRssi5) {
        $(id + "_rssi5_head").show();
    } else {
        $(id + "_rssi5_head").hide();
    }
    
    // 无信号时显示"No Signal"
    if (!info.rssi && !info.rsrp && !info.rsrp2) {
        $(id + "_rssi4_head").show();
        $(id + "_rssi").text($.i18n("nosignal"));
    }
    
    //RSRQ行控制显示 - 4G
    var hasRsrq4 = info.rsrq || info.sinr;
    if (hasRsrq4) {
        $(id + "_rsrq4_head").show();
    } else {
        $(id + "_rsrq4_head").hide();
    }

    //RSRQ行控制显示 - 5G
    var hasRsrq5 = info.rsrq2 || info.sinr2;
    if (hasRsrq5) {
        $(id + "_rsrq5_head").show();
    } else {
        $(id + "_rsrq5_head").hide();
    }

    //Band行控制显示 - 4G
    if(info.band){
        $(id + "_band4_head").show();
    }else{
        $(id + "_band4_head").hide();
    }

    //Band行控制显示 - 5G
    if(info.band2){
        $(id + "_band5_head").show();
    }else{
        $(id + "_band5_head").hide();
    }

    // 延迟头部显示控制
    if (info.delay) {
        $(id + "_delay_head").show();
    } else {
        $(id + "_delay_head").hide();
    }

    // IPv6头部显示控制
    if (info.method && info.method !== "disable") {
        $(id + "_addr_head").show();
        $(id + "_hop_head").show();
    } else {
        $(id + "_addr_head").hide();
        $(id + "_hop_head").hide();
    }
}

function wan_show(info, id) {
    if (!info) {
        $(id).hide().attr('or-online', 'false');
        return;
    }
    var isOnline = (info.status === "up");
    $(id).attr('or-online', isOnline ? 'true' : 'false');

    // 状态和按钮
    if (info.status) {
        showChart(id);
        $(id + "_btn").html('<i class="ace-icon fa fa-pause"></i>');
        $(id + "_status").text($.i18n(info.status));
        
        if (info.status === "up" || 
            info.status === "uping" || 
            info.status === "connect" || 
            info.status === "connecting") {
            if (info.step && info.step !== "online") {
                $(id + "_status").text($.i18n(info.step));
            }
        } else if (info.status === "down") {
            if (info.step && info.step !== "online") {
                $(id + "_status").text($.i18n(info.step));
            }
            $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        }
        
        if (info.error) {
            $(id + "_status").text($.i18n(info.error));
        }
    } else {
        $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        $(id + "_status").text($.i18n("down"));
    }
    // 模式
    if (info.mode) {
        $(id + "_mode").text($.i18n(info.mode));
    } else {
        $(id + "_mode").text("");
    }
    
    // MAC地址
    $(id + "_mac").text(info.mac || ' ');
    
    // IPv4
    $(id + "_ip").text(info.ip || ' ');
    $(id + "_mask").text(info.mask || ' ');
    
    if (!info.gw || info.gw === "0.0.0.0") {
        $(id + "_gateway").text(info.dstip || ' ');
    } else {
        $(id + "_gateway").text(info.gw || ' ');
    }
    
    // DNS
    $(id + "_dns").text(info.dns || ' ');
    $(id + "_dns2").text(info.dns2 || ' ');
    
    // IPv6
    if (info.method && info.method !== "disable") {
        $(id + "_addr_head").show();
        $(id + "_hop_head").show();

        $(id + "_method").text($.i18n(info.method));
        
        if (info.addr) {
            $(id + "_addr").text(info.addr);
        } else {
            $(id + "_addr").text("");
        }
        
        if (info.addr2) {
            $(id + "_addr2").text(info.addr2);
        } else {
            $(id + "_addr2").text("");
        }
        
        if (info.addr3) {
            $(id + "_addr3").text(info.addr3);
        } else {
            $(id + "_addr3").text("");
        }
        
        if (info.hop) {
            $(id + "_hop").text(info.hop);
        } else {
            $(id + "_hop").text("");
        }
        
        if (info.resolve) {
            $(id + "_resolve").text(info.resolve);
        } else {
            $(id + "_resolve").text("");
        }
        
        if (info.resolve2) {
            $(id + "_resolve2").text(info.resolve2);
        } else {
            $(id + "_resolve2").text("");
        }
    } else {
        $(id + "_addr_head").hide();
        $(id + "_hop_head").hide();

        $(id + "_method").text("");
        $(id + "_addr").text("");
        $(id + "_addr2").text("");
        $(id + "_addr3").text("");
        $(id + "_hop").text("");
        $(id + "_resolve").text("");
        $(id + "_resolve2").text("");
    }
    
    // 流量统计
    $(id + "_rxtx").text(byte2readable(info.rx_bytes || "0") + " / " + byte2readable(info.tx_bytes || "0"));
    
    // 延迟
    if (info.delay) {
        $(id + "_ack_head").show();
        if (info.delay === "failed" || info.delay === "block") {
            $(id + "_ack").text($.i18n(info.delay));
        } else {
            $(id + "_ack").text(info.delay + "ms");
        }
    } else {
        $(id + "_ack_head").hide();
        $(id + "_ack").text("");
    }
    
    // 在线时间
    $(id + "_livetime").text(info.livetime || ' ');
}

function lan_show(info, id) {
    if (!info) {
        $(id).hide();
        return;
    }
    
    // 状态
    if (info.status) {
        showChart(id);
        $(id + "_status").text($.i18n(info.status));
        $(id + "_mac").text(info.mac || ' ');
        
        if (info.status === "up") {
            // IPv4
            $(id + "_ip").text(info.ip || ' ');
            $(id + "_mask").text(info.mask || ' ');
            $(id + "_rxtx").text(byte2readable(info.rx_bytes || "0") + " / " + byte2readable(info.tx_bytes || "0"));
            
            // IPv6
            if (info.method && info.method !== "disable") {
                $(id + "_addr_head").show();

                $(id + "_method").text($.i18n(info.method));
                
                if (info.addr) {
                    $(id + "_addr").text(info.addr);
                } else {
                    $(id + "_addr").text("");
                }
                
                if (info.addr2) {
                    $(id + "_addr2").text(info.addr2);
                } else {
                    $(id + "_addr2").text("");
                }
                
                if (info.addr3) {
                    $(id + "_addr3").text(info.addr3);
                } else {
                    $(id + "_addr3").text("");
                }
                
                if (info.hop) {
                    $(id + "_hop").text(info.hop);
                } else {
                    $(id + "_hop").text("");
                }
            } else {
                $(id + "_addr_head").hide();

                $(id + "_method").text("");
                $(id + "_addr").text("");
                $(id + "_addr2").text("");
                $(id + "_addr3").text("");
                $(id + "_hop").text("");
            }
        }
    } else {
        $(id + "_status").text($.i18n("down"));
    }
}

function wisp_show(info, id) {
    if (!info) {
        $(id).hide().attr('or-online', 'false');
        return;
    }
    var isOnline = (info.status === "up");
    $(id).attr('or-online', isOnline ? 'true' : 'false');

    // 状态和按钮
    if (info.status) {
        showChart(id);
        $(id + "_btn").html('<i class="ace-icon fa fa-pause"></i>');
        $(id + "_status").text($.i18n(info.status));
        
        if (info.status === "up" || 
            info.status === "uping" || 
            info.status === "connect" || 
            info.status === "connecting") {
            if (info.step && info.step !== "online") {
                $(id + "_status").text($.i18n(info.step));
            }
        } else if (info.status === "down") {
            if (info.step && info.step !== "online") {
                $(id + "_status").text($.i18n(info.step));
            }
            $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        }
        
        if (info.error) {
            $(id + "_status").text($.i18n(info.error));
        }
    } else {
        $(id + "_btn").html('<i class="ace-icon fa fa-play"></i>');
        $(id + "_status").text($.i18n("down"));
    }
    
    // 模式
    if (info.mode) {
        $(id + "_mode").text($.i18n(info.mode));
    } else {
        $(id + "_mode").text("");
    }
    
    // WISP特定信息
    $(id + "_peer").text(info.peer || ' ');
    $(id + "_peermac").text(info.peermac || ' ');
    
    if (info.rate) {
        $(id + "_rate").text(info.rate + 'Mbps');
    } else {
        $(id + "_rate").text("");
    }
    
    $(id + "_channel").text(info.channel || ' ');
    
    // 信号
    if (info.sig) {
        $(id + "_rssi").text(info.sig + "%");
    } else if (info.rssi) {
        $(id + "_rssi").text(info.rssi + "dBm");
    } else {
        $(id + "_rssi").text("");
    }
    
    if (info.signal) {
        $(id + "_rssiimg").attr("src", "/assets/css/images/signal_" + info.signal + ".png");
    } else {
        $(id + "_rssiimg").attr("src", "/assets/css/images/signal_0.png");
    }
    
    // MAC地址
    $(id + "_mac").text(info.mac || ' ');
    
    // IPv4
    $(id + "_ip").text(info.ip || ' ');
    $(id + "_mask").text(info.mask || ' ');
    
    if (!info.gw || info.gw === "0.0.0.0") {
        $(id + "_gateway").text(info.dstip || ' ');
    } else {
        $(id + "_gateway").text(info.gw || ' ');
    }
    
    // DNS
    $(id + "_dns").text(info.dns || ' ');
    $(id + "_dns2").text(info.dns2 || ' ');
    
    // IPv6
    if (info.method && info.method !== "disable") {
        $(id + "_addr_head").show();
        $(id + "_hop_head").show();

        $(id + "_method").text($.i18n(info.method));
        
        if (info.addr) {
            $(id + "_addr").text(info.addr);
        } else {
            $(id + "_addr").text("");
        }
        
        if (info.addr2) {
            $(id + "_addr2").text(info.addr2);
        } else {
            $(id + "_addr2").text("");
        }
        
        if (info.addr3) {
            $(id + "_addr3").text(info.addr3);
        } else {
            $(id + "_addr3").text("");
        }
        
        if (info.hop) {
            $(id + "_hop").text(info.hop);
        } else {
            $(id + "_hop").text("");
        }
        
        if (info.resolve) {
            $(id + "_resolve").text(info.resolve);
        } else {
            $(id + "_resolve").text("");
        }
        
        if (info.resolve2) {
            $(id + "_resolve2").text(info.resolve2);
        } else {
            $(id + "_resolve2").text("");
        }
    } else {
        $(id + "_addr_head").hide();
        $(id + "_hop_head").hide();

        $(id + "_method").text("");
        $(id + "_addr").text("");
        $(id + "_addr2").text("");
        $(id + "_addr3").text("");
        $(id + "_hop").text("");
        $(id + "_resolve").text("");
        $(id + "_resolve2").text("");
    }
    
    // 流量统计
    $(id + "_rxtx").text(byte2readable(info.rx_bytes || "0") + " / " + byte2readable(info.tx_bytes || "0"));
    
    // 延迟
    if (info.delay) {
        $(id + "_ack_head").show();
        if (info.delay === "failed" || info.delay === "block") {
            $(id + "_ack").text($.i18n(info.delay));
        } else {
            $(id + "_ack").text(info.delay + "ms");
        }
    } else {
        $(id + "_ack_head").hide();
        $(id + "_ack").text("");
    }
    
    // 在线时间
    $(id + "_livetime").text(info.livetime || ' ');
}

function wifi_24g(info) {
    if (!info || Object.keys(info).length === 0) {
        $("#wifi_24g_container").hide();
        return;
    }
    $("#wifi_24g_container").show();
    $("#wifi_24g_ssid").text(info.ssid || "");
    $("#wifi_24g_bssid").text(info.bssid || "");
    $("#wifi_24g_channel").text(info.channel || "");
}

function wifi_58g(info) {
    if (!info || Object.keys(info).length === 0) {
        $("#wifi_58g_container").hide();
        return;
    }
    $("#wifi_58g_container").show();
    $("#wifi_58g_ssid").text(info.ssid || "");
    $("#wifi_58g_bssid").text(info.bssid || "");
    $("#wifi_58g_channel").text(info.channel || "");
}

function switch_show(ethInfo, wifi24Info, wifi58Info) {
    var hasEthData = ethInfo && Object.keys(ethInfo).length > 0;
    var hasWifi24Data = wifi24Info && Object.keys(wifi24Info).length > 0;
    var hasWifi58Data = wifi58Info && Object.keys(wifi58Info).length > 0;
    
    // 显示WiFi信息
    wifi_24g(wifi24Info);
    wifi_58g(wifi58Info);

    if (!hasEthData && !hasWifi24Data && !hasWifi58Data) {
        $("#switch").hide();
        return;
    }
    
    $("#switch").show();

    if (hasEthData) {
        var container = $("#compact-ports-container");
        container.empty();
        
        var portCount = 0;
        
        // 遍历所有网口信息
        for (var name in ethInfo) {
            if (!ethInfo.hasOwnProperty(name)) continue;
            
            var value = ethInfo[name];
            if (!value) continue;
            
            portCount++;
            
            var portItem = createPortItem(portCount, name, value);
            container.append(portItem);
        }
        
        if (portCount === 0) {
            $("#switch").hide();
        }
    }
}

function createPortItem(index, name, value) {
    var portItem = $('<div>', {
        'class': 'compact-port-item',
        'id': 'compact-port-' + index,
        'style': 'display: block;'
    });
    
    var portWrapper = $('<div>', {
        'class': 'compact-port-wrapper'
    });
    
    var imgContainer = $('<span>', {
        'class': 'port-img-container'
    });
    
    var offlineImg = $('<img>', {
        'src': '/assets/css/images/net_offline.jpg',
        'class': 'port-img offline-img',
        'style': value.status === 'up' ? 'display: none;' : 'display: block;'
    });
    
    var onlineImg = $('<img>', {
        'src': '/assets/css/images/net_online.jpg',
        'class': 'port-img online-img',
        'style': value.status === 'up' ? 'display: block;' : 'display: none;'
    });
    
    imgContainer.append(offlineImg).append(onlineImg);
    
    var portLabel = $('<span>', {
        'class': 'compact-port-label',
        'id': 'sport' + index + 't',
        'text': $.i18n(name) || name,
    });
    
    portWrapper.append(imgContainer);
    portItem.append(portWrapper).append(portLabel);
    
    return portItem;
}

function interface_load() {
    // 获取外网数据
    he.bkload(['network@frame.extern']).then(function(externData) {
        if (!externData) return;
        
        var data = externData[0];
        if (!data) return;
        
        // LTE
        if (data['ifname@lte']) {
            lte_show(data['ifname@lte'], "#lte");
        }
        
        if (data['ifname@lte2']) {
            lte_show(data['ifname@lte2'], "#lte2");
        }
        
        if (data['ifname@lte3']) {
            lte_show(data['ifname@lte3'], "#lte3");
        }
        
        if (data['ifname@lte4']) {
            lte_show(data['ifname@lte4'], "#lte4");
        }
        
        // WISP
        if (window.ifdev && window.ifdev["wifi@n"] === true && data['ifname@wisp']) {
            wisp_show(data['ifname@wisp'], "#wisp");
        }
        
        if (window.ifdev && window.ifdev["wifi@a"] === true && data['ifname@wisp2']) {
            wisp_show(data['ifname@wisp2'], "#wisp2");
        }

        // WAN
        if (data['ifname@wan']) {
            wan_show(data['ifname@wan'], "#wan");
        }
        
        if (data['ifname@wan2']) {
            wan_show(data['ifname@wan2'], "#wan2");
        }
        
        if (data['ifname@wan3']) {
            wan_show(data['ifname@wan3'], "#wan3");
        }
        
        if (data['ifname@wan4']) {
            wan_show(data['ifname@wan4'], "#wan4");
        }
        
        adjustBoxLayout();
    });
    
    // 获取本地数据
    he.bkload(['network@frame.local']).then(function(localData) {
        if (!localData) return;
        
        var data = localData[0];
        if (!data) return;
        
        // LAN
        if (data['ifname@lan']) {
            lan_show(data['ifname@lan'], "#lan");
        }
        
        if (data['ifname@lan2']) {
            lan_show(data['ifname@lan2'], "#lan2");
        }
        
        if (data['ifname@lan3']) {
            lan_show(data['ifname@lan3'], "#lan3");
        }
        
        if (data['ifname@lan4']) {
            lan_show(data['ifname@lan4'], "#lan4");
        }
        adjustBoxLayout();
    });
    
    // 获取以太网状态
    he.bkload(['arch@ethernet.status']).then(function(ethData) {
        if (!ethData) return;
        
        var data = ethData[0];

         // 获取WiFi信息
        he.bkload(['wifi@nssid.status', 'wifi@assid.status']).then(function(wifiData) {
            var wifi24Data = wifiData[0] || null;
            var wifi58Data = wifiData[1] || null;
            
            switch_show(data, wifi24Data, wifi58Data);
            
            adjustBoxLayout();
        });
    });
}

// 按钮事件绑定函数
function bindButtonEvents() {
    $('#lte_btn').on(ace.click_event, function() {
        toggleLteInterface('lte');
    });
    
    $('#lte2_btn').on(ace.click_event, function() {
        toggleLteInterface('lte2');
    });
    
    $('#lte3_btn').on(ace.click_event, function() {
        toggleLteInterface('lte3');
    });
    
    $('#lte4_btn').on(ace.click_event, function() {
        toggleLteInterface('lte4');
    });
    
    $('#wan_btn').on(ace.click_event, function() {
        toggleWanInterface('wan');
    });
    
    $('#wan2_btn').on(ace.click_event, function() {
        toggleWanInterface('wan2');
    });
    
    $('#wan3_btn').on(ace.click_event, function() {
        toggleWanInterface('wan3');
    });
    
    $('#wan4_btn').on(ace.click_event, function() {
        toggleWanInterface('wan4');
    });
    
    $('#wisp_btn').on(ace.click_event, function() {
        toggleWispInterface('wisp');
    });
    
    $('#wisp2_btn').on(ace.click_event, function() {
        toggleWispInterface('wisp2');
    });
}

// LTE设备开关函数
function toggleLteInterface(type) {
    var status = $('#' + type + '_status').text();
    
    if (status !== $.i18n("up") && status !== $.i18n("uping") && status !== $.i18n("connect")) {
        he.exec(['wui@admin.ttyd_kill', 'ifname@' + type + '.setup']).then(function(result) {
            location.reload();
        });
    } else {
        he.exec(['ifname@' + type + '.shut']).then(function(result) {
            location.reload();
        });
    }
}

// WAN设备开关函数
function toggleWanInterface(type) {
    var status = $('#' + type + '_status').text();
    
    if (status === $.i18n("down")) {
        he.exec(['ifname@' + type + '.setup']).then(function(result) {
            location.reload();
        });
    } else {
        he.exec(['ifname@' + type + '.shut']).then(function(result) {
            location.reload();
        });
    }
}

// WISP设备开关函数
function toggleWispInterface(type) {
    var status = $('#' + type + '_status').text();
    
    if (status === $.i18n("down")) {
        he.exec(['ifname@' + type + '.setup']).then(function(result) {
            location.reload();
        });
    } else {
        he.exec(['ifname@' + type + '.shut']).then(function(result) {
            location.reload();
        });
    }
}

/* init */
$.i18n().load(page.lang('dashboard')).then(function() {
    
    /* init the language */
    $.i18n().locale = lang;
    $('body').i18n();
    
    /* button bind */
    bindButtonEvents();

    /* load the configure */
    interface_load();

    adjustBoxLayout();
    
    /* set the timer flush */
    page.timing({
        refresh: function() {
            interface_load();
        },
        interval: flush_interval * 1000
    });
    
});