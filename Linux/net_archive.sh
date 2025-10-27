# Archive files
#

	echo "Syncing Document Folder in 5 sec..."
	sleep 5
	rsync -av --delete-after --rsync-path=/bin/rsync /home/ad752h/ad752h/Documents/ adishong@10.0.1.166:/volume2/Backups/ATT

	rsync -av --rsync-path=/bin/rsync /home/ad752h/ad752h/Downloads adishong@10.0.1.166:/volume2/Backups/ATT
	echo $?

if [ "$?" -ne 0 ]
then
	echo "rsync for $sdwn did not complete"
fi

	ls /home/ad752h/ad752h/Downloads/*
	echo $?
if [  "$?" -ne 0 ]
then
	echo "No files in Download Folder"
	exit 1
else
	echo "Removing Downloads"
	rm -r /home/ad752h/ad752h/Downloads/*

fi
