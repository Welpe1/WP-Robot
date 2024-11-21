#!/bin/bash

echo "input:0   cancel"
echo "input:1   build boot.sd"
echo "input:2   build fip.bin"
read user_choice
echo "user input:${user_choice}"

if [ "$user_choice" -eq 0 ]; then
    echo "cancel"
    exit 0
elif [ "$user_choice" -gt 2 ] || [ "$user_choice" -lt 0 ]; then
    echo "usage err! please input 0, 1, or 2"
    exit 1
fi

clear

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

cd ${main_dir}
source device/milkv-duo-sd/boardconfig.sh
source build/milkvsetup.sh
defconfig ${Config}

if [ "$user_choice" -eq 1 ];then
    # clean_uboot
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
elif [ "$user_choice" -eq 2 ]; then
    cd ./install/${Board}
    if [ -f fip.bin ]; then  
        rm ./fip.bin  
        sleep 2
        echo "delete fip.bin"
        
    fi
    cd ${main_dir}/freertos/cvitek/install/lib
    rm -f *
    # clean_uboot
    build_uboot
    cd ${main_dir}/install/${Board}
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
    
fi











