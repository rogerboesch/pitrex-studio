#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#include <stdint.h>
#include "rb_bas_platform.h"

extern uint16_t __line;

const char *last_error = NULL;

void error(const char *error_msg) {
    last_error = error_msg;
	
	char temp[255];
	sprintf(temp, "%s at line %d", error_msg, __line);
    platform_print_console(temp);
    
    textbuffer_print_line(temp);
}
