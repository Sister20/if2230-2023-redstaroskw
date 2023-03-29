#include "lib-header/keyboard.h"
#include "lib-header/portio.h"
#include "lib-header/framebuffer.h"
#include "lib-header/stdmem.h"

static bool key_pressed = FALSE;
static bool backspace_pressed = FALSE;

const char keyboard_scancode_1_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd',  'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',  'z', 'x',  'c',  'v',
    'b',  'n', 'm', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};

static struct KeyboardDriverState keyboard_state = {FALSE, FALSE, 0, {0}};

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void){
    keyboard_state.keyboard_input_on = TRUE;
    // keyboard_state.buffer_index = 0;
    memset(keyboard_state.keyboard_buffer, 0, sizeof(keyboard_state.keyboard_buffer));
    keyboard_isr();
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void){
    keyboard_state.keyboard_input_on = FALSE;
    keyboard_state.buffer_index = 0;
    memset(keyboard_state.keyboard_buffer, 0, KEYBOARD_BUFFER_SIZE);
}

// Get keyboard buffer values - @param buf Pointer to char buffer, recommended size at least KEYBOARD_BUFFER_SIZE
void get_keyboard_buffer(char *buf){
    memcpy(buf, keyboard_state.keyboard_buffer, KEYBOARD_BUFFER_SIZE);
}

// Check whether keyboard ISR is active or not - @return Equal with keyboard_input_on value
bool is_keyboard_blocking(void){
    return keyboard_state.keyboard_input_on;
}

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 * 
 * Will only print printable character into framebuffer.
 * Stop processing when enter key (line feed) is pressed.
 * 
 * Note that, with keyboard interrupt & ISR, keyboard reading is non-blocking.
 * This can be made into blocking input with `while (is_keyboard_blocking());` 
 * after calling `keyboard_state_activate();`
 */
int8_t row = 0;
// static bool 
void keyboard_isr(void) {
    if (!keyboard_state.keyboard_input_on) {
        keyboard_state.buffer_index = 0;
    }else{
        uint8_t scancode = in(KEYBOARD_DATA_PORT);
        char mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
        if(mapped_char == '\b'){
            backspace_pressed = TRUE;
            framebuffer_write(row, keyboard_state.buffer_index-1, ' ', 0x0F, 0x00);
            framebuffer_set_cursor(row,keyboard_state.buffer_index-1);
            keyboard_state.keyboard_buffer[keyboard_state.buffer_index-1] = ' ';
        }
        else if (scancode == 0x1C && !key_pressed)
        {
            keyboard_state_deactivate();
            row++;
            framebuffer_set_cursor(row,keyboard_state.buffer_index);
            key_pressed = TRUE;
        }
        else if (scancode >= 0x02 && scancode <=0x4A && !key_pressed) {
            key_pressed = TRUE;
            framebuffer_write(row, keyboard_state.buffer_index, mapped_char, 0x0F, 0x00);
            framebuffer_set_cursor(row,keyboard_state.buffer_index+1);
            keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
        }
        else if (scancode >= 0x80 && backspace_pressed){
            backspace_pressed = FALSE;
            if(keyboard_state.buffer_index != 0)
                keyboard_state.buffer_index--;

            /* Supaya cursor naik 
             * Cek apakah di kolom ke-0 dan bukan di row ke 0
             * Jika sudah di kolom ke-0 naikkan rownya
             * Kalau sudah naik, while sampai ketemu karakter
             * Set cursor di tempat tsb.
             */
            if(keyboard_state.buffer_index == 0 && row != 0){
                row--;
                int setCol = keyboard_state.buffer_index;
                while(keyboard_state.keyboard_buffer[keyboard_state.buffer_index] > 0x1B)
                {
                    setCol--;
                }
                /* Ini dia masih gak mau ke char yang gak kosong */
                framebuffer_set_cursor(row, setCol);
            }
        }
        else if (scancode >= 0x80 && scancode != 0x9C  && key_pressed) {
            key_pressed = FALSE;
            keyboard_state.buffer_index++;
        }
        else if(scancode == 0x9C)
        {
            key_pressed = FALSE;
        }
    }
    pic_ack(IRQ_KEYBOARD);
}