#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "rb_bas_io.h"
#include "rb_bas_platform.h"

void basic_io_print(char* buffer) {
    textbuffer_print(buffer);
}

char basic_get_char() {
	return platform_get_key();
}

char* basic_io_readline(char* prompt, char* buffer, size_t buffer_size, boolean echo) {
    int len = 0;
    char ch;
    
    basic_io_print(prompt);
    
    while ((ch = basic_get_char()) != '\r' && len < buffer_size - 1) {
        switch(ch) {
            case '\b':
                if (len>0) {
                    buffer[--len] = '\0';
                }
                break;

			case 0:
				// Means no key, wait and then read again
				platform_sleep(100);
				break;

            default:
                buffer[len++] = platform_to_uppercase(ch);
				
				if (echo) {
					textbuffer_print_char(ch);
                    platform_render_frame();
				}
				break;
        }
    }

	buffer[len] = '\0';

	if (echo)
        textbuffer_print_line("");
	
	return buffer;
}
