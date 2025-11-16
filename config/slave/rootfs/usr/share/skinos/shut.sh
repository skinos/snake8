#!/bin/bash
LANDDIR=/usr/share/skinos/land

$LANDDIR/bin/daemon stop15exit

# delay
$LANDDIR/bin/he land@uninit.call[delay5]
$LANDDIR/bin/he land@uninit.call[delay4]
$LANDDIR/bin/he land@uninit.call[delay3]
$LANDDIR/bin/he land@uninit.call[delay3]
$LANDDIR/bin/he land@uninit.call[delay]
# app
$LANDDIR/bin/he land@uninit.call[general]
$LANDDIR/bin/he land@uninit.call[app2]
$LANDDIR/bin/he land@uninit.call[app]

# internet
#$LANDDIR/bin/he land@uninit.call[extern]
# local
#$LANDDIR/bin/he land@uninit.call[local]
# manage
#$LANDDIR/bin/he land@uninit.call[manage]

# network
#$LANDDIR/bin/he land@uninit.call[network]
# device
#$LANDDIR/bin/he land@uninit.call[device]
# bus
#$LANDDIR/bin/he land@uninit.call[bus]

# network frame
$LANDDIR/bin/he network@frame.shut
# core
$LANDDIR/bin/he land@uninit.call[land]
$LANDDIR/bin/he land@uninit.call[arch]
# shutdown the arch data
$LANDDIR/bin/he arch@data.shut

# skinos daemon exit
$LANDDIR/bin/daemon exit

