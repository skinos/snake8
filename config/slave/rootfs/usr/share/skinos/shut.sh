#!/bin/bash
LANDDIR=/usr/share/skinos/land

$LANDDIR/bin/daemon stop

# app
$LANDDIR/bin/he land@uninit.call[general]
$LANDDIR/bin/he land@uninit.call[app2]
$LANDDIR/bin/he land@uninit.call[app]

# manage
$LANDDIR/bin/he land@uninit.call[manage2]
$LANDDIR/bin/he land@uninit.call[manage]

# core
$LANDDIR/bin/he land@uninit.call[core]
$LANDDIR/bin/he land@uninit.call[core2]

# shutdown the log
$LANDDIR/bin/he land@syslog.shut

# shutdown the arch data
$LANDDIR/bin/he arch@data.shut

# skinos daemon exit
$LANDDIR/bin/daemon exit

