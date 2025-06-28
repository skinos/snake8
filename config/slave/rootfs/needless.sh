#!/bin/bash

# delete ifname
rm -fr ./usr/share/skinos/ifname
# delete network
rm -fr ./usr/share/skinos/network
# delete clock
rm -fr ./usr/share/skinos/clock
# delete execute bin for compatibility tiger7
rm -fr ./usr/local/bin/daemon
rm -fr ./usr/local/bin/he

# delete self
rm -fr ./needless.sh


