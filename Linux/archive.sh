# Archive files
#

	echo "Syncing Document Folder in 5 sec..."
	sleep 5
#rsync -av --delete-after --rsync-path=/bin/rsync /home/ad752h/ad752h/Documents root@10.0.1.166:/volume2/Backups/ATT 

##Home 
	rsync -av --delete-after --rsync-path=/bin/rsync  --exclude={'AppData','.cisco','Roaming','Searches','ntuser.dat.*','NTUSER.DAT','Documents/afterSentDocuments'} /home/ad752h/ad752h/ root@10.0.1.166:/volume2/Backups/ATT 
if [ "$?" -eq 0 ]
then
        echo `date +%F`": rsync for Home Directory was successful" >>~/Logs/backup.log
else
        echo `date +%F`": rsync for $sdwn did not complete" >>~/Logs/backup.log
fi

##cygwin 
	rsync -av --delete-after --rsync-path=/bin/rsync /home/ad752h root@10.0.1.166:/volume2/Backups/ATT/Cygwin_Backup 
if [ "$?" -eq 0 ]
then
        echo `date +%F`": rsync for cygwin Directory was successful" >>~/Logs/backup.log
else
        echo `date +%F`": rsync for cygwin did not complete" >>~/Logs/backup.log
fi

##Downloads
	rsync -av /home/ad752h/ad752h/Downloads --rsync-path=/bin/rsync root@10.0.1.166:/volume2/Backups/
	echo $?
if [ "$?" -eq 0 ]
then
        echo `date +%F`": rsync for Downloads Directory was successful" >>~/Logs/backup.log
else
        echo `date +%F`": rsync for Downloads  did not complete" >>~/Logs/backup.log
fi

