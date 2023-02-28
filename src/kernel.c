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
    enter_protected_mode(&_gdt_gdtr);
    framebuffer_clear();
    framebuffer_write(4,42,' ',0,0x4);
    framebuffer_write(5,36,' ',0,0x1);
    framebuffer_write(5,37,' ',0,0x3);
    framebuffer_write(5,38,' ',0,0x3);
    framebuffer_write(5,39,' ',0,0x3);
    framebuffer_write(5,42,' ',0,0x6);
    framebuffer_write(5,41,' ',0,0x8);

    framebuffer_write(6,35,' ',0,0x1);
    framebuffer_write(6,36,' ',0,0x3);
    framebuffer_write(6,37,' ',0,0x3);
    framebuffer_write(6,38,' ',0,0x9);
    framebuffer_write(6,39,' ',0,0x9);
    framebuffer_write(6,40,' ',0,0x3);
    
    framebuffer_write(7,34,' ',0,0x1);
    framebuffer_write(7,35,' ',0,0x1);
    framebuffer_write(7,37,' ',0,0x3);
    framebuffer_write(7,37,' ',0,0x3);
    framebuffer_write(7,38,' ',0,0x3);
    framebuffer_write(7,39,' ',0,0x9);
    framebuffer_write(7,40,' ',0,0x9);
    framebuffer_write(7,41,' ',0,0x3);

    framebuffer_write(8,34,' ',0,0x1);
    framebuffer_write(8,35,' ',0,0x1);
    framebuffer_write(8,36,' ',0,0x7);
    framebuffer_write(8,37,' ',0,0x3);
    framebuffer_write(8,38,' ',0,0x3);
    framebuffer_write(8,39,' ',0,0x3);
    framebuffer_write(8,40,' ',0,0x9);
    framebuffer_write(8,41,' ',0,0x3);

    framebuffer_write(9,34,' ',0,0x8);
    framebuffer_write(9,35,' ',0,0x1);
    framebuffer_write(9,36,' ',0,0x7);
    framebuffer_write(9,37,' ',0,0x7);
    framebuffer_write(9,38,' ',0,0x3);
    framebuffer_write(9,39,' ',0,0x3);
    framebuffer_write(9,40,' ',0,0x3);
    framebuffer_write(9,41,' ',0,0x3);

    framebuffer_write(10,34,' ',0,0x8);
    framebuffer_write(10,35,' ',0,0x8);
    framebuffer_write(10,36,' ',0,0x7);
    framebuffer_write(10,37,' ',0,0x7);
    framebuffer_write(10,38,' ',0,0x7);
    framebuffer_write(10,39,' ',0,0x7);
    framebuffer_write(10,40,' ',0,0x7);
    framebuffer_write(10,41,' ',0,0x7);

    framebuffer_write(11,35,' ',0,0x8);
    framebuffer_write(11,36,' ',0,0x8);
    framebuffer_write(11,37,' ',0,0x8);
    framebuffer_write(11,38,' ',0,0x7);
    framebuffer_write(11,39,' ',0,0x7);
    framebuffer_write(11,40,' ',0,0x7);

    framebuffer_write(12,36,' ',0,0x8);
    framebuffer_write(12,37,' ',0,0x8);
    framebuffer_write(12,38,' ',0,0x8);
    framebuffer_write(12,39,' ',0,0x8);

    // framebuffer_write();
    framebuffer_write(14,37,'b',0xF,0);
    framebuffer_write(14,38,'y',0xF,0);
    framebuffer_write(15,34,'1',0xF,0);
    framebuffer_write(15,35,'3',0xF,0);
    framebuffer_write(15,36,'5',0xF,0);
    framebuffer_write(15,37,'2',0xF,0);
    framebuffer_write(15,38,'1',0xF,0);
    framebuffer_write(15,39,'0',0xF,0);
    framebuffer_write(15,40,'0',0xF,0);
    framebuffer_write(15,41,'4',0xF,0);

    framebuffer_write(16,34,'1',0xF,0);
    framebuffer_write(16,35,'3',0xF,0);
    framebuffer_write(16,36,'5',0xF,0);
    framebuffer_write(16,37,'2',0xF,0);
    framebuffer_write(16,38,'1',0xF,0);
    framebuffer_write(16,39,'0',0xF,0);
    framebuffer_write(16,40,'0',0xF,0);
    framebuffer_write(16,41,'7',0xF,0);

    framebuffer_write(17,34,'1',0xF,0);
    framebuffer_write(17,35,'3',0xF,0);
    framebuffer_write(17,36,'5',0xF,0);
    framebuffer_write(17,37,'2',0xF,0);
    framebuffer_write(17,38,'1',0xF,0);
    framebuffer_write(17,39,'0',0xF,0);
    framebuffer_write(17,40,'0',0xF,0);
    framebuffer_write(17,41,'8',0xF,0);

    framebuffer_write(18,34,'1',0xF,0);
    framebuffer_write(18,35,'3',0xF,0);
    framebuffer_write(18,36,'5',0xF,0);
    framebuffer_write(18,37,'2',0xF,0);
    framebuffer_write(18,38,'1',0xF,0);
    framebuffer_write(18,39,'0',0xF,0);
    framebuffer_write(18,40,'2',0xF,0);
    framebuffer_write(18,41,'4',0xF,0);
    framebuffer_set_cursor(3, 42);
    while (TRUE);
}