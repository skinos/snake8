
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
    }
    /* dhcpsv4 */
    if ( config.dhcps )
    {
        var dhcps = config.dhcps;
        $('#dhcps').prop('checked', able2boole(dhcps.status) );
        $('#startip').val(dhcps.startip || '');
        $('#endip').val(dhcps.endip || '');
        $('#lease').val(dhcps.lease || '');
        $('#gw').val(dhcps.gw || '');
        $('#dns').val(dhcps.dns || '');
        $('#dns2').val(dhcps.dns2 || '');
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
        $('#dns2').val(dhcps.dns2 || '');
        $('#options').val(dhcps.options||"");
    }
    /* ipv6 */
    if ( config.manual && config.manual.addr )
    {
        $('#addr').val(config.manual.addr || '');
        $('#prefix').val(config.manual.prefix || '');
    }
    if ( config.method )
    {
        $('#ipv6_cfg').show();
        $('#method').val( config.method||"disable" );
        $('#method').unbind('change').change(function (e) {
          var type = e.target.value;
          switch (type)
          {
            case 'disable':
              $('#manual_cfg').hide();
              $('#dhcpsv6_cfg').hide();
              break;
            case 'relay':
              $('#manual_cfg').hide();
              $('#dhcpsv6_cfg').hide();
              break;
            case 'manual':
              $('#manual_cfg').show();
              break;
          }
        }).trigger('change');
          /* dhcpsv6 */
        if ( config.dhcpsv6 )
        {
            var dhcps = config.dhcpsv6;
            $('#dhcpsv6').prop('checked', able2boole(dhcps.status) );
            $('#startaddr').val(dhcps.startaddr || '');
            $('#endaddr').val(dhcps.endaddr || '');
            $('#leasetime').val(dhcps.leasetime || '');
            $('#hop').val(dhcps.hop || '');
            $('#resolve').val(dhcps.resolve || '');
            $('#resolve2').val(dhcps.resolve2 || '');
        }
        $('#dhcpsv6').unbind('change').change(function (){
            if ($(this).prop('checked'))
            {
                $('#dhcpsv6_cfg').show();
            }
            else
            {
                $('#dhcpsv6_cfg').hide();
            }
        }).trigger('change');
    }
  });
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
    if ( !config.static )
    {
        config.static = {};
    }
    config.mode = "static";
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
        dhcps.options = $('#options').val();
    }
    /* IPV6 addr */
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
            /* IPV6 dhcps */
            if ( !config.dhcpsv6 )
            {
                config.dhcpsv6 = {};
            }
            var dhcps = config.dhcpsv6;
            dhcps.status = boole2able( $('#dhcpsv6').prop('checked') );
            if ( dhcps.status == "enable" )
            {
                dhcps.startaddr = $('#startaddr').val();
                if ( check.ipv6(dhcps.startaddr) == false )
                {
                    page.alert( { message: $.i18n('Start IPv6 Address')+" "+$.i18n('must be a valid IPv6 address') } );
                    return;
                }
                dhcps.endaddr = $('#endaddr').val();
                if ( check.ipv6(dhcps.endaddr) == false )
                {
                    page.alert( { message: $.i18n('End IPv6 Address')+" "+$.i18n('must be a valid IPv6 address') } );
                    return;
                }
                dhcps.prefix = $('#prefix').val();
                dhcps.leasetime = $('#leasetime').val();
                if ( dhcps.leasetime && check.number(dhcps.leasetime) == false )
                {
                    page.alert( { message: $.i18n('IPv6 Lease(Sec)')+" "+$.i18n('must be a valid number') } );
                    return;
                }
                dhcps.hop = $('#hop').val();
                if ( dhcps.hop && check.ipv6(dhcps.hop) == false )
                {
                    page.alert( { message: $.i18n('IPv6 Next Hop')+" "+$.i18n('must be a valid IPv6 address') } );
                    return;
                }
                dhcps.resolve = $('#resolve').val();
                if ( dhcps.resolve && check.ipv6(dhcps.resolve) == false )
                {
                    page.alert( { message: $.i18n('IPv6 DNS')+" "+$.i18n('must be a valid IPv6 address') } );
                    return;
                }
                dhcps.resolve2 = $('#resolve2').val();
                if ( dhcps.resolve2 && check.ipv6(dhcps.resolve2) == false )
                {
                    page.alert( { message: $.i18n('IPv6 DNS2')+" "+$.i18n('must be a valid IPv6 address') } );
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
            he.save( [ object+"="+JSON.stringify(config) ] ).then( function(){
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



