#include "lib-header/keyboard.h"
#include "lib-header/portio.h"
#include "lib-header/framebuffer.h"
#include "lib-header/stdmem.h"

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
// void keyboard_isr(void) 
// {
//     if (!keyboard_state.keyboard_input_on) {
//         keyboard_state.buffer_index = 0;
//     }
//     else {
//         uint8_t scancode = in(KEYBOARD_DATA_PORT);
//         char mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
//         // if(mapped_char == '\n') {
//         //     keyboard_state_deactivate();
//         //     keyboard_state.buffer_index = 0;
//         //     return;
//         // }
//         // else if(mapped_char != 0) {
//         //     keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
//         //     framebuffer_set_cursor(keyboard_state.buffer_index, keyboard_state.buffer_index);
//         //     framebuffer_write(0, keyboard_state.buffer_index, keyboard_state.keyboard_buffer[keyboard_state.buffer_index], 0x0F, 0x00);
//         // }
//         if(mapped_char == 0x1E || scancode == 0x1E)
//         {
//             keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
//             framebuffer_set_cursor(0, keyboard_state.buffer_index);
//             framebuffer_write(0, keyboard_state.buffer_index, keyboard_state.keyboard_buffer[keyboard_state.buffer_index], 0x0F, 0x00);
//             keyboard_state.buffer_index++; // increment buffer index after processing input
//             if (keyboard_state.buffer_index >= 80)
//             {
//                 keyboard_state.buffer_index = 0;
//             }
//         }
//     }
//     pic_ack(IRQ_KEYBOARD);
// }

static bool key_pressed = FALSE;

void keyboard_isr(void) {
    // Read the scancode from the keyboard
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    char mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
    // Check if the scancode corresponds to the "a" key
    if (scancode >= 0x02 && scancode <=0x4A) {
        // If the scancode corresponds to a "make" event for the "a" key,
        // set the flag indicating that the key is pressed and write the
        // corresponding character to the framebuffer.
        key_pressed = TRUE;
        framebuffer_write(0, keyboard_state.buffer_index, mapped_char, 0x0F, 0x00);
        framebuffer_set_cursor(0,keyboard_state.buffer_index+1);
        keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
    } else if (scancode >= 0x80 && key_pressed) {
        // If the scancode corresponds to a "break" event for the "a" key,
        // clear the flag indicating that the key is pressed.
        key_pressed = FALSE;
        keyboard_state.buffer_index++;

    }
    // Acknowledge the interrupt
    pic_ack(IRQ_KEYBOARD);
}