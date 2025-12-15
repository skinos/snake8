#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    # set ip forward
    sudo sysctl -w net.ipv4.ip_forward=1
    
    # mkdir basic directory
    TROOT=/tmp/skinos
    VROOT=/var/skinos
    MROOT=/mnt/skinos
    PROJECT_REG_DIR=$TROOT/.reg
    PROJECT_SER_DIR=$TROOT/.ser
    PROJECT_COM_DIR=$TROOT/.com
    PROJECT_CAH_DIR=$TROOT/.cah
    PROJECT_CONF_DIR=$TROOT/.conf
    PROJECT_CFG_DIR=$MROOT/config
    PROJECT_DBS_DIR=$MROOT/dbs
    PROJECT_OEM_DIR=$MROOT/.oem
    PROJECT_INT_DIR=$MROOT/internal
    PROJECT_APP_DIR=$PROJECT_INT_DIR/skinos
    LANDDIR=/usr/share/skinos/land

    sudo mkdir -p $TROOT
    sudo chmod a+rwx $TROOT
    mkdir -p $PROJECT_REG_DIR
    mkdir -p $PROJECT_SER_DIR
    mkdir -p $PROJECT_COM_DIR
    mkdir -p $PROJECT_CAH_DIR
    mkdir -p $PROJECT_CONF_DIR

    sudo mkdir -p $VROOT
    sudo chmod a+rwx $VROOT

    sudo mkdir -p $MROOT
    sudo chmod a+rwx $MROOT
    mkdir -p $PROJECT_CFG_DIR
    mkdir -p $PROJECT_DBS_DIR
    mkdir -p $PROJECT_OEM_DIR
    mkdir -p $PROJECT_INT_DIR

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
    MODEL=`$LANDDIR/bin/he arch@data:cmodel`
    $LANDDIR/bin/he land@register.set_string[cmodel,$MODEL]
    MAC=`$LANDDIR/bin/he arch@data:mac`
    $LANDDIR/bin/he land@register.set_string[mac,$MAC]
    # default the configure if order
    if [ -e $PROJECT_CFG_DIR/.customv8 ]; then
    	echo "mount the configure"
    else
    	echo "clear the configure"
        rm -fr $PROJECT_CFG_DIR/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $PROJECT_CFG_DIR/.customv8
    fi
    if [ -e $PROJECT_INT_DIR/.customv8 ]; then
    	echo "mount the interval"
	else
    	echo "clear the interval"
		rm -fr $PROJECT_INT_DIR/*
        echo "$gPLATFORM-$gHARDWARE-$gCUSTOM-$gSCOPE" > $PROJECT_INT_DIR/.customv8
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

