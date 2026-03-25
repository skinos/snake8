var iconfig;
var modem = page.param('modem', location.hash);   
var ifname = page.param('object', location.hash);

var set_table = '#set-grid-table';
var set_pager = '#set-grid-pager';
var watch_table = '#watch-grid-table';
var watch_pager = '#watch-grid-pager';

// 加载数据
function lte_at() {
    window.LteConfigManager.loadSettings(modem, ifname, true).then(function(v) {
        iconfig = v[1];
        if ( !iconfig )
        {
            iconfig = {};
        }

        // custom set
        if ( iconfig.custom_set )
        {
            var s = v[3];
            var c = iconfig.custom_set;
            var count = 0;
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
                count++;
            }
            $(set_table).jqGrid("setCaption", $.i18n('Custom setting AT') + '(' + count + ')' );
            var scrollPos = jqtable.getScrollPos();
            $("#set-grid-table").jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
            jqtable.setScrollPos(scrollPos);
        }
		// custom watch
        if ( iconfig.custom_watch )
        {
            var s = v[4];
            var c = iconfig.custom_watch;
            var count = 0;
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
                count++;
            }
            $(watch_table).jqGrid("setCaption", $.i18n('Custom watching AT') + '(' + count + ')' );
            var scrollPos = jqtable.getScrollPos();
            $("#watch-grid-table").jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
            jqtable.setScrollPos(scrollPos);
        }

    })
}

// 保存数据
function at_save() {
    var cmds = [];
	var needsave = false;
	if ( iconfig == null )
	{
		return;
	}
	var icopy = JSON.parse(JSON.stringify(iconfig));

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
    var msg = $.i18n('The LTE connecttion will be disconneted because of the change of configuration');
	page.confirm( { message: msg } ).then( function(result){
    if ( result )
    {
        he.exec( cmds ).then( function(){
            page.hint2succeed( $.i18n('Modify successfully') );
            lte_at();
        });
    }
	});
}

function init_at() {
    var $setTable = $(set_table);
    var $watchTable = $(watch_table);
    
    // 调试bug通过
    if ($setTable.length === 0 || $watchTable.length === 0) {
        //console.warn("Table elements not found, skipping init_at.");
        return false; 
    }

    if ($setTable[0].grid || $watchTable[0].grid) {
        return true; 
    }
    
    // 初始化设置表格
    jqtable.create(set_table, set_pager, {
        caption: $.i18n("Custom setting AT"),
        toolbar: [true, "top"],
        colNames: [$.i18n('Command Name'), $.i18n('AT Command'), $.i18n('Return')],
        colModel: [
            { name: 'name', width: 100, editable: true, editrules: { required: true } },
            { name: 'at', width: 250, editable: true, editrules: { required: true } },
            { name: 'result', width: 400, editable: false }
        ],
        pager: set_pager,
        rowNum: 10,
        viewrecords: true,
        pgbuttons: true,
        pagerpos: 'center',
        pginput: true,
        autowidth: true,
        loadonce: true,
        shrinkToFit: true,
        responsive: true,
    }).jqGrid('navGrid', set_pager,
        $.extend(true, {}, jqtable.navOptions, {
            add: true, addicon: 'ace-icon fa fa-plus-circle purple',
            del: true, delicon: 'ace-icon fa fa-trash-o red',
            edit: true, editicon: 'ace-icon fa fa-pencil blue',
            search: false, refresh: false, view: false
        }),
        jqtable.editOptions,
        $.extend(true, {}, jqtable.addOptions, {
            afterShowForm: function(form) {
                $("label[for='name']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
                $("label[for='at']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
                var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
                    '<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
                    '</div>';
                $("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
                $("#name", form).attr("placeholder", $.i18n('Enter Command Name'));
                $("#at", form).attr("placeholder", $.i18n('Enter AT Command'));
            }
        }),
        jqtable.deleteOptions
    );
    
    // 初始化监控表格
    jqtable.create(watch_table, watch_pager, {
        caption: $.i18n("Custom watching AT"),
        toolbar: [true, "top"],
        colNames: [$.i18n('Command Name'), $.i18n('AT Command'), $.i18n('Return')],
        colModel: [
            { name: 'name', width: 200, editable: true, editrules: { required: true } },
            { name: 'at', width: 300, editable: true, editrules: { required: true } },
            { name: 'result', width: 300, editable: false }
        ],
        pager: watch_pager,
        rowNum: 10,
        viewrecords: true,
        pgbuttons: true,
        pagerpos: 'center',
        pginput: true,
        autowidth: true,
        loadonce: true,
        shrinkToFit: true,
        responsive: true,
    }).jqGrid('navGrid', watch_pager,
        $.extend(true, {}, jqtable.navOptions, {
            add: true, addicon: 'ace-icon fa fa-plus-circle purple',
            del: true, delicon: 'ace-icon fa fa-trash-o red',
            edit: true, editicon: 'ace-icon fa fa-pencil blue',
            search: false, refresh: false, view: false
        }),
        jqtable.editOptions,
        $.extend(true, {}, jqtable.addOptions, {
            afterShowForm: function(form) {
                $("label[for='name']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
                $("label[for='at']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
                var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
                    '<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
                    '</div>';
                $("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
                $("#name", form).attr("placeholder", $.i18n('Enter Command Name'));
                $("#at", form).attr("placeholder", $.i18n('Enter AT Command'));
            }
        }),
        jqtable.deleteOptions
    );
    
    // 设置工具栏
    var $setToolbar = $("#t_set-grid-table");
    $setToolbar.append($('#grid-controls').children());
    $setToolbar.css({
        'display': 'flex',
        'justify-content': 'space-between',
        'align-items': 'center',
        'background': '#f5f5f5',
        'padding': '8px 10px',
        'height': 'auto',
    });
    
    var $watchToolbar = $("#t_watch-grid-table");
    $watchToolbar.append($('#grid-controls2').children());
    $watchToolbar.css({
        'display': 'flex',
        'justify-content': 'space-between',
        'align-items': 'center',
        'background': '#f5f5f5',
        'padding': '8px 10px',
        'height': 'auto',
    });
    
    // 行数选择器
    $('#rowNums').on('change', function() {
        var newRowNum = parseInt($(this).val(), 10);
        $(set_table).jqGrid('setGridParam', { rowNum: newRowNum }).trigger('reloadGrid');
        $(watch_table).jqGrid('setGridParam', { rowNum: newRowNum }).trigger('reloadGrid');
    });
    //console.log("初始化成功啦")
    return true;
}

$.i18n().load( page.lang('lte') ).then( function () {
	/* init the langauage */
	$.i18n().locale = lang; $('body').i18n();

	/* init the table */
    //init_at();

    /* load the configure */
	//lte_at();

    // 延迟解决jqgrid的styleUI问题
    function safeStart() {
        if (init_at()) {
            // 只有表格初始化成功了 才加载配置和数据
            lte_at();
            //console.log("LTE AT Page Initialized Successfully.");
        } else {
            // 如果init_at失败 找不到元素 说明DOM还没渲染完 100ms 后重试
            //console.log("DOM not ready, retrying in 100ms...");
            setTimeout(safeStart, 100);
        }
    }

    safeStart();

    /* bind the refresh */
	$('#at_refresh').on(ace.click_event, function () {
		location.reload();
	});
	/* bind the apply */
	$('#at_apply').off('click').on('click', function() {
        at_save();
    });
})
