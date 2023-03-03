#include <stdio.h>
#include "rb_bas_platform.h"

void hexdump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*) addr;
    char temp[256];
    
    if (desc != NULL) {
        printf ("%s:\n", desc);
    }

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                sprintf(temp, "  %s\n", buff);
                platform_print_console(temp);
            }
      
            sprintf(temp, "  %04x ", i);
            platform_print_console(temp);
        }

        sprintf(temp, " %02x", pc[i]);
        platform_print_console(temp);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        }
        else {
            buff[i % 16] = pc[i];
        }
        
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        platform_print_console("   ");

        i++;
    }
    
    sprintf(temp, "  %s", buff);
    platform_print_console(temp);
}
