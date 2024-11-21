将replace-boot.sh上传至milkv duo开发板
同时上传boot.sd
执行replace-boot.sh可以自动替换系统的boot.sd(如设备树)

将replace-fip.sh上传至milkv duo开发板
同时上传fip.bin
执行replace-fip.sh可以自动替换系统的fip.bin(如小核freertos)

replace-all.sh脚本结合了replace-boot.sh与replace-fip.sh的功能
用户可以根据需要选择这三个脚本中的任一个