#!/bin/bash

# delete clock
rm -fr ./usr/share/skinos/clock
# delete execute bin for compatibility tiger7
#rm -fr ./usr/local/bin/daemon
#rm -fr ./usr/local/bin/he
# delete local
rm -fr ./usr/share/skinos/agent/local.cfg
# delete portc
rm -fr ./usr/share/skinos/agent/portc.com

# delete self
rm -fr ./needless.sh


