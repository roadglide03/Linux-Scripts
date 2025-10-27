#!/bin/bash
filename='nfs.o'
exec 4<$filename

while read -u4 p ; do
    showmount -e $p
done
