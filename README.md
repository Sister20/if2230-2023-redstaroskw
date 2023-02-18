# TUGAS BESAR SISTEM OPERASI - IF2230 
> Milestone 1: Pembuatan Sistem Operasi x86, Booting, Kernel, 32 bit Protected Mode

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
Dalam tugas ini akan membuat sebuah program Sistem Operasi. Sistem Operasi yang akan dibuat akan berjalan pada arsitektur x86 32 bit yang nanti akan dijalankan dengan emulator QEMU. Pada milestone pertama, hal-hal yang akan dibuat adalah sebagai berikut:
- Membuat build script
- Menjalankan sistem operasi
- Membuat output dengan text
- Memasuki Protected Mode

## Struktur File
```bash
📦if2230-2023-redstaroskw
 ┣ 📂bin
 ┃ ┗ 📜.gitignore
 ┣ 📂other
 ┃ ┗ 📜grub1
 ┣ 📂src
 ┃ ┣ 📂lib-header
 ┃ ┣ ┣ 📜framebuffer.h
 ┃ ┣ ┣ 📜gdt.h
 ┃ ┣ ┣ 📜kernel_loader.h
 ┃ ┣ ┣ 📜portio.h
 ┃ ┣ ┣ 📜stdmem.h
 ┃ ┣ ┗ 📜stdtype.h
 ┃ ┣ 📜framebuffer.c
 ┃ ┣ 📜gdt.c
 ┃ ┣ 📜kernel.c
 ┃ ┣ 📜kernel_loader.s
 ┃ ┣ 📜linker.ld
 ┃ ┣ 📜menu.lst
 ┃ ┣ 📜portio.c
 ┃ ┗ 📜stdmem.c
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
 3. Jalankan program menjalankan `make all` di terminal pada directory repository ini
 
 ## Acknowledgements
- Tuhan Yang Maha Esa
- Dosen Mata Kuliah yaitu Pak Judhi (K1), Pak Imam (K2), dan Bu Robithoh (K3)
- Kakak-Kakak Asisten Mata Kuliah Sistem Operasi IF2230
