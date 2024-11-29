#!/bin/sh  

if [ -d "/root/rom" ]; then
	:
else
    mkdir /root/rom
fi



if [ -f boot.sd ]; then  
    mount /dev/mmcblk0p1 /root/rom || {  
        echo "Failed to mount /dev/mmcblk0p1 to /root/rom"  
        exit 1  
    }  
    mv boot.sd /root/rom  
    umount /root/rom  
    
    sleep 1
    reboot

else  
    echo "no boot.sd in the current directory" 
fi

#上传至milkv-duo,执行此脚本可以自动替换boot.sd并重启