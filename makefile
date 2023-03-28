# Compiler & linker
ASM           = nasm
LIN           = ld
CC            = gcc
ISO			  = genisoimage

# Directory
SOURCE_FOLDER = src
OUTPUT_FOLDER = bin
ISO_NAME      = os2023

# Flags
WARNING_CFLAG = -Wall -Wextra -Werror
DEBUG_CFLAG   = -ffreestanding -fshort-wchar -g
STRIP_CFLAG   = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs
CFLAGS        = $(DEBUG_CFLAG) $(WARNING_CFLAG) $(STRIP_CFLAG) -m32 -c -I$(SOURCE_FOLDER)
AFLAGS        = -f elf32 -g -F dwarf
LFLAGS        = -T $(SOURCE_FOLDER)/linker.ld -melf_i386
IFLAGS		  = -R -b boot/grub/grub1 -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table
DISK_NAME     = storage
DISK_LOAD	  = -drive file=bin/storage.bin,format=raw,if=ide,index=0,media=disk -cdrom 

run: all
	@qemu-system-i386 -s $(DISK_LOAD) $(OUTPUT_FOLDER)/$(ISO_NAME).iso

disk:
	@qemu-img create -f raw $(OUTPUT_FOLDER)/$(DISK_NAME).bin 4M

all: build

build: iso

clean:
	rm -rf *.o *.iso $(OUTPUT_FOLDER)/kernel $(OUTPUT_FOLDER)/*o $(OUTPUT_FOLDER)/*.iso $(OUTPUT_FOLDER)/*.bin

kernel:disk
	@$(ASM) $(AFLAGS) $(SOURCE_FOLDER)/kernel_loader.s -o $(OUTPUT_FOLDER)/kernel_loader.o
	@$(ASM) $(AFLAGS) $(SOURCE_FOLDER)/intsetup.s -o $(OUTPUT_FOLDER)/intsetup.o
	$(foreach file, $(wildcard $(SOURCE_FOLDER)/*.c), $(CC) $(CFLAGS) $(file) -o $(OUTPUT_FOLDER)/$(notdir $(file:.c=.o));)
	@$(LIN) $(LFLAGS) bin/*.o -o $(OUTPUT_FOLDER)/kernel
	@rm -f bin/*.o
	

iso: kernel
	@mkdir -p $(OUTPUT_FOLDER)/iso/boot/grub
	@cp $(OUTPUT_FOLDER)/kernel     $(OUTPUT_FOLDER)/iso/boot/
	@cp other/grub1                 $(OUTPUT_FOLDER)/iso/boot/grub/
	@cp $(SOURCE_FOLDER)/menu.lst   $(OUTPUT_FOLDER)/iso/boot/grub/
	@echo Creating ISO image...
	@$(ISO) $(IFLAGS) -o $(OUTPUT_FOLDER)/$(ISO_NAME).iso $(OUTPUT_FOLDER)/iso
	@rm -r $(OUTPUT_FOLDER)/iso/