var lte;
var modem = page.param('modem', location.hash);   
var ifname = page.param('object', location.hash);

var smslist_table = '#smslist-grid-table';  
var smslist_pager = '#smslist-grid-pager';  
//var obj = window.object || page.param('object', location.hash);

function lte_sms() {
    window.LteConfigManager.loadSettings(modem, ifname, true).then(function(v) {
    lte = v[0];
    $("#button_sms").show();
    $('#sms').prop('checked', lte.sms === "enable");  

    // 根据开关状态来调整表格宽度 需要处理好表格还没有正式渲染好的处理
    $('#sms').off('change').on('change', function () {  
        if ($(this).prop('checked')) {  
            // 计算容器宽度并调整 jqGrid (防止表格错位)
            var containerWidth = $('#sms_cfg').parent().width() || $(window).width() - 100;  
            // 修改判断逻辑
            if ($(smslist_table)[0].grid) { 
                $(smslist_table).jqGrid('setGridWidth', containerWidth);
            }
            $('#sms_cfg').show();  
        } else {  
            $('#sms_cfg').hide();  
        }  
    }).trigger('change');  

    if (lte.sms_cfg) {  
        $('#he_contact').val(lte.sms_cfg.he_contact || '');  
        $('#he_prefix').val(lte.sms_cfg.he_prefix || '');  
        $('#he').prop('checked', lte.sms_cfg.he === "enable");  
    } else {
        $('#he').prop('checked', false);
    }

    $('#he').off('change').on('change', function () {  
        $(this).prop('checked') ? $('#he_cfg').show() : $('#he_cfg').hide();  
    }).trigger('change');  
    });
}

function smslist_load()
{
    window.LteConfigManager.loadSettings(modem, ifname, true).then(function(v) {
    var list = v[7];
    if (!list) {
        list = {};
    }
    
    var count = 0;
    var rows = [];
    for ( var id in list )
    {
        var row = { sid: id, contact: list[id].contact, date: list[id].date, content: list[id].content };
        rows.push(row);
        count++;
    }
    // 表头设置为在线客户端的数量
    $(smslist_table).jqGrid("setCaption", $.i18n('SMS List') + '(' + count + ')' );
    // 记住滚动条的位置
    var scrollPos = jqtable.getScrollPos();
    // 给表格设置数据
    $(smslist_table).jqGrid('clearGridData').jqGrid('setGridParam', { data: rows }).trigger('reloadGrid');
    // 恢复滚动条的位置
    jqtable.setScrollPos(scrollPos);
  })
}
// 删除多个短信
function delete_smss( indexStr )
{
    var key;
    // 选中的行
    var indexs = indexStr.split(',');
    // 删除路由的he命令
    var cmds = [];
    for ( var index  = 0; index < indexs.length; index++ )
    {
        // 选中行的数据
        var row = $(smslist_table).jqGrid('getRowData', indexs[index]);
        key = row.sid;
        // 通过sid删除
        cmds.push( modem+'.smsdel[' + row.sid + ']' );
    }
    // 执行删除
    he.exec( cmds ).then(function (){
        // 重新加载数据
        return smslist_load();
    }).then(function (){
        // 提示成功
        page.hint2succeed( $.i18n('Delete successfully') );
    });
}


/* save the configure */
function sms_save()
{
  if ( lte == null )
  {
    return;
  }
  var ltecopy = JSON.parse(JSON.stringify(lte));;

  lte.sms = boole2able( $('#sms').prop('checked') );
  if ( lte.sms == "enable" )
  {
    console.log(lte.sms_cfg)
      if ( !lte.sms_cfg )
      {
        lte.sms_cfg = {};
      }
      lte.sms_cfg.he = boole2able( $('#he').prop('checked') );
      if ( lte.sms_cfg.he == "enable" )
      {
        lte.sms_cfg.he_contact = $('#he_contact').val();
        lte.sms_cfg.he_prefix = $('#he_prefix').val();
        if ( lte.sms_cfg.he_contact == "" )
        {
            page.alert( { message: $.i18n("Command Contact")+" "+$.i18n('Can not be empty') } );
            return;
        }

        if ( lte.sms_cfg.he_prefix == "" )
        {
            page.alert( { message: $.i18n("Command Prefix")+" "+$.i18n('Can not be empty') } );
            return;
        }
      }
  }
  if ( ocompare( lte, ltecopy ) )
  {
      page.alert( { message: $.i18n('Settings unchanged') } );
      return;
  }

  var msg = $.i18n('The LTE connection will be disconnected because of the change of configuration');
  page.confirm( { message: msg } ).then( function(result){
    if (!result) return location.reload();
    
    var cmds = [ modem+"="+JSON.stringify(lte) ];
    he.exec(cmds).then( function(){
      page.hint2succeed( $.i18n('Modify successfully') );
      lte_sms();
    });
    
  });
}

function init_sms(){
  var $smslistTable = $(smslist_table);
  // 调试bug通过
  if ($smslistTable.length === 0) {
      //console.warn("Table elements not found, skipping init_at.");
      return false; 
  }

  if ($smslistTable[0].grid) {
      return true; 
  }

  //初始化表格
  jqtable.create( smslist_table, smslist_pager,
      {
          caption: ' ', // 必需设置值, 防止表格不能折叠
          toolbar: [true, "top"],
          colNames: [ $.i18n('Contact'), $.i18n('SMS ID'), $.i18n('Date'), $.i18n('Content'), $.i18n('Operation') ],
          colModel:
          [
              {
                  name:'contact', width:200, editable: false
              },
              {
                  name:'sid', width:200, editable: false,  hidden: true
              },
              {
                  name:'date', width:150, editable: false
              },
              {
                  name: 'content', width: 500, editable: false
              },
              $.extend( true, {}, jqtable.actionOptions,
                  { formatoptions:{ delOptions:{ onclickSubmit:function(params, data) { delete_smss(data); } }, editformbutton:false, editbutton:false } } )
          ],
          pager: '#smslist-grid-pager',
          rowNum: 10,
          viewrecords: true,

          pgbuttons: true,
          pagerpos:'center',
          pginput:true,

          autowidth:true,
          loadonce:true,
          shrinkToFit:true,
          responsive:true,
          
      }
  ).jqGrid( 'navGrid', smslist_pager,
      $.extend(true, {}, jqtable.navOptions, 
          {
              add: false,
              del: true,delicon : 'ace-icon fa fa-trash-o red',
              edit: false,
              search: false, refresh: false, view: true
          }
      ),
      null,
      null,
      $.extend(true, {}, jqtable.deleteOptions, { onclickSubmit: function(params, data) { delete_smss(data); } })
  );

    var $toolbar = $("#t_" + smslist_table.replace('#', ''));
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
        $(smslist_table).jqGrid('setGridParam',{rowNum:newRowNum}).trigger('reloadGrid')
    });
    //console.log("初始化成功啦")
    return true;
}

/* init */
page.password('passwd', 'password-icon' );
$.i18n().load( page.lang('lte') ).then( function () {
  /* init the language */
  $.i18n().locale = lang; $('body').i18n();
  /* init the table */
  //init_sms();

  /* load the configure */
  //lte_sms();

  // 设置定时器
  //smslist_load();
  function safeStart() {
        if (init_sms()) {
            // 只有表格初始化成功了 才加载配置和数据
            lte_sms();
            smslist_load();
            //console.log("LTE SMS Page Initialized Successfully.");
        } else {
            // 如果init_sms失败 找不到元素 说明DOM还没渲染完 100ms 后重试
            //console.log("DOM not ready, retrying in 100ms...");
            setTimeout(safeStart, 100);
        }
    }

    safeStart();

  /* bind the refresh */
  $('#sms_refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply */
  $('#sms_apply').off('click').on('click', function() {
        sms_save();
    });
});

