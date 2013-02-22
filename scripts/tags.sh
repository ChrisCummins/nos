#!/bin/bash
# tags.sh - Generate TAGS file.
#           Usage tags.sh <srcdir>

# Be very verbose.
set -x

if [ -z "$1" ]
then
    echo "Usage: $(basename $0) <srcdir>" >&2
    exit 1
fi

srcdir="$1"

if [ ! -d "$1" ]
then
    echo "Directory '$1' not found" >&2
    exit 1
fi

sources=$(find "$srcdir" -name '*.[ch]')

etags $sources
