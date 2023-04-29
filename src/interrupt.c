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
uint8_t length_of_terminal = 0;
void puts(char *str, uint32_t len, uint32_t color) {
    if (memcmp(str,"cls",3) == 0)
    {
        row_now = 0;
        for (uint32_t i = 0; i < 25; i++)
        {
            for (uint32_t j = 0; j < 80; j++)
            {
                framebuffer_write(i, j, ' ', color, 0);
            }
        }
    }
    else
    {
        row_now++;
        uint32_t col = 0;
        for (uint32_t i = 0; i < len; i++)
        {
            if (str[i] == '\n'){
                row_now ++;
                col = 0;
            }
            else{
                framebuffer_write(row_now, col, str[i], color, 0);
                col++;
            }
        }
        row_now ++;
    }
}

void puts_terminal(char *str, uint32_t len, uint32_t color)
{
    for (uint32_t i = 0; i < len; i++)
    {
        framebuffer_write(row_now, i, str[i], color, 0);
    }
}

void puts_path(char *str, uint32_t len, uint32_t color)
{
    framebuffer_write(row_now, length_of_terminal-1, ':', 0x8, 0);
    framebuffer_write(row_now, length_of_terminal, '/', color, 0);
    for (uint32_t i = 0; i < len; i++)
    {
        framebuffer_write(row_now, length_of_terminal+1+i, str[i], color, 0);
    }
    framebuffer_write(row_now, length_of_terminal+1+len, '/', color, 0);
    framebuffer_write(row_now, length_of_terminal+3+len, '$', 0xF, 0);
}


void syscall(struct CPURegister cpu, __attribute__((unused)) struct InterruptStack info) {
    if (cpu.eax == 0) // FS read
    {
        struct FAT32DriverRequest request = *(struct FAT32DriverRequest*) cpu.ebx;
        *((int32_t*) cpu.ecx) = read(request);
    }
    else if (cpu.eax == 1) // FS read_directory
    {
        struct FAT32DriverRequest request = *(struct FAT32DriverRequest*) cpu.ebx;
        *((int32_t*) cpu.ecx) = read_directory(request);
    }
    else if (cpu.eax == 2) // FS write
    {
        struct FAT32DriverRequest request = *(struct FAT32DriverRequest*) cpu.ebx;
        *((int32_t*) cpu.ecx) = write(request);
    }
    else if (cpu.eax == 3) // FS delete
    {
        struct FAT32DriverRequest request = *(struct FAT32DriverRequest*) cpu.ebx;
        *((int32_t*) cpu.ecx) = delete(request);
    }
    else if (cpu.eax == 4) // KEYBOARD INPUT
    {
        keyboard_state_activate();
        __asm__("sti"); // Due IRQ is disabled when main_interrupt_handler() called
        while (is_keyboard_blocking());
        char buf[KEYBOARD_BUFFER_SIZE];
        get_keyboard_buffer(buf);
        memcpy((char *) cpu.ebx, buf, cpu.ecx);
    }
    else if (cpu.eax == 5) // TEXT OUTPUT
    {
        puts((char *) cpu.ebx, cpu.ecx, cpu.edx); // Modified puts() on kernel side
    }
    else if (cpu.eax == 6) // TERMINAL OUTPUT
    {
        length_of_terminal = cpu.ecx;
        puts_terminal((char *) cpu.ebx, cpu.ecx, cpu.edx);
    }
    else if (cpu.eax == 7) // PATH OUTPUT
    {
        framebuffer_set_cursor(row_now, length_of_terminal);
        puts_path((char *) cpu.ebx, cpu.ecx, cpu.edx);
        length_of_terminal += cpu.ecx;
        set_terminal_length(length_of_terminal + 5);
        set_col(length_of_terminal + 5);
        set_row(row_now);
        framebuffer_set_cursor(row_now, length_of_terminal + 5);
    }
}