#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel_loader.h"
#include "lib-header/idt.h"
#include "lib-header/interrupt.h"
#include "lib-header/keyboard.h"
#include "lib-header/fat32.h"


void kernel_setup(void) {
    enter_protected_mode(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    keyboard_state_activate();

    struct ClusterBuffer cbuf[5];
    for (uint32_t i = 0; i < 5; i++)
        for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
            cbuf[i].buf[j] = i + 'a';

    struct FAT32DriverRequest request = {
        .buf                   = cbuf,
        .name                  = "ikanaide",
        .ext                   = "uwu",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0,
    } ;

    write(request);  // Create folder "ikanaide"
    memcpy(request.name, "kano1\0\0\0", 8);
    write(request);  // Create folder "kano1"
    memcpy(request.name, "ikanaide", 8);
    delete(request); // Delete first folder, thus creating hole in FS

    memcpy(request.name, "daijoubu", 8);
    request.buffer_size = 5*CLUSTER_SIZE;
    write(request);  // Create fragmented file "daijoubu"

    struct ClusterBuffer readcbuf;
    read_clusters(&readcbuf, ROOT_CLUSTER_NUMBER+1, 1); 
    // If read properly, readcbuf should filled with 'a'

    request.buffer_size = CLUSTER_SIZE;
    read(request);   // Failed read due not enough buffer size
    request.buffer_size = 5*CLUSTER_SIZE;
    read(request);   // Success read on file "daijoubu"

    while (TRUE);
}













// void kernel_setup(void) {
//     enter_protected_mode(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
//     __asm__("int $0x4");
//     while (TRUE);
// }

// void kernel_setup(void) {
//     uint32_t a;
//     uint32_t volatile b = 0x0000BABE;
//     __asm__("mov $0xCAFE0000, %0" : "=r"(a));
//     while (TRUE) b += 1;
// }


// void kernel_setup(void) {
//     enter_protected_mode(&_gdt_gdtr);
//     framebuffer_clear();
//     framebuffer_write(4,42,' ',0,0x4);
//     framebuffer_write(5,36,' ',0,0x1);
//     framebuffer_write(5,37,' ',0,0x3);
//     framebuffer_write(5,38,' ',0,0x3);
//     framebuffer_write(5,39,' ',0,0x3);
//     framebuffer_write(5,42,' ',0,0x6);
//     framebuffer_write(5,41,' ',0,0x8);

//     framebuffer_write(6,35,' ',0,0x1);
//     framebuffer_write(6,36,' ',0,0x3);
//     framebuffer_write(6,37,' ',0,0x3);
//     framebuffer_write(6,38,' ',0,0x9);
//     framebuffer_write(6,39,' ',0,0x9);
//     framebuffer_write(6,40,' ',0,0x3);
    
//     framebuffer_write(7,34,' ',0,0x1);
//     framebuffer_write(7,35,' ',0,0x1);
//     framebuffer_write(7,37,' ',0,0x3);
//     framebuffer_write(7,37,' ',0,0x3);
//     framebuffer_write(7,38,' ',0,0x3);
//     framebuffer_write(7,39,' ',0,0x9);
//     framebuffer_write(7,40,' ',0,0x9);
//     framebuffer_write(7,41,' ',0,0x3);

//     framebuffer_write(8,34,' ',0,0x1);
//     framebuffer_write(8,35,' ',0,0x1);
//     framebuffer_write(8,36,' ',0,0x7);
//     framebuffer_write(8,37,' ',0,0x3);
//     framebuffer_write(8,38,' ',0,0x3);
//     framebuffer_write(8,39,' ',0,0x3);
//     framebuffer_write(8,40,' ',0,0x9);
//     framebuffer_write(8,41,' ',0,0x3);

//     framebuffer_write(9,34,' ',0,0x8);
//     framebuffer_write(9,35,' ',0,0x1);
//     framebuffer_write(9,36,' ',0,0x7);
//     framebuffer_write(9,37,' ',0,0x7);
//     framebuffer_write(9,38,' ',0,0x3);
//     framebuffer_write(9,39,' ',0,0x3);
//     framebuffer_write(9,40,' ',0,0x3);
//     framebuffer_write(9,41,' ',0,0x3);

//     framebuffer_write(10,34,' ',0,0x8);
//     framebuffer_write(10,35,' ',0,0x8);
//     framebuffer_write(10,36,' ',0,0x7);
//     framebuffer_write(10,37,' ',0,0x7);
//     framebuffer_write(10,38,' ',0,0x7);
//     framebuffer_write(10,39,' ',0,0x7);
//     framebuffer_write(10,40,' ',0,0x7);
//     framebuffer_write(10,41,' ',0,0x7);

//     framebuffer_write(11,35,' ',0,0x8);
//     framebuffer_write(11,36,' ',0,0x8);
//     framebuffer_write(11,37,' ',0,0x8);
//     framebuffer_write(11,38,' ',0,0x7);
//     framebuffer_write(11,39,' ',0,0x7);
//     framebuffer_write(11,40,' ',0,0x7);

//     framebuffer_write(12,36,' ',0,0x8);
//     framebuffer_write(12,37,' ',0,0x8);
//     framebuffer_write(12,38,' ',0,0x8);
//     framebuffer_write(12,39,' ',0,0x8);

//     framebuffer_write(17,33,'R',0xF,0);
//     framebuffer_write(17,34,'e',0xF,0);
//     framebuffer_write(17,35,'d',0xF,0);
//     framebuffer_write(17,36,'S',0xF,0);
//     framebuffer_write(17,37,'t',0xF,0);
//     framebuffer_write(17,38,'a',0xF,0);
//     framebuffer_write(17,39,'r',0xF,0);
//     framebuffer_write(17,40,'O',0xF,0);
//     framebuffer_write(17,41,'S',0xF,0);
//     framebuffer_write(17,42,'K',0xF,0);
//     framebuffer_write(17,43,'W',0xF,0);

//     framebuffer_write(18,37,'B',0xF,0);
//     framebuffer_write(18,38,'y',0xF,0);
//     framebuffer_write(18,39,':',0xF,0);

//     framebuffer_write(19,30,'H',0xF,0);
//     framebuffer_write(19,31,'e',0xF,0);
//     framebuffer_write(19,32,'n',0xF,0);
//     framebuffer_write(19,33,'r',0xF,0);
//     framebuffer_write(19,34,'y',0xF,0);
//     framebuffer_write(19,35,' ',0xF,0);
//     framebuffer_write(19,36,'-',0xF,0);
//     framebuffer_write(19,37,' ',0xF,0);
//     framebuffer_write(19,38,'1',0xF,0);
//     framebuffer_write(19,39,'3',0xF,0);
//     framebuffer_write(19,40,'5',0xF,0);
//     framebuffer_write(19,41,'2',0xF,0);
//     framebuffer_write(19,42,'1',0xF,0);
//     framebuffer_write(19,43,'0',0xF,0);
//     framebuffer_write(19,44,'0',0xF,0);
//     framebuffer_write(19,45,'4',0xF,0);

//     framebuffer_write(20,29,'M',0xF,0);
//     framebuffer_write(20,30,'a',0xF,0);
//     framebuffer_write(20,31,'t',0xF,0);
//     framebuffer_write(20,32,'t',0xF,0);
//     framebuffer_write(20,33,'h',0xF,0);
//     framebuffer_write(20,34,'e',0xF,0);
//     framebuffer_write(20,35,'w',0xF,0);
//     framebuffer_write(20,36,' ',0xF,0);
//     framebuffer_write(20,37,'-',0xF,0);
//     framebuffer_write(20,38,' ',0xF,0);
//     framebuffer_write(20,39,'1',0xF,0);
//     framebuffer_write(20,40,'3',0xF,0);
//     framebuffer_write(20,41,'5',0xF,0);
//     framebuffer_write(20,42,'2',0xF,0);
//     framebuffer_write(20,43,'1',0xF,0);
//     framebuffer_write(20,44,'0',0xF,0);
//     framebuffer_write(20,45,'0',0xF,0);
//     framebuffer_write(20,46,'7',0xF,0);

//     framebuffer_write(21,30,'J',0xF,0);
//     framebuffer_write(21,31,'a',0xF,0);
//     framebuffer_write(21,32,'s',0xF,0);
//     framebuffer_write(21,33,'o',0xF,0);
//     framebuffer_write(21,34,'n',0xF,0);
//     framebuffer_write(21,35,' ',0xF,0);
//     framebuffer_write(21,36,'-',0xF,0);
//     framebuffer_write(21,37,' ',0xF,0);
//     framebuffer_write(21,38,'1',0xF,0);
//     framebuffer_write(21,39,'3',0xF,0);
//     framebuffer_write(21,40,'5',0xF,0);
//     framebuffer_write(21,41,'2',0xF,0);
//     framebuffer_write(21,42,'1',0xF,0);
//     framebuffer_write(21,43,'0',0xF,0);
//     framebuffer_write(21,44,'0',0xF,0);
//     framebuffer_write(21,45,'8',0xF,0);

//     framebuffer_write(22,30,'N',0xF,0);
//     framebuffer_write(22,31,'a',0xF,0);
//     framebuffer_write(22,32,'d',0xF,0);
//     framebuffer_write(22,33,'i',0xF,0);
//     framebuffer_write(22,34,'l',0xF,0);
//     framebuffer_write(22,35,' ',0xF,0);
//     framebuffer_write(22,36,'-',0xF,0);
//     framebuffer_write(22,37,' ',0xF,0);
//     framebuffer_write(22,38,'1',0xF,0);
//     framebuffer_write(22,39,'3',0xF,0);
//     framebuffer_write(22,40,'5',0xF,0);
//     framebuffer_write(22,41,'2',0xF,0);
//     framebuffer_write(22,42,'1',0xF,0);
//     framebuffer_write(22,43,'0',0xF,0);
//     framebuffer_write(22,44,'2',0xF,0);
//     framebuffer_write(22,45,'4',0xF,0);

//     // framebuffer_write(15,35,' ',0,)
//     framebuffer_set_cursor(3, 42);
// }