#!/bin/bash
LANDDIR=/usr/share/skinos/land

# modify the maximum number of open file descriptors
ulimit -n 65535

# init the dir and general register
$LANDDIR/bin/he arch@data.setup
# fpk register the project
$LANDDIR/bin/he land@machine.setup

# skinos daemon
$LANDDIR/bin/daemon

# core
$LANDDIR/bin/he land@init.call[arch]
$LANDDIR/bin/he land@init.call[land]
# network frame
$LANDDIR/bin/he network@frame.setup

# bus
#$LANDDIR/bin/he land@init.call[bus]
# device
#$LANDDIR/bin/he land@init.call[device]
# network
#$LANDDIR/bin/he land@init.call[network]

# manage
#$LANDDIR/bin/he land@init.call[manage]
# local
#$LANDDIR/bin/he land@init.call[local]
# internet
#$LANDDIR/bin/he land@init.call[extern]

# app
$LANDDIR/bin/he land@init.call[app]
$LANDDIR/bin/he land@init.call[app2]
$LANDDIR/bin/he land@init.call[general]

# delay
sleep 1
$LANDDIR/bin/he land@init.call[delay]
#sleep 1
#$LANDDIR/bin/he land@init.call[delay2]
#sleep 1
#$LANDDIR/bin/he land@init.call[delay3]
#sleep 1
#$LANDDIR/bin/he land@init.call[delay4]
#sleep 1
#$LANDDIR/bin/he land@init.call[delay5]

