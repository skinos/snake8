#!/bin/sh
f=/tmp/ovpns/note.$$
mkdir -p /tmp/ovpns
printf '%s\n%s\n%s:%s\n%s\n' connect "$common_name" "$untrusted_ip" "$untrusted_port" "$tls_serial_0" > "$f"
he "ovpns@server.note[$f]" >/dev/null 2>&1 || true
rm -f "$f"
exit 0
