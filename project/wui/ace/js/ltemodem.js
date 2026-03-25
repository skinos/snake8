
/* get the object */
var state;
var mconfig;
var iconfig;
var modem = page.param('modem', location.hash);   
var ifname = page.param('object', location.hash);

/* load the configure on the input */
function lte_modem()
{   
	window.LteConfigManager.loadSettings(modem, ifname, true).then(function(v) {
		mconfig = v[0];
        if ( !mconfig )
        {
            mconfig = {};
        }
		iconfig = v[1];
        if ( !iconfig )
        {
            iconfig = {};
        }
		state = v[2];
        if ( !state )
        {
            state = {};
        }
		// GNSS
        if ( iconfig.gnss == "enable" )
        {
			$('#gnss').prop('checked', true );
        }
		else
		{
            $('#gnss').prop('checked', false );
		}
		// lock nettype
        $("#lock_nettype").empty();
        $("#lock_nettype").append("<option value='auto'>" + $.i18n("Auto") + "</option>");
        if ( state.na == "enable" )
        {
			$("#lock_nettype").append("<option value='nsa'>" + "NSA(5G)" + "</option>");
			$("#lock_nettype").append("<option value='sa'>" + "SA(5G)" + "</option>");
        }
		$("#lock_nettype").append("<option value='lte'>" + "LTE(4G)" + "</option>");
		$("#lock_nettype").append("<option value='wcdma'>" + "WCDMA" + "</option>");
		$("#lock_nettype").append("<option value='tdscdma'>" + "TDSCDMA" + "</option>");
		$("#lock_nettype").append("<option value='evdo'>" + "EVDO" + "</option>");
		$("#lock_nettype").append("<option value='cdma'>" + "CDMA" + "</option>");
		$("#lock_nettype").append("<option value='gsm'>" + "GSM" + "</option>");
        $('#lock_nettype').val(iconfig.lock_nettype || 'auto');
		// lock pin
        $('#lock_pin').val( iconfig.lock_pin||'' );
		// lock imei
        if ( v[5] )
        {
            $('#lock_imei').text( v[5] );
            $('#imei_lock').hide();
        }
        else
        {
            $('#lock_imei').text( $.i18n("Unlocked" ) );
            $('#imei_lock').show();
        }
		// lock imsi
        if ( v[6] )
        {
            $('#lock_imsi').text( v[6] );
            $('#imsi_lock').hide();
        }
        else
        {
            $('#lock_imsi').text( $.i18n("Unlocked" ) );
            $('#imsi_lock').show();
        }
		
		// watch interval
        $('#watch_interval').val( iconfig.watch_interval||'' );

		// need simcard
        $('#need_simcard').prop('checked', true );
        if ( iconfig.need_simcard == "disable" )
        {
            $('#need_simcard').prop('checked', false );
        }
        $('#simcard_failed_threshold').val( iconfig.simcard_failed_threshold||'' );
        $('#simcard_failed_threshold2').val( iconfig.simcard_failed_threshold2||'' );
        $('#simcard_failed_threshold3').val( iconfig.simcard_failed_threshold3||'' );
        $('#simcard_failed_everytime').val( iconfig.simcard_failed_everytime||'' );
		// need plmn
        $('#need_plmn').prop('checked', true );
        if ( iconfig.need_plmn == "disable" )
        {
            $('#need_plmn').prop('checked', false );
        }
		// need signal
        $('#need_signal').prop('checked', true );
        if ( iconfig.need_signal == "disable" )
        {
            $('#need_signal').prop('checked', false );
        }
        $('#signal_failed_threshold').val( iconfig.signal_failed_threshold||'' );
        $('#signal_failed_threshold2').val( iconfig.signal_failed_threshold2||'' );
        $('#signal_failed_threshold3').val( iconfig.signal_failed_threshold3||'' );
        $('#signal_failed_everytime').val( iconfig.signal_failed_everytime||'' );
		// attach failed
        $('#need_attach').prop('checked', true );
        if ( iconfig.need_attach == "disable" )
        {
            $('#need_attach').prop('checked', false );
        }
        $('#attach_failed_threshold').val( iconfig.attach_failed_threshold||'' );
        $('#attach_failed_threshold2').val( iconfig.attach_failed_threshold2||'' );
        $('#attach_failed_threshold3').val( iconfig.attach_failed_threshold3||'' );
        $('#attach_failed_everytime').val( iconfig.attach_failed_everytime||'' );
		// dial failed
        $('#failed_threshold').val( iconfig.failed_threshold||'' );
        $('#failed_threshold2').val( iconfig.failed_threshold2||'' );
        $('#failed_threshold3').val( iconfig.failed_threshold3||'' );
        $('#signal_failed_everytime').val( iconfig.signal_failed_everytime||'' );
      })
}

/* save the configure */
function modem_save() {
    if (!iconfig) return;

    // 克隆旧配置用于对比
    var icopy = JSON.parse(JSON.stringify(iconfig));

    // 定义字段映射
    var fields = {
        checkbox: [
            'gnss', 'need_simcard', 'need_plmn', 'need_signal', 'need_attach'
        ],
        text: [
            'lock_nettype', 'lock_pin', 'watch_interval',
            'simcard_failed_everytime', 'signal_failed_everytime', 'attach_failed_everytime', 'failed_everytime'
        ]
    };

    var categories = ['simcard_failed', 'signal_failed', 'attach_failed', 'failed'];
    categories.forEach(function(cat) {
        fields.text.push(cat + '_threshold', cat + '_threshold2', cat + '_threshold3');
    });

    // 抓取数据
    fields.checkbox.forEach(function(id) {
        iconfig[id] = boole2able($('#' + id).prop('checked'));
    });

    fields.text.forEach(function(id) {
        var val = $('#' + id).val();
        // 如果值为空 直接删除该键值 否则赋值
        if (val === "") {
            delete iconfig[id];
        } else {
            iconfig[id] = val;
        }
    });

    if (ocompare(iconfig, icopy)) {
        return page.alert({ message: $.i18n('Settings unchanged') });
    }

    var msg = $.i18n('The LTE connecttion will be disconneted because of the change of configuration');
    page.confirm({ message: msg }).then(function(result) {
        if (!result) return location.reload();

        var cmds = [ ifname + "=" + JSON.stringify(iconfig) ];
        he.exec(cmds).then(function() {
            page.hint2succeed($.i18n('Modify successfully'));
            lte_modem();
        });
    });
}



/* init */
page.password('passwd', 'password-icon' );
$.i18n().load( page.lang('lte') ).then( function () {
	/* init the langauage */
	$.i18n().locale = lang; $('body').i18n();

	/* load the configure */
	lte_modem();

	// 锁IMEI
	$('#imei_lock').off('click').on('click', function () {
		page.confirm( { message: $.i18n('Once locked cannot be undone'), callback:function(result){
			if ( result )
			{ 
				// 执行修改
				he.exec( [ ifname+".lock_imei["+state.imei+"]" ] ).then( function(v){
					var ret = v[0];
					if ( ret == true )
					{
						location.reload();
					}
				});
			}
		} } );
	});
	// 锁IMSI
	$('#imsi_lock').off('click').on('click', function () {
		page.confirm( { message: $.i18n('Once locked cannot be undone'), callback:function(result){
			if ( result )
			{ 
				// 执行修改
				he.exec( [ ifname+".lock_imsi["+state.imsi+"]" ] ).then( function(v){
					var ret = v[0];
					if ( ret == true )
					{
						location.reload();
					}
				});
			}
		} } );
	});

	/* bind the refresh */
	$('#modem_refresh').on(ace.click_event, function () {
		location.reload();
	});
	/* bind the apply */
	$('#modem_apply').off('click').on('click', function() {
        modem_save();
    });
});

