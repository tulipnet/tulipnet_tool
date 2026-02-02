#!/bin/bash

ALREADY_TREATED_SHA256_SUMS_FILE_PATH="$1"
ORACLE_INPUTS_PATH="$2"

for FIC in `find $ORACLE_INPUTS_PATH -type f`
do
    FIC_SHA256_SUM_FULL=`sha256sum $FIC 2> /dev/null | cut -d' ' -f1`

    if [ $? -eq 0 ] && [ "x$FIC_SHA256_SUM_FULL" != "x" ] # To check if file realy exists
    then
        FIC_SHA256_SUM=`echo $FIC_SHA256_SUM_FULL | cut -d' ' -f1`

        grep $FIC_SHA256_SUM $ALREADY_TREATED_SHA256_SUMS_FILE_PATH > /dev/null 2>&1

        if [ $? -eq 0 ] # If found in file, we have to delete it
        then
            echo $FIC | grep __sha256_sums__ > /dev/null 2>&1

            if [ $? -eq 1 ] # Trolololololol hehehehhe
            then
                rm -f $FIC
            fi
        fi
    fi
done