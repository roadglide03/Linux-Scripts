#dpinger form udmpro
#
/usr/bin/dpinger -f -i eth8-mon8-10.0.1.1 -B 192.168.1.80 -I eth8 -m 0x001c0000 -s 60s -d 1 -t 86400s -u /run/eth8-mon8-10.0.1.1.sock 10.0.1.1
/usr/bin/dpinger -f -i eth8-mon9-10.0.1.1 -B 192.168.1.80 -I eth8 -m 0x001c0000 -s 3s -d 1 -t 60s -u /run/eth8-mon9-10.0.1.1.sock -L 50 -C /usr/bin/wan-failover-script 10.0.1.1
/usr/bin/dpinger -f -i eth8-mon14-1.1.1.1 -B 192.168.1.80 -I eth8 -m 0x001c0000 -s 10s -d 1 -t 30s -u /run/eth8-mon14-1.1.1.1.sock 1.1.1.1
/usr/bin/dpinger -f -i eth9-mon1-10.0.1.1 -B 70.15.5.99 -I eth9 -m 0x001a0000 -s 60s -d 1 -t 86400s -u /run/eth9-mon1-10.0.1.1.sock 10.0.1.1
/usr/bin/dpinger -f -i eth9-mon2-10.0.1.1 -B 70.15.5.99 -I eth9 -m 0x001a0000 -s 3s -d 1 -t 60s -u /run/eth9-mon2-10.0.1.1.sock -L 50 -C /usr/bin/wan-failover-script 10.0.1.1
/usr/bin/dpinger -f -i eth9-mon7-1.1.1.1 -B 70.15.5.99 -I eth9 -m 0x001a0000 -s 10s -d 1 -t 30s -u /run/eth9-mon7-1.1.1.1.sock 1.1.1.1
/usr/bin/dpinger -f -i eth9-mon5-23.200.197.152-www.microsoft.com -B 70.15.5.99 -I eth9 -m 0x001a0000 -s 10s -d 1 -t 30s -u /run/eth9-mon5-23.200.197.152-www.microsoft.com.sock 23.200.197.152
/usr/bin/dpinger -f -i eth8-mon13-142.250.64.238-google.com -B 192.168.1.80 -I eth8 -m 0x001c0000 -s 10s -d 1 -t 30s -u /run/eth8-mon13-142.250.64.238-google.com.sock 142.250.64.238
/usr/bin/dpinger -f -i eth9-mon6-142.250.81.238-google.com -B 70.15.5.99 -I eth9 -m 0x001a0000 -s 10s -d 1 -t 30s -u /run/eth9-mon6-142.250.81.238-google.com.sock 142.250.81.238
/usr/bin/dpinger -f -i eth8-mon12-184.25.165.167-www.microsoft.com -B 192.168.1.80 -I eth8 -m 0x001c0000 -s 10s -d 1 -t 30s -u /run/eth8-mon12-184.25.165.167-www.microsoft.com.sock 184.25.165.167
