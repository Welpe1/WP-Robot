#!/bin/sh  
  
if [ -f fip.bin ]; then  
    mount /dev/mmcblk0p1 /root/rom || {  
        echo "Failed to mount /dev/mmcblk0p1 to /root/rom"  
        exit 1  
    }  
    mv fip.bin /root/rom  
    umount /root/rom  
    
    sleep 1
    reboot

else  
    echo "no fip.bin in the current directory" 
fi

#上传至milkv-duo,执行此脚本可以自动替换fip.bin并重启