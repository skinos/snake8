#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    # mkdir basic directory
    VROOT=/var/skinos
    TROOT=/tmp/skinos
    LANDDIR=/usr/share/skinos/land
    sudo mkdir -p $TROOT
    sudo mkdir -p $VROOT
    sudo chmod a+rwx $TROOT
    sudo chmod a+rwx $VROOT
    mkdir -p $VROOT/.reg
    mkdir -p $VROOT/.ser
    mkdir -p $VROOT/.com
    mkdir -p $VROOT/.conf
    mkdir -p $VROOT/mnt
    mkdir -p $VROOT/mnt/config
    mkdir -p $VROOT/mnt/internal
    # load the basic ko
    if [ -e /usr/prj/pdriver/crackid.ko ]; then
        insmod /usr/prj/pdriver/crackid.ko
    fi
    # make the register value default
    RAND=`date +%N`
    $LANDDIR/bin/he land@register.set_int[rand,$RAND]
    $LANDDIR/bin/he land@register.set_string[platform,$gPLATFORM]
    $LANDDIR/bin/he land@register.set_string[hardware,$gHARDWARE]
    $LANDDIR/bin/he land@register.set_string[custom,$gCUSTOM]
    $LANDDIR/bin/he land@register.set_string[scope,$gSCOPE]
    $LANDDIR/bin/he land@register.set_string[version,$gVERSION]
    NETDEV=`$LANDDIR/bin/he arch@data:local_netdev`
    if [ "X${NETDEV}" != "X" ]; then
        $LANDDIR/bin/he land@register.set_string[local_ifname,ifname@lan]
        $LANDDIR/bin/he land@register.set_string[local_netdev,$NETDEV]
    fi
    MODEL=`$LANDDIR/bin/he arch@data:model`
    $LANDDIR/bin/he land@register.set_string[model,$MODEL]
    MAC=`$LANDDIR/bin/he arch@data:mac`
    $LANDDIR/bin/he land@register.set_string[mac,$MAC]
    # default the configure if order
    if [ -e $VROOT/mnt/config/.customv6 ]; then
    	echo "mount the configure"
    else
    	echo "clear the configure"
        rm -fr $VROOT/mnt/config/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $VROOT/mnt/config/.customv6
    fi
    if [ -e $VROOT/mnt/internal/.customv6 ]; then
    	echo "mount the interval"
	else
    	echo "clear the interval"
		rm -fr $VROOT/mnt/internal/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $VROOT/mnt/internal/.customv6
    fi

    NAME=`hostname`
    $LANDDIR/bin/he land@machine:name=$NAME
    
    creturn ttrue
}

shut()
{
    sync
    creturn ttrue
}

# call the method, you cannot delete this
cend

