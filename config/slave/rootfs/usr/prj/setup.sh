#!/bin/bash

# modify the maximum number of open file descriptors
ulimit -n 65535

# init the dir and general register
he arch@data.setup
# fpk register the project
he land@machine.setup
# start the log first
he land@syslog.setup

# skinos daemon
daemon&

# init the date
#he clock@date.setup
# init the hotplug
#he arch@hotplug.setup
# init the gpio and led
#he arch@gpio.setup

# core
he land@init.call[core]
he land@init.call[core2]

# network frame
#he network@frame.setup
#he land@init.call[network]
#he land@init.call[vlan]
#he land@init.call[bridge]
# local preset
#he land@init.call[local]
# setup modem
#he land@init.call[modem]
# ethernet
#he land@init.call[ethernet]
# wifi
#sleep 1
#he land@init.call[wifi]
#he land@init.call[nradio]
#he land@init.call[aradio]
# internet
#he land@init.call[extern]

# manage
he land@init.call[manage]
he land@init.call[manage2]

# app
he land@init.call[app]
he land@init.call[app2]
he land@init.call[general]

