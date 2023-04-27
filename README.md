# TUGAS BESAR SISTEM OPERASI - IF2230 
> Milestone 1: Pembuatan Sistem Operasi x86, Booting, Kernel, 32 bit Protected Mode

> Milestone 2: Pembuatan Sistem Operasi x86, Interrupt, Driver, dan Filesystem

> Milestone 3: Pembuatan Sistem Operasi x86 Paging, User Mode, dan Shell

## Anggota Kelompok
<table>
    <tr>
        <td colspan="3", align = "center"><center>Nama Kelompok: redStarOSKW</center></td>
    </tr>
    <tr>
        <td>No.</td>
        <td>Nama</td>
        <td>NIM</td>
    </tr>
    <tr>
        <td>1.</td>
        <td>Henry Anand Septian Radityo</td>
        <td>13521004</td>
    </tr>
    <tr>
        <td>2.</td>
        <td>Matthew Mahendra</td>
        <td>13521007</td>
    </tr>
    <tr>
        <td>3.</td>
        <td>Jason Rivalino</td>
        <td>13521008</td>
    </tr>
        <tr>
        <td>4.</td>
        <td>Ahmad Nadil</td>
        <td>13521024</td>
    </tr>
</table>

## Table of Contents
* [Deskripsi Singkat](#deskripsi-singkat)
* [Struktur File](#struktur-file)
* [Requirements](#requirements)
* [Cara Menjalankan Program](#cara-menjalankan-program)
* [Acknowledgements](#acknowledgements)

## Deskripsi Singkat 
Dalam tugas ini akan membuat sebuah program Sistem Operasi. Sistem Operasi yang akan dibuat akan berjalan pada arsitektur x86 32 bit yang nanti akan dijalankan dengan emulator QEMU. 

Pada milestone pertama, hal-hal yang akan dibuat adalah sebagai berikut:
- Membuat build script
- Menjalankan sistem operasi
- Membuat output dengan text
- Memasuki Protected Mode

Pada milestone kedua, hal-hal yang akan dibuat adalah sebagai berikut:
- Interrupt & IDT
- Keyboard driver
- Disk driver
- File System

Pada milestone ketiga, hal-hal yang akan dibuat adalah sebagai berikut:
- Paging
- User Mode
- Shell


## Struktur File
```bash
📦if2230-2023-redstaroskw
 ┣ 📂bin
 ┃ ┗ 📜.gitignore
 ┣ 📂other
 ┃ ┗ 📜grub1
 ┣ 📂src
 ┃ ┣ 📂lib-header
 ┃ ┃ ┣ 📜disk.h
 ┃ ┃ ┣ 📜fat32.h
 ┃ ┃ ┣ 📜framebuffer.h
 ┃ ┃ ┣ 📜gdt.h
 ┃ ┃ ┣ 📜idt.h
 ┃ ┃ ┣ 📜interrupt.h
 ┃ ┃ ┣ 📜kernel_loader.h
 ┃ ┃ ┣ 📜keyboard.h
 ┃ ┃ ┣ 📜portio.h
 ┃ ┃ ┣ 📜paging.h
 ┃ ┃ ┣ 📜stdmem.h
 ┃ ┃ ┗ 📜stdtype.h
 ┃ ┣ 📜disk.c 
 ┃ ┣ 📜external-inserter.c 
 ┃ ┣ 📜fat32.c
 ┃ ┣ 📜framebuffer.c
 ┃ ┣ 📜gdt.c
 ┃ ┣ 📜idt.c
 ┃ ┣ 📜interrupt.c
 ┃ ┣ 📜intsetup.s
 ┃ ┣ 📜kernel.c
 ┃ ┣ 📜kernel_loader.s
 ┃ ┣ 📜keyboard.c
 ┃ ┣ 📜linker.ld
 ┃ ┣ 📜menu.lst
 ┃ ┣ 📜paging.c
 ┃ ┣ 📜portio.c
 ┃ ┗ 📜stdmem.c
 ┃ ┣ 📜user-entry.s 
 ┃ ┣ 📜user-linker.ld
 ┃ ┗ 📜user-shell.c
 ┣ 📜README.md
 ┗ 📜makefile
 ```
 
 ## Requirements
 1. Netwide assembler (Compiler assembly utama)
 2. GNU C Compiler (Compiler C utama untuk sistem operasi)
 3. GNU Linker (Linker object code hasil kompilasi)
 4. QEMU - System i386 (Emulator utama untuk menjalankan sistem operasi)
 5. GNU Make (Build tools untuk sistem operasi)
 6. genisoimage (Tool untuk pembuatan image sistem operasi)
 7. Sistem Operasi Ubuntu 20.04.
 8. Visual Studio Code
 
 ## Cara Menjalankan Program
 1. Clone repository Github ini
 2. Install semua requirements yang diperlukan
 3. Jalankan program menjalankan `make os` di terminal pada directory repository ini
 
 ## Acknowledgements
- Tuhan Yang Maha Esa
- Dosen Mata Kuliah yaitu Pak Judhi (K1), Pak Imam (K2), dan Bu Robithoh (K3)
- Kakak-Kakak Asisten Mata Kuliah Sistem Operasi IF2230
