#ifndef _bas_callback_h
#define _bas_callback_h

#include <stdbool.h>

typedef struct error {
    int error;
};

typedef bool (*callback)(error err, void* data);

#endif 
