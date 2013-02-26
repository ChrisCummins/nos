#!/bin/bash
# mkinitrd.sh - run with ' --help' for usage information.

DIR=floppy
IMAGE=floppy.img
DEV=/dev/loop0
MOUNTPOINT=loop0

usage() {
    echo "Usage: mkfloppy [--help]"
    echo ""
    echo "Automatically generate a bootable image from an input directory."
    echo ""
    echo "    Directory: '$DIR/'"
    echo "    Image:     'floppy.img'"
}

mount() {
    sudo mkdir -p "$MOUNTPOINT"
    set +e
    sudo losetup -d "$DEV" &>/dev/null
    sudo losetup "$DEV" "$IMAGE"
    sudo mount "$DEV" "$MOUNTPOINT"
    set -e
}

umount() {
    set +e
    sudo umount $DEV
    sudo losetup -d $DEV
    set -e
    sudo rmdir "$MOUNTPOINT"
}

set -e

# Enable debugging if needed.
test -n "$DEBUG" && set -x

# Parse --help argument first.
for arg in $@; do
    if [ $arg = "--help" ]; then
        usage
        exit 0
    fi
done

case $1 in
    "mount")
        mount
        exit 0
        ;;
    "umount")
        umount
        exit 0
        ;;
esac

if [ ! -f "$IMAGE" ]; then
    echo "$(basename $0): image file '$IMAGE' not found!" >&2
    exit 1
fi

if [ ! -d "$DIR" ]; then
    echo "$(basename $0): input directory '$DIR' does not exist!" >&2
    exit 1
fi

set -u
mount
sudo rsync -avh --exclude 'lost+found' --exclude 'Makefile' --exclude '.gitignore' "$DIR/" "$MOUNTPOINT/"
umount
echo "$(basename $0): generated '$IMAGE'"
