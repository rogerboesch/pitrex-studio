#ifndef bas_interpreter_h
#define bas_interpreter_h

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "rb_bas_platform.h"

void basic_init(size_t memory_size, size_t stack_size);
void basic_destroy(void);
void basic_eval(char* line);
void basic_run(void);
void basic_load(char* filename, char* path);
void basic_restart(void);
void basic_pause(bool flag);
void basic_stop(void);
void basic_next_line_only(void);
void basic_show_prompt(void);
bool basic_program_is_running(void);

#ifdef __cplusplus
}
#endif

#endif 
