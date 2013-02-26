#!/bin/bash
# bochs.sh - run with ' --help' for usage information.

DEV=/dev/loop0
IMAGE=floppy.img
CONF=bochs/bochsrc.txt
LOG=bochs/bochs.log

usage () {
    echo "Usage: $(basename $0)"
    echo ""
    echo "Begins a bochs x86 emulator IA-32 session with the following configuration:"
    echo ""
    echo "    image:   '$IMAGE'"
    echo "    bochsrc: '$CONF'"
    echo "    log:     '$LOG'"
}

# Enable debugging if needed.
test -n "$DEBUG" && set -x

# Parse --help argument first.
for arg in $@; do
    if [ $arg = "--help" ]; then
        usage
        exit 0
    fi
done

if [ ! -f "$IMAGE" ]; then
    echo "$(basename $0): file does not exist '$IMAGE'" >&2
    exit 1
fi

if [ ! -f "$CONF" ]; then
    echo "$(basename $0): file does not exist '$CONF'" >&2
    exit 1
fi

sudo losetup -d $DEV &>/dev/null
sudo losetup $DEV "$IMAGE"
sudo bochs -qf "$CONF" -log "$LOG"
sudo losetup -d $DEV
echo "Session log: '$LOG'"
