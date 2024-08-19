#!/bin/bash

mount /dev/mmcblk0p1 /root/rom

if [ -f fip.bin ]; then  
	mv fip.bin /root/rom
	echo "need reboot?  Y(y)/N(n)"
	read answer

	answer="${answer,,}"  

	if [[ "$answer" =~ ^(y)$ ]]; then  
    
    	reboot
    fi  
fi
	