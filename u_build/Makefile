merge: Image.lzma cv1800b_milkv_duo_sd.dtb
	mkimage -f multi.its boot.sd
dump:
	./dump.sh multi.its
	
dtb:	Image.lzma cv1800b_milkv_duo_sd.dtb
	./dump.sh multi.its
	mkimage -f multi.its boot.sd
fip:
	./fip-build.sh
	
