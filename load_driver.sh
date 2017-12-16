#!/bin/bash -x

# load_driver.sh:
#   Because our device driver has its device numbers _dynamically_ allocated
#   via alloc_chrdev_region (rather than _statically_ via register_chrdev_region)
#   we must use a bash script find the device number
#   at runtime and create our device file inside /dev/

device="jjl"

# invoke insmod to insert our dynamically insert module into the kernel
/sbin/insmod ./jjl_driver.ko $* || exit 1

# retrieve major number
major=$(awk "\$2==\"$device\" {print \$1}" /proc/devices)

# remove any stale files in case unload_driver.sh was not correctly called
# then create a fresh file
dev_file="/dev/${device}"
rm -f $dev_file
mknod $dev_file c $major 0

# give appropriate permissions/ownership to the newly created device file
# everyone needs read access. for all linux special files, the owner
# should be "root", give root exclusive write access
# not all linux distros have the "staff" group, some use "wheel" instead
# check if staff is a defined group in the list of all groups (/etc/group)
# if its not there assume we're using wheel instead
group="staff"
grep -q '^staff:' /etc/group || group="wheel"
chgrp $group $dev_file
chmod 664 $dev_file
