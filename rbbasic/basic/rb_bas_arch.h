#ifndef _bas_arch_h_
#define _bas_arch_h_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

int arch_init(void);

typedef void (*arch_load_out_cb)(char *line, void* context);
int arch_load(char* name, char* path, arch_load_out_cb cb, void* context);

typedef uint16_t (*arch_save_cb)(char** line, void* context);
int arch_save(char* filename, arch_save_cb cb, void* context);

typedef void (*arch_dir_out_cb)(char *name, char *ext, size_t size, bool label, void* context);
int arch_dir(char* ext, arch_dir_out_cb cb, void* context);

int arch_delete(char* filename);

#endif
