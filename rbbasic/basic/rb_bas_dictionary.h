#ifndef _bas_dictionary_h
#define _bas_dictionary_h

#include <stdbool.h>

typedef struct dictionary dictionary;

typedef void (*dictionary_each_cb)(char* name, void* value, void* context);

dictionary* dictionary_new(void);
void dictionary_destroy(dictionary* d, dictionary_each_cb cb);
void dictionary_put(dictionary* d, char* name, void* value);
bool dictionary_has(dictionary* d, char* name);
void* dictionary_get(dictionary* d, char* name);
void dictionary_each(dictionary* d, dictionary_each_cb cb, void* context);
void* dictionary_del(dictionary* d, char* name);

#endif 
