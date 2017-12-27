#!/bin/bash -x

device="jjl"

# invoke rmmod with all arguments we got
/sbin/rmmod "jjl_driver" $* || exit 1

# remove device files
rm -f /dev/${device}
