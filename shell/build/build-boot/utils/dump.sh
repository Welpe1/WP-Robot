#!/bin/bash


image_data=($(grep 'data = /incbin/("./.*");' $1 | awk -F'["]' '{print $2}'))

i=$((0))
for item in "${image_data[@]}"
do
    echo find[$i]:{$item}
    dumpimage boot.sd -T flat_dt -p $i -o $item
    i=$((i + 1))
done
