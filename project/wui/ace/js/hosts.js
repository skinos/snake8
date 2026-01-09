
var obj = "network@hosts";
var hosts;
var hosts_table = '#hosts-grid-table';
var hosts_pager = '#hosts-grid-pager';

/* load the configure fill the page */
function hosts_load()
{
    he.load( [ obj ] ).then( function(v){
        hosts = v[0];
        if ( !hosts )
        {
            return;
        }
        hostscopy = JSON.parse(JSON.stringify(hosts));
        var rows = [];
        for ( var hostname in hosts )
        {
            var row = { hostname: hostname, ip: hosts[hostname] };
            rows.push(row);
        }
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给表格设置数据
        $(hosts_table).jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);
    });
}
function hosts_save()
{
    // 清除所有旧规则
    hosts = {};
    // 根据表格数据生成hosts规则
    var rowNum =  $(hosts_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
    var total = $(hosts_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
    $(hosts_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
    var rows =  $(hosts_table).jqGrid('getDataIDs');
    for ( var i = 0; i < rows.length; i++ )
    {
        var row = $(hosts_table).jqGrid('getRowData', rows[i] );
        hosts[row.hostname] = row.ip;
    }
    $(hosts_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态        
    if ( ocompare( hosts, hostscopy ) )
    {
        page.alert( { message: $.i18n('Settings unchanged') } );
        return;
    }
    he.exec( [ obj+"="+JSON.stringify(hosts) ] ).then( function(){
        page.hint2succeed( $.i18n('Modify successfully') );
        hosts_load();
    });
}


/* init */
$.i18n().load( page.lang('hosts') ).then( function () {
    /* init the langauage */
    $.i18n().locale = window.lang; $('body').i18n();

    //初始化表格
    jqtable.create( hosts_table, hosts_pager,
        {
            caption: $.i18n('Hosts Table'),
            colNames: [ $.i18n('Hostname'), $.i18n('IP Address'), $.i18n('Operation') ],
            colModel:
            [
                {
                    name:'hostname', width:260, editable: true, 
                    editoptions:{ maxlength:'256' },
                    editrules:{ required: true }
                },
                {
                    name: 'ip', width: 130, editable: true,
                    editoptions: { maxlength:'15' },
                    editrules: { required: true }
                },
                jqtable.actionOptions,
            ],
            autowidth:true,
            loadonce:true,
            shrinkToFit:true,
            responsive:true,
        }
    ).jqGrid( 'navGrid', hosts_pager,
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
        $.extend(true,{},jqtable.addOptions,{
            afterShowForm:function(form){
                // 添加自定义样式和提示
                    $("label[for='hostname']", form).append('<span style="color: red; margin-left: 3px;">*</span>');
                    $("label[for='ip']", form).append('<span style="color: red; margin-left: 3px;">*</span>');

                    var hintText = '<div style="margin-bottom: 15px; padding: 8px 12px; background-color: #f8f9fa; border-left: 4px solid #007bff; border-radius: 3px;">' +
                        '<span style="color: red;">*</span> ' + $.i18n('Fields marked with * are required') +
                        '</div>';
                    
                    $("table > tbody > tr:first", form).before('<tr><td colspan="2">' + hintText + '</td></tr>');
                    // 设置 placeholder
                    $("#hostname", form).attr("placeholder", $.i18n('Enter Hostname'));
                    $("#ip", form).attr("placeholder", $.i18n('Enter IP'));
            }
        }),
        jqtable.deleteOptions
    );

    /* 加载参数 */
    hosts_load();
    /* bind the refresh */
    $('#refresh').on(ace.click_event, function () {
      location.reload();
    });
    /* bind the apply */
    $('#apply').on(ace.click_event, function () {
      hosts_save();
    });

});


