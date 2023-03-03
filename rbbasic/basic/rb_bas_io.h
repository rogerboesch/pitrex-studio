#ifndef _bas_io_h
#define _bas_io_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rb_bas_platform.h"

void basic_io_print(char* buffer);
char* basic_io_readline(char* prompt, char* buffer, size_t buffer_size, boolean echo);

#endif
