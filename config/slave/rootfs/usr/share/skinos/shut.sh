#!/bin/bash
LANDDIR=/usr/share/skinos/land

$LANDDIR/bin/daemon stop15exit >/dev/null 2>&1

# delay
$LANDDIR/bin/he land@uninit.call[delay5] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[delay4] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[delay3] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[delay3] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[delay] >/dev/null 2>&1
# app
$LANDDIR/bin/he land@uninit.call[general] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[app2] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[app] >/dev/null 2>&1

# internet
#$LANDDIR/bin/he land@uninit.call[extern] >/dev/null 2>&1
# local
#$LANDDIR/bin/he land@uninit.call[local] >/dev/null 2>&1
# manage
#$LANDDIR/bin/he land@uninit.call[manage] >/dev/null 2>&1

# network
#$LANDDIR/bin/he land@uninit.call[network] >/dev/null 2>&1
# device
#$LANDDIR/bin/he land@uninit.call[device] >/dev/null 2>&1
# bus
#$LANDDIR/bin/he land@uninit.call[bus] >/dev/null 2>&1

# network frame
$LANDDIR/bin/he network@frame.shut >/dev/null 2>&1
# core
$LANDDIR/bin/he land@uninit.call[land] >/dev/null 2>&1
$LANDDIR/bin/he land@uninit.call[arch] >/dev/null 2>&1
# shutdown the arch data
$LANDDIR/bin/he arch@data.shut >/dev/null 2>&1

# skinos daemon exit
$LANDDIR/bin/daemon exit >/dev/null 2>&1
exit 0

