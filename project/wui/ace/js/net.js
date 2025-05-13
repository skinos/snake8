
var vlan;
var bridge;
var local;
var extern;
var ethercfg;
var etherstat;
var netcfg;
var framecfg;
var netobj = "arch@net";
var etherobj = "arch@ethernet";
var frameobj = "network@frame";
var list_table = '#list-grid-table';
var list_pager = '#list-grid-pager';
var vlan_table = '#vlan-grid-table';
var vlan_pager = '#vlan-grid-pager';
var bridge_table = '#bridge-grid-table';
var bridge_pager = '#bridge-grid-pager';
var local_table = '#local-grid-table';
var local_pager = '#local-grid-pager';
var extern_table = '#extern-grid-table';
var extern_pager = '#extern-grid-pager';

/* edit vlan custom */
function edit_vlan_submit( params, postdata )
{
    if ( postdata.identify )
    {
        postdata.name = "vlan@"+postdata.identify;
    }
    return postdata;
}
/* edit form custom */
function edit_bridge_before( form, action )
{
    $('#stp').unbind('change').change(function () {
        if ( $('#stp').val() === 'enable' )
        {
            $('#tr_hellotime').show();
            $('#tr_maxage').show();
            $('#tr_forwarddelay').show();
            $('#tr_prio').show();
        }
        else
        {
            $('#tr_hellotime').show();
            $('#tr_maxage').show();
            $('#tr_forwarddelay').show();
            $('#tr_prio').show();
        }
        $(window).triggerHandler('resize.jqGrid');            
    }).trigger('change');

    if (action == 'add')
    {
        // 将原输入框的值提取到时间下拉框里面
        $('#hellotime').val( "2" );
        $('#maxage').val( "20" );
        $('#forwarddelay').val( "4" );
        $('#prio').val( "32768" );
        $('#stp').val( "disable" );
    }
}
/* edit data custom */
function edit_bridge_submit( params, postdata )
{
    if ( postdata.identify )
    {
        postdata.name = "bridge@"+postdata.identify;
    }
    return postdata;
}
/* edit data custom */
function edit_local_before( form, action )
{
    $('#concom').attr( "disabled","disabled" );
    if ( $('#concom').val() == "" )
    {
        $('#concom').val( "ifname@ethcon" );
    }
}
function edit_local_submit( params, postdata )
{
    // 表单的行为为编辑
    if ( postdata.identify )
    {
        postdata.name = "ifname@"+postdata.identify;
    }
    return postdata;
}
function edit_extern_submit( params, postdata )
{
    // 表单的行为为编辑
    if ( postdata.identify )
    {
        postdata.name = "ifname@"+postdata.identify;
    }
    if ( postdata.identify == "wan" || postdata.identify == "wan2" || postdata.identify == "wan3" || postdata.identify == "wan4" )
    {
        if ( postdata.ifdev == "modem@lte" || postdata.ifdev == "modem@lte2" || postdata.ifdev == "wifi@nsta" || postdata.ifdev == "wifi@asta" )
        {
            postdata.ifdev = "";
        }
    }
    else if ( postdata.identify == "lte" || postdata.identify == "lte2" )
    {
        postdata.ifdev = "modem@"+postdata.identify;
    }
    else if ( postdata.identify == "wisp" )
    {
        postdata.ifdev = "wifi@nsta";
    }
    else if ( postdata.identify == "wisp2" )
    {
        postdata.ifdev = "wifi@asta";
    }
    return postdata;
}


/* load the table infomation */
function load_net_config()
{
    he.bkload( [ etherobj, etherobj+".status", netobj, frameobj ] ).then( function(v){
        ethercfg = v[0];
        etherstat = v[1];
        netcfg = v[2];
        vlan = netcfg["vlan"];
        bridge = netcfg["bridge"];
        framecfg = v[3];
        local = framecfg["local"];
        extern = framecfg["extern"];

        $('#mode').val(ethercfg.mode||"switch");
        // 保存所有的行
        var count = 0;
        var rows = [];
        for ( var i in ethercfg.phy )
        {
            var value = ethercfg.phy[i];
            var row = {};
            if ( !value["pid"] )
            {
                continue;
            }
            row[ 'key'] = i;
            row[ 'name'] =  $.i18n(i);
            row[ 'pid'] = value["pid"];
            if ( etherstat[i].status == "up" )
            {
                row[ 'status'] = '<i class="ace-icon fa fa-check green"></i>';
            }
            else
            {
                row[ 'status'] = '<i class="ace-icon fa fa-times red"></i>';
            }
            row[ 'pvid'] = value["pvid"];
            row[ 'vtype'] = value["vtype"];
            var vidsstr = null;
            var vids = value["vids"];
            for ( var id in vids )
            {
                if ( vidsstr )
                {
                    vidsstr += ";";
                    vidsstr += id;
                }
                else
                {
                    vidsstr = id;
                }
            }
            row[ 'vids'] = vidsstr;
            rows.push( row );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(list_table).jqGrid( "setCaption", $.i18n('Ethernet(ethernet@lan) Port Number') + '(' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给clients表格设置数据
        $(list_table).jqGrid('clearGridData').jqGrid('setGridParam',{data: rows}).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);
        $('#mode').unbind('change').change(function (e) {
          var type = e.target.value;
          switch (type)
          {
            case 'switch':
              $('#list-sets').hide();
              break;
            case 'vlan':
              $('#list-sets').show();
              break;
          }
        }).trigger('change');



        // 保存所有的行
        count = 0;
        rows = [];
        for ( var i in vlan )
        {
            var value = vlan[i];
            var row = {};

            row[ 'name'] =  "vlan@"+i;
            row[ 'identify'] = i;
            row[ 'ifdev'] = value["ifdev"];
            row[ 'vid'] = value["vid"];
            rows.push( row );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(vlan_table).jqGrid( "setCaption", $.i18n('VLAN Interface Device') + '(' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给clients表格设置数据
        $(vlan_table).jqGrid('clearGridData').jqGrid('setGridParam',{data: rows}).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);



        // 保存所有的行
        count = 0;
        rows = [];
        for ( var i in bridge )
        {
            var value = bridge[i];
            var row = {};

            row[ 'name'] = "bridge@"+i;
            row[ 'identify'] = i;
            row[ 'stp'] = value["stp"];
            row[ 'hellotime'] = value["hellotime"];
            row[ 'maxage'] = value["maxage"];
            row[ 'forwarddelay'] = value["forwarddelay"];
            row[ 'prio'] = value["prio"];

            var memstr = null;
            var mem = value["member"];
            for ( var id in mem )
            {
                if ( memstr )
                {
                    memstr += ";";
                    memstr += id;
                }
                else
                {
                    memstr = id;
                }
            }
            row[ 'member'] = memstr;

            rows.push( row );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(bridge_table).jqGrid( "setCaption", $.i18n('Bridge Interface Device') + '(' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给clients表格设置数据
        $(bridge_table).jqGrid('clearGridData').jqGrid('setGridParam',{data: rows}).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);



        // 保存所有的行
        count = 0;
        rows = [];
        for ( var i in local )
        {
            var value = local[i];
            var row = {};

            row[ 'name'] =  i;
            var na = i.split("@");
            row[ 'identify' ] = na[1];
            row[ 'ifdev'] = value["ifdev"];
            row[ 'concom'] = value["concom"];
            rows.push( row );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(local_table).jqGrid( "setCaption", $.i18n('Local Interface Name') + '(' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给clients表格设置数据
        $(local_table).jqGrid('clearGridData').jqGrid('setGridParam',{data: rows}).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);



        // 保存所有的行
        count = 0;
        rows = [];
        for ( var i in extern )
        {
            var value = extern[i];
            var row = {};

            row[ 'name'] =  i;
            var na = i.split("@");
            row[ 'identify' ] = na[1];
            row[ 'ifdev'] = value["ifdev"];
            row[ 'concom'] = value["concom"];
            rows.push( row );
            count++;
        }
        // 表头设置为在线客户端的数量
        $(extern_table).jqGrid( "setCaption", $.i18n('Extern Interface Name') + '(' + count + ')' );
        // 记住滚动条的位置
        var scrollPos = jqtable.getScrollPos();
        // 给clients表格设置数据
        $(extern_table).jqGrid('clearGridData').jqGrid('setGridParam',{data: rows}).trigger('reloadGrid');
        // 恢复滚动条的位置
        jqtable.setScrollPos(scrollPos);


        
    });
}
function save_net_config()
{
    var cmds = [];


    // 交换机清除所有旧规则
    var nethercfg = JSON.parse(JSON.stringify(ethercfg));
    ethercfg.mode = $('#mode').val();
    if ( ethercfg.mode == "vlan" )
    {
        var phy = {};
        // 根据表格数据生成hosts规则
        var rowNum =  $(list_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
        var total = $(list_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
        $(list_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
        var rows =  $(list_table).jqGrid('getDataIDs');
        for ( var i = 0; i < rows.length; i++ )
        {
            var row = $(list_table).jqGrid('getRowData', rows[i] );
            var port = {};
            port.pid = row.pid;
            port.vtype = row.vtype;
            port.pvid = row.pvid;
            var vids = {};
            var vidsa = row.vids.split(";");
            for ( var t = 0; t < vidsa.length; t++ )
            {
                vids[vidsa[t]] = "";
            }
            port.vids = vids;
            phy[row.key] = port;
        }
        $(list_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态
        ethercfg.phy = phy;
    }
    if ( !ocompare( ethercfg, nethercfg ) )
    {
        cmds.push( etherobj+"="+JSON.stringify(ethercfg) );
    }



    var nnetcfg = JSON.parse(JSON.stringify(netcfg));

    // VLAN清除所有旧规则
    var nvlan = {};
    // 根据表格数据生成hosts规则
    var rowNum =  $(vlan_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
    var total = $(vlan_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
    $(vlan_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
    var rows =  $(vlan_table).jqGrid('getDataIDs');
    for ( var i = 0; i < rows.length; i++ )
    {
        var row = $(vlan_table).jqGrid('getRowData', rows[i] );
        var vl = {};
        vl.ifdev = row.ifdev;
        vl.vid = row.vid;
        nvlan[row.identify] = vl;
    }
    $(vlan_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态
    netcfg["vlan"] = nvlan;

    // BRIDGE清除所有旧规则
    var nbridge = {};
    // 根据表格数据生成hosts规则
    var rowNum =  $(bridge_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
    var total = $(bridge_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
    $(bridge_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
    var rows =  $(bridge_table).jqGrid('getDataIDs');
    for ( var i = 0; i < rows.length; i++ )
    {
        var row = $(bridge_table).jqGrid('getRowData', rows[i] );
        var br = {};
        br.stp = row.stp;
        br.hellotime = row.hellotime;
        br.maxage = row.maxage;
        br.forwarddelay = row.forwarddelay;
        br.prio = row.prio;
        var mem = {};
        var mema = row.member.split(";");
        for ( var t = 0; t < mema.length; t++ )
        {
            mem[mema[t]] = "";
        }
        br.member = mem;
        nbridge[row.identify] = br;
    }
    $(bridge_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态
    netcfg["bridge"] = nbridge;

    if ( !ocompare( netcfg, nnetcfg ) )
    {
        cmds.push( netobj+"="+JSON.stringify(netcfg) );
    }



    var nframecfg = JSON.parse(JSON.stringify(framecfg));;

    // local清除所有旧规则
    var nlocal = {};
    // 根据表格数据生成hosts规则
    var rowNum =  $(local_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
    var total = $(local_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
    $(local_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
    var rows =  $(local_table).jqGrid('getDataIDs');
    for ( var i = 0; i < rows.length; i++ )
    {
        var row = $(local_table).jqGrid('getRowData', rows[i] );
        var la = {};
        la.ifdev = row.ifdev;
        la.concom = row.concom;
        nlocal[row.name] = la;
    }
    $(local_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态
    framecfg["local"] = nlocal;

    // extern清除所有旧规则
    var nextern = {};
    // 根据表格数据生成hosts规则
    var rowNum =  $(extern_table).jqGrid('getGridParam', 'rowNum');                   //获取当前页条数
    var total = $(extern_table).jqGrid('getGridParam', 'records');                    //获取总数 （包含未显示的数据）
    $(extern_table).jqGrid('setGridParam', { rowNum: total }).trigger('reloadGrid');  //把grid重新加载成全部数据（前端不会变化）
    var rows =  $(extern_table).jqGrid('getDataIDs');
    for ( var i = 0; i < rows.length; i++ )
    {
        var row = $(extern_table).jqGrid('getRowData', rows[i] );
        var la = {};
        la.ifdev = row.ifdev;
        la.concom = row.concom;
        nextern[row.name] = la;
    }
    $(extern_table).jqGrid('setGridParam', { rowNum: rowNum }).trigger('reloadGrid');  //还原成原先状态
    framecfg["extern"] = nextern;

    if ( !ocompare( framecfg, nframecfg ) )
    {
        cmds.push( frameobj+"="+JSON.stringify(framecfg) );
    }


    
    if ( cmds.length == 0 )
    {
        page.alert( { message: $.i18n('Settings unchanged') } );
        return;
    }        
    page.confirm( { message: $.i18n('The system will restart because of the change of configuration') } ).then( function(result){
        if ( result )
        {
            he.exec( cmds ).then( function(){
                page.confirm( { message: $.i18n('Need to restart the system') } ).then( function(result){
                    if ( result )
                    {
                        he.reboot( { title: $.i18n('Restarting to apply...'), hint:$.i18n('Make sure that the device is reconnected') } );
                    }
                    else
                    {
                        load_net_config();
                    }
                });
            });
        }
    });
}



$.i18n().load( page.lang('net') ).then( function () {
  /* init the langauage */
  $.i18n().locale = lang; $('body').i18n();

  /* init ethenret */
  jqtable.create(  list_table, list_pager,
    {
        caption: 'Ethernet(ethernet@lan) Port Number', // 必需设置值, 防止表格不能折叠
        colNames: [ $.i18n('Key'), $.i18n('Port Name'), $.i18n('Port Identify'), $.i18n('Status'), $.i18n('PVID'), $.i18n('Type'), $.i18n('VIDs(Separated by semicolon)') ],
        colModel: [
            { name:'key', editable:false, hidden: true },
            { name:'name', width:100, editable:false },
            { name:'pid', width:100, editable:false },
            { name:'status', width:80, editable:false },
            { name:'pvid', width:50, editable:true },
            {
                name: 'vtype', width:150, editable: true, formatter: 'select', edittype: 'select',
                editoptions: { value: 'untag:UNTAG;tag:TAG' }
            },
            { name:'vids', width:400, editable:true },
        ],
    }
  ).jqGrid( 
        'navGrid', list_pager,
        $.extend(true, {}, jqtable.navOptions, 
            {
                add: false,
                del: false,
                edit: true,editicon : 'ace-icon fa fa-pencil blue',
                view: false, search: false, refresh: false
            }
        ),
        $.extend(true, {}, jqtable.editOptions )
  );

  /* init vlan interface */
  jqtable.create(  vlan_table, vlan_pager,
    {
        caption: $.i18n('VLAN Interface Device'),
        colNames: [ $.i18n('Interface Device(ifdev)'), $.i18n('ID'), $.i18n('Baseon Interface Device(ifdev)'), $.i18n('VID'), $.i18n('Operation') ],
        colModel: [
            { name:'name', width:260, editable:false },
            { name:'identify', width:220, editable:true },
            {
                name: 'ifdev', width:220, editable: true, formatter: 'select', edittype: 'select',
                editoptions: { value: 'ethernet@lan:ethernet@lan' }
            },
            { name:'vid', width:160, editable:true },
            $.extend( true, {}, jqtable.actionOptions,
                {
                    formatoptions:
                    {
                        editOptions: { onclickSubmit: edit_vlan_submit }
                    }
                }
            )
        ],
    }
  ).jqGrid( 'navGrid', vlan_pager,
      $.extend(true, {}, jqtable.navOptions, 
          {
              add: true,addicon : 'ace-icon fa fa-plus-circle purple',
              del: true,delicon : 'ace-icon fa fa-trash-o red',
              edit: true,editicon : 'ace-icon fa fa-pencil blue',
              search: false, refresh: false, view: false
          }
      ),
      $.extend(true, {}, jqtable.editOptions, { onclickSubmit: edit_vlan_submit } ),
      $.extend(true, {}, jqtable.addOptions, { onclickSubmit: edit_vlan_submit } ),
      $.extend(true, {}, jqtable.deleteOptions ),
      {},
      jqtable.viewOptions
  );



  /* init bridge interface */
  jqtable.create(  bridge_table, bridge_pager,
    {
        caption: $.i18n('Bridge Interface Device'),
        colNames: [ $.i18n('Interface Device(ifdev)'), $.i18n('ID'), $.i18n('Members(ifdev Separated by semicolon)'), $.i18n('STP'), $.i18n('Hello Time'), $.i18n('Max Age'), $.i18n('Forword Delay'), $.i18n('Priority'), $.i18n('Operation') ],
        colModel: [
            { name:'name', width:150, editable:false },
            { name:'identify', width:50, editable:true },
            { name:'member', width:300, editable:true },
            {
                name: 'stp',
                width:60,
                editable: true,
                formatter: 'select',
                edittype: 'select',
                editoptions:
                {
                    value: 'enable:' + $.i18n('Enable') + ';disable:' + $.i18n('Disable')
                }
            },
            { name:'hellotime', width:80, editable:true },
            { name:'maxage', width:60, editable:true },
            { name:'forwarddelay', width:100, editable:true },
            { name:'prio', width:60, editable:true },
            $.extend( true, {}, jqtable.actionOptions,
                {
                    formatoptions:
                    {
                        editOptions: { beforeShowForm : edit_bridge_before, onclickSubmit: edit_bridge_submit }
                    }
                }
            )
        ],
    }
  ).jqGrid( 'navGrid', bridge_pager,
      $.extend(true, {}, jqtable.navOptions, 
          {
              add: true,addicon : 'ace-icon fa fa-plus-circle purple',
              del: true,delicon : 'ace-icon fa fa-trash-o red',
              edit: true,editicon : 'ace-icon fa fa-pencil blue',
              search: false, refresh: false, view: false
          }
      ),
      $.extend(true, {}, jqtable.editOptions, { beforeShowForm : edit_bridge_before, onclickSubmit: edit_bridge_submit } ),
      $.extend(true, {}, jqtable.addOptions, { beforeShowForm : edit_bridge_before, onclickSubmit: edit_bridge_submit } ),
      $.extend(true, {}, jqtable.deleteOptions ),
      {},
      jqtable.viewOptions
  );



  /* init local interface */
  jqtable.create(  local_table, local_pager,
    {
        caption: $.i18n('Local Interface Name'),
        colNames: [ $.i18n('Interface Name(ifname)'), $.i18n('Name'), $.i18n('Baseon Interface Device(ifdev)'), $.i18n('Connect Component'), $.i18n('Operation') ],
        colModel: [
            { name:'name', width:200, editable:false },
            {
                name: 'identify',
                width:100,
                editable: true,
                formatter: 'select',
                edittype: 'select',
                editoptions:
                {
                    value: 'lan:' + $.i18n('LAN') + ';lan2:' + $.i18n('LAN2') + ';lan3:' + $.i18n('LAN3') + ';lan4:' + $.i18n('LAN4')
                }
            },
            { name:'ifdev', width:200, editable:true },
            { name:'concom', width:150, editable:true },
            $.extend( true, {}, jqtable.actionOptions,
                {
                    formatoptions:
                    {
                        editOptions: { beforeShowForm : edit_local_before, onclickSubmit: edit_local_submit }
                    }
                }
            )
        ],
    }
  ).jqGrid( 'navGrid', local_pager,
      $.extend(true, {}, jqtable.navOptions, 
          {
              add: true,addicon : 'ace-icon fa fa-plus-circle purple',
              del: true,delicon : 'ace-icon fa fa-trash-o red',
              edit: true,editicon : 'ace-icon fa fa-pencil blue',
              search: false, refresh: false, view: false
          }
      ),
      $.extend(true, {}, jqtable.editOptions, { beforeShowForm : edit_local_before, onclickSubmit: edit_local_submit } ),
      $.extend(true, {}, jqtable.addOptions, { beforeShowForm : edit_local_before, onclickSubmit: edit_local_submit } ),
      $.extend(true, {}, jqtable.deleteOptions ),
      {},
      jqtable.viewOptions
  );


  /* init extern interface */
  jqtable.create(  extern_table, extern_pager,
    {
        caption: $.i18n('Extern Interface Name'),
        colNames: [ $.i18n('Interface Name(ifname)'), $.i18n('Name'), $.i18n('Baseon Interface Device(ifdev)'), $.i18n('Connect Component'), $.i18n('Operation') ],
        colModel: [
            { name:'name', width:200, editable:false },
            {
                name: 'identify',
                width:100,
                editable: true,
                formatter: 'select',
                edittype: 'select',
                editoptions:
                {
                    value: 'wan:' + $.i18n('WAN') + ';wan2:' + $.i18n('WAN2') + ';wan3:' + $.i18n('WAN3') + ';wan4:' + $.i18n('WAN4') + ';lte:' + $.i18n('LTE') + ';lte2:' + $.i18n('LTE2') + ';wisp:' + $.i18n('WISP') + ';wisp2:' + $.i18n('WISP2')
                }
            },
            { name:'ifdev', width:200, editable:true },
            { name:'concom', width:150, editable:true },
            $.extend( true, {}, jqtable.actionOptions,
                {
                    formatoptions:
                    {
                        editOptions: { beforeShowForm : edit_local_before, onclickSubmit: edit_extern_submit }
                    }
                }
            )
        ],
    }
  ).jqGrid( 'navGrid', extern_pager,
      $.extend(true, {}, jqtable.navOptions, 
          {
              add: true,addicon : 'ace-icon fa fa-plus-circle purple',
              del: true,delicon : 'ace-icon fa fa-trash-o red',
              edit: true,editicon : 'ace-icon fa fa-pencil blue',
              search: false, refresh: false, view: false
          }
      ),
      $.extend(true, {}, jqtable.editOptions, { beforeShowForm : edit_local_before, onclickSubmit: edit_extern_submit } ),
      $.extend(true, {}, jqtable.addOptions, { beforeShowForm : edit_local_before, onclickSubmit: edit_extern_submit } ),
      $.extend(true, {}, jqtable.deleteOptions ),
      {},
      jqtable.viewOptions
  );



  /* load the configure */
  load_net_config();
  /* bind the refresh */
  $('#refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply */
  $('#apply').on(ace.click_event, function () {
    save_net_config();
  });

});

