
/* get the object */
var state;
var config;
var object = "ifname@wisp";
var currentActiveRow = 1; // 记录当前操作的是第几行
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
	  $(id+"_ip").text( info.ip||' ' );
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

      // 延迟头部显示控制
      if (info.delay) {
          $(id + "_delay_head").show();
      } else {
          $(id + "_delay_head").hide();
      }
      
      /* txrx */
	  $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
	  $(id+"_livetime").text( info.livetime||' ' );
  })
}

/* load the configure on the input */
function config_load()
{
  he.load( [ object, object+".chlist", object+".securelist" ] ).then( function(v){
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

   
    [ '', '2', '3' ].forEach(function(s) {
        var macValue = config['peermac' + s] || '';
        var $macInput = $('#peermac' + s);
        
        // 加载时同时设置值和备份
        $macInput.val(macValue).data('backup-mac', macValue);
        
        // 根据是否有值自动勾选 lock
        $('#lock' + s).prop('checked', !!macValue).trigger('change');
        $('#peer' + s).val(config['peer' + s] || '');
    });

    $('#nossid').prop( 'checked', config.nossid=="enable" );
    $('#wpa_key').val( config.wpa_key || '' );
    $('#wpa_key2').val( config.wpa_key2 || '' );
    $('#wpa_key3').val( config.wpa_key3 || '' );
    var securelist = v[2];
    // ssid
	if ( securelist )
	{
		$("#secure").empty();			 
		for( m in securelist )
		{
			$("#secure").append("<option value='"+m+"'>" + $.i18n(m) + "</option>");
		}
		$('#secure').val( config.secure || 'disable' );
		$('#secure').unbind('change').change(function (e) {
		  var secure = e.target.value;
		  if ( secure == "disable")
		  {
			  $("#secure_cfg").hide();
              $("#secure_cfg2").hide();
		  }
		  else
		  {
			  $("#wpa_encrypt").empty();			
			  $("#secure_cfg").show();
              $("#secure_cfg2").show();
			  if ( securelist[secure] )
			  {
				  for( b in securelist[secure] )
				  {
					  $("#wpa_encrypt").append("<option value='"+b+"'>" + $.i18n(b) + "</option>");
				  }
			  }
			  $('#wpa_encrypt').val( config.wpa_encrypt || 'aes' );
		  }
		}).trigger('change');
	}
	else
	{
		$('#secure').val( config.secure || 'disable' );
		$('#wpa_encrypt').val( config.wpa_encrypt || 'aes' );
		$('#secure').unbind('change').change(function (e) {
		  var secure = e.target.value;
		  if ( secure == 'disable' )
		  {
			$('#secure_cfg').hide();
            $('#secure_cfg2').hide();
		  }
		  else
		  {
			$('#secure_cfg').show();
            $('#secure_cfg2').show();
		  }
		}).trigger('change');
	}

    // ssid2
    if (securelist) {
        $("#secure2").empty();
        for (m in securelist) {
            $("#secure2").append("<option value='" + m + "'>" + $.i18n(m) + "</option>");
        }
        $('#secure2').val(config.secure2 || 'disable'); 
        $('#secure2').unbind('change').change(function (e) {
            var secure = e.target.value;
            if (secure == "disable") {
                $("#secure2_cfg").hide(); 
                $("#secure2_cfg2").hide();
            } else {
                $("#wpa_encrypt2").empty();
                $("#secure2_cfg").show();
                $("#secure2_cfg2").show();
                if (securelist[secure]) {
                    for (b in securelist[secure]) {
                        $("#wpa_encrypt2").append("<option value='" + b + "'>" + $.i18n(b) + "</option>");
                    }
                }
                $('#wpa_encrypt2').val(config.wpa_encrypt2 || 'aes');
            }
        }).trigger('change');
    } else {
        $('#secure2').val(config.secure2 || 'disable');
        $('#wpa_encrypt2').val(config.wpa_encrypt2 || 'aes');
        $('#secure2').unbind('change').change(function (e) {
            var secure = e.target.value;
            if (secure == 'disable') {
                $("#secure2_cfg").hide(); 
                $("#secure2_cfg2").hide();
            } else {
                $("#secure2_cfg").show();
                $("#secure2_cfg2").show();
            }
        }).trigger('change');
    }

    // ssid3
    if (securelist) {
        $("#secure3").empty();
        for (m in securelist) {
            $("#secure3").append("<option value='" + m + "'>" + $.i18n(m) + "</option>");
        }
        $('#secure3').val(config.secure3 || 'disable');
        $('#secure3').unbind('change').change(function (e) {
            var secure = e.target.value;
            if (secure == "disable") {
                $("#secure3_cfg").hide();
                $("#secure3_cfg2").hide();
            } else {
                $("#wpa_encrypt3").empty();
                $("#secure3_cfg").show();
                $("#secure3_cfg2").show();
                if (securelist[secure]) {
                    for (b in securelist[secure]) {
                        $("#wpa_encrypt3").append("<option value='" + b + "'>" + $.i18n(b) + "</option>");
                    }
                }
                $('#wpa_encrypt3').val(config.wpa_encrypt3 || 'aes');
            }
        }).trigger('change');
    } else {
        $('#secure3').val(config.secure3 || 'disable');
        $('#wpa_encrypt3').val(config.wpa_encrypt3 || 'aes');
        $('#secure3').unbind('change').change(function (e) {
            var secure = e.target.value;
            if (secure == 'disable') {
                $("#secure3_cfg").hide();
                $("#secure3_cfg2").hide();
            } else {
                $("#secure3_cfg").show();
                $("#secure3_cfg2").show();
            }
        }).trigger('change');
    }
    $('#hidden').prop( 'checked', config.peermode=="hidden" );
    $('#hidden2').prop('checked', config.peermode2 == "hidden");
    $('#hidden3').prop('checked', config.peermode3 == "hidden");
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
  config.peermac2 = '';
  config.peermac3 = '';
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

  if ( $('#lock2').prop('checked') )
  {
    config.peermac2 = $('#peermac2').val();
    if ( check.mac( config.peermac2, true ) == false )
    {
        page.alert( {
            message: $.i18n('Peer BSSID')+" "+$.i18n('must be a valid MAC address'),
            callback: function()
            {
                $('#peermac2').select();
            }
        } );
        return;
    }
  }

  if ( $('#lock3').prop('checked') )
  {
    config.peermac3 = $('#peermac3').val();
    if ( check.mac( config.peermac3, true ) == false )
    {
        page.alert( {
            message: $.i18n('Peer BSSID')+" "+$.i18n('must be a valid MAC address'),
            callback: function()
            {
                $('#peermac3').select();
            }
        } );
        return;
    }
  }

  config.secure = $('#secure').val();
  if ( config.secure != 'disable')
  {
      config.wpa_encrypt = $('#wpa_encrypt').val()||"aes";
      config.wpa_key = $('#wpa_key').val();
      if ( !config.wpa_key )
      {
          page.alert( { message: $.i18n('Password')+" "+$.i18n('Can not be empty') } );
          return;
      }
  }
  config.secure2 = $('#secure2').val();
  if ( config.secure2 != 'disable')
  {
      config.wpa_encrypt2 = $('#wpa_encrypt2').val()||"aes";
      config.wpa_key2 = $('#wpa_key2').val();
      if ( !config.wpa_key2 )
      {
          page.alert( { message: $.i18n('Password')+" "+$.i18n('Can not be empty') } );
          return;
      }
  }
  config.secure3 = $('#secure3').val();
  if ( config.secure3 != 'disable')
  {
      config.wpa_encrypt3 = $('#wpa_encrypt3').val()||"aes";
      config.wpa_key3 = $('#wpa_key3').val();
      if ( !config.wpa_key3 )
      {
          page.alert( { message: $.i18n('Password')+" "+$.i18n('Can not be empty') } );
          return;
      }
  }
  config.nossid = boole2able( $('#nossid').prop('checked') );
  delete config.peermode;
  delete config.peermode2;
  delete config.peermode3;
  if ( $('#hidden').prop('checked') )
  {
      config.peermode = "hidden";
  }
  if ( $('#hidden2').prop('checked') )
  {
      config.peermode2 = "hidden";
  }
  if ( $('#hidden3').prop('checked') )
  {
      config.peermode3 = "hidden";
  }
  if ( ocompare( config, copy ) )
  {
      page.alert( { message: $.i18n('Settings unchanged') } );
      return;
  }
  page.confirm( { message: $.i18n('The WISP connection will be disconnected because of the change of configuration') } ).then( function(result){
    if (!result) return location.reload();
    if ( result )
    {
      he.exec( [ object+"="+JSON.stringify(config)] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        config_load();
      });
    }
  });
}


/* init */
page.password('password', 'password-icon' );
page.password('wpa_key', 'wpa_key-icon' );
page.password('wpa_key2', 'wpa_key2-icon' );
page.password('wpa_key3', 'wpa_key3-icon' );
$.i18n().load( page.lang('wisp') ).then( function () {
    /* init the language */
    $.i18n().locale = lang; $('body').i18n();

    // jqgrid
    jqtable.create( '#aplist-grid-table', '#aplist-grid-pager',
        {
            multiselect: false,
            caption: $.i18n('AP List'),
            toolbar: [true, "top"],
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
                { name: 'channel', width: 70 },
                {
                    name: 'signal', width: 70,
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
                { name: 'mac', width: 160 },
                {
                    name: 'secure', width: 190,
                    formatter: function ( cellvalue )
                    {
                        var safeValue = cellvalue;
                        if (!safeValue || typeof safeValue === 'object') 
                        {
                            safeValue = ""; // 强制设置为空字符串，避免出现 [object Object]
                        }
                        // 如果 safeValue 为空，则不调用 $.i18n 否则可能返回键名本身
                        var displayText = (safeValue === "") ? "" : $.i18n(safeValue);
                        
                        return '<span data-secure="' + safeValue + '">' + displayText + '</span>';
                    },
                    unformat: function (cellvalue, options, cell)
                    {
                        return $(cell).children('span').data('secure');
                    }
                },
                {
                    name: 'wpa_encrypt', width: 100,
                    formatter: function (cellvalue,rowObject)
                    {
                        var secureMode = rowObject.secure || '';
                        var safeValue = cellvalue;

                        if (secureMode === 'disable' || !safeValue || typeof safeValue === 'object') 
                        {
                            safeValue = ""; // 强制设置为空字符串，避免出现 [object Object]
                        }
                        // 如果 safeValue 为空，则不调用 $.i18n 否则可能返回键名本身
                        var displayText = (safeValue === "") ? "" : $.i18n(safeValue);
                        
                        return '<span data-wpa_encrypt="' + safeValue + '">' + displayText + '</span>';  
                        
                        //return '<span data-wpa_encrypt="' + cellvalue + '">' + $.i18n(cellvalue) + '</span>';
                    },
                    unformat: function (cellvalue, options, cell)
                    {
                        return $(cell).children('span').data('wpa_encrypt');
                    }
                }
            ],
            pager: '#aplist-grid-pager',
            rowNum: 10,
            viewrecords: true,

            pgbuttons: true,
            pagerpos:'center',
            pginput:true,
            
        }
    ).jqGrid( 
          'navGrid', '#aplist-grid-pager',
          $.extend(true, {}, jqtable.navOptions, { add: false, edit: false, search: false, refresh: false, del: false, view: false } ),
          {},{},{},{},{},
    );

    var $toolbar = $("#t_" + "aplist-grid-table");
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
        $('#aplist-grid-table').jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
    });

    /* load the configure */
    status_load();
    config_load();

    // 监听三个 lock 开关的变化
    $('#lock, #lock2, #lock3').on('change', function() {
        var suffix = this.id.replace('lock', '');
        var $macInput = $('#peermac' + suffix);

        if ($(this).prop('checked')) {
            var backup = $macInput.data('backup-mac') || '';
            if (backup) {
                $macInput.val(backup);
            }
        } 
    });

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

    // scanning 修复scan赋值bug
    $('.apscan-btn').unbind(ace.click_event).on(ace.click_event, function () {
        currentActiveRow = $(this).data('row-index'); 

        he.exec( [ object+'.aplist' ], $.i18n("Scanning") ).then( function(v){
            var list = v[0];
            var rows = json2array( list, {}, "mac" );

            apDataStore = {};

            // 将原始数据存储到全局变量中
            if (Array.isArray(list)) {
                list.forEach(function(ap) {
                    if (ap && ap.mac) {
                        // 存储原始数据
                        apDataStore[ap.mac] = {
                            secure: ap.secure || '',
                            wpa_encrypt: ap.wpa_encrypt || '',
                        };
                    }
                });
            }

            // 给表格赋值
            $('#aplist-grid-table').jqGrid('clearGridData').jqGrid('setGridParam', {
                data: rows
            }).trigger('reloadGrid');
            $('#aplist-modal').modal('show');
        });
    });

    $('#rescan').unbind(ace.click_event).on(ace.click_event, function () {
        currentActiveRow = $(this).data('row-index'); 

        he.exec( [ object+'.aplist' ], $.i18n("Scanning") ).then( function(v){
            var list = v[0];
            var rows = json2array( list, {}, "mac" );

            apDataStore = {};

            // 将原始数据存储到全局变量中
            if (Array.isArray(list)) {
                list.forEach(function(ap) {
                    if (ap && ap.mac) {
                        // 存储原始数据
                        apDataStore[ap.mac] = {
                            secure: ap.secure || '',
                            wpa_encrypt: ap.wpa_encrypt || '',
                        };
                    }
                });
            }

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

/* 全局变量存储AP列表原始数据 */
 var apDataStore = {};

function isOptionExist(val, selSuffix) {
    return $("#secure" + selSuffix + " option[value='" + val + "']").length > 0;
}

/* 必须在全局中才可以被调用到 */
function ap_select(rowId) {
    // 隐藏表格
    $('#aplist-modal').modal('hide');

    // 获取行数据
    var ap = $('#aplist-grid-table').jqGrid('getRowData', rowId);
    var mac = ap.mac;
    
    // 从全局存储中获取原始数据
    var originalData = apDataStore[mac];
    
    var suffix = (currentActiveRow === 1) ? '' : currentActiveRow;

    // 将选中的数据填到对应的输入框中
    $('#peer' + suffix).val(ap.ssid);
    var $macInput = $('#peermac' + suffix);
    $macInput.val(mac).data('backup-mac', mac); 

    $('#lock' + suffix).prop('checked', false).trigger('change');

    // 处理secure安全模式的数据回退
    var rawSecure = ap.secure || 'disable';
    var targetSecure = 'disable';

    // 精确匹配
    if (isOptionExist(rawSecure,suffix)) {
        targetSecure = rawSecure;
    } 
    else {
        // 回退匹配逻辑
        if (rawSecure.indexOf('wpa3psk') !== -1 || rawSecure.indexOf('wpa2pskwpa3psk') !== -1) {
            if (isOptionExist('wpa2pskwpa3psk',suffix)){
                targetSecure = 'wpa2pskwpa3psk';
            }
            else if (isOptionExist('wpa2psk',suffix)){
                targetSecure = 'wpa2psk';
            }
            else if (isOptionExist('wpapsk',suffix)){
                targetSecure = 'wpapsk';
            }
            else{
                targetSecure = 'disable';
            }
        } 
        else if (rawSecure.indexOf('wpa2psk') !== -1 || rawSecure.indexOf('wpapskwpa2psk') !== -1) {
            if (isOptionExist('wpapskwpa2psk',suffix)){
                targetSecure = 'wpapskwpa2psk';
            }
            else if (isOptionExist('wpa2psk',suffix)){
                targetSecure = 'wpa2psk';
            }
            else if (isOptionExist('wpapsk',suffix)){
                targetSecure = 'wpapsk';
            }
            else targetSecure = 'disable';
        }
        else {
            // 其他无法识别的情况
            targetSecure = 'disable';
        }
    }
    // 设置选择框的值
    $('#secure' + suffix).val(targetSecure).trigger('change');
    
    // 处理WPA加密模式的映射
    var wpaEncryptValue = originalData ? originalData.wpa_encrypt : ap.wpa_encrypt;
    var cleanValue = (wpaEncryptValue || '').toString().trim().toLowerCase();
    var finalValue = 'tkipaes'; // 默认值
    
    if (cleanValue === 'tkip') {
        finalValue = 'tkip';
    } else if (cleanValue === 'aes') {
        finalValue = 'aes';
    } else if (cleanValue === 'tkipaes') {
        finalValue = 'tkipaes';
    }
    // auto 和 tkipaes 都使用默认值 tkipaes
    
    // 设置选择框的值
    $('#wpa_encrypt').val(finalValue);
    
    // 修复i18n翻译问题：直接设置选中选项的文本
    setTimeout(function() {
        var $targetEl = $('#wpa_encrypt' + suffix);
        var $option = $('#wpa_encrypt option[value="' + finalValue + '"]');
    
    if ($option.length > 0) {
        // 如果存在，正常赋值并触发更新
        $targetEl.val(finalValue).trigger('change');
    } else {
        // 如果扫描到的值不存在 比如该模式下不支持TKIP，
        // 则尝试选择默认的 'tkipaes' 或第一个可用选项
        var fallback = $targetEl.find('option[value="tkipaes"]').length > 0 ? 'tkipaes' : $targetEl.find('option:first').val();
        $targetEl.val(fallback).trigger('change');
    }
    }, 50);
    
    // 触发必要的事件
    $('#hidden' + suffix).prop('checked', false).trigger('change');
    $('#radio').val('');
    $('#channel').val('');
}
  
