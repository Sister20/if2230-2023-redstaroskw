#include "lib-header/stdtype.h"
#include "lib-header/portio.h"

/** x86 inb/outb:
 * @param dx target port 
 * @param al input/output byte
 */

void out(uint16_t port, uint8_t data) {
    __asm__ volatile(
        "outb %0, %1"
        : // <Empty output operand>
        : "a"(data), "Nd"(port)
    );
    // "a" (data) -> data akan dimasukkan ke register
    // "Nd" (port) -> port akan dimasukkan ke register
    // "outb" -> untuk menuliskan 'data' ke 'port'
    // "%0" -> data akan dimasukkan ke register %eax
    // "%1" -> port akan dimasukkan ke register %edx
    // volatile -> menandakan bahwa kode assembly memiliki efek dimana akan melakukan writing pada I/O port supaya compiler tidak mengoptimasi kode assembly tersebut
}

uint8_t in(uint16_t port) {
    uint8_t result;
    __asm__ volatile(
        "inb %1, %0" 
        : "=a"(result) 
        : "Nd"(port)
    );
    return result;
}