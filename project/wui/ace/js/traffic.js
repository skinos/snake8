const FLUSH_INTERVAL = 1; // 刷新间隔
var currentFrameData = null;

// 全局缓冲区
const buff = {};

// 网络接口配置
const interfaceConfigs = {
    'ifname@lte': { type: 'lte', max: 15, height: 150 },
    'ifname@lte2': { type: 'lte', max: 15, height: 150 },
    'ifname@lte3': { type: 'lte', max: 15, height: 150 },
    'ifname@lte4': { type: 'lte', max: 15, height: 150 },
    'ifname@wan': { type: 'wan', max: 15, height: 150 },
    'ifname@wan2': { type: 'wan', max: 15, height: 150 },
    'ifname@wan3': { type: 'wan', max: 15, height: 150 },
    'ifname@wan4': { type: 'wan', max: 15, height: 150 },
    'ifname@wisp': { type: 'wisp', max: 15, height: 150 },
    'ifname@wisp2': { type: 'wisp', max: 15, height: 150 },
    'ifname@lan': { type: 'lan', max: 15, height: 100 },
    'ifname@lan2': { type: 'lan', max: 15, height: 100 },
    'ifname@lan3': { type: 'lan', max: 15, height: 100 },
    'ifname@lan4': { type: 'lan', max: 15, height: 100 },
    'wifi@nssid': { type: 'wifi', max: 15, height: 100 },
    'wifi@assid': { type: 'wifi', max: 15, height: 100 }
};

// 处理extern中的图表接口
const externChartIds = ['ifname@lte', 'ifname@lte2', 'ifname@lte3', 'ifname@lte4','ifname@wisp', 'ifname@wisp2',
                        'ifname@wan', 'ifname@wan2', 'ifname@wan3', 'ifname@wan4'];

const localChartIds = ['ifname@lan','ifname@lan2','ifname@lan3','ifname@lan4','wifi@nssid','wifi@assid'];

// 初始化缓冲区
function initBuffers() {
    Object.keys(interfaceConfigs).forEach(ifname => {
        buff[`${ifname}_max`] = interfaceConfigs[ifname].max;
        buff[`${ifname}_rx`] = 0;
        buff[`${ifname}_tx`] = 0;
        buff[`${ifname}_rxdata`] = [];
        buff[`${ifname}_txdata`] = [];
    });
}

function fetchNetworkFrame() {
    // 负责加载数据并存入全局变量 加载完后触发一次 adjustBoxLayout
    return he.bkload([ "network@frame"]).then(function(v) {
        if (v[0]) {
            currentFrameData = v[0];
            chartManager.updateLayouts(); // 数据拿到后，重新刷一遍布局
        }
    });
}

// 初始化缓冲区
initBuffers();

// 辅助函数：转义CSS选择器中的特殊字符
function escapeSelector(selector) {
    return selector.replace(/[!"#$%&'()*+,.\/:;<=>?@[\\\]^`{|}~]/g, '\\$&');
}

// 图表管理器
const chartManager = {
    // 图表状态缓存
    chartStates: {},
    
    // 当前显示的布局
    currentLayouts: {
        extern: 0,
        local: 0
    },
    
    // 初始化图表管理器
    init: function() {
        Object.keys(interfaceConfigs).forEach(ifname => {
            this.chartStates[ifname] = {
                visible: false,
                isExtern: externChartIds.includes(ifname),
                data: {
                    rx_bytes: 0,
                    tx_bytes: 0,
                    rx_data: [],
                    tx_data: []
                }
            };
        });
        
        // 初始隐藏所有布局框架
        this.hideAllLayouts();
    },
    
    // 隐藏所有布局框架
    hideAllLayouts: function() {
        // 隐藏所有extern布局框架
        for (let i = 1; i <= 10; i++) {
            const layout = document.getElementById(`extern-layout-chart-${i}`);
            if (layout) {
                layout.style.display = 'none';
            }
        }
        
        // 隐藏所有local布局框架
        for (let i = 1; i <= 6; i++) {
            const layout = document.getElementById(`local-layout-chart-${i}`);
            if (layout) {
                layout.style.display = 'none';
            }
        }
        
        // 隐藏分割线
        const hr = document.querySelector('.hr.hr32.hr-dotted');
        if (hr) {
            hr.style.display = 'none';
        }
    },
    
    // 更新接口图表数据
    updateInterfaceChart: function(ifname, value) {
        const config = interfaceConfigs[ifname];
        if (!config) return false;
        
        // 检查接口状态
        if (!value || value.status === "down" || value.status === "nodevice" || value.status === "register") {
            this.hideChart(ifname);
            return false;
        }
        
        // 处理数据
        this.processChartData(ifname, value);

        // 获取当前图表是否可见的状态
        const wasVisible = this.chartStates[ifname].visible;

        if( value.status === "up"){
            // 显示图表
            this.showChart(ifname);
        }
        // 如果图表之前已经可见，直接绘制
        if (wasVisible) {
            this.drawChart(ifname);
        }
        //调用drawAllVisibleCharts函数 绘制所有可见图表
        return true;
    },
    
    // 显示图表
    showChart: function(ifname) {
        // 更新图表状态
        this.chartStates[ifname].visible = true;
        this.updateChartDisplay(ifname, true);

        // 更新布局
        this.updateLayouts();
    },
    
    // 隐藏图表
    hideChart: function(ifname) {
        // 更新图表状态
        this.chartStates[ifname].visible = false;
        this.updateChartDisplay(ifname, false);

        // 更新布局
        this.updateLayouts();
    },
    
    // 更新图表可见性
    updateChartDisplay: function(ifname, show) {
        const display = show ? 'block' : 'none';
        const escapedIfname = escapeSelector(ifname);
        const chartSelector = `#${escapedIfname}-charts`;
        
        document.querySelectorAll(chartSelector).forEach(el => el.style.display = display);
    },
    
    // 更新布局
    updateLayouts: function() {
        const pool = document.getElementById('widget-pool');
        if (!pool) return;

        // 统一将所有可能显示的图表先撤回到 pool 中隐藏
        const allIfnames = Object.keys(this.chartStates).filter(id => this.chartStates[id].visible);
        allIfnames.forEach(ifname => {
            const widgetRow = document.querySelector(`#widget-${escapeSelector(ifname)}-row`);
            if (widgetRow) {
                widgetRow.style.display = 'none';
                if (widgetRow.parentElement !== pool) {
                    pool.appendChild(widgetRow);
                }
            }
        });

        // 处理 Extern
        const sortedExternIfnames = this.getSortedExternIfnames();
        const externCount = sortedExternIfnames.length;
        this.updateExternLayout(externCount); // 显示 extern-layout-chart-N

        if (externCount > 0) {
            const layoutId = `extern-layout-chart-${Math.min(externCount, 10)}`;
            const layoutEl = document.getElementById(layoutId);
            if (layoutEl) {
                const slots = layoutEl.querySelectorAll('.chart-slot');
                sortedExternIfnames.forEach((ifname, index) => {
                    const widgetRow = document.querySelector(`#widget-${escapeSelector(ifname)}-row`);
                    if (widgetRow && slots[index]) {
                        slots[index].appendChild(widgetRow);
                        widgetRow.style.display = 'block';
                    }
                });
            }
        }

        // 处理 Local
        const sortedLocalIfnames = this.getSortedLocalIfnames();
        const localCount = sortedLocalIfnames.length;
        this.updateLocalLayout(localCount); // 显示 local-layout-chart-N

        if (localCount > 0) {
            const layoutId = `local-layout-chart-${Math.min(localCount, 6)}`;
            const layoutEl = document.getElementById(layoutId);
            if (layoutEl) {
                const slots = layoutEl.querySelectorAll('.chart-slot');
                sortedLocalIfnames.forEach((ifname, index) => {
                    const widgetRow = document.querySelector(`#widget-${escapeSelector(ifname)}-row`);
                    if (widgetRow && slots[index]) {
                        slots[index].appendChild(widgetRow);
                        widgetRow.style.display = 'block';
                    }
                });
            }
        }

        this.updateHrVisibility(externCount,localCount);
       
        // 如果布局结构发生变化，执行重绘
        if (this.prevExternLayout !== this.currentLayouts.extern || 
            this.prevLocalLayout !== this.currentLayouts.local) {
            
            this.prevExternLayout = this.currentLayouts.extern;
            this.prevLocalLayout = this.currentLayouts.local;
            this.drawAllVisibleCharts();
        }
    },

    getSortedExternIfnames: function() {
        const sorted = Object.keys(this.chartStates).filter(ifname => 
            this.chartStates[ifname].visible && this.chartStates[ifname].isExtern === true
        );

        // 排列规则1
        var priorityMap = {};
        if (currentFrameData) {
            for (var i = 1; i <= 6; i++) {
                var ifname = currentFrameData[i.toString()]; 
                if (ifname && ifname !== "") {
                    priorityMap[ifname] = i; 
                    // 拿取图表id
                    var shortName = ifname.replace("ifname@", "");
                    if (!priorityMap[shortName]) {
                        priorityMap[shortName] = i;
                    }
                }
            }
        }

        sorted.sort((a, b) => {
            var weightA = priorityMap[a] || 999;
            var weightB = priorityMap[b] || 999;
            if (weightA !== weightB) return weightA - weightB;
            return externChartIds.indexOf(a) - externChartIds.indexOf(b);
        });

        return sorted;
    },

    getSortedLocalIfnames: function() {
        const sorted = Object.keys(this.chartStates).filter(ifname => 
            this.chartStates[ifname].visible && this.chartStates[ifname].isExtern === false
        );

        // 内网一般按默认定义顺序排序lan lan2 lan3 lan4 wifi
        sorted.sort((a, b) => {
            return localChartIds.indexOf(a) - localChartIds.indexOf(b);
        });

        return sorted;
    },

    // 通用布局更新函数
    updateLayout: function(type, count, max) {
        const prefix = type === 'extern' ? 'extern' : 'local';
        
        // 隐藏所有该类型的布局
        for (let i = 1; i <= max; i++) {
            const layout = document.getElementById(`${prefix}-layout-chart-${i}`);
            if (layout) layout.style.display = 'none';
        }
        
        if (count === 0) {
            this.currentLayouts[type] = 0;
            return;
        }
        
        const layoutNumber = Math.min(count, max);
        const targetLayout = document.getElementById(`${prefix}-layout-chart-${layoutNumber}`);
        if (targetLayout) {
            targetLayout.style.display = 'block';
            this.currentLayouts[type] = layoutNumber;
        }
    },
    
    updateExternLayout: function(count) {
        this.updateLayout('extern', count, 10);
    },
    
    updateLocalLayout: function(count) {
        this.updateLayout('local', count, 6);
    },
    
    //绘制所有可见图表
    drawAllVisibleCharts: function() {
        Object.keys(this.chartStates).forEach(ifname => {
            if (this.chartStates[ifname].visible) {
                this.drawChart(ifname);
            }
        });
    },
    
    // 更新分割线可见性
    updateHrVisibility: function(externCount,localCount) {
        const hr = document.querySelector('.hr.hr32.hr-dotted');
        if (!hr) return;
      
        // 只有当有extern图表且同时有local图表时才显示分割线
        hr.style.display = (externCount > 0 && localCount > 0) ? 'block' : 'none';
    },
    
    // 处理图表数据
    processChartData: function(ifname, value) {
        // 获取时间戳
        const date = new Date();
        const day = date.getHours();
        const d_time = new Date(date.setHours(8 + day)).getTime();
        
        // 计算数据传输速率
        let rx = 0;
        let tx = 0;
        const orx = buff[`${ifname}_rx`];
        const otx = buff[`${ifname}_tx`];
        
        buff[`${ifname}_rx`] = parseInt(value.rx_bytes || 0);
        buff[`${ifname}_tx`] = parseInt(value.tx_bytes || 0);
        
        if (orx !== 0 || otx !== 0) {
            rx = buff[`${ifname}_rx`] - orx;
            tx = buff[`${ifname}_tx`] - otx;
            
            if (rx < 0) rx = 0;
            if (tx < 0) tx = 0;
        }
        
        // 转换为KB
        if (rx > 0) rx = Math.round(rx / 1024);
        if (tx > 0) tx = Math.round(tx / 1024);
        
        // 更新最大值
        if (buff[`${ifname}_max`] < rx) buff[`${ifname}_max`] = rx;
        if (buff[`${ifname}_max`] < tx) buff[`${ifname}_max`] = tx;
        
        // 添加数据点
        buff[`${ifname}_txdata`].push([d_time, tx]);
        buff[`${ifname}_rxdata`].push([d_time, rx]);
        
        // 保持数据长度
        if (buff[`${ifname}_txdata`].length > 60) buff[`${ifname}_txdata`].shift();
        if (buff[`${ifname}_rxdata`].length > 60) buff[`${ifname}_rxdata`].shift();
    },
    
    // 绘制图表
    drawChart: function(ifname) {
        const isExtern = externChartIds.includes(ifname);
        const layoutType = isExtern ? 'extern' : 'local';
        const layoutNumber = this.currentLayouts[layoutType];
        
        if (layoutNumber === 0) return;
        
        const layoutId = `${layoutType}-layout-chart-${layoutNumber}`;
        const layout = document.getElementById(layoutId);
        if (!layout) return;
        
        // 查找图表容器
        let chartElement = layout.querySelector(`#${escapeSelector(ifname)}-charts`) || 
                          document.getElementById(`${ifname}-charts`);
        
        if (!chartElement) return;
        if (chartElement.style.display === 'none') {
            chartElement.style.display = 'block';
        }
        
        const datas = [
            { 
                label: $.i18n('TX byte'), 
                color: "#0000ff", 
                data: buff[`${ifname}_txdata`]
            },
            { 
                label: $.i18n('RX byte'), 
                color: "#00ff00", 
                data: buff[`${ifname}_rxdata`]
            },
        ];
        
        // 图表选项
        const opt = {
            points: { clickable: true, hoverable: true },
            lines: { show: true, lineWidth: 1 },
            selection: { mode: "x" },
            yaxis: { 
                max: buff[`${ifname}_max`], 
                tickFormatter: function(axis) { 
                    return axis.toFixed(0) + "K"; 
                } 
            },
            xaxis: { 
                mode: "time", 
                timeformat: "%M:%S", 
                minTickSize: [1, "second"] 
            },
            legend: { 
                position: "ne", 
                backgroundColor: "#fff" 
            }
        };
        
        // 绘制图表
        $.plot(chartElement, datas, opt);
    }
};

// 终端列表管理器
var timer;
var nstalist;
var astalist;
var clientlist;
var isDataLoading = false; 

var object = "client@station";
var clients_table = '#clients-grid-table';
var clients_pager = '#clients-grid-pager';

/* load the table infomation */
    function load_clients()
    {
        he.bkload( [ object+'.list', "wifi@n.stalist", "wifi@a.stalist" ] ).then( function(v){
            clientlist = v[0];
            if ( !clientlist )
            {
                clientlist = {};
            }
            nstalist = v[1];
            if ( !nstalist )
            {
                nstalist = {};
            }
            astalist = v[2];
            if ( !astalist )
            {
                astalist = {};
            }
            $('#traffic_html').show();
            
            // 保存所有的客户端行
            var count = 0;
            var rows = [];
            for ( var index in clientlist )
            {
                var client = clientlist[index];
                if ( !client )
                {
                    continue;
                }
                if ( nstalist[index] )
                {
                    var x = nstalist[index];
                    if ( x.ifdev )
                    {
                        client.ifdev = x.ifdev;
                        client.rssi = x.rssi;
                    }
                    nstalist[index] = null;
                }
                else if ( astalist[index] )
                {
                    var x = astalist[index];
                    if ( x.ifdev )
                    {
                        client.ifdev = x.ifdev;
                        client.rssi = x.rssi;
                    }
                    astalist[index] = null;
                }
                var row = {};
                row[ 'mac'] =  index;
                row[ 'ip'] = client.ip || '';
                row[ 'name'] = client.name || '';
                if ( !client.rx_bytes && !client.tx_bytes )
                {
                    row[ 'rxtx' ] = '';
                }
                else
                {
                    if ( !client.rx_bytes )
                    {
                        client.rx_bytes = "0";
                    }
                    if ( !client.tx_bytes )
                    {
                        client.tx_bytes = "0";
                    }
                    row[ 'rxtx' ] = byte2readable( client.rx_bytes ) + " / " + byte2readable(client.tx_bytes);
                }

                if ( client.ifname )
                {
                    row[ 'ifname'] =  $.i18n(client.ifname);
                }
                if ( client.ifdev )
                {
                    row[ 'ifdev'] =  $.i18n(client.ifdev);
                }
                if ( client.livetime )
                {
                    row[ 'livetime'] = time2string(client.livetime);
                    count++;
                    rows.unshift( row );
                }
                else
                {
                    row[ 'livetime'] = $.i18n('Leave');
                    rows.push( row );
                }
            }
            for ( var index in nstalist )
            {
                var client = nstalist[index];
                if ( client == null )
                {
                    continue;
                }
                var row = {};
                row[ 'mac'] = index;
                if ( client.ifdev )
                {
                    row[ 'ifdev'] =  $.i18n(client.ifdev);
                }
                if ( client.livetime )
                {
                    row[ 'livetime'] = time2string(client.livetime);
                    count++;
                    rows.unshift( row );
                }
                else
                {
                    row[ 'livetime'] = $.i18n('Leave');
                    rows.push( row );
                }
            }
            for ( var index in astalist )
            {
                var client = astalist[index];
                if ( client == null )
                {
                    continue;
                }
                var row = {};
                row[ 'mac'] =  index;
                if ( client.ifdev )
                {
                    row[ 'ifdev'] =  $.i18n(client.ifdev);
                }
                if ( client.livetime )
                {
                    row[ 'livetime'] = time2string(client.livetime);
                    count++;
                    rows.unshift( row );
                }
                else
                {
                    row[ 'livetime'] = $.i18n('Leave');
                    rows.push( row );
                }
            }
            // 表头设置为在线客户端的数量
            $(clients_table).jqGrid( "setCaption", '<i class="ace-icon fa fa-users"></i> ' + $.i18n('Clients Table') + ' (' + count + ')' );
            // 记住滚动条的位置
            var scrollPos = jqtable.getScrollPos();
            // 获取当前分页状态
            var currentPage = $(clients_table).jqGrid('getGridParam', 'page');
            var currentRowNum = $(clients_table).jqGrid('getGridParam', 'rowNum');
            
            // 给clients表格设置数据
            $(clients_table).jqGrid('setGridParam', {
                data: rows,
                page: currentPage,
                rowNum: currentRowNum
            }).trigger('reloadGrid');
            // 恢复滚动条的位置
            jqtable.setScrollPos(scrollPos);
            // 改变离线行的颜色
            $('td[title=' + $.i18n('Leave') + ']').closest('tr').css({color: '#888'});
        });
    }

    function controlRefresh() {
        var $grid = $(clients_table);
        
        var selectedIds = $grid.jqGrid('getGridParam', 'selarrrow');
        if (selectedIds.length > 0) return true;
        
        var currentPage = $grid.getGridParam('page');
        if (currentPage !== 1) return true;
        
        var activeEl = document.activeElement;
        if (activeEl && $(activeEl).hasClass('ui-pg-input')) {
            return true;
        }
        if (isDataLoading) return true;

        return false; // 不忙，可以刷新
    }

    function startRefreshTimer() {
        if (timer) clearInterval(timer);

        timer = setInterval(function() {
            var $grid = $(clients_table);

            if ($grid.length === 0) {
                //console.log("检测到表格已不在当前页面，正在自动销毁定时器...");
                stopRefreshTimer();
                return; 
            }

            if (!controlRefresh()) {
                //console.log('条件满足，执行加载...');
                // 执行加载前加锁
                isDataLoading = true;
                load_clients(); 
            } else {
                // console.log('跳过刷新，原因:', busyReason);
            }
        }, 3000);
    }

    function stopRefreshTimer() {
        if (timer) {
            clearInterval(timer);
            timer = null;
        }
    }

     // 表单自定义逻辑
    var customForm = {
        afterShowForm: function(form) {
            // 添加自定义样式和提示
            $("label[for='name']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
            $("label[for='mac']", form).append('<span style="color: red; margin-left: 3px;">*</span>');

            var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
                '<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
                '</div>';
            
            $("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
            
            // 设置 placeholder
            $("#name", form).attr("placeholder", $.i18n('Enter Hostname'));
            $("#mac", form).attr("placeholder", $.i18n('Enter MAC Address'));
        }
    };

    function init_table(){
        jqtable.create(  clients_table, clients_pager,
        {
            caption: ' ', // 必需设置值, 防止表格不能折叠
            toolbar: [true, "top"], 
            colNames: [ $.i18n('Hostname'), $.i18n('MAC Address'), $.i18n('IP Address'), $.i18n('Live Time'), $.i18n('Rx/Tx'), $.i18n('Interface'), $.i18n('Interface Device')],
            colModel: [
                { name:'name', width:200, editable:true, editrules:{ required:true } },
                { name:'mac', width:150, editable:true, editrules:{ required:true } },
                { name:'ip', width:130 },
                { name:'livetime', width:90 },
                { name:'rxtx', width:150 },
                { name:'ifname', width:100 },
                { name:'ifdev', width:100 },
               
            ],
            pager: '#clients-grid-pager',
            rowNum: 10,
            viewrecords: true,
            multiselect:false,

            pgbuttons: true,
            pagerpos:'center',
            pginput:true,

            autowidth:true,
            loadonce:true,
            shrinkToFit:true,
            responsive:true,
            forceFit: true,
            
            // 当用户进行分页操作时触发
            onPaging: function(pgButton) {
                //console.log('用户操作分页，按钮:', pgButton);

                // 如果用户输入页码
                if (pgButton === 'user') {
                    // 获取输入的页码
                    var inputPage = null;
                    var pagerInput = $(this).closest('.ui-jqgrid')
                                .find('.ui-pg-table .ui-pg-input');
        
                    if (pagerInput.length > 0) {
                        inputPage = parseInt(pagerInput.val(), 10);
                        //console.log('获取到输入页码:', inputPage);
                    }
                        // 如果输入的是第一页，设置标记
                        if (inputPage === 1) {
                            //console.log('用户输入页码1 重启定时器');
                            startRefreshTimer();
                        }
                    }
                    // 如果是点击第一页按钮
                    else if (pgButton === 'first') {
                        //console.log('点击第一页按钮 重启定时器');
                        startRefreshTimer();
                    }
                    // 如果是点击上一页按钮且当前在第二页
                    else if (pgButton === 'prev') {
                        var currentPage = $(this).getGridParam('page');
                        if (currentPage === 2) {
                            //console.log('从第二页点击上一页 重启定时器');
                            startRefreshTimer();
                        }
                    }
                    return true;
                },

            gridComplete: function() {
                isDataLoading = false; 
                startRefreshTimer();
            
                var $pagerInput = $(this).closest('.ui-jqgrid').find('.ui-pg-table .ui-pg-input');
                $pagerInput.off('blur').on('blur', function() {
                    // 失去焦点时不立即刷新，而是等下一秒的定时器自己去判定
                });
            },
        }
      )
        var $toolbar = $("#t_" + clients_table.replace('#', ''));
        $toolbar.append($('#grid-controls').children());
        $toolbar.css({
            'display': 'flex',
            'justify-content': 'space-between', // 撑开两端
            'align-items': 'center',            // 垂直居中
            'background': '#f5f5f5',
            'padding': '8px 10px',
            'height': 'auto',                   // 覆盖默认高度
            //'border-bottom': '1px solid #e1e1e1' // 加个分割线
        });

        $('#rowNums').on('change',function(){
                var newRowNum = parseInt($(this).val(),10);
                $(clients_table).jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
        });
    }

// 主数据加载函数
function dashboard_reload() {
    he.bkload([
        'network@frame.extern', 
        'network@frame.local', 
        "wifi@nssid.status", 
        "wifi@assid.status"
    ]).then(function(v) {
        // 初始化图表管理器
        if (!chartManager.chartStates['ifname@lte']) {
            chartManager.init();
        }
        
        // 处理外部网络接口
        if (v[0]) {
            for (const id in v[0]) {
                const value = v[0][id];
                chartManager.updateInterfaceChart(id, value);
            }
        }
        
        // 处理内部网络接口
        if (v[1]) {
            for (const id in v[1]) {
                const value = v[1][id];
                chartManager.updateInterfaceChart(id, value);
            }
        }
        
        // 处理WiFi接口
        if (v[2]) {
            chartManager.updateInterfaceChart('wifi@nssid', v[2]);
        }
        
        if (v[3]) {
            chartManager.updateInterfaceChart('wifi@assid', v[3]);
        }
    });
}

// 初始化函数
function initDashboard() {
    $.i18n().load(page.lang('traffic')).then(function() {
        // 设置语言
        $.i18n().locale = lang;
        $('body').i18n();
        
        // 初始化客户端管理器
        init_table();

        load_clients();

        // 初始启动客户端定时器
        startRefreshTimer();

        // 加载初始数据 折线图
        dashboard_reload();
        
        // 设置定时刷新 折线图
        page.timing({
            refresh: function() {
                dashboard_reload();
            },
            interval: FLUSH_INTERVAL * 1000
        });
        
    });
}

// 页面加载完成后初始化
$(document).ready(function() {
    initDashboard();
    fetchNetworkFrame();
});