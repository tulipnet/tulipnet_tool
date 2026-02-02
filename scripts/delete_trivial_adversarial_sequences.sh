#!/bin/bash

ADVERSARIAL_SEQUENCES_PATH="$1"

if [ "x${ADVERSARIAL_SEQUENCES_PATH}" != "x" ]
then
    for FIC in `find $ADVERSARIAL_SEQUENCES_PATH -type f`
    do
        cat $FIC | tr "\n" ";" | grep -v "*" > /dev/null 2>&1

        if [ $? -eq 0 ]
        then
            rm -f $FIC
        fi
    done

    exit 0
else
    echo ERROR : $1 is empty

    exit 1
fi