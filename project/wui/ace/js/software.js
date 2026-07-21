
var projects;
var project_table = '#project-grid-table';
var newst_url = null;

/* show the FPK table */
function project_load()
{
  he.load( [ 'land@fpk.list' ] ).then( function(v){
      projects = v[0];
      var count = 0;
      var rows = [];
      for (var index in projects)
      {
        var prj = projects[index];
        //var path = prj.path;
        //if ( path && 0 == path.indexOf("/prj" ) )
        //{
        //    continue;
        //}
        rows.push( { pname: index, size:prj.size||'', version:prj.version||'', path:prj.path||'', author:prj.author||'', intro:prj.intro||'' } )
        count++;
      }
      // 表头设置为在线客户端的数量
      $(project_table).jqGrid( "setCaption", $.i18n('FPK Number') + '(' + count + ')' );
      // 记住滚动条的位置
      var scrollPos = jqtable.getScrollPos();
      // load data into the projects grid
      $(project_table).jqGrid( 'clearGridData' ).jqGrid( 'setGridParam', { data: rows } ).trigger( 'reloadGrid' );
      // 恢复滚动条的位置
      jqtable.setScrollPos(scrollPos);
  });
}
/* delete the project */
function delete_project( indexStr )
{
    var key;
    // 选中的行
    var indexs = indexStr.split(',');
    // 删除路由的he命令
    var cmds = [];
    for ( var index  = 0; index < indexs.length; index++ )
    {
        // 选中行的数据
        var row = $(project_table).jqGrid('getRowData', indexs[index]);
        key = row.pname;
        var path = projects[key].path;
        if ( path && 0 == path.indexOf("/prj" ) )
        {
            page.alert( { message: $.i18n("Cannot delete the software buildin firmware") } );
            return;
        }
        cmds.push( 'land@fpk.delete[ '+key+']' );
    }
    // 执行删除
    he.exec( cmds ).then(function (){
        // 重新加载数据
        project_load();
    }).then(function (){
        // 提示成功
        page.alert( { message: $.i18n('Delete successfully'), callback: function( result ){location.reload();} } );
    });
}



/* init */
$.i18n().load( page.lang('software') ).then( function () {

  /* init the language */
  $.i18n().locale = lang; $('body').i18n();

  $('#software_html').show();

  /* load the firmware info */
  if ( window.wui.bigversion && window.wui.bigversion == "enable" )
  {
    var v = window.machines.version.split('.');
    $('#firmware_version').text( v[0]+"."+v[1] );
  }
  else
  {
    $('#firmware_version').text(window.machines.version);
  }
  $('#firmware_sdk').text( window.machines.platform );
  $('#firmware_id').text( window.machines.hardware+"-"+window.machines.custom+"-"+window.machines.scope );
  if ( !window.wui.firmware_id || window.wui.firmware_id != "disable" )
  {
      $("#firmware_sdk").closest('.form-group').show();
      $("#firmware_id").closest('.form-group').show();
  }

  // upgrade local
  $('#upgrade').ace_file_input( { no_file: $.i18n('No File'), btn_choose: $.i18n('Choose'), btn_change: $.i18n('Change'), icon_remove: '' } );
  // bind the upgrade
  $('#upgrade').fileupload({
	url:'/upload?username='+window.username+'&key='+window.talkkey+'&object=arch@firmware&api=zz&p=[]',
    paramName: 'filename',
    dataType: 'text',
    add: function( e, data )
	{ // 选择文件之后，执行升级之前
		page.confirm( { message: $.i18n('The device may need to be restarted, confirm whether to continue'),
			callback: function( result ) {
				if ( result )
				{
					var url = '/upload?username='+window.username+'&key='+window.talkkey+'&object=arch@firmware&api=zz&p=[]';
					if ( $('#restart').is(':checked') )
					{
						url += '&p2=restart';
					}
					data.url = url;
					// 提示正在升级
					page.overlay($.i18n('Upgrading...'));
					// 执行升级
					data.submit();
				}
				else
				{
					location.reload();
				}
			}
		})
    },
    done: function( e, data )
    {
		var hint = "";
		var wait = null;
		var ret = JSON.parse( data.result );
		var result = ret["return"];
		if ( result.msgs )
		{
			for( var ind in result.msgs )
			{
				hint += ind;
				hint += "---";
				hint += $.i18n( result.msgs[ind] );
				hint += "<br>";
			}
		}
		else if ( result.msg )
		{
			hint = $.i18n( result.msg );
		}
		if ( result.wait )
		{
			wait = result.wait;
		}
		page.overlay2hide();
		if ( result.status === 'success' )
		{ // 升级成功
			if ( $('#restart').is(':checked') )
			{ // 用户已勾选重启，直接提示后进入重启进度条
				if ( !hint )
				{
					hint = $.i18n( "Success" );
				}
				page.alert( { message: hint, callback: function()
				{
					he.upgrade_reboot( { title: $.i18n('Update successfully, now restarting...'), hint:$.i18n('Make sure that the device is reconnected'), restartTime:wait, norestart: true } );
				}});
			}
			else if ( !result.restart )
			{
				if ( !hint )
				{
					hint = $.i18n( "Success" );
				}
				page.alert( { message: hint, callback: function( result ){location.reload();} } );
			}
			else
			{
				if ( result.msgs )
				{
					hint += "<br><br><center>"+$.i18n( "Restart the system or not" )+"</center>";
				}
				else if ( hint )
				{
					hint += ", "+$.i18n( "Restart the system or not" );
				}
				else
				{
					hint = $.i18n( "Success" )+", "+$.i18n( "Restart the system or not" );
				}
				page.confirm( { message: hint,	callback: function( result )
				{
					if ( result )
					{
						he.upgrade_reboot( { title: $.i18n('Update successfully, now restarting...'), hint:$.i18n('Make sure that the device is reconnected'), restartTime:wait } );
					}
					else
					{
						location.reload();
					}
				}});
			}
		}
		else
		{ // 升级失败
			page.alert( { message: hint } );
		}
	}
  });


  if ( window.wui.upgrade_online && window.wui.upgrade_online == "enable" )
  {
      $("#upgrade_online").show();
  }
  if ( window.wui.repo_online && window.wui.repo_online == "enable" )
  {
      $("#repo_online").show();
  }
  // check new version online
  $('#check').unbind(ace.click_event).on(ace.click_event, function () {
      he.exec( [ 'arch@firmware.online_check' ], $.i18n("Checking...") ).then( function(v){
        if ( !v[0] )
        {
            page.alert( { message: $.i18n('No new version'), callback: function( result ){
                $('#new_version').hide();
                $('#new_changelog').hide();
            } } );
        }
        else
        {
            if ( ! v[0].url )
            {
                page.alert( { message: $.i18n( "It's the latest version"), callback: function( result ){
                    $('#new_version').hide();
                    if ( v[0].changelog )
                    {
                        var log = "";
                        for ( var i in v[0].changelog )
                        {
                            log += i;
                            log += ":";
                            log += v[0].changelog[i];
                            log += "<br>";
                        }
                        $('#changelog').html( log );
                        $('#new_changelog').show();
                    }
                    else
                    {
                        $('#new_changelog').hide();
                    }
                } } );
            }
            else
            {
                newst_url = v[0].url;
                $('#online_version').html( v[0].version+ '   <i class="ace-icon fa fa-arrow-up">' );
                $('#new_version').show();
                if ( v[0].changelog )
                {
                    var log = "";
                    for ( var i in v[0].changelog )
                    {
                        log += i;
                        log += ":";
                        log += v[0].changelog[i];
                        log += "<br>";
                    }
                    $('#changelog').html( log );
                    $('#new_changelog').show();
                }
                else
                {
                    $('#new_changelog').hide();
                }
            }
        }
      });
  });
  /* upgrade online */
  $('#online_version').unbind(ace.click_event).on(ace.click_event, function () {
      if ( !newst_url )
      {
          page.hint2warning($.i18n('No new version'));
          return;
      }
      page.confirm( { message: $.i18n('Are you sure you want to upgrade the newst version online, it will take long time') } ).then( function(result){
          if ( result )
          {
              he.exec( [ 'arch@firmware.online_upgrade['+newst_url+']' ], $.i18n("Upgrading...") ).then( function(v){
                if ( v[0] )
                {
                    page.confirm( { message: $.i18n("Upgrade online successfully, Restart the system or not"),
                      callback: function( result )
                      {
                        if ( result )
                        {
                          he.upgrade_reboot( { title: $.i18n('Upgrade online successfully, now restarting...'), hint:$.i18n('Make sure that the device is reconnected') } );
                        }
                        else
                        {
                          location.reload();
                        }
                      }
                    });
                
                }
                else
                {
                    page.alert( { message: $.i18n("Upgrade online Failure, Please try again"), callback: function( result ){} } );
                }
              });
          }
      });
      
  });
  // downlaod all change log
  $('#online_changelog').on(ace.click_event, function (e) {
      e.preventDefault();
      var submitForm = document.createElement("FORM");
      document.body.appendChild(submitForm);
      submitForm.method= "GET";
      submitForm.action= "/cgi/changelog";
      submitForm.submit();
  });


  /* init the table */
  jqtable.create( project_table, null, 
    {
        caption: ' ', // 必需设置值, 防止表格不能折叠
        multiselect: false,
        colNames: [$.i18n('Name'), $.i18n('Size'), $.i18n('Version'), $.i18n('Path'), $.i18n('Author'), $.i18n('Introduction'), $.i18n('Operation') ],
        colModel:
        [
          { name:'pname', width:100 },
          { name:'size', width:100 },
          { name:'version', width:80 },
          { name:'path', width:180 },
          { name:'author', width:150 },
          { name:'intro', width:300 },
          $.extend( true, {}, jqtable.actionOptions,
              { formatoptions:{ delOptions:{ onclickSubmit:function(params, data) { delete_project(data); } }, editformbutton:false, editbutton:false } } )
        ]
    }
  );
  // release
  $('#release').on(ace.click_event, function () {
      page.confirm( { message: $.i18n('Are you sure you want to clear all install FPK and default the system configure') } ).then( function(result){
          if ( result )
          {
              he.upgrade_reboot( { cmds:["arch@data.release"] } );
          }
      });
  });
  project_load();
  
});

