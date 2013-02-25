#!/bin/bash
# todo.sh - display all 'TODO:' and 'FIXME:' tags in source directories.
#           Usage: todo.sh <directory ...>

if [ -n "$DEBUG" ]; then
    set -x
fi

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

for s in $(find $@ -name '*.[ch]'); do
    grep -iHn 'TODO\|FIXME' $s | sed 's/\/\*//' | sed 's/\*\///' | \
        sed -r 's/([0-9]+):\s*(.*)/\2/' |tr ':' '	' >> .TODOtmp
done

cat .TODOtmp | column -t -s '	'
rm -f .TODOtmp

exit 0
