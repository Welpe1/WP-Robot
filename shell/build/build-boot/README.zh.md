build-fip.sh可编译fip.bin并自动上传开发板
build-boot.sh可编译boot.sd并自动上传开发板
build-boot.sh脚本结合了build-fip.sh与build-boot.sh的功能
用户可以根据需要选择这三个脚本中的任一个(暂未实现)

注意:
build_boot.sh脚本需要放在duo-buildroot-sdk文件夹下同时创建build-boot文件夹
其中build-boot文件夹目录如下
cv1800b_milkv_duo_sd文件夹存放milkv duo 64的dtb
cv1812cp_milkv_duo256m_sd文件夹存放milkv duo 256的dtb
utils存放常用工具:dump.sh脚本可以将boot.sd解包出Image.lzma和设备树.dtb

