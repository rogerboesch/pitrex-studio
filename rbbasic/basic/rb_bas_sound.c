
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "rb_bas_internal.h"
#include "rb_bas_sound.h"
#include "rb_bas_error.h"
#include "rb_bas_math.h"
#include "rb_bas_platform.h"

// -----------------------------------------------------------------------------
#pragma mark - Token list

static token t_keyword_sound = -1;
static token t_keyword_beep = -1;

// -----------------------------------------------------------------------------
#pragma mark - Frequency helpers

float ret_sound_frequency_from_pitch(int pitch) {
    float exp = ((float)pitch-49.0)/12.0;
    float p = powf(2.0, exp);
    
    return p*440;
}

// -----------------------------------------------------------------------------
#pragma mark - Sound integration

int do_sound(basic_type* rv) {
    accept_token(t_keyword_sound);
    
    int voice = 0;      // 0-3
    int pitch = 0;      // Frequencyd
    int vol = 0;
    int distortion = 0; // Wave form
    float duration = 0;

    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        voice = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }
    
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);
    }
    else {
        error((char *)expectcomma_errmsg);
        return 0;
    }

    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        pitch = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }
    
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);
    }
    else {
        error((char *)expectcomma_errmsg);
        return 0;
    }

    expression(&expr);

    if (expr.type == expression_type_numeric) {
        vol = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }
    
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);
    }
    else {
        error((char *)expectcomma_errmsg);
        return 0;
    }

    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        distortion = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

	// 5th parameter (duration) is optional
    if (current_sym() == T_COMMA) {
        accept_token(T_COMMA);

		expression(&expr);
		
		if (expr.type == expression_type_numeric) {
			duration = expr.value.numeric;
		}
		else {
			error((char *)expectnum_errmsg);
			return 0;
		}
	}
	
    float frequency = ret_sound_frequency_from_pitch(pitch);
    float volume = 1.0 / 15 * vol;
    int wave = 1; // Sine
    if (distortion > 0 && distortion < 6)
        wave = distortion;
    
    platform_sound(voice, frequency, volume, wave, duration*1000);

    return 0;
}

int do_beep(basic_type* rv) {
    accept_token(t_keyword_sound);
    
    platform_sound_beep();

    return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Initialize

void basic_sound_init() {
    t_keyword_sound = register_function_0(basic_function_type_keyword, "SOUND", do_sound);
    t_keyword_beep = register_function_0(basic_function_type_keyword, "BEEP", do_beep);
}

