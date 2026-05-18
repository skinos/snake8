#!/bin/sh

# ip white list
ipset create okip hash:ip
ipset add okip 192.168.1.200-192.168.1.254
ipset add okip 192.168.1.2
ipset add okip 192.168.1.3
ipset add okip 192.168.1.4
ipset add okip 192.168.1.5
ipset add okip 192.168.1.6
iptables -A FORWARD -i lan -m set ! --match-set okip src -j DROP

