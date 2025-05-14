#!/bin/bash
LANDDIR=/usr/share/skinos/land

$LANDDIR/bin/daemon stop

# app
$LANDDIR/bin/he land@uninit.call[general]
$LANDDIR/bin/he land@uninit.call[app2]
$LANDDIR/bin/he land@uninit.call[app]

# manage
$LANDDIR/bin/he land@uninit.call[manage]

# device
$LANDDIR/bin/he land@uninit.call[device]
# bus
$LANDDIR/bin/he land@uninit.call[bus]

# core
$LANDDIR/bin/he land@uninit.call[land]
$LANDDIR/bin/he land@uninit.call[arch]

# shutdown the arch data
$LANDDIR/bin/he arch@data.shut

# skinos daemon exit
$LANDDIR/bin/daemon exit

