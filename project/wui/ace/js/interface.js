
var lte;
var lte2;
var wan;
var wan2;
var wan3;
var wan4;
var lan;
var lan2;
var lan3;
var lan4;
var wisp;
var wisp2;
var sw;
/* flush interval */
var flush_interval = 2;

function ltedev_show( info, id )
{
    if ( !info )
    {
        $(id).hide();
        return;
    }
    if ( info.state )
    {
        $(id+"_status").text( $.i18n(info.state) );
    }
    else
    {
        $(id+"_status").text( "" );
    }
    $(id).show();
    if ( info.operator )
    {
        $(id+"_operator").text( $.i18n(info.operator) );
    }
    else if ( info.plmn )
    {
        $(id+"_operator").text( $.i18n(info.plmn) );
    }
    else
    {
        $(id+"_operator").text( "" );
    }
    if ( info.nettype )
    {
        $(id+"_nettype").text( info.nettype );
    }
    else
    {
        $(id+"_nettype").text( "" );
    }
    if ( info.signal )
    {
        $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );
    }
    else
    {
        $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_0.png" );
    }
    if ( info.csq )
    {
        $(id+"_csq").text( info.csq );
    }
    else
    {
        $(id+"_csq").text( "" );
    }
    /* Main signal */
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
    if ( !info.rssi && !info.rsrp )
    {
        $(id+"_rssi").text( $.i18n("nosignal") );
    }
    if ( info.rsrq )
    {
        $(id+"_rsrq").text( "RSRQ:"+info.rsrq+"dB" );
        $(id+"_rsrq_head").show();
    }
    else
    {
        $(id+"_rsrq").text("");
    }
    if ( info.sinr )
    {
        $(id+"_sinr").text( "SINR:"+info.sinr+"dB" );
        $(id+"_rsrq_head").show();
    }
    else
    {
        $(id+"_sinr").text("");
    }
    if ( !info.sinr && !info.rsrq )
    {
        $(id+"_rsrq_head").hide();
    }
    if ( info.band )
    {
        $(id+"_band").text( info.band );
        $(id+"_band_head").show();
    }
    else
    {
        $(id+"_band_head").hide();
    }
	if ( info.nettype2 )
	{
        $(id+"_nettype5").text( info.nettype2 );
        $(id+"_nettype5_head").show();
        if ( info.rsrp2 )
        {
            $(id+"_rsrp5").text( info.rsrp2+"dBm" );
        }
        if ( info.signal2 )
        {
            $(id+"_rssiimg5").attr( "src", "/assets/css/images/signal_"+info.signal2+".png" );
            $(id+"_rssi5_head").show();
        }
        else
        {
            $(id+"_rssi5_head").hide();
        }
        if ( info.rsrq2 )
        {
            $(id+"_rsrq5").text( "RSRQ:"+info.rsrq2+"dB" );
            $(id+"_rsrq5_head").show();
        }
        else
        {
            $(id+"_rsrq5").text( "" );
        }
        if ( info.sinr2 )
        {
            $(id+"_sinr5").text( "SINR:"+info.sinr2+"dB" );
            $(id+"_rsrq5_head").show();
        }
        else
        {
            $(id+"_sinr5").text( "" );
        }
        if ( !info.sinr2 && !info.rsrq2 )
        {
            $(id+"_rsrq5_head").hide();
        }
        if ( info.band2 )
        {
            $(id+"_band5").text( info.band2 );
            $(id+"_band5_head").show();
        }
        else
        {
            $(id+"_band5_head").hide();
        }
	}
	else
	{
        $(id+"_nettype5_head").hide();
        $(id+"_rssi5_head").hide();
        $(id+"_rsrq5_head").hide();
        $(id+"_band5_head").hide();
	}
	/* Device */
    if ( info.name )
	{
        $(id+"_vidpid").text( info.name );
    }
	if ( info.imei )
	{
    	$(id+"_imei").text( $.i18n(info.imei) );
	}
	if ( info.imsi )
	{
    	$(id+"_imsi").text( $.i18n(info.imsi) );
	}
    if ( info.iccid )
    {
        $(id+"_iccid").text( $.i18n(info.iccid) );
    }
    else
    {
        $(id+"_iccid").text( "" );
    }
    if ( info.lac )
    {
        $(id+"_lac").text( info.lac );
        $(id+"_lac_head").show();
    }
    else
    {
        $(id+"_lac_head").hide();
    }
    if ( info.ci )
    {
        $(id+"_ci").text( info.ci );
        $(id+"_ci_head").show();
    }
    else
    {
        $(id+"_ci_head").hide();
    }
    if ( info.arfcn )
    {
        $(id+"_arfcn").text( info.arfcn );
        $(id+"_arfcn_head").show();
    }
    else
    {
        $(id+"_arfcn_head").hide();
    }
}
function lte_show( info, id )
{
    if ( !info )
    {
        $(id).hide();
        return;
    }
    /* status end btn */
    if ( info.status )
    {
        $(id).show();
        $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
        $(id+"_status").text( $.i18n(info.status) );
        if ( info.status == "up" || info.status == "uping" || info.status == "connect" )
        {
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
    if ( info.operator )
    {
        $(id+"_operator").text( $.i18n(info.operator) );
    }
    else if ( info.plmn )
    {
        $(id+"_operator").text( $.i18n(info.plmn) );
    }
    else
    {
        $(id+"_operator").text( "" );
    }
    if ( info.nettype )
    {
        $(id+"_nettype").text( info.nettype );
    }
    else
    {
        $(id+"_nettype").text( "" );
    }
    if ( info.signal )
    {
        $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );
    }
    else
    {
        $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_0.png" );
    }
    if ( info.csq )
    {
        $(id+"_csq").text( info.csq );
    }
    else
    {
        $(id+"_csq").text( "" );
    }
    /* Main signal */
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
    if ( !info.rssi && !info.rsrp )
    {
        $(id+"_rssi").text( $.i18n("nosignal") );
    }
    if ( info.rsrq )
    {
        $(id+"_rsrq").text( "RSRQ:"+info.rsrq+"dB" );
        $(id+"_rsrq_head").show();
    }
    else
    {
        $(id+"_rsrq").text("");
    }
    if ( info.sinr )
    {
        $(id+"_sinr").text( "SINR:"+info.sinr+"dB" );
        $(id+"_rsrq_head").show();
    }
    else
    {
        $(id+"_sinr").text("");
    }
    if ( !info.sinr && !info.rsrq )
    {
        $(id+"_rsrq_head").hide();
    }
    if ( info.band )
    {
        $(id+"_band").text( info.band );
        $(id+"_band_head").show();
    }
    else
    {
        $(id+"_band_head").hide();
    }
    /* 5G signal */
	if ( info.nettype2 )
	{
        $(id+"_nettype5").text( info.nettype2 );
        $(id+"_nettype5_head").show();
        if ( info.rsrp2 )
        {
            $(id+"_rsrp5").text( info.rsrp2+"dBm" );
        }
        if ( info.signal2 )
        {
            $(id+"_rssiimg5").attr( "src", "/assets/css/images/signal_"+info.signal2+".png" );
            $(id+"_rssi5_head").show();
        }
        else
        {
            $(id+"_rssi5_head").hide();
        }
        if ( info.rsrq2 )
        {
            $(id+"_rsrq5").text( "RSRQ:"+info.rsrq2+"dB" );
            $(id+"_rsrq5_head").show();
        }
        else
        {
            $(id+"_rsrq5").text( "" );
        }
        if ( info.sinr2 )
        {
            $(id+"_sinr5").text( "SINR:"+info.sinr2+"dB" );
            $(id+"_rsrq5_head").show();
        }
        else
        {
            $(id+"_sinr5").text( "" );
        }
        if ( !info.sinr2 && !info.rsrq2 )
        {
            $(id+"_rsrq5_head").hide();
        }
        if ( info.band2 )
        {
            $(id+"_band5").text( info.band2 );
            $(id+"_band5_head").show();
        }
        else
        {
            $(id+"_band5_head").hide();
        }
	}
	else
	{
        $(id+"_nettype5_head").hide();
        $(id+"_rssi5_head").hide();
        $(id+"_rsrq5_head").hide();
        $(id+"_band5_head").hide();
	}
	/* Device */
	if ( info.name )
	{
        $(id+"_vidpid").text( info.name );
    }
	if ( info.imei )
	{
    	$(id+"_imei").text( $.i18n(info.imei) );
	}
	if ( info.imsi )
	{
    	$(id+"_imsi").text( $.i18n(info.imsi) );
	}
    if ( info.iccid )
    {
        $(id+"_iccid").text( $.i18n(info.iccid) );
    }
    else
    {
        $(id+"_iccid").text( "" );
    }
    if ( info.lac )
    {
        $(id+"_lac").text( info.lac );
        $(id+"_lac_head").show();
    }
    else
    {
        $(id+"_lac_head").hide();
    }
    if ( info.ci )
    {
        $(id+"_ci").text( info.ci );
        $(id+"_ci_head").show();
    }
    else
    {
        $(id+"_ci_head").hide();
    }
    if ( info.arfcn )
    {
        $(id+"_arfcn").text( info.arfcn );
        $(id+"_arfcn_head").show();
    }
    else
    {
        $(id+"_arfcn_head").hide();
    }
    /* IPV4 */
    $(id+"_ip").text( info.ip||' ' );
    $(id+"_mask").text( info.mask||' ' );
    if ( !info.gw || info.gw == "0.0.0.0" )
    {
        $(id+"_gateway").text( info.dstip||' ' );
    }
    else
    {
        $(id+"_gateway").text( info.gw||' ' );
    }
    if ( info.delay )
    {
        if ( info.delay == "failed" || info.delay == "block" )
        {
            $(id+"_delay").text( $.i18n(info.delay) );
        }
        else
        {
            $(id+"_delay").text( info.delay+"ms" );
        }
        $(id+"_delay_head").show();
    }
    else
    {
        $(id+"_delay_head").hide();
    }
    $(id+"_dns").text( info.dns||' ' );
    $(id+"_dns2").text( info.dns2||' ' );
    /* IPV6 */
    if ( info.method && info.method != "disable" )
    {
        $(id+"_method").text( $.i18n(info.method) );
        $(id+"_method_head").show();
        if ( info.addr )
        {
            $(id+"_addr").text( info.addr );
            $(id+"_addr_head").show();
        }
        else
        {
            $(id+"_addr_head").hide();
        }
        if ( info.addr2 )
        {
            $(id+"_addr2").text( info.addr2 );
            $(id+"_addr2_head").show();
        }
        else
        {
            $(id+"_addr2_head").hide();
        }
        if ( info.addr3 )
        {
            $(id+"_addr3").text( info.addr3 );
            $(id+"_addr3_head").show();
        }
        else
        {
            $(id+"_addr3_head").hide();
        }
        if ( info.hop )
        {
            $(id+"_hop").text( info.hop );
            $(id+"_hop_head").show();
        }
        else
        {
            $(id+"_hop_head").hide();
        }
        if ( info.resolve )
        {
            $(id+"_resolve").text( info.resolve );
            $(id+"_resolve_head").show();
        }
        else
        {
            $(id+"_resolve_head").hide();
        }
        if ( info.resolve2 )
        {
            $(id+"_resolve2").text( info.resolve2 );
            $(id+"_resolve2_head").show();
        }
        else
        {
            $(id+"_resolve2_head").hide();
        }
    }
    else
    {
        $(id+"_method_head").hide();
    }
    /* TC */
    $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
    $(id+"_livetime").text( info.livetime||' ' );
}
function wan_show( info, id    )
{
    if ( !info )
    {
        $(id).hide();
        return;
    }
    if ( info.status )
    {
        $(id).show();
        $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
        $(id+"_status").text( $.i18n(info.status) );
        if ( info.status == "uping" )
        {
            if ( info.step && info.step != "online" )
            {
                $(id+"_status").text( $.i18n(info.step) );
            }
        }
        else if ( info.status == "down" )
        {
            if ( info.step && info.step != "online" )
            {
                $(id+"_status").text( $.i18n(info.step) );
            }
            $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
        }
        if ( info.error )
        {
            $(id+"_status").text( $.i18n(info.error) );
        }
    }
    else
    {
        $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
        $(id+"_status").text( $.i18n("down") );
    }
    if ( info.mode )
    {
        $(id+"_mode").text( $.i18n(info.mode) );
    }
    $(id+"_mac").text( info.mac||' ' );
    /* IPV4 */
    $(id+"_ip").text( info.ip||' ' );
    $(id+"_mask").text( info.mask||' ' );
    if ( !info.gw || info.gw == "0.0.0.0" )
    {
        $(id+"_gateway").text( info.dstip||' ' );
    }
    else
    {
        $(id+"_gateway").text( info.gw||' ' );
    }
    $(id+"_dns").text( info.dns||' ' );
    $(id+"_dns2").text( info.dns2||' ' );
    /* IPV6 */
    if ( info.method && info.method != "disable" )
    {
        $(id+"_method").text( $.i18n(info.method) );
        $(id+"_method_head").show();
        if ( info.addr )
        {
            $(id+"_addr").text( info.addr );
            $(id+"_addr_head").show();
        }
        else
        {
            $(id+"_addr_head").hide();
        }
        if ( info.addr2 )
        {
            $(id+"_addr2").text( info.addr2 );
            $(id+"_addr2_head").show();
        }
        else
        {
            $(id+"_addr2_head").hide();
        }
        if ( info.addr3 )
        {
            $(id+"_addr3").text( info.addr3 );
            $(id+"_addr3_head").show();
        }
        else
        {
            $(id+"_addr3_head").hide();
        }
        if ( info.hop )
        {
            $(id+"_hop").text( info.hop );
            $(id+"_hop_head").show();
        }
        else
        {
            $(id+"_hop_head").hide();
        }
        if ( info.resolve )
        {
            $(id+"_resolve").text( info.resolve );
            $(id+"_resolve_head").show();
        }
        else
        {
            $(id+"_resolve_head").hide();
        }
        if ( info.resolve2 )
        {
            $(id+"_resolve2").text( info.resolve2 );
            $(id+"_resolve2_head").show();
        }
        else
        {
            $(id+"_resolve2_head").hide();
        }
    }
    else
    {
        $(id+"_method_head").hide();
    }
    /* TC */
    $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
    if ( info.delay )
    {
        if ( info.delay == "failed" || info.delay == "block" )
        {
            $(id+"_ack").text( $.i18n(info.delay) );
        }
        else
        {
            $(id+"_ack").text( info.delay+"ms" );
        }
        $(id+"_ack_head").show();
    }
    else
    {
        $(id+"_ack_head").hide();
    }
    $(id+"_livetime").text( info.livetime||' ' );
    
}
function lan_show( info, id )
{
    if ( !info )
    {
        $(id).hide();
        return;
    }
    if ( info.status )
    {
        $(id).show();
        $(id+"_status").text( $.i18n(info.status) );
        $(id+"_mac").text( info.mac );
        if ( info.status == "up" )
        {
            /* IPV4 */
            $(id+"_ip").text( info.ip );
            $(id+"_mask").text( info.mask );
            $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
            /* IPV6 */
            if ( info.method && info.method != "disable" )
            {
                $(id+"_method").text( $.i18n(info.method) );
                $(id+"_method_head").show();
                if ( info.addr )
                {
                    $(id+"_addr").text( info.addr );
                    $(id+"_addr_head").show();
                }
                else
                {
                    $(id+"_addr_head").hide();
                }
                if ( info.addr2 )
                {
                    $(id+"_addr2").text( info.addr2 );
                    $(id+"_addr2_head").show();
                }
                else
                {
                    $(id+"_addr2_head").hide();
                }
                if ( info.addr3 )
                {
                    $(id+"_addr3").text( info.addr3 );
                    $(id+"_addr3_head").show();
                }
                else
                {
                    $(id+"_addr3_head").hide();
                }
                if ( info.hop )
                {
                    $(id+"_hop").text( info.hop );
                    $(id+"_hop_head").show();
                }
                else
                {
                    $(id+"_hop_head").hide();
                }
            }
            else
            {
                $(id+"_method_head").hide();
            }
        }
    }
    else
    {
        $(id+"_status").text( $.i18n("down") );
    }
}
function wisp_show( info, id )
{
    if ( !info )
    {
        $(id).hide();
        return;
    }
    /* status end btn */
    if ( info.status )
    {
        $(id).show();
        $(id+"_btn").html( '<i class="ace-icon fa fa-pause"></i>' );
        $(id+"_status").text( $.i18n(info.status) );
        if ( info.status == "uping" )
        {
            if ( info.step && info.step != "online" )
            {
                $(id+"_status").text( $.i18n(info.step) );
            }
        }
        else if ( info.status == "down" )
        {
            if ( info.step && info.step != "online" )
            {
                $(id+"_status").text( $.i18n(info.step) );
            }
            $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
        }
        if ( info.error )
        {
            $(id+"_status").text( $.i18n(info.error) );
        }
    }
    else
    {
        $(id+"_btn").html( '<i class="ace-icon fa fa-play"></i>' );
        $(id+"_status").text( $.i18n("down") );
    }
    if ( info.mode )
    {
        $(id+"_mode").text( $.i18n(info.mode) );
    }
    $(id+"_peer").text( info.peer||' ' );
    $(id+"_peermac").text( info.peermac||' ' );
    if ( info.rate )
    {
        $(id+"_rate").text( info.rate+'Mbps' );
    }
    $(id+"_channel").text( info.channel||' ' );
    if ( info.sig )
    {
        $(id+"_rssi").text( info.sig+"%" );
    }
    else if ( info.rssi )
    {
        $(id+"_rssi").text( info.rssi+"dBm" );
    }
    if ( info.signal )
    {
        $(id+"_rssiimg").attr( "src", "/assets/css/images/signal_"+info.signal+".png" );            
    }
    $(id+"_mac").text( info.mac||' ' );
    /* IPV4 */
    $(id+"_ip").text( info.ip||' ' );
    $(id+"_mask").text( info.mask||' ' );
    if ( !info.gw || info.gw == "0.0.0.0" )
    {
        $(id+"_gateway").text( info.dstip||' ' );
    }
    else
    {
        $(id+"_gateway").text( info.gw||' ' );
    }
    $(id+"_dns").text( info.dns||' ' );
    $(id+"_dns2").text( info.dns2||' ' );
    /* IPV6 */
    if ( info.method && info.method != "disable" )
    {
        $(id+"_method").text( $.i18n(info.method) );
        $(id+"_method_head").show();
        if ( info.addr )
        {
            $(id+"_addr").text( info.addr );
            $(id+"_addr_head").show();
        }
        else
        {
            $(id+"_addr_head").hide();
        }
        if ( info.addr2 )
        {
            $(id+"_addr2").text( info.addr2 );
            $(id+"_addr2_head").show();
        }
        else
        {
            $(id+"_addr2_head").hide();
        }
        if ( info.addr3 )
        {
            $(id+"_addr3").text( info.addr3 );
            $(id+"_addr3_head").show();
        }
        else
        {
            $(id+"_addr3_head").hide();
        }
        if ( info.hop )
        {
            $(id+"_hop").text( info.hop );
            $(id+"_hop_head").show();
        }
        else
        {
            $(id+"_hop_head").hide();
        }
        if ( info.resolve )
        {
            $(id+"_resolve").text( info.resolve );
            $(id+"_resolve_head").show();
        }
        else
        {
            $(id+"_resolve_head").hide();
        }
        if ( info.resolve2 )
        {
            $(id+"_resolve2").text( info.resolve2 );
            $(id+"_resolve2_head").show();
        }
        else
        {
            $(id+"_resolve2_head").hide();
        }
    }
    else
    {
        $(id+"_method_head").hide();
    }
    /* TC */
    $(id+"_rxtx").text( byte2readable( (info.rx_bytes||"0") ) + " / " + byte2readable( (info.tx_bytes||"0") ) );
    if ( info.delay )
    {
        if ( info.delay == "failed" || info.delay == "block" )
        {
            $(id+"_ack").text( $.i18n(info.delay) );
        }
        else
        {
            $(id+"_ack").text( info.delay+"ms" );
        }
        $(id+"_ack_head").show();
    }
    else
    {
        $(id+"_ack_head").hide();
    }
    $(id+"_livetime").text( info.livetime||' ' );
}
function switch_show( info )
{
    var name;
    var value;
    var i = 1;
    if ( !info )
    {
        $("#switch").hide();
        return;
    }
    $("#switch").show();
    for ( name in info )
    {
        value = info[name];
        if ( !value )
        {
            continue;
        }
        $("#sport"+i+"t").text( $.i18n(name) );
        if ( value.status == "up" )
        {
	        $("#sport"+i+"v").html( '<i class="ace-icon fa fa-check green"></i>' );
        }
        else
        {
	        $("#sport"+i+"v").html( '<i class="ace-icon fa fa-times red"></i>' );
        }
        $("#sport"+i).show();
        i++;
    }
}

/* load the configure on the input */
function interface_load()
{
  he.bkload( [ 'ifname@lte.status', 'ifname@lte2.status', 'modem@lte.status', 'modem@lte2.status', 'ifname@wan.status', 'ifname@wan2.status', 'ifname@wan3.status', 'ifname@wan4.status', 'ifname@lan.status', 'ifname@lan2.status', 'ifname@lan3.status', 'ifname@lan4.status', 'ifname@wisp.status', 'ifname@wisp2.status', 'arch@ethernet.status' ] ).then( function(v){
    if ( v[0] )
    {
		lte = v[0];
		lte_show( lte, "#lte" );
	}
	else
	{
		lte = v[2];
        ltedev_show( lte, "#ltedev" );
	}
    if ( v[1] )
    {
		lte2 = v[1];
		lte_show( lte2, "#lte2" );
	}
	else
	{
		lte2 = v[3];
        ltedev_show( lte2, "#lte2dev" );
	}
	wan = v[4];
	wan_show( wan, "#wan" );
	wan2 = v[5];
	wan_show( wan2, "#wan2" );
	wan3 = v[6];
	wan_show( wan3, "#wan3" );
	wan4 = v[7];
	wan_show( wan4, "#wan4" );
	lan = v[8];
	lan_show( lan, "#lan" );
	lan2 = v[9];
	lan_show( lan2, "#lan2" );
	lan3 = v[10];
	lan_show( lan3, "#lan3" );
	lan4 = v[11];
	lan_show( lan4, "#lan4" );
	wisp = v[12];
	wisp_show( wisp, "#wisp" );
	wisp2 = v[13];
	wisp_show( wisp2, "#wisp2" );
	sw = v[14];
	switch_show( sw );
  })
}


/* init */
$.i18n().load( page.lang('dashboard') ).then( function () {

  /* init the langauage */
  $.i18n().locale = lang; $('body').i18n();

  /* button bind */
  $('#lte_btn').on(ace.click_event, function () {
      if ( lte.status != "up" && lte.status != "uping" && lte.status != "connect" )
      {
          he.exec( ['wui@admin.ttyd_kill','ifname@lte.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@lte.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#lte2_btn').on(ace.click_event, function () {
      if ( lte2.status != "up" && lte2.status != "uping" && lte2.status != "connect" )
      {
          he.exec( [ 'wui@admin.ttyd_kill','ifname@lte2.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@lte2.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wan_btn').on(ace.click_event, function () {
      if ( wan.status == "down" )
      {
          he.exec( [ 'ifname@wan.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wan.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wan2_btn').on(ace.click_event, function () {
      if ( wan2.status == "down" )
      {
          he.exec( [ 'ifname@wan2.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wan2.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wan3_btn').on(ace.click_event, function () {
      if ( wan3.status == "down" )
      {
          he.exec( [ 'ifname@wan3.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wan3.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wan4_btn').on(ace.click_event, function () {
      if ( wan4.status == "down" )
      {
          he.exec( [ 'ifname@wan4.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wan4.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wisp_btn').on(ace.click_event, function () {
      if ( wisp.status == "down" )
      {
          he.exec( [ 'ifname@wisp.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wisp.shut' ] ).then( function(result){
              location.reload();
          } );
      }
  });
  $('#wisp2_btn').on(ace.click_event, function () {
      if ( wisp2.status == "down" )
      {
          he.exec( [ 'ifname@wisp2.setup' ] ).then( function(result){
              location.reload();
          } );
      }
      else
      {
          he.exec( [ 'ifname@wisp2.shut' ] ).then( function(result){
              location.reload();
          ;} );
      }
  });

  /* load the configure */
  interface_load();
  /* set the timer flush  */
  page.timing({
    refresh: function ()
    {
        interface_load();
    },
    interval: flush_interval*1000
  });

});


