
/* get the object */
var config;
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
    config = v[0];
    if ( !config )
    {
        return;
    }
    /* ipv4 */
    $('#mode').val( config.mode || 'dhcpc' );
    if ( config.dhcpc )
    {
        $('#static').prop('checked', able2boole(config.dhcpc.static) );
    }
    if ( config.static )
    {
      $('#ip').val(config.static.ip || '');
      $('#mask').val(config.static.mask || '');
      if ( config.static.ip2 && config.static.mask2 )
      {
          $('#ip2').val(config.static.ip2 || '');
          $('#mask2').val(config.static.mask2 || '');
          $('#ipmask2').prop('checked', true );
      }
      else
      {
          $('#ipmask2').prop('checked', false );
      }
      if ( config.static.ip3 && config.static.mask3 )
      {
          $('#ip3').val(config.static.ip3 || '');
          $('#mask3').val(config.static.mask3 || '');
          $('#ipmask3').prop('checked', true );
      }
      else
      {
          $('#ipmask3').prop('checked', false );
      }
      $('#gw').val(config.static.gw);
      $('#dns').val(config.static.dns);
      $('#dns2').val(config.static.dns2);
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
          if ( config.dhcpc )
          {
            $('#static').prop('checked', able2boole(config.dhcpc.static) );
            $('#custom_dns').prop('checked', able2boole(config.dhcpc.custom_dns));
            $('#cdns').val( config.dhcpc.dns );
            $('#cdns2').val( config.dhcpc.dns2 );
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
    /* dhcpsv4 */
    if ( config.dhcps )
    {
        var dhcps = config.dhcps;
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
    if ( config == null )
    {
        return;
    }
    var copy = JSON.parse(JSON.stringify(config));

	/* IPV4 addr */
	config.mode = $('#mode').val();
	if ( config.mode == "dhcpc" )
	{
		if ( !config.dhcpc )
		{
			config.dhcpc = {};
		}
		config.dhcpc.static = boole2able( $('#static').prop('checked') );
		config.dhcpc.custom_dns = boole2able( $('#custom_dns').prop('checked') );
		if ( config.dhcpc.custom_dns == "enable" )
		{
			config.dhcpc.dns = $('#cdns').val();
			if ( check.ip(config.dhcpc.dns) == false )
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
	if ( config.mode == "static" || ( config.mode == "dhcpc" && config.dhcpc.static == "enable" ) )
	{
		/* IPV4 addr */
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
		if ( $('#ipmask2').prop('checked') == true )
		{
		    config.static.ip2 = $('#ip2').val();
		    if ( check.ip(config.static.ip2) == false )
		    {
		        page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
		        return;
		    }
		    config.static.mask2 = $('#mask2').val();
		    if ( check.ip(config.static.mask2) == false )
		    {
		        page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
		        return;
		    }
		}
		else
		{
		    config.static.ip2 = "";
		    config.static.mask2 = "";
		}
		if ( $('#ipmask3').prop('checked') == true )
		{
		    config.static.ip3 = $('#ip3').val();
		    if ( check.ip(config.static.ip3) == false )
		    {
		        page.alert( { message: $.i18n('IPv4 Address')+" "+$.i18n('must be a valid IP address') } );
		        return;
		    }
		    config.static.mask3 = $('#mask3').val();
		    if ( check.ip(config.static.mask3) == false )
		    {
		        page.alert( { message: $.i18n('Subnet Mask')+" "+$.i18n('must be a valid IP address') } );
		        return;
		    }
		}
		else
		{
		    config.static.ip3 = "";
		    config.static.mask3 = "";
		}
		/* IPV4 dhcps */
		if ( !config.dhcps )
		{
		    config.dhcps = {};
		}
		if ( config.mode == "dhcpc" )
		{
		    config.dhcps.status = "disable";
		}
		else
		{
		    var dhcps = config.dhcps;
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

	if ( ocompare( config, copy ) )
	{
	  page.alert( { message: $.i18n('Settings unchanged') } );
	  return;
	}

    // 校正DHCP池地址
    if ( config.static.ip != copy.static.ip 
        && config.dhcps.startip == copy.dhcps.startip
        && config.dhcps.endip == copy.dhcps.endip )
    {
        var arr = ipadd2array( config.static.ip, config.static.mask );
        // 起始IP
        arr[3] = 2;
        config.dhcps.startip = arr.join('.');
        // 结束IP
        arr[3] = 250;
        config.dhcps.endip = arr.join('.');
    }

    page.confirm( { message: $.i18n('The system will restart because of the change of configuration') } ).then( function(result){
        if ( result )
        {
            he.exec( [ object+"="+JSON.stringify(config) ] ).then( function(){
                page.confirm( { message: $.i18n('Need to restart the system') } ).then( function(result){
                    if ( result )
                    {
                        he.reboot( { title: $.i18n('Restarting to apply...'), hint:$.i18n('Make sure that the device is reconnected') } );
                    }
                    else
                    {
                        config_load();
                    }
                });
            });
        }
    });
}



/* init */
$.i18n().load( page.lang('lan') ).then( function () {
	/* init the language */
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



