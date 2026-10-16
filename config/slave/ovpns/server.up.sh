#!/bin/sh
dns=
dns2=
gw="${route_vpn_gateway}"
if [ "X$gw" = "X" ]; then
    gw="${ifconfig_remote}"
fi
ONLINEINFO="{\"netdev\":\"${dev}\",\"ifname\":\"${object}\",\"ifnametype\":\"vpn\",\"mtu\":\"${tun_mtu}\",\"ip\":\"${ifconfig_local}\",\"mask\":\"${ifconfig_netmask}\",\"gw\":\"${gw}\",\"dns\":\"${dns}\",\"dns2\":\"${dns2}\"}"
logger "network@frame.online[${ONLINEINFO}]"
he "network@frame.online[${ONLINEINFO}]"
exit 0
