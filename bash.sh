#!/bin/bash
# Usage: ./list_fds.sh <PARENT_PID>
while true
do
    sleep 1
    TARGET_PID=$1
    echo "FD table for Parent: $TARGET_PID"
    ls -l /proc/$TARGET_PID/fd 2>/dev/null

    # Find children
    CHILDREN=$(pgrep -P $TARGET_PID)

    for child in $CHILDREN; do
        echo "---"
        echo "FD table for Child: $child"
        ls -l /proc/$child/fd 2>/dev/null
    done
done