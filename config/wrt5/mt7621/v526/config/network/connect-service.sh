#!/bin/sh

# create bad route
ip route add default via 192.168.1.254 table 100
# work right
ip rule add from 192.168.1.3 table 1 pref 32800
ip rule add from 192.168.1.4 table 2 pref 32800
ip rule add from 192.168.1.5 table 9 pref 32800
ip rule add from 192.168.1.6 table 10 pref 32800
# work bad
ip rule add from 192.168.1.3 table 100 pref 32801
ip rule add from 192.168.1.4 table 100 pref 32801
ip rule add from 192.168.1.5 table 100 pref 32801
ip rule add from 192.168.1.6 table 100 pref 32801

