var syslog;
var filelist;
var wuimenu = window.wui.menu;
var syslogs_table = '#syslogs-grid-table';
var syslogs_pager = '#syslogs-grid-pager';

function load_syslog()
{
    he.load( [ 'land@syslog', 'land@syslog.list' ] ).then( function(v){

        syslog = v[0];
        if ( syslog.status == "enable" || syslog.status == "both" )
        {
            $('#status').prop('checked', true );
        }
        else if ( syslog.status == "file" )
        {
            $('#status').prop('checked', true );
            $("#location").closest('.form-group').hide();
            $("#remote").closest('.form-group').hide();
        }
        else
        {
            $('#status').prop('checked', false );
        }
        $('#location').val( syslog.location );
        $('#size').val( syslog.size );
        $('#level').val( syslog.level );
        $('#debug').val( syslog.debug );
        $('#verb').val( syslog.verb );
        $('#remote').prop('checked', !!syslog.remote);
        $('#server').val(syslog.remote);
        $('#port').val(syslog.port);

        $('#status').unbind('change').change(function () {
            if ($(this).prop('checked'))
            {
                $('#statusSets').show();
                var containerWidth = $('#size').outerWidth();  
                if ($("#syslogs-grid-table")[0] && $("#syslogs-grid-table")[0].grid) { 
                    $("#syslogs-grid-table").jqGrid('setGridWidth', containerWidth, true);
                }  
            }
            else
            {
                $('#statusSets').hide()
            }
        }).trigger('change');

        $('#remote').unbind('change').change(function () {
            if ($(this).prop('checked'))
            {
                $('#syslogdSets').show();
            }
            else
            {
                $('#syslogdSets').hide();
            }
        }).trigger('change')

        filelist = v[1];
        var count = 0;
        var rows = [];
        for ( var i in filelist )
        {
            var file = filelist[i];
            rows.push( { name: i } );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(syslogs_table).jqGrid( "setCaption", $.i18n('Log List') + ' (' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // load data into the grid
        $(syslogs_table).jqGrid( 'clearGridData' ).jqGrid( 'setGridParam', { data: rows } ).trigger( 'reloadGrid' );
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);

    });
}
function save_syslog()
{
    if ( syslog == null )
    {
        return;
    }
    var syslogcopy = JSON.parse(JSON.stringify(syslog));

    if ( $('#status').prop('checked') == true )
    {
        /* compatibility to 7.3 */
        if ( window.machine_status )
        {
            window.machines = window.machine_status;
        }
        if ( window.machines.platform == "host" )
        {
            syslog.status = "file";
        }
        else
        {
            syslog.status = "enable";
            syslog.location = $('#location').val();
        }
        syslog.size = $('#size').val();
        syslog.level = $('#level').val();
        syslog.debug = $('#debug').val();
        syslog.verb = $('#verb').val();
        // 启用远程日志
        if ( $('#remote').prop('checked') )
        {
            syslog.remote = $('#server').val();
            syslog.port = $('#port').val();
        }
        else
        {
            syslog.remote = '';
        }
    }
    else
    {
        syslog.status = "disable";
    }
    if ( ocompare( syslog, syslogcopy ) )
    {
        page.alert( { message: $.i18n('Settings unchanged') } );
        return;
    }
    he.exec( [ "land@syslog="+JSON.stringify(syslog) ] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        load_syslog();
    });
}

/* delete the client key */
function syslog_delete( indexStr )
{
    // 选中的行
    var indexs = indexStr.split(',');
    // 删除路由的he命令
    var cmds = [];
    for ( var index  = 0; index < indexs.length; index++ )
    {
        // 选中行的数据
        var row = $(syslogs_table).jqGrid('getRowData', indexs[index]);
        // 通过name删除
        cmds.push( 'land@syslog.delete['+row.name+"]" );
    }
    // 执行删除
    he.exec( cmds ).then(function (){
        // 重新加载数据
        return load_syslog();
    }).then(function (){
        // 提示成功
        page.hint2succeed( $.i18n('Delete successfully') );
    });
}

// 下载日志
function syslog_down( rowId )
{
	// Select the line
	var client = $('#syslogs-grid-table').jqGrid( 'getRowData', rowId );
	var name = client.name;
	// Create the from
	var opt;
	var sform = document.createElement("FORM");
	document.body.appendChild(sform);
	sform.hidden = true;
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "username"; opt.value = window.username; sform.appendChild( opt );
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "key"; opt.value = window.talkkey; sform.appendChild( opt );
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "object"; opt.value = "land@syslog"; sform.appendChild( opt );
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "api"; opt.value = "list"; sform.appendChild( opt );
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "a"; opt.value = name; sform.appendChild( opt );
	opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "f"; opt.value = name; sform.appendChild( opt );
	sform.method= "GET";
	sform.action= "/download";
	setTimeout(function() {
	  sform.submit();
	}, 200);
}
  
$.i18n().load( page.lang('syslog') ).then( function () {
  /* init the language */
  $.i18n().locale = lang; $('body').i18n();

  $('#syslog_html').show();

  /* init the clients table */
  jqtable.create(  syslogs_table, syslogs_pager,
    {
        caption: ' ', // 必需设置值, 防止表格不能折叠
        toolbar: [true, "top"],
        colNames: [ $.i18n('Log File'), $.i18n('Download'), $.i18n('Operation') ],
        colModel: [
            { name:'name'},
            {
                name: 'download', width: 85,
                fixed: true, sortable: false,
                formatter: function ( cellvalue, options, rowObject )
                {
                    return '<button class="btn btn-main btn-xs btn-choose" onclick="syslog_down(' + options.rowId + ')" data-id="' + options.rowId + '">' + $.i18n('Download') + '</button>'
                }
            },
            $.extend( true, {}, jqtable.actionOptions,
                { formatoptions:{ delOptions:{ onclickSubmit:function(params, data) { syslog_delete(data); } }, editformbutton:false, editbutton:false } } )
        ],
        pager: '#syslogs-grid-pager',
        rowNum: 10,
        viewrecords: true,

        pgbuttons: true,
        pagerpos:'center',
        pginput:true,

        autowidth:true,
        loadonce:true,
        shrinkToFit:true,
        //responsive:true,
       
    }
  ).jqGrid( 
        'navGrid', syslogs_pager,
        $.extend(true, {}, jqtable.navOptions, {add: false, edit: false, view: false}),
        jqtable.editOptions,
        jqtable.addOptions,
        $.extend(true, {}, jqtable.deleteOptions, { onclickSubmit: function(params, data) { syslog_delete(data); } }),
        {}
  );

    var $toolbar = $("#t_" + syslogs_table.replace('#', ''));
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
        $(syslogs_table).jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
    });

    $(window).unbind('resize.myGrid').on('resize.myGrid', function () {
        var inputWidth = $('#size').outerWidth();
        $("#syslogs-grid-table").jqGrid('setGridWidth', inputWidth);
    });

  /* load the configure */
  load_syslog();
  /* bind the refresh */
  $('#refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply to save configure */
  $('#apply').on(ace.click_event, function () {
    save_syslog();
  });
  
  if ( wuimenu && wuimenu.download_log == "disable" )
  {
      $('#download-log').hide();
  }

});

