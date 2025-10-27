#!/bin/bash
count=1
while [ $count -gt 0 ]
do
 	echo -ne "$count \c"  ; sleep 60
	clear
	count=`expr $count + 1`
done


