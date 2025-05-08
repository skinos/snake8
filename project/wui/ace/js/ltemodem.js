
/* get the object */
var state;
var config;
var object = "modem@lte";
var ifcfg;
var ifname = "ifname@lte";
var index = page.param( 'object', location.hash );
if ( index )
{
	object = index;
}
var index = page.param( 'ifname', location.hash );
if ( index )
{
	ifname = index;
}



/* load the configure on the input */
function config_load()
{
	he.load( [ object, object+".status", ifname, object+".custom_set", object+".custom_watch", object+".lock_imei", object+".lock_imsi" ] ).then( function(v){
		config = v[0];
        if ( !config )
        {
            config = {};
        }
		state = v[1];
        if ( !state )
        {
            state = {};
        }
		// GNSS
        if ( config.gnss == "enable" )
        {
			$('#gnss').prop('checked', true );
        }
		else
		{
            $('#gnss').prop('checked', false );
		}
		// TCP Port
		$('#tcpport').text( config.tcpport );
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
        $('#lock_nettype').val(config.lock_nettype || 'auto');
		// lock pin
        $('#lock_pin').val( config.lock_pin||'' );
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
		// custom set
        if ( config.custom_set )
        {
            var s = v[3];
            var c = config.custom_set;
            var rows = [];
            for ( var id in c )
            {
                var cmd = c[id];
                if ( !cmd )
                {
                    continue;
                }
                var st = "";
                if ( s )
                {
                    st = s[id];
                }
                var row = { name:id, at:cmd, result:st };
                rows.push( row );
            }
            var scrollPos = jqtable.getScrollPos();
            $("#set-grid-table").jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
            jqtable.setScrollPos(scrollPos);
        }
		// custom watch
        if ( config.custom_watch )
        {
            var s = v[4];
            var c = config.custom_watch;
            var rows = [];
            for ( var id in c )
            {
                var cmd = c[id];
                if ( !cmd )
                {
                    continue;
                }
                var st = "";
                if ( s )
                {
                    st = s[id];
                }
                var row = { name:id, at:cmd, result:st };
                rows.push( row );
            }
            var scrollPos = jqtable.getScrollPos();
            $("#watch-grid-table").jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
            jqtable.setScrollPos(scrollPos);
        }
		// watch interval
        $('#watch_interval').val( config.watch_interval||'' );

		// need simcard
		ifcfg = v[2];
        if ( !ifcfg )
        {
            ifcfg = {};
        }
        $('#need_simcard').prop('checked', true );
        if ( ifcfg.need_simcard == "disable" )
        {
            $('#need_simcard').prop('checked', false );
        }
        $('#simcard_failed_threshold').val( ifcfg.simcard_failed_threshold||'' );
        $('#simcard_failed_threshold2').val( ifcfg.simcard_failed_threshold2||'' );
        $('#simcard_failed_threshold3').val( ifcfg.simcard_failed_threshold3||'' );
        $('#simcard_failed_everytime').val( ifcfg.simcard_failed_everytime||'' );
		// need plmn
        $('#need_plmn').prop('checked', true );
        if ( ifcfg.need_plmn == "disable" )
        {
            $('#need_plmn').prop('checked', false );
        }
		// need signal
        $('#need_signal').prop('checked', true );
        if ( ifcfg.need_signal == "disable" )
        {
            $('#need_signal').prop('checked', false );
        }
        $('#signal_failed_threshold').val( ifcfg.signal_failed_threshold||'' );
        $('#signal_failed_threshold2').val( ifcfg.signal_failed_threshold2||'' );
        $('#signal_failed_threshold3').val( ifcfg.signal_failed_threshold3||'' );
        $('#signal_failed_everytime').val( ifcfg.signal_failed_everytime||'' );
		// attach failed
        $('#need_attach').prop('checked', true );
        if ( ifcfg.need_attach == "disable" )
        {
            $('#need_attach').prop('checked', false );
        }
        $('#attach_failed_threshold').val( ifcfg.attach_failed_threshold||'' );
        $('#attach_failed_threshold2').val( ifcfg.attach_failed_threshold2||'' );
        $('#attach_failed_threshold3').val( ifcfg.attach_failed_threshold3||'' );
        $('#attach_failed_everytime').val( ifcfg.attach_failed_everytime||'' );
		// dial failed
        $('#failed_threshold').val( ifcfg.failed_threshold||'' );
        $('#failed_threshold2').val( ifcfg.failed_threshold2||'' );
        $('#failed_threshold3').val( ifcfg.failed_threshold3||'' );
        $('#signal_failed_everytime').val( ifcfg.signal_failed_everytime||'' );

      })
}

/* save the configure */
function config_save()
{
	var cmds = [];
	var needsave = false;

	if ( config == null )
	{
		return;
	}
	var copy = JSON.parse(JSON.stringify(config));;
	// config
	config.gnss = boole2able( $('#gnss').prop('checked') );
	config.tcpport = $('#tcpport').val();
	config.lock_nettype = $('#lock_nettype').val();
	config.lock_pin = $('#lock_pin').val();
	var data = {};
	var rows = $("#set-grid-table").jqGrid('getDataIDs');
	for ( var i = 0; i < rows.length; i++ )
	{
		var row = $("#set-grid-table").jqGrid('getRowData', rows[i] );
		data[row.name] = row.at;
	}
	if ( rows.length > 0 )
	{
		config.custom_set = data;
	}
	else
	{
		config.custom_set= {};
	}
	var data = {};
	var rows = $("#watch-grid-table").jqGrid('getDataIDs');
	for ( var i = 0; i < rows.length; i++ )
	{
		var row = $("#watch-grid-table").jqGrid('getRowData', rows[i] );
		data[row.name] = row.at;
	}
	if ( rows.length > 0 )
	{
		config.custom_watch = data;
	}
	else
	{
		config.custom_watch= {};
	}
	config.watch_interval = $('#watch_interval').val();
	if ( !ocompare( config, copy ) )
	{
		cmds.push( object+"="+JSON.stringify(config) );
		needsave = true;
	}
	
	// ifcfg
	if ( ifcfg == null )
	{
		return;
	}
	var ifcopy = JSON.parse(JSON.stringify(ifcfg));;
	ifcfg.need_simcard = boole2able( $('#need_simcard').prop('checked') );
	ifcfg.simcard_failed_threshold = $('#simcard_failed_threshold').val();
	ifcfg.simcard_failed_threshold2 = $('#simcard_failed_threshold2').val();
	ifcfg.simcard_failed_threshold3 = $('#simcard_failed_threshold3').val();
	ifcfg.simcard_failed_everytime = $('#simcard_failed_everytime').val();
	ifcfg.need_plmn = boole2able( $('#need_plmn').prop('checked') );
	ifcfg.need_signal = boole2able( $('#need_signal').prop('checked') );
	ifcfg.signal_failed_threshold = $('#signal_failed_threshold').val();
	ifcfg.signal_failed_threshold2 = $('#signal_failed_threshold2').val();
	ifcfg.signal_failed_threshold3 = $('#signal_failed_threshold3').val();
	ifcfg.signal_failed_everytime = $('#signal_failed_everytime').val();
	ifcfg.need_attach = boole2able( $('#need_attach').prop('checked') );
	ifcfg.attach_failed_threshold = $('#attach_failed_threshold').val();
	ifcfg.attach_failed_threshold2 = $('#attach_failed_threshold2').val();
	ifcfg.attach_failed_threshold3 = $('#attach_failed_threshold3').val();
	ifcfg.attach_failed_everytime = $('#attach_failed_everytime').val();
	ifcfg.failed_threshold = $('#failed_threshold').val();
	ifcfg.failed_threshold2 = $('#failed_threshold2').val();
	ifcfg.failed_threshold3 = $('#failed_threshold3').val();
	ifcfg.failed_everytime = $('#failed_everytime').val();

	if ( !ocompare( ifcfg, ifcopy ) )
	{
		cmds.push( ifname+"="+JSON.stringify(ifcfg) );
		needsave = true;
	}


	if ( needsave == false )
	{
		page.alert( { message: $.i18n('Settings unchanged') } );
		return;
	}
	page.confirm( { message: $.i18n('The LTE connecttion will be disconneted because of the change of configuration') } ).then( function(result){
		if ( result )
		{
			he.exec( cmds ).then( function(){
				page.hint2succeed( $.i18n('Modify successfully') );
				config_load();
			});
		}
	});
}



/* init */
page.password('passwd', 'password-icon' );
$.i18n().load( page.lang('lte') ).then( function () {
	/* init the langauage */
	$.i18n().locale = lang; $('body').i18n();

	/* init the table */
	jqtable.create( "#set-grid-table", "set-grid-pager",
	{
		caption: $.i18n("Custom setting AT"),
		colNames: [ $.i18n('Command Name'), $.i18n('AT Command'), $.i18n('Return') ],
		colModel: [
			{
				name:'name', width:200,
				editable: true,
				editrules:{ required: true }
			},
			{
				name: 'at', width: 300,
				editable: true,
				editrules: { required: true }
			},
			{
				name: 'result', width: 300,
				editable: false
			}
		]
	} ).jqGrid(
		'navGrid', "#set-grid-pager",
		$.extend(true, {}, jqtable.navOptions, 
			{
				add: true,addicon : 'ace-icon fa fa-plus-circle purple',
				del: true,delicon : 'ace-icon fa fa-trash-o red',
				edit: true,editicon : 'ace-icon fa fa-pencil blue',
				search: false, refresh: false, view: false
			}
		),
		jqtable.editOptions,
		jqtable.addOptions,
		jqtable.deleteOptions
	);
	jqtable.create( "#watch-grid-table", "watch-grid-pager",
	{
		caption: $.i18n("Custom watching AT"),
		colNames: [ $.i18n('Command Name'), $.i18n('AT Command'), $.i18n('Return') ],
		colModel: [
			{
				name:'name', width:200,
				editable: true,
				editrules:{ required: true }
			},
			{
				name: 'at', width: 300,
				editable: true,
				editrules: { required: true }
			},
			{
				name: 'result', width: 300,
				editable: false
			}
		]
	} ).jqGrid(
		'navGrid', "#watch-grid-pager",
			$.extend(true, {}, jqtable.navOptions, 
			{
				add: true,addicon : 'ace-icon fa fa-plus-circle purple',
				del: true,delicon : 'ace-icon fa fa-trash-o red',
				edit: true,editicon : 'ace-icon fa fa-pencil blue',
				search: false, refresh: false, view: false
			}
		),
		jqtable.editOptions,
		jqtable.addOptions,
		jqtable.deleteOptions
	);

	/* load the configure */
	config_load();

	// 锁IMEI
	$('#imei_lock').on(ace.click_event, function () {
		page.confirm( { message: $.i18n('Once locked cannot be undone'), callback:function(result){
			if ( result )
			{ 
				// 执行修改
				he.exec( [ object+".lock_imei["+state.imei+"]" ] ).then( function(v){
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
	$('#imsi_lock').on(ace.click_event, function () {
		page.confirm( { message: $.i18n('Once locked cannot be undone'), callback:function(result){
			if ( result )
			{ 
				// 执行修改
				he.exec( [ object+".lock_imsi["+state.imsi+"]" ] ).then( function(v){
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
	$('#refresh').on(ace.click_event, function () {
		location.reload();
	});
	/* bind the apply */
	$('#apply').on(ace.click_event, function () {
		config_save();
	});
});

