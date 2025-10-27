# mount_iso.sh
#
mnt='/Users/andy/mnt'



if [ ! -d $mnt ]; then
 echo "$mnt is not Present, Creating Directory: $mnt "
 mkdir ~/mnt
 
else
 echo -e "Enter directory to be created for mount point: $mnt/? : \c "
 read dir
 sudo mkdir $mnt/$dir
 sudo ls /Volumes/Kickstart/*
 echo ""
 echo -e "Enter ISO to mount from above or enter new name: \c"
 read image
 sudo mount -o loop /Volumes/Kickstart/ISO/$image $mnt/$dir
 df /$mnt/$dir

exit 0

fi
