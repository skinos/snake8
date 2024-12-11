window.hepath = "/public";
window.talkkey = "";
window.username = "";
localStorage.setItem( "talkkey", window.talkkey );
localStorage.setItem( "username", window.username );

jQuery(function($) {

	// load
	he.load( [ 'wui@admin', 'arch@custom', 'land@machine', 'land@machine.status' ] ).then( function(v){
		window.wui = v[0];
		window.custom = v[1];
		window.machine = v[2];
		window.machines = v[3];
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
						localStorage.setItem( "lang", window.lang );
						localStorage.setItem( "talkkey", value["key"] );
						localStorage.setItem( "username", value["username"] );
						window.location.href = 'index.html';
						return;
					}
					else
					{
						page.alert( { message:$.i18n('Incorrect password') } ).then( function(){ window.location.href = 'login.html'; } );
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

