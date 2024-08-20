#!/bin/bash

clear
cd ./install/soc_cv1800b_milkv_duo_sd
if [ -f fip.bin ]; then  
	rm ./fip.bin  
	sleep 2
	echo "delete fip.bin"
	
fi

cd ../../
pwd

cd ./freertos/cvitek/install/lib
rm -f *

cd ../../../../

source device/milkv-duo-sd/boardconfig.sh
source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
#clean_uboot
build_uboot


cd ./install/soc_cv1800b_milkv_duo_sd
if [ -f fip.bin ]; then  
	ls -l fip.bin  
	sleep 2
	echo "need transfer fip.bin?  Y(y)/N(n)"
	read answer

	# 将用户的输入转换为小写以便比较  
	answer="${answer,,}"  

	if [[ "$answer" =~ ^(y)$ ]]; then  
    
    	scp fip.bin root@192.168.42.1:/root
    fi  
fi
	