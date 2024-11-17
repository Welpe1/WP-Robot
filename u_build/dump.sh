#!/bin/bash

cp ./../u-boot-2021.10/build/cv1800b_milkv_duo_sd/arch/riscv/dts/cv1800b_milkv_duo_sd.dtb ./


image_data=($(grep 'data = /incbin/("./.*");' $1 | awk -F'["]' '{print $2}'))

i=$((0))
for item in "${image_data[@]}"
do
    echo find[$i]:{$item}
    dumpimage boot.sd -T flat_dt -p $i -o $item
    i=$((i + 1))
done
