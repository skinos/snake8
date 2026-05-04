
window.object = "wui@admin";
window.hepath = "/he";
window.talkkey = sessionStorage.getItem( "talkkey" );
console.log( "GET:"+window.talkkey );
window.username = sessionStorage.getItem( "username" );
// disable load the subpage, loading when after i18n loaded
ace.demo.functions.enableDemoAjax= function () {};

// get IE type
var win = window;
var doc = win.document;
var input = doc.createElement("input");
function IEVersion() {
	var userAgent = navigator.userAgent; //取得浏览器的userAgent字符串
	var isIE = userAgent.indexOf("compatible") > -1 && userAgent.indexOf("MSIE") > -1; //判断是否IE<11浏览器
	var isEdge = userAgent.indexOf("Edge") > -1 && !isIE; //判断是否IE的Edge浏览器
	var isIE11 = userAgent.indexOf('Trident') > -1 && userAgent.indexOf("rv:11.0") > -1;
	if(isIE) {
		var reIE = new RegExp("MSIE (\\d+\\.\\d+);");
		reIE.test(userAgent);
		var fIEVersion = parseFloat(RegExp["$1"]);
		console.log('ieVersion：', fIEVersion);
		if(fIEVersion == 7) {
			return 7;
		} else if(fIEVersion == 8) {
			return 8;
		} else if(fIEVersion == 9) {
			return 9;
		} else if(fIEVersion == 10) {
			return 10;
		} else {
			return 6;//IE版本<=7
		}
	} else if(isEdge) {
		console.log('isEdge!');
		return 12;//edge
	} else if(isIE11) {
		console.log('isIE11!');
		return 11; //IE11
	}else{
		console.log('isOther!');
		return 13;//不是ie浏览器
	}
}
window.ie = IEVersion();			
if ( !window.location.origin )
{
	window.location.origin = window.location.protocol + "//" + window.location.hostname + (window.location.port ? ':' + window.location.port: '');
}

// logout
function logout_system()
{
	bootbox.confirm({
		message: $.i18n('Are you sure you want to logout?'),
		buttons:
		{
			confirm:
			{
				label: '<i class="fa fa-check"></i>'+$.i18n('Logout'), className: 'btn btn-main'
			},
			cancel:
			{
				label: '<i class="fa fa-times"></i>'+$.i18n('Cancel'), className: 'btn btn-second'
			}
		},
		callback: function (result)
		{
			if (!result)
			{
				return;
			}
			window.location.href = 'login.html';
		}
	});
}

// jqgrid setup
function jquery_setup()
{
	$.jgrid = $.jgrid || {};
	if(!$.jgrid.hasOwnProperty("regional"))
	{
		$.jgrid.regional = [];
	}
	$.jgrid.regional["all"] =
	{
		defaults :
		{
		  recordtext: $.i18n('recordtext'),
		  emptyrecords: $.i18n('No records to view'),
		  loadtext: "Loading...",
		  savetext: "Saving...",
		  pgtext : "{0} / {1}",
		  pgfirst : $.i18n('First Page'),
		  pglast : $.i18n('Last Page'),
		  pgnext : $.i18n('Next Page'),
		  pgprev : $.i18n('Previous Page'),
		  pgrecs : $.i18n('Records per Page'),
		  showhide: "Toggle Expand Collapse Grid",
		  // mobile
		  pagerCaption : "Grid::Page Settings",
		  pageText : "Page:",
		  recordPage : "Records per Page",
		  nomorerecs : "No more records...",
		  scrollPullup: "Pull up to load more...",
		  scrollPulldown : "Pull down to refresh...",
		  scrollRefresh : "Release to refresh..."
		},
		search :
		{
		  caption: "Search...",
		  Find: "Find",
		  Reset: "Reset",
		  odata: [{ oper:'eq', text:'equal'},{ oper:'ne', text:'not equal'},{ oper:'lt', text:'less'},{ oper:'le', text:'less or equal'},{ oper:'gt', text:'greater'},{ oper:'ge', text:'greater or equal'},{ oper:'bw', text:'begins with'},{ oper:'bn', text:'does not begin with'},{ oper:'in', text:'is in'},{ oper:'ni', text:'is not in'},{ oper:'ew', text:'ends with'},{ oper:'en', text:'does not end with'},{ oper:'cn', text:'contains'},{ oper:'nc', text:'does not contain'},{ oper:'nu', text:'is null'},{ oper:'nn', text:'is not null'}],
		  groupOps: [{ op: "AND", text: "all" },{ op: "OR",  text: "any" }],
		  operandTitle : "Click to select search operation.",
		  resetTitle : "Reset Search Value"
		},
		edit :
		{
		  addCaption: $.i18n('Add Record'),
		  editCaption: $.i18n('Edit Record'),
		  bSubmit: $.i18n('Submit'),
		  bCancel: $.i18n('Cancel'),
		  bClose: "Close",
		  saveData: "Data has been changed! Save changes?",
		  bYes : "Yes",
		  bNo : "No",
		  bExit : "Cancel",
		  msg:
		  {
			required: $.i18n('Field is required'),
			number:"Please, enter valid number",
			minValue: $.i18n('value must be greater than or equal to'),
			maxValue: $.i18n('value must be less than or equal to'),
			email: "is not a valid e-mail",
			integer: $.i18n('Please, enter valid integer value'),
			date: "Please, enter valid date value",
			url: "is not a valid URL. Prefix required ('http://' or 'https://')",
			nodefined : " is not defined!",
			novalue : " return value is required!",
			customarray : "Custom function should return array!",
			customfcheck : "Custom function should be present in case of custom checking!"
		  }
		},
		view :
		{
		  caption: $.i18n('View Record'),
		  bClose: "Close"
		},
		del :
		{
		  caption: $.i18n('Delete'),
		  msg: $.i18n('Delete selected record(s)?'),
		  bSubmit: $.i18n('Delete'),
		  bCancel: $.i18n('Cancel')
		},
		nav :
		{
		  edittext: "",
		  edittitle: $.i18n('Edit selected row'),
		  addtext:"",
		  addtitle: $.i18n('Add new row'),
		  deltext: "",
		  deltitle: $.i18n('Delete selected row'),
		  searchtext: "",
		  searchtitle: "Find records",
		  refreshtext: "",
		  refreshtitle: "Reload Grid",
		  alertcap: $.i18n('Warning'),
		  alerttext: $.i18n('Please, select row'),
		  viewtext: "",
		  viewtitle: $.i18n('View selected row'),
		  savetext: "",
		  savetitle: "Save row",
		  canceltext: "",
		  canceltitle : "Cancel row editing",
		  selectcaption : "Actions..."
		},
		col :
		{
		  caption: "Select columns",
		  bSubmit: "Ok",
		  bCancel: "Cancel"
		},
		errors :
		{
		  errcap : "Error",
		  nourl : "No url is set",
		  norecords: "No records to process",
		  model : "Length of colNames <> colModel!"
		},
		formatter :
		{
		  integer : {thousandsSeparator: ",", defaultValue: '0'},
		  number : {decimalSeparator:".", thousandsSeparator: ",", decimalPlaces: 2, defaultValue: '0.00'},
		  currency : {decimalSeparator:".", thousandsSeparator: ",", decimalPlaces: 2, prefix: "", suffix:"", defaultValue: '0.00'},
		  date :
		  {
			dayNames:
			[
			  "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat",
			  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
			],
			monthNames:
			[
			  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
			  "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
			],
			AmPm : ["am","pm","AM","PM"],
			S: function (j) {return j < 11 || j > 13 ? ['st', 'nd', 'rd', 'th'][Math.min((j - 1) % 10, 3)] : 'th';},
			srcformat: 'Y-m-d',
			newformat: 'n/j/Y',
			parseRe : /[#%\\\/:_;.,\t\s-]/,
			masks :
			{
			  // see http://php.net/manual/en/function.date.php for PHP format used in jqGrid
			  // and see http://docs.jquery.com/UI/Datepicker/formatDate
			  // and https://github.com/jquery/globalize#dates for alternative formats used frequently
			  // one can find on https://github.com/jquery/globalize/tree/master/lib/cultures many
			  // information about date, time, numbers and currency formats used in different countries
			  // one should just convert the information in PHP format
			  ISO8601Long:"Y-m-d H:i:s",
			  ISO8601Short:"Y-m-d",
			  // short date:
			  //	n - Numeric representation of a month, without leading zeros
			  //	j - Day of the month without leading zeros
			  //	Y - A full numeric representation of a year, 4 digits
			  // example: 3/1/2012 which means 1 March 2012
			  ShortDate: "n/j/Y", // in jQuery UI Datepicker: "M/d/yyyy"
			  // long date:
			  //	l - A full textual representation of the day of the week
			  //	F - A full textual representation of a month
			  //	d - Day of the month, 2 digits with leading zeros
			  //	Y - A full numeric representation of a year, 4 digits
			  LongDate: "l, F d, Y", // in jQuery UI Datepicker: "dddd, MMMM dd, yyyy"
			  // long date with long time:
			  //	l - A full textual representation of the day of the week
			  //	F - A full textual representation of a month
			  //	d - Day of the month, 2 digits with leading zeros
			  //	Y - A full numeric representation of a year, 4 digits
			  //	g - 12-hour format of an hour without leading zeros
			  //	i - Minutes with leading zeros
			  //	s - Seconds, with leading zeros
			  //	A - Uppercase Ante meridiem and Post meridiem (AM or PM)
			  FullDateTime: "l, F d, Y g:i:s A", // in jQuery UI Datepicker: "dddd, MMMM dd, yyyy h:mm:ss tt"
			  // month day:
			  //	F - A full textual representation of a month
			  //	d - Day of the month, 2 digits with leading zeros
			  MonthDay: "F d", // in jQuery UI Datepicker: "MMMM dd"
			  // short time (without seconds)
			  //	g - 12-hour format of an hour without leading zeros
			  //	i - Minutes with leading zeros
			  //	A - Uppercase Ante meridiem and Post meridiem (AM or PM)
			  ShortTime: "g:i A", // in jQuery UI Datepicker: "h:mm tt"
			  // long time (with seconds)
			  //	g - 12-hour format of an hour without leading zeros
			  //	i - Minutes with leading zeros
			  //	s - Seconds, with leading zeros
			  //	A - Uppercase Ante meridiem and Post meridiem (AM or PM)
			  LongTime: "g:i:s A", // in jQuery UI Datepicker: "h:mm:ss tt"
			  SortableDateTime: "Y-m-d\\TH:i:s",
			  UniversalSortableDateTime: "Y-m-d H:i:sO",
			  // month with year
			  //	Y - A full numeric representation of a year, 4 digits
			  //	F - A full textual representation of a month
			  YearMonth: "F, Y" // in jQuery UI Datepicker: "MMMM, yyyy"
			},
			reformatAfterEdit : false,
			userLocalTime : false
		  },
		  baseLinkUrl: '',
		  showAction: '',
		  target: '',
		  checkbox : {disabled:true},
		  idName : 'id'
		}
	};
	
	// datepicker setup
	var dates = $.fn.datepicker.dates =
	{
		en:
		{
			days: ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"],
			daysShort: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
			daysMin: ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"],
			months: ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"],
			monthsShort: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"],
			today: "Today",
			clear: "Clear"
		},
		// add chinese
		cn:
		{
			days: ["周日", "周一", "周二", "周三", "周四", "周五", "周六", "周日"],
			daysShort: ["日", "一", "二", "三", "四", "五", "六", "七"],
			daysMin: ["日", "一", "二", "三", "四", "五", "六", "七"],
			months: ["一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"],
			monthsShort: ["一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"],
			today: "今天",
			clear: "清除"
		}
	};
	var defaults = $.fn.datepicker.defaults = {
		autoclose: false,
		beforeShowDay: $.noop,
		calendarWeeks: false,
		clearBtn: false,
		daysOfWeekDisabled: [],
		endDate: Infinity,
		forceParse: true,
		format: 'mm/dd/yyyy',
		keyboardNavigation: true,
		language: window.lang,
		minViewMode: 0,
		orientation: "auto",
		rtl: false,
		startDate: -Infinity,
		startView: 0,
		todayBtn: false,
		todayHighlight: false,
		weekStart: 0
	};
}

// frame url location
function frame_url( hash )
{
	var name = '';
	var index = hash.indexOf('?');
	// get object name
	if ( index <= 0 )
	{
		name = hash;
	}
	else
	{
		name = hash.substring(0, index);
	}
	// custom the path
	if ( name === 'blank' )
	{
		return 'content/sdk.html';;
	}
	else if ( name === 'index' )
	{
		location.href = 'index.html'
		return;
	}
	/* for old page for tmp use */
	else if ( name === 'app' )
	{
		var paper = page.param( 'page', hash );
		return base64.decode(paper);
	}
	return 'content/' + name + '.html';
}



jQuery(function($) {

	// Configure load
	he.load( [ window.object,
			'land@machine',
			'land@machine.status',
			'arch@custom',
			'arch@gpio',
			'network@frame',
			'network@frame.mode_list',
			'network@frame.list',
			'network@frame.list[local]',
			'network@frame.list[extern]',
			'network@frame.list[outer]',
			'network@hosts',
			'land@fpk.wui_menu',
			'wui@ttyd.port',
			'.wifi@n',
			'.wifi@a',
			'.modem@lte.status',
			'.modem@lte2.status',
			'.modem@lte3.status',
			'.modem@lte4.status',
			'.ifname@lan.status',
			'.ifname@lan2.status',
			'.ifname@lan3.status',
			'.ifname@lan4.status',
			'.ifname@wan.status',
			'.ifname@wan2.status',
			'.ifname@wan3.status',
			'.ifname@wan4.status',
			'.ifname@lte.status',
			'.ifname@lte2.status',
			'.ifname@lte3.status',
			'.ifname@lte4.status',
			'.ifname@wisp.status',
			'.ifname@wisp2.status'
	] ).then( function(v){
		window.wui = v[0];
		window.machine = v[1];
		window.machines = v[2];
		window.custom = v[3];
		window.gpio = v[4];
		window.network_frame = v[5];
		window.mode_list = v[6];
		window.ifname_list = v[7];
		window.local_list = v[8];
		window.extern_list = v[9];
		window.outer_list = v[10];
		window.hosts = v[11];
		window.fpk_menu = v[12];
		window.ttyd_port = v[13];
		window.ifdev = {};
		window.ifdev["wifi@n"] = v[14];
		window.ifdev["wifi@a"] = v[15];
		window.ifdev["modem@lte"] = v[16];
		window.ifdev["modem@lte2"] = v[17];
		window.ifdev["modem@lte3"] = v[18];
		window.ifdev["modem@lte4"] = v[19];
		window.ifname = {};
		window.ifname["ifname@lan"] = v[20];
		window.ifname["ifname@lan2"] = v[21];
		window.ifname["ifname@lan3"] = v[22];
		window.ifname["ifname@lan4"] = v[23];
		window.ifname["ifname@wan"] = v[24];
		window.ifname["ifname@wan2"] = v[25];
		window.ifname["ifname@wan3"] = v[26];
		window.ifname["ifname@wan4"] = v[27];
		window.ifname["ifname@lte"] = v[28];
		window.ifname["ifname@lte2"] = v[29];
		window.ifname["ifname@lte3"] = v[30];
		window.ifname["ifname@lte4"] = v[31];
		window.ifname["ifname@wisp"] = v[32];
		window.ifname["ifname@wisp2"] = v[33];
		window.lang = window.machine.language;
		document.title = window.machine.name;

		/* get the machine config and setup the language */
		$.i18n().load( page.lang('index') ).then( function () { 
			$.i18n().locale = window.lang; $('body').i18n();
			var logoshow = false;
			var wuimenu = wui.menu;
			if ( window.wui )
			{
				if (  window.wui.logo_file )
				{
					logoshow = true;
					var stamp = '<img src="' + window.wui.logo_file + '" width="' + (window.wui.logo_width || '180px') + '" height="' + (window.wui.logo_height || '100px') + '" /></div>';
					$('#logo_img').append(stamp);
				}
				if ( window.wui.logo_title )
				{
					logoshow = true;
					if ( window.wui.logo_title == "#NAME" )
					{
						$('#logo_context').append( document.title );
					}
					else if ( window.wui.logo_title == "#MODEL" )
					{
						if ( window.machines.cmodel )
						{
							$('#logo_context').text( window.machines.cmodel );
						}
						else
						{
							$('#logo_context').text( window.machines.model );
						}
					}
					else
					{
						$('#logo_context').append( window.wui.logo_title );
					}
				}
				if ( window.wui.logo_model == "disable" )
				{
					$('#logo_table').hide();
					$('#logo_logout').show();
				}
				else
				{
					if ( window.machines.cmodel )
					{
						$('#cmodel').text( window.machines.cmodel );
					}
					else
					{
						$('#cmodel').text( window.machines.model );
					}
					$('#version').text( window.machines.version );
					$('#logo_table').show();
				}
				
				if ( window.wui.nav_bar == "disable" )
				{
					nbarshow = false;
				}
				else
				{
					nbarshow = true;
				}
			}
			if ( logoshow == true )
			{
				$('#logo').show();
				$("#copyright").hide();
				$('#sidebar-shortcuts').hide();
			}
			else
			{
				$('#sidebar-shortcuts').show();
				if ( window.wui.copyright && window.wui.copyright == "disable" )
				{
					$("#copyright").hide();
				}
				else
				{
					$("#copyright").show();
				}
			}
			if ( nbarshow == true )
			{
				$('#navbar').show();
				$('#prompt').html( document.title );
			}
			else
			{
				$('#navbar').hide();
			}

			/* setup the menus */
			var menus = [];
			var router = true;
			var homepage = "dashboard";
			if ( window.machine.mode == "default" || window.machine.mode == "parasite" )
			{
				router = false;
				$('#extern_nav').attr('href','#');
				$('#extern_nav_mini').attr('href','#');
			}
			else if ( window.machine.mode == "gateway" || window.machine.mode == "dgateway" )
			{
				if ( !wuimenu || wuimenu.wan != "disable" )
				{
					$('#extern_nav').attr('href','#wan?object=ifname@wan');
					$('#extern_nav_mini').attr('href','#wan?object=ifname@wan');
				}
			}
			else if ( window.machine.mode == "wisp" || window.machine.mode == "dwisp" )
			{
				if ( !wuimenu || wuimenu.wisp != "disable" )
				{
					$('#extern_nav').attr('href','#wisp?object=ifname@wisp');
					$('#extern_nav_mini').attr('href','#wisp?object=ifname@wisp');
				}
			}
			else if ( window.machine.mode == "nwisp" )
			{
				if ( !wuimenu || wuimenu.wisp2 != "disable" )
				{
					$('#extern_nav').attr('href','#wisp?object=ifname@wisp2');
					$('#extern_nav_mini').attr('href','#wisp?object=ifname@wisp2');
				}
			}
			else if ( window.machine.mode == "nmisp" )
			{
				if ( !wuimenu || wuimenu.lte2 != "disable" )
				{
					$('#extern_nav').attr('href','#lte?object=ifname@lte2' );
					$('#extern_nav_mini').attr('href','#lte?object=ifname@lte2' );
				}
			}
			else
			{
				if ( !wuimenu || wuimenu.lte != "disable" )
				{
					
					$('#extern_nav').attr('href','#lte?object=ifname@lte');
					$('#extern_nav_mini').attr('href','#lte?object=ifname@lte' );
				}
			}

			if ( !wuimenu || wuimenu.dashboard != "disable" )
			{
				menu.add( true, menus, $.i18n( 'Dashboard' ), 'dashboard', 'menu-icon fa fa-bookmark'  );
			}
			else if ( homepage == "dashboard" )
			{
				homepage = "";
			}
			if ( !wuimenu || wuimenu.utilization != "disable" )
			{
				menu.add( true, menus, $.i18n( 'Utilization' ), 'utilization', 'menu-icon fa fa-area-chart'  );
			}
			else if ( homepage == "utilization" )
			{
				homepage = "";
			}
			if ( !wuimenu || wuimenu.interface != "disable" )
			{
				menu.add( true, menus, $.i18n( 'Traffic' ), 'traffic', 'menu-icon fa fa-list' );
			}
			else if ( homepage == "traffic" )
			{
				homepage = "";
			}

			menu.add( false, menus, $.i18n( 'Network' ), 'network', 'menu-icon fa fa-sitemap' );
			if ( router == true )
			{
				if ( network_frame )
				{
					if ( !wuimenu || wuimenu.connection != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'Connection' ), window.machines.mode );
					}
				}
				if ( window.ifname["ifname@lte"]  )
				{
					if ( !wuimenu || wuimenu.lte != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LTE/NR' ), 'lte?object=ifname@lte' );
					}
				}
				if ( window.ifname["ifname@lte2"] )
				{
					if ( !wuimenu || wuimenu.lte2 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LTE/NR2' ), 'lte?object=ifname@lte2' );
					}
				}
				if ( window.ifname["ifname@lte3"] )
				{
					if ( !wuimenu || wuimenu.lte3 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LTE/NR3' ), 'lte?object=ifname@lte3' );
					}
				}
				if ( window.ifname["ifname@lte4"] )
				{
					if ( !wuimenu || wuimenu.lte4 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LTE/NR4' ), 'lte?object=ifname@lte4' );
					}
				}
				if ( window.ifname["ifname@wan"] )
				{
					if ( !wuimenu || wuimenu.wan != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WAN' ), 'wan?object=ifname@wan' );
					}
				}
				if ( window.ifname["ifname@wan2"] )
				{
					if ( !wuimenu || wuimenu.wan2 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WAN2' ), 'wan?object=ifname@wan2' );
					}
				}
				if ( window.ifname["ifname@wan3"] )
				{
					if ( !wuimenu || wuimenu.wan3 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WAN3' ), 'wan?object=ifname@wan3' );
					}
				}
				if ( window.ifname["ifname@wan4"] )
				{
					if ( !wuimenu || wuimenu.wan4 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WAN4' ), 'wan?object=ifname@wan4' );
					}
				}
				if ( window.ifdev["wifi@n"] == true && window.ifname["ifname@wisp"] )
				{
					if ( !wuimenu || wuimenu.wisp != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WISP(2.4G)' ), 'wisp?object=ifname@wisp' );
					}
				}
				if ( window.ifdev["wifi@a"] == true && window.ifname["ifname@wisp2"] )
				{
					if ( !wuimenu || wuimenu.wisp2 != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'WISP(5.8G)' ), 'wisp?object=ifname@wisp2' );
					}
				}
				if ( window.ifname["ifname@lan"] )
				{
					if ( !wuimenu || wuimenu.lwan != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LAN' ), 'lan?object=ifname@lan' );
					}
				}				 
				menu.add( false, menus, $.i18n( 'Route' ), 'route', 'menu-icon glyphicon glyphicon-move' );
				if ( window.hosts )
				{
					menu.addlink( menus, $.i18n( 'Route' ), $.i18n( 'Hosts' ), 'hosts' );
				}
			}
			else
			{
				if ( window.hosts )
				{
					menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'Hosts' ), 'hosts' );
				}
				if ( window.ifname["ifname@lan"] )
				{
					if ( !wuimenu || wuimenu.lwan != "disable" )
					{
						menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LAN' ), 'lwan?object=ifname@lan' );
					}
				}
			}

			if ( window.ifname["ifname@lan2"] )
			{
				if ( !wuimenu || wuimenu.lan2 != "disable" )
				{
					menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LAN2' ), 'lan?object=ifname@lan2' );
				}
			}
			if ( window.ifname["ifname@lan3"] )
			{
				if ( !wuimenu || wuimenu.lan3 != "disable" )
				{
					menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LAN3' ), 'lan?object=ifname@lan3' );
				}
			}
			if ( window.ifname["ifname@lan4"] )
			{
				if ( !wuimenu || wuimenu.lan4 != "disable" )
				{
					menu.addlink( menus, $.i18n( 'Network' ), $.i18n( 'LAN4' ), 'lan?object=ifname@lan4' );
				}
			}

			menu.add( false, menus, $.i18n( 'VPN' ), 'vpn', 'menu-icon fa fa-lock' );

			menu.add( false, menus, $.i18n( 'Wireless' ), 'ssid', 'menu-icon fa fa-wifi' );

			menu.add( false, menus, $.i18n( 'Station' ), 'station', 'menu-icon fa fa-mobile' );

			menu.add( false, menus, $.i18n( 'Application' ), '', 'menu-icon fa fa-building' );

			menu.add( false, menus, $.i18n( 'Service' ), '', 'menu-icon fa fa-building' );

			menu.add( false, menus, $.i18n( 'Sensor' ), '', 'menu-icon fa fa-exchange' );

			menu.add( false, menus, $.i18n( 'Indicator' ), '', 'menu-icon fa fa-eye' );

			menu.add( false, menus, $.i18n( 'Cloud' ), '', 'menu-icon fa fa-eye' );

			menu.add( false, menus, $.i18n( 'System' ), 'device', 'menu-icon fa fa-cogs' );
			menu.addlink( menus, $.i18n( 'System' ), $.i18n( 'Device' ), 'device' );
			if ( !wuimenu || wuimenu.configure != "disable" )
			{
				menu.addlink( menus, $.i18n( 'System' ), $.i18n( 'Configure' ), 'configure' );
			}
			if ( !wuimenu || wuimenu.software != "disable" )
			{
				menu.addlink( menus, $.i18n( 'System' ), $.i18n( 'Software' ), 'software' );
			}
			menu.addlink( menus, $.i18n( 'System' ), $.i18n( 'Password' ), 'password' );

			menu.add( false, menus, $.i18n( 'Debug' ), 'debug', 'menu-icon fa fa-bug' );
			menu.addlink( menus, $.i18n( 'Debug' ), $.i18n( 'Syslog' ), 'syslog' );
			if ( window.ttyd_port && ( !wuimenu || wuimenu.terminal != "disable" ) )
			{
				menu.addlink( menus, $.i18n( 'Debug' ), $.i18n( 'Terminal' ), 'terminal' );
			}
			if ( window.machines.scope == "std" )
			{
				if ( !wuimenu || wuimenu.development != "disable" )
				{
					menu.add( false, menus, $.i18n( 'Development' ), 'sdk', 'menu-icon fa fa-gavel' );
					menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'SDK' ), 'sdk' );
					menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Inittab' ), 'inittab' );
					menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Uninittab' ), 'uninittab' );
					menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Jointtab' ), 'jointtab' );
					menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Daemon' ), 'daemon' );
					//menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Crontab' ), 'crontab' );
				}
			}
			else
			{
				if ( wuimenu && wuimenu.development == "enable" )
					{
						menu.add( false, menus, $.i18n( 'Development' ), 'sdk', 'menu-icon fa fa-gavel' );
						menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'SDK' ), 'sdk' );
						menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Inittab' ), 'inittab' );
						menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Uninittab' ), 'uninittab' );
						menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Jointtab' ), 'jointtab' );
						menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Daemon' ), 'daemon' );
						//menu.addlink( menus, $.i18n( 'Development' ), $.i18n( 'Crontab' ), 'crontab' );
					}
			}

			// Load the FPK html
			var almark = false;
			for( var index in window.fpk_menu )
			{
				var app = window.fpk_menu[index];
				var app_menu = app['menu'];
				var app_mode = app['mode'];
				var app_page = app['page'];
				var app_lang = app['lang'][window.lang];
				var app_hash = 'app?page='+base64.encode(app_page);
				var app_title = app[window.lang];
				var app_object = app['object'];
				if ( app_mode && app_mode[window.machine.mode] != "enable" )
				{
					continue;
				}
				if ( window.wui.menu && window.wui.menu[index] == "disable" )
				{
					continue;
				}
				if ( !app_page )
				{
					continue;
				}
				if ( app_object )
				{
					app_hash += '&object='+app_object;
				}
				if ( app_lang )
				{
					app_hash += '&lang='+base64.encode(app_lang);
				}
				if ( !app_title )
				{
					app_title = app["en"];
				}
				if ( !app_menu )
				{
					app_menu = 'Application';
				}
				if ( app_menu == "Quick" || app_menu == "Wizard" || app_menu == "Home" )
				{
					menu.insert( true, menus, $.i18n( app_menu ), app_hash, 'menu-icon fa fa-bolt' );
					menu.mark( menus, $.i18n( app_menu ) );
					homepage = app_hash;
					almark = true;
				}
				else
				{
					menu.addlink( menus, $.i18n( app_menu ), app_title, app_hash );
				}
			}
			if ( homepage == "" )
			{
				homepage = "sdk";
				menu.mark( menus, $.i18n( 'SDK' ) );
			}
			menu.display( "#nav-list", menus );
			// ace Ajax setup
			$('.page-content-area').ace_ajax({
				//default url
				default_url: homepage,
				//close the right menu when moblie
				close_active: true,
				close_mobile_menu: '#sidebar',
				close_dropdowns: true,
				// subpage path
				content_url: frame_url
			});

			// Jquery setup
			jquery_setup();
			// 监听子页面加载完成
			$('.page-content-area').on('ajaxloadcomplete', function (e, params) {
				// 翻译子网页
				$('#main-container').i18n();
			});

		}) // $.i18n.load()

	}); // he.load()

	// Logout event
	$('#logout').on( ace.click_event, logout_system );

	// Logout2 event
	$('#logout2').on( ace.click_event, logout_system );

});
