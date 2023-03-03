
#include "rb_bas_platform.h"
#include <stdio.h>
#include <ctype.h>
#include <time.h>

// MARK: - platform basics
byte platform_to_uppercase(byte ch) {
    return toupper(ch);
}

double platform_get_millis(void) {
    clock_t now = clock();
    return now;
}

void platform_sleep(int millis) {
    struct timespec ts;
    ts.tv_sec = millis / 1000;
    ts.tv_nsec = (millis % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// MARK: - not yet implemented

char platform_get_key() {
    platform_print_console("platform_get_key not yet implemented");
    return '\0';
}

void platform_sound(int voice, float frequency, float volume, int wave, int duration) {
    platform_print_console("platform_sound not yet implemented");
}

void platform_sound_beep(void) {
    platform_print_console("platform_sound_beep not yet implemented");
}
