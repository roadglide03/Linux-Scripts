#!/bin/bash
#
# This script is for logging into cyclades
#
	echo -e "Enter Cyclades Port #: \c "
	read port
	echo -e "Enter Cyclades IP: \c "
	read ipaddr
	echo ""
	echo "Attempting Login: ssh admin:70$port@$ipaddr ..."
	ssh admin:70$port@$ipaddr
	
