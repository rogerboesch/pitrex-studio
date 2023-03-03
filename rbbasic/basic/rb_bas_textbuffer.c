
#include "rb_bas_platform.h"
#include "rb_bas_textbuffer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

// Base index to specify color palette as character in a print statament
#define RET_ESCAPE_BASE_INDEX_FOR_COLOR 240
#define RET_ESCAPE_INVERT_ON RET_ESCAPE_BASE_INDEX_FOR_COLOR-2
#define RET_ESCAPE_INVERT_OFF RET_ESCAPE_INVERT_ON+1

#define RET_TEXT_WIDTH 32
#define RET_TEXT_HEIGHT 24

static byte ret_text_screen[RET_TEXT_WIDTH*RET_TEXT_HEIGHT];		// text buffer
static byte ret_text_color[RET_TEXT_WIDTH*RET_TEXT_HEIGHT];		    // text color buffer
static int ret_text_immediate_print = 0;							// 0=needs a flush to print
static byte ret_brightness = 100;                                   // Text brightness
static int ret_text_cursor_visible = 0;                             // Cursor visible
static int ret_cursor_row = 0;										// Cursor position
static int ret_cursor_col = 0;
static int cursor_blink = 0;										// Cursor blink state, 0=Off
static int ret_cursor_cycles = 0;									// Cursor cycle count
static char ret_line_buffer[RET_TEXT_WIDTH];						// Text from a single line after press return

// MARK: - Text buffer rendering

static void ret_text_set_immediate_mode(int flag) {
	// 0=text buffering until flush, 1=direct print
	ret_text_immediate_print = flag;
}

static void ret_text_flush_buffer() {
    platform_render_frame();
}

static void ret_text_fast_clear_screen() {
    memset(&ret_text_screen[0], ' ', RET_TEXT_WIDTH*RET_TEXT_HEIGHT);
    memset(&ret_text_color[0], ' ', RET_TEXT_WIDTH*RET_TEXT_HEIGHT);
}

static boolean ret_text_print_character(int row, int col, char ch, byte color) {
    int width = RET_TEXT_WIDTH;
    int height = RET_TEXT_HEIGHT;
        
    byte code = (byte)ch;
    if (code >= RET_ESCAPE_BASE_INDEX_FOR_COLOR && code <= RET_ESCAPE_BASE_INDEX_FOR_COLOR + 15) {
        // Color escape
        int color = (int)code - RET_ESCAPE_BASE_INDEX_FOR_COLOR;
        ret_brightness = color;
        
        return false;
    }

    // Is the pixel actually visible?
    if (col >= 0 && col < width && row >= 0 && row < height) {
        int offset = row * RET_TEXT_WIDTH + col;
        
        ret_text_screen[offset] = ch;
        ret_text_color[offset] = color;
        
        if (ret_text_immediate_print == 1) {
            ret_text_flush_buffer();
        }
    }
	
	return true;
}

static char ret_text_get_character(int row, int col) {
    int offset = row * RET_TEXT_WIDTH + col;
    return ret_text_screen[offset];
}

static boolean ret_text_print_character_internal(int row, int col, char ch) {
    return ret_text_print_character(row, col, ch, ret_brightness);
}

static void ret_text_scroll_up() {
	byte* source;
	byte* dest;
	
	for (int row = 1; row < RET_TEXT_HEIGHT; row++) {
		source = &ret_text_screen[row*RET_TEXT_WIDTH];
		dest = &ret_text_screen[(row-1)*RET_TEXT_WIDTH];
		
		memcpy(dest, source, RET_TEXT_WIDTH);
	}

	// Clear last line
	dest = &ret_text_screen[(RET_TEXT_HEIGHT-1)*RET_TEXT_WIDTH];
	memset(dest, ' ', RET_TEXT_WIDTH);
}

// MARK: - Cursor handling

static void ret_text_cursor_show(boolean flag) {
    ret_text_cursor_visible = flag;
}

static void ret_text_draw_cursor() {
    if (!ret_text_cursor_visible) {
        return;
    }
}

static void ret_text_reset_character() {
}

static void ret_text_cursor_left() {
	ret_text_reset_character();
	
	ret_cursor_col--;
	
	if (ret_cursor_col < 0) {
		ret_cursor_row--;
		ret_cursor_col = RET_TEXT_WIDTH-1;
	}
	
	if (ret_cursor_row < 0) {
		ret_cursor_row = 0;
		ret_cursor_col = 0;
	}

	ret_text_draw_cursor();
}

static void ret_text_cursor_right() {
	ret_text_reset_character();

	ret_cursor_col++;
	
	if (ret_cursor_col > RET_TEXT_WIDTH-1) {
		ret_cursor_row++;
		ret_cursor_col = 0;
	}

	if (ret_cursor_row > RET_TEXT_HEIGHT-1) {
		ret_text_scroll_up();
		
		ret_cursor_row = RET_TEXT_HEIGHT-1;
	}
	
	ret_text_draw_cursor();
}

static void ret_text_cursor_up() {
	ret_text_reset_character();

	ret_cursor_row--;
		
	if (ret_cursor_row < 0) {
		ret_cursor_row = 0;
	}

	ret_text_draw_cursor();
}

static void ret_text_cursor_down() {
	ret_text_reset_character();

	ret_cursor_row++;
	
	if (ret_cursor_row > RET_TEXT_HEIGHT-1) {
		ret_text_scroll_up();

		ret_cursor_row = RET_TEXT_HEIGHT-1;
	}
	
	ret_text_draw_cursor();
}

static void ret_text_cursor_set(int x, int y) {
    ret_text_reset_character();

    ret_cursor_col = x;
    ret_cursor_row = y;
    
    ret_text_draw_cursor();
}

static void ret_text_new_line() {
	ret_text_reset_character();

	ret_cursor_row++;
	ret_cursor_col = 0;
	
	if (ret_cursor_row >= RET_TEXT_HEIGHT) {
		// Scroll screen up
		ret_text_scroll_up();
		
		ret_cursor_row = RET_TEXT_HEIGHT-1;
	}
	
	ret_text_draw_cursor();
}

static void ret_text_blink_cursor() {
	ret_cursor_cycles++;
	if (ret_cursor_cycles < 25)
		return;
	
	ret_cursor_cycles = 0;
	
	if (cursor_blink == 0) {
		ret_text_draw_cursor();
	}
	else {
		ret_text_reset_character();
	}
	
	cursor_blink = !cursor_blink;
}

static void ret_text_delete_char(void) {
	ret_text_cursor_left();
	
	// Remove character
	ret_text_screen[ret_cursor_row*RET_TEXT_WIDTH+ret_cursor_col] = ' ';
}

// MARK: - Private text functions

static boolean ret_text_print_char_at(int row, int col, char ch) {
    return ret_text_print_character_internal(row, col, ch);
}

static void ret_text_print_string_at(int row, int col, char* str) {
    while (*str != 0) {
        char ch = *str;
    
        ret_text_print_char_at(row, col, ch);
        col++;
        
        str++;
    }
}

static void ret_text_print_no_line_break(char* str, int size) {
    int count = 0;
    while (*str != 0) {
        char ch = *str;
    
        ret_text_print_char_at(ret_cursor_row, ret_cursor_col, ch);
        
        ret_cursor_col++;
        if (ret_cursor_col >= RET_TEXT_WIDTH) {
            ret_cursor_col = RET_TEXT_WIDTH-1;
        }
        
        ret_text_draw_cursor();

        str++;
        count++;
        
        if (count == size) {
            return;
        }
    }
}

// MARK: - Public text functions

void textbuffer_initialize() {
    ret_cursor_row = 0;
    ret_cursor_col = 0;
    cursor_blink = 0;
    ret_cursor_cycles = 0;
    
    ret_text_draw_cursor();
}

int textbuffer_get_rows() {
    return RET_TEXT_HEIGHT;
}

char line[RET_TEXT_WIDTH+1];

const char* textbuffer_get_line(int row) {
    int offset = row*RET_TEXT_WIDTH;
    memcpy(&line[0], &ret_text_screen[offset], RET_TEXT_WIDTH);
    line[RET_TEXT_WIDTH] = '\0';

    int i = RET_TEXT_WIDTH-1;
    while (i >= 0) {
        if (line[i] != ' ') {
            line[i+1] = '\0';
            return line;
        }
        
        i--;
    }
    
    line[0] = '\0';
    return line;
}

void textbuffer_print_char(char ch) {
    if (ch == '\n') {
        ret_text_new_line();
        return;
    }
    
	if (!ret_text_print_char_at(ret_cursor_row, ret_cursor_col, ch)) {
		return;
	}
	
	ret_cursor_col++;
	if (ret_cursor_col >= RET_TEXT_WIDTH) {
		ret_cursor_row++;
		ret_cursor_col = 0;
	}
	
	ret_text_draw_cursor();
}

void textbuffer_print(char* str) {
	while (*str != 0) {
		char ch = *str;
	
		textbuffer_print_char(ch);
		
		str++;
	}
}

void textbuffer_print_line(char* str) {
	textbuffer_print(str);	
	ret_text_new_line();
}

void textbuffer_clear_screen() {
    ret_cursor_row = 0;
    ret_cursor_col = 0;

    ret_text_fast_clear_screen();
    ret_text_flush_buffer();
}
