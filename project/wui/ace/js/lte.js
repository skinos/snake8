window.LteConfigManager = {
    cache: {},
    // 只负责拿数据
    loadStatus: function(targetObject, forceRefresh) {
        var self = this;
        if (!forceRefresh && self.cache[targetObject]) {
            return Promise.resolve(self.cache[targetObject]);
        }
        return he.load([
            targetObject,
            targetObject + ".ifdev",
            targetObject + ".operator",
            targetObject + ".status",
            targetObject + ".smslist",
        ]).then(function(v) {
          v = v || [];
          for (var i = 0; i <= 4; i++) {
              if (typeof v[i] === 'undefined' || v[i] === null) {
                  v[i] = (i >= 3) ? "" : {};
              }
          }
          self.cache[targetObject] = v;
          return v;
        });
    },

    loadSettings: function(modem, ifname, forceRefresh) {
        var self = this;
        var cacheKey = modem + "_" + ifname; // 组合 Key
        if (!forceRefresh && self.cache[cacheKey]) {
            return Promise.resolve(self.cache[cacheKey]);
        }
        return he.load([
            modem,
            ifname,           
            ifname + ".status",
            modem + ".custom_set",
            modem + ".custom_watch",
            ifname + ".lock_imei",
            ifname + ".lock_imsi",
            modem + ".sms_list"
        ]).then(function(v) {
            v = v || [];
            for (var i = 0; i <= 6; i++) {
                if (typeof v[i] === 'undefined' || v[i] === null) {
                    v[i] = (i >= 5) ? "" : {};
                }
            }
            self.cache[cacheKey] = v;
            return v;
        });
    }
};

var state;
var config;// 方便config_save调用v[0]
var modem;
var operator;
var object = "ifname@lte";

function config_load() {  
    window.LteConfigManager.loadStatus(object, true).then(function(v) {  
        if (!v) {    
            return; 
        }  
        lte_basic(v);  
        //status_load(v[3] || {}); 
    });  
};

/* load the status */
function status_load()
{
  he.bkload( [ object+".status" ] ).then( function(v){
    state = v[0]
    var info = state;

      if(!info){
        info = {};
      }

      var id = "#lte";
      /* status end btn */
      if ( info.status )
      {
          $(id+"_status").text( $.i18n(info.status) );
          if ( info.status == "down" )
          {
              $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          }
          else
          {
			  $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
          }
      }
      else
      {
          $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          $(id+"_status").text( $.i18n("down") );
      }
      /* network */
      if ( info.operator )
      {
          $(id+"_operator").text( $.i18n(info.operator) );
      }
      else if ( info.plmn )
      {
          $(id+"_operator").text( $.i18n(info.plmn) );
      }
      else
      {
          $(id+"_operator").text( "" );
      }
      if ( info.nettype )
      {
          $(id+"_nettype").text( info.nettype );
      }
      else
      {
          $(id+"_nettype").text( "" );
      }
      if ( info.signal )
      {
          $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );
      }
      else
      {
          $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_0.png" );
      }
      if ( info.csq )
      {
          $(id+"_csq").text( info.csq );
      }
      else
      {
          $(id+"_csq").text( "" );
      }
      /* signal */
      if ( info.rssi )
      {
          $(id+"_rssi").text( info.rssi+"dBm" );
      }
      else
      {
          $(id+"_rssi").text( "" );
      }
      if ( info.rsrp )
      {
          $(id+"_rsrp").text( "RSRP:"+info.rsrp+"dBm" );
      }
      else
      {
          $(id+"_rsrp").text( "" );
      }
      if ( !info.rssi && !info.rsrp )
      {
          $(id+"_rssi").text( $.i18n("nosignal") );
      }
      /* device info */
      if ( info.imei == "noimei" )
      {
          $(id+"_imei").text( $.i18n("noimei") );
      }
      else
      {
        $(id+"_imei").text( info.imei||' ' );
      }
      if ( info.imei == "noimsi" )
      {
          $(id+"_imei").text( $.i18n("noimsi") );
      }
      else
      {
          $(id+"_imsi").text( info.imsi||' ' );
      }
      if ( info.iccid )
      {
          $(id+"_iccid").text( $.i18n(info.iccid) );
      }
      else
      {
          $(id+"_iccid").text( "" );
      }
	 
      $(id+"_ip").text( info.ip||' ' );
      if ( info.delay )
	  {
		  if ( info.delay == "failed" || info.delay == "block" )
		  {
			  $(id+"_delay").text( $.i18n(info.delay) );
		  }
		  else
		  {
			  $(id+"_delay").text( $.i18n("Delay")+":"+ info.delay + "ms" );
		  }
	  }
	  else
	  {
		  $(id+"_delay").text( "" );
	  }

      /* txrx */
	  $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
	  $(id+"_livetime").text( info.livetime||' ' );
    })
  }

/* load the configure on the input */
function lte_basic(v)
{
    config = v[0];
    if(!config){
      config = {}
    }

    /* init the network mode select */
    $("#mode").empty();
    $("#mode").append("<option value=''>"+$.i18n('Advise')+"</option>");
    $("#mode").append("<option value='ppp'>"+$.i18n('PPP')+"</option>");
    $("#mode").append("<option value='dhcpc'>"+$.i18n('DHCP')+"</option>");
    $("#mode").append("<option value='static'>"+$.i18n('Static IP')+"</option>");
    /* profile */
    if ( config.profile == "enable" )
    {
        $('#profile').prop('checked', true );
    }
    else
    {
        $('#profile').prop('checked', false );
    }
    if ( config.profile_cfg )
    {
		$('#dial').val(config.profile_cfg.dial);
		$('#apn').val(config.profile_cfg.apn);
		$('#user').val(config.profile_cfg.user);
		$('#passwd').val(config.profile_cfg.passwd);
		$('#type').val(config.profile_cfg.type||"ipv4");
		$('#auth').val(config.profile_cfg.auth);
    }
    else
    {
    operator = v[2];
    if(!operator){
      operator = {};
    }
		if ( operator )
		{
			$('#dial').val(operator.dial);
			$('#apn').val(operator.apn);
			$('#user').val(operator.user);
			$('#password').val(operator.passwd);
			$('#type').val(operator.type||"ipv4");
			$('#auth').val(operator.auth);
		}
    }
    $('#profile').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#profile_cfg').show();
		if ( state && state.na != "enable" )
        {
			$('#mode').val( "ppp" );
        }
      } 
      else
      {
        $('#profile_cfg').hide();
		if ( state && state.na != "enable" )
		{
			$('#mode').val( "" );
		}
      }
    }).trigger('change');
	
    modem = v[1];
    if(!modem){
      modem = {};
    }
    window.modem = modem;

    /* status */
    if ( config.status && config.status == "disable" )
    {
        $('#status').prop('checked', false );
    }
    else
    {
        $('#status').prop('checked', true );
    }
    $('#status').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#status_config').show();
      }
      else
      {
        $('#status_config').hide();
      }
    }).trigger('change');
    /* IPV4 */
    $('#mode').val( config.mode||"" );
    if ( config.ppp )
    {
      $('#lcp_echo_interval').val(config.ppp.lcp_echo_interval);
      $('#lcp_echo_failure').val(config.ppp.lcp_echo_failure);
      $('#pppopt').val(config.ppp.pppopt);
    }
    if ( config.static )
    {
      $('#ip').val(config.static.ip);
      $('#mask').val(config.static.mask);
      $('#gw').val(config.static.gw);
      $('#dns').val(config.static.dns);
      $('#dns2').val(config.static.dns2);
    }
    $('#masq').prop('checked', able2boole(config.masq));
    $('#mtu').val(config.mtu||"");
    $('#mode').unbind('change').change(function (e) {
      var type = e.target.value;
      switch (type)
      {
        case '':
          $('#dhcpc_cfg').hide();
          $('#static_cfg').hide();
          $('#ppp_cfg').hide();
          $('#customdns_cfg').hide();
          $('#keeplive').val( "recv" );
          $('#icmp_cfg').hide();
          $('#recv_cfg').show();
          break;
        case 'static':
          $('#dhcpc_cfg').hide();
          $('#static_cfg').show();
          $('#ppp_cfg').hide();
          $('#customdns_cfg').hide();
          $('#keeplive').val( "recv" );
          $('#icmp_cfg').hide();
          $('#recv_cfg').show();
          break;
        case 'dhcpc':
          $('#dhcpc_cfg').show();
          $('#static_cfg').hide();
          $('#ppp_cfg').hide();
          $('#customdns_cfg').show();
          if ( config.dhcpc )
          {
            $('#custom_dns').prop('checked', able2boole(config.dhcpc.custom_dns));
            $('#cdns').val( config.dhcpc.dns );
            $('#cdns2').val( config.dhcpc.dns2 );
          }
          if ($('#custom_dns').prop('checked'))
          {
            $('#customdns_config').show();
          }
          else
          {
            $('#customdns_config').hide();
          }
          $('#keeplive').val( "recv" );
          $('#icmp_cfg').hide();
          $('#recv_cfg').show();
          break;
        case 'ppp':
          $('#dhcpc_cfg').hide();
          $('#static_cfg').hide();
          $('#ppp_cfg').show();
          $('#customdns_cfg').show();
          if ( config.ppp )
          {
            $('#custom_dns').prop('checked', able2boole(config.ppp.custom_dns));
            $('#cdns').val( config.ppp.dns );
            $('#cdns2').val( config.ppp.dns2 );
          }
          if ($('#custom_dns').prop('checked'))
          {
            $('#customdns_config').show();
          }
          else
          {
            $('#customdns_config').hide();
          }
          $('#keeplive').val( "disable" );
          $('#icmp_cfg').hide();
          $('#recv_cfg').hide();
          break;
      }
    }).trigger('change');
    $('#custom_dns').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#customdns_config').show();
      }
      else
      {
        $('#customdns_config').hide();
      }
    }).trigger('change');
    /* IPV6 */
    if ( config.method )
    {
        $('#ipv6_cfg').show();
        $('#method').val(config.method);
        if ( config.manual )
        {
          if ( config.manual.addr )
          {
            $('#addr').val(config.manual.addr);
            $('#prefix').val(config.manual.prefix);
          }
          $('#hop').val(config.manual.hop);
          $('#resolve').val(config.manual.resolve);
          $('#resolve2').val(config.manual.resolve2);
        }
    }
    $('#method').unbind('change').change(function (e) {
      var type = e.target.value;
      switch (type)
      {
        case 'disable':
        case 'slaac':
            $('#manual_cfg').hide();
            break;
        case 'automatic':
            $('#manual_cfg').hide();
            break;
        case 'manual':
          $('#manual_cfg').show();
          break;
      }
    }).trigger('change');
    /* Keeplive */
    if ( config.keeplive )
    {
        $('#keeplive').val( config.keeplive.type || 'disable');
        if ( config.keeplive.icmp )
        {
          if (config.keeplive.icmp.dest)
          {
            $('#icmp_test').val(config.keeplive.icmp.dest.test)
            $('#icmp_test2').val(config.keeplive.icmp.dest.test2)
            $('#icmp_test3').val(config.keeplive.icmp.dest.test3)
          }
          $('#icmp_timeout').val(config.keeplive.icmp.timeout);
          $('#icmp_failed').val(config.keeplive.icmp.failed);
          $('#icmp_interval').val(config.keeplive.icmp.interval);
        }
        if ( config.keeplive.dns )
        {
          $('#dns_timeout').val(config.keeplive.dns.timeout);
          $('#dns_failed').val(config.keeplive.dns.failed);
          $('#dns_interval').val(config.keeplive.dns.interval);
        }
        if ( config.keeplive.recv )
        {
          $('#recv_timeout').val(config.keeplive.recv.timeout);
          $('#recv_failed').val(config.keeplive.recv.failed);
          $('#recv_packets').val(config.keeplive.recv.packets);
        }
        $('#action').val( config.keeplive.action || '');
    }
    else
    {
        $('#keeplive').val( 'disable');
    }
    $('#keeplive').unbind('change').change(function (e) {
      var type = e.target.value;
      switch (type)
      {
          case 'disable':
            $('#icmp_cfg').hide();
            $('#dns_cfg').hide();
            $('#recv_cfg').hide();
            $('#action_cfg').hide();
            break;
          case 'icmp':
            $('#icmp_cfg').show();
            $('#dns_cfg').hide();
            $('#recv_cfg').hide();
            $('#action_cfg').show();
            break;
          case 'dns':
            $('#icmp_cfg').hide();
            $('#dns_cfg').show();
            $('#recv_cfg').hide();
            $('#action_cfg').show();
            break;
          case 'recv':
            $('#icmp_cfg').hide();
            $('#dns_cfg').hide();
            $('#recv_cfg').show();
            $('#action_cfg').show();
            break;
          case 'auto':
            $('#icmp_cfg').hide();
            $('#dns_cfg').show();
            $('#recv_cfg').show();
            $('#action_cfg').show();
            break;
      }
    }).trigger('change');
  
}

/* save the configure */
function config_save() {
    if (!config) return;

    var copy = JSON.parse(JSON.stringify(config));

    // 获取值并校验 失败则抛出异常停止执行
    var getAndValidate = function(selector, label, type, required) {
        var val = $(selector).val();
        if (required && (!val || val.trim() === "")) {
            page.alert({ message: $.i18n(label) + " " + $.i18n('Can not be empty') });
            throw "VALIDATION_FAILED"; // 抛出异常直接中断整个函数
        }
        if (val) {
            var isOk = true;
            if (type === 'ip') isOk = check.ip(val);
            else if (type === 'ipv6') isOk = check.ipv6(val);
            else if (type === 'num') isOk = check.number(val);
            
            if (!isOk) {
                page.alert({ message: $.i18n(label) + " " + $.i18n('is invalid') });
                throw "VALIDATION_FAILED";
            }
        }
        return val;
    };

    try {
        // Profile
        config.profile = boole2able($('#profile').prop('checked'));
        if (config.profile === "enable") {
            config.profile_cfg = config.profile_cfg || {};
            var p = config.profile_cfg;
            ['dial', 'apn', 'user', 'passwd', 'type', 'auth'].forEach(function(k) {
                p[k] = $('#' + k).val();
            });
        }

        // Status
        if (!$('#status').prop('checked')) {
            config.status = "disable";
        } else {
            config.status = (config.status && config.status !== "enable") ? "enable" : config.status || "enable";

            // IPv4
            config.mode = $('#mode').val();
            if (config.mode === "ppp") {
                config.ppp = config.ppp || {};
                config.ppp.custom_dns = boole2able($('#custom_dns').prop('checked'));
                if (config.ppp.custom_dns === "enable") {
                    config.ppp.dns = getAndValidate('#cdns', 'DNS', 'ip', true);
                    config.ppp.dns2 = getAndValidate('#cdns2', 'DNS2', 'ip');
                }
                config.ppp.lcp_echo_interval = getAndValidate('#lcp_echo_interval', 'LCP Echo Interval', 'num', true);
                config.ppp.lcp_echo_failure = getAndValidate('#lcp_echo_failure', 'LCP Echo Times', 'num', true);
                config.ppp.pppopt = $('#pppopt').val();
            } 
            else if (config.mode === "dhcpc") {
                config.dhcpc = config.dhcpc || {};
                config.dhcpc.routeopt = boole2able($('#routeopt').prop('checked'));
                config.dhcpc.custom_dns = boole2able($('#custom_dns').prop('checked'));
                if (config.dhcpc.custom_dns === "enable") {
                    config.dhcpc.dns = getAndValidate('#cdns', 'DNS', 'ip', true);
                    config.dhcpc.dns2 = getAndValidate('#cdns2', 'DNS2', 'ip');
                }
            } 
            else if (config.mode === "static") {
                config.static = config.static || {};
                var s = config.static;
                s.ip = getAndValidate('#ip', 'IPv4 Address', 'ip', true);
                s.mask = getAndValidate('#mask', 'Subnet Mask', 'ip', true);
                s.gw = getAndValidate('#gw', 'Gateway', 'ip');
                s.dns = getAndValidate('#dns', 'DNS', 'ip');
                s.dns2 = getAndValidate('#dns2', 'DNS2', 'ip');
            }

            config.masq = boole2able($('#masq').prop('checked'));
            config.mtu = getAndValidate('#mtu', 'MTU', 'num');

            // IPv6
            if (config.method) {
                config.method = $('#method').val();
                if (config.method === "manual") {
                    config.manual = config.manual || {};
                    var m = config.manual;
                    m.addr = getAndValidate('#addr', 'IPv6 Address', 'ipv6', true);
                    m.prefix = $('#prefix').val();
                    if (m.prefix && (m.prefix < 0 || m.prefix > 128)) {
                        page.alert({ message: $.i18n('Subnet Prefix') + " (0-128)" });
                        throw "VALIDATION_FAILED";
                    }
                    m.hop = getAndValidate('#hop', 'Next Hop', 'ipv6');
                    m.resolve = getAndValidate('#resolve', 'DNS', 'ipv6');
                    m.resolve2 = getAndValidate('#resolve2', 'DNS2', 'ipv6');
                }else if ( config.method === "slaac" )
                  {
                  }
                  else if ( config.method === "automatic" )
                  {
                  }
            }

            // Keeplive
            config.keeplive = config.keeplive || {};
            var kl = config.keeplive;
            kl.type = $('#keeplive').val();
            kl.action = $('#action').val();

            if (kl.type === "icmp") {
                kl.icmp = kl.icmp || { dest: {} };
                kl.icmp.dest = kl.icmp.dest || {};
                kl.icmp.dest.test = getAndValidate('#icmp_test', 'Test Address', '', true);
                kl.icmp.dest.test2 = $('#icmp_test2').val();
                kl.icmp.dest.test3 = $('#icmp_test3').val();
                kl.icmp.timeout = getAndValidate('#icmp_timeout', 'Each Query Timeout(sec)', 'num');
                kl.icmp.failed = getAndValidate('#icmp_failed', 'Test Times', 'num');
                kl.icmp.interval = getAndValidate('#icmp_interval', 'Test Interval(sec)', 'num');
            } 
            else if (kl.type === "dns" || kl.type === "recv" || kl.type === "auto") {
                var types = (kl.type === "auto") ? ["dns", "recv"] : [kl.type];
                types.forEach(function(t) {
                    kl[t] = kl[t] || {};
                    kl[t].timeout = getAndValidate('#' + t + '_timeout', 'Each Query Timeout(sec)', 'num', true);
                    kl[t].failed = getAndValidate('#' + t + '_failed', 'Query Times', 'num', true);
                    var lastField = (t === "dns") ? "interval" : "packets";
                    kl[t][lastField] = getAndValidate('#' + t + '_' + lastField, lastField, 'num', true);
                });
            }
        }

        if (ocompare(config, copy)) {
            page.alert({ message: $.i18n('Settings unchanged') });
            return;
        }

        var msg = $.i18n('The LTE connection will be disconneted because of the change of configuration');
        page.confirm({ message: msg }).then(function(result) {
            if (!result) return location.reload();
            he.exec([object + "=" + JSON.stringify(config)]).then(function() {
                page.hint2succeed($.i18n('Modify successfully'));
                config_load();
            });
        });

    } catch (e) {
        if (e === "VALIDATION_FAILED") {
            // 验证失败 代码通过page.alert提示 直接捕获并中断
            console.log("Validation interrupted.");
        }
    }
}

/* init */
page.password('passwd', 'password-icon' );
$.i18n().load( page.lang('lte') ).then( function () {
    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    status_load();
    /* load the configure */
    config_load();

    /* bind the button */
    $('#lte_btn').on(ace.click_event, function () {
        if ( state.status == "up" || state.status == "uping" )
        {
            he.exec( [ object+'.shut' ] ).then( function(result){status_load();} );
        }
        else
        {
            he.exec( [ object+'.setup' ] ).then( function(result){status_load();} );
        }
    });

    // set the timer
    page.timing({
      refresh: function ()
      {
          status_load();
      },
      interval: 1000
    });

    /* bind the refresh */
    $('#refresh').on(ace.click_event, function () {
        location.reload();
    });
    /* bind the apply */
    $('#apply').on(ace.click_event, function () {
        config_save();
    });
});

// 点击标签页 动态加载html内容 不刷新页面
$(document).ready(function() {    
    var TabManager = {    
        // 记录进入页面时的主路径 获取#lte
        // 在ace-admin框架下直接使用#ltemodem或是#ltesms 会导致直接页面html 而不保持标签页状态
        // 编写新hash解决问题
        mainRoute: window.location.hash.split('?')[0],   

        // 新增标签页直接增加tabs内容以及html代码段即可
        tabs: {    
            ltemodem: { htmlUrl: '/content/ltemodem.html'},    
            ltesms:   { htmlUrl: '/content/ltesms.html'},    
            lteat:  { htmlUrl: '/content/lteat.html'},    
            simcard:  { htmlUrl: '/content/ltesim.html'}    
        },    
        
        // 初始化
        init: function() {    
            this.syncParams();   
            
            $('#lteTabs a').off('click').on('click', this.handleTabClick.bind(this));    

            this.restoreState();   
        },   

        // 同步url参数
        syncParams: function() {   
            var currentHash = window.location.hash;
            
            // 获取参数
            var urlModem  = page.param('modem', currentHash);
            var urlObject = page.param('object', currentHash);
            var urlTab    = page.param('tab', currentHash);

            window.modem = urlModem || window.modem || 'modem@lte';   
            window.object = urlObject ||  window.object || 'ifname@lte';   
            
            // 存入session sessionStorage 跳转标签页时保存具体状态
            // 否则默认第一个标签页

            var tabId = urlTab || 'lte';

            if (!urlModem || !urlObject || !urlTab) {
                this.updateUrl(tabId, true); 
            }
        },   

        // 更新url
        updateUrl: function(tabId) {
            var newHash = this.mainRoute + '?tab=' + tabId + 
                          '&modem=' + encodeURIComponent(window.modem) + 
                          '&object=' + encodeURIComponent(window.object);
            
            if (window.location.hash === newHash) return;
            history.replaceState(null, '', window.location.pathname + newHash);
        },

        handleTabClick: function(e) {    
            e.preventDefault();    
            var tabId = $(e.currentTarget).attr('href').substring(1);    

            this.updateUrl(tabId, false);// 更新地址栏 
            this.switchToTab(tabId);// 切换标签页显示内容
        },    

        restoreState: function() {   
            var currentHash = window.location.hash;
            var tabId = page.param('tab', currentHash) || sessionStorage.getItem('lte_active_tab') || 'lte';   
            this.switchToTab(tabId);   
        },   
            
        switchToTab: function(tabId) {   
          var self = this;
          var $tabContent = $('.tab-content');
          var $newPane = $('#' + tabId);
          var $oldPane = $('.tab-pane.active');

          // 仅更新标签页按钮的高亮 不切内容
          $('#lteTabs li').removeClass('active');      
          $('#lteTabs a[href="#' + tabId + '"]').parent().addClass('active');      

          // 锁定当前高度 防止大变小时高度塌陷导
          // 只有在切换时才锁定，并在完成后释放
          var currentHeight = $oldPane.outerHeight();
          if(currentHeight > 0) {
              $tabContent.css('min-height', currentHeight + 'px');
          }

          // 立即隐藏面板 不再等待ajax
          $oldPane.removeClass('active');

          // 首页逻辑 静态内容直接切
          if (tabId === 'lte') {      
              $('.tab-pane').removeClass('active');
              $('#lte').addClass('active');
              $tabContent.css('min-height', ''); // 释放高度
              if (typeof config_load === 'function') config_load();      
              return;
          }

          // 加载完内容后 再把旧的隐 新的显
          var tab = this.tabs[tabId];
          $.ajax({
              url: tab.htmlUrl,
              type: 'GET',
              dataType: 'html',
              success: function(data) {
                  // 先往隐藏的 $newPane 里填内容
                  $newPane.html(data);

                  // 此时才执行切换
                  // 旧的移除 active 新的添加 active 高度锁定了 不会闪
                  $oldPane.removeClass('active');
                  $newPane.addClass('active');

                  // 延迟释放高度锁定 给浏览器渲染 HTML 留出大概 100ms 的吞吐时间
                  setTimeout(function() {
                      $tabContent.css('min-height', '');
                  }, 100);
              }
          });
      },    
    };    
    
    TabManager.init();    
});