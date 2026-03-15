const FLUSH_INTERVAL = 1; // 刷新间隔
const CLIENT_REFRESH_INTERVAL = 3000; // 客户端刷新间隔

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
const externChartIds = ['ifname@lte', 'ifname@lte2', 'ifname@lte3', 'ifname@lte4', 'ifname@wan', 'ifname@wan2', 
                       'ifname@wan3', 'ifname@wan4', 'ifname@wisp', 'ifname@wisp2'];

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
        if (!value || value.status === "down" || value.status === "nodevice") {
            this.hideChart(ifname);
            return false;
        }
        
        // 处理数据
        this.processChartData(ifname, value);

        // 获取当前图表是否可见的状态
        const wasVisible = this.chartStates[ifname].visible;

        // 显示图表
        this.showChart(ifname);

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
        const rowSelector = `#widget-${escapedIfname}-row`;
        const chartSelector = `#${escapedIfname}-charts`;
        
        // 合并查询和设置逻辑
        const setDisplay = selector => {
            document.querySelectorAll(selector).forEach(el => el.style.display = display);
        };
        
        setDisplay(rowSelector);
        setDisplay(chartSelector);
        
        // 如果是显示，确保父容器也可见
        if (show) {
            document.querySelectorAll(rowSelector).forEach(el => {
                let parent = el.parentElement;
                while (parent) {
                    if (parent.style && parent.style.display === 'none') {
                        parent.style.display = 'block';
                    }
                    parent = parent.parentElement;
                }
            });
        }
    },
    
    // 更新布局（extern和local同时处理）
    updateLayouts: function() {
       
        // 处理extern布局
        const externCount = this.getOnlineChartCount(true);
        const prevExternLayout = this.currentLayouts.extern;
        this.updateExternLayout(externCount);
        
        // 处理local布局
        const localCount = this.getOnlineChartCount(false);
        const prevLocalLayout = this.currentLayouts.local;
        this.updateLocalLayout(localCount);
        
        // 更新分割线显示状态
        this.updateHrVisibility();
        
        // 应用特殊布局调整（针对extern布局）
        this.adjustExternLayouts();

         // 应用特殊布局调整（针对local布局）
        this.adjustLocalLayouts();

        if (prevExternLayout !== this.currentLayouts.extern || 
        prevLocalLayout !== this.currentLayouts.local) {
            this.drawAllVisibleCharts();
        
    }
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
    
    // 调整extern布局
    adjustExternLayouts: function() {
        const layoutNumber = this.currentLayouts.extern;
        const layoutRules = {
            5: { layoutId: 'extern-layout-chart-5', start: 3, count: 2, class: 'col-sm-6' },   // 第4-5个图表对半
            7: { layoutId: 'extern-layout-chart-7', start: 6, count: 1, class: 'col-sm-12' },  // 第7个图表占满
            8: { layoutId: 'extern-layout-chart-8', start: 6, count: 2, class: 'col-sm-6' },   // 第7-8个图表对半
            10: { layoutId: 'extern-layout-chart-10', start: 9, count: 1, class: 'col-sm-12' },   // 第10个图表占满
        };
        
        if (layoutRules[layoutNumber]) {
            const rule = layoutRules[layoutNumber];
            // 创建规则对象，key是图表数量
            const rules = {};
            rules[layoutNumber] = {
                start: rule.start,
                count: rule.count,
                class: rule.class
            };
            this.adjustSpecificLayout(rule.layoutId, rules);
        }
    },

    adjustLocalLayouts: function() {
        const layoutNumber = this.currentLayouts.local;
        const layoutRules = {
            5: { layoutId: 'local-layout-chart-5', start: 3, count: 2, class: 'col-sm-6' },   // 第4-5个图表对半
        };
        
        if (layoutRules[layoutNumber]) {
            const rule = layoutRules[layoutNumber];
            // 创建规则对象，key是图表数量
            const rules = {};
            rules[layoutNumber] = {
                start: rule.start,
                count: rule.count,
                class: rule.class
            };
            this.adjustSpecificLayout(rule.layoutId, rules);
        }
    },

    // 调整特定布局 
    adjustSpecificLayout: function(layoutId, layoutRules) {
        const layout = document.getElementById(layoutId);
        if (!layout || layout.style.display === 'none') return;
        
        const allCols = Array.from(layout.querySelectorAll('.col-sm-4')).filter(col => {
            // 检查列本身的显示状态
            const colDisplay = window.getComputedStyle(col).display;
            const widget = col.querySelector('.widget-box');
            return colDisplay !== 'none' && widget;
        });
        
        const visibleCount = allCols.length;
      
        if (visibleCount === 0) return;
        
        allCols.forEach((col, index) => {
            col.className = 'col-sm-6';
        });
        
        // 获取对应图表数量的规则
        const rule = layoutRules[visibleCount];
        if (rule) {
            for (let i = rule.start; i < rule.start + rule.count && i < visibleCount; i++) {
                if (allCols[i]) {
                    allCols[i].className = rule.class;
                }
            }
        }
    },
    
    // 更新分割线可见性
    updateHrVisibility: function() {
        const hr = document.querySelector('.hr.hr32.hr-dotted');
        if (!hr) return;
        
        const externCount = this.getOnlineChartCount(true);
        const localCount = this.getOnlineChartCount(false);
      
        // 只有当有extern图表且同时有local图表时才显示分割线
        hr.style.display = (externCount > 0 && localCount > 0) ? 'block' : 'none';
    },
    
    // 计算在线图表数量
    getOnlineChartCount: function(isExtern) {
        const count = Object.values(this.chartStates).filter(state => 
            state.visible && state.isExtern === isExtern
        ).length;
        return count;
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


// 工具函数 字节转换为可读格式
function byte2readable(bytes) {
    bytes = parseInt(bytes);
    if (bytes === 0) return "0B";
    
    const units = ['B', 'KB', 'MB', 'GB', 'TB'];
    let i = 0;
    
    while (bytes >= 1024 && i < units.length - 1) {
        bytes /= 1024;
        i++;
    }
    
    return bytes.toFixed(2) + units[i];
}

// 工具函数 秒数转换为时间字符串
function time2string(seconds) {
    seconds = parseInt(seconds);
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;
    
    if (days > 0) {
        return days + "d " + hours + "h";
    } else if (hours > 0) {
        return hours + "h " + minutes + "m";
    } else if (minutes > 0) {
        return minutes + "m " + secs + "s";
    } else {
        return secs + "s";
    }
}

// 终端列表管理器
const clientManager = {
    // 客户端表格相关变量
    clients_table: '#dashboard-clients-grid-table',
    clients_pager: '#dashboard-clients-grid-pager',
    timer: null,
    nstalist: null,
    astalist: null,
    clientlist: null,
    object: 'center@heport',
    inputFocused:false,

    // 定时器控制相关变量
    refresh_interval: CLIENT_REFRESH_INTERVAL,
    
    // 初始化客户端表格
    init: function() {
        const self = this;
        
        $.i18n().load(page.lang('dashboard')).then(function() {
            // 获取翻译文本
            var connectedClientsText = $.i18n('Connected Clients');
            
            // 创建客户端表格
            jqtable.create(self.clients_table, self.clients_pager, {
                caption: '<i class="ace-icon fa fa-users"></i> ' + connectedClientsText + ' <span id="online-clients-count" style="font-size: 14px; margin-left: 5px;"></span>',
                toolbar: [true, "top"], 
                colNames: [
                    $.i18n('Hostname'), 
                    $.i18n('MAC Address'), 
                    $.i18n('IP Address'), 
                    $.i18n('Live Time'), 
                    $.i18n('Rx/Tx'), 
                    $.i18n('Interface'), 
                    $.i18n('Interface Device')
                ],
                colModel: [
                    { name: 'name', width: 200 },
                    { name: 'mac', width: 150 },
                    { name: 'ip', width: 130 },
                    { name: 'livetime', width: 90 },
                    { name: 'rxtx', width: 150 },
                    { name: 'ifname', width: 100 },
                    { name: 'ifdev', width: 100 }
                ],
                pager: '#dashboard-clients-grid-pager',
                rowNum: 10,
                viewrecords: true,

                pgbuttons: true,
                pagerpos:'center',
                pginput:true,

                autowidth: true,
                loadonce: true,
                shrinkToFit: true,
                responsive: true,
                
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
                                self.startRefreshTimer();
                            }
                        }
                        // 如果是点击第一页按钮
                        else if (pgButton === 'first') {
                            //console.log('点击第一页按钮 重启定时器');
                            self.startRefreshTimer();
                        }
                        // 如果是点击上一页按钮且当前在第二页
                        else if (pgButton === 'prev') {
                            var currentPage = $(this).getGridParam('page');
                            if (currentPage === 2) {
                                //console.log('从第二页点击上一页 重启定时器');
                                self.startRefreshTimer();
                            }
                        }
                        
                        return true;
                    },
                
                gridComplete: function() {
                    // 绑定分页行数选择器事件
                    $('#rowNums').off('change.grid').on('change.grid', function() {
                        //console.log('用户修改行数');

                        var newRowNum = parseInt($(this).val(), 10);
                        if (!isNaN(newRowNum)) {
                            $(self.clients_table).jqGrid('setGridParam', {
                                rowNum: newRowNum
                            }).trigger('reloadGrid');
                        }
                    });

                    // 绑定分页输入框事件
                    var pagerInput = $(this).closest('.ui-jqgrid')
                                           .find('.ui-pg-table .ui-pg-input');

                    if (pagerInput.length > 0) {
                        // 焦点获得时暂停定时器
                        pagerInput.on('focus', function() {
                            //console.log('分页输入框获得焦点，暂停定时器');
                            self.stopRefreshTimer();
                            self.inputFocused = true;
                        });
                        
                        // 焦点失去时处理
                        pagerInput.on('blur', function() {
                            //console.log('分页输入框失去焦点');
                            self.inputFocused = false;
                            
                            // 延迟处理 等待分页操作完成
                            setTimeout(function() {
                                var currentPage = $(self.clients_table).getGridParam('page');
                                //console.log('失去焦点后检查，当前页:', currentPage, '输入框焦点:', self.inputFocused);
                                
                                if (currentPage === 1 && !self.inputFocused) {
                                    //console.log('在第一页且输入框无焦点，重启定时器');
                                    self.startRefreshTimer();
                                }
                            }, 100);
                        });
                        
                        // 回车键处理
                        pagerInput.on('keyup', function(e) {
                            if (e.keyCode === 13) {
                                // 回车后会自动触发onPaging事件 处理定时器
                            }
                        });
                    }
                    
                    // 检查是否应该重启定时器
                    var currentPage = $(this).getGridParam('page');
                    if (currentPage === 1 && !self.inputFocused) {
                        // 如果在第一页且输入框无焦点
                        //console.log('需要重启定时器（由分页操作触发）');
                        self.startRefreshTimer();
                    }
                },
            });

            var $toolbar = $("#t_" + self.clients_table.replace('#', ''));
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

            // 初始加载客户端数据
            self.load_clients_dashboard();
            
            // 启动定时刷新
            self.startRefreshTimer();
        });
    },
    
    // 启动定时刷新器
    startRefreshTimer: function() {
        const self = this;
        
        // 如果定时器已经在运行，先停止
        if (self.timer) {
            clearInterval(self.timer);
        }
        self.timer = setInterval(function() {
        
            var currentPage = $(self.clients_table).getGridParam('page');
            //console.log('定时器触发，当前页:', currentPage);
            if (currentPage === 1) {
                // 只有在第一页时才刷新数据
                //console.log('在第一页，执行刷新');
                self.load_clients_dashboard();
            }
            // 不在第一页时什么也不做，定时器继续运行
        }, self.refresh_interval);
        
        //console.log('定时器已启动，间隔:', self.refresh_interval, 'ms');
    },
    
    // 停止定时刷新器
    stopRefreshTimer: function() {
        const self = this;
        
        if (self.timer) {
            clearInterval(self.timer);
            self.timer = null;
            //console.log('定时器已暂停');
        }
    },

    // 加载客户端信息
    load_clients_dashboard: function() {
        const self = this;
        
        he.bkload([
            self.object + '.list', 
            "wifi@n.stalist", 
            "wifi@a.stalist"
        ]).then(function(v) {
            self.clientlist = v[0];
            self.nstalist = v[1] || {};
            self.astalist = v[2] || {};
            
            // 处理客户端数据
            const rows = self.processClientData();
            
            // 更新在线客户端数量
            const onlineCount = rows.filter(row => row.livetime !== $.i18n('Leave')).length;
            $('#online-clients-count').text('(' + onlineCount + ' ' + $.i18n('Online') + ')');
            
            // 获取当前分页状态
            const currentPage = $(self.clients_table).getGridParam('page');
            const currentRowNum = $(self.clients_table).getGridParam('rowNum');
            
            //console.log('数据加载完成，当前页码:', currentPage);

            // 更新表格数据，保持当前分页状态
            $(self.clients_table).jqGrid('setGridParam', { 
                data: rows,
                page: currentPage,
                rowNum: currentRowNum
            }).trigger('reloadGrid');
            

            // 设置离线行的样式
            $('td[title=' + $.i18n('Leave') + ']').closest('tr').css({ color: '#888' });

        })
    },
    
    // 处理客户端数据
    processClientData: function() {
        const rows = [];
        const self = this;
        
        // 处理有线和WiFi客户端
        for (const index in self.clientlist) {
            const client = self.clientlist[index];
            const row = this.createClientRow(index, client);
            
            // 合并WiFi信息
            if (self.nstalist[index]) {
                this.mergeWifiInfo(row, self.nstalist[index]);
                self.nstalist[index] = null;
            } else if (self.astalist[index]) {
                this.mergeWifiInfo(row, self.astalist[index]);
                self.astalist[index] = null;
            }
            
            // 根据在线状态排序
            if (client.livetime) {
                rows.unshift(row);
            } else {
                rows.push(row);
            }
        }
        
        // 处理剩余的WiFi客户端
        this.processRemainingWifiClients(rows, self.nstalist);
        this.processRemainingWifiClients(rows, self.astalist);
        
        return rows;
    },
    
    // 创建客户端行
    createClientRow: function(mac, client) {
        const row = {
            'mac': mac,
            'ip': client.ip || '',
            'name': client.name || '',
            'rxtx': this.formatRxTx(client.rx_bytes, client.tx_bytes),
            'ifname': client.ifname ? $.i18n(client.ifname) : '',
            'ifdev': client.ifdev ? $.i18n(client.ifdev) : '',
            'livetime': client.livetime ? time2string(client.livetime) : $.i18n('Leave')
        };
        
        return row;
    },
    
    // 合并WiFi信息
    mergeWifiInfo: function(row, wifiInfo) {
        if (wifiInfo.ifdev) {
            row.ifdev = $.i18n(wifiInfo.ifdev);
        }
    },
    
    // 处理剩余的WiFi客户端
    processRemainingWifiClients: function(rows, wifiList) {
        for (const index in wifiList) {
            const client = wifiList[index];
            if (client == null) continue;
            
            const row = {
                'mac': index,
                'ip': '',
                'name': '',
                'rxtx': '',
                'ifname': '',
                'ifdev': client.ifdev ? $.i18n(client.ifdev) : '',
                'livetime': client.livetime ? time2string(client.livetime) : $.i18n('Leave')
            };
            
            if (client.livetime) {
                rows.unshift(row);
            } else {
                rows.push(row);
            }
        }
    },
    
    // 格式化Rx/Tx显示
    formatRxTx: function(rx_bytes, tx_bytes) {
        if (!rx_bytes && !tx_bytes) {
            return '';
        }
        
        const rx = rx_bytes || "0";
        const tx = tx_bytes || "0";
        return byte2readable(rx) + " / " + byte2readable(tx);
    }
};

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
    $.i18n().load(page.lang('dashboard')).then(function() {
        // 设置语言
        $.i18n().locale = lang;
        $('body').i18n();
        
        // 初始化客户端管理器
        clientManager.init();
        
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
});