#!/bin/bash
#
# Find and delete files
#
#BASH_SOURCE='sudo find $dir -name $file -exec rm -rf {} \;'

	 echo -e "Enter absoulute path for directory to be scanned: \c"
	read dir
	 echo -e "Enter search to be deleted: \c"
	read file
	 echo ""
	 echo -e "Searching for $dir and $file ... "
	 echo ""	
	read -p "Do you want to continue (y/n)?" REPLY
	if [ "$REPLY" = "y" ]; then
  	sudo find $dir -name "$file" -exec rm -rfv {} \; 
	else
  	 echo "Quiting";
fi
	
