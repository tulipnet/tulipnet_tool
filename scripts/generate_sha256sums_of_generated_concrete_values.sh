#!/bin/bash

CBD_OUTPUT_PATH="$1"

if [ "x${CBD_ALREADY_TREATED_CONCRETE_INPUTS_SHA256_SUMS_PATH}" != "x" ]
then
    cp ${CBD_ALREADY_TREATED_CONCRETE_INPUTS_SHA256_SUMS_PATH} $CBD_OUTPUT_PATH/__sha256_sums__
fi

touch $CBD_OUTPUT_PATH/__sha256_sums__

for FIC in `find $CBD_OUTPUT_PATH/apply_wildcard/ -type f`
do
    sha256sum $FIC | cut -d' ' -f1 >> $CBD_OUTPUT_PATH/__sha256_sums__
done

for FIC in `find $CBD_OUTPUT_PATH/static/ -type f`
do
    sha256sum $FIC | cut -d' ' -f1 >> $CBD_OUTPUT_PATH/__sha256_sums__
done