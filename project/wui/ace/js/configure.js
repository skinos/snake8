
$.i18n().load( page.lang('configure') ).then( function () {
    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    /* load the info */
    $('#cfgversion').text( window.machine.cfgversion||$.i18n('default') );

    // 备份配置
    var d = new Date();
	var year= d.getFullYear();
	var month= d.getMonth()+1;
	if ( month < 10 )
	{
		month = '0' + month;
	}
	var date= d.getDate();
	if ( date < 10 )
	{
		date = '0' + date;
	}
	var dtitle = ""+year+month+date;
	var backfile = "backup-"+dtitle+".tar";
    $('#backup').on(ace.click_event, function () {
		var opt;
        var sform = document.createElement("FORM");
        document.body.appendChild(sform);
		sform.hidden = true;
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "username"; opt.value = window.username; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "key"; opt.value = window.talkkey; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "object"; opt.value = "arch@data"; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "api"; opt.value = "backup"; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "p"; opt.value = dtitle; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "a"; opt.value = "path"; sform.appendChild( opt );
		opt = document.createElement("input"); opt.setAttribute("type", "hidden"); opt.name = "f"; opt.value = backfile; sform.appendChild( opt );
        sform.method= "GET";
        sform.action= "/download";
        setTimeout(function() {
            sform.submit();
        }, 200);
    });

    // 恢复默认设置
    $('#default').on(ace.click_event, function () {
        page.confirm( { message: $.i18n('Are you sure you want to default the system configure') } ).then( function(result){
            if ( result )
            {
                he.reboot( { title: $.i18n('Restarting...'), hint:$.i18n('Make sure that the device is reconnected'), cmds:["arch@data.default"] } );
            }
        });
    });

    // 导入配置
    $('#restore').ace_file_input( { no_file: $.i18n('No File'), btn_choose: $.i18n('Choose'), btn_change: $.i18n('Change'), icon_remove: '' } );
    $('#restore').fileupload({
        url:'/upload?username='+window.username+'&key='+window.talkkey+'&object=arch@data&api=restore',
        paramName: 'filename',
        dataType: 'text',
        add: function (e, data)
        { // 选择文件之后，执行导入之前
            page.confirm( { message: $.i18n('Need to restart, whether to continue'),
                callback: function (result) {
                    if (result)
                    {
                        // 提示正在导入配置
                        page.overlay($.i18n('Restore...'));
                        // 执行导入
                        data.submit();
                    }
                }
            })
        },
        done: function (e, data)
        {
			var ret = JSON.parse( data.result );
			var result = ret["return"];
            // 隐藏遮罩
            page.overlay2hide();
            if ( result === 'ttrue' )
            { // 导入成功重启
                he.reboot( { title: $.i18n('Restore the configuration is successful, now restarting...'), hint:$.i18n('Make sure that the device is reconnected') } );
            }
            else
            {
                // 提示升级失败
                page.alert( { message: $.i18n('Restore Failure') } ).then( function(){ location.reload(); } );
            }
        }
    });

});

