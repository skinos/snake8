window.LteConfigManager = {
    cache: {},
    // object is modem@lte / modem@lte2 — no separate ifdev
    loadStatus: function(targetObject, forceRefresh) {
        var self = this;
        if (!forceRefresh && self.cache[targetObject]) {
            return Promise.resolve(self.cache[targetObject]);
        }
        return he.load([
            targetObject,
            targetObject + ".operator",
            targetObject + ".status",
            targetObject + ".smslist",
        ]).then(function(v) {
          v = v || [];
          // [config, modemName, operator, status] — align with lte_basic layout
          var out = [];
          out[0] = v[0];
          out[1] = targetObject;
          out[2] = v[1];
          out[3] = v[2];
          out[4] = v[3];
          for (var i = 0; i <= 4; i++) {
              if (typeof out[i] === 'undefined' || out[i] === null) {
                  out[i] = (i >= 3) ? "" : {};
              }
          }
          self.cache[targetObject] = out;
          return out;
        });
    },

    // sub-tabs (ltemodem/ltesms/lteat) still call this with modem + object
    loadSettings: function(modem, ifname, forceRefresh) {
        var self = this;
        var cacheKey = modem + "_" + ifname;
        if (!forceRefresh && self.cache[cacheKey]) {
            return Promise.resolve(self.cache[cacheKey]);
        }
        return he.load([
            modem,
            ifname,
            ifname + ".status",
            modem + ".custom_set",
            modem + ".custom_watch",
            ifname + ".lock_imei",
            ifname + ".lock_imsi",
            modem + ".sms_list"
        ]).then(function(v) {
            v = v || [];
            for (var i = 0; i <= 6; i++) {
                if (typeof v[i] === 'undefined' || v[i] === null) {
                    v[i] = (i >= 5) ? "" : {};
                }
            }
            self.cache[cacheKey] = v;
            return v;
        });
    }
};

var state;
var config;
var modem;
var operator;
var object = "modem@lte";

function config_load() {
    window.LteConfigManager.loadStatus(object, true).then(function(v) {
        if (!v) {
            return;
        }
        lte_basic(v);
    });
};

/* load the status from modem@lte.status */
function status_load()
{
  he.bkload( [ object+".status" ] ).then( function(v){
    state = v[0];
    var info = state;
    var id = "#lte";
    var operatorText;
    var has4G;
    var has5G;
    var hasRssi4;
    var hasRssi5;
    var hasRsrq4;
    var hasRsrq5;

    if(!info){
      info = {};
    }

    $(id+"_btn").show();
    $(id+"_rssiimg").show();
    $(id+"_rssiimg5").show();

      /* status end btn */
      if ( info.status )
      {
          $(id+"_status").text( $.i18n(info.status) );
          if ( info.status == "up" || info.status == "uping" || info.status == "connect" ||
               info.status == "connecting" || info.status == "block" || info.status == "failed" ||
               info.status == "register" || info.status == "setup" )
          {
              $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
          }
          else
          {
              $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          }
      }
      else
      {
          $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
          $(id+"_status").text( $.i18n("down") );
      }

      /* operator */
      operatorText = "";
      if ( info.operator )
      {
          operatorText = $.i18n(info.operator);
      }
      else if ( info.plmn )
      {
          operatorText = $.i18n(info.plmn);
      }

      $(id+"_nettype").text( info.nettype || "" );
      $(id+"_nettype5").text( info.nettype2 || "" );
      $(id+"_csq").text( info.csq || "" );

      if ( info.signal )
      {
          $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );
      }
      else
      {
          $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_0.png" );
      }
      if ( info.signal2 )
      {
          $(id+"_rssiimg5").attr( "src", "/assets/css/images/signal_"+info.signal2+".png" );
      }
      else
      {
          $(id+"_rssiimg5").attr( "src", "/assets/css/images/signal_0.png" );
      }

      /* RSSI / RSRP */
      if ( info.rssi )
      {
          $(id+"_rssi").text( info.rssi+"dBm" );
      }
      else
      {
          $(id+"_rssi").text( "" );
      }
      if ( info.rsrp )
      {
          $(id+"_rsrp").text( "RSRP:"+info.rsrp+"dBm" );
      }
      else
      {
          $(id+"_rsrp").text( "" );
      }
      if ( info.rsrp2 )
      {
          $(id+"_rsrp5").text( info.rsrp2+"dBm" );
      }
      else
      {
          $(id+"_rsrp5").text( "" );
      }

      /* RSRQ / SINR */
      if ( info.rsrq )
      {
          $(id+"_rsrq").text( "RSRQ:"+info.rsrq+"dB" );
      }
      else
      {
          $(id+"_rsrq").text( "" );
      }
      if ( info.sinr )
      {
          $(id+"_sinr").text( "SINR:"+info.sinr+"dB" );
      }
      else
      {
          $(id+"_sinr").text( "" );
      }
      if ( info.rsrq2 )
      {
          $(id+"_rsrq5").text( "RSRQ:"+info.rsrq2+"dB" );
      }
      else
      {
          $(id+"_rsrq5").text( "" );
      }
      if ( info.sinr2 )
      {
          $(id+"_sinr5").text( "SINR:"+info.sinr2+"dB" );
      }
      else
      {
          $(id+"_sinr5").text( "" );
      }

      /* band / channel / name */
      $(id+"_band").text( info.band || "" );
      $(id+"_band5").text( info.band2 || "" );
      $(id+"_channel").text( info.channel || "" );
      $(id+"_channel2").text( info.channel2 || "" );
      $(id+"_name").text( info.name || "" );

      /* device info */
      if ( info.imei == "noimei" )
      {
          $(id+"_imei").text( $.i18n("noimei") );
      }
      else
      {
        $(id+"_imei").text( info.imei||' ' );
      }
      if ( info.imsi == "noimsi" )
      {
          $(id+"_imsi").text( $.i18n("noimsi") );
      }
      else
      {
          $(id+"_imsi").text( info.imsi || "" );
      }
      if ( info.iccid )
      {
          $(id+"_iccid").text( $.i18n(info.iccid) );
      }
      else
      {
          $(id+"_iccid").text( "" );
      }

      /* LAC / CI */
      $(id+"_lac").text( info.lac || "" );
      $(id+"_ci").text( info.ci || "" );
      if ( info.lac && info.ci )
      {
          $(id+"_separator").show();
      }
      else
      {
          $(id+"_separator").hide();
      }

      /* show / hide heads like dashboard */
      has4G = info.signal || info.nettype || info.csq;
      has5G = info.signal2 || info.nettype2;
      if ( has4G )
      {
          $(id+"_4g_head").show();
          $(id+"_operator").text( operatorText );
      }
      else
      {
          $(id+"_4g_head").hide();
          $(id+"_operator").text( "" );
      }
      if ( has5G )
      {
          $(id+"_5g_head").show();
          if ( !has4G && operatorText )
          {
              $(id+"_operator5").text( operatorText );
          }
          else
          {
              $(id+"_operator5").text( "" );
          }
      }
      else
      {
          $(id+"_5g_head").hide();
          $(id+"_operator5").text( "" );
      }
      if ( !has4G && !has5G && operatorText )
      {
          $(id+"_4g_head").show();
          $(id+"_operator").text( operatorText );
          $(id+"_nettype").text( "" );
          $(id+"_csq").text( "" );
      }
      if ( !info.nettype2 && !info.signal && !info.signal2 )
      {
          $(id+"_4g_head").show();
      }

      hasRssi4 = info.rssi || info.rsrp;
      if ( hasRssi4 )
      {
          $(id+"_rssi4_head").show();
      }
      else
      {
          $(id+"_rssi4_head").hide();
      }
      hasRssi5 = info.rsrp2;
      if ( hasRssi5 )
      {
          $(id+"_rssi5_head").show();
      }
      else
      {
          $(id+"_rssi5_head").hide();
      }
      if ( !info.rssi && !info.rsrp && !info.rsrp2 )
      {
          $(id+"_rssi4_head").show();
          $(id+"_rssi").text( $.i18n("nosignal") );
      }

      hasRsrq4 = info.rsrq || info.sinr;
      if ( hasRsrq4 )
      {
          $(id+"_rsrq4_head").show();
      }
      else
      {
          $(id+"_rsrq4_head").hide();
      }
      hasRsrq5 = info.rsrq2 || info.sinr2;
      if ( hasRsrq5 )
      {
          $(id+"_rsrq5_head").show();
      }
      else
      {
          $(id+"_rsrq5_head").hide();
      }

      if ( info.band )
      {
          $(id+"_band4_head").show();
      }
      else
      {
          $(id+"_band4_head").hide();
      }
      if ( info.band2 )
      {
          $(id+"_band5_head").show();
      }
      else
      {
          $(id+"_band5_head").hide();
      }
    });
  }

/* load the configure on the input */
function lte_basic(v)
{
    config = v[0];
    if(!config){
      config = {}
    }
    modem = v[1];
    if(!modem){
      modem = object;
    }
    window.modem = modem;
    window.object = object;

    var smsTabLi = $('#lteTabs a[href="#ltesms"]').parent();
    he.load([ '?modem@smsd' ]).then(function(sv) {
        if (sv[0] === true) {
            smsTabLi.show();
        } else {
            smsTabLi.hide();
        }
    });

    /* profile — stored on modem@lte */
    if ( config.profile == "enable" )
    {
        $('#profile').prop('checked', true );
    }
    else
    {
        $('#profile').prop('checked', false );
    }
    if ( config.profile_cfg )
    {
		$('#dial').val(config.profile_cfg.dial);
		$('#apn').val(config.profile_cfg.apn);
		$('#user').val(config.profile_cfg.user);
		$('#passwd').val(config.profile_cfg.passwd);
		$('#type').val(config.profile_cfg.type||"ipv4");
		$('#auth').val(config.profile_cfg.auth);
    }
    else
    {
    operator = v[2];
    if(!operator){
      operator = {};
    }
		if ( operator )
		{
			$('#dial').val(operator.dial);
			$('#apn').val(operator.apn);
			$('#user').val(operator.user);
			$('#passwd').val(operator.passwd);
			$('#type').val(operator.type||"ipv4");
			$('#auth').val(operator.auth);
		}
    }
    $('#profile').unbind('change').change(function () {
      if ($(this).prop('checked'))
      {
        $('#profile_cfg').show();
      }
      else
      {
        $('#profile_cfg').hide();
      }
    }).trigger('change');
}

/* save the configure */
function config_save() {
    if (!config) return;

    var copy = JSON.parse(JSON.stringify(config));

    config.profile = boole2able($('#profile').prop('checked'));
    if (config.profile === "enable") {
        config.profile_cfg = config.profile_cfg || {};
        var p = config.profile_cfg;
        p.dial = $('#dial').val();
        p.apn = $('#apn').val();
        p.user = $('#user').val();
        p.passwd = $('#passwd').val();
        p.type = $('#type').val();
        p.auth = $('#auth').val();
    }

    if (ocompare(config, copy)) {
        page.alert({ message: $.i18n('No changes to apply') });
        return;
    }

    var msg = $.i18n('Changing this setting will disconnect the LTE/NR connection.');
    page.confirm({ message: msg }).then(function(result) {
        if (!result) return location.reload();
        he.exec([ object + "=" + JSON.stringify(config) ]).then(function() {
            page.hint2succeed($.i18n('Modified successfully'));
            config_load();
        });
    });
}

/* init */
page.password('passwd', 'password-icon' );
$.i18n().load( page.lang('lte') ).then( function () {
    /* init the language */
    $.i18n().locale = lang; $('body').i18n();

    status_load();
    /* load the configure */
    config_load();

    /* bind the button */
    $('#lte_btn').on(ace.click_event, function () {
        if ( !state ) {
            return;
        }
        if ( state.status == "up" || state.status == "uping" || state.status == "connect" ||
             state.status == "connecting" || state.status == "block" || state.status == "failed" ||
             state.status == "register" || state.status == "setup" )
        {
            he.exec( [ object+'.shut' ] ).then( function(result){status_load();} );
        }
        else
        {
            he.exec( [ object+'.setup' ] ).then( function(result){status_load();} );
        }
    });

    // set the timer
    page.timing({
      refresh: function ()
      {
          status_load();
      },
      interval: 1000
    });

    /* bind the refresh */
    $('#refresh').on(ace.click_event, function () {
        location.reload();
    });
    /* bind the apply */
    $('#apply').on(ace.click_event, function () {
        config_save();
    });
});

// tab pages reuse ltemodem/ltesms/lteat content
$(document).ready(function() {
    var TabManager = {
        mainRoute: window.location.hash.split('?')[0],

        tabs: {
            ltemodem: { htmlUrl: '/content/ltemodem.html'},
            ltesms:   { htmlUrl: '/content/ltesms.html'},
            lteat:  { htmlUrl: '/content/lteat.html'}
        },

        init: function() {
            this.syncParams();

            $('#lteTabs a').off('click').on('click', this.handleTabClick.bind(this));

            this.restoreState();
        },

        syncParams: function() {
            var currentHash = window.location.hash;

            var urlModem  = page.param('modem', currentHash);
            var urlObject = page.param('object', currentHash);
            var urlTab    = page.param('tab', currentHash);

            // object is modem@lte / modem@lte2 — modem defaults to the same object
            window.object = urlObject || window.object || 'modem@lte';
            window.modem = urlModem || window.modem || window.object;
            object = window.object;

            var tabId = urlTab || 'lte';

            if (!urlModem || !urlObject || !urlTab) {
                this.updateUrl(tabId, true);
            }
        },

        updateUrl: function(tabId) {
            var newHash = this.mainRoute + '?tab=' + tabId +
                          '&modem=' + encodeURIComponent(window.modem) +
                          '&object=' + encodeURIComponent(window.object);

            if (window.location.hash === newHash) return;
            history.replaceState(null, '', window.location.pathname + newHash);
        },

        handleTabClick: function(e) {
            e.preventDefault();
            var tabId = $(e.currentTarget).attr('href').substring(1);

            this.updateUrl(tabId, false);
            this.switchToTab(tabId);
        },

        restoreState: function() {
            var currentHash = window.location.hash;
            var tabId = page.param('tab', currentHash) || sessionStorage.getItem('lte_active_tab') || 'lte';
            this.switchToTab(tabId);
        },

        switchToTab: function(tabId) {
          var self = this;
          var $tabContent = $('.tab-content');
          var $newPane = $('#' + tabId);
          var $oldPane = $('.tab-pane.active');

          $('#lteTabs li').removeClass('active');
          $('#lteTabs a[href="#' + tabId + '"]').parent().addClass('active');

          var currentHeight = $oldPane.outerHeight();
          if(currentHeight > 0) {
              $tabContent.css('min-height', currentHeight + 'px');
          }

          $oldPane.removeClass('active');

          if (tabId === 'lte') {
              $('.tab-pane').removeClass('active');
              $('#lte').addClass('active');
              $tabContent.css('min-height', '');
              if (typeof config_load === 'function') config_load();
              return;
          }

          var tab = this.tabs[tabId];
          $.ajax({
              url: tab.htmlUrl,
              type: 'GET',
              dataType: 'html',
              success: function(data) {
                  $newPane.html(data);

                  $oldPane.removeClass('active');
                  $newPane.addClass('active');

                  setTimeout(function() {
                      $tabContent.css('min-height', '');
                  }, 100);
              }
          });
      },
    };

    TabManager.init();
});
