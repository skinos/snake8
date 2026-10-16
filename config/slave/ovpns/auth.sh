#!/bin/sh
out=`he "ovpns@server.verify[$1]" 2>/dev/null`
echo "$out" | grep ttrue >/dev/null 2>&1
