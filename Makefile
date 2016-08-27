no:
	@echo "This is now how you want to build!"
	@echo ""
	@echo "Use make std-all to build the standard library for user mode"
	@echo "Build the payload in usr/init"
	@echo "Build for your target of choice with pi-qemu-all or pi-b-all"
	@echo "Then make install to combine the kernel and payload into a binary image"

pi-qemu-all: 
			$(MAKE) -f pi-qemu.mk pi-qemu-all
		
pi-b-all:
			$(MAKE) -f pi-b.mk pi-b-all
			
std-all:	
			cd usr/std
			$(MAKE) -f usr/std/Makefile
			cd ../../
		
		
#General targets
		
clean: 
		rm -f *o nos.elf nos.bin nos.sym nos.bin nos.sym.temp nos.img
		rm -r -f .obj
		
install: nos.elf
		set -e
		arm-none-eabi-nm nos.elf > nos.sym.temp
		arm-none-eabi-objcopy --set-section-flags .bss=alloc,load,contents nos.elf -O binary nos.bin
		./mk_img.sh
