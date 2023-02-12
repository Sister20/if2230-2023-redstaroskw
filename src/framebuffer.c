#include "lib-header/framebuffer.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/portio.h"

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    // TODO : Implement
    MEMORY_FRAMEBUFFER = MEMORY_FRAMEBUFFER + (y*r + r);
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    // TODO : Implement
    // Add the attribute
    uint8_t attr = (bg << 2) | (fg & 0x0F);
    // Set the position
    framebuffer_set_cursor(row, col) = c | (attr << 4);
}

void framebuffer_clear(void) {
    // TODO : Implement
    /* HIPOTESIS: kembalikan ke MEMORY_FRAMEBUFFER sebelumnya*/
}
