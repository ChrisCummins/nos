#!/bin/bash
# mkinitrd.sh - run with ' --help' for usage information.

INITRD=initrd.img
OUTPUT_DIR=boot
INPUT_DIR=boot/initrd
BACKUP_SUFFIX=.old

usage () {
    echo "Usage: $(basename $0)"
    echo ""
    echo "Generates an initrd file by composing the contents of the input"
    echo "directory into a ramdisk image. If an initrd image already exists"
    echo "at that location then a backup copy will be made with '$BACKUP_SUFFIX'"
    echo "appended to the filename."
    echo ""
    echo "    Input directory: '$INPUT_DIR'"
    echo "    Image:           '$OUTPUT_DIR/$INITRD'"
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

if [ ! -d "$INPUT_DIR" ]; then
    echo "$(basename $0): input directory '$INPUT_DIR' does not exist! initrd creation failed." >&2
    exit 1
fi

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir "$OUTPUT_DIR"
fi

# Backup existing initrd.
if [ -f "$OUTPUT_DIR/$INITRD" ]; then
    mv -f "$OUTPUT_DIR/$INITRD" "$OUTPUT_DIR/$INITRD$BACKUP_SUFFIX"
    echo "$(basename $0): '$INITRD'->'$INITRD$BACKUP_SUFFIX'"
fi

input_files="$(find $INPUT_DIR -type f)"

if [ -z "$input_files" ]; then
    echo "$(basename $0): no files found in '$INPUT_DIR'!" >&2
    exit 1
fi

rm -f .mkinitrd_tmp
for f in $input_files; do
    echo -n "$f $(basename $f) " >> .mkinitrd_tmp
done

input_files="$(cat .mkinitrd_tmp)"
rm -f .mkinitrd_tmp

echo ""
./initrd/initrd-gen $OUTPUT_DIR/$INITRD $input_files
