
/* get the object */
var state;
var config;
var object = "ifname@wisp";
var index = page.param( 'object', location.hash );
if ( index )
{
    object = index;
}



/* load the status */
function status_load()
{
  he.bkload( [ object+".status" ] ).then( function(v){
      state = v[0];
      var info = state;
      var id = "#wisp";
      /* status end btn */
      if ( info.status )
      {
          $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
          $(id+"_status").text( $.i18n(info.status) );
          if ( info.status == "up" || info.status == "uping" )
          {
          }
          else
          {
              $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          }
      }
      else
      {
          $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          $(id+"_status").text( $.i18n("down") );
      }
      /* station */
      if ( info.rate )
      {
          $(id+"_rate").text( info.rate+'Mbps' );
      }
      $(id+"_peer").text( info.peer||' ' );
      $(id+"_peermac").text( info.peermac||' ' );
      $(id+"_channel").text( info.channel||' ' );
      if ( info.sig )
      {
          $(id+"_rssi").text( info.sig+"%" );
      }
      else if ( info.rssi )
      {
          $(id+"_rssi").text( info.rssi+"dBm" );
      }
      if ( info.signal )
      {
          $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );            
      }
      $(id+"_mac").text( info.mac||' ' );
	  /* txrx */
	  $(id+"_ip").text( info.ip||' ' );
	  $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
	  $(id+"_livetime").text( info.livetime||' ' );
  })
}

/* load the configure on the input */
function config_load()
{
  he.load( [ object, object+".chlist" ] ).then( function(v){
    config = v[0];
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
    $('#mode').val( config.mode || 'dhcpc' );
    if ( config.pppoec )
    {
      $('#username').val(config.pppoec.username);
      $('#password').val(config.pppoec.password);
      $('#lcp_echo_interval').val(config.pppoec.lcp_echo_interval);
      $('#lcp_echo_failure').val(config.pppoec.lcp_echo_failure);
    }
    if ( config.dhcpc )
    {
        $('#routeopt').prop('checked', able2boole(config.dhcpc.routeopt) );
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
        case 'static':
          $('#dhcpc_cfg').hide();
          $('#static_cfg').show();
          $('#pppoe_cfg').hide();
          $('#customdns_cfg').hide();
          break;
        case 'dhcpc':
          $('#dhcpc_cfg').show();
          $('#static_cfg').hide();
          $('#pppoe_cfg').hide();
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
          break;
        case 'pppoec':
          $('#dhcpc_cfg').hide();
          $('#static_cfg').hide();
          $('#pppoe_cfg').show();
          $('#customdns_cfg').show();
          if ( config.pppoec )
          {
            $('#custom_dns').prop('checked', able2boole(config.pppoec.custom_dns));
            $('#cdns').val( config.pppoec.dns );
            $('#cdns2').val( config.pppoec.dns2 );
          }
          if ($('#custom_dns').prop('checked'))
          {
            $('#customdns_config').show();
          }
          else
          {
            $('#customdns_config').hide();
          }
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
            break;
          case 'icmp':
            $('#icmp_cfg').show();
            $('#dns_cfg').hide();
            $('#recv_cfg').hide();
            break;
          case 'dns':
            $('#icmp_cfg').hide();
            $('#dns_cfg').show();
            $('#recv_cfg').hide();
            break;
          case 'recv':
            $('#icmp_cfg').hide();
            $('#dns_cfg').hide();
            $('#recv_cfg').show();
            break;
          case 'auto':
            $('#icmp_cfg').hide();
            $('#dns_cfg').show();
            $('#recv_cfg').show();
            break;
      }
    }).trigger('change');

    /* WIFI STA */
    $('#peer').val( config.peer || '' );
    $('#peer2').val( config.peer2 || '' );
    $('#peer3').val( config.peer3 || '' );
    $('#peermac').val( config.peermac || '' );
    $('#secure').val( config.secure || 'disable' );
    $('#wpa_encrypt').val( config.wpa_encrypt || 'tkipaes' );
    $('#wpa_key').val( config.wpa_key || '' );
    $('#ssid_disable').prop( 'checked', config.ssid_disable=="enable" );
    $('#secure').unbind('change').change(function (e) {
        var secure = e.target.value;
        if ( secure === 'disable' )
        {
            $('#secure_cfg').hide();
        }
        else
        {
            $('#secure_cfg').show();
        }
    }).trigger('change');
    $('#lock').prop( 'checked', config.peermac );
    $('#lock').unbind('change').change(function (e) {
        if ($(this).prop('checked'))
        {
            $('#strong').prop( 'checked', false );
            $('#lock_cfg').show();
        }
        else
        {
            $('#lock_cfg').hide();
        }
    }).trigger('change');
    $('#hidden').prop( 'checked', config.peermode=="hidden" );
    $('#hidden').unbind('change').change(function (e) {
        if ($(this).prop('checked'))
        {
            $('#hidden_cfg').show();
        }
        else
        {
            $('#hidden_cfg').hide();
        }
    }).trigger('change');
    /* 初始化信道select */
    var chlist = v[1];
    var selects = "";
    for ( var ch in chlist )
    {
        selects += ('<option value="' + ch + '">' + $.i18n( ch ) + '</option>');
    }
    $("#channel").html( selects );
    $('#channel').val( config.channel || '1' );

  })
}

/* save the configure */
function config_save()
{
  if ( config == null )
  {
    return;
  }
  var copy = JSON.parse(JSON.stringify(config));;

  /* status */
  if ( $('#status').prop('checked') == false )
  {
      config.status = "disable";
  }
  else
  {
      if ( config.status && config.status != "enable" )
      {
        config.status = "enable";
      }
      /* IPV4 */
      config.mode = $('#mode').val();
      if ( config.mode == "pppoec" )
      {
        if ( !config.pppoec )
        {
          config.pppoec = {};
        }
        config.pppoec.custom_dns = boole2able( $('#custom_dns').prop('checked') );
        if ( config.pppoec.custom_dns == "enable" )
        {
          config.pppoec.dns = $('#cdns').val();
          if ( config.pppoec.dns && check.ip(config.pppoec.dns) == false )
          {
              page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
          config.pppoec.dns2 = $('#cdns2').val();
          if ( config.pppoec.dns2 && check.ip(config.pppoec.dns2) == false )
          {
              page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
        }
        config.pppoec.username = $('#username').val();
        config.pppoec.password = $('#password').val();
        config.pppoec.lcp_echo_interval = $('#lcp_echo_interval').val();
        if ( config.pppoec.lcp_echo_interval && check.number(config.pppoec.lcp_echo_interval) == false )
        {
            page.alert( { message: $.i18n('LCP Echo Interval')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.pppoec.lcp_echo_failure = $('#lcp_echo_failure').val();
        if ( config.pppoec.lcp_echo_failure && check.number(config.pppoec.lcp_echo_failure) == false )
        {
            page.alert( { message: $.i18n('LCP Echo Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      else if ( config.mode == "dhcpc" )
      {
        if ( !config.dhcpc )
        {
          config.dhcpc = {};
        }
        config.dhcpc.routeopt = boole2able( $('#routeopt').prop('checked') );
        config.dhcpc.custom_dns = boole2able( $('#custom_dns').prop('checked') );
        if ( config.dhcpc.custom_dns == "enable" )
        {
          config.dhcpc.dns = $('#cdns').val();
          if ( config.dhcpc.dns && check.ip(config.dhcpc.dns) == false )
          {
              page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
          config.dhcpc.dns2 = $('#cdns2').val();
          if ( config.dhcpc.dns2 && check.ip(config.dhcpc.dns2) == false )
          {
              page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
        }
      }
      else if ( config.mode == "static" )
      {
        if ( !config.static )
        {
            config.static = {};
        }
        config.static.ip = $('#ip').val();
        if ( check.ip(config.static.ip) == false )
        {
            page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        config.static.mask = $('#mask').val();
        if ( check.ip(config.static.mask) == false )
        {
            page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        config.static.gw = $('#gw').val();
        if ( config.static.gw && check.ip(config.static.gw) == false )
        {
            page.alert( { message: $.i18n('Gateway')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        config.static.dns = $('#dns').val();
        if ( config.static.dns && check.ip(config.static.dns) == false )
        {
            page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        config.static.dns2 = $('#dns2').val();
        if ( config.static.dns2 && check.ip(config.static.dns2) == false )
        {
            page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
      }
      config.masq = boole2able( $('#masq').prop('checked') );
      config.mtu = $('#mtu').val();
      if ( config.mtu && check.number(config.mtu) == false )
      {
         page.alert( { message: $.i18n('MTU')+" "+$.i18n('must be a valid number') } );
         return;
      }
      /* IPV6 */
      if ( config.method )
      {
          config.method = $('#method').val();
          if ( config.method == "manual" )
          {
            if ( !config.manual )
            {
                config.manual = {};
            }
            config.manual.addr = $('#addr').val();
            if ( check.ipv6(config.manual.addr) == false )
            {
                page.alert( { message: $.i18n('IPv6 Address')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            config.manual.prefix = $('#prefix').val();
            if ( config.manual.prefix && ( config.manual.prefix < 0 || config.manual.prefix > 128 ) )
            {
                page.alert( { message: $.i18n('Subnet Prefix')+" "+$.i18n('must be a number(0-128)') } );
                return;
            }
            config.manual.hop = $('#hop').val();
            if ( config.manual.hop && check.ipv6(config.manual.hop) == false )
            {
                page.alert( { message: $.i18n('Next Hop')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            config.manual.resolve = $('#resolve').val();
            if ( config.manual.resolve && check.ipv6(config.manual.resolve) == false )
            {
                page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            config.manual.resolve2 = $('#resolve2').val();
            if ( config.manual.resolve2 && check.ipv6(config.manual.resolve2) == false )
            {
                page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
          }
          else if ( config.method == "slaac" )
          {
          }
          else if ( config.method == "automatic" )
          {
          }
      }
      /* Keeplive */
      if ( !config.keeplive )
      {
        config.keeplive = {};
      }
      config.keeplive.type = $('#keeplive').val();
      if ( config.keeplive.type == "icmp" )
      {
        if ( !config.keeplive.icmp )
        {
          config.keeplive.icmp = {};
        }
        if ( !config.keeplive.icmp.dest )
        {
          config.keeplive.icmp.dest = {};
        }
        config.keeplive.icmp.dest.test = $('#icmp_test').val();
        if ( !config.keeplive.icmp.dest.test )
        {
            page.alert( { message: $.i18n('Test Address')+" "+$.i18n('Can not be empty') } );
            return;
        }
        config.keeplive.icmp.dest.test2 = $('#icmp_test2').val();
        config.keeplive.icmp.dest.test3 = $('#icmp_test3').val();
        config.keeplive.icmp.timeout = $('#icmp_timeout').val();
        if ( check.number(config.keeplive.icmp.timeout) == false )
        {
            page.alert( { message: $.i18n('Each Query Timeout(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.icmp.failed = $('#icmp_failed').val();
        if ( check.number(config.keeplive.icmp.failed) == false )
        {
            page.alert( { message: $.i18n('Test Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.icmp.interval = $('#icmp_interval').val();
        if ( check.number(config.keeplive.icmp.interval) == false )
        {
            page.alert( { message: $.i18n('Test Interval(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      if ( config.keeplive.type == "dns" || config.keeplive.type == "auto" )
      {
        if ( !config.keeplive.dns )
        {
          config.keeplive.dns = {};
        }
        config.keeplive.dns.timeout = $('#dns_timeout').val();
        if ( check.number(config.keeplive.dns.timeout) == false )
        {
            page.alert( { message: $.i18n('Each Query Timeout(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.dns.failed = $('#dns_failed').val();
        if ( check.number(config.keeplive.dns.failed) == false )
        {
            page.alert( { message: $.i18n('Query Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.dns.interval = $('#dns_interval').val();
        if ( check.number(config.keeplive.dns.interval) == false )
        {
            page.alert( { message: $.i18n('Query Interval(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      if ( config.keeplive.type == "recv" || config.keeplive.type == "auto" )
      {
        if ( !config.keeplive.recv )
        {
          config.keeplive.recv = {};
        }
        config.keeplive.recv.timeout = $('#recv_timeout').val();
        if ( check.number(config.keeplive.recv.timeout) == false )
        {
            page.alert( { message: $.i18n('Count Duration(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.recv.failed = $('#recv_failed').val();
        if ( check.number(config.keeplive.recv.failed) == false )
        {
            page.alert( { message: $.i18n('Count Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        config.keeplive.recv.packets = $('#recv_packets').val();
        if ( check.number(config.keeplive.recv.packets) == false )
        {
            page.alert( { message: $.i18n('Received Packets')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
  }

  /* here add the sta */
  config.peer = $('#peer').val();
  config.peer2 = $('#peer2').val();
  config.peer3 = $('#peer3').val();
  config.peermac = '';
  if ( $('#lock').prop('checked') )
  {
    config.peermac = $('#peermac').val();
    if ( check.mac( config.peermac, true ) == false )
    {
        page.alert( {
            message: $.i18n('Peer BSSID')+" "+$.i18n('must be a valid MAC address'),
            callback: function()
            {
                $('#peermac').select();
            }
        } );
        return;
    }
  }
  config.secure = $('#secure').val();
  if ( config.secure != 'disable' )
  {
      config.wpa_encrypt = $('#wpa_encrypt').val();
      config.wpa_key = $('#wpa_key').val();
      if ( !config.wpa_key )
      {
          page.alert( { message: $.i18n('Password')+" "+$.i18n('Can not be empty') } );
          return;
      }
  }
  config.ssid_disable = boole2able( $('#ssid_disable').prop('checked') );
  delete config.peermode;
  delete config.channel;
  if ( $('#hidden').prop('checked') )
  {
      config.peermode = "hidden";
      config.channel = $('#channel').val();
      if ( !config.channel )
      {
          page.alert( { message: $.i18n('Channel')+" "+$.i18n('Can not be empty') } );
          return;
      }
  }

  if ( ocompare( config, copy ) )
  {
      page.alert( { message: $.i18n('Settings unchanged') } );
      return;
  }
  page.confirm( { message: $.i18n('The WISP connecttion will be disconneted because of the change of configuration') } ).then( function(result){
    if ( result )
    {
      he.save( [ object+"="+JSON.stringify(config)] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        config_load();
      });
    }
  });
}



/* init */
page.password('password', 'password-icon' );
page.password('wpa_key', 'wpa_key-icon' );
$.i18n().load( page.lang('wan') ).then( function () {
    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    // jqgrid
    jqtable.create( '#aplist-grid-table', '#aplist-grid-pager',
        {
            multiselect: false,
            caption: $.i18n('AP List'),
            colNames: [ $.i18n('Choose'), $.i18n('SSID'), $.i18n('Channel'), $.i18n('Signal'), $.i18n('MAC'), $.i18n('Security Mode'), $.i18n('WPA Mode') ],
            colModel:
            [
                {
                    name: 'choose', width: 80,
                    fixed: true, sortable: false,
                    formatter: function ( cellvalue, options, rowObject )
                    {
                        return '<button class="btn btn-main btn-xs btn-choose" onclick="ap_select(' + options.rowId + ')" data-id="' + options.rowId + '">' + $.i18n('Choose') + '</button>'
                    }
                },
                { name: 'ssid', width: 180 },
                { name: 'channel', width: 50 },
                {
                    name: 'signal', width: 50,
                    formatter: function ( cellvalue, options, rowObject )
                    {
                        if ( cellvalue > 0 )
                        {
                            return "<img src='/assets/css/images/signal_"+cellvalue+".png' class='line-signal'></img>";
                        }
                        else
                        {
                            return "<img src='/assets/css/images/signal_0.png' class='line-signal'></img>";
                        }
                    }
                },
                { name: 'mac', width: 130 },
                {
                    name: 'secure', width: 130,
                    formatter: function ( cellvalue )
                    {
                        return '<span data-secure="' + cellvalue + '">' + $.i18n(cellvalue) + '</span>';
                    },
                    unformat: function (cellvalue, options, cell)
                    {
                        return $(cell).children('span').data('secure');
                    }
                },
                {
                    name: 'wpa_encrypt', width: 80,
                    formatter: function (cellvalue)
                    {
                        return '<span data-wpa_encrypt="' + cellvalue + '">' + $.i18n(cellvalue) + '</span>';
                    },
                    unformat: function (cellvalue, options, cell)
                    {
                        return $(cell).children('span').data('wpa_encrypt');
                    }
                }
            ]
        }
    ).jqGrid( 
          'navGrid', '#aplist-grid-pager',
          $.extend(true, {}, jqtable.navOptions, { add: false, edit: false, search: false, refresh: false, del: false, view: false } ),
          {},{},{},{},{},
    );

    /* load the configure */
    status_load();
    config_load();

    /* bind the button */
    $('#wisp_btn').on(ace.click_event, function () {
        if ( state.status == "down" )
        {
            he.exec( [ object+'.setup' ] ).then( function(result){status_load();} );
        }
        else
        {
            he.exec( [ object+'.shut' ] ).then( function(result){status_load();} );
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

    // scanning
    $('#apscan').unbind(ace.click_event).on(ace.click_event, function () {
        he.exec( [ object+'.aplist' ], $.i18n("Scanning") ).then( function(v){
            var list = v[0];
            var rows = json2array( list, {}, "mac" );
            // 给表格赋值
            $('#aplist-grid-table').jqGrid('clearGridData').jqGrid('setGridParam', {
                data: rows
            }).trigger('reloadGrid');
            $('#aplist-modal').modal('show');
        });
    });
    $('#rescan').unbind(ace.click_event).on(ace.click_event, function () {
        he.exec( [ object+'.aplist' ], $.i18n("Scanning") ).then( function(v){
            var list = v[0];
            var rows = json2array( list, {}, "mac" );
            // 给表格赋值
            $('#aplist-grid-table').jqGrid('clearGridData').jqGrid('setGridParam', {
                data: rows
            }).trigger('reloadGrid');
            // 显示对话框
            $('#aplist-modal').modal('show');
        });
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



/* 必须在全局中才可以被调用到 */
function ap_select( rowId )
{
    // 隐藏表格
    $('#aplist-modal').modal('hide');
    // 某个选中行的数据
    var ap = $('#aplist-grid-table').jqGrid( 'getRowData', rowId );
    // 将选中的数据填到对应的输入框中
    $('#peer').val( ap.ssid );
    $('#peermac').val( ap.mac );
    $('#lock').prop( 'checked', false );
    $('#lock').trigger('change');
    $('#secure').val( ap.secure || 'disable');
    // wpa_encrypt 为auto 时设置为tkipaes
    if ( ap.wpa_encrypt === 'auto' )
    {
        $('#wpa_encrypt').val('tkipaes');
    }
    else
    {
        $('#wpa_encrypt').val(ap.wpa_encrypt || 'tkipaes');
    }
    // 手动触发change，以便显示隐藏密码输入框
    $('#secure').trigger('change');
    // $('#wpa_key').val('');
    $('#hidden').prop( 'checked', false );
    $('#hidden').trigger('change');
    $('#radio').val('');
    $('#channel').val('');
}


  
