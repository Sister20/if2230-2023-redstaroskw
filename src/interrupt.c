#include "lib-header/interrupt.h"

void io_wait(void) {
    out(0x80, 0);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8)
        out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void) {
    uint8_t a1, a2;

    // Save masks
    a1 = in(PIC1_DATA); 
    a2 = in(PIC2_DATA);

    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); 
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100);      // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010);      // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore masks
    out(PIC1_DATA, a1);
    out(PIC2_DATA, a2);
}


void main_interrupt_handler(
    __attribute__((unused)) struct CPURegister cpu,
    uint32_t int_number,
    __attribute__((unused)) struct InterruptStack info
) {
    switch (int_number) {
        case PAGE_FAULT:
            __asm__("hlt");
            break;

        case PIC1_OFFSET + IRQ_KEYBOARD:
            // while (TRUE){
                // keyboard_state_activate();
            // }
            // keyboard_isr();
            // keyboard_state_activate();
            keyboard_isr();
            break;

        case 0x30:
            syscall(cpu, info);
            break;

        default:
            break;
    }
}

void activate_keyboard_interrupt(void) {
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK ^ (1 << IRQ_KEYBOARD));
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}   

struct TSSEntry _interrupt_tss_entry = {
    .ss0  = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void set_tss_kernel_current_stack(void) {
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile ("mov %%ebp, %0": "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8; 
}

uint8_t row_now = 0;
void puts(char *str, uint32_t len, uint32_t color) {
    if (memcmp(str,"Nadil@RedStarOSKW ",18) == 0) {
        for (uint32_t i = 0; i < len; i++) {
            framebuffer_write(row_now, i, str[i], color, 0);
        }
    }else if (memcmp(str,":",1) == 0) {
        framebuffer_write(row_now, 17, str[0], color, 0);
    }else if (memcmp(str,"/",1) == 0) {
        framebuffer_write(row_now, 18, str[0], color, 0);
    }else if (memcmp(str,"$",1) == 0) {
        framebuffer_write(row_now, 19, str[0], color, 0);
    }else if (memcmp(str,"cls",3) == 0) {
        row_now = 0;
        for (uint32_t i = 0; i < 25; i++) {
            for (uint32_t j = 0; j < 80; j++) {
                framebuffer_write(i, j, ' ', color, 0);
            }
        }
    }else{
        // add ": command not found" after command
        row_now++;
        for (uint32_t i = 0; i < len; i++) {
            framebuffer_write(row_now, i, str[i], color, 0);
        }
        char* not_found = ": command not found";  
        for (uint32_t i = 0; i < 19; i++) {
            framebuffer_write(row_now, len+i, not_found[i], color, 0);
        }
        row_now ++;
    }
}

void syscall(struct CPURegister cpu, __attribute__((unused)) struct InterruptStack info) {
    if (cpu.eax == 0) {
        struct FAT32DriverRequest request = *(struct FAT32DriverRequest*) cpu.ebx;
        *((int8_t*) cpu.ecx) = read(request);
    } else if (cpu.eax == 4) {
        keyboard_state_activate();
        __asm__("sti"); // Due IRQ is disabled when main_interrupt_handler() called
        while (is_keyboard_blocking());
            char buf[KEYBOARD_BUFFER_SIZE];
            get_keyboard_buffer(buf);
            memcpy((char *) cpu.ebx, buf, cpu.ecx);
    } else if (cpu.eax == 5) {
        set_col(21);
        framebuffer_set_cursor(row_now, 21);
        set_row(row_now);
    if (memcmp((char *) cpu.ebx, "cd", 2) == 0){
        puts((char *) cpu.ebx + 3, cpu.ecx, cpu.edx);
    } else if (memcmp((char *) cpu.ebx, "ls", 2) == 0){
        puts((char *) cpu.ebx + 3, cpu.ecx, cpu.edx);
    } else if (memcmp((char *) cpu.ebx, "mkdir", 5) == 0){
        puts((char *) cpu.ebx + 6, cpu.ecx, cpu.edx);
    } else if (memcmp((char * ) cpu.ebx, "cat", 3) == 0){
        puts((char *) cpu.ebx + 4, cpu.ecx, cpu.edx);
    } else if (memcmp((char * ) cpu.ebx, "cp", 2) == 0){
        puts((char *) cpu.ebx + 3, cpu.ecx, cpu.edx);
    } else if (memcmp((char *) cpu.ebx, "rm", 2) == 0){
        puts((char *) cpu.ebx + 3, cpu.ecx, cpu.edx);
    } else if (memcmp((char *) cpu.ebx, "mv", 2) == 0){
        puts((char *) cpu.ebx + 3, cpu.ecx, cpu.edx);
    } else if (memcmp((char *) cpu.ebx, "whereis", 7) == 0){
        puts((char *) cpu.ebx + 8, cpu.ecx, cpu.edx);
    } else{
        puts((char *) cpu.ebx, cpu.ecx, cpu.edx); // Modified puts() on kernel side
}
}
}