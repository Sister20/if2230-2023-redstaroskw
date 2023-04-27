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
    keyboard_state.buffer_index = 0;
    memset(keyboard_state.keyboard_buffer, 0, KEYBOARD_BUFFER_SIZE);
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void){
    keyboard_state.keyboard_input_on = FALSE;
}

// Get keyboard buffer values - @param buf Pointer to char buffer, recommended size at least KEYBOARD_BUFFER_SIZE
void get_keyboard_buffer(char *buf){
    memcpy(buf, keyboard_state.keyboard_buffer, KEYBOARD_BUFFER_SIZE);
}

// Check whether keyboard ISR is active or not - @return Equal with keyboard_input_on value
bool is_keyboard_blocking(void){
    return keyboard_state.keyboard_input_on;
}

int8_t row = 0;

void set_row(uint8_t r){
    row = r;
}

int8_t col = 0;

void set_col(uint8_t c){
    col = c;
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

// static bool 
void keyboard_isr(void) {
    if (!keyboard_state.keyboard_input_on) {
        keyboard_state.buffer_index = 0;
    }else{
        uint8_t scancode = in(KEYBOARD_DATA_PORT);
        char mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
        if(mapped_char == '\b'){
            if(col >= 22){
                backspace_pressed = TRUE;
                framebuffer_write(row, col-1, ' ', 0x0F, 0x00);
                framebuffer_set_cursor(row,col-1);
                keyboard_state.keyboard_buffer[keyboard_state.buffer_index-1] = ' ';
            }
        }
        else if (scancode == 0x1C && !key_pressed)
        {
            keyboard_state_deactivate();
            row++;
            framebuffer_set_cursor(row,col);
            key_pressed = TRUE;
        }
        else if (scancode >= 0x02 && scancode <=0x4A && !key_pressed) {
            framebuffer_write(row, col, mapped_char, 0x0F, 0x00);
            framebuffer_set_cursor(row,col+1);
            keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
            key_pressed = TRUE;
        }
        else if (scancode >= 0x80 && backspace_pressed){
            backspace_pressed = FALSE;
            if(keyboard_state.buffer_index != 0){
                keyboard_state.buffer_index--;
                col--;
            }

            if(keyboard_state.buffer_index == 0 && row != 0){
                uint8_t *fb = MEMORY_FRAMEBUFFER;
                framebuffer_write(row, col, ' ', 0x0F, 0x00);
                keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = ' ';
                row--;
                uint8_t col = 0;
                for (int i = 80; i > 0; i--)
                {
                    if(fb[(row*80-i)*2] != 0){
                        col = i;
                        break;
                    }
                }
                framebuffer_set_cursor(row,col+1);
                keyboard_state.buffer_index = col;
            }
        }
        else if (scancode >= 0x80 && scancode != 0x9C  && key_pressed) {
            key_pressed = FALSE;
            keyboard_state.buffer_index++;
            col++;
        }
        else if(scancode == 0x9C)
        {
            key_pressed = FALSE;
        }
    }
    pic_ack(IRQ_KEYBOARD);
}