!#/bin/bash

uname -a
uname -r
cat /etc/redhat-release
tail -f /var/log/messages
dmesg | grep -i 'err|warn|cri'
ss -tulpn
