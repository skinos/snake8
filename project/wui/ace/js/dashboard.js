
/* flush interval */
var flush_interval = 1;
/* global buffer */
var buff = {};

/* init the mem info */
buff["mem_data"] = [];
/* init the ifname@lte info */
buff["ifname@lte_max"] = 15;
buff["ifname@lte_rx"] = 0;
buff["ifname@lte_tx"] = 0;
buff["ifname@lte_rxdata"] = [];
buff["ifname@lte_txdata"] = [];
/* init the ifname@lte2 info */
buff["ifname@lte2_max"] = 15;
buff["ifname@lte2_rx"] = 0;
buff["ifname@lte2_tx"] = 0;
buff["ifname@lte2_rxdata"] = [];
buff["ifname@lte2_txdata"] = [];
/* init the ifname@wan info */
buff["ifname@wan_max"] = 15;
buff["ifname@wan_rx"] = 0;
buff["ifname@wan_tx"] = 0;
buff["ifname@wan_rxdata"] = [];
buff["ifname@wan_txdata"] = [];
/* init the ifname@wan2 info */
buff["ifname@wan2_max"] = 15;
buff["ifname@wan2_rx"] = 0;
buff["ifname@wan2_tx"] = 0;
buff["ifname@wan2_rxdata"] = [];
buff["ifname@wan2_txdata"] = [];
/* init the ifname@wan3 info */
buff["ifname@wan3_max"] = 15;
buff["ifname@wan3_rx"] = 0;
buff["ifname@wan3_tx"] = 0;
buff["ifname@wan3_rxdata"] = [];
buff["ifname@wan3_txdata"] = [];
/* init the ifname@wan4 info */
buff["ifname@wan4_max"] = 15;
buff["ifname@wan4_rx"] = 0;
buff["ifname@wan4_tx"] = 0;
buff["ifname@wan4_rxdata"] = [];
buff["ifname@wan4_txdata"] = [];
/* init the ifname@wisp info */
buff["ifname@wisp_max"] = 15;
buff["ifname@wisp_rx"] = 0;
buff["ifname@wisp_tx"] = 0;
buff["ifname@wisp_rxdata"] = [];
buff["ifname@wisp_txdata"] = [];
/* init the ifname@wisp2 info */
buff["ifname@wisp2_max"] = 15;
buff["ifname@wisp2_rx"] = 0;
buff["ifname@wisp2_tx"] = 0;
buff["ifname@wisp2_rxdata"] = [];
buff["ifname@wisp2_txdata"] = [];
/* init the ifname@lan info */
buff["ifname@lan_max"] = 15;
buff["ifname@lan_rx"] = 0;
buff["ifname@lan_tx"] = 0;
buff["ifname@lan_rxdata"] = [];
buff["ifname@lan_txdata"] = [];
/* init the ifname@lan2 info */
buff["ifname@lan2_max"] = 15;
buff["ifname@lan2_rx"] = 0;
buff["ifname@lan2_tx"] = 0;
buff["ifname@lan2_rxdata"] = [];
buff["ifname@lan2_txdata"] = [];
/* init the wifi@nssid info */
buff["wifi@nssid_max"] = 15;
buff["wifi@nssid_rx"] = 0;
buff["wifi@nssid_tx"] = 0;
buff["wifi@nssid_rxdata"] = [];
buff["wifi@nssid_txdata"] = [];
/* init the wifi@assid info */
buff["wifi@assid_max"] = 15;
buff["wifi@assid_rx"] = 0;
buff["wifi@assid_tx"] = 0;
buff["wifi@assid_rxdata"] = [];
buff["wifi@assid_txdata"] = [];
function lte_show( ifname, value )
{
    $("[id='"+ifname+"']").show();
    $("[id='"+ifname+"_status']").html( $.i18n(value.status) );
    $("[id='"+ifname+"_imei']").html(  $.i18n(value.imei) );
    $("[id='"+ifname+"_iccid']").html( $.i18n(value.iccid) );
    if ( value.operator )
    {
        $("[id='"+ifname+"_operator']").text( $.i18n(value.operator) );
    }
    else if ( value.plmn )
    {
        $("[id='"+ifname+"_operator']").text( $.i18n(value.plmn) );
    }
    else
    {
        $("[id='"+ifname+"_operator']").text( "" );
    }
    $("[id='"+ifname+"_nettype']").html( value.nettype||' ' );
    if ( value.signal )
    {
        $("[id='"+ifname+"_rssiimg']").attr( "src", "/assets/css/images/signal_"+value.signal+".png" );
    }
    else
    {
        $("[id='"+ifname+"_rssiimg']").attr( "src", "/assets/css/images/signal_0.png" );
    }
    if ( value.csq )
    {
        $("[id='"+ifname+"_csq']").text( value.csq );
    }
    else
    {
        $("[id='"+ifname+"_csq']").text( "" );            
    }

    if ( value.rssi )
    {
        $("[id='"+ifname+"_rssi']").text( value.rssi+"dBm" );            
    }
    else
    {
        $("[id='"+ifname+"_rssi']").text( "" );            
    }
    if ( value.rsrp )
    {
        $("[id='"+ifname+"_rsrp']").text( "RSRP:"+value.rsrp+"dBm" );
    }
    else
    {
        $("[id='"+ifname+"_rsrp']").text( "" );            
    }
    if ( !value.rssi && !value.rsrp )
    {
        $("[id='"+ifname+"_rssi']").text( $.i18n("nosignal") );
    }

    $("[id='"+ifname+"_ip']").html( value.ip||"" );
    if ( value.delay )
    {
        if ( value.delay == "failed" )
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("failed") );
        }
        else
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("Delay")+":"+value.delay );
        }
    }
    else
    {
        $("[id='"+ifname+"_delay']").text( "" );
    }
    if ( value.status != "up" )
    {
        return;
    }
    $("[id='"+ifname+"_livetime']").html( value.livetime||"" );
    $("[id='"+ifname+"_rx_bytes']").html( byte2readable( value.rx_bytes||"0" ) );
    $("[id='"+ifname+"_rx_packets']").html( value.rx_packets||"0" );
    $("[id='"+ifname+"_tx_bytes']").html( byte2readable( value.tx_bytes||"0" ) );
    $("[id='"+ifname+"_tx_packets']").html( value.tx_packets||"0" );
    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();
    /* get the byte */
    var rx = 0;
    var tx = 0;
    var orx = buff[ ifname+"_rx" ];
    var otx = buff[ ifname+"_tx" ];
    buff[ ifname+"_rx" ] = parseInt( value.rx_bytes );
    buff[ ifname+"_tx" ] = parseInt( value.tx_bytes );
    if ( orx != 0 || otx != 0 )
    {
        rx = buff[ ifname+"_rx" ] - orx;
        tx = buff[ ifname+"_tx" ] - otx;
        if ( rx < 0 )
        {
            rx = 0;
        }
        if ( tx < 0 )
        {
            tx = 0;
        }
    }
    if ( rx > 0 )
    {
        rx = Math.round( rx/1024 );
    }
    if ( tx > 0 )
    {
        tx = Math.round( tx/1024 );
    }
    if ( buff[ ifname+"_max" ] < rx )
    {
        buff[ ifname+"_max" ] = rx;
    }
    if ( buff[ ifname+"_max" ] < tx )
    {
        buff[ ifname+"_max" ] = tx;
    }
    buff[ifname+"_txdata"].push( [ d_time, tx ] );
    buff[ifname+"_rxdata"].push( [ d_time, rx ] );
    $("[id='"+ifname+"-charts']").css( {'width':'100%' , 'height':'294px'} );
    var datas =
    [
        { label: $.i18n('TX byte'), color: "#0000ff", data: buff[ifname+"_txdata"] },
        { label: $.i18n('RX byte'), color: "#00ff00", data: buff[ifname+"_rxdata"] },
    ];
    var opt =
    {
        points:{ clickable:true, hoverable:true },
        lines:{ show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: buff[ifname+"_max"], tickFormatter:function(axis) { return axis.toFixed(0)+"K"; } },
        xaxis: { mode: "time", timeformat: "%M:%S", minTickSize: [1, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "[id='"+ifname+"-charts']", datas, opt );
    /* get the usage */
    var usage;
    usage = (rx/(10*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_downusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_downusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
    usage = (tx/(10*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_upusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_upusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
}


function wan_show( ifname, value )
{
    $("[id='"+ifname+"']").show();
    $("[id='"+ifname+"_status']").html( $.i18n(value.status) );
    $("[id='"+ifname+"_ip']").html( value.ip||"" );
    if ( value.delay )
    {
        if ( value.delay == "failed" )
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("failed") );
        }
        else
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("Delay")+":"+value.delay );
        }
    }
    else
    {
        $("[id='"+ifname+"_delay']").text( "" );
    }
    $("[id='"+ifname+"_mask']").html( value.mask||"" );
    $("[id='"+ifname+"_gw']").html( value.gw||"" );
    $("[id='"+ifname+"_dns']").html( value.dns||"" );
    $("[id='"+ifname+"_mac']").html( value.mac||"" );
    if ( value.status != "up" )
    {
        return;
    }
    $("[id='"+ifname+"_livetime']").html( value.livetime||"" );
    $("[id='"+ifname+"_rx_bytes']").html( byte2readable( value.rx_bytes||"0" ) );
    $("[id='"+ifname+"_rx_packets']").html( value.rx_packets||"0" );
    $("[id='"+ifname+"_tx_bytes']").html( byte2readable( value.tx_bytes||"0" ) );
    $("[id='"+ifname+"_tx_packets']").html( value.tx_packets||"0" );
    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();
    /* get the byte */
    var rx = 0;
    var tx = 0;
    var orx = buff[ ifname+"_rx" ];
    var otx = buff[ ifname+"_tx" ];
    buff[ ifname+"_rx" ] = parseInt( value.rx_bytes );
    buff[ ifname+"_tx" ] = parseInt( value.tx_bytes );
    if ( orx != 0 || otx != 0 )
    {
        rx = buff[ ifname+"_rx" ] - orx;
        tx = buff[ ifname+"_tx" ] - otx;
        if ( rx < 0 )
        {
            rx = 0;
        }
        if ( tx < 0 )
        {
            tx = 0;
        }
    }
    if ( rx > 0 )
    {
        rx = Math.round( rx/1024 );
    }
    if ( tx > 0 )
    {
        tx = Math.round( tx/1024 );
    }
    if ( buff[ ifname+"_max" ] < rx )
    {
        buff[ ifname+"_max" ] = rx;
    }
    if ( buff[ ifname+"_max" ] < tx )
    {
        buff[ ifname+"_max" ] = tx;
    }
    buff[ifname+"_txdata"].push( [ d_time, tx ] );
    buff[ifname+"_rxdata"].push( [ d_time, rx ] );
    $("[id='"+ifname+"-charts']").css( {'width':'100%' , 'height':'294px'} );
    var datas =
    [
        { label: $.i18n('TX byte'), color: "#0000ff", data: buff[ifname+"_txdata"] },
        { label: $.i18n('RX byte'), color: "#00ff00", data: buff[ifname+"_rxdata"] },
    ];
    var opt =
    {
        points:{ clickable:true, hoverable:true },
        lines:{ show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: buff[ifname+"_max"], tickFormatter:function(axis) { return axis.toFixed(0)+"K"; } },
        xaxis: { mode: "time", timeformat: "%M:%S", minTickSize: [1, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "[id='"+ifname+"-charts']", datas, opt );
    /* get the usage */
    var usage;
    usage = (rx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_downusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_downusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
    usage = (tx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_upusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_upusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
}

function wisp_show( ifname, value )
{
    $("[id='"+ifname+"']").show();
    $("[id='"+ifname+"_status']").html( $.i18n(value.status) );
    $("[id='"+ifname+"_ip']").html( value.ip||"" );
    if ( value.delay )
    {
        if ( value.delay == "failed" )
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("failed") );
        }
        else
        {
            $("[id='"+ifname+"_delay']").text( $.i18n("Delay")+":"+value.delay );
        }
    }
    else
    {
        $("[id='"+ifname+"_delay']").text( "" );
    }
    $("[id='"+ifname+"_mask']").html( value.mask||"" );
    $("[id='"+ifname+"_gw']").html( value.gw||"" );
    $("[id='"+ifname+"_dns']").html( value.dns||"" );
    $("[id='"+ifname+"_mac']").html( value.mac||"" );
    $("[id='"+ifname+"_peer']").html( value.peer||"" );
    $("[id='"+ifname+"_peermac']").html( value.peermac||"" );
    $("[id='"+ifname+"_channel']").html( value.channel||"" );
    if ( value.status != "up" )
    {
        return;
    }
    $("[id='"+ifname+"_livetime']").html( value.livetime||"" );
    $("[id='"+ifname+"_rx_bytes']").html( byte2readable( value.rx_bytes||"0" ) );
    $("[id='"+ifname+"_rx_packets']").html( value.rx_packets||"0" );
    $("[id='"+ifname+"_tx_bytes']").html( byte2readable( value.tx_bytes||"0" ) );
    $("[id='"+ifname+"_tx_packets']").html( value.tx_packets||"0" );
    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();
    /* get the byte */
    var rx = 0;
    var tx = 0;
    var orx = buff[ ifname+"_rx" ];
    var otx = buff[ ifname+"_tx" ];
    buff[ ifname+"_rx" ] = parseInt( value.rx_bytes );
    buff[ ifname+"_tx" ] = parseInt( value.tx_bytes );
    if ( orx != 0 || otx != 0 )
    {
        rx = buff[ ifname+"_rx" ] - orx;
        tx = buff[ ifname+"_tx" ] - otx;
        if ( rx < 0 )
        {
            rx = 0;
        }
        if ( tx < 0 )
        {
            tx = 0;
        }
    }
    if ( rx > 0 )
    {
        rx = Math.round( rx/1024 );
    }
    if ( tx > 0 )
    {
        tx = Math.round( tx/1024 );
    }
    if ( buff[ ifname+"_max" ] < rx )
    {
        buff[ ifname+"_max" ] = rx;
    }
    if ( buff[ ifname+"_max" ] < tx )
    {
        buff[ ifname+"_max" ] = tx;
    }
    buff[ifname+"_txdata"].push( [ d_time, tx ] );
    buff[ifname+"_rxdata"].push( [ d_time, rx ] );
    $("[id='"+ifname+"-charts']").css( {'width':'100%' , 'height':'294px'} );
    var datas =
    [
        { label: $.i18n('TX byte'), color: "#0000ff", data: buff[ifname+"_txdata"] },
        { label: $.i18n('RX byte'), color: "#00ff00", data: buff[ifname+"_rxdata"] },
    ];
    var opt =
    {
        points:{ clickable:true, hoverable:true },
        lines:{ show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: buff[ifname+"_max"], tickFormatter:function(axis) { return axis.toFixed(0)+"K"; } },
        xaxis: { mode: "time", timeformat: "%M:%S", minTickSize: [1, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "[id='"+ifname+"-charts']", datas, opt );
    /* get the usage */
    var usage;
    usage = (rx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_downusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_downusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
    usage = (tx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_upusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_upusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
}


function lan_show( ifname, value )
{
    $("[id='"+ifname+"']").show();
    $("[id='"+ifname+"_ip']").html( value.ip||"" );
    $("[id='"+ifname+"_mask']").html( value.mask||"" );
    $("[id='"+ifname+"_mac']").html( value.mac||"" );
    if ( value.gw )
    {
        $("[id='"+ifname+"_gw']").html( value.gw );
        $("[id='"+ifname+"_gw_div']").show();
    }
    else
    {
        $("[id='"+ifname+"_gw_div']").hide();
    }
    if ( value.dns )
    {
        $("[id='"+ifname+"_dns']").html( value.dns );
        $("[id='"+ifname+"_dns_div']").show();
    }
    else
    {
        $("[id='"+ifname+"_dns_div']").hide();
    }
    if ( value.status != "up" )
    {
        return;
    }
    $("[id='"+ifname+"_livetime']").html( value.livetime||"" );
    $("[id='"+ifname+"_rx_bytes']").html( byte2readable( value.rx_bytes||"0" ) );
    $("[id='"+ifname+"_rx_packets']").html( value.rx_packets||"0" );
    $("[id='"+ifname+"_tx_bytes']").html( byte2readable( value.tx_bytes||"0" ) );
    $("[id='"+ifname+"_tx_packets']").html( value.tx_packets||"0" );
    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();
    /* get the byte */
    var rx = 0;
    var tx = 0;
    var orx = buff[ ifname+"_rx" ];
    var otx = buff[ ifname+"_tx" ];
    buff[ ifname+"_rx" ] = parseInt( value.rx_bytes );
    buff[ ifname+"_tx" ] = parseInt( value.tx_bytes );
    if ( orx != 0 || otx != 0 )
    {
        rx = buff[ ifname+"_rx" ] - orx;
        tx = buff[ ifname+"_tx" ] - otx;
        if ( rx < 0 )
        {
            rx = 0;
        }
        if ( tx < 0 )
        {
            tx = 0;
        }
    }
    if ( rx > 0 )
    {
        rx = Math.round( rx/1024 );
    }
    if ( tx > 0 )
    {
        tx = Math.round( tx/1024 );
    }
    if ( buff[ ifname+"_max" ] < rx )
    {
        buff[ ifname+"_max" ] = rx;
    }
    if ( buff[ ifname+"_max" ] < tx )
    {
        buff[ ifname+"_max" ] = tx;
    }
    buff[ifname+"_txdata"].push( [ d_time, tx ] );
    buff[ifname+"_rxdata"].push( [ d_time, rx ] );
    $("[id='"+ifname+"-charts']").css( {'width':'100%' , 'height':'110px'} );
    var datas =
    [
        { label: $.i18n('TX byte'), color: "#0000ff", data: buff[ifname+"_txdata"] },
        { label: $.i18n('RX byte'), color: "#00ff00", data: buff[ifname+"_rxdata"] },
    ];
    var opt =
    {
        points:{ clickable:true, hoverable:true },
        lines:{ show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: buff[ifname+"_max"], tickFormatter:function(axis) { return axis.toFixed(0)+"K"; } },
        xaxis: { mode: "time", timeformat: "%M:%S", minTickSize: [1, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "[id='"+ifname+"-charts']", datas, opt );
    /* get the usage */
    var usage;
    usage = (rx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_downusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_downusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
    usage = (tx/(100*1024))*100;
    if ( usage < 0 )
    {
        usage = 0;
    }
    $("[id='"+ifname+"_upusage']").text( usage.toFixed(0) );
    $("[id='"+ifname+"_upusage_pie']").data('easyPieChart').update( usage.toFixed(0) );
}
function ssid_show( ifname, value )
{
    if ( value == null || value.state != "up" )
    {
        $("[id='"+ifname+"_ssid_div']").hide();
        $("[id='"+ifname+"_bssid_div']").hide();
        $("[id='"+ifname+"_channel_div']").hide();
        $("[id='"+ifname+"_speed_div']").hide();
        return;
    }
    $("[id='"+ifname+"_ssid_div']").show();
    $("[id='"+ifname+"_bssid_div']").show();
    $("[id='"+ifname+"_channel_div']").show();
    $("[id='"+ifname+"_speed_div']").show();
    $("[id='"+ifname+"_ssid']").html( value.ssid||"" );
    $("[id='"+ifname+"_bssid']").html( value.bssid||"" );
    $("[id='"+ifname+"_channel']").html( value.channel||"" );
    /* get the time */
    var date = new Date();
    var day = date.getHours();
    date = date.setHours( 8+day );
    var newdate = new Date( date );
    var d_time = newdate.getTime();
    /* get the byte */
    var rx = 0;
    var tx = 0;
    var orx = buff[ ifname+"_rx" ];
    var otx = buff[ ifname+"_tx" ];
    buff[ ifname+"_rx" ] = parseInt( value.rx_bytes );
    buff[ ifname+"_tx" ] = parseInt( value.tx_bytes );
    if ( orx != 0 || otx != 0 )
    {
        rx = buff[ ifname+"_rx" ] - orx;
        tx = buff[ ifname+"_tx" ] - otx;
        if ( rx < 0 )
        {
            rx = 0;
        }
        if ( tx < 0 )
        {
            tx = 0;
        }
    }
    if ( rx > 0 )
    {
        rx = Math.round( rx/1024 );
    }
    if ( tx > 0 )
    {
        tx = Math.round( tx/1024 );
    }
    if ( buff[ ifname+"_max" ] < rx )
    {
        buff[ ifname+"_max" ] = rx;
    }
    if ( buff[ ifname+"_max" ] < tx )
    {
        buff[ ifname+"_max" ] = tx;
    }
    buff[ifname+"_txdata"].push( [ d_time, tx ] );
    buff[ifname+"_rxdata"].push( [ d_time, rx ] );
    $("[id='"+ifname+"-charts']").css( {'width':'100%' , 'height':'110px'} );
    var datas =
    [
        { label: $.i18n('TX byte'), color: "#0000ff", data: buff[ifname+"_txdata"] },
        { label: $.i18n('RX byte'), color: "#00ff00", data: buff[ifname+"_rxdata"] },
    ];
    var opt =
    {
        points:{ clickable:true, hoverable:true },
        lines:{ show:true, lineWidth: 1 },
        selection: { mode: "x" },
        yaxis: { max: buff[ifname+"_max"], tickFormatter:function(axis) { return axis.toFixed(0)+"K"; } },
        xaxis: { mode: "time", timeformat: "%M:%S", minTickSize: [1, "second"] },
        legend: { position: "ne", backgroundColor:"#fff" } 
    };
    $.plot( "[id='"+ifname+"-charts']", datas, opt );
}


/* load the configure on the input */
function dashboard_reload()
{
    var i;
    var id;
    var value;
    he.bkload( [ 'network@frame.extern', 'network@frame.local', 'network@frame.gateway', 'client@station.number', "wifi@nssid.status", "wifi@assid.status" ] ).then( function(v){

        for ( id in v[0] )
        {
            value = v[0][id];
            if ( id.indexOf( "lte" ) >= 0 )
            {
                lte_show( id, value );
            }
            else if ( id.indexOf( "wan" ) >= 0 )
            {
                wan_show( id, value );
            }
            else if ( id.indexOf( "wisp" ) >= 0 )
            {
                wisp_show( id, value );
            }
        }
        for ( id in v[1] )
        {
            value = v[1][id];
            lan_show( id, value );
            if ( v[3] )
            {
                $("[id='"+id+"_client']").html( v[3] );
            }
        }

        ssid_show( "wifi@nssid", v[4] );
        ssid_show( "wifi@assid", v[5] );
    })
}

/* init */
$.i18n().load( page.lang('dashboard') ).then( function () {

    /* init the langauage */
    $.i18n().locale = lang; $('body').i18n();

    /* init the easy-pie-chart */
    $('.easy-pie-chart.percentage').each( function(){
        $(this).easyPieChart( {
            barColor: function( percent )
            {
                percent /= 100;
                return "rgb(" + Math.round(255 * percent) + ", " + Math.round(255 * (1-percent)) + ", 0)";
            },
            trackColor: '#EEEEEE',
            scaleColor: false,
            lineCap: 'butt',
            lineWidth: 8,
            animate: ace.vars['old_ie'] ? false : 1000,
            size:75
        }).css( 'color', $(this).data('color') );
    });

    /* load the configure */
    dashboard_reload();
    /* set the timer flush  */
    page.timing({
        refresh: function ()
        {
            dashboard_reload();
        },
        interval: flush_interval*1000
    });

});


