/* get the object */
var wan;
var object = "ifname@lan";
var index = page.param( 'object', location.hash );
if ( index )
{
    object = index;
}
/* load the configure on the input */
function config_load()
{
  he.load( [ object ] ).then( function(v){
    wan = v[0];
    if ( !wan )
    {
        return;
    }
    /* ipv4 */
    $('#mode').val( wan.mode || 'dhcpc' );
    if ( wan.dhcpc )
    {
        $('#static').prop('checked', able2boole(wan.dhcpc.static) );
    }
    if ( wan.static )
    {
      $('#ip').val(wan.static.ip);
      $('#mask').val(wan.static.mask);
      if ( wan.static.ip2 && wan.static.mask2 )
      {
          $('#ip2').val(wan.static.ip2 || '');
          $('#mask2').val(wan.static.mask2 || '');
          $('#ipmask2').prop('checked', true );
      }
      else
      {
          $('#ipmask2').prop('checked', false );
      }
      if ( wan.static.ip3 && wan.static.mask3 )
      {
          $('#ip3').val(wan.static.ip3 || '');
          $('#mask3').val(wan.static.mask3 || '');
          $('#ipmask3').prop('checked', true );
      }
      else
      {
          $('#ipmask3').prop('checked', false );
      }
      $('#gw').val(wan.static.gw);
      $('#dns').val(wan.static.dns);
      $('#dns2').val(wan.static.dns2);
    }
    $('#ipmask2').unbind('change').change(function (){
        if ($(this).prop('checked'))
        {
            $('#ipmask2_cfg').show();
        }
        else
        {
            $('#ipmask2_cfg').hide();
        }
    }).trigger('change');
    $('#ipmask3').unbind('change').change(function (){
        if ($(this).prop('checked'))
        {
            $('#ipmask3_cfg').show();
        }
        else
        {
            $('#ipmask3_cfg').hide();
        }
    }).trigger('change');

    $('#mode').unbind('change').change(function (e) {
      var type = e.target.value;
      switch (type)
      {
        case 'static':
          $('#static_cfg').show();
          $('#dhcpc_cfg').hide();
          $('#dhcps_cfg').show();
          $('#gateway_cfg').show();
          $('#customdns_cfg').hide();
          break;
        case 'dhcpc':
          $('#static_cfg').hide();
          $('#dhcpc_cfg').show();
          $('#dhcps_cfg').hide();
          $('#gateway_cfg').hide();
          $('#customdns_cfg').show();
          if ( wan.dhcpc )
          {
            $('#static').prop('checked', able2boole(wan.dhcpc.static) );
            $('#custom_dns').prop('checked', able2boole(wan.dhcpc.custom_dns));
            $('#cdns').val( wan.dhcpc.dns );
            $('#cdns2').val( wan.dhcpc.dns2 );
          }
          if ($('#static').prop('checked'))
          {
            $('#static_cfg').show();
          }
          else
          {
            $('#static_cfg').hide();
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
    $('#static').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#static_cfg').show();
      }
      else
      {
        $('#static_cfg').hide();
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
    /* dhcps */
    if ( wan.dhcps )
    {
        var dhcps = wan.dhcps;
        $('#dhcps').prop('checked', able2boole(dhcps.status) );
        $('#startip').val(dhcps.startip || '');
        $('#endip').val(dhcps.endip || '');
        $('#lease').val(dhcps.lease || '');
        $('#assign_gw').val(dhcps.gw || '');
        $('#assign_dns').val(dhcps.dns || '');
        $('#assign_dns2').val(dhcps.dns2 || '');
        $('#dhcps').unbind('change').change(function (){
            if ($(this).prop('checked'))
            {
                $('#pool_cfg').show();
            }
            else
            {
                $('#pool_cfg').hide();
            }
        }).trigger('change');
    }

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

  /* IPV4 addr */
  wan.mode = $('#mode').val();
  if ( wan.mode == "dhcpc" )
  {
    if ( !wan.dhcpc )
    {
      wan.dhcpc = {};
    }
    wan.dhcpc.static = boole2able( $('#static').prop('checked') );
    wan.dhcpc.custom_dns = boole2able( $('#custom_dns').prop('checked') );
    if ( wan.dhcpc.custom_dns == "enable" )
    {
      wan.dhcpc.dns = $('#cdns').val();
      if ( check.ip(wan.dhcpc.dns) == false )
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
  if ( wan.mode == "static" || ( wan.mode == "dhcpc" && wan.dhcpc.static == "enable" ) )
  {
    /* IPV4 addr */
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
    if ( $('#ipmask2').prop('checked') == true )
    {
        wan.static.ip2 = $('#ip2').val();
        if ( check.ip(wan.static.ip2) == false )
        {
            page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.mask2 = $('#mask2').val();
        if ( check.ip(wan.static.mask2) == false )
        {
            page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
    }
    else
    {
        wan.static.ip2 = "";
        wan.static.mask2 = "";
    }
    if ( $('#ipmask3').prop('checked') == true )
    {
        wan.static.ip3 = $('#ip3').val();
        if ( check.ip(wan.static.ip3) == false )
        {
            page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
        wan.static.mask3 = $('#mask3').val();
        if ( check.ip(wan.static.mask3) == false )
        {
            page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
            return;
        }
    }
    else
    {
        wan.static.ip3 = "";
        wan.static.mask3 = "";
    }
    /* DHCPS */
    if ( !wan.dhcps )
    {
        wan.dhcps = {};
    }
    if ( wan.mode == "dhcpc" )
    {
        wan.dhcps.status = "disable";
    }
    else
    {
        var dhcps = wan.dhcps;
        dhcps.status = boole2able( $('#dhcps').prop('checked') );
        if ( dhcps.status == "enable" )
        {
            dhcps.startip = $('#startip').val();
            if ( check.ip(dhcps.startip) == false )
            {
                page.alert( { message: $.i18n('Start IP Address')+" "+$.i18n('must be a valid IP address') } );
                return;
            }
            dhcps.endip = $('#endip').val();
            if ( check.ip(dhcps.endip) == false )
            {
                page.alert( { message: $.i18n('End IP Address')+" "+$.i18n('must be a valid IP address') } );
                return;
            }
            dhcps.mask = $('#mask').val();
            dhcps.lease = $('#lease').val();
            if ( dhcps.lease && check.number(dhcps.lease) == false )
            {
                page.alert( { message: $.i18n('Lease(Sec)')+" "+$.i18n('must be a valid number') } );
                return;
            }
            dhcps.gw = $('#gw').val();
            if ( dhcps.gw && check.ip(dhcps.gw) == false )
            {
                page.alert( { message: $.i18n('Assign Gateway')+" "+$.i18n('must be a valid IP address') } );
                return;
            }
            dhcps.dns = $('#dns').val();
            if ( dhcps.dns && check.ip(dhcps.dns) == false )
            {
                page.alert( { message: $.i18n('Assign DNS')+" "+$.i18n('must be a valid IP address') } );
                return;
            }
            dhcps.dns2 = $('#dns2').val();
            if ( dhcps.dns2 && check.ip(dhcps.dns2) == false )
            {
                page.alert( { message: $.i18n('Assign DNS2')+" "+$.i18n('must be a valid IP address') } );
                return;
            }
        }
    }
  }

  // 校正DHCP池地址
  if ( wan.static.ip != wancopy.static.ip 
      && wan.dhcps.startip == wancopy.dhcps.startip
      && wan.dhcps.endip == wancopy.dhcps.endip )
  {
      var arr = ipadd2array( wan.static.ip, wan.static.mask );
      // 起始IP
      arr[3] = 2;
      wan.dhcps.startip = arr.join('.');
      // 结束IP
      arr[3] = 250;
      wan.dhcps.endip = arr.join('.');
  }
  
  page.confirm( { message: $.i18n('The system will restart because of the change of configuration') } ).then( function(result){
      if ( result )
      {
          he.save( [ object+"="+JSON.stringify(wan)] ).then( function(){
              page.confirm( { message: $.i18n('Need to restart the system') } ).then( function(result){
                  if ( result )
                  {
                      he.reboot( { title: $.i18n('Restarting to apply...'), hint:$.i18n('Make sure that the device is reconnected') } );
                  }
                  else
                  {
                      lan_load();
                  }
              });
          });
      }
  });
  
}



/* init */
$.i18n().load( page.lang('lan') ).then( function () {
    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    /* load the configure */
    config_load();

    /* bind the refresh */
    $('#refresh').on(ace.click_event, function () {
        location.reload();
    });
    /* bind the apply */
    $('#apply').on(ace.click_event, function () {
        config_save();
    });
});

