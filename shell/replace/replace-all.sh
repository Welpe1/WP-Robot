#!/bin/sh  

echo "input:0   cancel"
echo "input:1   replace boot.sd"
echo "input:2   replace fip.bin"
read user_choice
echo "user input:${user_choice}"


if [ "$user_choice" -eq 0 ];then
    echo "cancel"
    exit 1
elif [ "$user_choice" -eq 1 ]; then
    file_to_replace="boot.sd"
elif [ "$user_choice" -eq 2 ]; then
    file_to_replace="fip.bin"
else
    echo "usage err! please input 0 1 or 2"
    exit 1
fi

if [ -f "$file_to_replace" ]; then
    mount /dev/mmcblk0p1 /root/rom || {
        echo "Failed to mount /dev/mmcblk0p1 to /root/rom"
        exit 1
    }
    mv "$file_to_replace" /root/rom/
    umount /root/rom
    
    sleep 1
    reboot
else
    echo "no $file_to_replace in the current directory"
    exit 1
fi

#上传至milkv-duo,执行此脚本可以自动替换boot.sd或fip.bin并重启


