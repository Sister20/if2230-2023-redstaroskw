#include "lib-header/framebuffer.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/portio.h"

void port_byte_out(uint16_t port, uint8_t data) {
    /*
    I.S. Menerima port dan data
    F.S. Mengirimkan data ke port
    */
    asm volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
    // "a" (data) -> data akan dimasukkan ke register
    // "Nd" (port) -> port akan dimasukkan ke register
    // "outb" -> untuk menuliskan 'data' ke 'port'
    // "%0" -> data akan dimasukkan ke register %eax
    // "%1" -> port akan dimasukkan ke register %edx
    // volatile -> menandakan bahwa kode assembly memiliki efek dimana akan melakukan writing pada I/O port supaya compiler tidak mengoptimasi kode assembly tersebut
}

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    /*
    I.S. Menerima r sebagai baris cursor dan c sebagai kolom cursor
    F.S. Mengubah posisi cursor ke baris r dan kolom c
    */
    uint16_t pos = r * 80 + c; // Menghitung posisi cursor
    // Mengirimkan posisi cursor ke port CURSOR_PORT_CMD dan CURSOR_PORT_DATA
    port_byte_out(CURSOR_PORT_CMD, 0x0F);
    port_byte_out(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
    port_byte_out(CURSOR_PORT_CMD, 0x0E);
    port_byte_out(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    /*
    I.S. Menerima :
            row = baris dari karakter c
            col = kolom dari karakter c
            c = karakter yang akan ditulis
            fg = warna karakter
            bg = warna background
    F.S. Menulis karakter c ke framebuffer dengan warna fg dan warna background bg
    */
    uint16_t pos = row * 80 + col; // Menghitung posisi karakter c
    uint8_t color = (bg << 4) | fg; // Memberi warna dan warna background pada karakter c
    uint16_t *fb = (uint16_t *) MEMORY_FRAMEBUFFER; // Mengambil alamat memory framebuffer
    fb[pos] = (color << 8) | c; // Menulis karakter c ke framebuffer dengan warna color (shift informasi color ke HIGH BYTE)
}

void framebuffer_clear(void) {
    /*
    I.S. -
    F.S. Menghapus semua karakter yang ada di framebuffer
    */
    uint16_t *fb = (uint16_t *) MEMORY_FRAMEBUFFER; // Mengambil alamat memory framebuffer
    // Looping untuk menghapus semua karakter yang ada di framebuffer
    for (uint16_t i = 0; i < 80 * 25; i++) {
        fb[i] = (0x07 << 8) | 0x00;
    }
}