#!/bin/bash
# simulate.sh - create and then execute an image file inside an emulator.
#               Usage: simulate.sh

set -e

# Update the image archive.
sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 /mnt
sudo cp nos/kernel /mnt/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0

# Run the simulator.
sudo /sbin/losetup /dev/loop0 floppy.img
set +e
sudo bochs -f bochsrc.txt -q
set -e
sudo /sbin/losetup -d /dev/loop0
