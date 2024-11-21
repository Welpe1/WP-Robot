#!/bin/bash

Board=64
  
if [ "$Board" == "64" ]; then  
    Board=soc_cv1800b_milkv_duo_sd
    Config=cv1800b_milkv_duo_sd
    ssh_ip=192.168.42.1  
elif [ "$Board" == "256" ]; then  
    Board=soc_cv1812cp_milkv_duo256m_sd  
    Config=cv1812cp_milkv_duo256m_sd
    ssh_ip=192.168.42.2  
fi  
clear
cd ./../install/${Board}
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
defconfig ${Config}
#clean_rtos
clean_uboot
build_uboot


cd ./install/${Board}
if [ -f fip.bin ]; then  
	ls -l fip.bin  
	echo "need transfer fip.bin?  Y(y)/N(n)"
	read answer

	# 将用户的输入转换为小写以便比较  
	answer="${answer,,}"  

	if [[ "$answer" != "n" ]]; then  
    scp fip.bin root@${ssh_ip}:/root  
	fi
fi
	
