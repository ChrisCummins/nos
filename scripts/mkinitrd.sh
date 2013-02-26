#!/bin/bash
# mkinitrd.sh - run with ' --help' for usage information.

INITRD=initrd
OUTPUT_DIR=floppy
INPUT_DIR=initrd
INITRDGEN=./initrd-gen/initrd-gen

usage () {
    echo "Usage: $(basename $0) [--help]"
    echo ""
    echo "Generates an initrd file by composing the contents of the input"
    echo "directory into a ramdisk image."
    echo ""
    echo "    Input directory: '$INPUT_DIR/'"
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

input_files="$(find $INPUT_DIR -type f)"

if [ -z "$input_files" ]; then
    echo "$(basename $0): no files found in '$INPUT_DIR'!" >&2
    exit 1
fi

rm -f .mkinitrd_tmp
for f in $input_files; do
    file="$(basename $f)"
    if [ "$file" != ".gitignore" ] && [ "$file" != "Makefile" ]; then
        echo -n "$f $(basename $f) " >> .mkinitrd_tmp
    fi
done

input_files="$(cat .mkinitrd_tmp)"
rm -f .mkinitrd_tmp

echo ""
sudo $INITRDGEN $OUTPUT_DIR/$INITRD $input_files

sudo chown root $OUTPUT_DIR/$INITRD
sudo chgrp root $OUTPUT_DIR/$INITRD
echo "$(basename $0): generated '$INITRD'"
