CFLAGS = -ffreestanding -m32 -g

all: kernel boot image celesix run

kernel:
	/usr/local/i386elfgcc/bin/i386-elf-gcc $(CFLAGS) -c Source/kernel.cpp -o Binaries/kernel.o

boot:
	nasm -felf32 "Source/boot.asm" -f bin -o "Binaries/boot.bin"
# nasm -f elf32 "Source/boot.asm" -o "Binaries/boot.o"
	nasm "Source/kernel_entry.asm" -f elf -o "Binaries/kernel_entry.o"

# test MAIN system
#nasm "Source/cboot.asm" -f elf -o "Binaries/cboot.o"

image:
	/usr/local/i386elfgcc/bin/i386-elf-ld -o "Binaries/full_kernel.bin" -Ttext 0x1000 "Binaries/kernel_entry.o" "Binaries/kernel.o" "Binaries/cboot.o" --oformat binary

	cat "Binaries/boot.bin" "Binaries/full_kernel.bin" "Binaries/zeroes.bin" > "Binaries/image.bin"

celesix:
	nasm -felf32 "Source/cboot.asm" -f bin -o "Binaries/cboot.bin"

run:
# Create Images/coreboot.img from Binaries/image.bin, up to 50 sectors (50 * 512 bytes)
	dd if=Binaries/image.bin of=Images/coreboot.img bs=512 count=63
	dd if=/dev/zero of=Binaries/zeroes.bin bs=512 count=17000

# now finally concat the two

# add MAIN system
	dd if=Binaries/cboot.bin of=Images/coreboot.img bs=512 seek=64 conv=notrunc




	#qemu-system-x86_64 -drive format=raw,file="Images/coreboot.img" -m 128M -d int -no-reboot
	qemu-system-x86_64 -device piix3-ide,id=ide -drive id=disk,file="Images/coreboot.img",format=raw,if=none -device ide-hd,drive=disk,bus=ide.0  -m 128M -d int -no-reboot



# sudo fdisk -l /dev/sda
# sudo dd if=Images/coreboot.img of=/dev/sda bs=4M status=progress && sync

# FIX FOUND! make the "boot" extremely short and shunt everything off to kernel_entry.o where externs CAN be referenced

