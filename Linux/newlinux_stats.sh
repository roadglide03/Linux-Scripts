#!/bin/bash
# Author(s)        : Andrew Dishong
# Email            : adishong@forsythe.com

if [ ! -d /tmp/statlog ]

then
echo "Creating /tmp/statlog"
mkdir /tmp/statlog
else
echo "/tmp/statlog Exists!"
fi

#integer j=1
j=1

a=`date | awk '{print $2}'`
b=`date | awk '{print $3}'`
c=`date | awk '{print $4}'`

outdir=${a}_${b}_${c}
logdir=/var/tmp/statlog/`hostname`_$outdir
mkdir -p $logdir

(
cd $logdir
uname -n >$logdir/host.out/net

lshw >$logdir/lshw.out
nfsstat -m >$logdir/nfsstat_m.out


while [[ j -lt 50 ]]
do
echo "Running Stats"

if [ ! -d  run${j} ]
then
mkdir run${j}
fi
(
cd run${j}
date > $logdir/ps.before
ps -eaflLPc >> $logdir/ps.before

date > $logdir/net.before

mpstat 1 20 >> $logdir/mpstat.out &
iostat -xcn 1 20 >> $logdir/iostat.out &
vmstat 1 20 >> $logdir/vmstat.out &
sar -d 1 20  >> $logdir/sar.out &
nfsstat -c  >> $logdir/nfs.out &
netstat  >> $logdir/netstat.out &
top  -l20>> $logdir/top.out &
lsof >> $logdir/lsof.out &




sleep 5
wait

date > $logdir/ps.after
ps -eaflLPc >> $logdir/ps.after

date > $logdir/net.after
)
j=$(( j + 1))
done
)
echo ""
echo " Compressing Logs"
cd /tmp
tar -cvf - /tmp/statlog >/var/tmp/`hostname`_statlog.tar
gzip /var/tmp/`hostname`_statlog.tar
echo "FTP or Email /var/tmp/statlog.tar.gz"
echo " Logs can be found in $logdir"
echo ""

