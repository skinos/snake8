#!/bin/bash

# delete the needless init
lookdir="./etc/init.d"
exception="boot sysctl done preland land"
for file in `ls ${lookdir}`
do
    save=0
    for ext in ${exception}
    do
        if [ ${file} == ${ext} ]
        then
            save=1
        fi
    done
    if [ $save != 1 ]
    then
        rm -f ${lookdir}/${file}
    fi
done


# ln the var mnt to /tmp
rm -fr ./mnt
ln -s /tmp/mnt ./mnt
rm -fr ./var
ln -s /tmp/var ./var
# mv /lib/firmware to /usr/share/firmware make /lib/firmwre can write
rm -fr ./usr/share/firmware
mv ./lib/firmware .//usr/share/
ln -s /tmp/firmware .//lib/firmware

# delete 
rm -fr ./etc/apk
rm -fr ./etc/board.d
rm -fr ./etc/hotplug.d
rm -fr ./etc/openwrt_release
rm -fr ./etc/openwrt_version
rm -fr ./etc/preinit
rm -fr ./etc/rc.common
rm -fr ./etc/rc.d
rm -fr ./etc/rc.local
rm -fr ./etc/ssl
rm -fr ./etc/sysupgrade.conf
#rm -fr ./etc/capabilities
rm -fr ./etc/crontabs
rm -fr ./etc/iproute2
rm -fr ./etc/profile.d
rm -fr ./etc/rc.button
rm -fr ./etc/rc.wps
rm -fr ./etc/uci-defaults
rm -fr ./rom
rm -fr ./www
rm -fr ./overlay
rm -fr ./sbin/firstboot
rm -fr ./sbin/fixup-mac-address
rm -fr ./sbin/ubusd
rm -fr ./sbin/validate_data
rm -fr ./sbin/wifi
rm -fr ./bin/board_detect
rm -fr ./bin/ipcalc.sh
rm -fr ./bin/ubus
#rm -fr ./lib/libanl.so.1
#rm -fr ./lib/libuci.so.20250120
#rm -fr ./lib/libatomic.so.1.2.0
#rm -fr ./lib/libubus.so.20251202
rm -fr ./lib/apk
rm -fr ./lib/upgrade
rm -fr ./lib/preinit
rm -fr ./lib/netifd
#rm -fr ./lib/functions*
rm -fr ./usr/sbin/ntpd-hotplug
rm -fr ./usr/sbin/ubiattach
rm -fr ./usr/sbin/ubiblock
rm -fr ./usr/sbin/ubicrc32
rm -fr ./usr/sbin/ubidetach
rm -fr ./usr/sbin/ubiformat
rm -fr ./usr/sbin/ubihealthd
rm -fr ./usr/sbin/ubimkvol
rm -fr ./usr/sbin/ubinfo
rm -fr ./usr/sbin/ubinize
rm -fr ./usr/sbin/ubirename
rm -fr ./usr/sbin/ubirmvol
rm -fr ./usr/sbin/ubirsvol
rm -fr ./usr/sbin/ubiupdatevol
rm -fr ./usr/bin/ucode
rm -fr ./usr/bin/wireguard_watchdog
rm -fr ./usr//bin/jshn
rm -fr ./usr/bin/jsonfilter
rm -fr ./usr/lib/dnsmasq
rm -fr ./usr/lib/engines-3
rm -fr ./usr/lib/ucode
rm -fr ./usr/lib/ossl-modules
#rm -fr ./usr/lib/libucode.so.20230711
#rm -fr ./usr/lib/libudebug.so
#rm -fr ./usr/lib/libnghttp2.so.14.28.5
#rm -fr ./usr/lib/libmenuw.so.6.4
#rm -fr ./usr/lib/libformw.so.6.4
#rm -fr ./usr/lib/libcares.so.2.19.5
rm -fr ./usr/share/acl.d
rm -fr ./usr/share/ucode
rm -fr ./usr/share/dnsmasq


# delete self
rm -fr ./needless.sh

