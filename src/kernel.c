#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel_loader.h"

// void kernel_setup(void) {
//     uint32_t a;
//     uint32_t volatile b = 0x0000BABE;
//     __asm__("mov $0xCAFE0000, %0" : "=r"(a));
//     while (TRUE) b += 1;
// }

void kernel_setup(void) {
    framebuffer_clear();
    framebuffer_write(3, 8,  'H', 0, 0xF);
    framebuffer_write(3, 9,  'a', 0, 0xF);
    framebuffer_write(3, 10, 'i', 0, 0xF);
    framebuffer_write(3, 11, '!', 0, 0xF);
    // write hello world!
    framebuffer_write(4, 8,  'H', 0, 0xF);
    framebuffer_write(4, 9,  'e', 0, 0xF);
    framebuffer_write(4, 10, 'l', 0, 0xF);
    framebuffer_write(4, 11, 'l', 0, 0xF);
    framebuffer_write(4, 12, 'o', 0, 0xF);
    framebuffer_write(4, 13, ' ', 0, 0xF);
    framebuffer_write(4, 14, 'w', 0, 0xF);
    framebuffer_write(4, 15, 'o', 0, 0xF);
    framebuffer_write(4, 16, 'r', 0, 0xF);
    framebuffer_write(4, 17, 'l', 0, 0xF);
    framebuffer_write(4, 18, 'd', 0, 0xF);
    framebuffer_write(4, 19, '!', 0, 0xF);
    // write jason uff uff
    framebuffer_write(5, 8,  'J', 0, 0xF);
    framebuffer_write(5, 9,  'a', 0, 0xF);
    framebuffer_write(5, 10, 's', 0, 0xF);
    framebuffer_write(5, 11, 'o', 0, 0xF);
    framebuffer_write(5, 12, 'n', 0, 0xF);
    framebuffer_write(5, 13, ' ', 0, 0xF);
    framebuffer_write(5, 14, 'u', 0, 0xF);
    framebuffer_write(5, 15, 'f', 0, 0xF);
    framebuffer_write(5, 16, 'f', 0, 0xF);
    framebuffer_write(5, 17, ' ', 0, 0xF);
    framebuffer_write(5, 18, 'u', 0, 0xF);
    framebuffer_write(5, 19, 'f', 0, 0xF);
    framebuffer_write(5, 20, 'f', 0, 0xF);
    framebuffer_set_cursor(5, 19);
    while (TRUE);
}