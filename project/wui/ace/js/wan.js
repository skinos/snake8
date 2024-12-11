
/* get the object */
var wan;
var wans;
var object = "ifname@wan";
var index = page.param( 'object', location.hash );
if ( index )
{
    object = index;
}

/* load the status */
function status_load()
{
  he.bkload( [ object+".status" ] ).then( function(v){
      wans = v[0];
      var info = wans;
      var id = "#wan";
      /* status end btn */
      if ( info.status )
      {
          $(id).show();
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
      if ( !info.gw || info.gw == "0.0.0.0" )
      {
          $(id+"_gateway").text( info.dstip||' ' );
      }
      else
      {
          $(id+"_gateway").text( info.gw||' ' );
      }
      $(id+"_dns").text( info.dns||' ' );
      $(id+"_dns2").text( info.dns2||' ' );
      $(id+"_mac").text( info.mac||' ' );
      /* network */
      $(id+"_ip").text( info.ip||' ' );
	  if ( info.delay )
	  {
		  if ( info.delay == "failed" || info.delay == "block" )
		  {
			  $(id+"_delay").text( $.i18n(info.delay) );
		  }
		  else
		  {
			  $(id+"_delay").text( $.i18n("Delay")+":"+info.delay );
		  }
	  }
	  else
	  {
		  $(id+"_delay").text( "" );
	  }
      $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
      $(id+"_livetime").text( info.livetime||' ' );
  })
}

/* load the configure on the input */
function config_load()
{
  he.load( [ object ] ).then( function(v){
    wan = v[0];
    /* status */
    if ( wan.status && wan.status == "disable" )
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
    $('#mode').val( wan.mode || 'dhcpc' );
    if ( wan.pppoec )
    {
      $('#username').val(wan.pppoec.username);
      $('#password').val(wan.pppoec.password);
      $('#lcp_echo_interval').val(wan.pppoec.lcp_echo_interval);
      $('#lcp_echo_failure').val(wan.pppoec.lcp_echo_failure);
    }
    if ( wan.dhcpc )
    {
        $('#routeopt').prop('checked', able2boole(wan.dhcpc.routeopt) );
    }
    if ( wan.static )
    {
      $('#ip').val(wan.static.ip);
      $('#mask').val(wan.static.mask);
      $('#gw').val(wan.static.gw);
      $('#dns').val(wan.static.dns);
      $('#dns2').val(wan.static.dns2);
    }
    $('#masq').prop('checked', able2boole(wan.masq));
    $('#mtu').val(wan.mtu||"");
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
          if ( wan.dhcpc )
          {
            $('#custom_dns').prop('checked', able2boole(wan.dhcpc.custom_dns));
            $('#cdns').val( wan.dhcpc.dns );
            $('#cdns2').val( wan.dhcpc.dns2 );
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
          if ( wan.pppoec )
          {
            $('#custom_dns').prop('checked', able2boole(wan.pppoec.custom_dns));
            $('#cdns').val( wan.pppoec.dns );
            $('#cdns2').val( wan.pppoec.dns2 );
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
    if ( wan.method )
    {
        $('#ipv6_cfg').show();
        $('#method').val(wan.method);
        if ( wan.manual )
        {
          if ( wan.manual.addr )
          {
            $('#addr').val(wan.manual.addr);
            $('#prefix').val(wan.manual.prefix);
          }
          $('#hop').val(wan.manual.hop);
          $('#resolve').val(wan.manual.resolve);
          $('#resolve2').val(wan.manual.resolve2);
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
    if ( wan.keeplive )
    {
        $('#keeplive').val( wan.keeplive.type || 'disable');
        if ( wan.keeplive.icmp )
        {
          if (wan.keeplive.icmp.dest)
          {
            $('#icmp_test').val(wan.keeplive.icmp.dest.test)
            $('#icmp_test2').val(wan.keeplive.icmp.dest.test2)
            $('#icmp_test3').val(wan.keeplive.icmp.dest.test3)
          }
          $('#icmp_timeout').val(wan.keeplive.icmp.timeout);
          $('#icmp_failed').val(wan.keeplive.icmp.failed);
          $('#icmp_interval').val(wan.keeplive.icmp.interval);
        }
        if ( wan.keeplive.dns )
        {
          $('#dns_timeout').val(wan.keeplive.dns.timeout);
          $('#dns_failed').val(wan.keeplive.dns.failed);
          $('#dns_interval').val(wan.keeplive.dns.interval);
        }
        if ( wan.keeplive.recv )
        {
          $('#recv_timeout').val(wan.keeplive.recv.timeout);
          $('#recv_failed').val(wan.keeplive.recv.failed);
          $('#recv_packets').val(wan.keeplive.recv.packets);
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

  })
}

/* save the configure */
function config_save()
{
  if ( wan == null )
  {
    return;
  }
  var wancopy = JSON.parse(JSON.stringify(wan));;

  /* status */
  if ( $('#status').prop('checked') == false )
  {
      wan.status = "disable";
  }
  else
  {
      if ( wan.status && wan.status != "enable" )
      {
        wan.status = "enable";
      }
      /* IPV4 */
      wan.mode = $('#mode').val();
      if ( wan.mode == "pppoec" )
      {
        if ( !wan.pppoec )
        {
          wan.pppoec = {};
        }
        wan.pppoec.custom_dns = boole2able( $('#custom_dns').prop('checked') );
        if ( wan.pppoec.custom_dns == "enable" )
        {
          wan.pppoec.dns = $('#cdns').val();
          if ( wan.pppoec.dns && check.ip(wan.pppoec.dns) == false )
          {
              page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
          wan.pppoec.dns2 = $('#cdns2').val();
          if ( wan.pppoec.dns2 && check.ip(wan.pppoec.dns2) == false )
          {
              page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
        }
        wan.pppoec.username = $('#username').val();
        wan.pppoec.password = $('#password').val();
        wan.pppoec.lcp_echo_interval = $('#lcp_echo_interval').val();
        if ( wan.pppoec.lcp_echo_interval && check.number(wan.pppoec.lcp_echo_interval) == false )
        {
            page.alert( { message: $.i18n('LCP Echo Interval')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.pppoec.lcp_echo_failure = $('#lcp_echo_failure').val();
        if ( wan.pppoec.lcp_echo_failure && check.number(wan.pppoec.lcp_echo_failure) == false )
        {
            page.alert( { message: $.i18n('LCP Echo Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      else if ( wan.mode == "dhcpc" )
      {
        if ( !wan.dhcpc )
        {
          wan.dhcpc = {};
        }
        wan.dhcpc.routeopt = boole2able( $('#routeopt').prop('checked') );
        wan.dhcpc.custom_dns = boole2able( $('#custom_dns').prop('checked') );
        if ( wan.dhcpc.custom_dns == "enable" )
        {
          wan.dhcpc.dns = $('#cdns').val();
          if ( wan.dhcpc.dns && check.ip(wan.dhcpc.dns) == false )
          {
              page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
          wan.dhcpc.dns2 = $('#cdns2').val();
          if ( wan.dhcpc.dns2 && check.ip(wan.dhcpc.dns2) == false )
          {
              page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
              return;
          }
        }
      }
      else if ( wan.mode == "static" )
      {
        if ( !wan.static )
        {
            wan.static = {};
        }
        wan.static.ip = $('#ip').val();
        if ( check.ip(wan.static.ip) == false )
        {
            page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.mask = $('#mask').val();
        if ( check.ip(wan.static.mask) == false )
        {
            page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.gw = $('#gw').val();
        if ( wan.static.gw && check.ip(wan.static.gw) == false )
        {
            page.alert( { message: $.i18n('Gateway')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.dns = $('#dns').val();
        if ( wan.static.dns && check.ip(wan.static.dns) == false )
        {
            page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.dns2 = $('#dns2').val();
        if ( wan.static.dns2 && check.ip(wan.static.dns2) == false )
        {
            page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
      }
      wan.masq = boole2able( $('#masq').prop('checked') );
      wan.mtu = $('#mtu').val();
      if ( wan.mtu && check.number(wan.mtu) == false )
      {
         page.alert( { message: $.i18n('MTU')+" "+$.i18n('must be a valid number') } );
         return;
      }

      /* IPV6 */
      if ( wan.method )
      {
          wan.method = $('#method').val();
          if ( wan.method == "manual" )
          {
            if ( !wan.manual )
            {
                wan.manual = {};
            }
            wan.manual.addr = $('#addr').val();
            if ( check.ipv6(wan.manual.addr) == false )
            {
                page.alert( { message: $.i18n('IPv6 Address')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            wan.manual.prefix = $('#prefix').val();
            if ( wan.manual.prefix && ( wan.manual.prefix < 0 || wan.manual.prefix > 128 ) )
            {
                page.alert( { message: $.i18n('Subnet Prefix')+" "+$.i18n('must be a number(0-128)') } );
                return;
            }
            wan.manual.hop = $('#hop').val();
            if ( wan.manual.hop && check.ipv6(wan.manual.hop) == false )
            {
                page.alert( { message: $.i18n('Next Hop')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            wan.manual.resolve = $('#resolve').val();
            if ( wan.manual.resolve && check.ipv6(wan.manual.resolve) == false )
            {
                page.alert( { message: $.i18n('DNS')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
            wan.manual.resolve2 = $('#resolve2').val();
            if ( wan.manual.resolve2 && check.ipv6(wan.manual.resolve2) == false )
            {
                page.alert( { message: $.i18n('DNS2')+" "+$.i18n('must be a valid IPv6 address') } );
                return;
            }
          }
      }
      /* Keeplive */
      if ( !wan.keeplive )
      {
        wan.keeplive = {};
      }
      wan.keeplive.type = $('#keeplive').val();
      if ( wan.keeplive.type == "icmp" )
      {
        if ( !wan.keeplive.icmp )
        {
          wan.keeplive.icmp = {};
        }
        if ( !wan.keeplive.icmp.dest )
        {
          wan.keeplive.icmp.dest = {};
        }
        wan.keeplive.icmp.dest.test = $('#icmp_test').val();
        if ( !wan.keeplive.icmp.dest.test )
        {
            page.alert( { message: $.i18n('Test Address')+" "+$.i18n('Can not be empty') } );
            return;
        }
        wan.keeplive.icmp.dest.test2 = $('#icmp_test2').val();
        wan.keeplive.icmp.dest.test3 = $('#icmp_test3').val();
        wan.keeplive.icmp.timeout = $('#icmp_timeout').val();
        if ( check.number(wan.keeplive.icmp.timeout) == false )
        {
            page.alert( { message: $.i18n('Each Query Timeout(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.icmp.failed = $('#icmp_failed').val();
        if ( check.number(wan.keeplive.icmp.failed) == false )
        {
            page.alert( { message: $.i18n('Test Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.icmp.interval = $('#icmp_interval').val();
        if ( check.number(wan.keeplive.icmp.interval) == false )
        {
            page.alert( { message: $.i18n('Test Interval(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      if ( wan.keeplive.type == "dns" || wan.keeplive.type == "auto" )
      {
        if ( !wan.keeplive.dns )
        {
          wan.keeplive.dns = {};
        }
        wan.keeplive.dns.timeout = $('#dns_timeout').val();
        if ( check.number(wan.keeplive.dns.timeout) == false )
        {
            page.alert( { message: $.i18n('Each Query Timeout(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.dns.failed = $('#dns_failed').val();
        if ( check.number(wan.keeplive.dns.failed) == false )
        {
            page.alert( { message: $.i18n('Query Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.dns.interval = $('#dns_interval').val();
        if ( check.number(wan.keeplive.dns.interval) == false )
        {
            page.alert( { message: $.i18n('Query Interval(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
      if ( wan.keeplive.type == "recv" || wan.keeplive.type == "auto" )
      {
        if ( !wan.keeplive.recv )
        {
          wan.keeplive.recv = {};
        }
        wan.keeplive.recv.timeout = $('#recv_timeout').val();
        if ( check.number(wan.keeplive.recv.timeout) == false )
        {
            page.alert( { message: $.i18n('Count Duration(sec)')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.recv.failed = $('#recv_failed').val();
        if ( check.number(wan.keeplive.recv.failed) == false )
        {
            page.alert( { message: $.i18n('Count Times')+" "+$.i18n('must be a valid number') } );
            return;
        }
        wan.keeplive.recv.packets = $('#recv_packets').val();
        if ( check.number(wan.keeplive.recv.packets) == false )
        {
            page.alert( { message: $.i18n('Received packets')+" "+$.i18n('must be a valid number') } );
            return;
        }
      }
  }

  if ( ocompare( wan, wancopy ) )
  {
      page.alert( { message: $.i18n('Settings unchanged') } );
      return;
  }
  page.confirm( { message: $.i18n('The WAN connecttion will be disconneted because of the change of configuration') } ).then( function(result){
    if ( result )
    {
      he.save( [ object+"="+JSON.stringify(wan)] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        config_load();
      });
    }
  });
}


/* init */
page.password('password', 'password-icon' );
$.i18n().load( page.lang('wan') ).then( function () {
    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    /* load the configure */
    status_load();
    config_load();

    $('#wan_btn').on(ace.click_event, function () {
        if ( wans.status == "down" )
        {
            he.exec( [ object+'.setup' ] ).then( function(result){status_load();} );
        }
        else
        {
            he.exec( [ object+'.shut' ] ).then( function(result){status_load();} );
        }
    });

    // 设置定时器
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

