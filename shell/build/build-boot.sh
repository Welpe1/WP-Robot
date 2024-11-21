#!/bin/bash


#此脚本可以编译boot.sd并上传至milkv duo
#使用前确保编译过一次sdk
#请将此脚本放置在duo-buildroot-sdk下,并创建build-boot文件夹
#支持milkv duo 64和256


Board=64
main_dir=$(pwd)
build_boot_dir=${main_dir}/build-boot
  
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

# cd ${main_dir}/u-boot-2021.10/build/${Board}/arch/riscv/dts
# if [ -f cv1800b_milkv_duo_sd.dtb ]; then  
# 	rm ./cv1800b_milkv_duo_sd.dtb
# 	echo "already delete cv1800b_milkv_duo_sd.dtb"
# fi

cd ${main_dir}

source device/milkv-duo-sd/boardconfig.sh
source build/milkvsetup.sh
defconfig ${Config}
clean_uboot
build_uboot

cp "${main_dir}/u-boot-2021.10/build/${Config}/arch/riscv/dts/${Config}.dtb" "${build_boot_dir}/${Config}/"

cd ${build_boot_dir}/${Config}
mkimage -f multi.its boot.sd

if [ -f boot.sd ]; then  
	ls -l boot.sd 
	echo "need transfer boot.sd?  Y(y)/N(n)"
	read answer

	# 将用户的输入转换为小写以便比较  
	answer="${answer,,}"  

	if [[ "$answer" != "n" ]]; then  
    scp boot.sd root@${ssh_ip}:/root  
	fi
fi




