/* flush interval */
var flush_interval = 1;
/* global buffer */
var buff = {};
/* cpu id */
var cpuid = [ "cpu", "cpu0", "cpu1", "cpu2", "cpu3" ];
/* init the cpu info */
for ( var id in cpuid )
{
    buff[ cpuid[id]+"_idle"] = 0;
    buff[ cpuid[id]+"_total"] = 0;
    buff[ cpuid[id]+"_data"] = [];
}
/* init the mem info */
buff["mem_data"] = [];

// 存储组固定颜色方案
const storageGroupColors = [
    "#FF9800",  // 橙色
    "#2196F3",  // 蓝色
    "#4CAF50",  // 绿色
    "#9C27B0"   // 紫色
];

function cpumem_show( cpuinfo, meminfo )
{
    /* CPU PIE */
    var cpu = cpuinfo["cpu"];
    var oi = buff["cpu_idle"];
    var ot = buff["cpu_total"];
    var idle = parseInt(cpu.idle);
    var total = parseInt(cpu.user)+parseInt(cpu.system)+parseInt(cpu.nice)+parseInt(cpu.iowait)+parseInt(cpu.irq)+parseInt(cpu.softirq)+parseInt(cpu.idle);
    var usage = ((total-ot-(idle-oi))/(total-ot))*100;
    $("#cpu_usage").text( usage.toFixed(0) );
    $("#cpu_usage_pie").data('easyPieChart').update( usage.toFixed(0) );
    $("#cpu_user").text( cpu.user );
    $("#cpu_system").text( cpu.system );
    $("#cpu_irq").text( cpu.irq );
    $("#cpu_softirq").text( cpu.softirq );
    

    /* Memeory PIE */
    var use = meminfo.total-meminfo.free;
    var usage = (use/meminfo.total)*100;
    $("#mem_usage").text( usage.toFixed(0) );
    $("#mem_usage_pie").data('easyPieChart').update( usage.toFixed(0) );
    $("#mem_total").text( meminfo.total+"K" );
    $("#mem_free").text( meminfo.free+"K" );
    $("#memory_total").text( meminfo.total );
    $("#memory_free").text( meminfo.free );
    $("#memory_buffer").text( meminfo.buffers );
    $("#memory_cached").text( meminfo.cached );

    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();


    /* cpu line */
    var cpu_datas =[];
    for( var i in cpuid )
    {
        var id = cpuid[i];
        var info = cpuinfo[id];
        if ( info )
        {
            var datas = {};
            datas['label'] = $.i18n(id.toUpperCase());
            var info = cpuinfo[id];
            var oidle = buff[id+"_idle"];
            var ototal = buff[id+"_total"];
            var nidle = parseInt(info.idle);
            var ntotal = parseInt(info.user)+parseInt(info.system)+parseInt(info.nice)+parseInt(info.iowait)+parseInt(info.irq)+parseInt(info.softirq)+parseInt(info.idle);
            var usage = ((ntotal-ototal-(nidle-oidle))/(ntotal-ototal))*100;
            buff[id+"_idle"] = nidle;
            buff[id+"_total"] = ntotal;
            buff[id+"_data"].push( [d_time,usage] );  
            if( id == "cpu0" )
            {
                datas['color']= "#ff0000";
            }
            else if( id == "cpu1" )
            {
                datas['color']= "#00ff00";
            }
            else if( id == "cpu2" )
            {
                datas['color']= "#0000ff";
            }
            else if( id == "cpu3" )
            {
                datas['color']= "#ffff00";
            }
            datas['data'] = buff[id+"_data"];
            cpu_datas.push(datas);
        }
    }
    $("#cpu-charts").css( {'width':'100%' , 'height':'180px'} );
    var opt =
    {
        points: { clickable:true, hoverable:true },
        lines: { show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: 100, tickFormatter:function(axis) { return axis.toString()+"%"; } },
        xaxis: { mode:"time", timeformat: "%H:%M:%S", minTickSize: [3, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "#cpu-charts", cpu_datas, opt );

    /* memory line */
    var mem_datas =[];
    var mem_usage = ( ( meminfo.total-meminfo.free)/meminfo.total ) * 100;
    buff["mem_data"].push( [ d_time, mem_usage ] );  
    var datas = {}; 
    datas['label'] = $.i18n('Memory');
    datas['color'] = "#00ffff";
    datas['data'] = buff["mem_data"];
    mem_datas.push( datas );
    $("#mem-charts").css( {'width':'100%' , 'height':'180px'} );
    var opt =
    {
        points: { clickable:true, hoverable:true },
        lines: { show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: 100, tickFormatter:function(axis) { return axis.toString()+"%"; } },
        xaxis: { mode:"time", timeformat: "%H:%M:%S", minTickSize: [3, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "#mem-charts", mem_datas, opt );
}

// 存储组显示函数
function sg_show(storageGroups) {
    const container = $('#storage-groups-content');
    
    // 避免重复刷新
    if (container.children().length > 0) {
        return;
    }

    // 清空容器
    container.empty();
    
    if (!storageGroups || Object.keys(storageGroups).length === 0) {
        $('#storage-groups-container').hide();
        return;
    }
    
    $('#storage-groups-container').show();

    const groupsArray = Object.entries(storageGroups);
    const groupCount = groupsArray.length;
    
    // 根据存储组数量确定列宽
    let colClass;
    if (groupCount === 1) {
        colClass = 'col-xs-12 col-sm-12 col-md-12 col-lg-12';
    } else if (groupCount === 2) {
        colClass = 'col-xs-12 col-sm-6 col-md-6 col-lg-6';
    } else if (groupCount === 3) {
        colClass = 'col-xs-12 col-sm-6 col-md-4 col-lg-4';
    } else {
        colClass = 'col-xs-12 col-sm-6 col-md-3 col-lg-3';
    }
    
    // 饼图大小
    let baseSize = {
        width: 120,
        height: 90,
        fontSize: 12
    };
    
    let scaleFactor = 1;
    if (groupCount === 1) scaleFactor = 1.8;
    else if (groupCount === 2) scaleFactor = 1.6;
    else if (groupCount === 3) scaleFactor = 1.4;
    else scaleFactor = 1.0;
    
    const pieSize = {
        width: Math.round(baseSize.width * scaleFactor),
        height: Math.round(baseSize.height * scaleFactor),
        fontSize: Math.round(baseSize.fontSize * scaleFactor)
    };
    
    const displayGroups = groupsArray.slice(0, 4);
    
    const rowDiv = document.createElement('div');
    rowDiv.className = 'row';

    displayGroups.forEach(([name, info], index) => {
        if (!info.use) return;
        
        const usage = parseInt(info.use);
        const color = storageGroupColors[index];
        const free = info.available || '0';
        
        const template = document.getElementById('storage-widget-template');
        const clone = template.content.cloneNode(true);
        
        const storageItem = clone.querySelector('.storage-group-item');
        const pieContainer = clone.querySelector('.storage-pie-container');
        const svgElement = clone.querySelector('.storage-pie-svg');
        const percentElement = clone.querySelector('.chart-percent');
        const availableElement = clone.querySelector('.storage-available');
        const usedElement = clone.querySelector('.storage-used');
        const sizeElement = clone.querySelector('.storage-size');
        const freeElement = clone.querySelector('.storage-free');
        
        const colDiv = document.createElement('div');
        colDiv.className = colClass;
        
        pieContainer.style.width = `${pieSize.width}px`;
        pieContainer.style.height = `${pieSize.height}px`;
        
        svgElement.setAttribute('width', pieSize.width);
        svgElement.setAttribute('height', pieSize.height);
        svgElement.setAttribute('viewBox', `0 0 ${pieSize.width} ${pieSize.height}`);
        
        while (svgElement.firstChild) {
            svgElement.removeChild(svgElement.firstChild);
        }
        
        // 绘制3D立体饼图
        draw3DPieChart(svgElement, pieSize.width, pieSize.height, usage, color);
        
        percentElement.style.fontSize = `${pieSize.fontSize}px`;
        percentElement.textContent = usage;

        availableElement.textContent = info.available || '0';
        usedElement.textContent = name;
        sizeElement.textContent = info.size || '0';
        freeElement.textContent = free;
        
        colDiv.appendChild(storageItem);

        // 悬浮提示
        svgElement.addEventListener('mouseover', function() {
            this.style.transform = 'scale(1.02)';
            
            const translatedName = $.i18n ? $.i18n(name) : name;
            
            const tooltip = document.createElement('div');
            tooltip.className = 'storage-tooltip-simple';
            tooltip.id = 'storage-tooltip-' + index;
            tooltip.innerHTML = `
                <div class="tooltip-arrow"></div>
                <div class="tooltip-text">${translatedName}: ${usage}%</div>
            `;
            
            document.body.appendChild(tooltip);
            
            const rect = this.getBoundingClientRect();
            tooltip.style.top = (rect.top - 35) + 'px';
            tooltip.style.left = (rect.left + rect.width/2 - 60) + 'px';
        });

        svgElement.addEventListener('mouseout', function() {
            this.style.transform = 'scale(1)';
            
            const tooltip = document.getElementById('storage-tooltip-' + index);
            if (tooltip) {
                tooltip.remove();
            }
        });
        
        rowDiv.appendChild(colDiv);
    });
    
    container.append(rowDiv);

    if ($.i18n) {
        container.find('[data-i18n]').i18n();
    }
}

// 绘制扁平圆柱体切面
function draw3DPieChart(svg, width, height, percent, baseColor) {
    const cx = width / 2;
    const cy = height / 2 - 8;
    const rx = width / 2;
    const ry = height / 2 - 8;
    const thickness = 18; // 圆柱体厚度
    
    // 角度
    const startAngle = -Math.PI / 2;
    
    // 厚度偏移量
    const offsetY = thickness * 0.8;
    
    if (percent === 0) {
        // 绘制底面
        const bottomEllipse = document.createElementNS('http://www.w3.org/2000/svg', 'ellipse');
        bottomEllipse.setAttribute('cx', cx);
        bottomEllipse.setAttribute('cy', cy + offsetY);
        bottomEllipse.setAttribute('rx', rx);
        bottomEllipse.setAttribute('ry', ry);
        bottomEllipse.setAttribute('fill', '#c0c0c0');
        svg.appendChild(bottomEllipse);
        
        // 绘制顶面
        const topEllipse = document.createElementNS('http://www.w3.org/2000/svg', 'ellipse');
        topEllipse.setAttribute('cx', cx);
        topEllipse.setAttribute('cy', cy);
        topEllipse.setAttribute('rx', rx);
        topEllipse.setAttribute('ry', ry);
        topEllipse.setAttribute('fill', '#f0f0f0');
        svg.appendChild(topEllipse);
        
        return;
    }
    
    const angle = (percent / 100) * 2 * Math.PI;
    const endAngle = startAngle + angle;
    
    // 计算扇形路径点
    const x1 = cx + rx * Math.cos(startAngle);
    const y1 = cy + ry * Math.sin(startAngle);
    const x2 = cx + rx * Math.cos(endAngle);
    const y2 = cy + ry * Math.sin(endAngle);
    
    const largeArc = angle > Math.PI ? 1 : 0;
    
    // 计算颜色变暗版本用于侧面
    const darkerColor = adjustColorBrightness(baseColor, -20);
    
    // 绘制底面
    const bottomSector = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    
    const bottomData = [
        `M ${cx} ${cy + offsetY}`,
        `L ${x1} ${y1 + offsetY}`,
        `A ${rx} ${ry} 0 ${largeArc} 1 ${x2} ${y2 + offsetY}`,
        'Z'
    ].join(' ');
    
    bottomSector.setAttribute('d', bottomData);
    bottomSector.setAttribute('fill', darkerColor);
    svg.appendChild(bottomSector);
    
    // 绘制侧面

    // 外弧侧面
    const sidePath = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    
    const sideData = [
        `M ${x1} ${y1}`,
        `L ${x1} ${y1 + offsetY}`,
        `L ${x2} ${y2 + offsetY}`,
        `L ${x2} ${y2}`,
        'Z'
    ].join(' ');
    
    sidePath.setAttribute('d', sideData);
    sidePath.setAttribute('fill', darkerColor);
    svg.appendChild(sidePath);
    
    if (angle < 2 * Math.PI) {
        // 起始边侧面
        const startSidePath = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const startSideData = [
            `M ${cx} ${cy}`,
            `L ${cx} ${cy + offsetY}`,
            `L ${x1} ${y1 + offsetY}`,
            `L ${x1} ${y1}`,
            'Z'
        ].join(' ');
        
        startSidePath.setAttribute('d', startSideData);
        startSidePath.setAttribute('fill', darkerColor);
        svg.appendChild(startSidePath);
        
        // 结束边侧面
        const endSidePath = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const endSideData = [
            `M ${cx} ${cy}`,
            `L ${cx} ${cy + offsetY}`,
            `L ${x2} ${y2 + offsetY}`,
            `L ${x2} ${y2}`,
            'Z'
        ].join(' ');
        
        endSidePath.setAttribute('d', endSideData);
        endSidePath.setAttribute('fill', darkerColor);
        svg.appendChild(endSidePath);
    }
    
    // 绘制顶面
    const topSector = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    
    const topData = [
        `M ${cx} ${cy}`,
        `L ${x1} ${y1}`,
        `A ${rx} ${ry} 0 ${largeArc} 1 ${x2} ${y2}`,
        'Z'
    ].join(' ');
    
    topSector.setAttribute('d', topData);
    topSector.setAttribute('fill', baseColor);
    svg.appendChild(topSector);
    
    if (percent < 100) {
        const unusedStartAngle = endAngle;
        const unusedEndAngle = startAngle + 2 * Math.PI;
        const unusedAngle = unusedEndAngle - unusedStartAngle;
        
        const x3 = cx + rx * Math.cos(unusedStartAngle);
        const y3 = cy + ry * Math.sin(unusedStartAngle);
        const x4 = cx + rx * Math.cos(unusedEndAngle);
        const y4 = cy + ry * Math.sin(unusedEndAngle);
        
        const unusedLargeArc = unusedAngle > Math.PI ? 1 : 0;
        
        // 未使用部分的底面
        const unusedBottom = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const unusedBottomData = [
            `M ${cx} ${cy + offsetY}`,
            `L ${x3} ${y3 + offsetY}`,
            `A ${rx} ${ry} 0 ${unusedLargeArc} 1 ${x4} ${y4 + offsetY}`,
            'Z'
        ].join(' ');
        
        unusedBottom.setAttribute('d', unusedBottomData);
        unusedBottom.setAttribute('fill', '#c0c0c0');
        svg.appendChild(unusedBottom);
        
        // 未使用部分的侧面
        const unusedSide = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const unusedSideData = [
            `M ${x3} ${y3}`,
            `L ${x3} ${y3 + offsetY}`,
            `L ${x4} ${y4 + offsetY}`,
            `L ${x4} ${y4}`,
            'Z'
        ].join(' ');
        
        unusedSide.setAttribute('d', unusedSideData);
        unusedSide.setAttribute('fill', baseColor);
        svg.appendChild(unusedSide);
        
        // 未使用部分的顶面
        const unusedTop = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const unusedTopData = [
            `M ${cx} ${cy}`,
            `L ${x3} ${y3}`,
            `A ${rx} ${ry} 0 ${unusedLargeArc} 1 ${x4} ${y4}`,
            'Z'
        ].join(' ');
        
        unusedTop.setAttribute('d', unusedTopData);
        unusedTop.setAttribute('fill', '#f0f0f0');
        svg.appendChild(unusedTop);
    }
}

// 调整颜色亮度
function adjustColorBrightness(hex, percent) {
    if (hex.startsWith('#')) {
        let R = parseInt(hex.substring(1, 3), 16);
        let G = parseInt(hex.substring(3, 5), 16);
        let B = parseInt(hex.substring(5, 7), 16);
        
        R = Math.min(255, Math.max(0, R + percent));
        G = Math.min(255, Math.max(0, G + percent));
        B = Math.min(255, Math.max(0, B + percent));
        
        return `#${((1 << 24) + (R << 16) + (G << 8) + B).toString(16).slice(1)}`;
    }
    return hex;
}


/* load the configure on the input */
function utilization_reload() {
    he.bkload(['land@machine.cpuinfo', 'land@machine.meminfo', 'land@machine.sginfo']).then(function(v) {
        cpumem_show(v[0], v[1]);
        sg_show(v[2]);
    });
}

/* init */
$.i18n().load(page.lang('dashboard')).then(function() {
    /* init the language */
    $.i18n().locale = lang;
    $('body').i18n();

    /* init the easy-pie-chart for main charts */
    $('.easy-pie-chart.percentage').each(function() {
        if (!$(this).hasClass('initialized')) {
            $(this).addClass('initialized').easyPieChart({
                barColor: function(percent) {
                    percent /= 100;
                    return "rgb(" + Math.round(255 * percent) + ", " + Math.round(255 * (1-percent)) + ", 0)";
                },
                trackColor: '#EEEEEE',
                scaleColor: false,
                lineCap: 'butt',
                lineWidth: 8,
                animate: ace.vars['old_ie'] ? false : 1000,
                size: 75
            }).css('color', $(this).data('color'));
        }
    });

    /* load the configure */
    utilization_reload();
    
    /* set the timer flush */
    page.timing({
        refresh: function() {
            utilization_reload();
        },
        interval: flush_interval * 1000
    });
});