
var date;
var dates;
var ntps;
var wuimenu = window.wui.menu;
var timezones =  [
	{label: $.i18n('UTC-12(IDL- International Date Line)'),value: '-12'},
	{label: $.i18n('UTC-11(MIT - Midway Island Standard Time)'),value: '-11'},
	{label: $.i18n('UTC-10(HST - Hawaii- Aleutian Standard Time)'),value: '-10'},
	{label: $.i18n('UTC-9:30(MSIT - Marquesas Islands Standard Time)'),value: '-9:30'},
	{label: $.i18n('UTC-9(AKST - Alaska Standard Time)'),value: '-9'},
	{label: $.i18n('UTC-8(PSTA - Pacific Ocean Standard Time A)'),value: '-8'},
	{label: $.i18n('UTC-7(MST - North American mountains Standard Time)'),value: '-7'}, 
	{label: $.i18n('UTC-6(CST - Central North America Standard Time)'),value: '-6'}, 
	{label: $.i18n('UTC-5(EST - Eastern North America Standard Time)'),value: '-5'}, 
	{label: $.i18n('UTC-4(AST - Atlantic Ocean Standard Time)'),value: '-4'}, 
	{label: $.i18n('UTC-3:30(NST - Newfoundland Standard Time)'),value: '-3:30'}, 
	{label: $.i18n('UTC-3(SAT -  South America Standard Time)'),value: '-3'}, 
	{label: $.i18n('UTC-2(BRT - Brazil Time)'),value: '-2'}, 
	{label: $.i18n('UTC-1(CVT - Cape Verde Standard Time)'),value: '-1'}, 
	{label: $.i18n('UTC(WET - Western European Time Zone, GMT)'),value: '0'}, 
	{label: $.i18n('UTC+1(CET - Mid European Time Zone)'),value: '1'}, 
	{label: $.i18n('UTC+2(EET - Eastern Europe Time Zone)'),value: '2'}, 
	{label: $.i18n('UTC+3(MSK - Moscow Time Zone)'),value: '3'}, 
	{label: $.i18n('UTC+3:30(IRT - Iran Time Zone)'),value: '3:30'}, 
	{label: $.i18n('UTC+4(META - Middle East Time Zone A)'),value: '4'}, 
	{label: $.i18n('UTC+4:30(AFT- Afghanistan Standard Time)'),value: '4:30'}, 
	{label: $.i18n('UTC+5(METB - Middle East Time Zone B)'),value: '5'}, 
	{label: $.i18n('UTC+5:30(IDT - India Standard Time)'),value: '5:30'}, 
	{label: $.i18n('UTC+45(NPT - Nepal Standard Time)'),value: '5:45'}, 
	{label: $.i18n('UTC+6(BHT - Bangladesh Standard Time)'),value: '6'}, 
	{label: $.i18n('UTC+6:30(MRT - Burma Standard Time)'),value: '6:30'}, 
	{label: $.i18n('UTC+7(MST - Thailand Bangkok Standard Time)'),value: '7'}, 
	{label: $.i18n('UTC+8(EAT - China Standard Time(BJT)'),value: '8'}, 
	{label: $.i18n('UTC+8:30(KRT- Korea Republi Standard Time)'),value: '8:30'}, 
	{label: $.i18n('UTC+9(FET- Far East Standard Time)'),value: '9'}, 
	{label: $.i18n('UTC+9:30(ACST - Central Australia Standard Time)'),value: '9:30'}, 
	{label: $.i18n('UTC+10(ACST - Eastern Australia Standard Time)'),value: '10'}, 
	{label: $.i18n('UTC+10:30 (FAST - Far eastern Australia Standard Time)'),value: '10:30'}, 
	{label: $.i18n('UTC+11( VTT - Vanuatu Standard Time)'),value: '11'}, 
	{label: $.i18n('UTC+11:30(NFT - Norfolk Island Standard Time)'),value: '11:30'}, 
	{label: $.i18n('UTC+12(PSTB - Pacific Ocean Standard Time B)'),value: '12'}, 
	{label: $.i18n('UTC+12:45(CIT - Chatham Islands Standard Time)'),value: '12:45'}, 
	{label: $.i18n('UTC+13(PSTC - Pacific Ocean Standard Time C)'),value: '13'}, 
	{label: $.i18n('UTC+14(PSTD - Pacific Ocean Standard Time D)'),value: '14'}
];

/* load the configure on the input */
function date_load()
{
    // 生成时区select的options
    var options = '';
    for ( var i = 0; i < timezones.length; i++ )
    {
        var timezone = timezones[i];
        options += '<option value="' + timezone.value + '" data-i18n="' + timezone.label + '"></option>';
    }
    // 添加到时区上, 并翻译
    $('#timezone').append(options).i18n();
	// Load the configure
    he.load( [ 'clock@date', 'clock@date.status', 'clock@ntps' ] ).then( function(v){
        date = v[0];
        dates = v[1];
        ntps = v[2];
        if ( dates )
        {
            $('#current').text(date2string(dates.current));
            if ( dates.source )
            {
                $('#source').text( $.i18n(dates.source) );
            }
            else
            {
                $('#source').text( $.i18n( "sourcenull" ) );
            }
            $('#livetime').text(time2string(dates.livetime));
        }
        if ( date )
        {
            $('#time_panel').show();
            $('#timezone').val(date.timezone || '');
            $('#ntpclient').prop('checked', able2boole(date.ntpclient));
            $('#ntpserver').val(date.ntpserver || '');
            $('#ntpserver2').val(date.ntpserver2 || '');
            $('#ntpserver3').val(date.ntpserver3 || '');
            $('#ntpc_panel').show();
            $('#apply_panel').show();
        }
        else
        {
            $('#time_panel').hide();
            $('#ntpc_panel').hide();
        }

        if ( ntps )
        {
            $('#ntpservice').prop('checked', able2boole(ntps.status));
            $('#ntps_panel').show();
            $('#apply_panel').show();
        }
        else
        {
            $('#ntps_panel').hide();
        }
        $('#ntpclient').unbind('change').change(function () {
            if ($(this).prop('checked'))
            {
              $('#ntpc_cfg').show();
              $('#sync').show();
            }
            else
            {
              $('#ntpc_cfg').hide();
              $('#sync').hide();
            }
        }).trigger('change');
        // 电脑对时
        $('#copy').on(ace.click_event, function () {
            var m;
            var now = new Date();
            // 实际的月份
            m = parseInt(now.getMonth()) + 1;
            // 使用客户端时间
            he.exec( [ 'clock@date.current['+now.getHours()+':'+now.getMinutes()+':'+now.getSeconds()+':'+m+':'+now.getDate()+':'+now.getFullYear()+']' ] ).then(function ( result ) {
                if ( result[0] )
                {
                    page.hint2succeed($.i18n('Sync successfully'));
                }
                else
                {
                    page.hint2warning($.i18n('Sync Failure'));
                }
                date_load();
            });
        });
        // NTP对时
        $('#sync').on(ace.click_event, function () {
            he.exec( [ 'clock@date.ntpsync' ] ).then(function (result) {
                if ( result[0] )
                {
                    page.hint2succeed($.i18n('Sync successfully'));
                }
                else
                {
                    page.hint2warning($.i18n('Sync Failure'));
                }
                date_load();
            });
        });
    });
}
function date_save()
{
    var cmds = [];
    var datecopy = null;
    var ntpscopy = null;
    var needsave = false;

    if ( date )
    {
        datecopy = JSON.parse(JSON.stringify(date));
        date.timezone = $('#timezone').val();
        date.ntpclient = boole2able( $('#ntpclient').prop('checked') );
        if ( date.ntpclient == "enable" )
        {
            date.ntpserver = $('#ntpserver').val();
            date.ntpserver2 = $('#ntpserver2').val();
            date.ntpserver3 = $('#ntpserver3').val();
        }
        if ( !ocompare( date, datecopy ) )
        {
            cmds.push( "clock@date="+JSON.stringify(date) );
            needsave = true;
        }
    }
    if ( ntps )
    {
        ntpscopy = JSON.parse(JSON.stringify(ntps));
        if ( boole2able( $('#ntpservice').prop('checked') ) == "enable" )
        {
            ntps.status = "enable";
        }
        else
        {
            ntps.status = "disable";
        }
        if ( !ocompare( ntps, ntpscopy ) )
        {
            cmds.push( "clock@ntps="+JSON.stringify(ntps) );
            needsave = true;
        }
    }

    if ( needsave == false )
    {
      page.alert( { message: $.i18n('Settings unchanged') } );
      return;
    }
    he.exec( cmds ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        date_load();
    });
}



/* init */
$.i18n().load( page.lang('device') ).then( function () {
  /* init the language */
  $.i18n().locale = lang; $('body').i18n();

  $('#device_html').show();

  // load hostname
  $('#name_string').text(window.machine.name);
  if ( window.machines.cmodel )
  {
    $('#model_string').text(window.machines.cmodel);
  }
  else
  {
    $('#model_string').text(window.machines.model);
  }
  // load datecode
  if ( window.machines.datecode )
  {
      $('#datecode').text(window.machines.datecode);
  }
  // load the mac
  $('#mac').text(window.machine.mac);
  // bind the hostname modify
  $('#name_modify').on(ace.click_event, function () {
      page.prompt( { message: $.i18n('Input new device name'), value:window.machine.name, callback:function(result){
          if ( result )
          { 
          	  var name = result.replace(/"/g, '\\"');
              he.exec( [ "land@machine:name="+name ] ).then( function(v){
                  var ret = v[0];
                  if ( ret == true )
                  {
                    location.reload();
                  }
              });
          }
      } } );
  });
  // bind reboot
  $('#reboot').on(ace.click_event, function () {
      page.confirm( { message: $.i18n('Are you sure you want to restart') } ).then( function(result){
          if ( result )
          { 
              he.reboot( { title: $.i18n('Restarting...'), hint:$.i18n('Make sure that the device is reconnected') } );
          }
      });
  });

  // mode
  if ( window.custom && window.custom.mode )
  {
    var m;
    for( m in window.custom.mode )
    {
        if ( window.custom.mode[m] != "null" )
        {
            $("#mode").append("<option value='"+m+"'>" + $.i18n(m) + "</option>");
        }
    }
    if ( window.custom.mode[window.machine.mode] == null )
    {
        $("#mode").append("<option value='"+window.machine.mode+"'>" + $.i18n(window.machine.mode) + "</option>");
    }
  }
  else if ( window.mode_list )
  {
    var m;
    for( m in window.mode_list )
    {
        $("#mode").append("<option value='"+m+"'>" + $.i18n(m) + "</option>");
    }
    if ( window.mode_list[window.machine.mode] == null )
    {
        $("#mode").append("<option value='"+window.machine.mode+"'>" + $.i18n(window.machine.mode) + "</option>");
    }
  }
  else
  {
    $("#mode").append("<option value='default'>"+$.i18n('default')+"</option>");
    $("#mode").append("<option value='misp'>"+$.i18n('misp')+"</option>");
    $("#mode").append("<option value='gateway'>"+$.i18n('gateway')+"</option>");
    $("#mode").append("<option value='wisp'>"+$.i18n('wisp')+"</option>");
  }
  $("#mode").val( window.machine.mode );
  // bind mode modify
  $("#mode").change(function(){
    var nmode = $("#mode").val();
    if ( nmode != window.machine.mode )
    {
        page.confirm( { message: $.i18n('The system will restart because of the change of settings') } ).then( function(result){
            if ( result )
            {
                he.exec( [ "land@machine:mode="+nmode ] ).then( function(){
                    page.confirm( { message: $.i18n('Restart the system to apply') } ).then( function(result){
                        if ( result )
                        {
                            he.reboot( { title: $.i18n('Restarting to apply...'), hint:$.i18n('Make sure that the device is reconnected') } );
                        }
                    });
                });
            }
            else
            {
                $("#mode").val( window.machine.mode );
            }
        });
    }
  });

  // hide or show
  if ( !wuimenu || wuimenu.opmode != "disable" )
  {
	$("#mode").closest('.form-group').show();
  }
  if ( !wuimenu || wuimenu.model != "disable" )
  {
	$("#model_string").closest('.form-group').show();
  }
  if ( window.machines.datecode )
  {
	$("#datecode").closest('.form-group').show();
  }

  // load date
  date_load();

  /* bind the refresh */
  $('#refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply */
  $('#apply').on(ace.click_event, function () {
    date_save();
  });
    
});

