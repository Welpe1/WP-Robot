u_build文件夹需放在duo-buildroot-sdk下

执行make dump 将 boot.sd 拆解成Image.lzma和cv1800b_milkv_duo_sd.dtb
执行make merge 将 Image.lzma和cv1800b_milkv_duo_sd.dtb 打包成 boot.sd
执行make dtb 将取最新的dtb文件并打包成 boot.sd
执行make fip 将编译fip.bin文件并上传至milkv duo
