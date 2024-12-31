#!/bin/bash
LANDDIR=/usr/share/skinos/land

# modify the maximum number of open file descriptors
ulimit -n 65535

# init the dir and general register
$LANDDIR/bin/he arch@data.setup
# fpk register the project
$LANDDIR/bin/he land@machine.setup
# start the log first
$LANDDIR/bin/he land@syslog.setup

# skinos daemon
$LANDDIR/bin/daemon&

# core
$LANDDIR/bin/he land@init.call[core]
$LANDDIR/bin/he land@init.call[core2]

# manage
$LANDDIR/bin/he land@init.call[manage]
$LANDDIR/bin/he land@init.call[manage2]

# app
$LANDDIR/bin/he land@init.call[app]
$LANDDIR/bin/he land@init.call[app2]
$LANDDIR/bin/he land@init.call[general]

