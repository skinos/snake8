

window.object = "wui@admin";
window.hepath = "/public";
window.talkkey = "";
window.username = "";
sessionStorage.setItem( "talkkey", window.talkkey );
sessionStorage.setItem( "username", window.username );
console.log( "CLEAR: talkkey" );

function machine_status_has_ill(status)
{
    if ( !status ) return false;
    if ( !Object.prototype.hasOwnProperty.call(status, "ill") ) return false;
    if ( String(status.ill).trim() === "" ) return false;
    return true;
}

jQuery(function($) {

	// load
	he.load( [ window.object, 'arch@custom', 'land@machine', 'land@machine.status' ] ).then( function(v){
		window.wui = v[0];
		window.custom = v[1];
		window.machine = v[2];
		window.machines = v[3];

		// window.machines = {"ill":"10"};
		// 判断是否存在ill异常值
		if ( machine_status_has_ill(window.machines) )
		{
			console.warn("ill detected, jump directly to index.html#factory");

			sessionStorage.setItem("ill_direct_entry", "true");
			window.location.replace("index.html#factory");
			return;
		}

		window.lang = window.machine.language;
		document.title = window.machine.name;
		// prompt
		$('#prompt').html( document.title );
		// logo
		if ( window.wui && window.wui.logo_file )
		{
		    $('body').attr('class', 'login-layout light-login');
		    var stamp = '<img src="' + window.wui.logo_file + '" width="' + (window.wui.logo_width || '180px') + '" height="' + (window.wui.logo_height || '100px') + '" /></div>';
		    if ( window.wui.logo_align )
			{
		        $('#logo').removeClass("center").addClass( window.wui.logo_align );
			}
		    $('#logo').html(stamp);
		}
		else
		{
			// copyright
		    if ( window.wui.copyright && window.wui.copyright == "disable" )
		    {
		        $("#copyright").hide();
		    }
		    else
		    {
		        $("#copyright").show();
		    }
		}
		// get the machine config and setup the language
		page.password('password', 'password-icon' );
		$.i18n().load( page.lang('index') ).then( function () { 
			$.i18n().locale = lang; $('body').i18n();
			// username 
			$('#username').val( "admin" );
			/* focus on the input */
			$('#password').focus();
			// login bind
			$('#login').unbind(ace.click_event).on(ace.click_event, login_system );
			$(document).keyup( function(event) {
				if( event.keyCode ==13 )
				{
					login_system();
				}
			});
		})
		
	});

	// login
	function login_system(  )
	{
		var paramter = {};
		paramter["username"] = $("#username").val();
		paramter["password"] = base64.encode( md5( $("#password").val()+":"+$("#username").val()+":"+window.machines.rand ) );
	    $.ajax({
	        'url':"/auth",
			'type':'POST',
			'timeout':0,
			'async':true,
			'contentType':'application/x-www-form-urlencoded',
			'data':JSON.stringify(paramter),
	        'complete': function ( x, s )
	        {
				console.log( "Server Async Response String: "+x.responseText );
	            // string for Exception
	            if ( x.responseText.indexOf("{" ) < 0 )
	            {
					page.alert( { message:$.i18n('Data Error') } ).then( function(){ window.location.href = 'login.html'; } );
	            }
	            else
	            {
	                var value = eval( "(" + x.responseText + ")" );
	                if ( value == null )
	                {
	                    console.log( "Server Response Not JSON: "+x.responseText );
	                    return;
	                }
					if ( value["return"] == "true" )
					{
						sessionStorage.setItem( "talkkey", value["key"] );
						sessionStorage.setItem( "username", value["username"] );
						window.location.href = 'index.html';
						return;
					}
					else
					{
						page.alert( { message:$.i18n('Incorrect password') } ).then( function()
						{ window.location.href = 'login.html'; } );
						// 弹窗出来后，监听回车键
						setTimeout(function() {
							$(document).off('keydown.incorrectPassword').on('keydown.incorrectPassword', function(e) {
								if (e.key === 'Enter' || e.keyCode === 13) {
									e.preventDefault();

									$(document).off('keydown.incorrectPassword');
									window.location.href = 'login.html';
								}
							});
						}, 200);
					}
	            }
	        }
	    });
	}

	$(document).on('click', '.toolbar a[data-target]', function(e) {
		e.preventDefault();
		var target = $(this).data('target');
		$('.widget-box.visible').removeClass('visible');//hide others
		$(target).addClass('visible');//show target
	});

});

