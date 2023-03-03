
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "rb_bas_internal.h"
#include "rb_bas_vgraphic.h"
#include "rb_bas_error.h"
#include "rb_bas_math.h"
#include "rb_bas_platform.h"

static token t_keyword_vdraw = -1;
static token t_keyword_vplot = -1;
static token t_keyword_vink = -1;
static token t_keyword_vcls = -1;

// MARK: - Platform implementation

static int draw_cursor_x = 0;
static int draw_cursor_y = 0;

void platform_clear_screen(void) {
    textbuffer_clear_screen();
    vline_buffer_clear_screen();
    
    draw_cursor_x = 0;
    draw_cursor_y = 0;
}

// MARK: - Line graphics

static int do_vdraw(basic_type* rv) {
    accept_token(t_keyword_vdraw);

    int x = 0;
    int y = 0;
    
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        x = expr.value.numeric;
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
        y = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

    platform_draw_line(draw_cursor_x, draw_cursor_y, draw_cursor_x+x, draw_cursor_y+y);
    
    draw_cursor_x += x;
    draw_cursor_y += y;

    platform_render_frame();

    return 0;
}

static int do_vplot(basic_type* rv) {
    accept_token(t_keyword_vplot);

    int x = 0;
    int y = 0;
    
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        x = expr.value.numeric;
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
        y = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

    platform_draw_line(x, y, x, y);

    draw_cursor_x = x;
    draw_cursor_y = y;

    return 0;
}

static int do_vink(basic_type* rv) {
    accept_token(t_keyword_vink);

	int brightness = 0;
	
    expression_result expr;
    expression(&expr);
    
    if (expr.type == expression_type_numeric) {
        brightness = expr.value.numeric;
    }
    else {
        error((char *)expectnum_errmsg);
        return 0;
    }

    return 0;
}

int do_vcls(basic_type* rv) {
    accept_token(t_keyword_vcls);

    platform_clear_screen();
    
    return 0;
}

// -----------------------------------------------------------------------------
#pragma mark - Initialize

void basic_vgraphic_init() {
	t_keyword_vdraw = register_function_0(basic_function_type_keyword, "DRAW", do_vdraw);
    t_keyword_vplot = register_function_0(basic_function_type_keyword, "PLOT", do_vplot);
	t_keyword_vcls = register_function_0(basic_function_type_keyword, "CLS", do_vcls);
	t_keyword_vink = register_function_0(basic_function_type_keyword, "INK", do_vink);
}
