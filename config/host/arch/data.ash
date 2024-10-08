#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    # mkdir basic directory
    VROOT=/var/skin
    TROOT=/tmp/skin
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
    he land@register.set_int[rand,$RAND]
    he land@register.set_string[platform,$gPLATFORM]
    he land@register.set_string[hardware,$gHARDWARE]
    he land@register.set_string[custom,$gCUSTOM]
    he land@register.set_string[scope,$gSCOPE]
    he land@register.set_string[version,$gVERSION]
    NETDEV=`he arch@data:local_netdev`
    if [ "X${NETDEV}" != "X" ]; then
        he land@register.set_string[local_ifname,ifname@lan]
        he land@register.set_string[local_netdev,$NETDEV]
    fi
    MODEL=`he arch@data:model`
    he land@register.set_string[model,$MODEL]
    MAC=`he arch@data:mac`
    he land@register.set_string[mac,$MAC]
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
    he land@machine:name=$NAME
    
    creturn ttrue
}

shut()
{
    sync
    creturn ttrue
}

# call the method, you cannot delete this
cend

