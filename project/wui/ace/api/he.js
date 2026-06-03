


/**
*
*  communication to system API
*  http://www.ashyelf.com/
*
**/
function shouldIgnoreAuthError()
{
    return sessionStorage.getItem("ill_direct_entry") === "true";
}
var he =
{
    /* excute cmd api */
    cmd:function ( a, args, func )
    {
        var i;
        var t;
        var ret;
        var uri;
        var hekey;
        var heindex;
        var htmlobj;
        var callbak;
        var timeout;
        var paramter;

        if ( a == null )
        {
            return null;
        }
        // timeout
        if ( args && args.timeout )
        {
            timeout = args.timeout;
        }
        else
        {
            timeout = 0;
        }
        // make the json command
        t = 0;
        heindex = {};
        paramter = {};
        if ( a instanceof Array )               // array
        {
            for ( i in a )
            {
                if ( t == 0 )
                {
                    hekey = "he";
                }
                else
                {
                    hekey = "he" + t;
                }
                heindex[ hekey ] = i;
				paramter[hekey] = a[i];
                t++;
            }
        }
        else if ( a instanceof Object )         // object
        {
            for ( i in a )
            {
                if ( t == 0 )
                {
                    hekey = "he";
                }
                else
                {
                    hekey = "he" + t;
                }
                heindex[ hekey ] = i;
				paramter[hekey] = a[i];
                t++;
            }
        }
        if ( paramter == {} )
        {
            return null;
        }
		//console.log( "POST:"+window.talkkey );
        paramter["key"] = window.talkkey;
		paramter["username"] = window.username;

        // 打印交互数据
        console.log( "Request:", paramter );

        /* get the uri */
        uri = window.hepath+"?rand=" + Math.random();
        /* show the loading */
        if ( args && args.loading )
        {
            page.overlay( args.loading );
        }
        /* get the async */
        if ( func != null )
        {
            $.ajax({
                'url':uri, 'type':'POST', 'timeout':timeout, 'async':true, dataType: "json", 'contentType':'application/json', 'data':JSON.stringify(paramter),
                'complete': function ( x, s )
                {

                    // return while in rebooting
                    if ( window.rebooting )
                    {
                        return;
                    }
                    // string for Exception
                    if ( x.responseText.indexOf("{" ) < 0 )
                    {
                        console.log( "Server Async Response String: "+x.responseText );
						if ( x.responseText == "Auth Error" )
                        {
                            console.log("Auth Error");
                            if (shouldIgnoreAuthError())
                            {
                                console.warn("Auth Error ignored in ill factory mode");
                                func(x.responseText);
                                return;
                            }
                            window.location.href = 'login.html';
                            return;
                        }
						else if ( x.responseText == "Data Error"  )
						{
							console.log( "Data Error" );
						}
						func( x.responseText );
                    }
                    else
                    {
                        callbak = eval( "(" + x.responseText + ")" );
                        // 打印交互数据
                        console.log( "Return:", callbak );

                        if ( callbak == null )
                        {
                            console.log( "Server Response Not JSON: "+x.responseText );
                            return;
                        }
                        var value = new Object();
						if ( callbak["key"] )
						{
							window.talkkey = callbak["key"];
							sessionStorage.setItem( "talkkey", window.talkkey );
						}
                        for ( i in heindex )
                        {
                            if ( typeof callbak[i] == "string" )
                            {
                                if ( callbak[i] == "ttrue" || callbak[i] == "true")
                                {
                                    value[ heindex[i] ] = true;
                                    continue;
                                }
                                else if ( callbak[i].indexOf("tfalse") == 0 || callbak[i].indexOf("false") == 0 )
								{
                                    value[ heindex[i] ] = false;
                                    continue;
								}
								else if ( callbak[i].indexOf("terror") == 0 || callbak[i].indexOf("tpanic") == 0 )
                                {
                                    value[ heindex[i] ] = false;
									console.log( "Server Response terror for: "+paramter[i] );
                                    continue;
                                }
								else if ( callbak[i] == "NULL" )
								{
                                    value[ heindex[i] ] = null;
									continue;
								}
                            }
                            value[ heindex[i] ] = callbak[i];
                        }
                        ret = value;
                        func( ret );
                    }
                    if ( args && args.loading )
                    {
                        page.overlay2hide();
                    }
                }
            });
        }
        else
        {
            htmlobj = $.ajax( { 'url':uri, 'type':'POST', 'timeout':timeout, 'async':false, dataType: "json", 'contentType':'application/json', 'data':JSON.stringify(paramter) } );

            // return while in rebooting
            if ( window.rebooting )
            {
                return null;
            }
            // string for Exception
            if ( htmlobj.responseText.indexOf("{" ) < 0 )
            {
                console.log( "Server Response String: "+htmlobj.responseText );
				if ( htmlobj.responseText == "Auth Error" )
                {
                    console.log("Auth Error");
                    if (shouldIgnoreAuthError())
                    {
                        console.warn("Auth Error ignored in ill factory mode");
                        ret = htmlobj.responseText;
                        return ret;
                    }
                    window.location.href = 'login.html';
                    return;
                }
				else if ( htmlobj.responseText == "Data Error"  )
				{
					console.log( "Data Error" );
				}
				ret = htmlobj.responseText;
            }
            else
            {
                callbak = eval( "(" + htmlobj.responseText + ")" );
                // 打印交互数据
                console.log( "Return:", callbak );
                if ( callbak == null )
                {
                    console.log( "Server Sync Response Not JSON: "+htmlobj.responseText );
                    ret = null;
                }
                else
                {
                    var value = new Object();
					if ( callbak["key"] )
					{
						window.talkkey = callbak["key"];
						sessionStorage.setItem( "talkkey", window.talkkey );
					}
                    for ( i in heindex )
                    {
                        if ( typeof callbak[ i ] == "string" )
                        {
							if ( callbak[i] == "ttrue" || callbak[i] == "true")
							{
								value[ heindex[i] ] = true;
								continue;
							}
							else if ( callbak[i].indexOf("tfalse") == 0 || callbak[i].indexOf("false") == 0 )
							{
								value[ heindex[i] ] = false;
								continue;
							}
							else if ( callbak[i].indexOf("terror") == 0 || callbak[i].indexOf("tpanic") == 0 )
							{
								value[ heindex[i] ] = false;
								console.log( "Server Response terror for: "+heindex[i] );
								continue;
							}
    						else if ( callbak[ i ] == "NULL" )
    						{
    							value[ heindex[ i ] ] = null;
                                continue;
    						}
                        }
                        value[ heindex[i] ] = callbak[i];
                    }
                    ret = value;
                }
            }
            if ( args && args.loading )
            {
                page.overlay2hide();
            }
            return ret;
        }
    },

    load:function( a, loading )
    {
        var dfd = $.Deferred();
        page.overlay( loading|| $.i18n('Loading') );
        this.cmd( a, null, function (v) {
            page.overlay2hide();
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
    bkload:function( a )
    {
        var dfd = $.Deferred();
        this.cmd( a, null, function (v) {
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },

    exec:function( a, loading )
    {
        var dfd = $.Deferred();
        page.overlay( loading||$.i18n('Running') );
        this.cmd( a, null, function (v) {
            page.overlay2hide();
            if ( typeof v == "string" )
            {
                page.alert( { message: $.i18n( v ) } );
            }
            else if ( typeof v == "object" )
            {
                dfd.resolve(v);
            }
        })
        return dfd.promise();
    },
	bkexec:function( a )
	{
		var dfd = $.Deferred();
		this.cmd( a, null, function (v) {
			if ( typeof v == "string" )
			{
				page.alert( { message: $.i18n( v ) } );
			}
			else if ( typeof v == "object" )
			{
				dfd.resolve(v);
			}
		})
		return dfd.promise();
	},

    /*
     * 重启路由器，屏显示进度条
     * @param {any} args 
     * args.title
     * args.restartTime
     * args.href
     */
    reboot: function( args )
    {
        var timeout;
        var arg = args || {};

        if ( arg.restartTime )
        {
            timeout = arg.restartTime;
        }
        else if ( window.custom && window.custom.restart_time > 0 )
        {
            timeout = window.custom.restart_time;
        }
        else
        {
            timeout = 60;
        }
        // 显示进度条
        page.progress({
            title: arg.title || $.i18n('Rebooting...'),
            sec: timeout,
            callback: function ()
            {
                page.alert( {message:arg.hint||$.i18n('Make sure that the device is reconnected')} ).then(function () {
                    if ( arg.href )
                    {
                        window.location.href = arg.href;
                    }
                    else
                    {
                        // 进度条读完之后, 刷新页面
                        window.location.reload(true);
                    }
                })
            }
        });
        // 执行cmds中的命令
        var cmds = arg.cmds || [];
        cmds.push('machine.restart[0,wui]');
        // 设置正在重启的标志位
        window.rebooting = true;
        he.cmd(cmds, null, function () {});
    },
 
    /*
     * 升级后重启路由器，屏显示进度条
     * @param {any} args 
     * args.title
     * args.restartTime
     * args.href
     */
    upgrade_reboot: function( args )
    {
        var timeout;
        var arg = args || {};

        if ( arg.restartTime )
        {
            timeout = arg.restartTime;
        }
        else if ( window.custom && window.custom.upgrade_wait > 0 )
        {
            timeout = window.custom.upgrade_wait;
        }
        else
        {
            timeout = 150;
        }
        // 显示进度条
        page.progress({
            title: arg.title || $.i18n('Restarting...'),
            sec: timeout,
            callback: function ()
            {
                page.alert( {message:arg.hint||$.i18n('Make sure that the device is reconnected')} ).then(function () {
                    if ( arg.href )
                    {
                        window.location.href = arg.href;
                    }
                    else
                    {
                        // 进度条读完之后, 刷新页面
                        window.location.reload(true);
                    }
                })
            }
        });
        // 执行cmds中的命令
        var cmds = arg.cmds || [];
        cmds.push('machine.restart[0,wui]');
        // 设置正在重启的标志位
        window.rebooting = true;
        he.cmd(cmds, null, function () {});
    }
  

}


