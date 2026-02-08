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
var smslist_table = '#smslist-grid-table';
var smslist_pager = '#smslist-grid-pager';



/* load the configure on the input */
function config_load()
{
  he.load( [ object ] ).then( function(v){
    lte = v[0];
    if ( !lte )
    {
        lte = {};
    }
    if ( lte.sms == "enable" )
    {
        $('#sms').prop('checked', true );
    }
    else
    {
        $('#sms').prop('checked', false );
    }
    $('#sms').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        var containerWidth = $('#sms_cfg').parent().width() || 
                           $(window).width() - 100; // 估算
        
        // 设置表格宽度后再显示
        if ($(smslist_table).hasClass('ui-jqgrid-btable')) {
            $(smslist_table).jqGrid('setGridWidth', containerWidth);
        }

        $('#sms_cfg').show();
      } 
      else
      {
        $('#sms_cfg').hide();
      }
    }).trigger('change');

    if ( lte.sms_cfg )
    {
        $('#he_contact').val( lte.sms_cfg.he_contact||'' );
        $('#he_prefix').val( lte.sms_cfg.he_prefix||'' );
        if ( lte.sms_cfg.he == "enable" )
        {
            $('#he').prop('checked', true );
        }
        else
        {
            $('#he').prop('checked', false );
        }
    }
    $('#he').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#he_cfg').show();
      } 
      else
      {
        $('#he_cfg').hide();
      }
    }).trigger('change');

  })
}

function smslist_load()
{
  he.bkload( [ object+".smslist" ] ).then( function(v){
    var count = 0;
    var list = v[0];
    if ( !list )
    {
        return;
    }
    var rows = [];
    for ( var id in list )
    {
        var row = { sid: id, contact: list[id].contact, date: list[id].date, content: list[id].content };
        rows.push(row);
        count++;
    }
    // 表头设置为在线客户端的数量
    $(smslist_table).jqGrid( "setCaption", $.i18n('SMS List') + '(' + count + ')' );
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
        cmds.push( object+'.smsdel[' + row.sid + ']' );
    }
    // 执行删除
    he.exec( cmds ).then(function (){
        // 重新加载数据
        return route_load();
    }).then(function (){
        // 提示成功
        page.hint2succeed( $.i18n('Delete successfully') );
    });
}


/* save the configure */
function config_save()
{
  if ( lte == null )
  {
    return;
  }
  var ltecopy = JSON.parse(JSON.stringify(lte));;

  lte.sms = boole2able( $('#sms').prop('checked') );
  if ( lte.sms == "enable" )
  {
      if ( !lte.sms_cfg )
      {
        lte.sms_cfg = {};
      }
      lte.sms_cfg.he = boole2able( $('#he').prop('checked') );
      if ( lte.sms_cfg.he == "enable" )
      {
        lte.sms_cfg.he_contact = $('#he_contact').val();
        lte.sms_cfg.he_prefix = $('#he_prefix').val();
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
  page.confirm( { message: $.i18n('The LTE connecttion will be disconneted because of the change of configuration') } ).then( function(result){
    if ( result )
    {
      he.save( [ object+"="+JSON.stringify(lte)] ).then( function(){
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

    var $toolbar = $("#t_" + smslist_table_table.replace('#', ''));
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

    /* load the configure */
    config_load();

  // 设置定时器
  smslist_load();
  //page.timing({
  //  refresh: function ()
  //  {
  //      smslist_load();
  //  },
  //  interval: 10000
  //});

  /* bind the refresh */
  $('#refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply */
  $('#apply').on(ace.click_event, function () {
    config_save();
  });
});

