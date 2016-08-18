pi-qemu-all: 
			$(MAKE) -f pi-qemu.mk pi-qemu-all
		
pi-b-all:
			$(MAKE) -f pi-b.mk pi-b-all
		
		
#General targets
		
clean: 
		rm -f *o nos.elf nos.bin nos.sym nos.bin nos.sym.temp nos.img
		
install: nos.elf
		set -e
		arm-none-eabi-nm nos.elf > nos.sym.temp
#		tools/sysmap_bld.exe nos.sym.temp nos.sym
		arm-none-eabi-objcopy --set-section-flags .bss=alloc,load,contents nos.elf -O binary nos.bin
#		./mk_img.sh
		cat nos.bin > nos.img
