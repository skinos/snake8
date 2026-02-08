
/* get the object */
var state;
var mconfig;
var iconfig;
var modem = "modem@lte";
var ifname = "ifname@lte";
var index = page.param( 'modem', location.hash );
if ( index )
{
	modem = index;
}
var index = page.param( 'ifname', location.hash );
if ( index )
{
	ifname = index;
}



/* load the configure on the input */
function config_load()
{
	he.load( [ modem, ifname, ifname+".status", ifname+".custom_set", ifname+".custom_watch", ifname+".lock_imei", ifname+".lock_imsi" ] ).then( function(v){
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
		// custom set
        if ( iconfig.custom_set )
        {
            var s = v[3];
            var c = iconfig.custom_set;
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
        if ( iconfig.custom_watch )
        {
            var s = v[4];
            var c = iconfig.custom_watch;
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
function config_save()
{
	var cmds = [];
	var needsave = false;
	if ( iconfig == null )
	{
		return;
	}
	var icopy = JSON.parse(JSON.stringify(iconfig));;

	// config
	iconfig.gnss = boole2able( $('#gnss').prop('checked') );
	iconfig.lock_nettype = $('#lock_nettype').val();
	iconfig.lock_pin = $('#lock_pin').val();
	var data = {};
	var rows = $("#set-grid-table").jqGrid('getDataIDs');
	for ( var i = 0; i < rows.length; i++ )
	{
		var row = $("#set-grid-table").jqGrid('getRowData', rows[i] );
		data[row.name] = row.at;
	}
	if ( rows.length > 0 )
	{
		iconfig.custom_set = data;
	}
	else
	{
		delete iconfig.custom_set;
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
		iconfig.custom_watch = data;
	}
	else
	{
		delete iconfig.custom_watch;
	}
	iconfig.watch_interval = $('#watch_interval').val();
	iconfig.need_simcard = boole2able( $('#need_simcard').prop('checked') );
	iconfig.simcard_failed_threshold = $('#simcard_failed_threshold').val();
	iconfig.simcard_failed_threshold2 = $('#simcard_failed_threshold2').val();
	iconfig.simcard_failed_threshold3 = $('#simcard_failed_threshold3').val();
	iconfig.simcard_failed_everytime = $('#simcard_failed_everytime').val();
	iconfig.need_plmn = boole2able( $('#need_plmn').prop('checked') );
	iconfig.need_signal = boole2able( $('#need_signal').prop('checked') );
	iconfig.signal_failed_threshold = $('#signal_failed_threshold').val();
	iconfig.signal_failed_threshold2 = $('#signal_failed_threshold2').val();
	iconfig.signal_failed_threshold3 = $('#signal_failed_threshold3').val();
	iconfig.signal_failed_everytime = $('#signal_failed_everytime').val();
	iconfig.need_attach = boole2able( $('#need_attach').prop('checked') );
	iconfig.attach_failed_threshold = $('#attach_failed_threshold').val();
	iconfig.attach_failed_threshold2 = $('#attach_failed_threshold2').val();
	iconfig.attach_failed_threshold3 = $('#attach_failed_threshold3').val();
	iconfig.attach_failed_everytime = $('#attach_failed_everytime').val();
	iconfig.failed_threshold = $('#failed_threshold').val();
	iconfig.failed_threshold2 = $('#failed_threshold2').val();
	iconfig.failed_threshold3 = $('#failed_threshold3').val();
	iconfig.failed_everytime = $('#failed_everytime').val();
	for ( var id in iconfig )
	{
		if ( iconfig[id] == "" )
		{
			delete iconfig[id];
		}
	}

	// compare
	if ( !ocompare( iconfig, icopy ) )
	{
		cmds.push( ifname+"="+JSON.stringify(iconfig) );
		needsave = true;
	}
	if ( needsave == false )
	{
		page.alert( { message: $.i18n('Settings unchanged') } );
		return;
	}

	// save
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
		toolbar: [true, "top"],
		colNames: [ $.i18n('Command Name'), $.i18n('AT Command'), $.i18n('Return') ],
		colModel: [
			{
				name:'name', width:100,
				editable: true,
				editrules:{ required: true }
			},
			{
				name: 'at', width: 250,
				editable: true,
				editrules: { required: true }
			},
			{
				name: 'result', width: 400,
				editable: false
			}
		],
		pager: '#set-grid-pager',
		rowNum: 10,
		viewrecords: true,

		pgbuttons: true,
		pagerpos:'center',
		pginput:true,

		autowidth:true,
		loadonce:true,
		shrinkToFit:true,
		responsive:true,
		
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
		//jqtable.addOptions,
		$.extend(true, {}, jqtable.addOptions, 
		{
			afterShowForm:function(form){
			// 添加自定义样式和提示
				$("label[for='name']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
				$("label[for='at']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
			
				var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
					'<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
					'</div>';
				
				$("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
				// 设置 placeholder
				$("#name", form).attr("placeholder", $.i18n('Enter Command Name'));
				$("#at", form).attr("placeholder", $.i18n('Enter AT Command'));
		}
		}),
		jqtable.deleteOptions
	);
	jqtable.create( "#watch-grid-table", "watch-grid-pager",
	{
		caption: $.i18n("Custom watching AT"),
		toolbar: [true, "top"],
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
		],
		pager: '#watch-grid-pager',
        rowNum: 10,
        viewrecords: true,

		pgbuttons: true,
		pagerpos:'center',
		pginput:true,

		autowidth:true,
		loadonce:true,
		shrinkToFit:true,
		responsive:true,
		
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
		//jqtable.addOptions,
		$.extend(true, {}, jqtable.addOptions, 
		{
			afterShowForm:function(form){
			// 添加自定义样式和提示
				$("label[for='name']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
				$("label[for='at']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
			
				var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
					'<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
					'</div>';
				
				$("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
				// 设置 placeholder
				$("#name", form).attr("placeholder", $.i18n('Enter Command Name'));
				$("#at", form).attr("placeholder", $.i18n('Enter AT Command'));
		}
		}),
		jqtable.deleteOptions
	);

	var $toolbar = $("#t_" + "set-grid-table");
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

	var $toolbar = $("#t_" + "watch-grid-table");
	$toolbar.append($('#grid-controls2').children());
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
			$('#set-grid-table').jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
	});

	$('#rowNums').on('change',function(){
			var newRowNum = parseInt($(this).val(),10);
			$('#watch-grid-table').jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
	});
	/* load the configure */
	config_load();

	// 锁IMEI
	$('#imei_lock').on(ace.click_event, function () {
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
	$('#imsi_lock').on(ace.click_event, function () {
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
	$('#refresh').on(ace.click_event, function () {
		location.reload();
	});
	/* bind the apply */
	$('#apply').on(ace.click_event, function () {
		config_save();
	});
});

