#!/bin/bash



echo "input:0   cancel"
echo "input:1   build boot.sd"
echo "input:2   build fip.bin"
read user_choice
echo "user input:${user_choice}"


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
cd ./install/${Board}
if [ -f fip.bin ]; then  
	rm ./fip.bin  
	sleep 2
	echo "delete fip.bin"
	
fi

