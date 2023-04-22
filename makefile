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

os: disk insert-shell run

run: all
	@qemu-system-i386 -s $(DISK_LOAD) $(OUTPUT_FOLDER)/$(ISO_NAME).iso

disk:
	@qemu-img create -f raw $(OUTPUT_FOLDER)/$(DISK_NAME).bin 4M

all: build

build: iso

clean:
	rm -rf *.o *.iso $(OUTPUT_FOLDER)/kernel $(OUTPUT_FOLDER)/*o $(OUTPUT_FOLDER)/*.iso $(OUTPUT_FOLDER)/*.bin

kernel:
	@$(ASM) $(AFLAGS) $(SOURCE_FOLDER)/kernel_loader.s -o $(OUTPUT_FOLDER)/kernel_loader.o
	@$(ASM) $(AFLAGS) $(SOURCE_FOLDER)/intsetup.s -o $(OUTPUT_FOLDER)/intsetup.o
#@$(foreach file, $(wildcard $(SOURCE_FOLDER)/*.c), $(CC) $(CFLAGS) $(file) -o $(OUTPUT_FOLDER)/$(notdir $(file:.c=.o));)
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/disk.c -o $(OUTPUT_FOLDER)/disk.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/fat32.c -o $(OUTPUT_FOLDER)/fat32.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/framebuffer.c -o $(OUTPUT_FOLDER)/framebuffer.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/gdt.c -o $(OUTPUT_FOLDER)/gdt.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/idt.c -o $(OUTPUT_FOLDER)/idt.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/interrupt.c -o $(OUTPUT_FOLDER)/interrupt.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/kernel.c -o $(OUTPUT_FOLDER)/kernel.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/keyboard.c -o $(OUTPUT_FOLDER)/keyboard.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/paging.c -o $(OUTPUT_FOLDER)/paging.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/portio.c -o $(OUTPUT_FOLDER)/portio.o
	@$(CC) $(CFLAGS) $(SOURCE_FOLDER)/stdmem.c -o $(OUTPUT_FOLDER)/stdmem.o
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

inserter:
	@$(CC) -Wno-builtin-declaration-mismatch \
		$(SOURCE_FOLDER)/stdmem.c $(SOURCE_FOLDER)/fat32.c \
		$(SOURCE_FOLDER)/external-inserter.c \
		-o $(OUTPUT_FOLDER)/inserter

user-shell:
	@$(ASM) $(AFLAGS) $(SOURCE_FOLDER)/user-entry.s -o user-entry.o
	@$(CC)  $(CFLAGS) -fno-pie $(SOURCE_FOLDER)/user-shell.c -o user-shell.o
	@$(LIN) -T $(SOURCE_FOLDER)/user-linker.ld -melf_i386 \
		user-entry.o user-shell.o -o $(OUTPUT_FOLDER)/shell
	@echo Linking object shell object files and generate flat binary...
	@size --target=binary bin/shell
	@rm -f *.o

insert-shell: inserter user-shell
	@echo Inserting shell into root directory...
	@cd $(OUTPUT_FOLDER); ./inserter shell 2 $(DISK_NAME).bin