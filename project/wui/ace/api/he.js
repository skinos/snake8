


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
    /* JS string must be encodable as well-formed UTF-8 (reject lone surrogates) */
    _string_utf8_ok: function( s )
    {
        var i;
        var c;
        var d;
        var enc;
        var dec;

        if ( s == null )
        {
            return true;
        }
        if ( typeof s != "string" )
        {
            return true;
        }
        if ( typeof TextEncoder != "undefined" && typeof TextDecoder != "undefined" )
        {
            try
            {
                enc = new TextEncoder().encode( s );
                dec = new TextDecoder( "utf-8", { fatal: true } ).decode( enc );
                if ( dec !== s )
                {
                    return false;
                }
                return true;
            }
            catch ( e )
            {
                return false;
            }
        }
        for ( i = 0; i < s.length; i++ )
        {
            c = s.charCodeAt( i );
            if ( c >= 0xD800 && c <= 0xDBFF )
            {
                if ( i + 1 >= s.length )
                {
                    return false;
                }
                d = s.charCodeAt( i + 1 );
                if ( d < 0xDC00 || d > 0xDFFF )
                {
                    return false;
                }
                i++;
            }
            else if ( c >= 0xDC00 && c <= 0xDFFF )
            {
                return false;
            }
        }
        return true;
    },

    /* Check all HE command strings in the POST parameter object */
    _payload_utf8_ok: function( paramter )
    {
        var k;
        var v;

        if ( !paramter )
        {
            return true;
        }
        for ( k in paramter )
        {
            v = paramter[k];
            if ( typeof v != "string" )
            {
                continue;
            }
            if ( he._string_utf8_ok( v ) == false )
            {
                return false;
            }
        }
        return true;
    },

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

        if ( he._payload_utf8_ok( paramter ) == false )
        {
            if ( args && args.loading )
            {
                page.overlay2hide();
            }
            page.alert( { message: $.i18n( 'Invalid character encoding' ) } );
            if ( func != null )
            {
                return;
            }
            return null;
        }

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
     * After reboot/upgrade: wait until the device web is back, then reload.
     * restart_time / upgrade wait is the max wait (progress estimate).
     * @param {object} arg - title/hint/href from reboot callers
     * @param {number} timeoutSec - max wait seconds
     * @param {string} titleDefault - progress title when arg.title missing
     */
    _waitDeviceReload: function ( arg, timeoutSec, titleDefault )
    {
        var pollMs = 2000;       /* probe interval */
        var probeTimeout = 1500;
        var needOk = 2;          /* consecutive successes after offline */
        /* Keep enough time after grace for offline + two live probes when T is short */
        var graceMs = Math.min(10000, Math.max(3000, (timeoutSec - 20) * 1000));
        var start = Date.now();
        var offlineSeen = false;
        var okStreak = 0;
        var finished = false;
        var probeTimer = null;
        var maxTimer = null;
        var bar;

        function goReload()
        {
            window.rebooting = false;
            /* Prefer replace so history does not return to the pre-reboot page */
            if ( arg.href )
            {
                window.location.replace(arg.href);
            }
            else
            {
                /* Session is gone after reboot; go login directly to avoid flashing the old page */
                window.location.replace('login.html');
            }
        }

        function stopWait()
        {
            if (probeTimer)
            {
                clearInterval(probeTimer);
                probeTimer = null;
            }
            if (maxTimer)
            {
                clearTimeout(maxTimer);
                maxTimer = null;
            }
        }

        function finishEarly()
        {
            if (finished)
            {
                return;
            }
            finished = true;
            stopWait();
            if (bar)
            {
                /* Keep overlay until navigation completes — hide would flash the old page */
                bar.finish({ skipCallback: true, keepVisible: true });
            }
            $('#overlay-progress-title').text($.i18n('Reboot successfully'));
            page.hint2succeed($.i18n('Reboot successfully'));
            setTimeout(goReload, 1200);
        }

        function finishTimeout()
        {
            if (finished)
            {
                return;
            }
            finished = true;
            stopWait();
            if (bar)
            {
                /* Must hide overlay or bootbox alert underneath cannot be clicked */
                bar.finish({ skipCallback: true });
            }
            page.alert({
                message: arg.hint || $.i18n('Make sure that the device is reconnected')
            }).then(function () {
                goReload();
            });
        }

        function probeOnce()
        {
            if (finished)
            {
                return;
            }
            $.ajax({
                url: '/login.html?_=' + Date.now(),
                type: 'GET',
                timeout: probeTimeout,
                cache: false,
                complete: function (x, s)
                {
                    var elapsed;
                    var alive;

                    if (finished)
                    {
                        return;
                    }
                    elapsed = Date.now() - start;
                    alive = (s === 'success' && x.status >= 200 && x.status < 400);
                    if (elapsed < graceMs)
                    {
                        /* Grace: only remember offline, never treat as ready */
                        if (!alive)
                        {
                            offlineSeen = true;
                            okStreak = 0;
                        }
                        return;
                    }
                    if (!alive)
                    {
                        offlineSeen = true;
                        okStreak = 0;
                        return;
                    }
                    /* Alive after grace: require prior offline to avoid old process */
                    if (!offlineSeen)
                    {
                        return;
                    }
                    okStreak++;
                    if (okStreak >= needOk)
                    {
                        finishEarly();
                    }
                }
            });
        }

        bar = page.progress({
            title: arg.title || titleDefault,
            sec: timeoutSec,
            holdAt: 95
        });
        probeTimer = setInterval(probeOnce, pollMs);
        probeOnce();
        maxTimer = setTimeout(finishTimeout, timeoutSec * 1000);
    },

    /*
     * Reboot the device and show a progress bar until web is reachable again.
     * @param {any} args
     * args.title
     * args.restartTime - max wait seconds (also progress estimate)
     * args.href
     * args.hint
     * args.cmds - extra HE commands before machine.restart
     */
    reboot: function( args )
    {
        var timeout;
        var arg = args || {};
        var cmds;

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

        window.rebooting = true;
        he._waitDeviceReload(arg, timeout, $.i18n('Rebooting...'));

        cmds = arg.cmds || [];
        cmds.push('machine.restart[0,wui]');
        he.cmd(cmds, null, function () {});
    },
 
    /*
     * Progress UX after firmware upgrade (optional restart).
     * @param {any} args
     * args.title
     * args.restartTime - max wait seconds
     * args.href
     * args.hint
     * args.norestart - only wait/reload UX, do not send restart
     * args.cmds
     */
    upgrade_reboot: function( args )
    {
        var timeout;
        var arg = args || {};
        var cmds;

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

        window.rebooting = true;
        he._waitDeviceReload(arg, timeout, $.i18n('Restarting...'));

        if ( !arg.norestart )
        {
            cmds = arg.cmds || [];
            cmds.push('machine.restart[0,wui]');
            he.cmd(cmds, null, function () {});
        }
    }
  

}

