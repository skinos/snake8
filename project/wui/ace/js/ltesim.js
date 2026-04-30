var lte;
var modem = page.param('modem', location.hash);
/* Same target as lte.js TabManager: hash wins, then session default from parent page */
var object = page.param('object', location.hash)  || 'ifname@lte';

function lte_sim() {
    window.LteConfigManager.loadStatus(object, true).then(function(v) {
        lte = v[0];
        $("#button_sim").show();
        /* bsim 主开关 */
        $('#bsim').off('change').on('change', function () {
            $(this).prop('checked') ? $('#bsim_cfg').show() : $('#bsim_cfg').hide();
        });

        /* mode 切换：控制 mode_cfg 显隐 */
        $('#simmode').off('change').on('change', function (e) {
            var type = e.target.value;
            switch (type) {
                case 'auto':
                    $('#mode_cfg').show();
                    break;
                case 'detect':
                case 'main':
                case 'back':
                default:
                    /* detect is not implemented in UI yet — same as fixed SIM slots */
                    $('#mode_cfg').hide();
                    break;
            }
        });

        /* profile 开关：控制 profile_cfg 显隐 */
        $('#simprofile').off('change').on('change', function () {
            $(this).prop('checked') ? $('#simprofile_cfg').show() : $('#simprofile_cfg').hide();
        });


        /* bsim 主开关赋值 */
        $('#bsim').prop('checked', lte.bsim === "enable");

        if (lte.bsim_cfg) {
            /* mode赋值 */
            $('#simmode').val(lte.bsim_cfg.mode || "auto");

            /* mode_cfg 内各字段赋值 */
            $('#simcard_failed_threshold_sim').val(lte.bsim_cfg.simcard_failed_threshold || '');
            $('#simcard_failed_threshold2_sim').val(lte.bsim_cfg.simcard_failed_threshold2 || '');
            $('#simcard_failed_threshold3_sim').val(lte.bsim_cfg.simcard_failed_threshold3 || '');
            $('#simcard_failed_everytime_sim').val(lte.bsim_cfg.simcard_failed_everytime || '');

            $('#signal_failed_threshold_sim').val(lte.bsim_cfg.signal_failed_threshold || '');
            $('#signal_failed_threshold2_sim').val(lte.bsim_cfg.signal_failed_threshold2 || '');
            $('#signal_failed_threshold3_sim').val(lte.bsim_cfg.signal_failed_threshold3 || '');
            $('#signal_failed_everytime_sim').val(lte.bsim_cfg.signal_failed_everytime || '');

            $('#attach_failed_threshold_sim').val(lte.bsim_cfg.attach_failed_threshold || '');
            $('#attach_failed_threshold2_sim').val(lte.bsim_cfg.attach_failed_threshold2 || '');
            $('#attach_failed_threshold3_sim').val(lte.bsim_cfg.attach_failed_threshold3 || '');
            $('#attach_failed_everytime_sim').val(lte.bsim_cfg.attach_failed_everytime || '');

            $('#failed_threshold_sim').val(lte.bsim_cfg.failed_threshold || '');
            $('#failed_threshold2_sim').val(lte.bsim_cfg.failed_threshold2 || '');
            $('#failed_threshold3_sim').val(lte.bsim_cfg.failed_threshold3 || '');
            $('#failed_everytime_sim').val(lte.bsim_cfg.failed_everytime || '');

            $('#failover').val(lte.bsim_cfg.failover || '');
            $('#simpin').val(lte.bsim_cfg.pin || '');
            $('#keeplive_switch').prop('checked', lte.bsim_cfg.keeplive_switch === "enable");

            /* profile 开关赋值 */
            $('#simprofile').prop('checked', lte.bsim_cfg.profile === "enable");

            /* profile_cfg 内各字段赋值 */
            if (lte.bsim_cfg.profile_cfg) {
                $('#simdial').val(lte.bsim_cfg.profile_cfg.dial || '');
                $('#simauth').val(lte.bsim_cfg.profile_cfg.auth || '');
                $('#simapn').val(lte.bsim_cfg.profile_cfg.apn || '');
                $('#simuser').val(lte.bsim_cfg.profile_cfg.user || '');
                $('#simpasswd').val(lte.bsim_cfg.profile_cfg.passwd || '');
                $('#simtype').val(lte.bsim_cfg.profile_cfg.type || 'ipv4');
            }
        } else {
            /* 没有 bsim_cfg 时，重置相关字段 */
            $('#simmode').val('');
            $('#keeplive_switch').prop('checked', false);
            $('#simprofile').prop('checked', false);
        }

        $('#bsim').trigger('change');     /* 控制 bsim_cfg 显隐 */
        $('#simmode').trigger('change');     /* 控制 mode_cfg 显隐 */
        $('#simprofile').trigger('change');  /* 控制 profile_cfg 显隐 */
    });
}

/* save the configure */
function sim_save()
{
  if ( lte == null )
  {
    return;
  }
  var ltecopy = JSON.parse(JSON.stringify(lte));

  lte.bsim = boole2able( $('#bsim').prop('checked') );
  if ( lte.bsim == "enable" )
  {
      if ( !lte.bsim_cfg )
      {
        lte.bsim_cfg = {};
      }
      lte.bsim_cfg.mode = $('#simmode').val();

      /* Threshold switching rules apply when mode is auto (lte.md bsim_cfg.mode) */
      if ( lte.bsim_cfg.mode === "auto" )
      {
          lte.bsim_cfg.simcard_failed_threshold = $('#simcard_failed_threshold_sim').val();
          lte.bsim_cfg.simcard_failed_threshold2 = $('#simcard_failed_threshold2_sim').val();
          lte.bsim_cfg.simcard_failed_threshold3 = $('#simcard_failed_threshold3_sim').val();
          lte.bsim_cfg.simcard_failed_everytime = $('#simcard_failed_everytime_sim').val();

          lte.bsim_cfg.signal_failed_threshold = $('#signal_failed_threshold_sim').val();
          lte.bsim_cfg.signal_failed_threshold2 = $('#signal_failed_threshold2_sim').val();
          lte.bsim_cfg.signal_failed_threshold3 = $('#signal_failed_threshold3_sim').val();
          lte.bsim_cfg.signal_failed_everytime = $('#signal_failed_everytime_sim').val();

          lte.bsim_cfg.attach_failed_threshold = $('#attach_failed_threshold_sim').val();
          lte.bsim_cfg.attach_failed_threshold2 = $('#attach_failed_threshold2_sim').val();
          lte.bsim_cfg.attach_failed_threshold3 = $('#attach_failed_threshold3_sim').val();
          lte.bsim_cfg.attach_failed_everytime = $('#attach_failed_everytime_sim').val();

          lte.bsim_cfg.failed_threshold = $('#failed_threshold_sim').val();
          lte.bsim_cfg.failed_threshold2 = $('#failed_threshold2_sim').val();
          lte.bsim_cfg.failed_threshold3 = $('#failed_threshold3_sim').val();
          lte.bsim_cfg.failed_everytime = $('#failed_everytime_sim').val();

          /* failover / keeplive_switch: bsim_cfg fields used regardless of mode (lte.md) */
          lte.bsim_cfg.failover = $('#failover').val();
          lte.bsim_cfg.keeplive_switch = boole2able( $('#keeplive_switch').prop('checked') );
      }

      lte.bsim_cfg.pin = $('#simpin').val();
      lte.bsim_cfg.profile = boole2able( $('#simprofile').prop('checked') );
      if ( lte.bsim_cfg.profile == "enable" )
      {
        if ( !lte.bsim_cfg.profile_cfg )
        {
            lte.bsim_cfg.profile_cfg = {};
        }
        lte.bsim_cfg.profile_cfg.dial = $('#simdial').val();
        lte.bsim_cfg.profile_cfg.auth = $('#simauth').val();
        lte.bsim_cfg.profile_cfg.apn = $('#simapn').val();
        lte.bsim_cfg.profile_cfg.user = $('#simuser').val();
        lte.bsim_cfg.profile_cfg.passwd = $('#simpasswd').val();
        lte.bsim_cfg.profile_cfg.type = $('#simtype').val();
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
    
    var cmds = [ object+"="+JSON.stringify(lte) ];
    he.exec(cmds).then( function(){
      page.hint2succeed( $.i18n('Modify successfully') );
      lte_sim();
    });
    
  });
}

/* init */
page.password('simpasswd', 'password-icon');
$.i18n().load( page.lang('lte') ).then( function () {
  /* init the language */
  $.i18n().locale = lang; $('body').i18n();
  /* init the table */

  /* load the configure */
  lte_sim();

  /* bind the refresh */
  $('#sim_refresh').on(ace.click_event, function () {
    location.reload();
  });
  /* bind the apply */
  $('#sim_apply').off('click').on('click', function() {
        sim_save();
    });
});