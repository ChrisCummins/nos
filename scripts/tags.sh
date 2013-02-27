#!/bin/bash
# tags.sh - Generate TAGS file.
#           Usage tags.sh <directory ...>

if [ -z "$1" ]
then
    echo "Usage: $(basename $0) <directory ...>" >&2
    exit 1
fi

for d in $@; do
    if [ ! -d "$1" ]
    then
        echo "Directory '$d' not found" >&2
        exit 1
    fi
done

sources=$(find $@ -name '*.[ch]')

etags $sources
